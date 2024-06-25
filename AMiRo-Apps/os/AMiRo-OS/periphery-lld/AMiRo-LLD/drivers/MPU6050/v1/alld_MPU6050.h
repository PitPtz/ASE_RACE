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
 * @file    alld_MPU6050.h
 * @brief   Gyro & Accelerometer macros and structures.
 *
 * @defgroup lld_MPU6050_v1 Version 1
 * @brief   Version 1
 * @ingroup lld_MPU6050
 *
 * @addtogroup lld_MPU6050_v1
 * @{
 */

#ifndef AMIROLLD_MPU6050_H
#define AMIROLLD_MPU6050_H

#include <amiro-lld.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief   Maximum I2C frequency.
 */
#define MPU6050_LLD_I2C_MAXFREQUENCY            400000

/**
 * @brief   Default I2C address
 */
#define MPU6050_LLD_I2C_ADDR_FIXED              0x68

/**
 * @brief   Optional I2C address mask, if AD0 is high.
 */
#define MPU6050_LLD_I2C_ADDR_AD0                0x01

/**
 * @brief   Constant value of the WHO_AM_I register.
 */
#define MPU6050_LLD_WHO_AM_I                    0x68

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

/**
 * @brief   Register map.
 */
typedef enum {
  MPU6050_LLD_REGISTER_SELF_TEST_X        = 0x0D,
  MPU6050_LLD_REGISTER_SELF_TEST_Y        = 0x0E,
  MPU6050_LLD_REGISTER_SELF_TEST_Z        = 0x0F,
  MPU6050_LLD_REGISTER_SELF_TEST_A        = 0x10,
  MPU6050_LLD_REGISTER_SMPLRT_DIV         = 0x19,
  MPU6050_LLD_REGISTER_CONFIG             = 0x1A,
  MPU6050_LLD_REGISTER_GYRO_CONFIG        = 0x1B,
  MPU6050_LLD_REGISTER_ACCEL_CONFIG       = 0x1C,
  MPU6050_LLD_REGISTER_FIFO_EN            = 0x23,
  MPU6050_LLD_REGISTER_I2C_MST_CTRL       = 0x24,
  MPU6050_LLD_REGISTER_I2C_SLV0_ADDR      = 0x25,
  MPU6050_LLD_REGISTER_I2C_SLV0_REG       = 0x26,
  MPU6050_LLD_REGISTER_I2C_SLV0_CTRL      = 0x27,
  MPU6050_LLD_REGISTER_I2C_SLV1_ADDR      = 0x28,
  MPU6050_LLD_REGISTER_I2C_SLV1_REG       = 0x29,
  MPU6050_LLD_REGISTER_I2C_SLV1_CTRL      = 0x2A,
  MPU6050_LLD_REGISTER_I2C_SLV2_ADDR      = 0x2B,
  MPU6050_LLD_REGISTER_I2C_SLV2_REG       = 0x2C,
  MPU6050_LLD_REGISTER_I2C_SLV2_CTRL      = 0x2D,
  MPU6050_LLD_REGISTER_I2C_SLV3_ADDR      = 0x2E,
  MPU6050_LLD_REGISTER_I2C_SLV3_REG       = 0x2F,
  MPU6050_LLD_REGISTER_I2C_SLV3_CTRL      = 0x30,
  MPU6050_LLD_REGISTER_I2C_SLV4_ADDR      = 0x31,
  MPU6050_LLD_REGISTER_I2C_SLV4_REG       = 0x32,
  MPU6050_LLD_REGISTER_I2C_SLV4_DO        = 0x33,
  MPU6050_LLD_REGISTER_I2C_SLV4_CTRL      = 0x34,
  MPU6050_LLD_REGISTER_I2C_SLV4_DI        = 0x35,
  MPU6050_LLD_REGISTER_I2C_MST_STATUS     = 0x36,
  MPU6050_LLD_REGISTER_INT_PIN_CFG        = 0x37,
  MPU6050_LLD_REGISTER_INT_ENABLE         = 0x38,
  MPU6050_LLD_REGISTER_INT_STATUS         = 0x3A,
  MPU6050_LLD_REGISTER_ACCEL_XOUT_H       = 0x3B,
  MPU6050_LLD_REGISTER_ACCEL_XOUT_L       = 0x3C,
  MPU6050_LLD_REGISTER_ACCEL_YOUT_H       = 0x3D,
  MPU6050_LLD_REGISTER_ACCEL_YOUT_L       = 0x3E,
  MPU6050_LLD_REGISTER_ACCEL_ZOUT_H       = 0x3F,
  MPU6050_LLD_REGISTER_ACCEL_ZOUT_L       = 0x40,
  MPU6050_LLD_REGISTER_TEMP_OUT_H         = 0x41,
  MPU6050_LLD_REGISTER_TEMP_OUT_L         = 0x42,
  MPU6050_LLD_REGISTER_GYRO_XOUT_H        = 0x43,
  MPU6050_LLD_REGISTER_GYRO_XOUT_L        = 0x44,
  MPU6050_LLD_REGISTER_GYRO_YOUT_H        = 0x45,
  MPU6050_LLD_REGISTER_GYRO_YOUT_L        = 0x46,
  MPU6050_LLD_REGISTER_GYRO_ZOUT_H        = 0x47,
  MPU6050_LLD_REGISTER_GYRO_ZOUT_L        = 0x48,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_00   = 0x49,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_01   = 0x4A,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_02   = 0x4B,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_03   = 0x4C,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_04   = 0x4D,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_05   = 0x4E,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_06   = 0x4F,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_07   = 0x50,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_08   = 0x51,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_09   = 0x52,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_10   = 0x53,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_11   = 0x54,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_12   = 0x55,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_13   = 0x56,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_14   = 0x57,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_15   = 0x58,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_16   = 0x59,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_17   = 0x5A,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_18   = 0x5B,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_19   = 0x5C,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_20   = 0x5D,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_21   = 0x5E,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_22   = 0x5F,
  MPU6050_LLD_REGISTER_EXT_SENS_DATA_23   = 0x60,
  MPU6050_LLD_REGISTER_I2C_SLV0_DO        = 0x63,
  MPU6050_LLD_REGISTER_I2C_SLV1_DO        = 0x64,
  MPU6050_LLD_REGISTER_I2C_SLV2_DO        = 0x65,
  MPU6050_LLD_REGISTER_I2C_SLV3_DO        = 0x66,
  MPU6050_LLD_REGISTER_I2C_MST_DELAY_CTRL = 0x67,
  MPU6050_LLD_REGISTER_SIGNAL_PATH_RESET  = 0x68,
  MPU6050_LLD_REGISTER_USER_CTRL          = 0x6A,
  MPU6050_LLD_REGISTER_PWR_MGMT_1         = 0x6B,
  MPU6050_LLD_REGISTER_PWR_MGMT_2         = 0x6C,
  MPU6050_LLD_REGISTER_FIFO_COUNTH        = 0x72,
  MPU6050_LLD_REGISTER_FIFO_COUNTL        = 0x73,
  MPU6050_LLD_REGISTER_FIFO_R_W           = 0x74,
  MPU6050_LLD_REGISTER_WHO_AM_I           = 0x75,
} mpu6050_lld_register_t;

/**
 * @brief The MPU6050Driver struct.
 */
typedef struct {
  apalI2CDriver_t* i2cd;  /**< Pointer to the connected I2C driver. */
  apalI2Caddr_t addr;     /**< The address of the module for I2C communication. */
} MPU6050Driver;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
  apalExitStatus_t mpu6050_lld_read_register(const MPU6050Driver* const mpu6050, const mpu6050_lld_register_t addr, uint8_t* const data, const uint8_t num, const apalTime_t timeout);
  apalExitStatus_t mpu6050_lld_write_register(const MPU6050Driver* const mpu6050, const mpu6050_lld_register_t addr, const uint8_t* const data, const uint8_t num, const apalTime_t timeout);
#ifdef __cplusplus
}
#endif

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROLLD_MPU6050_H */

/** @} */
