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
 * @file    compass.c
 */

#include <compass.h>
#include <amiroos.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

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
 * @brief   Name of compass nodes.
 */
static const char _compass_name[] = "Compass";

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   Callback function for the trigger timer.
 *
 * @param[in] flags   Flags to emit for the trigger event.
 *                    Pointer is reinterpreted as integer value.
 */
static void _compass_triggercb(virtual_timer_t* timer, void* params)
{
  (void)timer;
  (void)params;

  // broadcast ring event
  chEvtBroadcastFlagsI(&((compass_node_t*)params)->trigger.source, (urt_osEventFlags_t)0);

  return;
}

/**
 * @brief   Setup callback function for compass nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] compass    Pointer to the compass structure.
 *                    Must nor be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _compass_Setup(urt_node_t* node, void* compass)
{
  urtDebugAssert(compass != NULL);
  (void)node;

  // set thread name
  chRegSetThreadName(_compass_name);

  // set the configuration
  ((compass_node_t*)compass)->driver_data.cfg.avg = HMC5883L_LLD_AVG1;
  ((compass_node_t*)compass)->driver_data.cfg.outrate = HMC5883L_LLD_75_HZ;
  ((compass_node_t*)compass)->driver_data.cfg.mbias = HMC5883L_LLD_MB_NORMAL;
  ((compass_node_t*)compass)->driver_data.cfg.gain = HMC5883L_LLD_GN_0_GA;
  ((compass_node_t*)compass)->driver_data.cfg.highspeed = HMC5883L_LLD_HS_DISABLE;
  ((compass_node_t*)compass)->driver_data.cfg.mode = HMC5883L_LLD_MM_CONTINUOUS;
  // write the configuration
  hmc5883l_lld_write_config(((compass_node_t*)compass)->driver_data.driver,
                            ((compass_node_t*)compass)->driver_data.cfg,
                            ((compass_node_t*)compass)->driver_data.timeout);

  // register trigger event
  urtEventRegister(&((compass_node_t*)compass)->trigger.source, &((compass_node_t*)compass)->trigger.listener, TRIGGEREVENT, 0);

  // activate the timer
  aosTimerPeriodicInterval(&((compass_node_t*)compass)->trigger.timer, (1.0f / ((compass_node_t*)compass)->driver_data.frequency) * MICROSECONDS_PER_SECOND, _compass_triggercb, compass);

  return TRIGGEREVENT;
}

/**
 * @brief   Loop callback function for compass nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] compass   Pointer to the compass structure.
 *                    Must not be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _compass_Loop(urt_node_t* node, urt_osEventMask_t event, void* compass)
{
  urtDebugAssert(compass != NULL);

  (void)node;

  switch(event) {
    case TRIGGEREVENT:
    {

      hmc5883l_lld_read_data(((compass_node_t*)compass)->driver_data.driver,
                                  ((compass_node_t*)compass)->driver_data.data.data,
                                  ((compass_node_t*)compass)->driver_data.timeout);

      urtTimeNow(&((compass_node_t*)compass)->time);
      // publish compass data
      urtPublisherPublish(&((compass_node_t*)compass)->publisher,
                          &((compass_node_t*)compass)->driver_data.data,
                          sizeof(((compass_node_t*)compass)->driver_data.data),
                          &((compass_node_t*)compass)->time,
                          URT_PUBLISHER_PUBLISH_ENFORCING);
      break;
    }
    default: break;
  }

  return TRIGGEREVENT;
}

/**
 * @brief   Shutdown callback function for compass nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] compass  Pointer to the compass structure.
 *                    Must nor be NULL.
 */
void _compass_Shutdown(urt_node_t* node, urt_status_t reason, void* compass)
{
  urtDebugAssert(compass != NULL);

  (void)node;
  (void)reason;

  // unregister trigger event
  urtEventUnregister(&((compass_node_t*)compass)->trigger.source, &((compass_node_t*)compass)->trigger.listener);

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_compass
 * @{
 */


void compassInit(compass_node_t* compass, urt_topicid_t topicid, urt_osThreadPrio_t prio, float frequency)
{
  urtDebugAssert(compass != NULL);

  // set/initialize event data
  urtEventSourceInit(&compass->trigger.source);
  urtEventListenerInit(&compass->trigger.listener);
  // initialize the timer
  aosTimerInit(&compass->trigger.timer);
  // set the topicid
  compass->topicid = topicid;
#if (URT_CFG_PUBSUB_ENABLED == true)
  // initialize the publisher for the compass data
  urtPublisherInit(&compass->publisher, urtCoreGetTopic(topicid));
#endif /* URT_CFG_PUBSUB_ENABLED == true */

  // set the driver
  compass->driver_data.driver = &moduleLldCompass;
  // set the frequency
  compass->driver_data.frequency = frequency;
  // set the timeout
  compass->driver_data.timeout = MICROSECONDS_PER_SECOND;

  // initialize the node
  urtNodeInit(&compass->node, (urt_osThread_t*)compass->thread, sizeof(compass->thread), prio,
              _compass_Setup, compass,
              _compass_Loop, compass,
              _compass_Shutdown, compass);


  return;
}

/** @} */
