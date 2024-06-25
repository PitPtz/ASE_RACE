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
 * @file    aos_test_MIC9494x.c
 * @brief   Power switch driver test types implementation.
 *
 * @addtogroup test_MIC9494x_v1
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "aos_test_MIC9404x.h"

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
 * @brief   MIC9404x test function.
 *
 * @param[in] stream  Stream for input/output.
 * @param[in] test    Test object.
 *
 * @return            Test result value.
 */
aos_testresult_t aosTestMic9404xFunc(BaseSequentialStream* stream, const aos_test_t* test)
{
  aosDbgCheck(test->data != NULL && ((aos_test_mic9404data_t*)test->data)->driver != NULL);

  // local variables
  mic9404x_lld_state_t state_orig;
  mic9404x_lld_state_t state;
  aos_testresult_t result;
  int32_t status;

  aosTestResultInit(&result);

  chprintf(stream, "reading current status...\n");
  status = mic9404x_lld_get(((aos_test_mic9404data_t*)test->data)->driver, &state_orig);
  if (status == APAL_STATUS_OK) {
    aosTestPassedMsg(stream, &result, "power %s\n", (state_orig == MIC9404x_LLD_STATE_ON) ? "enabled" : "disabled");
  } else {
    aosTestFailed(stream, &result);
  }

  state = state_orig;
  for (uint8_t i = 0; i < 2; ++i) {
    if (state == MIC9404x_LLD_STATE_ON) {
      chprintf(stream, "disabling for five seconds...\n");
      status = mic9404x_lld_set(((aos_test_mic9404data_t*)test->data)->driver, MIC9404x_LLD_STATE_OFF);
      status |= mic9404x_lld_get(((aos_test_mic9404data_t*)test->data)->driver, &state);
      if (state == MIC9404x_LLD_STATE_OFF) {
        aosThdSSleep(5);
        status |= mic9404x_lld_get(((aos_test_mic9404data_t*)test->data)->driver, &state);
      }
      if ((status == APAL_STATUS_OK) && (state == MIC9404x_LLD_STATE_OFF)) {
        aosTestPassed(stream, &result);
      } else {
        aosTestFailed(stream, &result);
        break;
      }
    } else {
      chprintf(stream, "enabling for five seconds...\n");
      status = mic9404x_lld_set(((aos_test_mic9404data_t*)test->data)->driver, MIC9404x_LLD_STATE_ON);
      status |= mic9404x_lld_get(((aos_test_mic9404data_t*)test->data)->driver, &state);
      if (state == MIC9404x_LLD_STATE_ON) {
        aosThdSSleep(5);
        status |= mic9404x_lld_get(((aos_test_mic9404data_t*)test->data)->driver, &state);
      }
      if ((status == APAL_STATUS_OK) && (state == MIC9404x_LLD_STATE_ON)) {
        aosTestPassed(stream, &result);
      } else {
        aosTestFailed(stream, &result);
        break;
      }
    }
  }
  // restore original state
  if (state != state_orig) {
    status = mic9404x_lld_set(((aos_test_mic9404data_t*)test->data)->driver, state);
    if (status != APAL_STATUS_OK) {
      chprintf(stream, "\t\tWARNING: failed to restore original status!\n");
    }
  }

  aosTestInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(MIC9404xDriver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
