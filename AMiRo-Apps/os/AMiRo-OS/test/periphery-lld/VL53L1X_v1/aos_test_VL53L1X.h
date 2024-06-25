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
 * @file    aos_test_VL43L1X.h
 * @brief   Time-of-flight sensor driver test types and function declarations.
 *
 * @defgroup test_VL43L1X_v1 VL43L1X (v1)
 * @ingroup test
 * @brief   Time-of-flight sensor driver test.
 *
 * @addtogroup test_VL43L1X_v1
 * @{
 */

#ifndef AMIROOS_TEST_VL53L1X
#define AMIROOS_TEST_VL53L1X

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include <alld_VL53L1X.h>

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
   * @brief   Pointer to the driver to use.
   */
  VL53L1XDriver* vl53l1x;
  int tb;
  VL53L1_DistanceModes distanceMode;
  VL53L1_UserRoi_t roiConfig;
  int measurements;
  int calibrationRefDistmm;
} aos_test_vl53l1xdata_t;



/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  aos_testresult_t aosTestVL53L1XFunc(BaseSequentialStream* stream, const aos_test_t* test);
  aos_testresult_t aosTestVL53L1X_shortRangeModeInterrupt(BaseSequentialStream* stream, const aos_test_t* test);
  aos_testresult_t aosTestVL53L1X_setConfig(BaseSequentialStream* stream, const aos_test_t* test);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

#endif /* AMIROOS_TEST_VL53L1X */

/** @} */
