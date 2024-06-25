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
 * @file    module_test_VL6180X.c
 *
 * @addtogroup nucleol476rg_test
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "module_test_VL6180X.h"
#include <aos_test_VL6180X.h>

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

static aos_test_vl6180xdata_t _data = {
  .vl6180xd = &moduleLldVl6180x,
};

static AOS_TEST(_test, "VL6180X", NULL, moduleTestVl6180xShellCb, aosTestVl6180xFunc, &_data);

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup nucleol476rg_test
 * @{
 */

/**
 * @brief   VL6180X test.
 *
 * @param[in]  stream   Stream to print output to.
 * @param[in]  argc     Number of arguments.
 * @param[in]  argv     Argument list
 * @param[out] result   Pointer to store the test result to.
 *                      May be NULL.
 *
 * @return  Status indicating the success of the function call.
 */
int moduleTestVl6180xShellCb(BaseSequentialStream* stream, int argc, const char* argv[], aos_testresult_t* result)
{
  (void)argc;
  (void)argv;

  if (result != NULL) {
    *result = aosTestRun(stream, &_test, NULL);
  } else {
    aosTestRun(stream, &_test, NULL);
  }

  return AOS_OK;
}

/** @} */

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
