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
 * @file    alld_bq241xx.c
 * @brief   Battery charger function implementations.
 *
 * @addtogroup lld_bq241xx_v1
 * @{
 */
#include "alld_bq241xx.h"

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED VARIABLES                                                         */
/******************************************************************************/

/******************************************************************************/
/* LOCAL TYPES                                                                */
/******************************************************************************/

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

/**
 * @brief Read the value of the POWER_EN Gpio pin.
 * @param[in]   bq241xx     bq241xx driver
 * @param[out]  enable      current value of the gpio pin
 * @return                  An indicator whether the call was successfull
 */
apalExitStatus_t bq241xx_lld_get_enabled(const BQ241xxDriver* const bq241xx, bq241xx_lld_enable_t* const enable)
{
  apalDbgAssert(bq241xx != NULL);
  apalDbgAssert(enable != NULL);

  apalControlGpioState_t gpio_state;
  apalExitStatus_t status = apalControlGpioGet(bq241xx->gpio_enabled, &gpio_state);
  *enable = gpio_state == APAL_GPIO_ON ? BQ241xx_LLD_ENABLED : BQ241xx_LLD_DISABLED;
  return status;
}

/**
 * @brief Set the value of the POWER_EN Gpio pin.
 * @param[in]   bq241xx     bq241xx driver
 * @param[in]   enable      new value of the gpio pin
 * @return                  An indicator whether the call was successfull
 */
apalExitStatus_t bq241xx_lld_set_enabled(const BQ241xxDriver* const bq241xx, const bq241xx_lld_enable_t enable)
{
  apalDbgAssert(bq241xx != NULL);

  return apalControlGpioSet(bq241xx->gpio_enabled, enable == BQ241xx_LLD_ENABLED ? APAL_GPIO_ON : APAL_GPIO_OFF);
}

/**
 * @brief Read the value of the STAT1 charge status Gpio pin.
 * @param[in]   bq241xx     bq241xx driver
 * @param[out]  charge      current value of the gpio pin
 * @return                  An indicator whether the call was successfull
 */
apalExitStatus_t bq241xx_lld_get_charge_status(const BQ241xxDriver* const bq241xx, bq241xx_lld_charge_state_t* const charge)
{
  apalDbgAssert(bq241xx != NULL);
  apalDbgAssert(charge != NULL);

  apalControlGpioState_t gpio_state;
  apalExitStatus_t status = apalControlGpioGet(bq241xx->gpio_charge_status, &gpio_state);
  *charge = gpio_state == APAL_GPIO_ON ? BQ241xx_LLD_CHARGING : BQ241xx_LLD_NOT_CHARGING;
  return status;
}

/** @} */

