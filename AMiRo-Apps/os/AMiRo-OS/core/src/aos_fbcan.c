/*
AMiRo-OS is an operating system designed for the Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2016..2022  Thomas Schöpping et al.

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
 * @file    aos_fbcan.c
 * @brief   Filtered & Buffered CAN driver code.
 * @details Implementation of the AosFBCANDriver.
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_FBCAN1_ENABLE == true) ||                                     \
     (AMIROOS_CFG_FBCAN2_ENABLE == true) ||                                     \
     (AMIROOS_CFG_FBCAN3_ENABLE == true)) ||                                    \
    defined(__DOXYGEN__)

#if (AMIROOS_CFG_FBCAN_SERIAL_BUFFERSIZE > 0)
#include <string.h>
#endif /* (AMIROOS_CFG_FBCAN_SERIAL_BUFFERSIZE > 0) */

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/**
 * @brief   Maximum number of bytes that can be transmitted via CAN according to
 *          the standard.
 */
#define CAN_FRAME_MAXBYTES                      8

#if (AMIROOS_CFG_FBCAN_SERIAL_BUFFERSIZE > 0) || defined(__DOXYGEN__)
/**
 * @brief   Delay SDCAN transmissions to reduce bus load and thus allow the
 *          receivers to keep up.
 * @details When tranmitting long texts (thus lots of CAN messages) the
 *          receiving node may be unable to keep up, i.e. because of further
 *          successive I/O (e.g. printing to another serial connection). By
 *          introducing an brief pause before each CAN transmission, this issue
 *          can be leveraged.
 */
#if !defined(SDCAN_SENDDELAY_US)
#define SDCAN_SENDDELAY_US                      1000
#endif

#if (SDCAN_SENDDELAY_US > 0) || defined(__DOXYGEN__)
/**
 * @brief   Delay SDCAN transmission by SDCAN_SENDDELAY_US.
 */
# define _aosSdcanDelayTransmission()           aosThdUSleep(SDCAN_SENDDELAY_US)
#else
# define _aosSdcanDelayTransmission()           (void)
#endif /* (SDCAN_SENDDELAY_US > 0) */

#endif /* (AMIROOS_CFG_FBCAN_SERIAL_BUFFERSIZE > 0) */

/******************************************************************************/
/* EXPORTED VARIABLES                                                         */
/******************************************************************************/

/**
 * @addtogroup core_fbcan
 * @{
 */

#if (AMIROOS_CFG_FBCAN1_ENABLE == true) || defined(__DOXYGEN__)
/**
 * @brief   FBCAN driver for CAN1.
 */
AosFBCANDriver FBCAND1 = {
  /* CAN driver */ &CAND1,
  /* rxfilters  */ NULL,
  /* txbuffer   */ {
    /* avail            */ NULL,
    /* size             */ 0,
    /* queue_front      */ NULL,
    /* queue_back       */ NULL,
    /* enqueue_callback */ NULL,
    /* cbparams         */ NULL,
  },
  /* events     */ {},
};
#endif /* (AMIROOS_CFG_FBCAN1_ENABLE == true) */

#if (AMIROOS_CFG_FBCAN2_ENABLE == true) || defined(__DOXYGEN__)
/**
 * @brief   FBCAN driver for CAN2.
 */
AosFBCANDriver FBCAND2 = {
  /* CAN driver */ &CAND2,
  /* rxfilters  */ NULL,
  /* txbuffer   */ {
    /* avail            */ NULL,
    /* size             */ 0,
    /* queue_front      */ NULL,
    /* queue_back       */ NULL,
    /* enqueue_callback */ NULL,
    /* cbparams         */ NULL,
  },
  /* events     */ {},
};
#endif /* (AMIROOS_CFG_FBCAN2_ENABLE == true) */

#if (AMIROOS_CFG_FBCAN3_ENABLE == true) || defined(__DOXYGEN__)
/**
 * @brief   FBCAN driver for CAN3.
 */
AosFBCANDriver FBCAND3 = {
  /* CAN driver */ &CAND3,
  /* rxfilters  */ NULL,
  /* txbuffer   */ {
    /* avail            */ NULL,
    /* size             */ 0,
    /* queue_front      */ NULL,
    /* queue_back       */ NULL,
    /* enqueue_callback */ NULL,
    /* cbparams         */ NULL,
  },
  /* events     */ {},
};
#endif /* (AMIROOS_CFG_FBCAN3_ENABLE == true) */

/** @} */

/******************************************************************************/
/* LOCAL TYPES                                                                */
/******************************************************************************/

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   LLD receive callback.
 * @details This callback is called only once when a message arrives in the
 *          mailbox. It is the responsibility of this function to fetch messages
 *          from the mailbox until it is empty.
 *
 * @param[in] canp    Pointer to the @p CANDriver object.
 * @param[in] flags   Flags that have been set by the ISR before.
 */
static void _aosFbcanRxfullCallback(CANDriver* canp, uint32_t flags)
{
  // local variables
  AosFBCANDriver* const fbcanp =
#if (AMIROOS_CFG_FBCAN1_ENABLE == true)
      (canp == &CAND1) ? &FBCAND1 :
#endif /* (AMIROOS_CFG_FBCAN1_ENABLE == true) */
#if (AMIROOS_CFG_FBCAN2_ENABLE == true)
      (canp == &CAND2) ? &FBCAND2 :
#endif /* (AMIROOS_CFG_FBCAN2_ENABLE == true) */
#if (AMIROOS_CFG_FBCAN3_ENABLE == true)
      (canp == &CAND3) ? &FBCAND3 :
#endif /* (AMIROOS_CFG_FBCAN3_ENABLE == true) */
      NULL;
  CANRxFrame frame;

  chSysLockFromISR();

  if (canp==NULL) {
    chSysHalt("CAN is NULL");
  }
  if (&frame==NULL) { 
    chSysHalt("FRAME is NULL"); 
  }
  if (canp->state == CAN_STOP) {
    chSysHalt("CAN is in CAN_STOP STATE!");
  }
  if (!(canp->state == CAN_READY) || (canp->state == CAN_SLEEP)) {
    chSysHalt("CAN is in unknown state");
  }

  // fetch first frame
  bool empty = canTryReceiveI(canp, CAN_ANY_MAILBOX, &frame);
  while (!empty) {
    // iterate over all filters
    aos_fbcan_filter_t* filter = fbcanp->rxfilters;
    while (filter != NULL) {
      // check whether frame matches the filter
      if ((filter->mask.std.rtr ? filter->filter.std.rtr == frame.RTR : true) &&
          (filter->mask.std.ide ? filter->filter.std.ide == frame.IDE : true) &&
          (frame.IDE == CAN_IDE_STD ?
           (filter->mask.std.id & filter->filter.std.id) == (filter->mask.std.id & frame.SID) :
           (filter->mask.ext.id & filter->filter.ext.id) == (filter->mask.ext.id & frame.EID))) {
        // execute filter callback
        filter->callback(&frame, filter->cbparams);
      }
      filter = filter->next;
    }

    // try to fetch further frame
    empty = canTryReceiveI(canp, CAN_ANY_MAILBOX, &frame);

    if (!empty) {
      // allow for other ISRs to be executed
      chSysUnlockFromISR();
      chSysLockFromISR();
    }
  }

  // broadcast event
  chEvtBroadcastFlagsI(&fbcanp->events.rxfull, flags);

  chSysUnlockFromISR();

  return;
}

/**
 * @brief   LLD transmit callback.
 * @details This callback is called only once when a mailbox becomes available.
 *
 * @param[in] canp    Pointer to the @p CANDriver object.
 * @param[in] flags   Flags that have been set by the ISR before.
 */
static void _aosFbcanTxemptyCallback(CANDriver* canp, uint32_t flags)
{
  // local variables
  AosFBCANDriver* const fbcanp =
#if (AMIROOS_CFG_FBCAN1_ENABLE == true)
      (canp == &CAND1) ? &FBCAND1 :
#endif /* (AMIROOS_CFG_FBCAN1_ENABLE == true) */
#if (AMIROOS_CFG_FBCAN2_ENABLE == true)
      (canp == &CAND2) ? &FBCAND2 :
#endif /* (AMIROOS_CFG_FBCAN2_ENABLE == true) */
#if (AMIROOS_CFG_FBCAN3_ENABLE == true)
      (canp == &CAND3) ? &FBCAND3 :
#endif /* (AMIROOS_CFG_FBCAN3_ENABLE == true) */
      NULL;

  chSysLockFromISR();

  // transmit as many frames from the queue as possible
  while (fbcanp->txbuffer.queue_front != NULL &&
         can_lld_is_tx_empty(canp, CAN_ANY_MAILBOX)) {
    // fetch frame from queue
    aos_fbcan_txframe_t* frame = fbcanp->txbuffer.queue_front;
    fbcanp->txbuffer.queue_front = fbcanp->txbuffer.queue_front->next;
    if (fbcanp->txbuffer.queue_front != NULL) {
      fbcanp->txbuffer.queue_front->prev = NULL;
    } else {
      fbcanp->txbuffer.queue_back = NULL;
    }
    // put frame to harwdare mailbox
    can_lld_transmit(canp, CAN_ANY_MAILBOX, &frame->frame);
    // make frame available again
    frame->next = fbcanp->txbuffer.avail;
    fbcanp->txbuffer.avail = frame;
  }

  // broadcast event
  chEvtBroadcastFlagsI(&fbcanp->events.txempty, flags);

  chSysUnlockFromISR();

  return;
}

/**
 * @brief   LLD error callback.
 *
 * @param[in] canp    Pointer to the @p CANDriver object.
 * @param[in] flags   Flags that have been set by the ISR before.
 */
static void _aosFbcanErrorCallback(CANDriver* canp, uint32_t flags)
{
  // local variables
  AosFBCANDriver* const fbcanp =
#if (AMIROOS_CFG_FBCAN1_ENABLE == true)
      (canp == &CAND1) ? &FBCAND1 :
#endif /* (AMIROOS_CFG_FBCAN1_ENABLE == true) */
#if (AMIROOS_CFG_FBCAN2_ENABLE == true)
      (canp == &CAND2) ? &FBCAND2 :
#endif /* (AMIROOS_CFG_FBDAN2_ENABLE == true) */
#if (AMIROOS_CFG_FBCAN3_ENABLE == true)
      (canp == &CAND3) ? &FBCAND3 :
#endif /* (AMIROOS_CFG_FBCAN3_ENABLE == true) */
      NULL;

  chSysLockFromISR();

  // broadcast event
  chEvtBroadcastFlagsI(&fbcanp->events.error, flags);

  chSysUnlockFromISR();

  return;
}

#if (CAN_USE_SLEEP_MODE == TRUE) || defined (__DOXYGEN__)

/**
 * @brief   LLD wakeup callback
 *
 * @param[in] canp    Pointer to the @p CANDriver object.
 * @param[in] flags   Flags that have been set by the ISR before.
 */
static void _aosFbcanWakeupCallback(CANDriver* canp, uint32_t flags)
{
  // local variables
  AosFBCANDriver* const fbcanp =
#if (AMIROOS_CFG_FBCAN1_ENABLE == true)
      (canp == &CAND1) ? &FBCAND1 :
#endif /* (AMIROOS_CFG_FBCAN1_ENABLE == true) */
#if (AMIROOS_CFG_FBCAN2_ENABLE == true)
      (canp == &CAND2) ? &FBCAND2 :
#endif /* (AMIROOS_CFG_FBCAN2_ENABLE == true) */
#if (AMIROOS_CFG_FBCAN3_ENABLE == true)
      (canp == &CAND3) ? &FBCAND3 :
#endif /* (AMIROOS_CFG_FBCAN3_ENABLE == true) */
      NULL;

  chSysLockFromISR();

  // broadcast event
  chEvtBroadcastFlagsI(&fbcanp->events.wakeup, flags);

  chSysUnlockFromISR();

  return;
}

#endif /* (CAN_USE_SLEEP_MODE == TRUE) */

/**
 * @brief   Default enqueue callbakc function.
 *
 * @param[in]     frame         Frame to enqueue.
 *                              Must not be NULL.
 * @param[in,out] queue_front   First element in the queue.
 *                              Must not be NULL.
 *                              If the frame is prepended, the @p prev pointer
 *                              must be modified accordingly.
 * @param[in,out] queue_back    Last element in the queue.
 *                              Must not be NULL.
 *                              If the frame is appended, the @p next pointer
 *                              must be modified accordingly.
 * @param[in]     params        Optional parameters.
 */
static void _aosFbcanEnqueueCallback(aos_fbcan_txframe_t* frame, aos_fbcan_txframe_t* queue_front, aos_fbcan_txframe_t* queue_back, void* params)
{
  aosDbgCheck(frame != NULL);
  aosDbgCheck(queue_front != NULL);
  aosDbgCheck(queue_back != NULL);

  (void)params;

  // append frame to the queue (FIFO)
  frame->prev = queue_back;
  frame->next = NULL;
  queue_back->next = frame;

  return;
}

#if (AMIROOS_CFG_FBCAN_SERIAL_BUFFERSIZE > 0) || defined(__DOXYGEN__)

/**
 * @brief   SerialCANDriver filter callback.
 * @details Incomung data it put in the input queue.
 *          If the queue is full any further received data is discarded.
 *
 * @param[in] frame   Pointer to the received frame.
 * @param[in] params  Pointer to the SerialCANDriver object.
 */
static void _aosSdcanFilterCallback(const CANRxFrame* frame, void* params)
{
  // put bytes into queue but discard incoming bytes if the buffer is full
  for (size_t byte = 0; byte < frame->DLC; ++byte) {
    if (iqPutI(&((SerialCANDriver*)params)->iqueue, frame->data8[byte]) != MSG_OK) {
      break;
    }
  }
  // fire event
  chEvtBroadcastFlagsI(&((SerialCANDriver*)params)->event, CHN_INPUT_AVAILABLE);

  return;
}

/**
 * @brief   Serial write function.
 *
 * @param[in] ip  Pointer to the SerialCANDriver object.
 * @param[in] bp  Pointer to a serial byte buffer.
 * @param[in] n   Number of bytes to write rom the buffer.
 *
 * @return  Number of bytes written.
 */
static size_t _aosSdcanWrite(void *ip, const uint8_t *bp, size_t n) {
  // local variables
  size_t transmitted = 0;
  aos_status_t status = AOS_OK;
  CANTxFrame frame = {
    {
      /* DLC  */ 0,
      /* RTR  */ CAN_RTR_DATA,
      /* IDE  */ ((SerialCANDriver*)ip)->config->canid.ext.ide,
    },
    /* SID/EID  */ {},
    /* data     */ {},
  };
  if (frame.IDE == CAN_IDE_STD) {
    frame.SID = ((SerialCANDriver*)ip)->config->canid.std.id;
  } else {
    frame.EID = ((SerialCANDriver*)ip)->config->canid.ext.id;
  }

  // divide write buffer into chunks
  while (transmitted < n) {
    // transmit frame
    frame.DLC = (n - transmitted < CAN_FRAME_MAXBYTES) ? n - transmitted : CAN_FRAME_MAXBYTES;
    memcpy(frame.data8, bp, frame.DLC);
    _aosSdcanDelayTransmission();
    status = aosFBCanTransmitTimeout(((SerialCANDriver*)ip)->config->fbcanp, &frame, TIME_INFINITE);
    if (status != AOS_FAILURE && status != AOS_TIMEOUT) {
      // proceed
      bp += frame.DLC;
      transmitted += frame.DLC;
    } else {
      break;
    }
  }

  return transmitted;
}

/**
 * @brief   Serial read function.
 *
 * @param[in]  ip   Pointer to the SerialCANDriver object.
 * @param[out] bp   Buffer to store the data to.
 * @param[in]  n    Number of bytes to read.
 *
 * @return  Number of bytes read.
 */
static size_t _aosSdcanRead(void *ip, uint8_t *bp, size_t n) {
  return iqReadTimeout(&((SerialCANDriver*)ip)->iqueue, bp, n, TIME_INFINITE);
}

/**
 * @brief   Sreial put function.
 *
 * @param[in] ip  Pointer to the SerialCANDriver object.
 * @param[in] b   Byte to write.
 *
 * @return  The operation result.
 * @retval  MSG_OK      Byte was transmitted successfully.
 * @retval  MSG_RESET   Operation failed.
 */
static msg_t _aosSdcanPut(void *ip, uint8_t b) {
  // local variables
  CANTxFrame frame = {
    {
      /* DLC  */ 1,
      /* RTR  */ CAN_RTR_DATA,
      /* IDE  */ ((SerialCANDriver*)ip)->config->canid.ext.ide,
    },
    /* SID/EID  */ {},
    /* data     */ {},
  };
  if (frame.IDE == CAN_IDE_STD) {
    frame.SID = ((SerialCANDriver*)ip)->config->canid.std.id;
  } else {
    frame.EID = ((SerialCANDriver*)ip)->config->canid.ext.id;
  }
  frame.data8[0] = b;

  _aosSdcanDelayTransmission();
  const aos_status_t status = aosFBCanTransmitTimeout(((SerialCANDriver*)ip)->config->fbcanp, &frame, TIME_INFINITE);

  return (status == AOS_FAILURE) ? MSG_RESET : MSG_OK;
}

/**
 * @brief   Serial get function.
 *
 * @param[in] ip  Pointer to the SerialCANDriver object.
 *
 * @return  Byte value.
 * @retval MSG_RESET  The input queue has been reset.
 */
static msg_t _aosSdcanGet(void *ip) {
  return iqGet(&((SerialCANDriver*)ip)->iqueue);
}

/**
 * @brief   Serial put function with timeout.
 *
 * @param[in] ip        Pointer to the SerialCANDriver object.
 * @param[in] b         Byte to write.
 * @param[in] timeout   Timeout in system ticks.
 *
 * @return  The operation result.
 * @retval MSG_OK       Operation successfull.
 * @retval MSG_TIMEOUT  Operation timed out.
 * @retval MSG_RESET    Operation failed.
 */
static msg_t _aosSdcanPutt(void *ip, uint8_t b, sysinterval_t timeout) {
  // local variables
  CANTxFrame frame = {
    {
      /* DLC  */ 1,
      /* RTR  */ CAN_RTR_DATA,
      /* IDE  */ ((SerialCANDriver*)ip)->config->canid.ext.ide,
    },
    /* SID/EID  */ {},
    /* data     */ {},
  };
  if (frame.IDE == CAN_IDE_STD) {
    frame.SID = ((SerialCANDriver*)ip)->config->canid.std.id;
  } else {
    frame.EID = ((SerialCANDriver*)ip)->config->canid.ext.id;
  }
  frame.data8[0] = b;

  _aosSdcanDelayTransmission();
  const aos_status_t status = aosFBCanTransmitTimeout(((SerialCANDriver*)ip)->config->fbcanp, &frame, (timeout > SDCAN_SENDDELAY_US) ? (timeout - SDCAN_SENDDELAY_US) : TIME_IMMEDIATE);

  return (status == AOS_FAILURE) ? MSG_RESET : (status == AOS_TIMEOUT) ? MSG_TIMEOUT : MSG_OK;
}

/**
 * @brief   Serial get function with timeout.
 *
 * @param[in] ip        Pointer to the SerialCANDriver object.
 * @param[in] timeout   Timeout in system ticks.
 *
 * @return  Byte value.
 * @retval MSG_TIMEOUT  Operation timed out.
 * @retval MSG_RESET    The input queue has been reset.
 */
static msg_t _aosSdcanGett(void *ip, sysinterval_t timeout) {
  return iqGetTimeout(&((SerialCANDriver*)ip)->iqueue, timeout);
}

/**
 * @brief   Serial write functio with timeout.
 *
 * @param[in] ip        Pointer to the SerialCANDriver object.
 * @param[in] bp        Pointer to a serial byte buffer.
 * @param[in] n         Number of bytes to write rom the buffer.
 * @param[in] timeout   Timeout in system ticks.
 *
 * @return  Number of bytes written.
 */
static size_t _aosSdcanWritet(void *ip, const uint8_t *bp, size_t n, sysinterval_t timeout) {
  // local variables
  size_t transmitted = 0;
  aos_status_t status = AOS_OK;
  aos_timestamp_t starttime;
  CANTxFrame frame = {
    {
      /* DLC  */ 0,
      /* RTR  */ CAN_RTR_DATA,
      /* IDE  */ ((SerialCANDriver*)ip)->config->canid.ext.ide,
    },
    /* SID/EID  */ {},
    /* data     */ {},
  };
  if (frame.IDE == CAN_IDE_STD) {
    frame.SID = ((SerialCANDriver*)ip)->config->canid.std.id;
  } else {
    frame.EID = ((SerialCANDriver*)ip)->config->canid.ext.id;
  }
  aosSysGetUptime(&starttime);

  // divide write buffer into chunks
  while (transmitted < n) {
    // transmit frame
    frame.DLC = (n - transmitted < CAN_FRAME_MAXBYTES) ? n - transmitted : CAN_FRAME_MAXBYTES;
    memcpy(frame.data8, bp, frame.DLC);
    _aosSdcanDelayTransmission();
    if (timeout == TIME_IMMEDIATE) {
      status = aosFBCanTransmitTimeout(((SerialCANDriver*)ip)->config->fbcanp, &frame, TIME_IMMEDIATE);
    } else if (timeout == TIME_INFINITE) {
      status = aosFBCanTransmitTimeout(((SerialCANDriver*)ip)->config->fbcanp, &frame, TIME_INFINITE);
    } else {
      aos_timestamp_t currtime;
      aosSysGetUptime(&currtime);
      const sysinterval_t duration = TIME_US2I(aosTimestampDiff(starttime, currtime));
      status = aosFBCanTransmitTimeout(((SerialCANDriver*)ip)->config->fbcanp, &frame, (timeout > duration) ? (timeout - duration) : TIME_IMMEDIATE);
    }
    if (status != AOS_FAILURE && status != AOS_TIMEOUT) {
      // proceed
      bp += frame.DLC;
      transmitted += frame.DLC;
    } else {
      break;
    }
  }

  return transmitted;
}

/**
 * @brief   Serial read function with timeout.
 *
 * @param[in]  ip       Pointer to the SerialCANDriver object.
 * @param[out] bp       Buffer to store the data to.
 * @param[in]  n        Number of bytes to read.
 * @param[in]  timeout  Timeout in system ticks.
 *
 * @return  Number of bytes read.
 */
static size_t _aosSdcanReadt(void *ip, uint8_t *bp, size_t n, sysinterval_t timeout) {
  return iqReadTimeout(&((SerialCANDriver*)ip)->iqueue, bp, n, timeout);
}

/**
 * @brief   Serial control function.
 *
 * @param[in] ip          Pointer to the SerialCANDriver object.
 * @param[in] operation   Control operation to execute.
 * @param[in] arg         Optional arguments.
 *
 * @return  The operation result.
 */
static msg_t _aosSdcanCtl(void *ip, unsigned int operation, void *arg) {
  SerialCANDriver *sdcanp = (SerialCANDriver*)ip;

  osalDbgCheck(sdcanp != NULL);

  switch (operation) {
  case CHN_CTL_NOP:
    osalDbgCheck(arg == NULL);
    break;
  case CHN_CTL_INVALID:
    osalDbgAssert(false, "invalid CTL operation");
    break;
  default:
#if defined(SDCAN_LLD_IMPLEMENTS_CTL)
    /* The SDCAN driver does not have a LLD but the application can use this
       hook to implement extra controls by supplying this function.*/
    extern msg_t sdcan_lld_control(SerialCANDriver *sdcanp, unsigned int operation, void *arg);
    return sdcan_lld_control(sdcanp, operation, arg);
#else
    break;
#endif
  }

  return MSG_OK;
}

static const struct SerialCANDriverVMT _vmt = {
  (size_t)0,
  _aosSdcanWrite,
  _aosSdcanRead,
  _aosSdcanPut,
  _aosSdcanGet,
  _aosSdcanPutt,
  _aosSdcanGett,
  _aosSdcanWritet,
  _aosSdcanReadt,
  _aosSdcanCtl
};

#endif /* (AMIROOS_CFG_FBCAN_SERIAL_BUFFERSIZE > 0) */

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

/**
 * @addtogroup core_fbcan
 * @{
 */

/**
 * @brief   Inizialize a Filtered & Buffered CAN (FBCAN) object.
 *
 * @param[in] fbcanp            Pointer to the @p AosFBCANDriver object.
 * @param[in] enqueuecb         Optional callback function used to enqueue
 *                              messages to the transmit buffer.
 * @param[in] enqueuecbparams   Optional parameter to be propagated to the
 *                              enqueue callback.
 */
void aosFBCanInit(AosFBCANDriver* fbcanp, aos_fbcan_enqueuecb_t enqueuecb, void* enqueuecbparams)
{
  aosDbgCheck(fbcanp != NULL);

  // set CAN driver callbacks
  fbcanp->can->rxfull_cb = _aosFbcanRxfullCallback;
  fbcanp->can->txempty_cb = _aosFbcanTxemptyCallback;
  fbcanp->can->error_cb = _aosFbcanErrorCallback;
#if (CAN_USE_SLEEP_MODE == TRUE)
  fbcanp->can->wakeup_cb = _aosFbcanWakeupCallback;
#endif /* (CAN_USE_SLEEP_MODE == TRUE) */

  // reset receive filters
  fbcanp->rxfilters = NULL;

  // reset transmit buffer
  while (fbcanp->txbuffer.queue_front != NULL) {
    // make frame available again
    aos_fbcan_txframe_t* frame = fbcanp->txbuffer.queue_front;
    fbcanp->txbuffer.queue_front = fbcanp->txbuffer.queue_front->next;
    frame->prev = NULL;
    frame->next = fbcanp->txbuffer.avail;
    fbcanp->txbuffer.avail = frame;
  }
  fbcanp->txbuffer.queue_back = NULL;

  // set enqueue callback
  fbcanp->txbuffer.enqueue_callback = (enqueuecb != NULL) ? enqueuecb : _aosFbcanEnqueueCallback;
  fbcanp->txbuffer.cbparams = (enqueuecb != NULL) ? enqueuecbparams : NULL;

  // initialize events
  chEvtObjectInit(&fbcanp->events.rxfull);
  chEvtObjectInit(&fbcanp->events.txempty);
  chEvtObjectInit(&fbcanp->events.error);
#if (CAN_USE_SLEEP_MODE == TRUE)
  chEvtObjectInit(&fbcanp->events.sleep);
  chEvtObjectInit(&fbcanp->events.wakeup);
#endif /* (CAN_USE_SLEEP_MODE == TRUE) */

  return;
}

/**
 * @brief   Provide additional memory for the transmit buffer.
 *
 * @param[in] fbcanp  Pointer to the @p AosFBCANDriver object.
 * @param[in] buffer  Pointer to a buffer to provide.
 * @param[in] size    Size of the buffer as number of @p aos_fbcan_txframe_t
 *                    elements.
 *
 * @return  Resulting size of the transmit buffer.
 */
size_t aosFBCanProvideBuffer(AosFBCANDriver* fbcanp, aos_fbcan_txframe_t* buffer, size_t size)
{
  aosDbgCheck(fbcanp != NULL);
  aosDbgCheck(buffer != NULL);
  aosDbgCheck(size > 0);

  chSysLock();

  // prepend the buffer to the list of available frames
  for (size_t idx = 0; idx < size; ++idx) {
    buffer[idx].prev = NULL;
    buffer[idx].next = fbcanp->txbuffer.avail;
    fbcanp->txbuffer.avail = &buffer[idx];
  }

  // increase size variable
  fbcanp->txbuffer.size += size;

  chSysUnlock();

  return fbcanp->txbuffer.size;
}

/**
 * @brief   Try to acquire a transmit frame from the buffer.
 *
 * @param[in] fbcanp    Pointer to the @p AosFBCANDriver to acquire a frame from
 *                      its transmit buffer.
 * @param[in] timeout   Number of ticks before the operation times out.
 *
 * @return  Pointer to the acquired frame or @p NULL if acquisition timed out.
 */
CANTxFrame* aosFBCanAcquireTxFrame(AosFBCANDriver* fbcanp, sysinterval_t timeout)
{
  aosDbgCheck(fbcanp != NULL);

  // local varibles
  aos_fbcan_txframe_t* frame = NULL;
  msg_t msg = MSG_OK;
  aos_timestamp_t starttime;

  chSysLock();

  // This only applies if the transmit buffer is not of zero size
  if (fbcanp->txbuffer.size > 0) {
    aosSysGetUptimeX(&starttime);

    while (frame == NULL && msg == MSG_OK) {
      // try to acquire frame from buffer
      if (fbcanp->txbuffer.avail != NULL) {
        frame = fbcanp->txbuffer.avail;
        fbcanp->txbuffer.avail = frame->next;
        frame->next = NULL;
      } else {
        if (timeout == TIME_IMMEDIATE) {
          msg = MSG_TIMEOUT;
        }
        else if (timeout == TIME_INFINITE) {
          msg = chThdEnqueueTimeoutS(&fbcanp->can->txqueue, TIME_INFINITE);
        } else {
          aos_timestamp_t currtime;
          aosSysGetUptimeX(&currtime);
          msg = chThdEnqueueTimeoutS(&fbcanp->can->txqueue, timeout - TIME_US2I(aosTimestampDiff(starttime, currtime)));
        }
      }
    }
  }

  chSysUnlock();

  return &frame->frame;
}

/**
 * @brief   CAN frame transmission.
 * @details The specified frame is queued for transmission.
 *          If the hardware queue is full, the frame is enqueued in the transmit
 *          queue. The frame is always enqueued in either queue, because it had
 *          to be acquired from the transmit buffer before.
 *
 * @param[in] fbcanp  Pointer to the @p AosFBCANDriver object.
 * @param[in] frame   Pointer to the CAN frame to be transmitted.
 *                    Must have been acquired using @p aosFBCanAcquireTxFrame().
 *
 * @return  The operation result.
 * @retval AOS_SUCCESS  The frame has been queued to the hardware.
 * @retval AOS_WARNING  The frame has been queued to the transmit buffer.
 */
aos_status_t aosFBCanTransmit(AosFBCANDriver* fbcanp, const CANTxFrame* frame)
{
  aosDbgCheck(fbcanp != NULL);
  aosDbgCheck(frame != NULL);

  // local variables
  bool transmitted = false;

  chSysLock();

  // try hardware queue first
  if (fbcanp->can->state == CAN_READY && can_lld_is_tx_empty(fbcanp->can, CAN_ANY_MAILBOX)) {
    can_lld_transmit(fbcanp->can, CAN_ANY_MAILBOX, frame);
    transmitted = true;
  }

  // if the frame was put to the hardware transmit mailbox
  if (transmitted) {
    // make the buffer element available again
    ((aos_fbcan_txframe_t*)frame)->next = fbcanp->txbuffer.avail;
    fbcanp->txbuffer.avail = (aos_fbcan_txframe_t*)frame;
  }
  // if the hardware transmit mailbox is full, but the queue buffer is empty
  else if (fbcanp->txbuffer.queue_front == NULL) {
    // the frame becomes the first and only element in the queue
    ((aos_fbcan_txframe_t*)frame)->prev = NULL;
    ((aos_fbcan_txframe_t*)frame)->next = NULL;
    fbcanp->txbuffer.queue_front = (aos_fbcan_txframe_t*)frame;
    fbcanp->txbuffer.queue_back = (aos_fbcan_txframe_t*)frame;
  }
  // if the hardware transmit mailbox is full, and the queue buffer is not empty
  else {
    // enqueue frame using the callback
    fbcanp->txbuffer.enqueue_callback((aos_fbcan_txframe_t*)frame, fbcanp->txbuffer.queue_front, fbcanp->txbuffer.queue_back, fbcanp->txbuffer.cbparams);
    // check whether front or back elements have changed
    if (fbcanp->txbuffer.queue_front->prev != NULL) {
      fbcanp->txbuffer.queue_front = fbcanp->txbuffer.queue_front->prev;
    } else if (fbcanp->txbuffer.queue_back->next != NULL) {
      fbcanp->txbuffer.queue_back = fbcanp->txbuffer.queue_back->next;
    }
  }

  chSysUnlock();

  return transmitted ? AOS_SUCCESS : AOS_WARNING;
}

/**
 * @brief   Transmit a frame via CAN.
 *
 * @param[in] bcanp     Pointer to the @p AosFBCANDriver object.
 * @param[in] frame     Pointer to the CANTxFrame to transmit.
 * @param[in] timeout   Number of ticks before the operation times out.
 *
 * @return  The operation result.
 * @retval AOS_SUCCESS  The frame has been queued to the hardware.
 * @retval AOS_WARNING  The frame has been queued to the transmit buffer.
 * @retval AOS_TIMEOUT  The frame could not be scheduled for transmission due to
 *                      a timeout.
 * @retval AOS_FAILURE  Transmission failed due to an issue in the CAN driver.
 */
aos_status_t aosFBCanTransmitTimeout(AosFBCANDriver* bcanp, const CANTxFrame* frame, sysinterval_t timeout)
{
  aosDbgCheck(bcanp != NULL);
  aosDbgCheck(frame != NULL);

  // if the transmit buffer size is greater than zero
  if (bcanp->txbuffer.size > 0) {
    // try to acquire a frame from the buffer
    CANTxFrame* acquired = aosFBCanAcquireTxFrame(bcanp, timeout);
    if (acquired != NULL) {
      // copy frame content and transmit
      *acquired = *frame;
      return aosFBCanTransmit(bcanp, acquired);
    } else {
      return AOS_TIMEOUT;
    }
  }
  // if there is no transmit buffer
  else {
    const msg_t msg = canTransmitTimeout(bcanp->can, CAN_ANY_MAILBOX, frame, timeout);
    return (msg == MSG_OK) ? AOS_SUCCESS : (msg == MSG_TIMEOUT) ? AOS_TIMEOUT : AOS_FAILURE;
  }
}

/**
 * @brief   Renove a receive filter.
 *
 * @param[in] fbcanp  Pointer to the @p AosFBCANDriver object.
 * @param[in] filter  Pointer to the filter to be added.
 *
 * @return  The operation result.
 * @retval AOS_SUCCESS  The filter has been removed.
 * @retval AOS_ERROR    The filter is not associated to this @p AosFBCANDriver.
 */
aos_status_t aosFBCanRemoveFilterI(AosFBCANDriver* fbcanp, aos_fbcan_filter_t* filter)
{
  chDbgCheckClassI();
  aosDbgCheck(fbcanp != NULL);
  aosDbgCheck(filter != NULL);

  // local variables
  bool removed = false;

  // search for the filter
  aos_fbcan_filter_t* f = fbcanp->rxfilters;
  // special case: the filter is the first in the list
  if (f == filter) {
    fbcanp->rxfilters = filter->next;
    filter->next = NULL;
    removed = true;
  }
  // the filter is second or later in the list
  else {
    while (f != NULL && f->next != filter) {
      f = f->next;
    }
    // if the filter was found
    if (f != NULL && f->next == filter) {
      f->next = filter->next;
      filter->next = NULL;
      removed = true;
    }
  }

  return removed ? AOS_OK : AOS_ERROR;
}

#if (CAN_USE_SLEEP_MODE == TRUE) || defined(__DOXYGEN__)

/**
 * @brief   Enters the sleep mode.
 * @details This function puts the CAN driver in sleep mode and broadcasts the
 *          @p sleep_event event source.
 * @pre     In order to use this function the option @p CAN_USE_SLEEP_MODE must
 *          be enabled and the @p CAN_SUPPORTS_SLEEP mode must be supported by
 *          the low level driver.
 *
 * @param[in] fbcanp  Pointer to the @p aosBufferedCanDriver object.
 */
void aosFBCanSleep(AosFBCANDriver* fbcanp)
{
  aosDbgCheck(fbcanp != NULL);

  chSysLock();

  // set CAN driver to sleep mode
  aosDbgAssertMsg((fbcanp->can->state == CAN_READY) ||
                  (fbcanp->can->state == CAN_SLEEP), "invalid state");
  if (fbcanp->can->state == CAN_READY) {
    can_lld_sleep(fbcanp->can);
    fbcanp->can->state = CAN_SLEEP;
  }

  // broadcast event
  chEvtBroadcastFlagsI(&fbcanp->events.sleep, (eventflags_t)0);
  chSchRescheduleS();

  chSysUnlock();

  return;
}

/**
 * @brief   Enforces leaving the sleep mode.
 * @note    The sleep mode is supposed to be usually exited automatically by an
 *          hardware event.
 *
 * @param[in] fbcanp  Pointer to the @p aosBufferedCanDriver object.
 */
void aosFBCanWakeup(AosFBCANDriver *fbcanp)
{
  aosDbgCheck(fbcanp != NULL);

  chSysLock();

  // wakeup CAN driver
  aosDbgAssertMsg((fbcanp->can->state == CAN_READY) ||
                  (fbcanp->can->state == CAN_SLEEP), "invalid state");
  if (fbcanp->can->state == CAN_SLEEP) {
    can_lld_wakeup(fbcanp->can);
    fbcanp->can->state = CAN_READY;
  }

  // broadcast event
  chEvtBroadcastFlagsI(&fbcanp->events.wakeup, (eventflags_t)0);
  chSchRescheduleS();

  chSysUnlock();
}

#endif /* (CAN_USE_SLEEP_MODE == TRUE) */

/** @} */

#if (AMIROOS_CFG_FBCAN_SERIAL_BUFFERSIZE > 0) || defined(__DOXYGEN__)

/**
 * @addtogroup core_fbcan_sdcan
 * @{
 */

/**
 * @brief   SerialCANDriver object initialization.
 *
 * @param[in] sdcanp  SerialCANDriver object to initialize.
 */
void sdcanObjectInit(SerialCANDriver* sdcanp)
{
  aosDbgCheck(sdcanp != NULL);

  sdcanp->vmt = &_vmt;
  chEvtObjectInit(&sdcanp->event);
  iqObjectInit(&sdcanp->iqueue, sdcanp->ib, AMIROOS_CFG_FBCAN_SERIAL_BUFFERSIZE, NULL, sdcanp);
  sdcanp->filter.mask.raw = ~0;
  sdcanp->filter.callback = _aosSdcanFilterCallback;
  sdcanp->filter.cbparams = sdcanp;
  sdcanp->state = SDCAN_STOP;

  return;
}

/**
 * @brief   Start the SerialCANDriver.
 *
 * @param[in] sdcanp  The SerialCANDriver object.
 * @param[in] config  Configuration to set.
 */
void sdcanStart(SerialCANDriver* sdcanp, const SerialCANConfig* config)
{
  aosDbgCheck(sdcanp != NULL);
  aosDbgCheck(config != NULL && config->fbcanp != NULL && config->fbcanp->can != NULL);

  chSysLock();

  aosDbgAssertMsg((config->fbcanp->can->state == CAN_READY) ||
                  (config->fbcanp->can->state == CAN_SLEEP), "invalid CAN state");
  aosDbgAssertMsg((sdcanp->state == SDCAN_STOP) ||
                  (sdcanp->state == SDCAN_READY), "invalid state");

  sdcanp->config = config;
  sdcanp->filter.filter = config->canid;
  aosFBCanAddFilterI(sdcanp->config->fbcanp, &sdcanp->filter);
  sdcanp->state = SDCAN_READY;

  chSysUnlock();

  return;
}

/**
 * @brief   Stop the SerialCANDriver.
 *
 * @param[in] sdcanp  The SerialCANDriver object.
 */
void sdcanStop(SerialCANDriver* sdcanp)
{
  aosDbgCheck(sdcanp != NULL);

  chSysLock();

  aosDbgAssertMsg((sdcanp->state == SDCAN_STOP) ||
                  (sdcanp->state == SDCAN_READY), "invalid state");
  sdcanp->state = SDCAN_STOP;
  aosFBCanRemoveFilterI(sdcanp->config->fbcanp, &sdcanp->filter);
  iqResetI(&sdcanp->iqueue);

  chSysUnlock();

  return;
}

/**
 * @brief   Control the SerialCANDriver.
 *
 * @param[in] sdcanp      The SerialCANDriver object.
 * @param[in] operation   Control operation to execute.
 * @param[in] arg         Optional argument.
 *
 * @return  The operation result.
 */
msg_t sdcanControl(SerialCANDriver* sdcanp, unsigned int operation, void* arg)
{
  return _aosSdcanCtl((void*)sdcanp, operation, arg);
}

/** @} */

#endif /* (AMIROOS_CFG_FBCAN_SERIAL_BUFFERSIZE > 0) */

#endif /* (AMIROOS_CFG_FBCANX_ENABLE == true) */
