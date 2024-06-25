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
 * @file    alld_switch.h
 * @brief   Switch macros and structures.
 *
 * @defgroup lld_switch_v1 Version 1
 * @brief   Version 1
 * @ingroup lld_switch
 *
 * @addtogroup lld_switch_v1
 * @{
 */

#ifndef AMIROLLD_SWITCH_H
#define AMIROLLD_SWITCH_H

#include <amiro-lld.h>

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
 * @brief Switch driver data structure.
 */
typedef struct {
  const apalControlGpio_t* gpio;  /**< @brief The identifier of the GPIO.  */
} SwitchDriver;

/**
 * @brief The state of the switch.
 */
typedef enum {
  SWITCH_LLD_STATE_OFF = 0x00,   /**< 'released' state of the switch */
  SWITCH_LLD_STATE_ON  = 0x01,   /**< 'pressed' state of the switch */
} switch_lld_state_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
  apalExitStatus_t switch_lld_get(const SwitchDriver* const switchp, switch_lld_state_t* const state);
#ifdef __cplusplus
}
#endif

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROLLD_SWITCH_H */

/** @} */

