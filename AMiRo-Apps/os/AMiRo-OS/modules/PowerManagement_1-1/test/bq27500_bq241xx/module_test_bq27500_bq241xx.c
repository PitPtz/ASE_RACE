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
 * @file    module_test_bq27500_bq241xx.c
 *
 * @addtogroup powermanagement11_test
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "module_test_bq27500_bq241xx.h"
#include <aos_test_bq27500_bq241xx.h>
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

static aos_test_bq27500bq241xxdata_t _data = {
  /* bq27500 driver   */ NULL,
  /* bq24103a driver  */ NULL,
  /* timeout          */ MICROSECONDS_PER_SECOND,
};

static AOS_TEST(_test, "bq27500 & bq24103a", "fuel gauge & battery charger", moduleTestBq27500Bq241xxShellCb, aosTestBq27500Bq241xxFunc, &_data);

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

int moduleTestBq27500Bq241xxShellCb(BaseSequentialStream* stream, int argc, const char* argv[], aos_testresult_t* result)
{
  enum {
    UNKNOWN,
    FRONT, REAR,
  } charger = UNKNOWN;

  // evaluate argument
  if (argc == 2) {
    if (strcmp(argv[1], "-f") == 0 || strcmp(argv[1], "--front") == 0) {
      charger = FRONT;
    } else if (strcmp(argv[1], "-r") == 0 || strcmp(argv[1], "--rear") == 0) {
      charger = REAR;
    }
  }

  // handle valid charger
  if (charger != UNKNOWN) {
    aos_testresult_t res = {0, 0};

    switch (charger) {
      case FRONT:
        _data.bq27500 = &moduleLldFuelGaugeFront;
        _data.bq241xx = &moduleLldBatteryChargerFront;
        res = aosTestRun(stream, &_test, "front battery");
        _data.bq27500 = NULL;
        _data.bq241xx = NULL;
        break;
      case REAR:
        _data.bq27500 = &moduleLldFuelGaugeRear;
        _data.bq241xx = &moduleLldBatteryChargerRear;
        res = aosTestRun(stream, &_test, "rear battery");
        _data.bq27500 = NULL;
        _data.bq241xx = NULL;
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
    chprintf(stream, "  --front, -f\n");
    chprintf(stream, "    Test the front battery fuel gauge and charger.\n");
    chprintf(stream, "  --rear, -r\n");
    chprintf(stream, "    Test the rear battery fuel gauge and charger.\n");
    if (result != NULL) {
      result->passed = 0;
      result->failed = 0;
    }
    return AOS_INVALIDARGUMENTS;
   }
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
