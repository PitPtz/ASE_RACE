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
 * @file    alld_TPS20xxB.c
 * @brief   Power-Distribution function implementations.
 *
 * @addtogroup lld_TPS20xxB_v1
 * @{
 */

#include "alld_TPS20xxB.h"

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
 * @brief Set the laser_en GPIO pin.
 * @param[in]   tps20xxb    TPS20xxB driver
 * @param[in]   enable      new state of the pin
 * @return                  An indicator whether the call was successfull
 */
apalExitStatus_t tps20xxb_lld_set_enable(const TPS20xxBDriver* const tps20xxb, const tps20xxb_lld_enable_t enable)
{
  apalDbgAssert(tps20xxb != NULL);

  return apalControlGpioSet(tps20xxb->enable, enable == TPS20xxB_LLD_ENABLE ? APAL_GPIO_ON : APAL_GPIO_OFF);
}

/**
 * @brief Read the laser_en GPIO pin.
 * @param[in]   tps20xxb    TPS20xxB driver
 * @param[out]  enable      state of the enable pin
 * @return                  An indicator whether the call was successfull
 */
apalExitStatus_t tps20xxb_lld_read_enable(const TPS20xxBDriver* const tps20xxb, tps20xxb_lld_enable_t* const enable)
{
  apalDbgAssert(tps20xxb != NULL);
  apalDbgAssert(enable != NULL);

  apalControlGpioState_t gpio_state;
  apalExitStatus_t status = apalControlGpioGet(tps20xxb->enable, &gpio_state);
  *enable = gpio_state == APAL_GPIO_ON ? TPS20xxB_LLD_ENABLE : TPS20xxB_LLD_DISABLE;
  return status;
}

/**
 * @brief Read the laser_oc GPIO pin.
 * @param[in]   tps20xxb    TPS20xxB driver
 * @param[out]  oc          state of the overcurrent pin
 * @return                  An indicator whether the call was successfull
 */
apalExitStatus_t tps20xxb_lld_read_overcurrent(const TPS20xxBDriver* const tps20xxb, tps20xxb_lld_overcurrent_t* const oc)
{
  apalDbgAssert(tps20xxb != NULL);
  apalDbgAssert(oc != NULL);

  apalControlGpioState_t gpio_state;
  apalExitStatus_t status = apalControlGpioGet(tps20xxb->overcurrent, &gpio_state);
  *oc = gpio_state == APAL_GPIO_ON ? TPS20xxB_LLD_OVERCURRENT : TPS20xxB_LLD_NO_OVERCURRENT;
  return status;
}

/** @} */

