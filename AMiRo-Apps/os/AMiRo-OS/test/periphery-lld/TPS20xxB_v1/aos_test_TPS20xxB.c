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
 * @file    aos_test_TPS20xxB.c
 * @brief   Power-Distribution switch driver test types implementation.
 *
 * @addtogroup test_TPS20xxB_v1
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "aos_test_TPS20xxB.h"

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
 * @brief   TPS20xxB test function.
 *
 * @param[in] stream  Stream for input/output.
 * @param[in] test    Test object.
 *
 * @return            Test result value.
 */
aos_testresult_t aosTestTps20xxbFunc(BaseSequentialStream* stream, const aos_test_t* test)
{
  aosDbgCheck(test->data != NULL && ((aos_test_tps20xxbdata_t*)test->data)->driver != NULL);

  // local variables
  aos_testresult_t result;
  int32_t status;
  tps20xxb_lld_enable_t en;
  tps20xxb_lld_overcurrent_t oc;

  aosTestResultInit(&result);

  chprintf(stream, "write laser enable...\n");
  status = tps20xxb_lld_set_enable(((aos_test_tps20xxbdata_t*)test->data)->driver, TPS20xxB_LLD_ENABLE);
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailed(stream, &result);
  }

  chprintf(stream, "read laser enable...\n");
  status = tps20xxb_lld_read_enable(((aos_test_tps20xxbdata_t*)test->data)->driver, &en);
  if (status == APAL_STATUS_SUCCESS && en == TPS20xxB_LLD_ENABLE) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailed(stream, &result);
  }

  chprintf(stream, "read laser oc...\n");
  status = tps20xxb_lld_read_overcurrent(((aos_test_tps20xxbdata_t*)test->data)->driver, &oc);
  if (status == APAL_STATUS_SUCCESS && oc == TPS20xxB_LLD_NO_OVERCURRENT) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailed(stream, &result);
  }

  chprintf(stream, "disable laser...\n");
  status = tps20xxb_lld_set_enable(((aos_test_tps20xxbdata_t*)test->data)->driver, TPS20xxB_LLD_DISABLE);
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailed(stream, &result);
  }

  aosTestInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(TPS20xxBDriver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
