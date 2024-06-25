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
 * @file    aos_debug.h
 * @brief   Macros used for debugging.
 *
 * @addtogroup core_debug
 * @{
 */

#ifndef AMIROOS_DEBUG_H
#define AMIROOS_DEBUG_H

#include <aosconf.h>
#include "aos_system.h"
#include <ch.h>

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

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

#if (AMIROOS_CFG_DBG == true) || defined(__DOXYGEN__)

/**
 * @brief   Printf function for messages only printed in debug builds.
 *
 * @param[in] fmt   Formatted string to print.
 */
#define aosDbgPrintf(fmt, ...)        aosprintf(fmt, ##__VA_ARGS__)

/**
 * @brief   Flush debug output in debug builds.
 */
#define aosDbgFlush()                 aosflush()

/**
 * @brief   Function parameters check.
 * @details If the condition check fails, the thread complains and exits.
 *
 * @param[in] c The condition to be verified to be true.
 */
#define aosDbgCheck(c) do {                                                     \
  if (!(c)) {                                                                   \
    aosDbgPrintf("%s(%u): aosDbgCheck failed", __FILE__, __LINE__);             \
    chThdExit(MSG_RESET);                                                       \
  }                                                                             \
} while (false)

/**
 * @brief   Condition assertion.
 * @details If the condition check fails, the thread complains and exits.
 *
 * @param[in] c The condition to be verified to be true.
 */
#define aosDbgAssert(c) do {                                                    \
  if (!(c)) {                                                                   \
    aosDbgPrintf("%s(%u): aosDbgAssert failed", __FILE__, __LINE__);            \
    chThdExit(MSG_RESET);                                                       \
  }                                                                             \
} while (false)

/**
 * @brief   Condition assertion.
 * @details If the condition check fails, the thread complains and exits.
 *
 * @param[in] c     The condition to be verified to be true.
 * @param[in] fmt   A custom remark string.
 */
#define aosDbgAssertMsg(c, fmt, ...) do {                                       \
  if (!(c)) {                                                                   \
    aosDbgPrintf(fmt, ##__VA_ARGS__);                                           \
    chThdExit(MSG_RESET);                                                       \
  }                                                                             \
} while (false)

#else /* (AMIROOS_CFG_DBG == true) */

#define aosDbgPrintf(fmt, ...)                                                  \
  (void)(fmt)

#define aosDbgFlush()

#define aosDbgCheck(c)                                                          \
  (void)(c)

#define aosDbgAssert(c)                                                         \
  (void)(c)

#define aosDbgAssertMsg(c, r)                                                   \
  (void)(c);                                                                    \
  (void)(r)

#endif /* (AMIROOS_CFG_DBG == true) */

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROOS_DEBUG_H */

/** @} */
