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
 * @file    alld_VL53L1X.h
 * @brief   ToF sensor macros and structures.
 *
 * @defgroup lld_VL53L1X_v1 Version 1
 * @brief   Version 1
 * @ingroup lld_VL53L1X
 *
 * @addtogroup lld_VL53L1X_v1
 * @{
 */

#ifndef AMIROLLD_VL53L1X_H
#define AMIROLLD_VL53L1X_H

#include <amiro-lld.h>
#include <vl53l1_api.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief   Maximum I2C frequency (in Hz).
 */
#define VL53L1X_LLD_I2C_MAXFREQUENCY            1000000

/**
 * @brief   Default I2C address.
 */
#define VL53L1X_LLD_I2C_ADDR_DEFAULT            0x29

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

/*
 * Structures and types are declared in a separate file, because ST API requires
 * this information, too.
 */
#include <vl53l1_platform_user_data.h>

/**
 * @brief   The state of the VL53L1X device.
 * @details Represents the state of the XSHUT signal.
 */
typedef enum {
  VL53L1X_LLD_STATE_OFF = 0x00, /**< Device is deactivated via XSHUT signal. */
  VL53L1X_LLD_STATE_ON  = 0x01, /**< Device is active according to XSHUT signal. */
} vl53l1x_lld_state_t;

typedef enum {
  VL53L1X_LLD_RM_SHORT = 0,
  VL53L1X_LLD_RM_MED = 1,
  VL53L1X_LLD_RM_LONG = 2
} vl53l1x_lld_range_mode;
/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
  apalExitStatus_t vl53l1x_lld_getState(VL53L1XDriver* vl53l1x, vl53l1x_lld_state_t* state);
  apalExitStatus_t vl53l1x_lld_init(VL53L1XDriver* vl53l1x);
  apalExitStatus_t vl53l1x_lld_reset(VL53L1XDriver* vl53l1x);
  apalExitStatus_t vl53l1x_lld_setRangemode(VL53L1XDriver* dev, vl53l1x_lld_range_mode mode);
  apalExitStatus_t vl53l1x_lld_start_ranging(VL53L1XDriver* vl53l1x);
#ifdef __cplusplus
}
#endif

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROLLD_VL53L1X_H */

/** @} */
