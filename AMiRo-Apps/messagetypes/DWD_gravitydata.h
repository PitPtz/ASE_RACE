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
 * @file    DWD_gravitydata.h
 * @brief   Data type describing the payload of the accelerometer data.
 *
 * @defgroup msgtypes_gravity Gravity
 * @ingroup	msgtypes
 * @brief   todo
 * @details todo
 *
 * @addtogroup msgtypes_gravity
 * @{
 */


#ifndef DWD_GRAVITYDATA_H
#define DWD_GRAVITYDATA_H

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
 * @brief   Union to represent gravity values.
 */
typedef union {
  float data[3];
  struct {
    float x;
    float y;
    float z;
  }values;
} gravity_data_t;


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

#endif /* DWD_GRAVITYDATA_H */

/** @} */
