/*
AMiRo-Apps is a collection of applications and configurations for the
Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2018..2022  Thomas Schöpping et al.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU (Lesser) General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU (Lesser) General Public License for more details.

You should have received a copy of the GNU (Lesser) General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    canbridge.c
 */

#include <canbridge.h>
#include <amiroos.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/**
 * @brief   Event mask to set on a frame event to publish /request an incoming frame.
 */
#define FRAMEEVENT                          (urtCoreGetEventMask() << 1)
/**
 * @brief   Event mask to set on a request answer events to send an request answer over the CANBridge.
 */
#define REQUESTANSWEREVENT                  (urtCoreGetEventMask() << 2)
/**
 * @brief   Event mask to set on a new data event (new data are coming in over the CANBridge)
 */
#define NEWDATAEVENT                        (urtCoreGetEventMask() << 3)

// CAN Type data -> Topic and Request
#define TOPIC_DATA    0
#define NRT_REQUEST_DATA  1
#define FRT_REQUEST_DATA  3
#define SRT_REQUEST_DATA  7
#define HRT_REQUEST_DATA  5
#define REQUEST_ANSWER_DATA  8

// CAN Board_IDs
#define DIWHEELDRIVE     0
#define POWERMANAGEMENT  1
#define LIGHTRING        2
#define UNKNOWN          3

/******************************************************************************/
/* EXPORTED VARIABLES                                                         */
/******************************************************************************/

/******************************************************************************/
/* LOCAL TYPES                                                                */
/******************************************************************************/

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/

/**
 * @brief   Name of canbridge nodes.
 */
static const char _canbridge_name[] = "CANBridge";

extern struct urt_baserequest_VMT _urt_nrtrequest_vmt;
extern struct urt_baserequest_VMT _urt_srtrequest_vmt;
extern struct urt_baserequest_VMT _urt_frtrequest_vmt;
extern struct urt_baserequest_VMT _urt_hrtrequest_vmt;
extern void _urt_hrtrequest_calculateDeadline(urt_hrtrequest_t const* const request, urt_osTime_t* const t);

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   Copy the frame payload into the given payload buffer
 *
 * @param[in] can             Pointer to the CAN Bridge node object.
 *                            Must not be NULL.
 * @param[in] payloadbuffer   Pointer where to copy the payload to 
 * @param[in] current_frame   Frame from which the payload should be copied
 */
static void _canBridgecopyPayload(canBridge_node_t* can, uint8_t* payloadbuffer, uint8_t current_frame) {
  urtDebugAssert(can != NULL);

  canBridge_buffer_t* current_payload = can->frames[current_frame].payload;
  canBridge_buffer_t* last_payload;
  size_t currentsize = 0;
  size_t diff;

  while (current_payload != NULL) {
    if (currentsize + sizeof(((CANTxFrame*)0)->data8) < can->frames[current_frame].bytes) {
      diff = sizeof(((CANTxFrame*)0)->data8);
    } else {
      diff = can->frames[current_frame].bytes - currentsize;
    }
    memcpy(&payloadbuffer[currentsize], current_payload->payload, diff);
    currentsize += diff;

    last_payload = current_payload;
    current_payload = current_payload->next;
    last_payload->next = NULL;
    last_payload->used = FALSE;
  }
  can->frames[current_frame].payload = NULL;
  return;
}

/**
 * @brief   Submit the over the CANBridge received request data.
 *
 * @param[in] can           Pointer to the CAN Bridge node object.
 *                          Must not be NULL.
 * @param[in] num_request   Number of request which should be submitted in the CAN List of requests.
 */
static void _canBridgesendRequest(canBridge_node_t* can, uint8_t num_request, uint16_t service_id, size_t bytes, uint8_t type, urt_osTime_t deadline, bool fireforget) {
  urtDebugAssert(can != NULL);

  // initialize base request data
  can->requests[num_request].base.prev = NULL;
  can->requests[num_request].base.next = NULL;
  can->requests[num_request].base.service = NULL;
  urtTimeSet(&can->requests[num_request].base.submissionTime, 0);
  can->requests[num_request].base.notification.thread = NULL;
  can->requests[num_request].base.notification.mask = 0;
  can->requests[num_request].base.payload = can->request_payloads[num_request];
#if (URT_CFG_RPC_PROFILING == true)
  can->requests[num_request].base.profiling.sumLatencies = 0;
  can->requests[num_request].base.profiling.numCalls = 0;
  can->requests[num_request].base.profiling.numFails = 0;
  can->requests[num_request].base.profiling.maxLatency = URT_DELAY_IMMEDIATE;
  can->requests[num_request].base.profiling.minLatency = URT_DELAY_INFINITE;
#endif /* (URT_CFG_RPC_PROFILING == true) */

  urt_osEventMask_t fireandForget = 0;
  if (!fireforget) {
    fireandForget = REQUESTANSWEREVENT;
    can->dispatched_service_ids[num_request] = (urt_serviceid_t)service_id;
  }

  switch(type) {
  case NRT_REQUEST_DATA: {
    // initialize NRT data
    can->requests[num_request].nrt.vmt = &_urt_nrtrequest_vmt;
    // Submit the NRT Request
    urtNrtRequestSubmit(&((canBridge_node_t*)can)->requests[num_request].nrt,
                        urtCoreGetService(service_id),
                        bytes,
                        fireandForget);
    break;
  }
  case SRT_REQUEST_DATA: {
    // initialize SRT data
    can->requests[num_request].srt.vmt = &_urt_srtrequest_vmt;
    //Submit the SRT Request
    urtSrtRequestSubmit(&((canBridge_node_t*)can)->requests[num_request].srt,
                        urtCoreGetService(service_id),
                        bytes,
                        fireandForget);
    
    break;
  }
  case FRT_REQUEST_DATA:  {
    // initialize FRT data
    can->requests[num_request].frt.vmt = &_urt_frtrequest_vmt;
    #if (URT_CFG_RPC_QOS_DEADLINECHECKS == true)
      can->requests[num_request].frt.deadlineOffset = 0;
    #endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) */

    #if (URT_CFG_RPC_QOS_JITTERCHECKS == true)
      can->requests[num_request].frt.maxJitter = 0;
      can->requests[num_request].frt.minLatency = URT_DELAY_INFINITE;
      can->requests[num_request].frt.maxLatency = URT_DELAY_IMMEDIATE;
    #else /* (URT_CFG_RPC_QOS_JITTERCHECKS == true) */
      (void)jitter;
    #endif /* (URT_CFG_RPC_QOS_JITTERCHECKS == true) */ 

    //Submit the FRT Request
    urtFrtRequestSubmit(&((canBridge_node_t*)can)->requests[num_request].frt,
                      urtCoreGetService(service_id),
                      bytes,
                      fireandForget,
                      0);
    
    break;
  }
  case HRT_REQUEST_DATA: {
    can->requests[num_request].hrt.vmt = &_urt_hrtrequest_vmt;
    // initialize HRT data
    #if (URT_CFG_RPC_QOS_DEADLINECHECKS == true)
      can->requests[num_request].hrt.deadlineOffset = 0;
    #endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) */

    #if (URT_CFG_RPC_QOS_JITTERCHECKS == true)
      can->requests[num_request].hrt.maxJitter = 0;
      can->requests[num_request].hrt.minLatency = URT_DELAY_INFINITE;
      can->requests[num_request].hrt.maxLatency = URT_DELAY_IMMEDIATE;
    #else /* (URT_CFG_RPC_QOS_JITTERCHECKS == true) */
      (void)jitter;
    #endif /* (URT_CFG_RPC_QOS_JITTERCHECKS == true) */

    #if (URT_CFG_RPC_QOS_DEADLINECHECKS == true || URT_CFG_RPC_QOS_JITTERCHECKS == true)
      urtTimerReset(&can->requests[num_request].hrt.qosTimer);
    #endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true || URT_CFG_RPC_QOS_JITTERCHECKS == true) */

    urt_osTime_t timeNow;
    urtTimeNow(&timeNow);   

    //Submit the HRT Request
    urtHrtRequestSubmit(&((canBridge_node_t*)can)->requests[num_request].hrt,
                      urtCoreGetService(service_id),
                      bytes,
                      fireandForget,
                      (urt_delay_t)urtTimeDiff(&timeNow, &deadline));

    break;
  }
  default:break;
  }
  return;
}

static void _canBridgetrashFrameCouldbeTrashed(canBridge_frames_t* can_frame) {
  if (can_frame->status == METATRASH && can_frame->current_payloads == can_frame->metafftp.total_payloads) {
    // Unset the reserved payloads
    canBridge_buffer_t* first_payload = can_frame->payload;
    canBridge_buffer_t* current_payload = NULL;
    while (first_payload != NULL) {
      current_payload = first_payload;
      first_payload->used = FALSE;
      first_payload = first_payload->next;
      current_payload->next = NULL;
    }
    can_frame->payload = NULL;
    can_frame->status = BLANK;
  }
  return;
}

static void _canBridgereservePayload(canBridge_node_t* can, canBridge_frames_t* can_frame, const CANRxFrame* frame, canBridge_id_map_t frame_map) {
  can_frame->current_payloads++;
  // Check if there is a free payload
  for (uint8_t p = 0; p <= CAN_NUM_PAYLOADS; p++) {
    if (p == CAN_NUM_PAYLOADS) {
      // No empty payload, frame status is trash
      if ( can_frame->status == META) {
        can_frame->status = METATRASH;
      } else {
        can_frame->status = TRASH;
      }
      _canBridgetrashFrameCouldbeTrashed(can_frame);
      return;
    }

    //Empty payload
    if (can->payloads[p].used == FALSE) {
      can->payloads[p].used = TRUE;
      can->payloads[p].cnt = frame_map.cnt;
      memcpy(can->payloads[p].payload, frame->data8, (size_t)frame->DLC);

      // local variables
      canBridge_buffer_t* current_payload = can_frame->payload;
      if (current_payload == NULL) {
        can_frame->payload = &can->payloads[p];
      } else {

        while (current_payload->next != NULL) {
          if (current_payload->cnt == (can->payloads[p].cnt+1)){
            break;
          }
          current_payload = current_payload->next;
        }
        can->payloads[p].next = current_payload->next;
        current_payload->next = &can->payloads[p];
        
      }
      break;
    }
  }

  if (can_frame->status == META && can_frame->current_payloads == can_frame->metafftp.total_payloads) {
    can_frame->status = DONE;
    // Signal the can loop
    chEvtBroadcastFlagsI(&can->frame_signal.source, (urt_osEventFlags_t)NEWDATAEVENT);
  }

  return;
}

/**
 * @brief   Copy the received frame data.
 *
 * @param[in] frame       Pointer to the received frame.
*                         Must not be NULL.
 * @param[in] can         Pointer to the CAN Bridge node object.
 *                        Must not be NULL.
 */
static void _canBridgegetFrameData(const CANRxFrame* frame, canBridge_node_t* can) {
  urtDebugAssert(frame != NULL);
  urtDebugAssert(can != NULL);

  canBridge_id_map_t frame_map;
  frame_map.raw = frame->EID;
  size_t bytes = (size_t)frame->DLC;
  int8_t blank_frame = -1;

  for (uint8_t f = 0; f <= CAN_NUM_FRAMES; f++) {
    // End of list -> CANBridge does not know data set
    if (f == CAN_NUM_FRAMES) {
      if (blank_frame == -1) {
        //Error case list of frames is full!
        urtDebugAssert(blank_frame != -1);
      } else {
        // reserve a frame
        can->frames[blank_frame].board_id = frame_map.board_id;
        can->frames[blank_frame].id = frame_map.id;
        can->frames[blank_frame].type = frame_map.type;
        can->frames[blank_frame].current_payloads = 0;
        can->frames[blank_frame].bytes = 0;
        can->frames[blank_frame].status = WIP;

        if (frame_map.cnt == 0) {
          can->frames[blank_frame].status = META;
          // Set the meta information of this frame (fireandforget, timestamp, total_payloads)
          memcpy(&can->frames[blank_frame].timestamp.time, frame->data8, bytes-sizeof(frame->data8[7]));
          can->frames[blank_frame].metafftp.raw = frame->data8[7];
          return;
        } else {
          // No Meta frame 
          can->frames[blank_frame].bytes += bytes;
          _canBridgereservePayload(can, &can->frames[blank_frame], frame, frame_map);
        }
      }      
      break;
    }

    // Frame is already used
    if (can->frames[f].status == META || can->frames[f].status == WIP || can->frames[f].status == TRASH || can->frames[f].status == METATRASH) {
      // CANBridge knows data set 
      if (can->frames[f].type == frame_map.type && can->frames[f].id == frame_map.id && can->frames[f].board_id == frame_map.board_id) {
        //Check if this is the Meta-Frame
        if (frame_map.cnt == 0) {
          if (can->frames[f].status == TRASH) {
            can->frames[f].status = METATRASH;
          } else {
            can->frames[f].status = META;
          }
          // Set the meta information of this frame (fireandforget, timestamp, total_payloads)
          memcpy(&can->frames[f].timestamp.time, frame->data8, bytes-sizeof(frame->data8[7]));
          can->frames[f].metafftp.raw = frame->data8[7];
          return;
        } else if (can->frames[f].status == METATRASH) {
          _canBridgetrashFrameCouldbeTrashed(&can->frames[f]);
        } else {
          // Not the Meta Frame and Frame status is not TRASH
          can->frames[f].bytes += bytes; 
          _canBridgereservePayload(can, &can->frames[f], frame, frame_map);
        }
        return;
      }
    }

    if (blank_frame == -1 && can->frames[f].status == BLANK) {
      blank_frame = f;
    }
  }

  return;
}

/**
 * @brief   Copy the received request data into can frames and send this frames over the CAN Bridge.
 *
 * @param[in] can               Pointer to the canbridge structure.
 *                              Must not be NULL.
 * @param[in] currentRequest    Current request in the CANBridge list of requests.
 */
static void _canBridgetransmitRequestAnswerData(canBridge_node_t* can, uint8_t currentRequest, size_t payload_size) {
  urtDebugAssert(can != NULL);

  // local variables
  uint8_t payload_buffer[payload_size];
  uint8_t position = 0;
  uint8_t current_size;
  uint8_t total_frames;
  memcpy(payload_buffer, can->requests[currentRequest].base.payload, payload_size);

  total_frames = payload_size/sizeof(((CANTxFrame*)0)->data8);
  if (payload_size % sizeof(((CANTxFrame*)0)->data8) != 0) 
    total_frames++;
  can->filter_map.cnt = 0;
  can->filter_map.id = can->dispatched_service_ids[currentRequest];
  can->filter_map.board_id = can->board_id;
  can->filter_map.type = REQUEST_ANSWER_DATA;

  // Meta-Frame
  CANTxFrame frame = {
    {
      /* DLC */ sizeof(((CANTxFrame*)0)->data8), 
      /* RTR */ CAN_RTR_DATA,
      /* IDE */ CAN_IDE_EXT,
    },
    /* ID */ {{}},
    /* data */ {},
  };
  frame.EID = can->filter_map.raw;   
  // For request answer data the Meta-Frame only holds the total number of frames     
  canBridge_meta_data_t meta;
  meta.total_payloads = total_frames; 
  frame.data8[7] = meta.raw;      
  // send the Meta-Frame
  aosFBCanTransmitTimeout(&MODULE_HAL_CAN, &frame, TIME_MAX_INTERVAL);

  can->filter_map.cnt = total_frames; //One Meta-Frame
  while (position < payload_size) {
    current_size = (position + sizeof(((CANTxFrame*)0)->data8) < payload_size) ? 
                      sizeof(((CANTxFrame*)0)->data8) : (payload_size - position);
    // send data over the CAN Bus
    CANTxFrame frame = {
      {
        /* DLC */ current_size, 
        /* RTR */ CAN_RTR_DATA,
        /* IDE */ CAN_IDE_EXT,
      },
      /* ID */ {{}},
      /* data */ {},
    };
    frame.EID = can->filter_map.raw; 

    memcpy(frame.data8, &payload_buffer[position], current_size);
    // send the frame
    aosFBCanTransmitTimeout(&MODULE_HAL_CAN, &frame, TIME_MAX_INTERVAL);

    //Calculate the new Position
    position += current_size;
    can->filter_map.cnt--;
  }

  return;
}


/**
 * @brief   Copy the received request data into can frames and send this frames over the CAN Bridge.
 *
 * @param[in] can               Pointer to the canbridge structure.
 *                              Must not be NULL.
 * @param[in] current_service   Current service in the CANBridge list of services.
 */
static void _canBridgetransmitRequestData(canBridge_node_t* can, canBridge_service_list_t* current_service) {
  urtDebugAssert(can != NULL);

  // local variables
  uint8_t position = 0;
  size_t current_size;
  size_t payload_size;
  bool fireandforget;
  void* payload;
  uint8_t payload_buffer[current_service->payload_size];
  payload = payload_buffer;
  bool new_request;
  uint8_t total_frames;

  for (uint8_t dispatched = 0; dispatched < CAN_NUM_RESPONSES; dispatched++) {
    if (!urtServiceDispatchedIsValid(&can->dispatched[dispatched])) {
      new_request = urtServiceDispatch(current_service->service,
                                      &can->dispatched[dispatched],  
                                      payload,
                                      &payload_size,
                                      &fireandforget);

      while(new_request) {
        total_frames = payload_size / sizeof(((CANTxFrame*)0)->data8);
        if (payload_size % sizeof(((CANTxFrame*)0)->data8) != 0) 
          total_frames++;
        can->filter_map.id = current_service->service->id;
        can->filter_map.board_id = can->board_id;

        urt_rtclass_t request_class = urtRequestGetRtClass(can->dispatched[dispatched].request);
        switch (request_class) {
          case URT_NRT: {
            can->filter_map.type = NRT_REQUEST_DATA;
            break;
          }
          case URT_FRT: {
            can->filter_map.type = FRT_REQUEST_DATA;
            break;
          }
          case URT_SRT: {
            can->filter_map.type = SRT_REQUEST_DATA;
            break;
          }
          case URT_HRT: {
            can->filter_map.type = HRT_REQUEST_DATA;
            break;
          }
          default: break;
        }
        
        // Meta-Frame
        CANTxFrame frame = {
          {
            /* DLC */ sizeof(((CANTxFrame*)0)->data8), 
            /* RTR */ CAN_RTR_DATA,
            /* IDE */ CAN_IDE_EXT,
          },
          /* ID */ {{}},
          /* data */ {},
        };
        frame.EID = can->filter_map.raw;   
        // For HRT the last frame holds the deadline
        if (can->filter_map.type == HRT_REQUEST_DATA || can->filter_map.type == (HRT_REQUEST_DATA | REQUEST_ANSWER_DATA)) {
          urt_osTime_t t;
          _urt_hrtrequest_calculateDeadline(((urt_hrtrequest_t*)can->dispatched[dispatched].request), &t);
          // Copy the timestamp   
          memcpy(frame.data8, &t.time, sizeof(((CANTxFrame*)0)->data8)-sizeof(frame.data8[7]));
        } 
        //meta frame also holds the total number of frames (Timestamp really needs only 58 Bits = ~9000 years)
        canBridge_meta_data_t meta;
        meta.total_payloads = total_frames;
        // Check if the request service should respond
        if (fireandforget) {
          meta.fireandforget = 1; //Request service should not respond
        } else {
          meta.fireandforget = 0; //Request service should respond
        } 
        frame.data8[7] = meta.raw;
        // send the Meta-Frame
        aosFBCanTransmitTimeout(&MODULE_HAL_CAN, &frame, TIME_MAX_INTERVAL);

        can->filter_map.cnt = total_frames;
        while (position < payload_size) {
          current_size = (position + sizeof(((CANTxFrame*)0)->data8) < payload_size) ? sizeof(((CANTxFrame*)0)->data8) : (payload_size - position);
          // send data over the CAN Bus
          CANTxFrame frame = {
            {
              /* DLC */ current_size, 
              /* RTR */ CAN_RTR_DATA,
              /* IDE */ CAN_IDE_EXT,
            },
            /* ID */ {{}},
            /* data */ {},
          };
          frame.EID = can->filter_map.raw; 

          memcpy(frame.data8, &payload_buffer[position], current_size);
          // send the frame
          aosFBCanTransmitTimeout(&MODULE_HAL_CAN, &frame, TIME_MAX_INTERVAL);

          //Calculate the new Position
          position += current_size;
          can->filter_map.cnt--;
        } 

        // signal the request back
        if (can->dispatched[dispatched].request != NULL) {
          if (urtServiceTryAcquireRequest(current_service->service, &can->dispatched[dispatched]) == URT_STATUS_OK && !(can->filter_map.type & REQUEST_ANSWER_DATA)) {  
            urtServiceRespond(&can->dispatched[dispatched], current_service->payload_size);
          }
        }
        
        for (uint8_t dispatched = 0; dispatched < CAN_NUM_RESPONSES; dispatched++) {
          if (!urtServiceDispatchedIsValid(&can->dispatched[dispatched])) {
            new_request = urtServiceDispatch(current_service->service,
                                            &can->dispatched[dispatched],  
                                            payload,
                                            NULL,
                                            NULL);
            break;
          }
        }
      }
      return;
    }
  }

  return;
}

/**
 * @brief   Copy the received request data into can frames and send this frames over the CAN Bridge.
 *
 * @param[in] can           Pointer to the canbridge structure.
 *                          Must not be NULL.
 * @param[in] current_sub   Subscriber in the CANBridge list of subscribers.
 */
static void _canBridgetransmitSubscriberData(canBridge_node_t* can, canBridge_subscriber_list_t* current_sub) {
  (void)can;

  // local variables
  urt_osTime_t origin_time;
  void* payload;
  uint8_t payload_buffer[current_sub->payload_size];
  payload = payload_buffer;
  uint8_t position = 0;
  uint8_t current_size;
  uint8_t total_frames;
  size_t payload_size;

  // fetch NRT Subscriber
  while (urtNrtSubscriberFetchNext(current_sub->subscriber, payload, &payload_size, &origin_time, NULL) != URT_STATUS_FETCH_NOMESSAGE) {
    total_frames = payload_size / sizeof(((CANTxFrame*)0)->data8);
    if (payload_size % sizeof(((CANTxFrame*)0)->data8) != 0) 
      total_frames++;
    can->filter_map.cnt = 0;
    can->filter_map.id = current_sub->topic_id;
    can->filter_map.type = TOPIC_DATA;
    can->filter_map.board_id = can->board_id;

    // Meta-Frame
    CANTxFrame frame = {
      {
        /* DLC */ sizeof(((CANTxFrame*)0)->data8), 
        /* RTR */ CAN_RTR_DATA,
        /* IDE */ CAN_IDE_EXT,
      },
      /* ID */ {{}},
      /* data */ {},
    };
    frame.EID = can->filter_map.raw;
    //Meta frame holds the timestamp
    memcpy(frame.data8, &origin_time.time, sizeof(((CANTxFrame*)0)->data8)-sizeof(frame.data8[7]));
    //Meta frame also holds the total_frames (Timestamp really needs only 58 Bits = ~9000 years)
    canBridge_meta_data_t meta;
    meta.total_payloads = total_frames;
    frame.data8[7] = meta.raw;
    // send the frame
    aosFBCanTransmitTimeout(&MODULE_HAL_CAN, &frame, TIME_MAX_INTERVAL);  

    can->filter_map.cnt = total_frames; 
    while (position < payload_size) {
      current_size = (position + sizeof(((CANTxFrame*)0)->data8) < payload_size) ? sizeof(((CANTxFrame*)0)->data8) : (payload_size - position);
      // send data over the CAN Bus
      CANTxFrame frame = {
        {
          /* DLC */ current_size, 
          /* RTR */ CAN_RTR_DATA,
          /* IDE */ CAN_IDE_EXT,
        },
        /* ID */ {{}},
        /* data */ {},
      };
      frame.EID = can->filter_map.raw; 

      memcpy(frame.data8, &payload_buffer[position], current_size);
      // send the frame
      aosFBCanTransmitTimeout(&MODULE_HAL_CAN, &frame, TIME_MAX_INTERVAL);

      //Calculate the new Position
      position += current_size;
      can->filter_map.cnt--;
    }
  }

  return;
}


/**
 * @brief   CAN Callback function for the received frames.
 *
 * @param[in] frame         Pointer to the received frame.
 *                          Must not be NULL.
 * @param[in] canBridge     Pointer to the canbridge structure.
 *                          Must nor be NULL.
 */
static void _canCallback(const CANRxFrame* frame, void* canBridge)
{
  urtDebugAssert(canBridge != NULL);
  urtDebugAssert(frame != NULL);

  // local constants
  canBridge_node_t* const can = (canBridge_node_t*)canBridge;

  canBridge_id_map_t frame_map;
  frame_map.raw = frame->EID;

  switch (frame_map.type) {
  case TOPIC_DATA: {
    for (uint8_t pub = 0; pub < can->num_publisher; pub++) {
      if (frame_map.id == can->publisher_topic_ids[pub]) {
        _canBridgegetFrameData(frame, can);
        break;
      }
    }
    break;
  }
  case REQUEST_ANSWER_DATA: {
    canBridge_service_list_t* current_service = can->services;
    while(current_service != NULL) {
      if (frame_map.id == current_service->service->id) {
        _canBridgegetFrameData(frame, can);
        break;
      } 
    current_service = current_service->next;
    }
    break;
  }
  case NRT_REQUEST_DATA:
  case FRT_REQUEST_DATA:
  case SRT_REQUEST_DATA:
  case HRT_REQUEST_DATA: 
  case NRT_REQUEST_DATA | REQUEST_ANSWER_DATA:
  case FRT_REQUEST_DATA | REQUEST_ANSWER_DATA:
  case SRT_REQUEST_DATA | REQUEST_ANSWER_DATA:
  case HRT_REQUEST_DATA | REQUEST_ANSWER_DATA: {
    for (uint8_t request = 0; request < can->num_request; request++) {
      if (frame_map.id == can->request_service_ids[request]) {
        _canBridgegetFrameData(frame, can);
        break;
      }
    }
    break;
  }
  default:break;
  }

  return;
}

/**
 * @brief   Setup callback function for canbridge nodes.
 *
 * @param[in] node         Pointer to the node object.
 *                         Must not be NULL.
 * @param[in] canBridge    Pointer to the canbridge structure.
 *                         Must not be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _canBridgeSetup(urt_node_t* node, void* canBridge)
{
  urtDebugAssert(node != NULL);
  urtDebugAssert(canBridge != NULL);
  (void)node;

#if CH_CFG_USE_REGISTRY == TRUE
  // set thread name
  chRegSetThreadName(_canbridge_name);
#endif

  // local constants
  canBridge_node_t* const can = (canBridge_node_t*)canBridge;

  // set callback of the can filter
  can->can_filter.callback = _canCallback;
  // add CAN Bus filter to the CAN driver
  aosFBCanAddFilter(&MODULE_HAL_CAN, &can->can_filter);

  for (uint8_t f = 0; f < CAN_NUM_FRAMES; f++) {
    can->frames[f].status = BLANK;
  }

  // subscribe to the topics
  canBridge_subscriber_list_t* current_sub = can->subscriber;
  while (current_sub != NULL) {
    //subscribe to the topic
    urt_topic_t* const topic = urtCoreGetTopic(current_sub->topic_id);
    urtDebugAssert(topic != NULL);
    urtNrtSubscriberSubscribe(current_sub->subscriber, topic, *current_sub->mask);

    current_sub = current_sub->next;
  }

  // register the queue event to the event listener
  urtEventRegister(&can->frame_signal.source, &can->frame_signal.listener, FRAMEEVENT,
                   NEWDATAEVENT);

  return can->frame_signal.events;
}

/**
 * @brief   Loop callback function for canbridge nodes.
 *
 * @param[in] node        Execution node of the CANBridge.
 * @param[in] event       Received event.
 * @param[in] canBridge   Pointer to the canbridge structure.
*                         Must not be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _canBridgeLoop(urt_node_t* node, urt_osEventMask_t event, void* canBridge)
{
  urtDebugAssert(canBridge != NULL);
  (void)node;

  // local constants
  canBridge_node_t* const can = (canBridge_node_t*)canBridge;

  //Check if a new frame is received over the CANBridge
  if (event & FRAMEEVENT) {
    // get flags
    urt_osEventFlags_t flags = urtEventListenerClearFlags(&can->frame_signal.listener, 
                                                    NEWDATAEVENT);

    if (flags & NEWDATAEVENT) {
      for (uint8_t f = 0; f < CAN_NUM_FRAMES; f++) {
        if (can->frames[f].status == DONE) {
          switch(can->frames[f].type) {
            case TOPIC_DATA: {
              // Copy the frame payload into the publisher payload
              _canBridgecopyPayload(can, can->publisher_payloads, f);
              urtPublisherInit(&can->publisher, urtCoreGetTopic(can->frames[f].id));
              urtPublisherPublish(&can->publisher, 
                    &can->publisher_payloads, 
                    can->frames[f].bytes, 
                    &can->frames[f].timestamp, 
                    URT_PUBLISHER_PUBLISH_DETERMINED);
              can->frames[f].status = BLANK;
              break;
            }
            case REQUEST_ANSWER_DATA: {
              for (uint8_t response = 0; response < CAN_NUM_RESPONSES; response++) {
                if (can->dispatched[response].request->service->id == can->frames[f].id) {
                  _canBridgecopyPayload(can, can->response_payloads[response], f);
                  // Copy the current data payload into the dispatched payload
                  memcpy(can->dispatched[response].request->payload, can->response_payloads[response], can->frames[f].bytes);
                  // Respond to the request
                  urtServiceRespond(&can->dispatched[response], can->frames[f].bytes);
                  can->frames[f].status = BLANK;  
                  break;
                }
              }
              break;
            }
            case NRT_REQUEST_DATA: 
            case FRT_REQUEST_DATA: 
            case SRT_REQUEST_DATA: 
            case HRT_REQUEST_DATA: 
            case NRT_REQUEST_DATA | REQUEST_ANSWER_DATA: 
            case FRT_REQUEST_DATA | REQUEST_ANSWER_DATA: 
            case SRT_REQUEST_DATA | REQUEST_ANSWER_DATA: 
            case HRT_REQUEST_DATA | REQUEST_ANSWER_DATA: {
              for (uint8_t request = 0; request <= CAN_NUM_REQUESTS; request++) { //TODO: Problem - no request "vorrätig" - überlauf -> Trash?
                if (request == CAN_NUM_REQUESTS) {
                  // Also trash this frame because there is not empty request ("Sender of this frame works faster than receiver")
                  can->frames[f].status = METATRASH;
                  _canBridgetrashFrameCouldbeTrashed(&can->frames[f]);
                  break;
                }
                if (urtRequestTryAcquire(((urt_baserequest_t*)&can->requests[request])) == URT_STATUS_OK) {
                  _canBridgecopyPayload(can, can->request_payloads[request], f);
                  _canBridgesendRequest((canBridge_node_t*)can,
                              request, 
                              can->frames[f].id, 
                              can->frames[f].bytes, 
                              can->frames[f].type, 
                              can->frames[f].timestamp, 
                              can->frames[f].metafftp.fireandforget);
                  can->frames[f].status = BLANK;
                  break;
                }
              }
              break;
            }
            default:break; 
          }
        }
      }
      event &= ~FRAMEEVENT;
    }
  }

  if (event & REQUESTANSWEREVENT) {
    for (uint8_t request = 0; request < CAN_NUM_REQUESTS; request++) {
      size_t size;
      if (urtRequestIsPending(&can->requests[request].base)
        && urtRequestRetrieve(&can->requests[request].base, URT_REQUEST_RETRIEVE_DETERMINED , &size, NULL) == URT_STATUS_OK) {     
        _canBridgetransmitRequestAnswerData(can, request, size);
        urtRequestRelease(&can->requests[request].base);
        event &= ~REQUESTANSWEREVENT;
      }
    }
  }

  //Check if request data should be send over the CANBridge
  canBridge_service_list_t* current_service = can->services;
  while (current_service != NULL) {
    if (event & current_service->service->notification.mask) {
      _canBridgetransmitRequestData(can, current_service);
      event  &= ~(current_service->service->notification.mask);
    }
    current_service = current_service->next;
  }

  //Check if subscriber data should be send over the CANBridge
  canBridge_subscriber_list_t* current_sub = can->subscriber;
  uint8_t sub_cnt = 0;
  while(current_sub != NULL) {
    if (event & *current_sub->mask) {
      can->subscriber_transmit_cnt[sub_cnt]++;
      if (can->subscriber_transmit_cnt[sub_cnt]>=current_sub->transmit_factor) {
        _canBridgetransmitSubscriberData(can, current_sub);
        event  &= ~(*current_sub->mask);
        can->subscriber_transmit_cnt[sub_cnt]=0;
      }
    }
    current_sub = current_sub->next;
    sub_cnt++;
  }

  return can->frame_signal.events;
}

/**
 * @brief   Shutdown callback function for canbridge nodes.
 *
 * @param[in] node    Execution node of the CANBridge
 * @param[in] reason  Reason for the shutdown.
 * @param[in] can     Pointer to the canbridge structure.
 *                    Must not be NULL.
 */
void _canBridgeShutdown(urt_node_t* node, urt_status_t reason, void* can)
{
  urtDebugAssert(can != NULL);

  (void)reason;
  (void)node;

  canBridge_subscriber_list_t* current_sub = ((canBridge_node_t*)can)->subscriber;
  while (current_sub != NULL) {
    // Unsubscribe from module specific topic
    urtNrtSubscriberUnsubscribe(current_sub->subscriber);
    current_sub = current_sub->next;
  } 

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_canbridge
 * @{
 */

/**
 * @brief   CAN Bridge initialization function.
 *
 * @param[in] can                             Can object to initialize.
 *                                            Must not be NULL.
 * @param[in] subscriber                      List with CANBridge subscriber information.
 *                                            Can be NULL if the CANBridge should not transmit topic data.
 * @param[in] publisher_topic_ids             List with topic ids where the CANBridge should listen to.
 *                                            Can be NULL if the CANBridge should not receive topic data.
 * @param[in] num_publisher                   Number of Publisher ids.
 * @param[in] services                        List with CANBridge service information.
 *                                            Can be NULL if the CANBridge should not transmit service data.
 * @param[in] request_service_ids             List with all service IDs where the CANBridge should listen to.
 *                                            Can be NULL if the CANBridge should not receive service data.
 * @param[in] num_request                     Number of request ids.
 * @param[in] prio                            Priority of the execution thread.
 */
void canBridgeInit(canBridge_node_t* can, 
                   canBridge_subscriber_list_t* subscriber,
                   urt_topicid_t publisher_topic_ids[],
                   uint8_t num_publisher,
                   canBridge_service_list_t* services,
                   urt_serviceid_t request_service_ids[],
                   uint8_t num_request,
                   urt_osThreadPrio_t prio)
{
  urtDebugAssert(can != NULL);

  // set the service information in the can node object
  can->services = services;

  //set the subscriber information in the can node object
  can->subscriber = subscriber;

  //check sub struct
  uint8_t sub_cnt = 0;
  canBridge_subscriber_list_t* current_sub = can->subscriber;
  while(current_sub != NULL) {
    urtDebugAssert(current_sub->transmit_factor>=1);
    //todo check payload == topic payload
    can->subscriber_transmit_cnt[sub_cnt]=0;
    current_sub = current_sub->next;
    sub_cnt++;
  }

  //set the publisher information in the can node object
  can->publisher_topic_ids = publisher_topic_ids;
  can->num_publisher = num_publisher;

  //set the service information in the can node object
  can->request_service_ids = request_service_ids;
  can->num_request = num_request;

if (AMIROOS_CFG_SSSP_ENABLE == true && aos.sssp.moduleId != AOS_SSSP_MODULEID_INVALID)
  can->board_id = aos.sssp.moduleId;
else {
#if (defined(BOARD_DIWHEELDRIVE_1_1) || defined(BOARD_DIWHEELDRIVE_1_2)) 
  can->board_id = DIWHEELDRIVE;
#elif (defined(BOARD_POWERMANAGEMENT_1_1) || defined(BOARD_POWERMANAGEMENT_1_2))
  can->board_id = POWERMANAGEMENT;
#elif (defined(BOARD_LIGHTRING_1_1) || defined(BOARD_LIGHTRING_1_2))
  can->board_id = LIGHTRING;
#else
  can->board_id = UNKNOWN;
#endif  
}

  // initialize the node
  urtNodeInit(&can->node, (urt_osThread_t*)can->thread, sizeof(can->thread), prio,
              _canBridgeSetup, can,
              _canBridgeLoop, can,
              _canBridgeShutdown, can);

  // specify CAN Bus filter to get only for this app relevant messages
  can->can_filter.mask.ext.id = 0; //Equal bit of these two masks must match other not
  can->can_filter.mask.ext.ide = ~0;
  can->can_filter.mask.ext.rtr = ~0;
  can->can_filter.filter.ext.id = ~0; // should only receive messages from this ID
  can->can_filter.filter.ext.ide = CAN_IDE_EXT;
  can->can_filter.filter.ext.rtr = CAN_RTR_DATA;
  can->can_filter.cbparams = can;

  // set/initialize event data
  urtEventSourceInit(&can->frame_signal.source);
  urtEventListenerInit(&can->frame_signal.listener);

  // Node should listen to Frame events (new frame should be published/ requested)
  // Node should listen to Request answer events (service responds to request)
  can->frame_signal.events = FRAMEEVENT | REQUESTANSWEREVENT;

  // initialize the services
  canBridge_service_list_t* current_service = services;
  while(current_service != NULL) {
    urtServiceInit(current_service->service,
                   current_service->service->id,
                   can->node.thread,
                   current_service->service->notification.mask);
    //node should also listen to the service event
    can->frame_signal.events |= current_service->service->notification.mask;
    current_service = current_service->next;
  }

  // initialize the CAN requests as NRT request
  for (uint8_t request = 0; request < CAN_NUM_REQUESTS; request++) {
    urtNrtRequestInit(&can->requests[request].nrt, can->request_payloads[request]);
  }

  // initialize the CAN subscriber as NRT subscriber
  current_sub = subscriber;
  while (current_sub != NULL) {
    // initialize all subscriber
    urtNrtSubscriberInit(current_sub->subscriber);
    //node should also listen to the subscriber event
    can->frame_signal.events |= *current_sub->mask;

    current_sub = current_sub->next;
  }

  return;
}

/** @} */
