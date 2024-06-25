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
 * @file    alld_PCAL6524.c
 * @brief   GPIO extender function implementations.
 *
 * @addtogroup lld_PCAL6524_v1
 * @{
 */

#include "alld_PCAL6524.h"

#include <string.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

#define MAXIMUM_GROUP_SIZE                      6

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
 * @brief   Returns the size of the group for a specified command.
 *
 * @param[in] cmd   Command to retrieve the group size for.
 *
 * @return    The size of the group in bytes.
 */
uint8_t pcal6524_lld_cmd_groupsize(const pcal6524_lld_cmd_t cmd)
{
  switch (cmd) {
    case PCAL6524_LLD_CMD_OUTPUTCONFIGURATION:
    case PCAL6524_LLD_CMD_SWITCHDEBOUNCECOUNT:
      return 1;
    case PCAL6524_LLD_CMD_SWITCHDEBOUNCEENABLE_P0:
    case PCAL6524_LLD_CMD_SWITCHDEBOUNCEENABLE_P1:
      return 2;
    case PCAL6524_LLD_CMD_INPUT_P0:
    case PCAL6524_LLD_CMD_INPUT_P1:
    case PCAL6524_LLD_CMD_INPUT_P2:
    case PCAL6524_LLD_CMD_OUTPUT_P0:
    case PCAL6524_LLD_CMD_OUTPUT_P1:
    case PCAL6524_LLD_CMD_OUTPUT_P2:
    case PCAL6524_LLD_CMD_POLARITYINVERSION_P0:
    case PCAL6524_LLD_CMD_POLARITYINVERSION_P1:
    case PCAL6524_LLD_CMD_POLARITYINVERSION_P2:
    case PCAL6524_LLD_CMD_CONFIGURATION_P0:
    case PCAL6524_LLD_CMD_CONFIGURATION_P1:
    case PCAL6524_LLD_CMD_CONFIGURATION_P2:
    case PCAL6524_LLD_CMD_INPUTLATCH_P0:
    case PCAL6524_LLD_CMD_INPUTLATCH_P1:
    case PCAL6524_LLD_CMD_INPUTLATCH_P2:
    case PCAL6524_LLD_CMD_PUPDENABLE_P0:
    case PCAL6524_LLD_CMD_PUPDENABLE_P1:
    case PCAL6524_LLD_CMD_PUPDENABLE_P2:
    case PCAL6524_LLD_CMD_PUPDSELECTION_P0:
    case PCAL6524_LLD_CMD_PUPDSELECTION_P1:
    case PCAL6524_LLD_CMD_PUPDSELECTION_P2:
    case PCAL6524_LLD_CMD_INTERRUPTMASK_P0:
    case PCAL6524_LLD_CMD_INTERRUPTMASK_P1:
    case PCAL6524_LLD_CMD_INTERRUPTMASK_P2:
    case PCAL6524_LLD_CMD_INTERRUPTSTATUS_P0:
    case PCAL6524_LLD_CMD_INTERRUPTSTATUS_P1:
    case PCAL6524_LLD_CMD_INTERRUPTSTATUS_P2:
    case PCAL6524_LLD_CMD_INTERRUPTCLEAR_P0:
    case PCAL6524_LLD_CMD_INTERRUPTCLEAR_P1:
    case PCAL6524_LLD_CMD_INTERRUPTCLEAR_P2:
    case PCAL6524_LLD_CMD_INPUTSTATUS_P0:
    case PCAL6524_LLD_CMD_INPUTSTATUS_P1:
    case PCAL6524_LLD_CMD_INPUTSTATUS_P2:
    case PCAL6524_LLD_CMD_INDIVIDUALPINOUTPUTCONFIGURATION_P0:
    case PCAL6524_LLD_CMD_INDIVIDUALPINOUTPUTCONFIGURATION_P1:
    case PCAL6524_LLD_CMD_INDIVIDUALPINOUTPUTCONFIGURATION_P2:
      return 3;
    case PCAL6524_LLD_CMD_OUTPUTDRIVESTRENGTH_P0A:
    case PCAL6524_LLD_CMD_OUTPUTDRIVESTRENGTH_P0B:
    case PCAL6524_LLD_CMD_OUTPUTDRIVESTRENGTH_P1A:
    case PCAL6524_LLD_CMD_OUTPUTDRIVESTRENGTH_P1B:
    case PCAL6524_LLD_CMD_OUTPUTDRIVESTRENGTH_P2A:
    case PCAL6524_LLD_CMD_OUTPUTDRIVESTRENGTH_P2B:
    case PCAL6524_LLD_CMD_INTERRUPTEDGE_P0A:
    case PCAL6524_LLD_CMD_INTERRUPTEDGE_P0B:
    case PCAL6524_LLD_CMD_INTERRUPTEDGE_P1A:
    case PCAL6524_LLD_CMD_INTERRUPTEDGE_P1B:
    case PCAL6524_LLD_CMD_INTERRUPTEDGE_P2A:
    case PCAL6524_LLD_CMD_INTERRUPTEDGE_P2B:
      return 6;
  }
  // must never occur
  apalDbgAssert(false);
  return 0;
}

/*============================================================================*/
/* general register access                                                    */
/*============================================================================*/

// Reading the ID is currently not supported, since the required I²C sequence can not trivially executed via periphAL and ChibiOS/HAL.
///**
// * @brief   Read device ID and manufacturer information.
// *
// * @param[in]   pcal6524d   The PCAL6524 driver to use.
// * @param[out]  info        Pointer where to store the information data.
// * @param[in]   timeout     Timeout for the function to return.
// *
// * @return    Indicator whether the function call was successful or a timeout occurred.
// */
//apalExitStatus_t pcal6524_lld_read_id(const PCAL6524Driver* const pcal6524d, pcal6524_lld_deviceid_t* const info, const apalTime_t timeout)
//{
//  apalDbgAssert(pcal6524d != NULL && pcal6524d->i2cd != NULL);
//  apalDbgAssert(info != NULL);
//
//  const uint8_t txbuf = (uint8_t)pcal6524d->addr;
//  return apalI2CMasterTransmit(pcal6524d->i2cd, PCAL6524_LLD_I2C_ADDR_DEVICEID, &txbuf, 1, info->raw, sizeof(info->raw), timeout);
//}

/**
 * @brief   Read 8bit data from any register.
 *
 * @param[in]   pcal6524d       The PCAL6524 driver to use.
 * @param[in]   reg             Command register to read.
 * @param[out]  data            Pointer to store the register data to.
 * @param[in]   timeout         Timeout for the function to return (in microseconds).
 *
 * @return    Indicator whether the function call was successful or a timeout occurred.
 */
apalExitStatus_t pcal6524_lld_read_reg(const PCAL6524Driver* const pcal6524d, const pcal6524_lld_cmd_t reg, uint8_t* const data, const apalTime_t timeout)
{
  apalDbgAssert(pcal6524d != NULL && pcal6524d->i2cd != NULL);
  apalDbgAssert(data != NULL);

  const uint8_t txbuf = (uint8_t)reg;
  return apalI2CMasterTransmit(pcal6524d->i2cd, pcal6524d->addr, &txbuf, 1, data, 1, timeout);
}

/**
 * @brief   Write 8bit data to any (writable) register.
 *
 * @param[in]   pcal6524d       The PCAL6524 driver to use.
 * @param[in]   reg             Command register to write to.
 * @param[in]   data            Data to transmit.
 * @param[in]   timeout         Timout for the function to return (in microseconds).
 *
 * @return    Indicator whether the function call was successful or a timeout occurred.
 */
apalExitStatus_t pcal6524_lld_write_reg(const PCAL6524Driver* const pcal6524d, const pcal6524_lld_cmd_t reg, const uint8_t data, const apalTime_t timeout)
{
  apalDbgAssert(pcal6524d != NULL && pcal6524d->i2cd != NULL);

  uint8_t txbuf[2] = { (uint8_t)reg, data };
  return apalI2CMasterTransmit(pcal6524d->i2cd, pcal6524d->addr, txbuf, 2, NULL, 0, timeout);
}

/**
 * @brief   Read register group starting with an arbitrary register.
 *
 * @param[in]   pcal6524d   The PCAL6524 driver to use.
 * @param[in]   reg         Command register to start at.
 * @param[out]  data        Pointer to store rhe group data to.
 *                          The reqired number of bytes depends on the group.
 * @param[in]   timeout     Timeout for the function to return (in microseconds).
 *
 * @return    Indicator whether the function call was successful or a timeout occurred.
 */
apalExitStatus_t pcal6524_lld_read_group(const PCAL6524Driver* const pcal6524d, const pcal6524_lld_cmd_t reg, uint8_t* const data, const apalTime_t timeout)
{
  apalDbgAssert(pcal6524d != NULL && pcal6524d->i2cd != NULL);
  apalDbgAssert(data != NULL);

  const uint8_t txbuf = (uint8_t)reg;
  return apalI2CMasterTransmit(pcal6524d->i2cd, pcal6524d->addr, &txbuf, 1, data, pcal6524_lld_cmd_groupsize(reg), timeout);
}

/**
 * @brief   Write register group data to any (writable) register.
 *
 * @param[in]   pcal6524d       The PCAL6524 driver to use.
 * @param[in]   reg             Command register to start at.
 * @param[in]   data            Data to transmit.
 *                              The reqired number of bytes depends on the group.
 * @param[in]   timeout         Timout for the function to return (in microseconds).
 *
 * @return    Indicator whether the function call was successful or a timeout occurred.
 */
apalExitStatus_t pcal6524_lld_write_group(const PCAL6524Driver* const pcal6524d, const pcal6524_lld_cmd_t reg, const uint8_t* const data, const apalTime_t timeout)
{
  apalDbgAssert(pcal6524d != NULL && pcal6524d->i2cd != NULL);
  apalDbgAssert(data != NULL);

  uint8_t txbuf[MAXIMUM_GROUP_SIZE + 1] = {(uint8_t)reg};
  memcpy(&txbuf[1], data, pcal6524_lld_cmd_groupsize(reg));
  return apalI2CMasterTransmit(pcal6524d->i2cd, pcal6524d->addr, txbuf, pcal6524_lld_cmd_groupsize(reg) + 1, NULL, 0, timeout);
}

/**
 * @brief   Read multiple registers continously, starting at an arbitrary one.
 *
 * @param[in]   pcal6524d   The PCAL6524 driver to use.
 * @param[in]   reg         Command register to start at.
 * @param[out]  data        Pointer to store the group data to.
 * @param[in]   length      Number of registers/bytes to read.
 * @param[in]   timeout     Timeout for the function to return (in microseconds).
 *
 * @return    Indicator whether the function call was successful or a timeout occurred.
 */
apalExitStatus_t pcal6524_lld_read_continuous(const PCAL6524Driver* const pcal6524d, const pcal6524_lld_cmd_t reg, uint8_t* const data, const uint8_t length, const apalTime_t timeout)
{
  apalDbgAssert(pcal6524d != NULL && pcal6524d->i2cd != NULL);
  apalDbgAssert(data != NULL);
  apalDbgAssert(length <= PCAL6524_LLD_NUM_REGISTERS);

  uint8_t txbuf = (uint8_t)reg | PCAL6524_LLD_CMD_AUTOINCREMENT;
  return apalI2CMasterTransmit(pcal6524d->i2cd, pcal6524d->addr, &txbuf, 1, data, length, timeout);
}

/**
 * @brief   Write multiple registers continously, starting at an arbitrary one.
 *
 * @param[in]   pcal6524d   The PCAL6524 driver to use.
 * @param[in]   reg         Command register to start at.
 * @param[in]   data        Pointer to the data to write.
 * @param[in]   length      Number of registers/bytes to write.
 * @param[in]   timeout     Timeout for the function to return (in microseconds).
 *
 * @return    Indicator whether the function call was successful or a timeout occurred.
 */
apalExitStatus_t pcal6524_lld_write_continuous(const PCAL6524Driver* const pcal6524d, const pcal6524_lld_cmd_t reg, const uint8_t* const data, const uint8_t length, const apalTime_t timeout)
{
  apalDbgAssert(pcal6524d != NULL && pcal6524d->i2cd != NULL);
  apalDbgAssert(data != NULL);
  apalDbgAssert(length <= PCAL6524_LLD_NUM_REGISTERS);

  uint8_t txbuf[PCAL6524_LLD_NUM_REGISTERS + 1] = {(uint8_t)reg | PCAL6524_LLD_CMD_AUTOINCREMENT};
  memcpy(&txbuf[1], data, length);
  return apalI2CMasterTransmit(pcal6524d->i2cd, pcal6524d->addr, txbuf, length + 1, NULL, 0, timeout);
}

/** @} */

