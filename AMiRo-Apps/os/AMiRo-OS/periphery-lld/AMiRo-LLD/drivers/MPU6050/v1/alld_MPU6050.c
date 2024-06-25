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
 * @file    alld_MPU6050.c
 * @brief   Gyro & Accelerometer implementation.
 *
 * @addtogroup lld_MPU6050_v1
 * @{
 */

#include "alld_MPU6050.h"

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
 * @brief Read the value of one or more of the registers.
 * @param[in]   mpu6050     MPU6050 driver
 * @param[in]   addr        register address
 * @param[out]  data        register content
 * @param[in]   num         number of subsequent registers to read
 * @param[in]   timeout     timeout
 * @return                  An indicator whether the call was successfull
 */
apalExitStatus_t mpu6050_lld_read_register(const MPU6050Driver* const mpu6050, const mpu6050_lld_register_t addr, uint8_t* const data, const uint8_t num, const apalTime_t timeout)
{
  apalDbgAssert(mpu6050 != NULL);
  apalDbgAssert(mpu6050->i2cd != NULL);
  apalDbgAssert(data != NULL || num == 0);

  return apalI2CMasterTransmit(mpu6050->i2cd, (MPU6050_LLD_I2C_ADDR_FIXED | mpu6050->addr), (uint8_t*)&addr, 1, data, num, timeout);
}

/**
 * @brief Write the value of one or more of the registers.
 * @param[in]   mpu6050     MPU6050 driver
 * @param[in]   addr        register address
 * @param[in]   data        data to write
 * @param[in]   num         number of subsequent registers to read
 * @param[in]   timeout     timeout
 * @return                  An indicator whether the call was successfull
 */
apalExitStatus_t mpu6050_lld_write_register(const MPU6050Driver* const mpu6050, const mpu6050_lld_register_t addr, const uint8_t* const data, const uint8_t num, const apalTime_t timeout)
{
  apalDbgAssert(mpu6050 != NULL);
  apalDbgAssert(mpu6050->i2cd != NULL);
  apalDbgAssert(data != NULL || num == 0);

  uint8_t buffer[1+num];
  buffer[0] = addr;
  memcpy(&buffer[1], data, num);
  return apalI2CMasterTransmit(mpu6050->i2cd, (MPU6050_LLD_I2C_ADDR_FIXED | mpu6050->addr), buffer, 1+num, NULL, 0, timeout);
}

/** @} */
