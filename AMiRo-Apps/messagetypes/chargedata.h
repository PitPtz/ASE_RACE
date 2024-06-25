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
 * @file    chargedata.h
 * @brief   Data type holding charge information.
 *
 * @defgroup msgtypes_charge Charge
 * @ingroup	msgtypes
 * @brief   todo
 * @details todo
 *
 * @addtogroup msgtypes_charge
 * @{
 */

#ifndef CHARGEDATA_H
#define CHARGEDATA_H

#include <stdint.h>
#include <stdbool.h>
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
 * @brief   Charge data using µV representation.
 * @struct chargedata_u
 */
typedef struct chargedata_u {
  /**
   * @brief   Voltage in µV.
   */
  uint32_t volts;

  /**
   * @brief   Flag whether charging is active.
   */
  bool charging;
} chargedata_u;

/**
 * @brief   Charge data using SI representation.
 * @struct chargedata_si
 */
typedef struct chargedata_si {
  /**
   * @brief   Voltage in V.
   */
  float volts;

  /**
   * @brief   Flag whether charging is active.
   */
  bool charging;
} chargedata_si;

/**
 * @brief   Charge data of the battery level.
 * @struct battery_data_t
 */
typedef struct battery_data {

  /**
   * @brief   Average battery level over front and rear battery in percent.
   */
  uint16_t percentage;

  /**
   * @brief   Average battery level over front and rear battery in mAh.
   */
  uint16_t size;

} battery_data_t;


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

#endif /* CHARGEDATA_H */

/** @} */
