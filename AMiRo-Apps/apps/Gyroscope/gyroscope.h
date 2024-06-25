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
 * @file    gyroscope.h
 *
 * @defgroup apps_gyroscope Gyroscope
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_gyroscope
 * @{
 */

#ifndef GYROSCOPE_H
#define GYROSCOPE_H

#include <urt.h>
#include "../../messagetypes/DWD_gyrodata.h"

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#if !defined(GYRO_STACKSIZE) || defined(__DOXYGEN__)
/**
 * @brief   Stack size of gyroscope threads.
 */
#define GYRO_STACKSIZE             256
#endif /* !defined(GYRO_STACKSIZE) */

/**
 * @brief   Event flag to identify trigger events related to publish-subscribe.
 */
#define TRIGGERFLAG_GYRO           (urt_osEventFlags_t)(1 << 0)

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
 * @brief   Gyro node.
 * @struct  gyro_node
 */
typedef struct gyro_node {
  /**
   * @brief   Thread memory.
   */
  URT_THREAD_MEMORY(thread, GYRO_STACKSIZE);

  /**
   * @brief   Node object.
   */
  urt_node_t node;

  /**
   * @brief Id of the topic where the gyroscope data are published on.
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
     * @brief  driver for the gyroscope
     */
    L3G4200DDriver* driver;

    /**
     * @brief  gyroscope configuration
     */
    l3g4200d_lld_cfg_t cfg;

    /**
     * @brief  SPI Configuration
     */
    const SPIConfig *spiconf;

    /**
     * @brief node frequency in Hz.
     */
    float frequency;

    /**
     * @brief Gyroscope data of the L3G4200D driver
     */
    gyro_data_t data;
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
     * @brief   Timer to trigger gyroscope data.
     */
    aos_timer_t timer;
  } trigger;

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
  /**
   * @brief   Publisher to publish the gyroscope data.
   */
  urt_publisher_t publisher;

  urt_publisher_t can_publisher;
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

} gyro_node_t;


/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

  void gyroInit(gyro_node_t* gyro,
                urt_topicid_t topicid,
                urt_osThreadPrio_t prio,
                float frequency);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* GYROSCOPE_H */

/** @} */
