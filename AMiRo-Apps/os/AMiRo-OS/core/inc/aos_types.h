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
 * @file    aos_types.h
 * @brief   Data types used throughout AMiRo-OS
 *
 * @addtogroup core_types
 * @{
 */

#ifndef AMIROOS_TYPES_H
#define AMIROOS_TYPES_H

#include <stdint.h>
#include <ch.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief   Success: no error or warning occurred.
 */
#define AOS_OK                                  ((aos_status_t)0x00u)

/**
 * @brief   Success: no error or warning occurred.
 */
#define AOS_SUCCESS                             AOS_OK

/**
 * @brief   Some unspecified waring occurred.
 */
#define AOS_WARNING                             ((aos_status_t)0x01u)

/**
 * @brief   Invalid arguments detected.
 */
#define AOS_INVALIDARGUMENTS                    ((aos_status_t)0x02u)

/**
 * @brief   A timeout occurred.
 */
#define AOS_TIMEOUT                             ((aos_status_t)0x03u)

/**
 * @brief   An error occurred.
 */
#define AOS_ERROR                               ((aos_status_t)(1 << ((sizeof(aos_status_t) * 8) - 1)))

/**
 * @brief   An error occurred.
 */
#define AOS_FAILURE                             AOS_ERROR

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
 * @brief   Status type generally used in AMiRo-OS.
 * @details By definition, the most significant bit (MSB) indicates whether an
 *          error occurred (e.g. the function call failed). All other bits can
 *          be used to specify the issue in more detail. If the MSB is unset,
 *          all other bits can still be used to indicate and specify
 *          non-critical warnings. As a result, a value of 0 (no bit set)
 *          indicates a flawles success.
 */
typedef uint8_t aos_status_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/**
 * @brief   Converts a GPIO pad number to a unique event flag.
 */
#define AOS_GPIOEVENT_FLAG(pad)                 ((eventflags_t)1 << pad)

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROOS_TYPES_H */

/** @} */
