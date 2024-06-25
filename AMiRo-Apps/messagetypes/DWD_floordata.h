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
 * @file    DWD_floordata.h
 * @brief   Data type describing the payload of the four floor sensors.
 *
 * @defgroup msgtypes_floor Floor
 * @ingroup	msgtypes
 * @brief   todo
 * @details todo
 *
 * @addtogroup msgtypes_floor
 * @{
 */

#ifndef DWD_FLOORDATA_H
#define DWD_FLOORDATA_H

#include <stdint.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief   Number of ground facing VCNL4020 sensors.
 */
#define FLOOR_NUM_SENSORS             4

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

typedef uint16_t floor_proximity_t;
typedef uint16_t floor_ambient_t;

/**
 * @brief   Union holding a set of proximity data.
 */
typedef union {
  floor_proximity_t data[FLOOR_NUM_SENSORS];
  struct {
    floor_proximity_t left_wheel;
    floor_proximity_t left_front;
    floor_proximity_t right_front;
    floor_proximity_t right_wheel;
  } sensors;
} floor_proximitydata_t;

/**
 * @brief   Union holding a set of ambient data.
 */
typedef union {
  floor_ambient_t data[FLOOR_NUM_SENSORS];
  struct {
    floor_ambient_t left_wheel;
    floor_ambient_t left_front;
    floor_ambient_t right_front;
    floor_ambient_t right_wheel;
  } sensors;
} floor_ambientdata_t;

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
      FLOOR_CALIBRATION_GET,      /**< Return the current offsets. */
      FLOOR_CALIBRATION_SETONE,   /**< Set offset for a single sensor. */
      FLOOR_CALIBRATION_SETALL,   /**< Set offset for all sensors. */
      FLOOR_CALIBRATION_AUTO,     /**< Calibrate offset from measurements. */
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
        floor_proximity_t value;
      } offset;

      /**
       * @brief   Offset data to set for all sensors.
       */
      floor_proximitydata_t offsets;

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
      FLOOR_CALIBSTATUS_OK,     /**< Calibration was successful. */
      FLOOR_CALIBSTATUS_FAIL,   /**< Calibration failed. */
    } status;
    /**
     * @brief   Either the current or the newly set offsets (depends on request command).
     */
    floor_proximitydata_t offsets;
  } response;
} floor_calibration_t;

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

#endif /* DWD_FLOORDATA_H */

/** @} */
