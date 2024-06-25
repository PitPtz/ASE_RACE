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
 * @file    alld_LED.h
 * @brief   Led macros and structures.
 *
 * @defgroup lld_LED_v1 Version 1
 * @brief   Version 1
 * @ingroup lld_LED
 *
 * @addtogroup lld_LED_v1
 * @{
 */

#ifndef AMIROLLD_LED_H
#define AMIROLLD_LED_H

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
 * @brief LED driver data structure.
 */
typedef struct {
  const apalControlGpio_t* gpio;  /**< @brief The identifier of the GPIO.  */
} LEDDriver;

/**
 * @brief The state of the LED.
 */
typedef enum {
  LED_LLD_STATE_OFF = 0x00,   /**< 'on' state of the LED */
  LED_LLD_STATE_ON  = 0x01,   /**< 'off' state of the LED */
} led_lld_state_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
  apalExitStatus_t led_lld_set(const LEDDriver* const led, const led_lld_state_t state);
  apalExitStatus_t led_lld_get(const LEDDriver* const led, led_lld_state_t* const state);
  apalExitStatus_t led_lld_toggle(const LEDDriver* const led);
#ifdef __cplusplus
}
#endif

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROLLD_LED_H */

/** @} */

