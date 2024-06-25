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
 * @file    alld_AT24C01B.c
 * @brief   EEPROM function implementations.
 *
 * @addtogroup lld_AT24C01B_v1
 * @{
 */

#include "alld_AT24C01B.h"

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

/**
 * @brief Acknowledgement polling to check if a write operation finished.
 * @param[in]   at24c01b  The eeprom driver to use.
 * @param[in]   timeout   Timeout for the function to return (in microseconds)
 *
 * @return  The return status indicates whether the function call was succesfull or a timeout occurred.
 */
apalExitStatus_t at24c01b_lld_poll_ack(const AT24C01BDriver* const at24c01b, const apalTime_t timeout)
{
  apalDbgAssert(at24c01b != NULL);

  uint8_t tmp = 0;
  return apalI2CMasterReceive(at24c01b->i2cd, (AT24C01B_LLD_I2C_ADDR_FIXED | at24c01b->addr), &tmp, 1, timeout);
}

/**
 * @brief Read the data from the eeprom starting at the current address.
 * @param[in]   at24c01b  The eeprom driver to use.
 * @param[out]  data      The data read from the AT24C01B.
 * @param[in]   num       The amount of bytes to read.
 * @param[in]   timeout   Timeout for the function to return (in microseconds)
 *
 * @return  The return status indicates whether the function call was succesfull or a timeout occurred.
 */
apalExitStatus_t at24c01b_lld_read_current_address(const AT24C01BDriver* const at24c01b, uint8_t* const data, const uint8_t num, const apalTime_t timeout)
{
  apalDbgAssert(at24c01b != NULL);
  apalDbgAssert(data != NULL);

  return apalI2CMasterReceive(at24c01b->i2cd, (AT24C01B_LLD_I2C_ADDR_FIXED | at24c01b->addr), data, num, timeout);
}

/**
 * @brief Read the data from the eeprom starting at a given address.
 * @param[in]   at24c01b  The eeprom driver to use.
 * @param[in]   addr      The address to read.
 * @param[out]  data      The data read from the AT24C01B.
 * @param[in]   num       The amount of bytes to read.
 * @param[in]   timeout   Timeout for the function to return (in microseconds)
 *
 * @return  The return status indicates whether the function call was succesfull or a timeout occurred.
 */
apalExitStatus_t at24c01b_lld_read(const AT24C01BDriver* const at24c01b, const uint8_t addr, uint8_t* const data, const uint8_t num, const apalTime_t timeout)
{
  apalDbgAssert(at24c01b != NULL);
  apalDbgAssert(data != NULL);
  apalDbgAssert(addr + num <= AT24C01B_LLD_SIZE_BYTES);

  apalExitStatus_t status = apalI2CMasterTransmit(at24c01b->i2cd, (AT24C01B_LLD_I2C_ADDR_FIXED | at24c01b->addr), &addr, 1, NULL, 0, timeout);
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    status |= apalI2CMasterReceive(at24c01b->i2cd, (AT24C01B_LLD_I2C_ADDR_FIXED | at24c01b->addr), data, num, timeout);
  }

  return status;
}

/**
 * @brief Write a byte to the eeprom.
 * @param[in]   at24c01b  The eeprom driver to use.
 * @param[in]   addr      The address to start writing.
 * @param[out]  data      The data to be written to the AT24C01BN.
 * @param[in]   timeout   Timeout for the function to return (in microseconds)
 *
 * @return  The return status indicates whether the function call was succesfull or a timeout occurred.
 */
apalExitStatus_t at24c01b_lld_write_byte(const AT24C01BDriver* const at24c01b, const uint8_t addr, const uint8_t data, const apalTime_t timeout)
{
  apalDbgAssert(at24c01b != NULL);

  const uint8_t writedata[2] = {addr, data};
  return apalI2CMasterTransmit(at24c01b->i2cd, (AT24C01B_LLD_I2C_ADDR_FIXED | at24c01b->addr), writedata, 2, NULL, 0, timeout);
}

/**
 * @brief Write a page to the eeprom.
 * @param[in]   at24c01b  The eeprom driver to use.
 * @param[in]   addr      The address to start writing.
 * @param[in]   data      The data to be written to the AT24C01BN.
 * @param[in]   num       The amount of bytes to write.
 * @param[in]   timeout   Timeout for the function to return (in microseconds)
 *
 * @return  The return status indicates whether the function call was succesfull or a timeout occurred.
 */
apalExitStatus_t at24c01b_lld_write_page(const AT24C01BDriver* const at24c01b, const uint8_t addr, const uint8_t* const data, const uint8_t num, const apalTime_t timeout)
{
  apalDbgAssert(at24c01b != NULL);
  apalDbgAssert(data != NULL);

  uint8_t writedata[num+1];
  writedata[0] = addr;
  memcpy(&writedata[1], data, num);
  return apalI2CMasterTransmit(at24c01b->i2cd, (AT24C01B_LLD_I2C_ADDR_FIXED | at24c01b->addr), writedata, num+1, NULL, 0, timeout);
}

/** @} */

