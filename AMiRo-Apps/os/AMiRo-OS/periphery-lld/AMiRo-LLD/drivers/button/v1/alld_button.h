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
 * @file    alld_button.h
 * @brief   Button macros and structures.
 *
 * @defgroup lld_button_v1 Version 1
 * @brief   Version 1
 * @ingroup lld_button
 *
 * @addtogroup lld_button_v1
 * @{
 */

#ifndef AMIROLLD_BUTTON_H
#define AMIROLLD_BUTTON_H

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
 * @brief Button driver data structure.
 */
typedef struct {
  const apalControlGpio_t* gpio;  /**< @brief The identifier of the GPIO.  */
} ButtonDriver;

/**
 * @brief The state of the button.
 */
typedef enum {
  BUTTON_LLD_STATE_RELEASED = 0x00,   /**< 'released' state of the button */
  BUTTON_LLD_STATE_PRESSED  = 0x01,   /**< 'pressed' state of the button */
} button_lld_state_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
  apalExitStatus_t button_lld_get(const ButtonDriver* const button, button_lld_state_t* const state);
#ifdef __cplusplus
}
#endif

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROLLD_BUTTON_H */

/** @} */

