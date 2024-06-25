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
 * @file    DWD_accodata.h
 * @brief   Data type describing the payload of the accelerometer data.
 *
 * @defgroup msgtypes_accelerometer Accelerometer
 * @ingroup	msgtypes
 * @brief   todo
 * @details todo
 *
 * @addtogroup msgtypes_accelerometer
 * @{
 */


#ifndef DWD_ACCODATA_H
#define DWD_ACCODATA_H
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
 * @brief   Union to represent to float converted accelerometer values.
 */
typedef union {
  float data[3];
  struct {
    float x;
    float y;
    float z;
  }values;
} acco_converted_data_t;


/**
 * @brief   Union to represent accelerometer data.
 */
typedef union {
  int16_t data[3];
  struct {
    int16_t x;
    int16_t y;
    int16_t z;
  }values;
} acco_sensor_t;

/**
 * @brief   Structure to handle the accelerometer data.
 */
typedef struct acco_data {
  acco_sensor_t values;
}acco_data_t;

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

#endif /* DWD_ACCELEROMETER_H */

/** @} */
