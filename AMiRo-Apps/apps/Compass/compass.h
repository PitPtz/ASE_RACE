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
 * @file    compass.h
 *
 * @defgroup apps_compass Compass
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_compass
 * @{
 */

#ifndef COMPASS_H
#define COMPASS_H

#include <urt.h>
#include "../../messagetypes/DWD_compassdata.h"

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#if !defined(COMPASS_STACKSIZE) || defined(__DOXYGEN__)
/**
 * @brief   Stack size of compass threads.
 */
#define COMPASS_STACKSIZE             256
#endif /* !defined(COMPASS_STACKSIZE) */

/**
 * @brief   Event flag to identify trigger events related to publish-subscribe.
 */
#define TRIGGERFLAG_COMPASS           (urt_osEventFlags_t)(1 << 0)

/**
 * @brief   Event mask to set on a trigger event.
 */
#define TRIGGEREVENT                (urt_osEventMask_t)(1<< 1)

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
 * @brief   compass node.
 * @struct  compass_node
 */
typedef struct compass_node {
  /**
   * @brief   Thread memory.
   */
  URT_THREAD_MEMORY(thread, COMPASS_STACKSIZE);

  /**
   * @brief   Node object.
   */
  urt_node_t node;

  /**
   * @brief Id of the topic where the compass data are published on.
   */
  urt_topicid_t topicid;

  /**
   * @brief   Time of the published data.
   */
  urt_osTime_t time;

  /**
   * @brief   Driver related data.
   */
  struct {
    /**
     * @brief  driver for the compass
     */
    HMC5883LDriver* driver;

    /**
     * @brief  compass configuration
     */
    hmc5883l_lld_config_t cfg;

    /**
     * @brief node frequency in Hz.
     */
    float frequency;

    /**
     * @brief compass data of the HMC5883L driver
     */
    compass_sensor_t data;

    /**
     * @brief   Timeout value in microseconds.
     */
    apalTime_t timeout;
  } driver_data;

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
     * @brief   Timer to trigger compass data.
     */
    aos_timer_t timer;
  } trigger;

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
  /**
   * @brief   Publisher to publish the compass data.
   */
  urt_publisher_t publisher;
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

} compass_node_t;


/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

  void compassInit(compass_node_t* compass, urt_topicid_t topicid, urt_osThreadPrio_t prio, float frequency);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* COMPASS_H */

/** @} */
