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
 * @file    amiro_bno055.h
 *
 * @defgroup apps_bno055 BNO055
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_bno055
 * @{
 */

#ifndef AMIRO_BNO055_H
#define AMIRO_BNO055_H

#include <urt.h>
#include "../../messagetypes/DWD_gyrodata.h"
#include "../../messagetypes/DWD_magnodata.h"
#include "../../messagetypes/DWD_accodata.h"
#include "../../messagetypes/DWD_gravitydata.h"

#if defined(BOARD_DIWHEELDRIVE_1_2) || defined(__DOXYGEN__)

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#if !defined(AMIRO_BNO_STACKSIZE) || defined(__DOXYGEN__)
/**
 * @brief   Stack size of bno055 threads.
 */
#define AMIRO_BNO_STACKSIZE             512
#endif /* !defined(AMIRO_BNO_STACKSIZE) */

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
 * @brief   amiro_bno055 node.
 * @struct  amiro_bno_node
 */
typedef struct amiro_bno_node {
  /**
   * @brief   Thread memory.
   */
  URT_THREAD_MEMORY(thread, AMIRO_BNO_STACKSIZE);

  /**
   * @brief   Node object.
   */
  urt_node_t node;

  /**
   * @brief   Time of the published data.
   */
  urt_osTime_t time;

  /**
   * @brief  driver for the bno055
   */
  BNO055DRIVER* driver;

  /**
   * @brief  Operation mode of the bno055
   * @details Common modes are BNO055_OPERATION_MODE_AMG, BNO055_OPERATION_MODE_IMUPLUS, BNO055_OPERATION_MODE_NDOF
   * BNO055_OPERATION_MODE_AMG (no fusioned data)
   * BNO055_OPERATION_MODE_IMUPLUS (relative orientation is calculated out of the accelerometer and gyroscope)
   * BNO055_OPERATION_MODE_NDOF (absolute orientation is calculated)
   */
  uint8_t operation_mode;

  struct {
    /**
     * @brief Gyrometer frequency in Hz.
     */
    float frequency;

    /**
     * @brief   Timer to trigger bno055 gyroscope data.
     */
    aos_timer_t timer;

  #if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
    /**
     * @brief   Publisher to publish the bno055 gyroscope data.
     */
    urt_publisher_t publisher;
  #endif /* (URT_CFG_PUBSUB_ENABLED == true) */

    /**
     * @brief   To float converted data from the BNO055.
     */
    gyro_converted_data_t data;
  } gyro;

  struct {
    /**
     * @brief Accelerometer frequency in Hz.
     */
    float frequency;

    /**
     * @brief   Timer to trigger bno055 accelerometer data.
     */
    aos_timer_t timer;

  #if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
    /**
     * @brief   Publisher to publish the bno055 accelerometer data.
     */
    urt_publisher_t publisher;
  #endif /* (URT_CFG_PUBSUB_ENABLED == true) */

    /**
     * @brief   To float converted data from the BNO055.
     */
    acco_converted_data_t data;
  } acco;

  struct {
    /**
     * @brief   Timer to trigger bno055 magnetometer data.
     */
    aos_timer_t timer;

    /**
     * @brief Magnetometer frequency in Hz.
     */
    float frequency;

  #if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
    /**
     * @brief   Publisher to publish the bno055 magnetometer data.
     */
    urt_publisher_t publisher;
  #endif /* (URT_CFG_PUBSUB_ENABLED == true) */

    /**
     * @brief   To float converted data from the BNO055.
     */
    magno_converted_data_t data;
  } magno;

  struct {
    /**
     * @brief   Timer to trigger bno055 gravity data.
     */
    aos_timer_t timer;

  #if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
    /**
     * @brief   Publisher to publish the bno055 gravity data.
     */
    urt_publisher_t publisher;
  #endif /* (URT_CFG_PUBSUB_ENABLED == true) */

    /**
     * @brief   Gravity data from the BNO055.
     */
    gravity_data_t data;
  } gravity;

} amiro_bno_node_t;


/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

  void amiroBnoInit(amiro_bno_node_t* bno,
               urt_topicid_t gyro_topicid, 
               urt_topicid_t magno_topicid, 
               urt_topicid_t acco_topicid, 
               urt_topicid_t grav_topicid, 
               float gyro_frequency,
               float magno_frequency,
               float acco_frequency,
               uint8_t operation_mode,
               urt_osThreadPrio_t prio);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* BOARD_DIWHEELDRIVE_1_2 */

#endif /* AMIRO_BNO055_H */

/** @} */
