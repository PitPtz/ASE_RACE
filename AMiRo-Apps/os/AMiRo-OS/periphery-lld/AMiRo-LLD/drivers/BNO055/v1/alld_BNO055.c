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
 * @file    alld_bno055.c
 * @brief   IMU function implementations.
 *
 * @addtogroup lld_BNO055_v1
 * @{
 */

#include <alld_BNO055.h>
#include <string.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

#if !defined BNO055I2CDRIVER
  #error "BNO055I2CDRIVER not defined in alldconf.h"
#endif

#if !defined BNO055I2CTIMEOUT
  #error "BNO055I2CTIMEOUT not defined in alldconf.h"
#endif

#if !defined BNO055_API
  #error "BNO055_API not defined in all_BNO055.h"
#endif

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

#ifdef  BNO055_API

s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
  u8 txbuf[cnt + 1]; //buffer containing data to send

  txbuf[0] = reg_addr;
  memcpy(&txbuf[1], reg_data, cnt);

  apalExitStatus_t status = apalI2CMasterTransmit(BNO055I2CDRIVER, (apalI2Caddr_t)dev_addr, txbuf, cnt + 1, NULL, 0, BNO055I2CTIMEOUT);

  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_SUCCESS) {
    return (s8)BNO055_SUCCESS;
  } else {
    return (s8)BNO055_ERROR;
  }
}

s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
  apalExitStatus_t status = apalI2CMasterTransmit(BNO055I2CDRIVER, (apalI2Caddr_t)dev_addr, &reg_addr, 1, reg_data, cnt, BNO055I2CTIMEOUT);

  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_SUCCESS) {
    return (s8)BNO055_SUCCESS;
  } else {
    return (s8)BNO055_ERROR;
  }
}

void BNO055_delay_msek(u32 msek)
{
  apalSleep(msek*1000);
  return;
}

s8 I2C_routine(BNO055DRIVER* bno055)
{
  bno055->bno.bus_write = BNO055_I2C_bus_write;
  bno055->bno.bus_read = BNO055_I2C_bus_read;
  bno055->bno.delay_msec = BNO055_delay_msek;
  bno055->bno.dev_addr = BNO055_I2C_ADDR1;

  return BNO055_INIT_VALUE;
}
#endif

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

/**
 * @brief Initialize the BNO055 Driver
 *
 * @param[in]   bno               BNO055 driver object.
 * @param[in]   acco_frequency    Acceleromenter sampling frequency.
 * @param[in]   gyro_frequency    Gyroscope sampling frequency.
 * @param[in]   magno_frequency   Magnetometer sampling frequency.
 *
 * @return                  An indicator whether the call was successfull
 */
apalExitStatus_t bno055_lld_init(BNO055DRIVER* const bno, uint8_t acco_frequency, uint8_t gyro_frequency, uint8_t magno_frequency) {
  // return value of communication routine
  int comres = BNO055_ERROR;
#ifdef  BNO055_API
    I2C_routine(bno);
#endif

  // initialize the bno
  comres = bno055_init(&bno->bno);

  // set the power mode as NORMAL
  comres += bno055_set_power_mode(BNO055_POWER_MODE_NORMAL);

  // Change the X and Y axis
  comres += bno055_set_axis_remap_value(BNO055_REMAP_X_Y);

  // set the accelerometer frequency
  comres += bno055_set_accel_bw(acco_frequency);

  // set the gyroscope frequency
  comres += bno055_set_gyro_bw(gyro_frequency);

  // set the magnetometer frequency
  comres += bno055_set_mag_data_output_rate(magno_frequency);

  if (comres == BNO055_SUCCESS) {
    return APAL_STATUS_OK;
  } else {
    return APAL_STATUS_FAILURE;
  }
}


/**
 * @brief Set the operation mode of the BNO055 driver
 * @param[in]   operation_mode    Operation mode. Possible Modi are:
 * operation_mode                       | comments
 * -----------------------------------------------------------------
 * BNO055_OPERATION_MODE_CONFIG         | Configuration mode
 * BNO055_OPERATION_MODE_ACCONLY        | Reads accel data alone
 * BNO055_OPERATION_MODE_MAGONLY        | Reads mag data alone
 * BNO055_OPERATION_MODE_GYRONLY        | Reads gyro data alone
 * BNO055_OPERATION_MODE_ACCMAG         | Reads accel and mag data
 * BNO055_OPERATION_MODE_ACCGYRO        | Reads accel and gyro data
 * BNO055_OPERATION_MODE_MAGGYRO        | Reads accel and mag data
 * OPERATION_MODE_ANY_MOTION            | Reads accel mag and gyro data
 * BNO055_OPERATION_MODE_IMUPLUS        | Inertial measurement unit
 *       -                              | Reads accel,gyro and fusion data
 * BNO055_OPERATION_MODE_COMPASS        | Reads accel, mag data
 *       -                              | and fusion data
 * BNO055_OPERATION_MODE_M4G            | Reads accel, mag data
 *       -                              | and fusion data
 * BNO055_OPERATION_MODE_NDOF_FMC_OFF   | Nine degrees of freedom with
 *       -                              | fast magnetic calibration
 *       -                              | Reads accel,mag, gyro
 *       -                              | and fusion data
 * BNO055_OPERATION_MODE_NDOF           | Nine degrees of freedom
 *       -                              | Reads accel,mag, gyro
 *       -                              | and fusion data
 * @return                  An indicator whether the call was successfull
 */
apalExitStatus_t bno055_lld_set_mode(uint8_t const operation_mode) {
  if (bno055_set_operation_mode(operation_mode) == BNO055_SUCCESS) {
    return APAL_STATUS_OK;
  } else {
    return APAL_STATUS_FAILURE;
  }
}

/**
 * @brief Read the raw Gyroscope data of the BNO055
 * @param[in]   gyro_data        Gyroscope data struct
 * @return                       An indicator whether the call was successfull
 */
apalExitStatus_t bno055_lld_read_raw_gyro(int16_t* const gyro_data) {
  if (bno055_read_gyro_xyz((struct bno055_gyro_t*)gyro_data) == BNO055_SUCCESS) {
    return APAL_STATUS_OK;
  } else {
    return APAL_STATUS_FAILURE;
  }
}

/**
 * @brief Read the Gyroscope data in RPS of the BNO055
 * @param[in]   gyro_data        Gyroscope data struct
 * @return                       An indicator whether the call was successfull
 */
apalExitStatus_t bno055_lld_read_converted_gyro(float* const gyro_data) {
  if (bno055_convert_float_gyro_xyz_rps((struct bno055_gyro_float_t*)gyro_data) == BNO055_SUCCESS) {
    return APAL_STATUS_OK;
  } else {
    return APAL_STATUS_FAILURE;
  }
}

/**
 * @brief Read the raw Magnetometer data of the BNO055
 * @param[in]   magno_data        Magnetometer data struct
 * @return                  An indicator whether the call was successfull
 */
apalExitStatus_t bno055_lld_read_raw_magno(int16_t* const magno_data) {
  if (bno055_read_mag_xyz((struct bno055_mag_t*)magno_data) == BNO055_SUCCESS) {
    return APAL_STATUS_OK;
  } else {
    return APAL_STATUS_FAILURE;
  }
}

/**
 * @brief Read the Magnetometer data in μT the BNO055
 * @param[in]   magno_data        Magnetometer data struct
 * @return                  An indicator whether the call was successfull
 */
apalExitStatus_t bno055_lld_read_converted_magno(float* const magno_data) {
  if (bno055_convert_float_mag_xyz_uT((struct bno055_mag_float_t*)magno_data) == BNO055_SUCCESS) {
    return APAL_STATUS_OK;
  } else {
    return APAL_STATUS_FAILURE;
  }
}

/**
 * @brief Read the raw Accelerometer data of the BNO055
 * @param[in]   acco_data        Accelerometer data struct
 * @return                  An indicator whether the call was successfull
 */
apalExitStatus_t bno055_lld_read_raw_acco(int16_t* const acco_data) {
  if (bno055_read_accel_xyz((struct bno055_accel_t*)acco_data) == BNO055_SUCCESS) {
    return APAL_STATUS_OK;
  } else {
    return APAL_STATUS_FAILURE;
  }
}


/**
 * @brief Read the Accelerometer data in m/s^2 of the BNO055
 * @param[in]   acco_data        Accelerometer data struct
 * @return                  An indicator whether the call was successfull
 */
apalExitStatus_t bno055_lld_read_converted_acco(float* const acco_data) {
  if (bno055_convert_float_accel_xyz_msq((struct bno055_accel_float_t*)acco_data) == BNO055_SUCCESS) {
    return APAL_STATUS_OK;
  } else {
    return APAL_STATUS_FAILURE;
  }
}

/**
 * @brief Read the Linear Accelerometer data in m/s^2 of the BNO055
 * @param[in]   acco_data        Accelerometer data struct
 * @return                  An indicator whether the call was successfull
 */
apalExitStatus_t bno055_lld_read_linear_acco(float* const acco_data) {
  if (bno055_convert_float_linear_accel_xyz_msq((struct bno055_linear_accel_float_t*)acco_data) == BNO055_SUCCESS) {
    return APAL_STATUS_OK;
  } else {
    return APAL_STATUS_FAILURE;
  }
}

/**
 * @brief Read the Gravity data in m/s^2 of the BNO055
 * @param[in]   gravity_data        Gravity data struct
 * @return                  An indicator whether the call was successfull
 */
apalExitStatus_t bno055_lld_read_gravity(float* const gravity_data) {
  if (bno055_convert_float_gravity_xyz_msq((struct bno055_gravity_float_t*)gravity_data) == BNO055_SUCCESS) {
    return APAL_STATUS_OK;
  } else {
    return APAL_STATUS_FAILURE;
  }
}

/** @} */

