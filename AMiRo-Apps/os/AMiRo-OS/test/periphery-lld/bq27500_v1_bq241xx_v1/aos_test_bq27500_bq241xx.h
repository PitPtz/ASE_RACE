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
 * @file    aos_test_bq27500_bq241xx.h
 * @brief   Fuel gauge & battery charger drivers test types and function declarations.
 *
 * @defgroup test_bq27500_v1_bq241xx_v1 bq27500 (v1) & bq241xx (v1)
 * @ingroup test
 * @brief   Fuel gauge & battery charger drivers test.
 *
 * @addtogroup test_bq27500_v1_bq241xx_v1
 * @{
 */

#ifndef AMIROOS_TEST_BQ27500_BQ241xx_H
#define AMIROOS_TEST_BQ27500_BQ241xx_H

#include <aosconf.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include <alld_bq27500.h>
#include <alld_bq241xx.h>
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
   * @brief   BQ27500 driver to use.
   */
  BQ27500Driver* bq27500;

  /**
   * @brief   BQ24103A driver to use.
   */
  BQ241xxDriver* bq241xx;

  /**
   * @brief   Timeout value (in us).
   */
  apalTime_t timeout;
} aos_test_bq27500bq241xxdata_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  aos_testresult_t aosTestBq27500Bq241xxFunc(BaseSequentialStream* stream, const aos_test_t* test);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

#endif /* AMIROOS_TEST_BQ27500_BQ241xx_H */

/** @} */
