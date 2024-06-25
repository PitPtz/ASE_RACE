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
 * @file    module_test_INA219.h
 *
 * @addtogroup powermanagement11_test
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "module_test_INA219.h"
#include <aos_test_INA219.h>
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

static aos_test_ina219data_t _data = {
  /* driver           */ NULL,
  /* expected voltage */ 0.0f,
  /* tolerance        */ 0.05f,
  /* timeout */ MICROSECONDS_PER_SECOND,
};

static AOS_TEST(_test, "INA219", "power monitor", moduleTestIna219ShellCb, aosTestIna219Func, &_data);

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

int moduleTestIna219ShellCb(BaseSequentialStream* stream, int argc, const char* argv[], aos_testresult_t* result)
{
  enum {
    UNKNOWN,
    VDD, VIO18, VIO33, VSYS42, VIO50,
  } monitor = UNKNOWN;

  // evaluate argument
  if (argc == 2) {
    if (strcmp(argv[1], "VDD") == 0) {
      monitor = VDD;
    } else if (strcmp(argv[1], "VIO1.8") == 0) {
      monitor = VIO18;
    } else if (strcmp(argv[1], "VIO3.3") == 0) {
      monitor = VIO33;
    } else if (strcmp(argv[1], "VSYS4.2") == 0) {
      monitor = VSYS42;
    } else if (strcmp(argv[1], "VIO5.0") == 0) {
      monitor = VIO50;
    }
  }

  // handle valid monitor
  if (monitor != UNKNOWN) {
    aos_testresult_t res = {0, 0};

    switch (monitor) {
      case VDD:
        _data.inad = &moduleLldPowerMonitorVdd;
        _data.v_expected = 3.3f;
        res = aosTestRun(stream, &_test, "VDD (3.3V)");
        _data.inad = NULL;
        _data.v_expected = 0.0f;
        break;
      case VIO18:
        _data.inad = &moduleLldPowerMonitorVio18;
        _data.v_expected = 1.8f;
        res = aosTestRun(stream, &_test, "VIO (1.8V)");
        _data.inad = NULL;
        _data.v_expected = 0.0f;
        break;
      case VIO33:
        _data.inad = &moduleLldPowerMonitorVio33;
        _data.v_expected = 3.3f;
        res = aosTestRun(stream, &_test, "VIO (3.3V)");
        _data.inad = NULL;
        _data.v_expected = 0.0f;
        break;
      case VSYS42:
        _data.inad = &moduleLldPowerMonitorVsys42;
        _data.v_expected = 4.2f;
        res = aosTestRun(stream, &_test, "VSYS (4.2V)");
        _data.inad = NULL;
        _data.v_expected = 0.0f;
        break;
      case VIO50:
        _data.inad = &moduleLldPowerMonitorVio50;
        _data.v_expected = 5.0f;
        res = aosTestRun(stream, &_test, "VIO (5.0V)");
        _data.inad = NULL;
        _data.v_expected = 0.0f;
        break;
      default:
        break;
    }
    if (result != NULL) {
      *result = res;
    }
    return AOS_OK;
  }
  // handle invalid arguments
  else {
    // print help
    chprintf(stream, "Usage: %s OPTION\n", argv[0]);
    chprintf(stream, "Options:\n");
    chprintf(stream, "  VDD\n");
    chprintf(stream, "    Test VDD (3.3V) power monitor.\n");
    chprintf(stream, "  VIO1.8\n");
    chprintf(stream, "    Test VIO 1.8V power monitor.\n");
    chprintf(stream, "  VIO3.3\n");
    chprintf(stream, "    Test VIO 3.3V power monitor.\n");
    chprintf(stream, "  VSYS4.2\n");
    chprintf(stream, "    Test VSYS 4.2V power monitor.\n");
    chprintf(stream, "  VIO5.0\n");
    chprintf(stream, "    Test VIO 5.0V power monitor.\n");
    if (result != NULL) {
      result->passed = 0;
      result->failed = 0;
    }
    return AOS_INVALIDARGUMENTS;
  }
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
