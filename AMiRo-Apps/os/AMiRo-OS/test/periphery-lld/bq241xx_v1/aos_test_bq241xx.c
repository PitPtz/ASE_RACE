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
 * @file    aos_test_bq241xx.c
 * @brief   MoBattery chargertor driver test types implementation.
 *
 * @addtogroup test_bq241xx_v1
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "aos_test_bq241xx.h"

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


aos_testresult_t aosTestBq241xxFunc(BaseSequentialStream* stream, const aos_test_t* test)
{
  aosDbgCheck(test->data != NULL);

  // local variables
  aos_testresult_t result;
  int32_t status;
  bq241xx_lld_enable_t en[3];
  bq241xx_lld_charge_state_t charge;

  aosTestResultInit(&result);

  chprintf(stream, "read enable pin...\n");
  status = bq241xx_lld_get_enabled(((aos_test_bq241xxdata_t*)test->data)->driver, &en[0]);
  chprintf(stream, "\t\tcurrently %s\n", (en[0] == BQ241xx_LLD_ENABLED) ? "enabled" : "disabled");
  if(status == APAL_STATUS_SUCCESS){
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "write enable pin...\n");
  status = bq241xx_lld_set_enabled(((aos_test_bq241xxdata_t*)test->data)->driver, (en[0] == BQ241xx_LLD_ENABLED) ? BQ241xx_LLD_DISABLED : BQ241xx_LLD_ENABLED);
  aosThdMSleep(1);
  status |= bq241xx_lld_get_enabled(((aos_test_bq241xxdata_t*)test->data)->driver, &en[1]);
  status |= bq241xx_lld_set_enabled(((aos_test_bq241xxdata_t*)test->data)->driver, en[0]);
  aosThdMSleep(1);
  status |= bq241xx_lld_get_enabled(((aos_test_bq241xxdata_t*)test->data)->driver, &en[2]);
  if(status == APAL_STATUS_SUCCESS && en[0] != en[1] && en[0] == en[2] && en[1] != en[2]){
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X, %u-%u\n", status, en[1], en[2]);
  }

  chprintf(stream, "read status pin...\n");
  aosThdMSleep(500);
  status = bq241xx_lld_get_charge_status(((aos_test_bq241xxdata_t*)test->data)->driver, &charge);
  chprintf(stream, "\t\tcharge status: %scharging\n", (charge == BQ241xx_LLD_CHARGING) ? "" : "not ");
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  aosTestInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(BQ241xxDriver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
