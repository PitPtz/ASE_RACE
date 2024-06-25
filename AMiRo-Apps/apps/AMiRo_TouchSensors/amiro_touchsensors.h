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
 * @file    amiro_touchsensors.h
 *
 * @defgroup apps_amiro_touchsensors Tocuh Sensors
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_amiro_touchsensors
 * @{
 */

#ifndef AMIRO_TOUCHSENSORS_H
#define AMIRO_TOUCHSENSORS_H

#include <urt.h>
#include "../../messagetypes/TouchSensordata.h"

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#if !defined(AMIRO_TOUCHSENSORS_STACKSIZE) || defined(__DOXYGEN__)
/**
 * @brief   Stack size of touchsensors threads.
 */
#define AMIRO_TOUCHSENSORS_STACKSIZE             256
#endif /* !defined(AMIRO_TOUCHSENSORS_STACKSIZE) */

/**
 * @brief   Event flag to identify trigger events related to publish-subscribe.
 */
#define TRIGGERFLAG_TOUCHSENSORS           (urt_osEventFlags_t)(1 << 0)

/**
 * @brief   Event mask to set on a trigger event.
 */
#define TRIGGEREVENT                (urt_osEventMask_t)(1<< 1)

#define INTERRUPT_EVENT_ID    1

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

/**
 * @brief   TouchSensors node.
 * @struct  amiro_touchsensors_node
 */
typedef struct amiro_touchsensors_node {
  /**
   * @brief   Thread memory.
   */
  URT_THREAD_MEMORY(thread, AMIRO_TOUCHSENSORS_STACKSIZE);

  /**
   * @brief   Node object.
   */
  urt_node_t node;

  /**
   * @brief Time of the published data
   */
  urt_osTime_t publish_time;

  /**
   * @brief   Driver related data.
   */
  struct {
    /**
     * @brief   Pointer to the MPR121 driver to use.
     */
    MPR121Driver* mprd;

    /**
     * @brief timeout of the functions
     */
    apalTime_t timeout;

    /**
     * @brief node frequency in Hz.
     */
    float frequency;

    /**
     * @brief Touch data of the MPR121 driver
     */
    touch_sensors_t touch_data;
    uint8_t tstate;
  }driver_data;

  /**
   * @brief   Trigger related data.
   */
  struct {
    /**
     * @brief   Pointer to the trigger event source.
     */
    urt_osEventSource_t source;

    /**
     * @brief   Event listener for trigger events.
     */
    urt_osEventListener_t listener;

    /**
     * @brief   Timer to trigger ring data.
     */
    aos_timer_t timer;
  } trigger;

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
  /**
   * @brief   Publisher to publish the touch sensor data.
   */
  urt_publisher_t publisher;
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
} amiro_touchsensors_node_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void amiroTouchSensorsInit(amiro_touchsensors_node_t* touchsensors, urt_topicid_t touch_topicid, urt_osThreadPrio_t prio);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/******************************************************************************/
/* SUBSYSTEMS                                                                 */
/******************************************************************************/

#endif /* AMIRO_TOUCHSENSORS_H */

/** @} */
