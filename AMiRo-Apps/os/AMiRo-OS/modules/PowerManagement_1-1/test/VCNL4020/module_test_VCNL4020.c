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
 * @file    module_test_VCNL4020.h
 *
 * @addtogroup powermanagement11_test
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "module_test_VCNL4020.h"
#include <aos_test_VCNL4020.h>
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

static aos_test_vcnl4020data_t _data = {
  /* driver       */ NULL,
  /* timeout      */ MICROSECONDS_PER_SECOND,
  /* event source */ &aos.events.gpio,
  /* event flags  */ 0,
};

static AOS_TEST(_test, "VCNL4020", "proximity sensor", moduleTestVcnl4020ShellCb, aosTestVcnl4020Func, &_data);

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

static void _disableInterrupt(VCNL4020Driver* vcnl)
{
  uint8_t intstatus;
  vcnl4020_lld_writereg(vcnl, VCNL4020_LLD_REGADDR_INTCTRL, 0, _data.timeout);
  vcnl4020_lld_readreg(vcnl, VCNL4020_LLD_REGADDR_INTSTATUS, &intstatus, _data.timeout);
  if (intstatus) {
    vcnl4020_lld_writereg(vcnl, VCNL4020_LLD_REGADDR_INTSTATUS, intstatus, _data.timeout);
  }
  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

int moduleTestVcnl4020ShellCb(BaseSequentialStream* stream, int argc, const char* argv[], aos_testresult_t* result)
{
  enum {
    UNKNOWN,
    NNE, ENE, ESE, SSE, SSW, WSW, WNW, NNW,
  } sensor = UNKNOWN;

  // evaluate arguments
  if (argc == 2) {
    if (strcmp(argv[1], "-nne") == 0) {
      sensor = NNE;
    } else if (strcmp(argv[1], "-ene") == 0) {
      sensor = ENE;
    } else if (strcmp(argv[1], "-ese") == 0) {
      sensor = ESE;
    } else if (strcmp(argv[1], "-sse") == 0) {
      sensor = SSE;
    } else if (strcmp(argv[1], "-ssw") == 0) {
      sensor = SSW;
    } else if (strcmp(argv[1], "-wsw") == 0) {
      sensor = WSW;
    } else if (strcmp(argv[1], "-wnw") == 0) {
      sensor = WNW;
    } else if (strcmp(argv[1], "-nnw") == 0) {
      sensor = NNW;
    }
  }

  // handle valid sensor
  if (sensor != UNKNOWN) {
    aos_testresult_t res = {0, 0};
    PCA9544ADriver* mux = NULL;
    switch (sensor) {
      case SSE:
      case SSW:
      case WSW:
      case WNW:
        mux = &moduleLldI2cMultiplexer1;
        _data.vcnld = &moduleLldProximity1;
        _data.evtflags = MODULE_OS_GPIOEVENTFLAG_IRINT2;
        break;
      case NNW:
      case NNE:
      case ENE:
      case ESE:
        mux = &moduleLldI2cMultiplexer2;
        _data.vcnld = &moduleLldProximity2;
        _data.evtflags = MODULE_OS_GPIOEVENTFLAG_IRINT1;
        break;
      default:
        break;
    }
    pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH0, _data.timeout);
    _disableInterrupt(_data.vcnld);
    pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH1, _data.timeout);
    _disableInterrupt(_data.vcnld);
    pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH2, _data.timeout);
    _disableInterrupt(_data.vcnld);
    pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH3, _data.timeout);
    _disableInterrupt(_data.vcnld);
    switch (sensor) {
      case NNE:
        pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH1, _data.timeout);
        res = aosTestRun(stream, &_test, "north-northeast sensor (I2C #2)");
        break;
      case ENE:
        pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH3, _data.timeout);
        res = aosTestRun(stream, &_test, "east-northeast sensor (I2C #2)");
        break;
      case ESE:
        pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH2, _data.timeout);
        res = aosTestRun(stream, &_test, "east-southeast sensor (I2C #2)");
        break;
      case SSE:
        pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH0, _data.timeout);
        res = aosTestRun(stream, &_test, "south-southeast sensor (I2C #1)");
        break;
      case SSW:
        pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH1, _data.timeout);
        res = aosTestRun(stream, &_test, "south-southwest sensor (I2C #1)");
        break;
      case WSW:
        pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH3, _data.timeout);
        res = aosTestRun(stream, &_test, "west-southwest sensor (I2C #1)");
        break;
      case WNW:
        pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH2, _data.timeout);
        res = aosTestRun(stream, &_test, "west-northwest sensor (I2C #1)");
        break;
      case NNW:
        pca9544a_lld_setchannel(mux, PCA9544A_LLD_CH0, _data.timeout);
        res = aosTestRun(stream, &_test, "north-northwest sensor (I2C #2)");
        break;
      default:
        break;
    }
    _data.vcnld = NULL;
    _data.evtflags = 0;
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
    chprintf(stream, "  -nne\n");
    chprintf(stream, "    Test north-northeast sensor.\n");
    chprintf(stream, "  -ene\n");
    chprintf(stream, "    Test east-northeast sensor.\n");
    chprintf(stream, "  -ese\n");
    chprintf(stream, "    Test east-southeast sensor.\n");
    chprintf(stream, "  -sse\n");
    chprintf(stream, "    Test south-southeast sensor.\n");
    chprintf(stream, "  -ssw\n");
    chprintf(stream, "    Test south-southwest sensor.\n");
    chprintf(stream, "  -wsw\n");
    chprintf(stream, "    Test west-southwest sensor.\n");
    chprintf(stream, "  -wnw\n");
    chprintf(stream, "    Test west-northwest sensor.\n");
    chprintf(stream, "  -nnw\n");
    chprintf(stream, "    Test north-northwest sensor.\n");
    if (result != NULL) {
      result->passed = 0;
      result->failed = 0;
    }
    return AOS_INVALIDARGUMENTS;
  }
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
