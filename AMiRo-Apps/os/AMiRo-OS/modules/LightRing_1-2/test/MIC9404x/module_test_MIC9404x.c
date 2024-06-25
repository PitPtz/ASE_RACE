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
 * @file    module_test_MIC9404x.c
 *
 * @addtogroup lightring12_test
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "module_test_MIC9404x.h"
#include <aos_test_MIC9404x.h>
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

static aos_test_mic9404data_t _data = {
  /* driver */ NULL,
};

static AOS_TEST(_test, "MIC9404x", "power switch", moduleTestMic9404xShellCb, aosTestMic9404xFunc, &_data);

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

int moduleTestMic9404xShellCb(BaseSequentialStream* stream, int argc, const char* argv[], aos_testresult_t* result)
{
  enum {
    UNKNOWN,
    V18,
    V33,
// The 4.2V switch is disabled due to a hardware bug.
//  V42,
    V50,
    VSYS,
  } mic = UNKNOWN;

  // evaluate argument
  if (argc == 2) {
    if (strcmp(argv[1], "1.8V") == 0) {
      mic = V18;
    } else if (strcmp(argv[1], "3.3V") == 0) {
      mic = V33;
// The 4.2V switch is disabled due to a hardware bug.
//  } else if (strcmp(argv[1], "4.2V") == 0) {
//    mic = V42;
    } else if (strcmp(argv[1], "5.0V") == 0) {
      mic = V50;
    } else if (strcmp(argv[1], "VSYS") == 0) {
      mic = VSYS;
    }
  }

  // handle valid power switch
  if (mic != UNKNOWN) {
    aos_testresult_t res = {0, 0};

    switch (mic) {
      case V18:
        _data.driver = &moduleLldPowerSwitchV18;
        aosTestRun(stream, &_test, "1.8V");
        _data.driver = NULL;
        break;
      case V33:
        _data.driver = &moduleLldPowerSwitchV33;
        aosTestRun(stream, &_test, "3.3V");
        _data.driver = NULL;
        break;
// The 4.2V switch is disabled due to a hardware bug.
//    case V42:
//      _data.driver = &moduleLldPowerSwitchV42;
//      aosTestRun(stream, &_test, "4.2V");
//      _data.driver = NULL;
//      break;
      case V50:
        _data.driver = &moduleLldPowerSwitchV50;
        aosTestRun(stream, &_test, "5.0V");
        _data.driver = NULL;
        break;
      case VSYS:
        _data.driver = &moduleLldPowerSwitchVsys;
        aosTestRun(stream, &_test, "VSYS");
        _data.driver = NULL;
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
    chprintf(stream, "  1.8V\n");
    chprintf(stream, "    Test power switch for 1.8V supply.\n");
    chprintf(stream, "  3.3V\n");
    chprintf(stream, "    Test power switch for 3.3V supply.\n");
// The 4.2V switch is disabled due to a hardware bug.
//  chprintf(stream, "  4.2V\n");
//  chprintf(stream, "    Test power switch for 4.2V supply.\n");
    chprintf(stream, "  5.0V\n");
    chprintf(stream, "    Test power switch for 5.0V supply.\n");
    chprintf(stream, "  VSYS\n");
    chprintf(stream, "    Test power switch for VSYS supply.\n");
    if (result != NULL) {
      result->passed = 0;
      result->failed = 0;
    }
    return AOS_INVALIDARGUMENTS;
  }
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
