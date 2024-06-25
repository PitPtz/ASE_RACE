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
 * @file    alld_TPS6211x.c
 * @brief   Step-Down Converter function implementations.
 *
 * @addtogroup lld_TPS6211x_v1
 * @{
 */

#include "alld_TPS6211x.h"

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
 * @param[in]   tps6211x    TPS6211x drivers
 * @param[out]  power       current value of the gpio pin
 * @return                  An indicator whether the call was successfull
 */
apalExitStatus_t tps6211x_lld_get_power_en(const TPS6211xDriver* const tps6211x, tps6211x_lld_power_en_t* const power)
{
  apalDbgAssert(tps6211x != NULL);
  apalDbgAssert(power != NULL);

  apalControlGpioState_t gpio_state;
  apalExitStatus_t status = apalControlGpioGet(tps6211x->gpio_power, &gpio_state);
  *power = (gpio_state == APAL_GPIO_ON) ? TPS6211x_LLD_POWER_ENABLED : TPS6211x_LLD_POWER_DISABLED;
  return status;
}

/**
 * @brief Set the value of the POWER_EN Gpio pin.
 * @param[in]   tps6211x    TPS6211x drivers
 * @param[in]   power       new value of the gpio pin
 * @return                  An indicator whether the call was successfull
 */
apalExitStatus_t tps6211x_lld_set_power_en(const TPS6211xDriver* const tps6211x, const tps6211x_lld_power_en_t power)
{
  apalDbgAssert(tps6211x != NULL);

  return apalControlGpioSet(tps6211x->gpio_power, (power == TPS6211x_LLD_POWER_ENABLED) ? APAL_GPIO_ON : APAL_GPIO_OFF);
}

/** @} */

