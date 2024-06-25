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
 * @file    aos_test_LED.c
 * @brief   Generic LED driver test types implementation.
 *
 * @addtogroup test_LED_v1
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "aos_test_LED.h"

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
 * @brief   LED test function.
 *
 * @param[in] stream  Stream for input/output.
 * @param[in] test    Test object.
 *
 * @return            Test result value.
 */
aos_testresult_t aosTestLedFunc(BaseSequentialStream* stream, const aos_test_t* test)
{
  aosDbgCheck(test->data != NULL && ((aos_test_leddata_t*)test->data)->led != NULL);

  // local variables
  aos_testresult_t result;
  int32_t status;

  aosTestResultInit(&result);

  chprintf(stream, "lighting up for two seconds...\n");
  led_lld_state_t state = LED_LLD_STATE_ON;
  status = led_lld_set(((aos_test_leddata_t*)test->data)->led, state);
  aosThdSSleep(2);
  status |= led_lld_get(((aos_test_leddata_t*)test->data)->led, &state);
  if (status == APAL_STATUS_OK && state == LED_LLD_STATE_ON) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailed(stream, &result);
  }

  chprintf(stream, "turning off for two seconds...\n");
  state = LED_LLD_STATE_OFF;
  status = led_lld_set(((aos_test_leddata_t*)test->data)->led, state);
  aosThdSSleep(2);
  status |= led_lld_get(((aos_test_leddata_t*)test->data)->led, &state);
  if (status == APAL_STATUS_OK && state == LED_LLD_STATE_OFF) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailed(stream, &result);
  }

  chprintf(stream, "toggling for two seconds...\n");
  status = led_lld_set(((aos_test_leddata_t*)test->data)->led, LED_LLD_STATE_ON);
  for (uint32_t i = 0; i < 2000/100; ++i) {
    status |= led_lld_toggle(((aos_test_leddata_t*)test->data)->led);
    status |= led_lld_get(((aos_test_leddata_t*)test->data)->led, &state);
    status |= ((uint8_t)state != (i & 0x01u)) ? APAL_STATUS_ERROR : APAL_STATUS_OK;
    aosThdMSleep(100);
  }
  status = led_lld_set(((aos_test_leddata_t*)test->data)->led, LED_LLD_STATE_OFF);
  if (status == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailed(stream, &result);
  }

  aosTestInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(LEDDriver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
