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
 * @file    amiro_touchsensors.c
 */

#include <amiro_touchsensors.h>
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
 * @brief   Name of touchsensors nodes.
 */
static const char _amiro_touchsensors_name[] = "Amiro_TouchSensors";

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   Callback function for the trigger timer.
 *
 * @param[in] flags   Flags to emit for the trigger event.
 *                    Pointer is reinterpreted as integer value.
 */
static void _amiro_touchsensors_triggercb(void* params)
{
  (void)params;

  // broadcast ring event
  chEvtBroadcastFlagsI(&((amiro_touchsensors_node_t*)params)->trigger.source, (urt_osEventFlags_t)0);

  return;
}

/**
 * @brief   Setup callback function for touchsensors nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] touchsensors    Pointer to the touchsensors structure.
 *                    Must nor be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _amiro_touchsensors_Setup(urt_node_t* node, void* touchsensors)
{
  urtDebugAssert(touchsensors != NULL);
  (void)node;

  // set thread name
  chRegSetThreadName(_amiro_touchsensors_name);

  // register trigger event
  urtEventRegister(&((amiro_touchsensors_node_t*)touchsensors)->trigger.source,
                   &((amiro_touchsensors_node_t*)touchsensors)->trigger.listener,
                   TRIGGEREVENT, 0);

  // activate the timer
  aosTimerPeriodicInterval(&((amiro_touchsensors_node_t*)touchsensors)->trigger.timer,
                           (1.0f / ((amiro_touchsensors_node_t*)touchsensors)->driver_data.frequency) * MICROSECONDS_PER_SECOND,
                           _amiro_touchsensors_triggercb, touchsensors);

  return TRIGGEREVENT;
}

/**
 * @brief   Loop callback function for touchsensors nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] touchsensors   Pointer to the touchsensors structure.
 *                    Must not be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _amiro_touchsensors_Loop(urt_node_t* node, urt_osEventMask_t event, void* touchsensors)
{
  urtDebugAssert(touchsensors != NULL);

  (void)node;

  switch(event) {
    case TRIGGEREVENT:
    {
      mpr121_lld_read_register(((amiro_touchsensors_node_t*)touchsensors)->driver_data.mprd,
                               MPR121_LLD_REGISTER_TOUCH_STATUS,
                               0, 1,
                               &((amiro_touchsensors_node_t*)touchsensors)->driver_data.tstate,
                               ((amiro_touchsensors_node_t*)touchsensors)->driver_data.timeout);
      ((amiro_touchsensors_node_t*)touchsensors)->driver_data.touch_data.data[0] = ((amiro_touchsensors_node_t*)touchsensors)->driver_data.tstate & 1;
      ((amiro_touchsensors_node_t*)touchsensors)->driver_data.touch_data.data[1] = (((amiro_touchsensors_node_t*)touchsensors)->driver_data.tstate >> 1) & 1;
      ((amiro_touchsensors_node_t*)touchsensors)->driver_data.touch_data.data[2] = (((amiro_touchsensors_node_t*)touchsensors)->driver_data.tstate >> 2) & 1;
      ((amiro_touchsensors_node_t*)touchsensors)->driver_data.touch_data.data[3] = (((amiro_touchsensors_node_t*)touchsensors)->driver_data.tstate >> 3) & 1;

      urtTimeNow(&((amiro_touchsensors_node_t*)touchsensors)->publish_time);
      // publish the touch sensor data
      urtPublisherPublish(&((amiro_touchsensors_node_t*)touchsensors)->publisher,
                        &((amiro_touchsensors_node_t*)touchsensors)->driver_data.touch_data,
                        sizeof(((amiro_touchsensors_node_t*)touchsensors)->driver_data.touch_data),
                        &((amiro_touchsensors_node_t*)touchsensors)->publish_time,
                        URT_PUBLISHER_PUBLISH_LAZY);
      break;
    }
    default: break;
  }

  return TRIGGEREVENT;
}

/**
 * @brief   Shutdown callback function for touchsensors nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] reason  Reason for the termination of the node.
 * @param[in] touchsensors   Pointer to the touchsensors structure.
 *                           Must nor be NULL.
 */
void _amiro_touchsensors_Shutdown(urt_node_t* node, urt_status_t reason, void* touchsensors)
{
  urtDebugAssert(touchsensors != NULL);

  (void)node;
  (void)reason;

  // unregister trigger event
  urtEventUnregister(&((amiro_touchsensors_node_t*)touchsensors)->trigger.source,
                     &((amiro_touchsensors_node_t*)touchsensors)->trigger.listener);

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_amiro_touchsensors
 * @{
 */

void amiroTouchSensorsInit(amiro_touchsensors_node_t* touchsensors, urt_topicid_t touch_topicid, urt_osThreadPrio_t prio)
{
  urtDebugAssert(touchsensors != NULL);

  // set/initialize event data
  urtEventSourceInit(&touchsensors->trigger.source);
  urtEventListenerInit(&touchsensors->trigger.listener);
  // initialize the timer
  urtTimerInit(&touchsensors->trigger.timer);
#if (URT_CFG_PUBSUB_ENABLED == true)
  // initialize the publisher for the touch sensor data
  urtPublisherInit(&touchsensors->publisher, urtCoreGetTopic(touch_topicid));
#endif /* URT_CFG_PUBSUB_ENABLED == true */

  touchsensors->driver_data.mprd = &moduleLldTouch;
  touchsensors->driver_data.timeout = MICROSECONDS_PER_SECOND;

  // initialize the node
  urtNodeInit(&touchsensors->node, (urt_osThread_t*)touchsensors->thread, sizeof(touchsensors->thread), prio,
              _amiro_touchsensors_Setup, touchsensors,
              _amiro_touchsensors_Loop, touchsensors,
              _amiro_touchsensors_Shutdown, touchsensors);

  return;
}

/** @} */
