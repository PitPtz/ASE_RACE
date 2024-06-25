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
 * @file    module_test_DW1000.c
 *
 * @addtogroup lightring12_test
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "module_test_DW1000.h"
#include <aos_test_DW1000.h>
#include <string.h>
#include <alld_MIC9404x.h>

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

static aos_test_dw1000data_t _data = {
  /* driver */ &moduleLldDw1000,
};

static AOS_TEST(_test, "DW1000", "UWB transceiver", moduleTestDw1000ShellCb, aosTestDw1000Func, &_data);

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

int moduleTestDw1000ShellCb(BaseSequentialStream* stream, int argc, const char* argv[], aos_testresult_t* result)
{
  (void)argc;
  (void)argv;

  // enable power
  mic9404x_lld_set(&moduleLldPowerSwitchV18, MIC9404x_LLD_STATE_ON);
  mic9404x_lld_set(&moduleLldPowerSwitchV33, MIC9404x_LLD_STATE_ON);

  if (result != NULL) {
    *result = aosTestRun(stream, &_test, NULL);
  } else {
    aosTestRun(stream, &_test, NULL);
  }

  chprintf(stream, "TODO: turn off the device\n");
  if (result) {
    aosTestFailed(stream, result);
  }

  return AOS_OK;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
