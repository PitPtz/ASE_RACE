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
 * @file    module_test_BNO055.h
 *
 * @addtogroup diwheeldrive12_test
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "module_test_BNO055.h"
#include <aos_test_BNO055.h>
#include <string.h>

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

static aos_test_bno055_t _data = {
  /* driver     */   &moduleLldBno,
  /* timeout    */   MICROSECONDS_PER_SECOND,
};

static AOS_TEST(_test, "BNO055", "Integrated Sensor fusion", moduleTestBno055ShellCb, aosTestBno055Func, &_data);

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

int moduleTestBno055ShellCb(BaseSequentialStream* stream, int argc, const char* argv[], aos_testresult_t* result)
{
  (void)argc;
  (void)argv;

  aos_testresult_t res = {0, 0};

  res = aosTestRun(stream, &_test, "front right");
  if (result != NULL) {
    *result = res;
  }
  return AOS_OK;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
