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
 * @file    module_test_PCA9544A.h
 *
 * @addtogroup powermanagement11_test
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "module_test_PCA9544A.h"
#include <aos_test_PCA9544A.h>
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

static aos_test_pca9544adata_t _data = {
  /* driver  */ NULL,
  /* timeout */ MICROSECONDS_PER_SECOND,
};

static AOS_TEST(_test, "PCA9544A", "I2C multiplexer", moduleTestPca9544aShellCb, aosTestPca9544aFunc, &_data);

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

int moduleTestPca9544aShellCb(BaseSequentialStream* stream, int argc, const char* argv[], aos_testresult_t* result)
{
  enum {
    UNKNOWN,
    MUX1, MUX2,
  } mux = UNKNOWN;

  // evaluate arguments
  if (argc == 2) {
    if (strcmp(argv[1], "#1") == 0) {
      mux = MUX1;
    } else if (strcmp(argv[1], "#2") == 0) {
      mux = MUX2;
    }
  }

  // handle valid mux
  if (mux != UNKNOWN) {
    aos_testresult_t res = {0 ,0};

    switch (mux) {
      case MUX1:
        _data.driver = &moduleLldI2cMultiplexer1;
        res = aosTestRun(stream, &_test, "I2C bus #1");
        _data.driver = NULL;
        break;
      case MUX2:
        _data.driver = &moduleLldI2cMultiplexer2;
        res = aosTestRun(stream, &_test, "I2C bus #2");
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
    chprintf(stream, "  #1\n");
    chprintf(stream, "    Test the multiplexer on the I2C bus #1.\n");
    chprintf(stream, "  #2\n");
    chprintf(stream, "    Test the multiplexer on the I2C bus #2.\n");
    if (result != NULL) {
      result->passed = 0;
      result->failed = 0;
    }
    return AOS_INVALIDARGUMENTS;
  }
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
