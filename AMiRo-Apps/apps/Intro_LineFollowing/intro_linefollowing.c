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
 * @file    intro_linefollowing.c
 */

#include <intro_linefollowing.h>
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
 * @brief   Name of intro_linefollowing nodes.
 */
static const char _intro_linefollowing_name[] = "Intro_LF";

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

//Signal the light service with the new colors for each LED
void signalLightService(intro_linefollowing_node_t* intro_lf) {
  urtDebugAssert(intro_lf != NULL);
  
  //Try to acquire the light service
  if (urtNrtRequestTryAcquire(&intro_lf->light.request) == URT_STATUS_OK) {
    //Signal the light service
    urtNrtRequestSubmit(&intro_lf->light.request,
                        intro_lf->light.service,
                        sizeof(intro_lf->light.data),
                        0);
  }

  return;
}

//Signal the service with the new motor data
void signalMotorService(intro_linefollowing_node_t* intro_lf, float translation, float rotation) {
  urtDebugAssert(intro_lf != NULL);

  //TODO: Set the motor data
    intro_lf->motor.data.translation.axes[0] = translation;
    intro_lf->motor.data.rotation.vector[2] = rotation;
    // Translation is set over translation.axes[0], Rotation over rotation.vector[2] 
    // in the data of the intro_lf_motor_t struct
  //TODO: Acquire and submit the motor request
  urt_status_t status = urtNrtRequestTryAcquire(&intro_lf->motor.request);
  
  if (status == URT_STATUS_OK) {
    urtNrtRequestSubmit(&intro_lf->motor.request,
                        intro_lf->motor.service,
                        sizeof(intro_lf->motor.data),
                        0);
  } else {
    urtPrintf("Could not acquire urt while signaling motor service! Reason:%i\n", status);
  }
  return;
}

void getData(intro_linefollowing_node_t* intro_lf, urt_osEventMask_t event) {
  urtDebugAssert(intro_lf != NULL);

  //local variables
  urt_status_t status;

  if (event & PROXRINGEVENT) {
    // fetch NRT of the ring proximity data
    do {
      status = urtNrtSubscriberFetchNext(&intro_lf->ring.nrt,
                                         &intro_lf->ring.data,
                                         NULL, NULL, NULL);
    } while (status != URT_STATUS_FETCH_NOMESSAGE);

    event &= ~PROXRINGEVENT;
  }

  if (event & PROXFLOOREVENT) {
    // fetch NRT of the floor proximity data
    do {
      status = urtNrtSubscriberFetchNext(&intro_lf->floor.nrt,
                                         &intro_lf->floor.data,
                                         NULL, NULL, NULL);
    } while (status != URT_STATUS_FETCH_NOMESSAGE);

    event &= ~PROXFLOOREVENT;
  }

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

void followLine(intro_linefollowing_node_t* intro_lf)
{
  // Read val
    // whit = 24000
    // black = 3000
    auto l_sensor = intro_lf -> floor.data.data[1];
    l_sensor = 26000 - l_sensor; 
    auto r_sensor = intro_lf -> floor.data.data[2];

    auto diff = r_sensor-l_sensor;
    float p_c1 = 0.0001;

    float turn_speed = p_c1 * diff;
    float speed = 0.1;
    signalMotorService(intro_lf,speed,turn_speed);

  return;
}

/**
 * @addtogroup apps_intro
 * @{
 */

/**
 * @brief   Setup callback function for intro_linefollowing nodes.
 *
 * @param[in] node      Pointer to the node object.
 *                      Must not be NULL.
 * @param[in] intro_lf  Pointer to the intro_lf structure.
 *                      Must not be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _intro_lf_Setup(urt_node_t* node, void* intro_lf)
{
  urtDebugAssert(intro_lf != NULL);
  (void)node;

  // set thread name
  chRegSetThreadName(_intro_linefollowing_name);

  // local constants
  intro_linefollowing_node_t* const lf = (intro_linefollowing_node_t*)intro_lf;

  #if (URT_CFG_PUBSUB_ENABLED == true)
  // subscribe to the floor proximity topic
  urt_topic_t* const floor_prox_topic = urtCoreGetTopic(lf->floor.topicid);
  urtDebugAssert(floor_prox_topic != NULL);
  urtNrtSubscriberSubscribe(&lf->floor.nrt, floor_prox_topic, PROXFLOOREVENT);

  // subscribe to the ring proximity topic
  urt_topic_t* const ring_prox_topic = urtCoreGetTopic(lf->ring.topicid);
  urtDebugAssert(ring_prox_topic != NULL);
  urtNrtSubscriberSubscribe(&lf->ring.nrt, ring_prox_topic, PROXRINGEVENT);
#endif /* URT_CFG_PUBSUB_ENABLED == true */


  return PROXRINGEVENT | PROXFLOOREVENT;
}

/**
 * @brief   Loop callback function for intro_linefollowing nodes.
 *
 * @param[in] node      Pointer to the node object.
 *                      Must not be NULL.
 * @param[in] event     Received event.
 * @param[in] intro_lf  Pointer to the intro_lf structure.
 *                      Must not be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _intro_lf_Loop(urt_node_t* node, urt_osEventMask_t event, void* intro_lf)
{
  urtDebugAssert(intro_lf != NULL);
  (void)node;

  // local constants
  intro_linefollowing_node_t* const lf = (intro_linefollowing_node_t*)intro_lf;

  // get the proximity data of the ring and floor sensors 
  getData(lf, event);

  switch (lf->state) {
  case IDLE: {
    /*
    if (lf->ring.data.data[6] > 40000) {
      urtPrintf("State: PRINTING\n");
      lf->state = PRINTING;
    } else if (lf->ring.data.data[4] > 40000) {
      urtPrintf("State: LIGHT\n");
      lf->state = LIGHT;
    } else if (lf->ring.data.data[2] > 40000) {
      urtPrintf("LINEFOLLOWING \n");
      lf->state = LINEFOLLOWING;
    }
    */
    break;
  }
  case LIGHT: {

    if (lf->ring.data.data[0] > 40000 && lf->ring.data.data[7] > 40000) {
      for (uint8_t l = l; l < NUM_LEDS; l++) {
        lf->light.data.color[l] = OFF;
      }
      urtPrintf("State: IDLE\n");
      lf->state = IDLE;
    } else {
      for (uint8_t l = 0; l < NUM_LEDS; l++) {
        lf->light.data.color[l] = YELLOW;
      }
      for (uint8_t r = 0; r < NUM_RING_SENSORS; r++) {
        if (lf->ring.data.data[r] > 40000) {
          lf->light.data.color[r*3] = TURQUOISE;
          lf->light.data.color[(r*3)+1] = TURQUOISE;
          lf->light.data.color[(r*3)+2] = TURQUOISE;
        }
      }
    }  
    
    signalLightService(lf);  
    break;
  }
  case PRINTING: {
    urtPrintf("Floor Proximity Data:\n");
    for (uint8_t f = 0; f < 4; f++) {
      urtPrintf("%u \t", lf->floor.data.data[f]);
    }
    urtPrintf("\n \n");
    urtPrintf("Ring Proximity Data:\n");
    for (uint8_t r = 0; r < 8; r++) {
      urtPrintf("%u \t", lf->ring.data.data[r]);
    }
    urtPrintf("\n\n");

    urtThreadSSleep(1); //Sleep a second for better readability

    if (lf->ring.data.data[0] > 40000 && lf->ring.data.data[7] > 40000) {
      urtPrintf("State: IDLE\n");
      lf->state = IDLE;
    }
    break;
  }
  case LINEFOLLOWING: {
    followLine(lf);

    if (lf->ring.data.data[0] > 40000 && lf->ring.data.data[7] > 40000) {
      signalMotorService(lf,0,0);
      urtPrintf("State: IDLE\n");
      lf->state = IDLE;
    }
    break;
  }
  default: break;
  }

  return PROXRINGEVENT | PROXFLOOREVENT;
}

/**
 * @brief @brief   Intro_LineFollowing shutdown callback.
 *
 * @param[in] node      Execution node of the intro_lf.
 * @param[in] reason    Reason for the shutdown.
 * @param[in] intro_lf  Pointer to the Intro_LineFollowing instance.
 */
void _intro_lf_Shutdown(urt_node_t* node, urt_status_t reason, void* intro_lf)
{
  urtDebugAssert(intro_lf != NULL);
  (void)node;
  (void)reason;

  // local constants
  intro_linefollowing_node_t* const lf = (intro_linefollowing_node_t*)intro_lf;

#if (URT_CFG_PUBSUB_ENABLED == true)
  // unsubscribe from topics
  urtNrtSubscriberUnsubscribe(&lf->floor.nrt);
  urtNrtSubscriberUnsubscribe(&lf->ring.nrt);
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

  return;
}


/**
 * @brief   Intro_LineFollowing (intro_lf) initialization function.
 *
 * @param[in] intro_lf               intro_lf object to initialize.
 * @param[in] prio                   Priority of the execution thread.
 * @param[in] serviceID_Light        Service ID of the light service.
 * @param[in] serviceID_Motor        Service ID of the motor service.
 * @param[in] topicID_floorProx      Topic ID to get floor proximity data.
 * @param[in] topicID_ringProx       Topic ID to get ring proximity data.
 */
void intro_lf_Init(intro_linefollowing_node_t* intro_lf, 
                   urt_osThreadPrio_t prio,
                   urt_serviceid_t serviceID_Light,
                   urt_serviceid_t serviceID_Motor,
                   urt_topicid_t topicID_floorProx,
                   urt_topicid_t topicID_ringProx)
{
  urtDebugAssert(intro_lf != NULL);

  intro_lf->state = IDLE;
  intro_lf->floor.topicid = topicID_floorProx;
  intro_lf->ring.topicid = topicID_ringProx;
  intro_lf->motor.service = urtCoreGetService(serviceID_Motor);
  intro_lf->light.service = urtCoreGetService(serviceID_Light);

  urtNrtSubscriberInit(&intro_lf->floor.nrt);
  urtNrtSubscriberInit(&intro_lf->ring.nrt);
  urtNrtRequestInit(&intro_lf->light.request, &intro_lf->light.data);
  //TODO: Initialize the motor request
  urtNrtRequestInit(&intro_lf->motor.request, &intro_lf->motor.data);
  // initialize the node
  urtNodeInit(&intro_lf->node, (urt_osThread_t*)intro_lf->thread, sizeof(intro_lf->thread), prio,
              _intro_lf_Setup, intro_lf,
              _intro_lf_Loop, intro_lf,
              _intro_lf_Shutdown, intro_lf);

  return;
}

/** @} */
