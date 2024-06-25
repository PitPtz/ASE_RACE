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
 * @file    aos_test_DW1000.h
 * @brief   UWB transceiver driver test types and function declarations.
 *
 * @defgroup test_DW1000_v1 DW1000 (v1)
 * @ingroup test
 * @brief   UWB transceiver driver test.
 *
 * @addtogroup test_DW1000_v1
 * @{
 */

#ifndef AMIROOS_TEST_DW1000_H
#define AMIROOS_TEST_DW1000_H

#include <aosconf.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include <alld_DW1000.h>
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
#if (BOARD_MIC9404x_CONNECTED == true)
typedef struct {
  /**
   * @brief   Pointer to the driver to use.
   */
  DW1000Driver* driver;

  /**
   * @brief   Event source to listen to.
   */
  event_source_t* evtsource;

  /**
   * @brief   Event flags to watch.
   */
  eventflags_t evtflags;

  /**
   * @brief MIC9404x driver to use.
  */
  MIC9404xDriver  *mic9404xd;

} aos_test_dw1000data_t;

#else
typedef struct {
  /**
   * @brief   Pointer to the driver to use.
   */
  DW1000Driver* driver;

  /**
   * @brief   Event source to listen to.
   */
  event_source_t* evtsource;

  /**
   * @brief   Event flags to watch.
   */
  eventflags_t evtflags;

} aos_test_dw1000data_t;

#endif /* BOARD_MIC9404x_CONNECTED == true  */

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  aos_testresult_t aosTestDw1000Func(BaseSequentialStream* stream, const aos_test_t* test);

  extern uint8_t s1switch;
  extern int instance_anchaddr;
  extern int dr_mode;
  extern int chan, tagaddr, ancaddr;
  extern int instance_mode;

  int UWB_Init(DW1000Driver* drv);
  int32_t inittestapplication(uint8_t s1switch, DW1000Driver* drv);
  int decarangingmode(uint8_t s1switch);
  void addressconfigure(uint8_t s1switch, uint8_t mode);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

#endif /* AMIROOS_TEST_DW1000_H */

/** @} */
