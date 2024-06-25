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
 * @file    alld_bq241xx.h
 * @brief   Battery charger constants and driver structure.
 *
 * @defgroup lld_bq241xx_v1 Version 1
 * @brief   Version 1
 * @ingroup lld_bq241xx
 *
 * @addtogroup lld_bq241xx_v1
 * @{
 */

#ifndef AMIROLLD_BQ241XX_H
#define AMIROLLD_BQ241XX_H

#include <amiro-lld.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief Active state of enabled gpio.
 */
#define BQ241xx_LLD_ENABLED_GPIO_ACTIVE_STATE         APAL_GPIO_ACTIVE_LOW

/**
 * @brief Active state of charge status gpio.
 */
#define BQ241xx_LLD_CHARGE_STATUS_GPIO_ACTIVE_STATE   APAL_GPIO_ACTIVE_HIGH

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
 * @brief The BQ241xx driver struct.
 */
typedef struct {
  const apalControlGpio_t* gpio_enabled;        /**< @brief GPIO to enable/disable the bq24103a */
  const apalControlGpio_t* gpio_charge_status;  /**< @brief GPIO showing the status of the bq24103a */
} BQ241xxDriver;

/**
 * @brief Enable enum.
 */
typedef enum {
  BQ241xx_LLD_ENABLED   = 0x01,         /**< @brief 'charging on' state */
  BQ241xx_LLD_DISABLED  = 0x00,         /**< @brief 'charging off' state */
} bq241xx_lld_enable_t;

/**
 * @brief Charge state enum.
 */
typedef enum {
  BQ241xx_LLD_CHARGING      = 0x01,     /**< @brief status indicating that the battery is being charged */
  BQ241xx_LLD_NOT_CHARGING  = 0x00,     /**< @brief status indicating that the battery is not being charged */
} bq241xx_lld_charge_state_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
  apalExitStatus_t bq241xx_lld_get_enabled(const BQ241xxDriver* const bq241xx, bq241xx_lld_enable_t* const enable);
  apalExitStatus_t bq241xx_lld_set_enabled(const BQ241xxDriver* const bq241xx, const bq241xx_lld_enable_t enable);
  apalExitStatus_t bq241xx_lld_get_charge_status(const BQ241xxDriver* const bq241xx, bq241xx_lld_charge_state_t* const charge);
#ifdef __cplusplus
}
#endif

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROLLD_BQ241XX_H */

/** @} */

