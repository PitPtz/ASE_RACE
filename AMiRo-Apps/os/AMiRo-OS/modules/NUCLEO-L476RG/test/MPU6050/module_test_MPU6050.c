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
 * @file    module_test_MPU6050.c
 *
 * @addtogroup nucleol476rg_test
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "module_test_MPU6050.h"
#include <aos_test_MPU6050.h>

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

static aos_test_mpu6050data_t _data = {
  /* driver   */ &moduleLldMpu6050,
  /* timeout  */ MICROSECONDS_PER_SECOND,
};

static AOS_TEST(_test, "MPU-6050", "Accelerometer & Gyroscope", moduleTestMpu6050ShellCb, aosTestMpu6050Func, &_data);

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup nucleol476rg_test
 * @{
 */

/**
 * @brief   MPU6050 test.
 *
 * @param[in]  stream   Stream to print output to.
 * @param[in]  argc     Number of arguments.
 * @param[in]  argv     Argument list
 * @param[out] result   Pointer to store the test result to.
 *                      May be NULL.
 *
 * @return  Status indicating the success of the function call.
 */
int moduleTestMpu6050ShellCb(BaseSequentialStream* stream, int argc, const char* argv[], aos_testresult_t* result)
{
  (void)argc;
  (void)argv;

  if (result != NULL) {
    *result = aosTestRun(stream, &_test, NULL);
  } else {
    aosTestRun(stream, &_test, NULL);
  }

  return AOS_OK;
}

/** @} */

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
