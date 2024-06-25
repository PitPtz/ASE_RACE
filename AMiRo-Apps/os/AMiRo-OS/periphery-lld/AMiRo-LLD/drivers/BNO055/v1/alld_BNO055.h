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
 * @file    alld_bno055.h
 * @brief   IMU constants and structures.
 *
 * @defgroup lld_BNO055_v1 Version 1
 * @brief   Version 1
 * @ingroup lld_BNO055
 *
 * @addtogroup lld_BNO055_v1
 * @{
 */

#ifndef AMIROLLD_BNO055_H
#define AMIROLLD_BNO055_H

#include "boschsensortecdriver/bosch_bno055.h"
#include <amiro-lld.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief Maximum I2C frequency.
 */
#define BNO055_LLD_I2C_MAXFREQUENCY   400000

#if !defined BNO055_API
  #define BNO055_API
#endif /* BNO055_API */

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

typedef struct BNO055 {
  apalI2CDriver_t* i2cd;
  apalI2Caddr_t addr;   /* The address of the BNO055 for I2C communication */
  struct bno055_t bno;
}BNO055DRIVER;

#ifdef  BNO055_API

s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
s8 I2C_routine(BNO055DRIVER* bno055);
void BNO055_delay_msek(u32 msek);

#endif

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
  apalExitStatus_t bno055_lld_init(BNO055DRIVER* const bno, uint8_t acco_frequency, uint8_t gyro_frequency, uint8_t magno_frequency);
  apalExitStatus_t bno055_lld_set_mode(const uint8_t operation_mode);
  apalExitStatus_t bno055_lld_read_raw_gyro(int16_t* const gyro_data);
  apalExitStatus_t bno055_lld_read_converted_gyro(float* const gyro_data);
  apalExitStatus_t bno055_lld_read_raw_magno(int16_t* const magno_data);
  apalExitStatus_t bno055_lld_read_converted_magno(float* const magno_data);
  apalExitStatus_t bno055_lld_read_raw_acco(int16_t* const acco_data);
  apalExitStatus_t bno055_lld_read_converted_acco(float* const acco_data);
  apalExitStatus_t bno055_lld_read_linear_acco(float* const acco_data);
  apalExitStatus_t bno055_lld_read_gravity(float* const gravity_data);
#ifdef __cplusplus
}
#endif


#endif /* AMIROLLD_BNO055_H */

/** @} */

