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
 * @file    intro_wallfollowing.c
 */

#include <intro_wallfollowing.h>
#include <amiroos.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

#define NUM_LEDS            24
#define NUM_RING_SENSORS    8
#define NUM_FLOOR_SENSORS   4

/**
 * @brief   Event masks.
 */
#define PROXRINGEVENT                 (urt_osEventMask_t)(1<< 1)
#define PROXFLOOREVENT                (urt_osEventMask_t)(1<< 2)

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
 * @brief   Name of intro_wallfollowing nodes.
 */
static const char _intro_wallfollowing_name[] = "Intro_WL";

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

//Signal the light service with the new colors for each LED
void intro_wl_signalLightService(intro_wallfollowing_node_t* intro_wl) {
  urtDebugAssert(intro_wl != NULL);

  //Try to acquire the light service
  if (urtNrtRequestTryAcquire(&intro_wl->light.request) == URT_STATUS_OK) {
    //Signal the light service
    urtNrtRequestSubmit(&intro_wl->light.request,
                        intro_wl->light.service,
                        sizeof(intro_wl->light.data),
                        0);
  }

  return;
}

//Signal the service with the new motor data
void wl_signalMotorService(intro_wallfollowing_node_t* intro_wl, float translation, float rotation) {
  urtDebugAssert(intro_wl != NULL);

  //  Set the motor data
    intro_wl->motor.data.translation.axes[0] = translation;
    intro_wl->motor.data.rotation.vector[2] = rotation;
    // Translation is set over translation.axes[0], Rotation over rotation.vector[2] 
    // in the data of the intro_wl_motor_t struct
  //  Acquire and submit the motor request
  urt_status_t status = urtNrtRequestTryAcquire(&intro_wl->motor.request);
  
  if (status == URT_STATUS_OK) {
    urtNrtRequestSubmit(&intro_wl->motor.request,
                        intro_wl->motor.service,
                        sizeof(intro_wl->motor.data),
                        0);
  } else {
    urtPrintf("Could not acquire urt while signaling motor service! Reason:%i\n", status);
  }
  return;
}

void intro_wl_getData(intro_wallfollowing_node_t* intro_wl, urt_osEventMask_t event) {
  urtDebugAssert(intro_wl != NULL);

  //local variables
  urt_status_t status;

  if (event & PROXRINGEVENT) {
    // fetch NRT of the ring proximity data
    do {
      status = urtNrtSubscriberFetchNext(&intro_wl->ring.nrt,
                                         &intro_wl->ring.data,
                                         NULL, NULL, NULL);
    } while (status != URT_STATUS_FETCH_NOMESSAGE);

    event &= ~PROXRINGEVENT;
  }

  if (event & PROXFLOOREVENT) {
    // fetch NRT of the floor proximity data
    do {
      status = urtNrtSubscriberFetchNext(&intro_wl->floor.nrt,
                                         &intro_wl->floor.data,
                                         NULL, NULL, NULL);
    } while (status != URT_STATUS_FETCH_NOMESSAGE);

    event &= ~PROXFLOOREVENT;
  }

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

void wallfolowing(intro_wallfollowing_node_t* intro_wl){
  uint8_t s_wwn = intro_wl->ring.data.data[1]; 
  uint8_t s_wws = intro_wl->ring.data.data[2];

  float rot = 0.0;
  float trans = 0.05;
  const int target = 10000;

  const float p_val = 0.0007;


  rot = rot + p_val*(+target - s_wwn);
  rot = rot + p_val*(-target + s_wws);
  

  if (intro_wl->ring.data.data[0] > 4000 || intro_wl->ring.data.data[7] > 4000
     || intro_wl->ring.data.data[1] > 4000
  ) {
    trans = 0;
    rot = -2;
  }
  

  wl_signalMotorService(intro_wl,trans,rot);
}

void wl_followLine(intro_wallfollowing_node_t* intro_wl)
{
  // Read val
    // white = 24000
    // black = 3000
    uint16_t l_sensor = intro_wl -> floor.data.data[1];
    l_sensor = 26000 - l_sensor; 
    uint16_t r_sensor = intro_wl -> floor.data.data[2];

    uint16_t diff = r_sensor-l_sensor;
    float p_c1 = 0.0001;

    float turn_speed = p_c1 * diff;
    float speed = 0.05;
    wl_signalMotorService(intro_wl,speed,turn_speed);

  return;
}

void publish_amiro_status(intro_wallfollowing_node_t* intro_wl, uint8_t status)
{
  status_data_t status_data;
  status_data.status = status;
  
  urt_osTime_t timestamp;
  urtTimeNow(&timestamp);

  urtPublisherPublish(&intro_wl->status_publisher,
                      &status_data,
                      sizeof(status_data),
                      &timestamp,
                      URT_PUBLISHER_PUBLISH_LAZY);
}

/**
 * @addtogroup apps_intro
 * @{
 */

/**
 * @brief   Setup callback function for intro_wallfollowing nodes.
 *
 * @param[in] node      Pointer to the node object.
 *                      Must not be NULL.
 * @param[in] intro_wl  Pointer to the intro_wl structure.
 *                      Must not be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _intro_wl_Setup(urt_node_t* node, void* intro_wl)
{
  urtDebugAssert(intro_wl != NULL);
  (void)node;

  // set thread name
  chRegSetThreadName(_intro_wallfollowing_name);

  // local constants
  intro_wallfollowing_node_t* const wl = (intro_wallfollowing_node_t*)intro_wl;

  #if (URT_CFG_PUBSUB_ENABLED == true)
  // subscribe to the ring proximity topic
  urt_topic_t* const ring_prox_topic = urtCoreGetTopic(wl->ring.topicid);
  urtDebugAssert(ring_prox_topic != NULL);
  urtNrtSubscriberSubscribe(&wl->ring.nrt, ring_prox_topic, PROXRINGEVENT);

  // subscribe to the floor proximity topic
  urt_topic_t* const floor_prox_topic = urtCoreGetTopic(wl->floor.topicid);
  urtDebugAssert(floor_prox_topic != NULL);
  urtNrtSubscriberSubscribe(&wl->floor.nrt, floor_prox_topic, PROXFLOOREVENT);
#endif /* URT_CFG_PUBSUB_ENABLED == true */


  return PROXRINGEVENT | PROXFLOOREVENT;
}

/**
 * @brief   Loop callback function for intro_wallfollowing nodes.
 *
 * @param[in] node      Pointer to the node object.
 *                      Must not be NULL.
 * @param[in] event     Received event.
 * @param[in] intro_wl  Pointer to the intro_wl structure.
 *                      Must not be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _intro_wl_Loop(urt_node_t* node, urt_osEventMask_t event, void* intro_wl)
{
  urtDebugAssert(intro_wl != NULL);
  (void)node;

  // local constants
  intro_wallfollowing_node_t* wl = (intro_wallfollowing_node_t*)intro_wl;

  // get the proximity data of the ring sensors
  intro_wl_getData(wl, event);

  switch (wl->state) {
  case WL_IDLE: {
      wl_signalMotorService(wl,0,0);
      if (wl->ring.data.data[2] > 40000) {
      urtPrintf("LINEFOLLOWING \n");
      wl->light.data.color[2] = GREEN;
      intro_wl_signalLightService(wl);
      wl->state = WL_LINEFOLLOWING;
      publish_amiro_status(wl, WL_LINEFOLLOWING);

    } else if (wl->ring.data.data[6] > 40000) {
      urtPrintf("State: WL_PRINTING\n");
      wl->light.data.color[2] = RED;
      intro_wl_signalLightService(wl);

      wl->state = WL_PRINTING;
      publish_amiro_status(wl, WL_PRINTING);
      urtThreadSSleep(2);
    }
    break;
  }
  case WL_WALLFOLLOWING: {
    if (wl->ring.data.data[3] > 40000 && wl->ring.data.data[4] > 40000) {
      wl_signalMotorService(wl,0,0);
      urtPrintf("State: WL_IDLE\n");
      wl->light.data.color[2] = BLUE;
      intro_wl_signalLightService(wl);

      wl->state = WL_IDLE;
      publish_amiro_status(wl, WL_IDLE);
    }

     wallfolowing(wl);
    if (wl->ring.data.data[0] < 15000 && wl->ring.data.data[7] < 15000 
    && ( wl -> floor.data.data[1] < 10000 ||  wl -> floor.data.data[2] < 10000 )) {
      wl->state = WL_LINEFOLLOWING;
      publish_amiro_status(wl, WL_LINEFOLLOWING);
      wl->light.data.color[2] = GREEN;
      intro_wl_signalLightService(wl);
      }
    break;
  }
  case WL_LINEFOLLOWING: {
    if (wl->ring.data.data[3] > 40000 && wl->ring.data.data[4] > 40000) {
      wl_signalMotorService(wl,0,0);
      urtPrintf("State: WL_IDLE\n");
      wl->light.data.color[2] = BLUE;
      intro_wl_signalLightService(wl);
      wl->state = WL_IDLE;
      publish_amiro_status(wl, WL_IDLE);

    }
    
    wl_followLine(wl);

    if (wl->ring.data.data[0] > 4000 || wl->ring.data.data[7] > 4000 
     || wl->ring.data.data[6] > 4000 || wl->ring.data.data[1] > 4000 ) {
      wl->state = WL_WALLFOLLOWING;
      publish_amiro_status(wl, WL_LINEFOLLOWING);

      wl->light.data.color[2] = ORANGE;
      intro_wl_signalLightService(wl);
      }
    break;
  } 
  case WL_PRINTING: {
    

    urtPrintf("Ring Proximity Data:\n");
    for (uint8_t r = 0; r < 8; r++) {
      urtPrintf("%u \t", wl->ring.data.data[r]);
    }
    urtPrintf("\n");
    urtPrintf("Floor Proximity Data:\n");
    for (uint8_t r = 0; r < 4; r++) {
      urtPrintf("%u \t", wl->floor.data.data[r]);
    }
    urtPrintf("\n\n");

    urtThreadSSleep(1); //Sleep a second for better readability

    if (wl->ring.data.data[3] > 40000 && wl->ring.data.data[4] > 40000) {
      urtPrintf("State: WL_IDLE\n");
      wl->light.data.color[2] = BLUE;
      intro_wl_signalLightService(wl);

      wl->state = WL_IDLE;
      publish_amiro_status(wl, WL_IDLE);

    }
    break;
  }
  default: break;
  }

  return PROXRINGEVENT | PROXFLOOREVENT;
}

/**
 * @brief @brief   Intro_WallFollowing shutdown callback.
 *
 * @param[in] node      Execution node of the intro_wl.
 * @param[in] reason    Reason for the shutdown.
 * @param[in] intro_wl  Pointer to the Intro_WallFollowing instance.
 */
void _intro_wl_Shutdown(urt_node_t* node, urt_status_t reason, void* intro_wl)
{
  urtDebugAssert(intro_wl != NULL);
  (void)node;
  (void)reason;

  // local constants
  intro_wallfollowing_node_t* const wl = (intro_wallfollowing_node_t*)intro_wl;

#if (URT_CFG_PUBSUB_ENABLED == true)
  // unsubscribe from topics
  urtNrtSubscriberUnsubscribe(&wl->floor.nrt);
  urtNrtSubscriberUnsubscribe(&wl->ring.nrt);
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

  return;
}


/**
 * @brief   Intro_WallFollowing (intro_wl) initialization function.
 *
 * @param[in] intro_wl               intro_wl object to initialize.
 * @param[in] prio                   Priority of the execution thread.
 * @param[in] serviceID_Light        Service ID of the light service.
 * @param[in] serviceID_Motor        Service ID of the motor service.
 * @param[in] topicID_ringProx       Topic ID to get ring proximity data.
 * @param[in] topicID_floorProx      Topic ID to get floor proximity data.
 */
void intro_wl_Init(intro_wallfollowing_node_t* intro_wl,
                   urt_osThreadPrio_t prio,
                   urt_serviceid_t serviceID_Light,
                   urt_serviceid_t serviceID_Motor,
                   urt_topicid_t topicID_floorProx,
                   urt_topicid_t topicID_ringProx,
                   urt_topicid_t topicID_status)
{
  urtDebugAssert(intro_wl != NULL);

  intro_wl->state = WL_IDLE;
  intro_wl->floor.topicid = topicID_floorProx;
  intro_wl->ring.topicid = topicID_ringProx;
  
  intro_wl->motor.service = urtCoreGetService(serviceID_Motor);
  intro_wl->light.service = urtCoreGetService(serviceID_Light);

  urtNrtSubscriberInit(&intro_wl->floor.nrt);
  urtNrtSubscriberInit(&intro_wl->ring.nrt);

  urtPublisherInit(&intro_wl->status_publisher, urtCoreGetTopic(topicID_status));

  urtNrtRequestInit(&intro_wl->light.request, &intro_wl->light.data);
  // Initialize the motor request
  urtNrtRequestInit(&intro_wl->motor.request, &intro_wl->motor.data);
  // initialize the node
  urtNodeInit(&intro_wl->node, (urt_osThread_t*)intro_wl->thread, sizeof(intro_wl->thread), prio,
              _intro_wl_Setup, intro_wl,
              _intro_wl_Loop, intro_wl,
              _intro_wl_Shutdown, intro_wl);

  //Set Color Of light Ring
  for (int i = 0; i < 12; i++) {
    intro_wl->light.data.color[i]  = GREEN; 
  }
  for (int i = 12; i < 24; i++) {
    intro_wl->light.data.color[i]  = YELLOW;
  } 
  intro_wl_signalLightService(intro_wl); 

  return;
}

/** @} */
