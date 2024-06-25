/*
AMiRo-LLD is a compilation of low-level hardware drivers for the Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2016..2022  Thomas Schöpping et al.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    alld_VL6180X.h
 * @brief   Proximity & ALS sensor constants and structures.
 *
 * @defgroup lld_VL6180X_v1 Version 1
 * @brief   Version 1
 * @ingroup lld_VL6180X
 *
 * @addtogroup lld_VL6180X_v1
 * @{
 */

#ifndef AMIROLLD_VL6180X_H
#define AMIROLLD_VL6180X_H

#include "vl6180x_api.h"

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief Maximum I2C frequency.
 */
#define VL6180X_LLD_I2C_MAXFREQUENCY   400000

/**
 * @brief Default I2C address.
 */
#define VL6180X_LLD_I2C_DEFAULTADDRESS  0x29

/**
 * @brief Module Identifier value.
 */
#define VL6180X_LLD_MODELID           0xB4

/**
 * @brief   Register address to enable interleaved mode.
 */
#define INTERLEAVED_MODE__ENABLE      0x02A3

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

typedef struct VL6180X VL6180XDriver;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROLLD_VL6180X_H */

/** @} */

