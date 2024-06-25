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
 * @file    alld_MIC9404x.c
 * @brief   MIC9404x power switch function implementations.
 *
 * @addtogroup lld_MIC9404x_v1
 * @{
 */

#include "alld_MIC9404x.h"

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
 * @brief Set the MIC9404x to a specific state.
 * @param[in]   mic9404x    The MIC9404x driver object.
 * @param[in]   state       The state to set the MIC9404x to.
 * @return                  An indicator whether the call was successful.
 */
apalExitStatus_t mic9404x_lld_set(const MIC9404xDriver* const mic9404x, const mic9404x_lld_state_t state)
{
  apalDbgAssert(mic9404x != NULL);

  return apalControlGpioSet(mic9404x->en, (state == MIC9404x_LLD_STATE_ON) ? APAL_GPIO_ON : APAL_GPIO_OFF);
}

/**
 * @brief Get the current status of the MIC9404x.
 * @param[in]   mic9404x    The MIC9404x driver object.
 * @param[out]  state       The state object to fill.
 * @return                  An indicator whether the call was successful.
 */
apalExitStatus_t mic9404x_lld_get(const MIC9404xDriver* const mic9404x, mic9404x_lld_state_t* const state)
{
  apalDbgAssert(mic9404x != NULL);
  apalDbgAssert(state != NULL);

  apalControlGpioState_t gpio_state;
  apalExitStatus_t status = apalControlGpioGet(mic9404x->en, &gpio_state);
  *state = gpio_state == APAL_GPIO_ON ? MIC9404x_LLD_STATE_ON : MIC9404x_LLD_STATE_OFF;
  return status;
}

/** @} */

