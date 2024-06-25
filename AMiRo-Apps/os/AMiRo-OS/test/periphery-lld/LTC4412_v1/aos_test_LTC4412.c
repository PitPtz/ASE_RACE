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
 * @file    aos_test_LTC4412.c
 * @brief   Powerpath controller driver test types implementation.
 *
 * @addtogroup test_LTC4412_v1
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "aos_test_LTC4412.h"

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

aos_testresult_t aosTestLtc4412Func(BaseSequentialStream* stream, const aos_test_t* test)
{
  aosDbgCheck(test->data != NULL && ((aos_test_ltc4412data_t*)test->data)->driver != NULL);

  // local variables
  aos_testresult_t result;
  int32_t status;

  aosTestResultInit(&result);

  chprintf(stream, "read ctrl pin...\n");
  ltc4412_lld_ctrl_t ctrl;
  status = ltc4412_lld_get_ctrl(((aos_test_ltc4412data_t*)test->data)->driver, &ctrl);
  if(status == APAL_STATUS_SUCCESS){
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "write ctrl pin...\n");
  status = ltc4412_lld_set_ctrl(((aos_test_ltc4412data_t*)test->data)->driver, LTC4412_LLD_CTRL_ACTIVE);
  status |= ltc4412_lld_get_ctrl(((aos_test_ltc4412data_t*)test->data)->driver, &ctrl);
  if (status == APAL_STATUS_SUCCESS && ctrl == LTC4412_LLD_CTRL_ACTIVE) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "read stat pin...\n");
  ltc4412_lld_stat_t stat;
  status = ltc4412_lld_get_stat(((aos_test_ltc4412data_t*)test->data)->driver, &stat);
  if(status == APAL_STATUS_SUCCESS){
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  aosTestInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(LTC4412Driver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
