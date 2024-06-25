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
 * @file    aos_test.c
 * @brief   Test code.
 * @details Functions to initialize and run tests,
 *          as well as utility functions to be used in tests.
 */

#include <amiroos.h>

#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)

#include <string.h>
#include <stdarg.h>

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

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup core_test
 * @{
 */

/**
 * @brief   Print the summary of a test.
 * @details The summary consists of:
 *          - total numer of tests executed
 *          - absolute number of passed tests
 *          - absolute number of failed tests
 *          - relative ratio of passed tests
 *
 * @param[in] stream    Stream to print the result to.
 * @param[in] result    Result to evaluate and print.
 * @param[in] heading   Optional heading (defaults to "summary").
 */
void aosTestResultPrintSummary(BaseSequentialStream *stream, const aos_testresult_t* result, const char* heading)
{
  aosDbgCheck(stream != NULL);
  aosDbgCheck(result != NULL);

  chprintf(stream, "%s:\n", (heading != NULL) ? heading : "summary");
  chprintf(stream, "\ttotal:  %3u\n", aosTestResultTotal(result));
  chprintf(stream, "\tpassed: %3u\n", result->passed);
  chprintf(stream, "\tfailed: %3u\n", result->failed);
  chprintf(stream, "\tratio:  %3u%%\n", (uint8_t)(aosTestResultRatio(result) * 100.0f)); // implicitly rounded off by cast to integer

  return;
}

/**
 * @brief   Run a test.
 *
 * @param[in] stream  A stream for printing messages.
 * @param[in] test    Test to execute.
 * @param[in] note    Optional note string.
 *
 * @return    Result of the test.
 */
aos_testresult_t aosTestRun(BaseSequentialStream *stream, const aos_test_t *test, const char* note)
{
  aosDbgCheck(stream != NULL);
  aosDbgCheck(test != NULL);

  // print name heading
  {
    chprintf(stream, "\n");
    const int nchars = chprintf(stream, "%s test\n", test->name);
    for (int c = 0; c < nchars-1; ++c) {
      streamPut(stream, '=');
    }
    chprintf(stream, "\n");
  }

  // print info (if any)
  if (test->info != NULL) {
    chprintf(stream, "info: %s\n", test->info);
  }
  // print note (if any)
  if (note != NULL) {
    chprintf(stream, "note: %s\n", note);
  }
  chprintf(stream, "\n");

  // run test
  aos_testresult_t result = test->testfunc(stream, test);

  // print summary
  aosTestResultPrintSummary(stream, &result, NULL);

  return result;
}

/**
 * @brief   Helper function for passed tests.
 * @details Prints a message that the test was passed, an additional custom
 *          message, and modifies the result accordigly.
 *
 * @param[in] stream      Stream to print the message to.
 * @param[in,out] result  Result object to modify.
 * @param[in] fmt         Formatted message string.
 */
void aosTestPassedMsg(BaseSequentialStream *stream, aos_testresult_t* result, const char *fmt, ...)
{
  aosDbgCheck(stream != NULL);
  aosDbgCheck(result != NULL);

  va_list ap;

  ++result->passed;
  chprintf(stream, "\tPASSED\t");
  va_start(ap, fmt);
  chvprintf(stream, fmt, ap);
  va_end(ap);
  chprintf(stream, "\n");

  return;
}

/**
 * @brief   Helper function for failed tests.
 * @details Prints a message that the test was failed, an additional custom
 *          message, and modifies the result accordigly.
 *
 * @param[in] stream      Stream to print the message to.
 * @param[in,out] result  Result object to modify.
 * @param[in] fmt         Formatted message string.
 */
void aosTestFailedMsg(BaseSequentialStream *stream, aos_testresult_t* result, const char *fmt, ...)
{
  aosDbgCheck(stream != NULL);
  aosDbgCheck(result != NULL);

  va_list ap;

  ++result->failed;
  chprintf(stream, "\tFAILED\t");
  va_start(ap, fmt);
  chvprintf(stream, fmt, ap);
  va_end(ap);
  chprintf(stream, "\n");

  return;
}

/**
 * @brief   Helper function for information messages.
 *
 * @param[in] stream  Strean to rpint the message to.
 * @param[in] fmt     Formatted message string.
 */
void aosTestInfoMsg(BaseSequentialStream* stream, const char* fmt, ...)
{
  aosDbgCheck(stream != NULL);

  va_list ap;
  va_start(ap, fmt);
  chvprintf(stream, fmt, ap);
  va_end(ap);
  chprintf(stream, "\n");

  return;
}

/** @} */

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */
