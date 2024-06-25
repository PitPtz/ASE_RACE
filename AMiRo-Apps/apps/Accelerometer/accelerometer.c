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
 * @file    accelerometer.c
 */

#include <accelerometer.h>
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
 * @brief   Name of accelerometer nodes.
 */
static const char _acco_name[] = "Accelerometer";

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   Callback function for the trigger timer.
 *
 * @param[in] flags   Flags to emit for the trigger event.
 *                    Pointer is reinterpreted as integer value.
 */
static void _acco_triggercb(virtual_timer_t* timer, void* params)
{
  (void)timer;
  (void)params;

  // broadcast ring event
  chEvtBroadcastFlagsI(&((acco_node_t*)params)->trigger.source, (urt_osEventFlags_t)0);

  return;
}

/**
 * @brief   Setup callback function for accelerometer nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] acco    Pointer to the accelerometer structure.
 *                    Must nor be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _acco_Setup(urt_node_t* node, void* acco)
{
  urtDebugAssert(acco != NULL);
  (void)node;

  // set thread name
  chRegSetThreadName(_acco_name);

  // start the spi
  spiStart(((acco_node_t*)acco)->driver_data.driver->spid, ((acco_node_t*)acco)->driver_data.spiconf);

  // set the configuration
  ((acco_node_t*)acco)->driver_data.cfg.registers.ctrl_reg1 = LIS331DLH_LLD_PM_ODR |
                                                              LIS331DLH_LLD_DR_1000HZ_780LP |
                                                              LIS331DLH_LLD_X_AXIS_ENABLE |
                                                              LIS331DLH_LLD_Y_AXIS_ENABLE |
                                                              LIS331DLH_LLD_Z_AXIS_ENABLE;
  ((acco_node_t*)acco)->driver_data.cfg.registers.ctrl_reg3 = 0x00;
  // write the configuration
  lis331dlh_lld_write_config(((acco_node_t*)acco)->driver_data.driver, &((acco_node_t*)acco)->driver_data.cfg);

  // register trigger event
  urtEventRegister(&((acco_node_t*)acco)->trigger.source, &((acco_node_t*)acco)->trigger.listener, TRIGGEREVENT, 0);

  // activate the timer
  aosTimerPeriodicInterval(&((acco_node_t*)acco)->trigger.timer, (1.0f / ((acco_node_t*)acco)->driver_data.frequency) * MICROSECONDS_PER_SECOND, _acco_triggercb, acco);

  return TRIGGEREVENT;
}

/**
 * @brief   Loop callback function for accelerometer nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] acco   Pointer to the accelerometer structure.
 *                    Must not be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _acco_Loop(urt_node_t* node, urt_osEventMask_t event, void* acco)
{
  urtDebugAssert(acco != NULL);

  (void)node;

  switch(event) {
    case TRIGGEREVENT:
    {

      lis331dlh_lld_read_all_data(((acco_node_t*)acco)->driver_data.driver,
                                  ((acco_node_t*)acco)->driver_data.data.data,
                                  &((acco_node_t*)acco)->driver_data.cfg);

      urtTimeNow(&((acco_node_t*)acco)->time);
      // publish ambient data
      urtPublisherPublish(&((acco_node_t*)acco)->publisher,
                          &((acco_node_t*)acco)->driver_data.data,
                          sizeof(((acco_node_t*)acco)->driver_data.data),
                          &((acco_node_t*)acco)->time,
                          URT_PUBLISHER_PUBLISH_ENFORCING);
      break;
    }
    default: break;
  }

  return TRIGGEREVENT;
}

/**
 * @brief   Shutdown callback function for accelerometer nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] acco  Pointer to the accelerometer structure.
 *                    Must nor be NULL.
 */
void _acco_Shutdown(urt_node_t* node, urt_status_t reason, void* acco)
{
  urtDebugAssert(acco != NULL);

  (void)node;
  (void)reason;

  // unregister trigger event
  urtEventUnregister(&((acco_node_t*)acco)->trigger.source, &((acco_node_t*)acco)->trigger.listener);

  // stop the spi
  spiStop(((acco_node_t*)acco)->driver_data.driver->spid);

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_accelerometer
 * @{
 */

void accoInit(acco_node_t* acco, urt_topicid_t topicid, urt_osThreadPrio_t prio, float frequency)
{
  urtDebugAssert(acco != NULL);

  // set/initialize event data
  urtEventSourceInit(&acco->trigger.source);
  urtEventListenerInit(&acco->trigger.listener);
  // initialize the timer
  aosTimerInit(&acco->trigger.timer);
  // set the topicid
  acco->topicid = topicid;
#if (URT_CFG_PUBSUB_ENABLED == true)
  // initialize the publisher for the acco data
  urtPublisherInit(&acco->publisher, urtCoreGetTopic(topicid));
#endif /* URT_CFG_PUBSUB_ENABLED == true */

  // set the driver
  acco->driver_data.driver = &moduleLldAccelerometer;
  // set the spi configuration
  acco->driver_data.spiconf = &moduleHalSpiAccelerometerConfig;
  // set the frequency
  acco->driver_data.frequency = frequency;

  // initialize the node
  urtNodeInit(&acco->node, (urt_osThread_t*)acco->thread, sizeof(acco->thread), prio,
              _acco_Setup, acco,
              _acco_Loop, acco,
              _acco_Shutdown, acco);


  return;
}

/** @} */
