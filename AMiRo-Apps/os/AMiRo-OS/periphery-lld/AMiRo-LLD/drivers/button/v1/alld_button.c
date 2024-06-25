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
 * @file    alld_button.c
 * @brief   Button function implementations.
 *
 * @addtogroup lld_button_v1
 * @{
 */

#include "alld_button.h"

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
 * @brief Get the current status of the button.
 * @param[in]   button  The button driver object.
 * @param[out]  state   The state object to fill.
 * @return              An indicator whether the call was successful.
 */
apalExitStatus_t button_lld_get(const ButtonDriver* const button, button_lld_state_t* const state)
{
  apalDbgAssert(button != NULL);
  apalDbgAssert(state != NULL);

  apalControlGpioState_t gpio_state;
  apalExitStatus_t status = apalControlGpioGet(button->gpio, &gpio_state);
  *state = gpio_state == APAL_GPIO_ON ? BUTTON_LLD_STATE_PRESSED : BUTTON_LLD_STATE_RELEASED;
  return status;
}

/** @} */

