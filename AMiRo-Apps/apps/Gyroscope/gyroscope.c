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
 * @file    gyroscope.c
 */

#include <gyroscope.h>
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
 * @brief   Name of gyroscope nodes.
 */
static const char _gyro_name[] = "Gyroscope";

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   Callback function for the trigger timer.
 *
 * @param[in] flags   Flags to emit for the trigger event.
 *                    Pointer is reinterpreted as integer value.
 */
static void _gyro_triggercb(virtual_timer_t* timer, void* params)
{
  (void)timer;
  (void)params;

  // broadcast ring event
  chEvtBroadcastFlagsI(&((gyro_node_t*)params)->trigger.source, (urt_osEventFlags_t)0);

  return;
}

/**
 * @brief   Setup callback function for gyroscope nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] gyro    Pointer to the gyroscope structure.
 *                    Must nor be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _gyro_Setup(urt_node_t* node, void* gyro)
{
  urtDebugAssert(gyro != NULL);
  (void)node;

  // set thread name
  chRegSetThreadName(_gyro_name);

  // start the spi
  spiStart(((gyro_node_t*)gyro)->driver_data.driver->spid, ((gyro_node_t*)gyro)->driver_data.spiconf);

  // set the configuration
  ((gyro_node_t*)gyro)->driver_data.cfg.registers.ctrl_reg1 = L3G4200D_LLD_PD | L3G4200D_LLD_DR_100_HZ | L3G4200D_LLD_BW_12_5 | L3G4200D_LLD_ZEN | L3G4200D_LLD_YEN | L3G4200D_LLD_XEN;

  // write the configuration
  l3g4200d_lld_write_config(((gyro_node_t*)gyro)->driver_data.driver, ((gyro_node_t*)gyro)->driver_data.cfg);

  // register trigger event
  urtEventRegister(&((gyro_node_t*)gyro)->trigger.source, &((gyro_node_t*)gyro)->trigger.listener, TRIGGEREVENT, 0);

  // activate the timer
  aosTimerPeriodicInterval(&((gyro_node_t*)gyro)->trigger.timer, (1.0f / ((gyro_node_t*)gyro)->driver_data.frequency) * MICROSECONDS_PER_SECOND, _gyro_triggercb, gyro);

  return TRIGGEREVENT;
}

/**
 * @brief   Loop callback function for gyroscope nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] gyro   Pointer to the gyroscope structure.
 *                    Must not be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _gyro_Loop(urt_node_t* node, urt_osEventMask_t event, void* gyro)
{
  urtDebugAssert(gyro != NULL);

  (void)node;

  switch(event) {
    case TRIGGEREVENT:
    {
      l3g4200d_lld_read_all_data(((gyro_node_t*)gyro)->driver_data.driver,
                                 ((gyro_node_t*)gyro)->driver_data.data.values.data,
                                 &((gyro_node_t*)gyro)->driver_data.cfg);

      urtTimeNow(&((gyro_node_t*)gyro)->time);
      // publish gyroscope data
      urtPublisherPublish(&((gyro_node_t*)gyro)->publisher,
                          &((gyro_node_t*)gyro)->driver_data.data,
                          sizeof(((gyro_node_t*)gyro)->driver_data.data),
                          &((gyro_node_t*)gyro)->time,
                          URT_PUBLISHER_PUBLISH_ENFORCING);

      break;
    }
    default: break;
  }

  return TRIGGEREVENT;
}

/**
 * @brief   Shutdown callback function for gyroscope nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] gyro  Pointer to the gyroscope structure.
 *                    Must nor be NULL.
 */
void _gyro_Shutdown(urt_node_t* node, urt_status_t reason, void* gyro)
{
  urtDebugAssert(gyro != NULL);

  (void)node;
  (void)reason;

  // unregister trigger event
  urtEventUnregister(&((gyro_node_t*)gyro)->trigger.source, &((gyro_node_t*)gyro)->trigger.listener);

  // stop the spi
  spiStop(((gyro_node_t*)gyro)->driver_data.driver->spid);

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_gyroscope
 * @{
 */

void gyroInit(gyro_node_t* gyro, urt_topicid_t topicid, urt_osThreadPrio_t prio, float frequency)
{
  urtDebugAssert(gyro != NULL);

  // set/initialize event data
  urtEventSourceInit(&gyro->trigger.source);
  urtEventListenerInit(&gyro->trigger.listener);
  // initialize the timer
  aosTimerInit(&gyro->trigger.timer);
  // set the topicid
  gyro->topicid = topicid;
#if (URT_CFG_PUBSUB_ENABLED == true)
  // initialize the publisher for the gyro data
  urtPublisherInit(&gyro->publisher, urtCoreGetTopic(topicid));
#endif /* URT_CFG_PUBSUB_ENABLED == true */

  // set the driver
  gyro->driver_data.driver = &moduleLldGyroscope;
  // set the spi configuration
  gyro->driver_data.spiconf = &moduleHalSpiGyroscopeConfig;
  // set the frequency
  gyro->driver_data.frequency = frequency;

  // initialize the node
  urtNodeInit(&gyro->node, (urt_osThread_t*)gyro->thread, sizeof(gyro->thread), prio,
              _gyro_Setup, gyro,
              _gyro_Loop, gyro,
              _gyro_Shutdown, gyro);


  return;
}

/** @} */
