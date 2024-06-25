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
 * @file    aos_test_MPU6050.h
 * @brief   IMU driver test types and function declarations.
 *
 * @defgroup test_MPU6050_v1 MPU6050 (v1)
 * @ingroup test
 * @brief   IMU driver test.
 *
 * @addtogroup test_MPU6050_v1
 * @{
 */

#ifndef AMIROOS_TEST_MPU6050_H
#define AMIROOS_TEST_MPU6050_H

#include <aosconf.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include <alld_MPU6050.h>
#include "core/inc/aos_test.h"

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

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
 * @brief   Custom data structure for the test.
 */
typedef struct {
  /**
   * @brief   MPU6050 driver to use.
   */
  MPU6050Driver *mpu;

  /**
   * @brief   Timeout delay for certain tests.
   */
  apalTime_t timeout;
} aos_test_mpu6050data_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  aos_testresult_t aosTestMpu6050Func(BaseSequentialStream* stream, const aos_test_t* test);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

#endif /* AMIROOS_TEST_MPU6050_H */

/** @} */
