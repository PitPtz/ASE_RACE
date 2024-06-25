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
 * @file    ProximitySensordata.h
 * @brief   Data type describing the payload of the eight ring sensors.
 *
 * @defgroup msgtypes_proximitysensor ProximitySensor
 * @ingroup	msgtypes
 * @brief   todo
 * @details todo
 *
 * @addtogroup msgtypes_proximitysensor
 * @{
 */

#ifndef PROXIMITYSENSORDATA_H
#define PROXIMITYSENSORDATA_H

#include <stdint.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief   Number of ground facing VCNL4020 sensors.
 */
#define PROXIMITYSENSOR_NUM_SENSORS             8

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

typedef uint16_t proximitysensor_proximity_t;
typedef uint16_t proximitysensor_ambient_t;

/**
 * @brief   Union holding a set of proximity data.
 */
typedef union {
  proximitysensor_proximity_t data[PROXIMITYSENSOR_NUM_SENSORS];
  struct {
    proximitysensor_proximity_t nnw;
    proximitysensor_proximity_t wnw;
    proximitysensor_proximity_t wsw;
    proximitysensor_proximity_t ssw;
    proximitysensor_proximity_t sse;
    proximitysensor_proximity_t ese;
    proximitysensor_proximity_t ene;
    proximitysensor_proximity_t nne;
  } sensors;
} proximitysensor_proximitydata_t;

/**
 * @brief   Union holding a set of proximity data.
 */
typedef union {
  proximitysensor_ambient_t data[PROXIMITYSENSOR_NUM_SENSORS];
  struct {
    proximitysensor_ambient_t nnw;
    proximitysensor_ambient_t wnw;
    proximitysensor_ambient_t wsw;
    proximitysensor_ambient_t ssw;
    proximitysensor_ambient_t sse;
    proximitysensor_ambient_t ese;
    proximitysensor_ambient_t ene;
    proximitysensor_ambient_t nne;
  } sensors;
} proximitysensor_ambientdata_t;

/**
 * @brief   Proximity calibration data.
 */
typedef union {
  /**
   * @brief   Data to be submitted to the service.
   */
  struct {
    /**
     * @brief   Calibration command.
     */
    enum {
      PROXIMITYSENSOR_CALIBRATION_GET,      /**< Return the current offsets. */
      PROXIMITYSENSOR_CALIBRATION_SETONE,   /**< Set offset for a single sensor. */
      PROXIMITYSENSOR_CALIBRATION_SETALL,   /**< Set offset for all sensors. */
      PROXIMITYSENSOR_CALIBRATION_AUTO,     /**< Calibrate offset from measurements. */
    } command;

    /**
     * @brief   Further payload data.
     */
    union {
      /**
       * @brief   Offset data to set for a single sensor.
       */
      struct {
        /**
         * @brief   The sensor for which to set the offset.
         */
        uint32_t sensor;

        /**
         * @brief   Offset value to be set for the sensor.
         */
        proximitysensor_proximity_t value;
      } offset;

      /**
       * @brief   Offset data to set for all sensors.
       */
      proximitysensor_proximitydata_t offsets;

      /**
       * @brief   Number of measurements for automatic calibration.
       */
      uint32_t measurements;
    } data;
  } request;

  /**
   * @brief   Data to be transmitted by the service on response.
   */
  struct {
    /**
     * @brief   Calibration status.
     */
    enum {
      PROXIMITYSENSOR_CALIBSTATUS_OK,     /**< Calibration was successful. */
      PROXIMITYSENSOR_CALIBSTATUS_FAIL,   /**< Calibration failed. */
    } status;
    /**
     * @brief   Either the current or the newly set offsets (depends on request command).
     */
    proximitysensor_proximitydata_t offsets;
  } response;
} proximitysensor_calibration_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/******************************************************************************/
/* SUBSYSTEMS                                                                 */
/******************************************************************************/

#endif /* PROXIMITYSENSORDATA_H */

/** @} */
