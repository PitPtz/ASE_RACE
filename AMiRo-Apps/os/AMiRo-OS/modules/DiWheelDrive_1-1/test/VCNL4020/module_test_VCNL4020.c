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
 * @addtogroup diwheeldrive11_test
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
  /* driver       */ &moduleLldProximity,
  /* timeout      */ MICROSECONDS_PER_SECOND,
  /* event source */ &aos.events.gpio,
  /* event flags  */ MODULE_OS_GPIOEVENTFLAG_IRINT,
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
    FL, FR, WL, WR,
  } sensor = UNKNOWN;

  // evaluate arguments
  if (argc == 2) {
    if (strcmp(argv[1], "--frontleft") == 0 || strcmp(argv[1], "-fl") == 0) {
      sensor = FL;
    } else if (strcmp(argv[1], "--frontright") == 0 || strcmp(argv[1], "-fr") == 0) {
      sensor = FR;
    } else if (strcmp(argv[1], "--wheelleft") == 0 || strcmp(argv[1], "-wl") == 0) {
      sensor = WL;
    } else if (strcmp(argv[1], "--wheelright") == 0 || strcmp(argv[1], "-wr") == 0) {
      sensor = WR;
    }
  }

  // handle valid sensor
  if (sensor != UNKNOWN) {
    aos_testresult_t res = {0, 0};

    pca9544a_lld_setchannel(&moduleLldI2cMultiplexer, PCA9544A_LLD_CH0, _data.timeout);
    _disableInterrupt(_data.vcnld);
    pca9544a_lld_setchannel(&moduleLldI2cMultiplexer, PCA9544A_LLD_CH1, _data.timeout);
    _disableInterrupt(_data.vcnld);
    pca9544a_lld_setchannel(&moduleLldI2cMultiplexer, PCA9544A_LLD_CH2, _data.timeout);
    _disableInterrupt(_data.vcnld);
    pca9544a_lld_setchannel(&moduleLldI2cMultiplexer, PCA9544A_LLD_CH3, _data.timeout);
    _disableInterrupt(_data.vcnld);
    switch (sensor) {
      case FL:
        pca9544a_lld_setchannel(&moduleLldI2cMultiplexer, PCA9544A_LLD_CH3, _data.timeout);
        res = aosTestRun(stream, &_test, "front left");
        break;
      case FR:
        pca9544a_lld_setchannel(&moduleLldI2cMultiplexer, PCA9544A_LLD_CH0, _data.timeout);
        res = aosTestRun(stream, &_test, "front right");
        break;
      case WL:
        pca9544a_lld_setchannel(&moduleLldI2cMultiplexer, PCA9544A_LLD_CH2, _data.timeout);
        res = aosTestRun(stream, &_test, "left wheel");
        break;
      case WR:
        pca9544a_lld_setchannel(&moduleLldI2cMultiplexer, PCA9544A_LLD_CH1, _data.timeout);
        res = aosTestRun(stream, &_test, "right wheel");
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
    chprintf(stream, "  --frontleft, -fl\n");
    chprintf(stream, "    Test front left proximity sensor.\n");
    chprintf(stream, "  --frontrigt, -fr\n");
    chprintf(stream, "    Test front right proximity sensor.\n");
    chprintf(stream, "  --wheelleft, -wl\n");
    chprintf(stream, "    Test left wheel proximity sensor.\n");
    chprintf(stream, "  --wheelright, -wr\n");
    chprintf(stream, "    Test right wheel proximity sensor.\n");
    if (result != NULL) {
      result->passed = 0;
      result->failed = 0;
    }
    return AOS_INVALIDARGUMENTS;
  }
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
