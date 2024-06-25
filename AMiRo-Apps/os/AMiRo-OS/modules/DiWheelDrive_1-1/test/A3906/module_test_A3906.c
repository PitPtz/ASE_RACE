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
 * @file    module_test_A3906.h
 *
 * @addtogroup diwheeldrive11_test
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "module_test_A3906.h"
#include <aos_test_A3906.h>
#include <stdlib.h>

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

static aos_test_a3906data_t _data = {
  /* driver           */ &moduleLldMotors,
  /* PWM information  */ {
    /* driver   */ &MODULE_HAL_PWM_DRIVE,
    /* channels */ {
      /* left wheel forward   */ MODULE_HAL_PWM_DRIVE_CHANNEL_LEFT_FORWARD,
      /* left wheel backward  */ MODULE_HAL_PWM_DRIVE_CHANNEL_LEFT_BACKWARD,
      /* right wheel forward  */ MODULE_HAL_PWM_DRIVE_CHANNEL_RIGHT_FORWARD,
      /* right wheel backward */ MODULE_HAL_PWM_DRIVE_CHANNEL_RIGHT_BACKWARD,
    },
  },
  /* QEI information  */ {
    /* left wheel               */ &MODULE_HAL_QEI_LEFT_WHEEL,
    /* right wheel              */ &MODULE_HAL_QEI_RIGHT_WHEEL,
    /* increment per revolution */ MODULE_HAL_QEI_INCREMENTS_PER_REVOLUTION,
    /* variation threshold      */ (apalQEICount_t)(7000 * 0.05f), // 7000 tps expeced, 5% variation acceptable
  },
  /* Wheel diameter   */ {
    /* left wheel   */ 0.05571f,
    /* right wheel  */ 0.05571f,
  },
  /* timeout          */ 10 * MICROSECONDS_PER_SECOND,
};

static AOS_TEST(_test, "A3906", "motor driver", moduleTestA3906ShellCb, aosTestA3906Func, &_data);

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

int moduleTestA3906ShellCb(BaseSequentialStream* stream, int argc, const char* argv[], aos_testresult_t* result)
{
  // print help text
  if (aosShellcmdCheckHelp(argc, argv, true) == AOS_SHELL_HELPREQUEST_EXPLICIT) {
    chprintf(stream, "Usage: %s [TIMEOUT]\n", argv[0]);
    chprintf(stream, "TIMEOUT:\n");
    chprintf(stream, "  Optional timeout value in seconds (default: 10).\n");
    return AOS_OK;
  }

  // set and check timeout
  _data.timeout = ((argc == 1) ? 10 : strtoul(argv[1], NULL, 0)) * MICROSECONDS_PER_SECOND;
  if (_data.timeout == 0) {
    return AOS_INVALIDARGUMENTS;
  }

  if (result != NULL) {
    *result = aosTestRun(stream, &_test, NULL);
  } else {
    aosTestRun(stream, &_test, NULL);
  }

  return AOS_OK;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
