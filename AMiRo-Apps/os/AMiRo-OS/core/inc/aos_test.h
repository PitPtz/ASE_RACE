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
 * @file    aos_test.h
 * @brief   Test structures and interfaces.
 *
 * @addtogroup core_test
 * @{
 */

#ifndef AMIROOS_TEST_H
#define AMIROOS_TEST_H

#include <aosconf.h>

#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)

#include <hal.h>
#include <chprintf.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

/**
 * forward declarations
 * @cond
 */
typedef struct aos_testresult aos_testresult_t;
typedef struct aos_test aos_test_t;
/** @endcond */

/**
 * @brief   Test interface function definition.
 *
 * @param[in] stream  The stream to use for printing messages.
 * @param[in] test    The object to run the test on.
 *
 * @return    Result containing the number of passed and failed tests.
 */
typedef aos_testresult_t (*aos_testfunction_t)(BaseSequentialStream* stream, const aos_test_t* test);

/**
 * @brief   Wrapper interface definition to allow programmatical shell-like call
 *          of the test.
 * @details While the first three arguments (stream, argc and argv) as well as
 *          the return value are identical to a shell command, the additional
 *          optional argument (result) is set by the function.
 *
 * @param[in]   stream  Stream to print to.
 * @param[in]   argc    Number of arguments given.
 * @param[in]   argv    List of arguments.
 * @param[out]  result  Result of the test (optional).
 *
 * @return    Execution status of the function, which can be passed on to a
 *            shell callback.
 */
typedef int (*aos_testshellcallback_t)(BaseSequentialStream* stream, int argc, const char* argv[], aos_testresult_t* result);

/**
 * @brief   Test result struct.
 */
typedef struct aos_testresult {
  /**
   * @brief   Number of passed tests.
   */
  uint32_t passed;

  /**
   * @brief   Number of failed tests.
   */
  uint32_t failed;
} aos_testresult_t;

/**
 * @brief   Test definition struct.
 */
typedef struct aos_test {
  /**
   * @brief   Name of the test.
   */
  const char* name;

  /**
   * @brief   Further information about the test.
   */
  const char* info;

  /**
   * @brief   Callback function to serve as wrapper between shell and test.
   *
   * @details The purpose of this wrapper is to be able to execute tests
   *          programatically exactly like a shell command from the CLI. It
   *          should handle any required setup depending on given arguments and
   *          eventually execute the test.
   */
  aos_testshellcallback_t shellcb;

  /**
   * @brief   Callback function to run that executes the test.
   */
  aos_testfunction_t testfunc;

  /**
   * @brief   Further test specific data.
   */
  void* data;
} aos_test_t;


/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/**
 * @brief   Test object generator macro.
 *
 * @param[in,out] var       Name of the instance/object.
 * @param[in]     name      Name of the test.
 * @param[in]     info      Further information about the test.
 * @param[in]     shellcb   Callback function to serve as wrapper between shell and test.
 * @param[in]     testfunc  Callback function to run that executes the test.
 * @param[in]     data      Pointer to further, test specific data.
 *                          May be NULL.
 *
 * @return  Instance of aos_test_t with name var.
 */
#define AOS_TEST(var, name, info, shellcb, testfunc, data) aos_test_t var = {   \
  /* name     */ name,                                                          \
  /* info     */ info,                                                          \
  /* shellcb  */ shellcb,                                                       \
  /* testfunc */ testfunc,                                                      \
  /* data     */ data,                                                          \
}

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void aosTestResultPrintSummary(BaseSequentialStream* stream, const aos_testresult_t* result, const char* heading);
  aos_testresult_t aosTestRun(BaseSequentialStream* stream, const aos_test_t* test, const char* note);
  void aosTestPassedMsg(BaseSequentialStream* stream, aos_testresult_t* result, const char* fmt, ...);
  void aosTestFailedMsg(BaseSequentialStream* stream, aos_testresult_t* result, const char* fmt, ...);
  void aosTestInfoMsg(BaseSequentialStream* stream, const char* fmt, ...);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/**
 * @brief   Initialize a result object.
 *
 * @param[in] result  Pointer to the object to be nitialized.
 */
static inline void aosTestResultInit(aos_testresult_t* result)
{
  result->passed = 0;
  result->failed = 0;

  return;
}

/**
 * @brief   Adds two result objects.
 *
 * @param[in] a   The first summand.
 * @param[in] b   The second summand.
 *
 * @return    Resulting sum of both summands added.
 */
static inline aos_testresult_t aosTestResultAdd(aos_testresult_t a, aos_testresult_t b)
{
  a.passed += b.passed;
  a.failed += b.failed;

  return a;
}

/**
 * @brief   Retrieve the total number of tests (passed and failed).
 *
 * @param[in] result    The result object to evaluate.
 *
 * @return  Number of total tests executed.
 */
static inline uint32_t aosTestResultTotal(const aos_testresult_t* result)
{
  return result->passed + result->failed;
}

/**
 * @brief   Retrieve the ratio of passed tests.
 *
 * @param[in] result    The result object to evaluate.
 *
 * @return  Ratio of passed tests to total tests as float in range [0, 1].
 */
static inline float aosTestResultRatio(const aos_testresult_t *result)
{
  if (aosTestResultTotal(result) > 0) {
    return (float)result->passed / (float)(aosTestResultTotal(result));
  } else {
    return 1.0f;
  }
}

/**
 * @brief   Helper function for passed tests.
 * @details Prints a message that the test was passed and modifies the result
 *          accordigly.
 *
 * @param[in] stream      Stream to print the message to.
 * @param[in,out] result  Result object to modify.
 */
static inline void aosTestPassed(BaseSequentialStream *stream, aos_testresult_t* result)
{
  ++result->passed;
  chprintf(stream, "\tPASSED\n");
  chprintf(stream, "\n");

  return;
}

/**
 * @brief   Helper function for failed tests.
 * @details Prints a message that the test was failed and modifies the result
 *          accordigly.
 *
 * @param[in] stream      Stream to print the message to.
 * @param[in,out] result  Result object to modify.
 */
static inline void aosTestFailed(BaseSequentialStream *stream, aos_testresult_t* result)
{
  ++result->failed;
  chprintf(stream, "\tFAILED\n");
  chprintf(stream, "\n");

  return;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */

#endif /* AMIROOS_TEST_H */

/** @} */
