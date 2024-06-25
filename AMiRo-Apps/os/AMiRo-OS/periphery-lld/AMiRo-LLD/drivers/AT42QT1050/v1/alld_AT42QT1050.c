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
 * @file    alld_AT42QT1050.c
 * @brief   Touch sensor function implementations.
 *
 * @addtogroup lld_AT42QT1050_v1
 * @{
 */

#include "alld_AT42QT1050.h"

#include <math.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/
#define AT42QT1050_LLD_WATCHDOGTIME_MAX         125000
#define AT42QT1050_LLD_INITIALIZATION_TIME_MAX   30000

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
 * @brief   Read 8bit data from any register.
 *
 * @param[in]   at42qt1050d   The AT42QT1050 driver to use.
 * @param[in]   reg           Register address to read from.
 * @param[out]  data          Pointer to store the register data to.
 * @param[in]   timeout       Timeout for the function to return (in microseconds).
 *
 * @return    Indicator whether the function call was successful or a timeout occurred.
 */
apalExitStatus_t at42qt1050_lld_read_reg(const AT42QT1050Driver* at42qt1050d, const at42qt1050_lld_register_t reg, uint8_t* const data, const apalTime_t timeout)
{
  apalDbgAssert(at42qt1050d != NULL && at42qt1050d->i2cd != NULL);
  apalDbgAssert(data != NULL);

  const uint8_t txbuf = (uint8_t)reg;
  return apalI2CMasterTransmit(at42qt1050d->i2cd, at42qt1050d->addr, &txbuf, 1, data, 1, timeout);
}

/**
 * @brief   Write 8bit data to any (writable) register.
 *
 * @param[in]   at42qt1050d   The AT42QT1050 driver to use.
 * @param[in]   reg           Register address to write to.
 * @param[in]   data          Data to transmit.
 * @param[in]   timeout       Timeout for the function to return (in microseconds).
 *
 * @return    Indicator whether the function call was successful or a timeout occurred.
 */
apalExitStatus_t at42qt1050_lld_write_reg(const AT42QT1050Driver* at42qt1050d, const at42qt1050_lld_register_t reg, const uint8_t data, const apalTime_t timeout)
{
  apalDbgAssert(at42qt1050d != NULL && at42qt1050d->i2cd != NULL);

  const uint8_t txbuf[2] = { (uint8_t)reg, data };
  return apalI2CMasterTransmit(at42qt1050d->i2cd, at42qt1050d->addr, txbuf, 2, NULL, 0, timeout);
}

/**
 * @brief   Read signal information of a key.
 *
 * @param[in]   at42qt1050d   The AT42QT1050 driver to use.
 * @param[in]   key           Key to read the signal information of.
 * @param[out]  signal        Pointer to store the data to.
 * @param[in]   timeout       Timeout for the function to return (in microseconds).
 *
 * @return    Indicator whether the function call was successful or a timeout occurred.
 */
apalExitStatus_t at42qt1050_lld_read_keyssignal(const AT42QT1050Driver* at42qt1050d, const uint8_t key, uint16_t* signal, const apalTime_t timeout)
{
  apalDbgAssert(at42qt1050d != NULL && at42qt1050d->i2cd != NULL);
  apalDbgAssert(key < AT42QT1050_LLD_NUM_KEYS);
  apalDbgAssert(signal != NULL);

  const at42qt1050_lld_register_t txbuf = at42qt1050_lld_addr_calc(AT42QT1050_LLD_REG_KEYSIGNAL_0, key);
  uint8_t rxbuf[2];
  const apalExitStatus_t status = apalI2CMasterTransmit(at42qt1050d->i2cd, at42qt1050d->addr, &txbuf, 1, rxbuf, 2, timeout);
  *signal = (rxbuf[0] << 8) | rxbuf[1];
  return status;
}

/**
 * @brief   Read reference data of a key.
 *
 * @param[in]   at42qt1050d   The AT42QT1050 driver to use.
 * @param[in]   key           Key to read the signal information of.
 * @param[out]  refdata       Pointer to store the data to.
 * @param[in]   timeout       Timeout for the function to return (in microseconds).
 *
 * @return    Indicator whether the function call was successful or a timeout occurred.
 */
apalExitStatus_t at42qt1050_lld_read_referencedata(const AT42QT1050Driver* at42qt1050d, const uint8_t key, uint16_t* refdata, const apalTime_t timeout)
{
  apalDbgAssert(at42qt1050d != NULL && at42qt1050d->i2cd != NULL);
  apalDbgAssert(key < AT42QT1050_LLD_NUM_KEYS);
  apalDbgAssert(refdata != NULL);

  const at42qt1050_lld_register_t txbuf = at42qt1050_lld_addr_calc(AT42QT1050_LLD_REG_REFERENCEDATA_0, key);
  uint8_t rxbuf[2];
  const apalExitStatus_t status = apalI2CMasterTransmit(at42qt1050d->i2cd, at42qt1050d->addr, &txbuf, 1, rxbuf, 2, timeout);
  *refdata = (rxbuf[0] << 8) | rxbuf[1];
  return status;
}

/**
 * @brief       Soft Reset of the device
 *
 * @param[in]   at42qt1050d   The AT42QT1050 driver to use.
 * @param[in]   timeout       Timeout for the function to return (in microseconds).
 * @param[in]   wait4wakeup   Wait for device wakeup (timeout must be > 155 ms)
 *
 * @return      Indicator whether the function call was successful or a timeout occurred.
 */
inline apalExitStatus_t at42qt1050_lld_reset_safe(const AT42QT1050Driver* at42qt1050d, const bool wait4wakeup, const apalTime_t timeout) {
  if(wait4wakeup)
    apalDbgAssert(timeout >= AT42QT1050_LLD_WATCHDOGTIME_MAX+AT42QT1050_LLD_INITIALIZATION_TIME_MAX);

  return at42qt1050_lld_reset(at42qt1050d, timeout-(AT42QT1050_LLD_WATCHDOGTIME_MAX+AT42QT1050_LLD_INITIALIZATION_TIME_MAX), wait4wakeup);
}

/**
 * @brief       Soft Reset of the device
 *
 * @param[in]   at42qt1050d   The AT42QT1050 driver to use.
 * @param[in]   timeout       Timeout for the i2c call (in microseconds).
 * @param[in]   wait4wakeup   Wait for device wakeup (155 ms)
 *
 * @return      Indicator whether the function call was successful or a timeout occurred.
 */
inline apalExitStatus_t at42qt1050_lld_reset(const AT42QT1050Driver* at42qt1050d, const apalTime_t timeout, const bool wait4wakeup) {
  apalDbgAssert(at42qt1050d != NULL && at42qt1050d->i2cd != NULL);

  const apalExitStatus_t status = at42qt1050_lld_write_reg(
    at42qt1050d, AT42QT1050_LLD_REG_RESET_CALIBRATE, AT42QT1050_LLD_RESETCALIBRATE_RESET, timeout);
  if(wait4wakeup)
    apalSleep(AT42QT1050_LLD_WATCHDOGTIME_MAX+AT42QT1050_LLD_INITIALIZATION_TIME_MAX+timeout); // watchdog timer+initialization -> datasheet
  return status;
}

/**
 * @brief   Convert a 4 bit pulse value to the representing number of samples.
 * @details Calculation: <#samples> = 2^(<pulse value>)
 *
 * @param[in]   pulse   Pulse value.
 *
 * @return    Resulting sample count.
 */
uint16_t at42qt1050_lld_pulse2samples(const uint8_t pulse)
{
  apalDbgAssert(pulse <= 0x0Fu);

  return (1 << pulse);
}

/**
 * @brief   Convert a desired number of samples to the according (theoretical) pulse value.
 * @details Calculation: <pulse value> = log2(<#samples>)
 *
 * @param[in]   samples   Desired number of samples.
 *
 * @return    The (theoretical) value to set to the pulse register.
 */
float at42qt1050_lld_samples2pulse(const uint16_t samples)
{
  return log2f(samples);
}

/**
 * @brief   Convert a 4 bit scale value to the accoring scaling factor.
 * @details Calculation: <scaling factor> = 2^(<scale value>)
 *
 * @param[in]   scale   Scale value.
 *
 * @return    Resulting scaling factor.
 */
uint16_t at42qt1050_lld_scale2scaling(const uint8_t scale)
{
  apalDbgAssert(scale <= 0x0Fu);

  return (1 << scale);
}

/**
 * @brief   Convert a desired scaling factor to the according (theoretical) scale value.
 * @details Calculation: <scale value> = log2(<scaling factor>
 * )
 * @param[in]   factor    Desired scaling factor.
 *
 * @return    The (theoretcial) value to set to the scale register.
 */
float at42qt1050_lld_scaling2scale(const uint16_t factor)
{
  return log2f(factor);
}

/** @} */

