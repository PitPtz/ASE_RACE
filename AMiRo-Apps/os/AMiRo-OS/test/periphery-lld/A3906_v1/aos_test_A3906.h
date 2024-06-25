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
 * @file    aos_test_A3906.h
 * @brief   Motor driver test types and function declarations.
 *
 * @defgroup test_A3906_v1 A3906 (v1)
 * @ingroup test
 * @brief   Motor driver test.
 *
 * @addtogroup test_A3906_v1
 * @{
 */

#ifndef AMIROOS_TEST_A3906_H
#define AMIROOS_TEST_A3906_H

#include <aosconf.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include <alld_A3906.h>
#include "core/inc/aos_test.h"

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
 * @brief   Custom data structure for the test.
 */
typedef struct {
  /**
   * @brief   Pointer to the driver to use.
   */
  A3906Driver* driver;

  /**
   * @brief   PWM driver information.
   */
  struct {
    /**
     * @brief   The PWM driver to use.
     */
    apalPWMDriver_t* driver;

    /**
     * @brief   PWM channel information.
     */
    struct {
      /**
     * @brief   PWM channel for the left wheel forward direction.
     */
      apalPWMchannel_t left_forward;

      /**
     * @brief   PWM channel for the left wheel backward direction.
     */
      apalPWMchannel_t left_backward;

      /**
     * @brief   PWM channel for the right wheel forward direction.
     */
      apalPWMchannel_t right_forward;

      /**
     * @brief   PWM channel for the right wheel backward direction.
     */
      apalPWMchannel_t right_backward;
    } channel;
  } pwm;

  /**
   * @brief   QEI driver information
   */
  struct {
    /**
     * @brief   QEI driver for the left wheel.
     */
    apalQEIDriver_t* left;

    /**
     * @brief   QEI driver for the right wheel.
     */
    apalQEIDriver_t* right;

    /**
     * @brief   QEI increments per wheel revolution.
     */
    apalQEICount_t increments_per_revolution;

    /**
     * @brief   Threshold for QEI variations.
     * @details Differences smaller than or equal to this value are neglected (interpreted as zero).
     *          The value can be interpreted as encoder ticks per second (tps).
     */
    apalQEICount_t variation_threshold;
  } qei;

  /**
   * @brief   Wheel diameter information.
   */
  struct {
    /**
     * @brief   Left wheel diameter in m.
     */
    float left;

    /**
     * @brief   Right wheel diameter in m.
     */
    float right;
  } wheel_diameter;

  /**
   * @brief   Timeout value (in us).
   */
  apalTime_t timeout;
} aos_test_a3906data_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  aos_testresult_t aosTestA3906Func(BaseSequentialStream* stream, const aos_test_t* test);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

#endif /* AMIROOS_TEST_A3906_H */

/** @} */
