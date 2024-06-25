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
 * @file    alld_PKxxxExxx.c
 * @brief   Buzzer function implementations.
 *
 * @addtogroup lld_PKxxxExxx_v1
 * @{
 */

#include "alld_PKxxxExxx.h"

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
 * @brief Check the configuration of the PWM for the correct frequency.
 * @param[in] pwm   The PWM driver to check.
 * @return  The return status indicates whether the function call was successful.
 */
apalExitStatus_t pkxxxexxx_lld_checkPWMconfiguration(apalPWMDriver_t* pwm)
{
  apalDbgAssert(pwm != NULL);

  apalPWMfrequency_t frequency = 0;
  apalPWMperiod_t period = 0;
  apalExitStatus_t status = apalPWMGetFrequency(pwm, &frequency);
  status |= apalPWMGetPeriod(pwm, &period);
  if (frequency / period == PKxxxExxx_LLD_FREQUENCY_SPEC) {
    status |= APAL_STATUS_OK;
  }
#if defined(PKxxxExxx_LLD_FREQUENCY_MIN) && !defined(PKxxxExxx_LLD_FREQUENCY_MAX)
  else if (frequency / period >= PKxxxExxx_LLD_FREQUENCY_MIN &&
           frequency / period <= PKxxxExxx_LLD_FREQUENCY_SPEC) {
    status |= APAL_STATUS_INVALIDARGUMENTS;
  }
#elif !defined(PKxxxExxx_LLD_FREQUENCY_MIN) && defined(PKxxxExxx_LLD_FREQUENCY_MAX)
  else if (frequency / period >= PKxxxExxx_LLD_FREQUENCY_SPEC &&
           frequency / period <= PKxxxExxx_LLD_FREQUENCY_MAX) {
    status |= APAL_STATUS_INVALIDARGUMENTS;
  }
#elif defined(PKxxxExxx_LLD_FREQUENCY_MIN) && defined(PKxxxExxx_LLD_FREQUENCY_MAX)
  else if (frequency / period >= PKxxxExxx_LLD_FREQUENCY_MIN &&
           frequency / period <= PKxxxExxx_LLD_FREQUENCY_MAX) {
    status |= APAL_STATUS_INVALIDARGUMENTS;
  }
#endif
  else {
    status |= APAL_STATUS_ERROR | APAL_STATUS_INVALIDARGUMENTS;
  }

  return status;
}

/**
 * @brief Turns the buzzer on or off.
 * @param[in] pwm       The PWM driver to use
 * @param[in] channel   The PWM channel to enable/disable.
 * @param[in] enable    Specifies whether the buzzer shall be turned on (true) or off (false).
 * @return  The return status indicates whether the function call was successful.
 */
apalExitStatus_t pkxxxexxx_lld_enable(apalPWMDriver_t* pwm, const apalPWMchannel_t channel, const bool enable)
{
  apalDbgAssert(pwm != NULL);

  if (enable) {
    apalPWMperiod_t period = 0;
    apalExitStatus_t status = apalPWMGetFrequency(pwm, &period);
    status |= apalPWMSet(pwm, channel, period/2);
    return status;
  } else {
    return apalPWMSet(pwm, channel, 0);
  }
}

/** @} */

