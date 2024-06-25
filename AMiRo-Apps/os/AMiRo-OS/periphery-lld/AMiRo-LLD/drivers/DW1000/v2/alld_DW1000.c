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
 * @file    alld_DW1000.c
 * @brief   UWB transceiver function implementations.
 *
 * @addtogroup lld_DW1000_v2
 * @{
 */

#include "alld_DW1000.h"

#include "decadriver/deca_device_api.h"
#include <string.h>

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

int writetospi(uint16 headerLength, const uint8 *headerBuffer, uint32 bodylength, const uint8 *bodyBuffer)
{
  apalDbgAssert(dw1000.spid != NULL);
  apalDbgAssert(headerLength + bodylength <= DW1000_LLD_SPIBUFLEN);

  uint8_t buffer[DW1000_LLD_SPIBUFLEN];
  memcpy(&buffer[0], headerBuffer, headerLength);
  memcpy(&buffer[headerLength], bodyBuffer, bodylength);

  return (apalSPITransmit(dw1000.spid, buffer, headerLength + bodylength) == APAL_STATUS_OK) ? 0 : -1;
}

int readfromspi(uint16 headerLength, const uint8 *headerBuffer, uint32 readlength, uint8 *readBuffer)
{
  apalDbgAssert(dw1000.spid != NULL);

  return (apalSPITransmitAndReceive(dw1000.spid, headerBuffer, readBuffer, headerLength, readlength) == APAL_STATUS_OK) ? 0 : -1;
}

decaIrqStatus_t decamutexon(void)
{
  apalDbgAssert(dw1000.gpio_exti != NULL);

  bool enabled;
  apalGpioIsInterruptEnabled(dw1000.gpio_exti->gpio, &enabled);
  if (enabled) {
    apalControlGpioSetInterrupt(dw1000.gpio_exti, false);
  }

  return enabled;
}

void decamutexoff(decaIrqStatus_t s)
{
  apalDbgAssert(dw1000.gpio_exti != NULL);

  if (s) {
    apalControlGpioSetInterrupt(dw1000.gpio_exti, true);
  }

  return;
}

void deca_sleep(unsigned int time_ms)
{
  apalSleep((apalTime_t)time_ms * 1000);
  return;
}

/** @} */

