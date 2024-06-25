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
 * @file    DWD_gyrodata.h
 * @brief   Data type describing the payload of the four floor sensors.
 *
 * @defgroup msgtypes_gyroscope Gyroscope
 * @ingroup	msgtypes
 * @brief   todo
 * @details todo
 *
 * @addtogroup msgtypes_gyroscope
 * @{
 */


#ifndef DWD_GYRODATA_H
#define DWD_GYRODATA_H
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
 * @brief   Union to represent to float converted gyroscope values.
 */
typedef union {
  float data[3];
  struct {
    float x;
    float y;
    float z;
  }values;
} gyro_converted_data_t;


/**
 * @brief   Union to represent gyroscope values.
 */
typedef union {
  int16_t data[3];
  struct {
    int16_t x;
    int16_t y;
    int16_t z;
  }values;
} gyro_sensor_t;

/**
 * @brief   Structure to handle the gyroscope data.
 */
typedef struct gyro_data {
  gyro_sensor_t values;
}gyro_data_t;

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

#endif /* DWD_GYRODATA_H */

/** @} */
