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
 * @file    aos_test_TPS6211.c
 * @brief   Step-Down converter driver test types implementation.
 *
 * @addtogroup test_TPS6211_v1
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "aos_test_TPS6211x.h"

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

aos_testresult_t aosTestTps6211xFunc(BaseSequentialStream* stream, const aos_test_t* test)
{
  aosDbgCheck(test->data != NULL && ((aos_test_tps6211xbdata_t*)test->data)->driver != NULL);

  // local variables
  aos_testresult_t result;
  int32_t status;
  tps6211x_lld_power_en_t power;

  aosTestResultInit(&result);

  chprintf(stream, "read pin...\n");
  status = tps6211x_lld_get_power_en(((aos_test_tps6211xbdata_t*)test->data)->driver, &power);
  if(status == APAL_STATUS_SUCCESS){
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "write pin...\n");
  status = tps6211x_lld_set_power_en(((aos_test_tps6211xbdata_t*)test->data)->driver, TPS6211x_LLD_POWER_ENABLED);
  status |= tps6211x_lld_get_power_en(((aos_test_tps6211xbdata_t*)test->data)->driver, &power);
  if(status == APAL_STATUS_SUCCESS && power == TPS6211x_LLD_POWER_ENABLED){
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  aosTestInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(TPS6211xDriver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
