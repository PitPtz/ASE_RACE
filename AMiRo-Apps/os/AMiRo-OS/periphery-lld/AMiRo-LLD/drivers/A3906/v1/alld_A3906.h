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
 * @file    alld_A3906.h
 * @brief   Motor driver macros and structures.
 *
 * @defgroup lld_A3906_v1 Version 1
 * @brief   Version 1
 * @ingroup lld_A3906
 *
 * @addtogroup lld_A3906_v1
 * @{
 */

#ifndef AMIROLLD_A3906_H
#define AMIROLLD_A3906_H

#include <amiro-lld.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief Active state of power pin.
 */
#define A3906_LLD_POWER_PIN_ACTIVE_STATE  APAL_GPIO_ACTIVE_HIGH

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
 * @brief A3906 driver struct.
 */
typedef struct {
  const apalControlGpio_t* power_pin;   /**< @brief GPIO to enable/disable power of the A3906. */
} A3906Driver;

/**
 * @brief Power state of the A3906.
 */
typedef enum {
  A3906_LLD_POWER_OFF = 0x00,   /**< 'power off' state  */
  A3906_LLD_POWER_ON  = 0x01,   /**< 'power on' state   */
} a3906_lld_power_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
  apalExitStatus_t a3906_lld_set_power(const A3906Driver* const a3906, const a3906_lld_power_t power_state);
  apalExitStatus_t a3906_lld_get_power(const A3906Driver* const a3906, a3906_lld_power_t* const power_state);
  apalExitStatus_t a3906_lld_set_pwm(apalPWMDriver_t* pwm, const apalPWMchannel_t channel, const apalPWMwidth_t width);
#ifdef __cplusplus
}
#endif

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROLLD_A3906_H */

/** @} */
