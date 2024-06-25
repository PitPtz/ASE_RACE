/*
AMiRo-OS is an operating system designed for the Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2016..2022  Thomas Schöpping et al.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU (Lesser) General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU (Lesser) General Public License for more details.

You should have received a copy of the GNU (Lesser) General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    aos_test_BNO055.c
 * @brief   IMU driver test types implementation.
 *
 * @addtogroup test_BNO055_v1
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "aos_test_BNO055.h"

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


aos_testresult_t aosTestBno055Func(BaseSequentialStream* stream, const aos_test_t* test)
{
  aosDbgCheck((test->data != NULL) &&
              ((aos_test_bno055_t*)(test->data))->bno055 != NULL);

  // local variables
  aos_testresult_t result;
  int32_t status;
  int16_t data[3] = {0};
  float converted_data[3] = {0};

  aosTestResultInit(&result);

  chprintf(stream, "Initialize the BNO055...\n");
  status = bno055_lld_init(((aos_test_bno055_t*)(test->data))->bno055, BNO055_ACCEL_BW_62_5HZ, BNO055_GYRO_BW_32HZ, BNO055_MAG_DATA_OUTRATE_10HZ);
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "Set the operation mode to BNO055_OPERATION_MODE_AMG...\n");
  status = APAL_STATUS_OK;
  status = bno055_lld_set_mode(BNO055_OPERATION_MODE_AMG);
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "Reading raw gyroscope data for ten seconds...\n");
  status = APAL_STATUS_OK;
  for (uint8_t i = 0; i < 10; ++i) {
    aosThdSSleep(1);
    status |= bno055_lld_read_raw_gyro(data);
    chprintf(stream, "\tx: %6i\ty: %6i\tz: %6i\n", data[0], data[1], data[2]);
  }
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "Reading gyroscope data in rps for ten seconds...\n");
  status = APAL_STATUS_OK;
  for (uint8_t i = 0; i < 10; ++i) {
    aosThdSSleep(1);
    status |= bno055_lld_read_converted_gyro(converted_data);
    chprintf(stream, "\tx: %6.3f\ty: %6.3f\tz: %6.3f\n", converted_data[0], converted_data[1], converted_data[2]);
  }
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "Reading raw magnetometer data for ten seconds...\n");
  status = APAL_STATUS_OK;
  for (uint8_t i = 0; i < 10; ++i) {
    aosThdSSleep(1);
    status |= bno055_lld_read_raw_magno(data);
    chprintf(stream, "\tx: %6i\ty: %6i\tz: %6i\n", data[0], data[1], data[2]);
  }
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "Reading magnetometer data in μT for ten seconds...\n");
  status = APAL_STATUS_OK;
  for (uint8_t i = 0; i < 10; ++i) {
    aosThdSSleep(1);
    status |= bno055_lld_read_converted_magno(converted_data);
    chprintf(stream, "\tx: %6.3f\ty: %6.3f\tz: %6.3f\n", converted_data[0], converted_data[1], converted_data[2]);
  }
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "Reading raw accelerometer data for ten seconds...\n");
  status = APAL_STATUS_OK;
  for (uint8_t i = 0; i < 10; ++i) {
    aosThdSSleep(1);
    status |= bno055_lld_read_raw_acco(data);
    chprintf(stream, "\tx: %6i\ty: %6i\tz: %6i\n", data[0], data[1], data[2]);
  }
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "Reading accelerometer data in m/s^2 for ten seconds...\n");
  status = APAL_STATUS_OK;
  for (uint8_t i = 0; i < 10; ++i) {
    aosThdSSleep(1);
    status |= bno055_lld_read_converted_acco(converted_data);
    chprintf(stream, "\tx: %6.3f\ty: %6.3f\tz: %6.3f\n", converted_data[0], converted_data[1], converted_data[2]);
  }
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "Set the operation mode to BNO055_OPERATION_MODE_IMUPLUS...\n");
  chprintf(stream, "Therefore the relative orientation is calculated out of the accelerometer and gyroscope\n");
  status = APAL_STATUS_OK;
  status = bno055_lld_set_mode(BNO055_OPERATION_MODE_IMUPLUS);
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "Reading linear accelerometer data in m/s^2 for ten seconds...\n");
  status = APAL_STATUS_OK;
  for (uint8_t i = 0; i < 10; ++i) {
    aosThdSSleep(1);
    status |= bno055_lld_read_linear_acco(converted_data);
    chprintf(stream, "\tx: %6.3f\ty: %6.3f\tz: %6.3f\n", converted_data[0], converted_data[1], converted_data[2]);
  }
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "Reading gravity data in m/s^2 for ten seconds...\n");
  status = APAL_STATUS_OK;
  for (uint8_t i = 0; i < 10; ++i) {
    aosThdSSleep(1);
    status |= bno055_lld_read_gravity(converted_data);
    chprintf(stream, "\tx: %6.3f\ty: %6.3f\tz: %6.3f\n", converted_data[0], converted_data[1], converted_data[2]);
  }
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "Set the operation mode to BNO055_OPERATION_MODE_NDOF...\n");
  chprintf(stream, "Therefore the absolute orientation is calculated\n");
  status = APAL_STATUS_OK;
  status = bno055_lld_set_mode(BNO055_OPERATION_MODE_NDOF);
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "Reading linear accelerometer data in m/s^2 for ten seconds...\n");
  status = APAL_STATUS_OK;
  for (uint8_t i = 0; i < 10; ++i) {
    aosThdSSleep(1);
    status |= bno055_lld_read_linear_acco(converted_data);
    chprintf(stream, "\tx: %6.3f\ty: %6.3f\tz: %6.3f\n", converted_data[0], converted_data[1], converted_data[2]);
  }
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "Reading gravity data in m/s^2 for ten seconds...\n");
  status = APAL_STATUS_OK;
  for (uint8_t i = 0; i < 10; ++i) {
    aosThdSSleep(1);
    status |= bno055_lld_read_gravity(converted_data);
    chprintf(stream, "\tx: %6.3f\ty: %.6f\tz: %6.3f\n", converted_data[0], converted_data[1], converted_data[2]);
  }
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
