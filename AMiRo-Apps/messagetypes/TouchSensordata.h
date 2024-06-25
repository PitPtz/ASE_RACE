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
 * @file    TouchSensordata.h
 * @brief   Data type describing the payload of the eight ring sensors.
 *
 * @defgroup msgtypes_touchsensor Touch Sensor
 * @ingroup	msgtypes
 * @brief   todo
 * @details todo
 *
 * @addtogroup msgtypes_touchsensor
 * @{
 */

#ifndef TOUCHSENSORDATA_H
#define TOUCHSENSORDATA_H
#include <stdint.h>
/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

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
 * @brief   Union to represent mpr121 values.
 */
typedef union {
  uint8_t data[4];
  struct {
    uint8_t nnw;
    uint8_t wnw;
    uint8_t wsw;
    uint8_t ssw;
  }values;
} touch_sensors_t;

/**
 * @brief   Structure to handle the touch sensor data.
 */
typedef struct touch_data {
  touch_sensors_t values;
}touch_data_t;

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

#endif /* TOUCHSENSORDATA_H */

/** @} */
