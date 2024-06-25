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
 * @file    aos_test_A3906.c
 * @brief   Motor driver test types implementation.
 *
 * @addtogroup test_A3906_v1
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "aos_test_A3906.h"
#include <stdlib.h>
#include <math.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/**
 * @brief   Interval to poll QEI in certain tests (in milliseconds).
 */
#define QEI_POLL_INTERVAL_MS          10

/**
 * @brief   Duration of certain QEI test (in seconds).
 */
#define QEI_TEST_DURATION_S           5

/******************************************************************************/
/* EXPORTED VARIABLES                                                         */
/******************************************************************************/

/******************************************************************************/
/* LOCAL TYPES                                                                */
/******************************************************************************/

/**
 * @brief   Enumerator to distinguish between left and right wheel.
 */
typedef enum {
  WHEEL_LEFT    = 0,  /**< left wheel identifier */
  WHEEL_RIGHT   = 1,  /**< right wheel identifier */
} wheel_t;

/**
 * @brief   Enumerator to distinguish directions.
 */
typedef enum {
  DIRECTION_FORWARD   = 0,  /**< forward direction identifier */
  DIRECTION_BACKWARD  = 1,  /**< backward direction identifier */
} direction_t;

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   helper function to test each wheel and direction separately.
 *
 * @param[in] stream      Stream for input/output.
 * @param[in] data        Test meta data.
 * @param[in] wheel       Wheel to test.
 * @param[in] direction   Direction to test.
 * @param[in,out] result  Result variable to modify.
 */
void _aosTestA3906wheelDirectionTest(BaseSequentialStream* stream, aos_test_a3906data_t* data, wheel_t wheel, direction_t direction, aos_testresult_t* result)
{
  // local variables
  int32_t status;
  bool qei_valid;
  apalQEIDiff_t qei_delta;
  apalTime_t timeout_counter;

  // local constants
  const apalPWMwidth_t pwm_inc = (APAL_PWM_WIDTH_MAX - APAL_PWM_WIDTH_MIN) / (QEI_TEST_DURATION_S * MILLISECONDS_PER_SECOND / QEI_POLL_INTERVAL_MS);

  chprintf(stream, "%s wheel %s...\n", (wheel == WHEEL_LEFT) ? "left" : "right", (direction == DIRECTION_FORWARD) ? "forward" : "backward");
  qei_valid = false;
  status = apalQEIGetDelta((wheel == WHEEL_LEFT) ? data->qei.left : data->qei.right, &qei_delta);
  // increase PWM incrementally and read QEI data
  for (apalPWMwidth_t pwm_width = APAL_PWM_WIDTH_MIN;;) {
    status |= a3906_lld_set_pwm(data->pwm.driver, (wheel == WHEEL_LEFT) ?
                                  ((direction == DIRECTION_FORWARD) ? data->pwm.channel.left_forward : data->pwm.channel.left_backward) :
                                  ((direction == DIRECTION_FORWARD) ? data->pwm.channel.right_forward : data->pwm.channel.right_backward),
                                pwm_width);
    aosThdMSleep(QEI_POLL_INTERVAL_MS);
    status |= apalQEIGetDelta((wheel == WHEEL_LEFT) ? data->qei.left : data->qei.right, &qei_delta);
    qei_valid = qei_valid || (qei_delta != 0);
    if (pwm_width == APAL_PWM_WIDTH_MAX) {
      break;
    } else if (APAL_PWM_WIDTH_MAX - pwm_width > pwm_inc) {
      pwm_width += pwm_inc;
    } else {
      pwm_width = APAL_PWM_WIDTH_MAX;
    }
  }
  status |= qei_valid ? 0x00 : 0x10;
  status |= ((direction == DIRECTION_FORWARD) ? (qei_delta > 0) : (qei_delta < 0)) ? 0x00 : 0x20;

  // let the wheel spin free until it stops
  status |= a3906_lld_set_pwm(data->pwm.driver, (wheel == WHEEL_LEFT) ?
                                ((direction == DIRECTION_FORWARD) ? data->pwm.channel.left_forward : data->pwm.channel.left_backward) :
                                ((direction == DIRECTION_FORWARD) ? data->pwm.channel.right_forward : data->pwm.channel.right_backward),
                              0);
  timeout_counter = 0;
  do {
    aosThdMSleep(QEI_POLL_INTERVAL_MS);
    status |= apalQEIGetDelta((wheel == WHEEL_LEFT) ? data->qei.left : data->qei.right, &qei_delta);
    timeout_counter += QEI_POLL_INTERVAL_MS * MICROSECONDS_PER_MILLISECOND;
  } while ((qei_delta != 0) && (timeout_counter <= data->timeout));
  status |= (timeout_counter > data->timeout) ? APAL_STATUS_ERROR | APAL_STATUS_TIMEOUT : APAL_STATUS_OK;

  // report result
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, result);
  } else {
    aosTestFailedMsg(stream, result, "0x%08X\n", status);
  }

  return;
}

void _aosTestA3906wheelSpeedTest(BaseSequentialStream* stream, aos_test_a3906data_t* data, wheel_t wheel, direction_t direction, aos_testresult_t* result)
{
  // local variables
  int32_t status;
  apalQEIDiff_t qei_delta[2] = {0};
  apalQEICount_t qei_delta_diff = 0;
  apalTime_t stable_counter = 0;
  apalTime_t timeout_counter = 0;

  chprintf(stream, "%s wheel full speed %s...\n", (wheel == WHEEL_LEFT) ? "left" : "right", (direction == DIRECTION_FORWARD) ? "forward" : "backward");
  // spin up the wheel with full speed
  status = a3906_lld_set_pwm(data->pwm.driver, (wheel == WHEEL_LEFT) ?
                               ((direction == DIRECTION_FORWARD) ? data->pwm.channel.left_forward : data->pwm.channel.left_backward) :
                               ((direction == DIRECTION_FORWARD) ? data->pwm.channel.right_forward : data->pwm.channel.right_backward),
                             APAL_PWM_WIDTH_MAX);
  aosThdMSleep(100);
  status |= apalQEIGetDelta((wheel == WHEEL_LEFT) ? data->qei.left : data->qei.right, &qei_delta[1]);
  do {
    // read QEI data to determine speed
    aosThdMSleep(QEI_POLL_INTERVAL_MS);
    qei_delta[1] = qei_delta[0];
    status |= apalQEIGetDelta((wheel == WHEEL_LEFT) ? data->qei.left : data->qei.right, &qei_delta[0]);
    qei_delta_diff = abs((int32_t)qei_delta[0] - (int32_t)qei_delta[1]) * ((float)MILLISECONDS_PER_SECOND / (float)QEI_POLL_INTERVAL_MS);
    if ((qei_delta[0] != 0 || qei_delta[1] != 0) && (qei_delta_diff <= data->qei.variation_threshold)) {
      stable_counter += QEI_POLL_INTERVAL_MS * MICROSECONDS_PER_MILLISECOND;
    } else {
      stable_counter = 0;
    }
    timeout_counter += QEI_POLL_INTERVAL_MS * MICROSECONDS_PER_MILLISECOND;
    if (qei_delta[1] != 0 && stable_counter == 0) {
      chprintf(stream, "\tSpeed unstable? Jitter of %u tps is above threshold of %u tps.\n", qei_delta_diff, data->qei.variation_threshold);
    }
  } while ((stable_counter < MICROSECONDS_PER_SECOND) && (timeout_counter <= data->timeout));
  status |= a3906_lld_set_pwm(data->pwm.driver, (wheel == WHEEL_LEFT) ?
                               ((direction == DIRECTION_FORWARD) ? data->pwm.channel.left_forward : data->pwm.channel.left_backward) :
                               ((direction == DIRECTION_FORWARD) ? data->pwm.channel.right_forward : data->pwm.channel.right_backward),
                             APAL_PWM_WIDTH_OFF);
  status |= (timeout_counter > data->timeout) ? APAL_STATUS_ERROR | APAL_STATUS_TIMEOUT : APAL_STATUS_OK;

  // report results
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, result);
    const float tps = qei_delta[0] * ((float)MILLISECONDS_PER_SECOND / (float)QEI_POLL_INTERVAL_MS);
    const float rpm = tps * SECONDS_PER_MINUTE / (float)data->qei.increments_per_revolution;
    const float velocity = tps / (float)data->qei.increments_per_revolution * ((wheel == WHEEL_LEFT) ? data->wheel_diameter.left : data->wheel_diameter.right) * acos(-1);
    chprintf(stream, "\t%f tps\n", tps);
    chprintf(stream, "\t%f RPM\n", rpm);
    chprintf(stream, "\t%f m/s\n", velocity);
    chprintf(stream, "\n");
  }
  else {
    aosTestFailedMsg(stream, result, "0x%08X\n", status);
  }
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

/**
 * @brief   A3905 test function.
 *
 * @param[in] stream  Stream for input/output.
 * @param[in] test    Test object.
 *
 * @return            Test result value.
 */
aos_testresult_t aosTestA3906Func(BaseSequentialStream* stream, const aos_test_t* test)
{
  aosDbgCheck((test->data != NULL) &&
              (((aos_test_a3906data_t*)test->data)->driver != NULL) &&
              (((aos_test_a3906data_t*)test->data)->pwm.driver != NULL) &&
              (((aos_test_a3906data_t*)test->data)->qei.left != NULL) &&
              (((aos_test_a3906data_t*)test->data)->qei.right != NULL));



  // local variables
  aos_testresult_t result;
  int32_t status;
  a3906_lld_power_t power_state;
  apalQEICount_t qei_count[2][2];
  uint32_t timeout_counter;
  uint32_t stable_counter;

  aosTestResultInit(&result);

  chprintf(stream, "enable power...\n");
  power_state = A3906_LLD_POWER_ON;
  status = a3906_lld_set_power(((aos_test_a3906data_t*)test->data)->driver, power_state);
  status |= a3906_lld_get_power(((aos_test_a3906data_t*)test->data)->driver, &power_state);
  status |= (power_state != A3906_LLD_POWER_ON) ? 0x10 : 0x00;
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  _aosTestA3906wheelDirectionTest(stream, (aos_test_a3906data_t*)test->data, WHEEL_LEFT, DIRECTION_FORWARD, &result);

  _aosTestA3906wheelDirectionTest(stream, (aos_test_a3906data_t*)test->data, WHEEL_RIGHT, DIRECTION_FORWARD, &result);

  _aosTestA3906wheelDirectionTest(stream, (aos_test_a3906data_t*)test->data, WHEEL_LEFT, DIRECTION_BACKWARD, &result);

  _aosTestA3906wheelDirectionTest(stream, (aos_test_a3906data_t*)test->data, WHEEL_RIGHT, DIRECTION_BACKWARD, &result);

  _aosTestA3906wheelSpeedTest(stream, (aos_test_a3906data_t*)test->data, WHEEL_LEFT, DIRECTION_FORWARD, &result);

  _aosTestA3906wheelSpeedTest(stream, (aos_test_a3906data_t*)test->data, WHEEL_RIGHT, DIRECTION_FORWARD, &result);

  _aosTestA3906wheelSpeedTest(stream, (aos_test_a3906data_t*)test->data, WHEEL_LEFT, DIRECTION_BACKWARD, &result);

  _aosTestA3906wheelSpeedTest(stream, (aos_test_a3906data_t*)test->data, WHEEL_RIGHT, DIRECTION_BACKWARD, &result);

  chprintf(stream, "disable power...\n");
  power_state = A3906_LLD_POWER_OFF;
  status = a3906_lld_set_power(((aos_test_a3906data_t*)test->data)->driver, power_state);
  status |= a3906_lld_get_power(((aos_test_a3906data_t*)test->data)->driver, &power_state);
  status |= (power_state != A3906_LLD_POWER_OFF) ? 0x10 : 0x00;
  qei_count[WHEEL_LEFT][0] = 0;
  qei_count[WHEEL_LEFT][1] = 0;
  qei_count[WHEEL_RIGHT][0] = 0;
  qei_count[WHEEL_RIGHT][1] = 0;
  timeout_counter = 0;
  stable_counter = 0;
  do {
    status |= apalQEIGetPosition(((aos_test_a3906data_t*)test->data)->qei.left, &qei_count[WHEEL_LEFT][0]);
    status |= apalQEIGetPosition(((aos_test_a3906data_t*)test->data)->qei.right, &qei_count[WHEEL_RIGHT][0]);
    aosThdMSleep(1);
    status |= apalQEIGetPosition(((aos_test_a3906data_t*)test->data)->qei.left, &qei_count[WHEEL_LEFT][1]);
    status |= apalQEIGetPosition(((aos_test_a3906data_t*)test->data)->qei.right, &qei_count[WHEEL_RIGHT][1]);
    ++timeout_counter;
    stable_counter = (qei_count[WHEEL_LEFT][0] == qei_count[WHEEL_LEFT][1] && qei_count[WHEEL_RIGHT][0] == qei_count[WHEEL_RIGHT][1]) ? stable_counter+1 : 0;
  } while((stable_counter < 100) && (timeout_counter * MICROSECONDS_PER_MILLISECOND <= ((aos_test_a3906data_t*)test->data)->timeout));
  status |= (timeout_counter * MICROSECONDS_PER_MILLISECOND > ((aos_test_a3906data_t*)test->data)->timeout) ? APAL_STATUS_ERROR | APAL_STATUS_TIMEOUT : APAL_STATUS_OK;
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  // stop the PWM
  a3906_lld_set_pwm(((aos_test_a3906data_t*)test->data)->pwm.driver, ((aos_test_a3906data_t*)test->data)->pwm.channel.left_forward, APAL_PWM_WIDTH_OFF);
  a3906_lld_set_pwm(((aos_test_a3906data_t*)test->data)->pwm.driver, ((aos_test_a3906data_t*)test->data)->pwm.channel.left_backward, APAL_PWM_WIDTH_OFF);
  a3906_lld_set_pwm(((aos_test_a3906data_t*)test->data)->pwm.driver, ((aos_test_a3906data_t*)test->data)->pwm.channel.right_forward, APAL_PWM_WIDTH_OFF);
  a3906_lld_set_pwm(((aos_test_a3906data_t*)test->data)->pwm.driver, ((aos_test_a3906data_t*)test->data)->pwm.channel.right_backward, APAL_PWM_WIDTH_OFF);

  aosTestInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(A3906Driver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
