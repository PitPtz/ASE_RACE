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
 * @file    aos_test_DW1000.c
 * @brief   UWB transceiver driver test types implementation.
 *
 * @addtogroup test_DW1000_v2
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "aos_test_DW1000.h"

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
 * @brief   DW1000 test function.
 *
 * @param[in] stream  Stream for input/output.
 * @param[in] test    Test object.
 *
 * @return            Test result value.
 */
aos_testresult_t aosTestDw1000Func(BaseSequentialStream* stream, const aos_test_t* test) {

  aosDbgCheck(test->data != NULL &&
      ((aos_test_dw1000data_t*)test->data)->driver != NULL);

  // local variables
  aos_testresult_t result;

  aosTestResultInit(&result);

  chprintf(stream, "TODO (%s): reimplement test routine\n", __FILE__);
  chprintf(stream, "  - Implement EVERYTHING in the test/periphery-lld/DW1000/v2/ folder!\n");
  chprintf(stream, "  - Preferably only use the two files aos_test_DW1000.h/.c!\n");
  chprintf(stream, "  - The test shall assume that any other involved periphery has been setup correctly before!\n");
  chprintf(stream, "  - Any module-specific code (e.g. initialization of voltages) belongs to the according <module>/test/DW1000/ folder!\n");
  chprintf(stream, "\n");

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
