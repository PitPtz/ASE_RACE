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
 * @file    accelerometer.h
 *
 * @defgroup apps_accelerometer Acceleromenter
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_accelerometer
 * @{
 */

#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <urt.h>
#include "../../messagetypes/DWD_accodata.h"

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#if !defined(ACCO_STACKSIZE) || defined(__DOXYGEN__)
/**
 * @brief   Stack size of accelerometer threads.
 */
#define ACCO_STACKSIZE             256
#endif /* !defined(ACCO_STACKSIZE) */

/**
 * @brief   Event flag to identify trigger events related to publish-subscribe.
 */
#define TRIGGERFLAG_ACCO           (urt_osEventFlags_t)(1 << 0)

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
 * @brief   accelerometer node.
 * @struct  acco_node
 */
typedef struct acco_node {
  /**
   * @brief   Thread memory.
   */
  URT_THREAD_MEMORY(thread, ACCO_STACKSIZE);

  /**
   * @brief   Node object.
   */
  urt_node_t node;

  /**
   * @brief Id of the topic where the accelerometer data are published on.
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
     * @brief  driver for the accelerometer
     */
    LIS331DLHDriver* driver;

    /**
     * @brief  accelerometer configuration
     */
    lis331dlh_lld_cfg_t cfg;

    /**
     * @brief  SPI Configuration
     */
    const SPIConfig *spiconf;

    /**
     * @brief node frequency in Hz.
     */
    float frequency;

    /**
     * @brief accelerometer data of the L3G4200D driver
     */
    acco_sensor_t data;
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
     * @brief   Timer to trigger accelerometer data.
     */
    aos_timer_t timer;
  } trigger;

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
  /**
   * @brief   Publisher to publish the accelerometer data.
   */
  urt_publisher_t publisher;
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

} acco_node_t;


/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

  void accoInit(acco_node_t* acco, urt_topicid_t topicid, urt_osThreadPrio_t prio, float frequency);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* ACCELEROMETER_H */

/** @} */
