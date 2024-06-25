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
 * @file    odometry.c
 */

#include <math.h>
#include <odometry.h>
#include <amiroos.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

#define CIRCLE_180  3.14

/**
 * @brief   Event mask to set on a trigger event.
 */
#define ODOMEVENT                 (urtCoreGetEventMask() << 1)

/**
 * @brief   Event mask to set on a service event.
 */
#define ODOMETRY_SERVICEEVENT     (urtCoreGetEventMask() << 2)

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
 * @brief   Name of odometry nodes.
 */
static const char _odometry_name[] = "Odometry";

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   Setup callback function for odometry nodes.
 *
 * @param[in] node        Pointer to the node object.
 *                        Must not be NULL.
 * @param[in] odometry    Pointer to the odometry structure.
 *                        Must not be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _odometrySetup(urt_node_t* node, void* odometry)
{
  urtDebugAssert(odometry != NULL);
  (void)node;

  // set thread name
  chRegSetThreadName(_odometry_name);

  // local constants
  odometry_node_t* const odom = (odometry_node_t*)odometry;

  // subscribe to the motion topic
  {
    // kind of hack to save memory
    urt_topic_t* const motor_topic = odom->nrt.topic;
    odom->nrt.topic = NULL;
    urtNrtSubscriberSubscribe(&odom->nrt, motor_topic, ODOMEVENT);
  }

  odom->odomData.location.axes[0] = 0; // set the x axis to 0
  odom->odomData.location.axes[1] = 0; // set the y axis to 0
  odom->odomData.location.axes[2] = 0.1;  //0.1 because AMiRos high is 10 centimetre
  odom->odomData.orientation.angle = 0; // set the orientation angle to 0

  urtTimeNow(&odom->lastTime);

  return ODOMEVENT | ODOMETRY_SERVICEEVENT;
}

/**
 * @brief   Loop callback function for odometry nodes.
 *
 * @param[in] node        Pointer to the node object.
 *                        Must not be NULL.
 * @param[in] event       Event mask, specifying the event that triggered loop execution.
 * @param[in] odometry    Pointer to the odometry structure.
 *                        Must not be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _odometryLoop(urt_node_t* node, urt_osEventMask_t event, void* odometry)
{
  urtDebugAssert(odometry != NULL);
  (void)node;

  // local constants
  odometry_node_t* const odom = (odometry_node_t*)odometry;

  if (event & ODOMETRY_SERVICEEVENT) {
    // local variables
    struct position_cv_ccsi resetPosition;

    // dispatch request
    urt_service_dispatched_t dispatched;
    urtServiceDispatch(&odom->service, &dispatched, &resetPosition, NULL, NULL);
    if (dispatched.request != NULL) {
      // reset the odometry to the given values
      odom->odomData = resetPosition;                                              

      // try to acquire and respond the request
      if (urtServiceAcquireRequest(&odom->service, &dispatched) == URT_STATUS_OK) {
        urtServiceRespond(&dispatched, 0);
      }
    }
    event &= ~ODOMETRY_SERVICEEVENT;
  }

  if (event & ODOMEVENT) {
    // local variables
    float vx, vy, vth, dt;
    urt_osTime_t currentTime;

    // fetch latest motion message
    if (urtNrtSubscriberFetchLatest(&odom->nrt,
                                    &odom->motionData,
                                    NULL,
                                    &currentTime, NULL) == URT_STATUS_OK) {

      vx = odom->motionData.translation.axes[0];
      vy = odom->motionData.translation.axes[1];
      vth = odom->motionData.rotation.vector[2];

      //check if the motion data changed -> If broken
      //if (vx != odom->old_motionData.translation.axes[0] ||
      //    vy != odom->old_motionData.translation.axes[1] ||
      //    vth != odom->old_motionData.rotation.vector[2]) {

        odom->old_motionData = odom->motionData;

        //Calculate the time difference in seconds because vx,vy,vth are per second
        dt = ((float)(currentTime.time - (odom->lastTime).time))/((float)MICROSECONDS_PER_SECOND);
        odom->lastTime = currentTime;

        odom->odomData.location.axes[0] += (vx * cosf(odom->odomData.orientation.angle)
                                                      - vy * sinf(odom->odomData.orientation.angle)) * dt;
        odom->odomData.location.axes[1] += (vx * sinf(odom->odomData.orientation.angle)
                                                      - vy * cosf(odom->odomData.orientation.angle)) * dt;
        odom->odomData.orientation.angle += vth * dt;

        if (odom->odomData.orientation.angle > CIRCLE_180) {
          odom->odomData.orientation.angle = (-2)*CIRCLE_180 + odom->odomData.orientation.angle;
        } else if (odom->odomData.orientation.angle < ((-1)*CIRCLE_180)) {
          odom->odomData.orientation.angle = 2*CIRCLE_180 + odom->odomData.orientation.angle;
        }

        // publish the odometry data
        urtPublisherPublish(&odom->odom_publisher,
                            &odom->odomData,
                            sizeof(odom->odomData),
                            &odom->lastTime,
                            URT_PUBLISHER_PUBLISH_LAZY);

      //}
    }

    event &= ~ODOMEVENT;
  }

  return ODOMEVENT | ODOMETRY_SERVICEEVENT;
}

/**
 * @brief   Shutdown callback function for odometry nodes.
 *
 * @param[in] node        Pointer to the node object.
 *                        Must not be NULL.
 * @param[in] reason      Reason for the termination of the node.
 * @param[in] odometry    Pointer to the odometry structure.
 *                        Must nor be NULL.
 */
void _odometryShutdown(urt_node_t* node, urt_status_t reason, void* odometry)
{
  urtDebugAssert(odometry != NULL);

  (void)node;
  (void)reason;

  // Unsubscribe from motion topic
  urtNrtSubscriberUnsubscribe(&((odometry_node_t*)odometry)->nrt);

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_odometry
 * @{
 */

void odometryInit(odometry_node_t* odom, urt_osThreadPrio_t prio, urt_topicid_t motion_topicid, urt_topicid_t odom_topicid, urt_serviceid_t reset_serviceid)
{
#if (URT_CFG_PUBSUB_ENABLED == true)
  // initialize the motion subscriber
  urtNrtSubscriberInit(&odom->nrt);
  odom->nrt.topic = urtCoreGetTopic(motion_topicid); // kind of hack to save memory

  // initialize the odometry publisher
  urtPublisherInit(&odom->odom_publisher, urtCoreGetTopic(odom_topicid));
#endif /* URT_CFG_PUBSUB_ENABLED == true */


  // initialize the node
  urtNodeInit(&odom->node, (urt_osThread_t*)odom->thread, sizeof(odom->thread), prio,
              _odometrySetup, odom,
              _odometryLoop, odom,
              _odometryShutdown, odom);

#if (URT_CFG_RPC_ENABLED == true) 
  // Initialize the service to reset the odometry values
  urtServiceInit(&odom->service, reset_serviceid, odom->node.thread, ODOMETRY_SERVICEEVENT);
#endif /* (URT_CFG_RPC_ENABLED == true) */

  return;
}

/** @} */
