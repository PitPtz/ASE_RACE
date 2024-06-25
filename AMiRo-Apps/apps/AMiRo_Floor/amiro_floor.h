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
 * @file    amiro_floor.h
 * @brief   High-level driver application for the AMiRo DiWheelDrive floor sensing capabilities.
 *
 * @defgroup apps_amiro_floor Floor
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_amiro_floor
 * @{
 */

#ifndef AMIRO_FLOOR_H
#define AMIRO_FLOOR_H

#include <urt.h>
#include "../../messagetypes/DWD_floordata.h"

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#if !defined(AMIRO_FLOOR_STACKSIZE) || defined(__DOXYGEN__)
/**
 * @brief   Stack size of floor threads.
 */
#define AMIRO_FLOOR_STACKSIZE             512
#endif /* !defined(AMIRO_FLOOR_STACKSIZE) */

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
 * @brief   Floor node.
 * @struct  amiro_floor_node
 */
typedef struct amiro_floor_node {
  /**
   * @brief   Thread memory.
   */
  URT_THREAD_MEMORY(thread, AMIRO_FLOOR_STACKSIZE);

  /**
   * @brief   Node object.
   */
  urt_node_t node;

  /**
   * @brief   Driver related data.
   */
  struct {
    /**
     * @brief  driver for all four floor sensors
     */
    VCNL4020Driver* vcnl;

    /**
     * @brief  mux for all four floor sensors
     */
    PCA9544ADriver* mux;

    /**
     * @brief   Proximity measurement frequency.
     */
    vcnl4020_lld_proxratereg_t proximity_frequency;

    /**
     * @brief   Ambient measurement parameters.
     */
    uint8_t ambient_params;
  } drivers;

  /**
   * @brief   Data related to the proximity capabilities.
   */
  struct {
    /**
     * @brief   Timer to trigger floor data.
     */
    aos_timer_t timer;

    /**
     * @brief   Publisher to publish the proximity data.
     */
    urt_publisher_t publisher;

    /**
     * @brief   Calibration related data.
     */
    struct {
      /**
       * @brief   Calibrated offsets for the proximity sensors.
       */
      floor_proximitydata_t offsets;

      /**
       * @brief   Service to calibrate proximity sensors.
       */
      urt_service_t service;

      /**
       * @brief   Dispatched request for the service.
       */
      urt_service_dispatched_t dispatched;

      /**
       * @brief   Data related to auto-calibration.
       */
      struct {
        /**
         * @brief Number of total measurements.
         */
        size_t measurements;

        /**
         * @brief   Number of finished measurements.
         */
        size_t counter;

        /**
         * @brief   Accumulated sensor data.
         */
        uint32_t accumulated[FLOOR_NUM_SENSORS];
      } automatic;
    } calibration;
  } proximity;

  /**
   * @brief   Data related to the ambient light capabilities.
   */
  struct {
    /**
     * @brief   Timer to trigger floor data.
     */
    aos_timer_t timer;

    /**
     * @brief   Publisher to publish the ambient data.
     */
    urt_publisher_t publisher;
  } ambient;

} amiro_floor_node_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void amiroFloorInit(amiro_floor_node_t* floor,
                 VCNL4020Driver* vcnl_driver,
                 PCA9544ADriver* mux_driver,
                 vcnl4020_lld_proxratereg_t proximity_frequency,
                 urt_topic_t* proximity_topic,
                 urt_serviceid_t proximity_serviceid,
                 uint8_t ambient_parameters,
                 urt_topic_t* ambient_topic,
                 urt_osThreadPrio_t prio);
#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
  int floorShellCallback_calibrate(BaseSequentialStream* stream, int argc, const char* argv[], urt_service_t* const service);
#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/******************************************************************************/
/* SUBSYSTEMS                                                                 */
/******************************************************************************/

#endif /* AMIRO_FLOOR_H */

/** @} */
