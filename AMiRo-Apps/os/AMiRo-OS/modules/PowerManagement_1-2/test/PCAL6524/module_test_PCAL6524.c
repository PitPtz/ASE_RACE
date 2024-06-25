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
 * @file    module_test_PCAL6524.h
 *
 * @addtogroup powermanagement12_test
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "module_test_PCAL6524.h"
#include <aos_test_PCAL6524.h>
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

static aos_test_pcal6524data_t _data = {
  /* driver   */ NULL,
  /* timeout  */ MICROSECONDS_PER_SECOND,
};

static AOS_TEST(_test, "PCAL6524", "GPIO extender", moduleTestPcal6524ShellCb, aosTestPcal6524Func, &_data);

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

int moduleTestPcal6524ShellCb(BaseSequentialStream* stream, int argc, const char* argv[], aos_testresult_t* result)
{
  enum {
    UNKNOWN,
    GPIOEXTENDER1, GPIOEXTENDER2,
  } extender = UNKNOWN;

  // evaluate arguments
  if (argc == 2) {
    if (strcmp(argv[1], "#1") == 0) {
      extender = GPIOEXTENDER1;
    } else if (strcmp(argv[1], "#2") == 0) {
      extender = GPIOEXTENDER2;
    }
  }

  // handle valid extender
  if (extender != UNKNOWN) {
    aos_testresult_t res = {0 ,0};

    switch (extender) {
      case GPIOEXTENDER1:
        _data.pcal6524d = &moduleLldGpioExtender1;
        res = aosTestRun(stream, &_test, "I2C bus #1");
        _data.pcal6524d = NULL;
        break;
      case GPIOEXTENDER2:
        _data.pcal6524d = &moduleLldGpioExtender2;
        res = aosTestRun(stream, &_test, "I2C bus #2");
        _data.pcal6524d = NULL;
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
    chprintf(stream, "  #1\n");
    chprintf(stream, "    Test the GPIO extender for the I2C bus #1.\n");
    chprintf(stream, "  #2\n");
    chprintf(stream, "    Test the GPIO extender for the I2C bus #2.\n");
    if (result != NULL) {
      result->passed = 0;
      result->failed = 0;
    }
    return AOS_INVALIDARGUMENTS;
  }
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
