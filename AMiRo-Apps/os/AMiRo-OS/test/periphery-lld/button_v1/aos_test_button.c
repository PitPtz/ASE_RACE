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
 * @file    aos_test_button.c
 * @brief   Generic button driver test types implementation.
 *
 * @addtogroup test_button_v1
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "aos_test_button.h"

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

#define INTERRUPT_EVENT_ID            1

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
 * @brief   Button test function.
 *
 * @param[in] stream  Stream for input/output.
 * @param[in] test    Test object.
 *
 * @return            Test result value.
 */
aos_testresult_t aosTestButtonFunc(BaseSequentialStream* stream, const aos_test_t* test)
{
  aosDbgCheck(test->data != NULL &&
      ((aos_test_buttondata_t*)test->data)->button != NULL &&
      ((aos_test_buttondata_t*)test->data)->evtsource != NULL);

  // local variables
  aos_testresult_t result;
  int32_t status;
  unsigned int count = 0;
  button_lld_state_t bstate;
  event_listener_t evtlistener;
  aos_timestamp_t tcurrent, tend;

  aosTestResultInit(&result);

  chprintf(stream, "test interrupts for ten seconds...\n");
  chEvtRegister(((aos_test_buttondata_t*)test->data)->evtsource, &evtlistener, INTERRUPT_EVENT_ID);
  status = APAL_STATUS_OK;
  aosSysGetUptime(&tend);
  aosTimestampAddInterval(tend, 10 * MICROSECONDS_PER_SECOND);
  do {
    const eventmask_t emask = chEvtWaitOneTimeout(EVENT_MASK(INTERRUPT_EVENT_ID), chTimeUS2I(MICROSECONDS_PER_SECOND));
    const eventflags_t eflags = chEvtGetAndClearFlags(&evtlistener);
    status |= button_lld_get(((aos_test_buttondata_t*)test->data)->button, &bstate);
    if (emask == EVENT_MASK(INTERRUPT_EVENT_ID) &&
        eflags == ((aos_test_buttondata_t*)test->data)->evtflags) {
      // a correct event was detected
      chprintf(stream, "\t\tinterrupt detected: %s\n", (bstate == BUTTON_LLD_STATE_PRESSED) ? "pressed" : "released");
      ++count;
    } else {
      // no button event was detected (usually timeout)
      chprintf(stream, "\t\twaiting for event...\n");
    }
    aosSysGetUptime(&tcurrent);
  } while (aosTimestampGet(tcurrent) < aosTimestampGet(tend));
  chEvtUnregister(((aos_test_buttondata_t*)test->data)->evtsource, &evtlistener);
  if (status == APAL_STATUS_OK && count > 0) {
    aosTestPassedMsg(stream, &result, "%u events detected\n", count);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X; %u\n", status, count);
  }

  aosTestInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(ButtonDriver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
