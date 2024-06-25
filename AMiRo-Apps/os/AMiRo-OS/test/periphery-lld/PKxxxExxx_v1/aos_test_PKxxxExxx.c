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
 * @file    aos_test_PKxxxExxx.c
 * @brief   Buzzer driver test types implementation.
 *
 * @addtogroup test_PKxxxExxx_v1
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "aos_test_PKxxxExxx.h"

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

aos_testresult_t aosTestPkxxxexxxFunc(BaseSequentialStream* stream, const aos_test_t* test)
{
  aosDbgCheck(test->data != NULL && ((aos_test_pkxxxexxxdata_t*)test->data)->driver != NULL);

  // local variables
  aos_testresult_t result;
  int32_t status;

  aosTestResultInit(&result);

  chprintf(stream, "checking PWM configuration...\n");
  status = pkxxxexxx_lld_checkPWMconfiguration(((aos_test_pkxxxexxxdata_t*)test->data)->driver);
  if (status == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
    // abort at this point
    return result;
  }

  chprintf(stream, "buzzing for one second...\n");
  status = pkxxxexxx_lld_enable(((aos_test_pkxxxexxxdata_t*)test->data)->driver, ((aos_test_pkxxxexxxdata_t*)test->data)->channel, true);
  aosThdSSleep(1);
  status |= pkxxxexxx_lld_enable(((aos_test_pkxxxexxxdata_t*)test->data)->driver, ((aos_test_pkxxxexxxdata_t*)test->data)->channel, false);
  if (status == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
