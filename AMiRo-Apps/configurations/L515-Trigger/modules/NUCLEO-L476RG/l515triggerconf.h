/*
AMiRo-Apps is a collection of applications and configurations for the
Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2018..2022  Thomas Schöpping et al.

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
 * @file    l515triggerconf.h
 * @brief   Intel RealSense L515 trigger configuration.
 *
 * @addtogroup configs_l515trigger_modules_nucleol476rg
 * @{
 */

#ifndef L515_TRIGGER_CONF_H
#define L515_TRIGGER_CONF_H

#include <periphAL.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief   Interval at which to switch GPIO activations.
 * @note    Must be at least 110ms according to Intel documentation:
 *          https://dev.intelrealsense.com/docs/lidar-camera-l515-multi-camera-setup#section-e-external-trigger
 */
#define L515TRIGGER_CFG_INTERVAL                  (125 * MICROSECONDS_PER_MILLISECOND)

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

static apalGpio_t l515trigger_gpios[] = {
  {PAL_LINE(GPIOC, GPIOC_PIN10)},
  {PAL_LINE(GPIOC, GPIOC_PIN11)},
  {PAL_LINE(GPIOC, GPIOC_PIN12)},
  {PAL_LINE(GPIOD, GPIOC_PIN2)},
};

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* L515_TRIGGER_CONF_H */

/** @} */
