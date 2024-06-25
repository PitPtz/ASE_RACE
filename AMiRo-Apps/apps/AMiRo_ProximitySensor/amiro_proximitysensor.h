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
 * @file    amiro_proximitysensor.h
 * @brief   High-level driver application for the AMiRo ProximitySensor module.
 *
 * @defgroup apps_amiro_proximitysensor ProximitySensor
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_amiro_proximitysensor
 * @{
 */

#ifndef AMIRO_PROXIMITYSENSOR_H
#define AMIRO_PROXIMITYSENSOR_H

#include <urt.h>
#include "../../messagetypes/ProximitySensordata.h"

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#if !defined(AMIRO_PROXIMITYSENSOR_STACKSIZE) || defined(__DOXYGEN__)
/**
 * @brief   Stack size of floor threads.
 */
#define AMIRO_PROXIMITYSENSOR_STACKSIZE               1024
#endif /* !defined(AMIRO_PROXIMITYSENSOR_STACKSIZE) */

/**
 * @brief   Number of I2C busses the ProximitySensor is connected to.
 */
#define PROXIMITYSENSOR_I2C_BUSSES              2

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
 * @brief   ProximitySensor node.
 * @struct  proximitysensor_node
 */
typedef struct proximitysensor_node {
  /**
   * @brief   Thread memory.
   */
  URT_THREAD_MEMORY(thread, AMIRO_PROXIMITYSENSOR_STACKSIZE);

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
    VCNL4020Driver* vcnl[PROXIMITYSENSOR_I2C_BUSSES];

    /**
     * @brief  mux for all four floor sensors
     */
    PCA9544ADriver* mux[PROXIMITYSENSOR_I2C_BUSSES];

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
      proximitysensor_proximitydata_t offsets;

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
        uint32_t accumulated[PROXIMITYSENSOR_NUM_SENSORS];
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

} amiro_proximitysensor_node_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void amiroProximitySensorInit(amiro_proximitysensor_node_t* proximitysensor,
                           VCNL4020Driver* vcnl_driver[PROXIMITYSENSOR_I2C_BUSSES],
                           PCA9544ADriver* mux_driver[PROXIMITYSENSOR_I2C_BUSSES],
                           vcnl4020_lld_proxratereg_t proximity_frequency,
                           urt_topic_t* proximity_topic,
                           urt_serviceid_t proximity_serviceid,
                           uint8_t ambient_parameters,
                           urt_topic_t* ambient_topic,
                           urt_osThreadPrio_t prio);
#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
  int proximitysensorShellCallback_calibrate(BaseSequentialStream* stream, int argc, const char* argv[], urt_service_t* const service);
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

#endif /* FLOOR_H */

/** @} */
