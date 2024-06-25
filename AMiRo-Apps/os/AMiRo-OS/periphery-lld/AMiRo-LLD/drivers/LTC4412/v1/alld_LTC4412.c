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
 * @file    alld_LTC4412.c
 * @brief   PowerPath Controller function implementations
 *
 * @addtogroup lld_LTC4412_v1
 * @{
 */

#include "alld_LTC4412.h"

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
 * @brief Read the value of the PATH_DCEN Gpio pin.
 * @param[in]   ltc4412     ltc4412 driver
 * @param[out]  ctrl        current value of the gpio pin
 * @return                  An indicator whether the call was successfull
 */
apalExitStatus_t ltc4412_lld_get_ctrl(const LTC4412Driver* const ltc4412, ltc4412_lld_ctrl_t* const ctrl)
{
  apalDbgAssert(ltc4412 != NULL);
  apalDbgAssert(ctrl != NULL);

  apalControlGpioState_t gpio_state;
  apalExitStatus_t status = apalControlGpioGet(ltc4412->gpio_ctrl, &gpio_state);
  *ctrl = gpio_state == APAL_GPIO_ON ? LTC4412_LLD_CTRL_ACTIVE : LTC4412_LLD_CTRL_INACTIVE;
  return status;
}

/**
 * @brief Set the value of the PATH_DCEN Gpio pin.
 * @param[in]   ltc4412     ltc4412 driver
 * @param[in]   ctrl        new value of the gpio pin
 * @return                  An indicator whether the call was successfull
 */
apalExitStatus_t ltc4412_lld_set_ctrl(const LTC4412Driver* const ltc4412, const ltc4412_lld_ctrl_t ctrl)
{
  apalDbgAssert(ltc4412 != NULL);

  return apalControlGpioSet(ltc4412->gpio_ctrl, ctrl == LTC4412_LLD_CTRL_ACTIVE ? APAL_GPIO_ON : APAL_GPIO_OFF);
}

/**
 * @brief Read the value of the PATH_DCSTAT Gpio pin.
 * @param[in]   ltc4412     ltc4412 driver
 * @param[out]  stat        current value of the gpio pin
 * @return                  An indicator whether the call was successfull
 */
apalExitStatus_t ltc4412_lld_get_stat(const LTC4412Driver* const ltc4412, ltc4412_lld_stat_t* const stat)
{
  apalDbgAssert(ltc4412 != NULL);
  apalDbgAssert(stat != NULL);

  apalControlGpioState_t gpio_state;
  apalExitStatus_t status = apalControlGpioGet(ltc4412->gpio_stat, &gpio_state);
  *stat = gpio_state == APAL_GPIO_ON ? LTC4412_LLD_STAT_ACTIVE : LTC4412_LLD_STAT_INACTIVE;
  return status;
}

/** @} */

