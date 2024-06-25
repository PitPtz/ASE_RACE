/*
µRT is a lightweight publish-subscribe & RPC middleware for real-time
applications. It was developed as part of the software habitat for the
Autonomous Mini Robot (AMiRo) but can be used for other purposes as well.

Copyright (C) 2018..2022  Thomas Schöpping et al.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    urt_primitives.h
 * @brief   Macros used for the middleware.
 *
 * @defgroup kernel_primitives Primitives
 * @ingroup kernel
 * @brief   Definitions of primitive types used by µRT.
 * @details µRT defines several primitive types, which are utilized throughout
 *          the middleware.
 *          Those types are called primitives, because they are aliases of
 *          fundamental types (e.g. int or float), enumerators and function
 *          pointers.
 *
 * @addtogroup kernel_primitives
 * @{
 */

#ifndef URT_PRIMITIVES_H
#define URT_PRIMITIVES_H

#include <urt.h>
#include <stdint.h>

/*============================================================================*/
/* CONSTANTS                                                                  */
/*============================================================================*/

/**
 * @brief Special constant value for immediate (nonexistent) delays.
 */
#define URT_DELAY_IMMEDIATE                     (urt_delay_t)0

/**
 * @brief Special constant value for infinite delays.
 */
#define URT_DELAY_INFINITE                      (urt_delay_t)~0

/*============================================================================*/
/* SETTINGS                                                                   */
/*============================================================================*/

/*============================================================================*/
/* CHECKS                                                                     */
/*============================================================================*/

/*============================================================================*/
/* DATA STRUCTURES AND TYPES                                                  */
/*============================================================================*/

/**
 * @brief   Type for temporal delays (in microseconds).
 */
#if (URT_CFG_DELAY_WIDTH == 32) || defined(__DOXYGEN__)
typedef uint32_t urt_delay_t;
#elif (URT_CFG_DELAY_WIDTH == 64)
typedef uint64_t urt_delay_t;
#else
# error "URT_CFG_DELAY_WIDTH must be set to 32 or 64."
#endif

/**
 * @brief   General status type used throughout µRT and its interfaces.
 */
typedef enum urt_status {
  URT_STATUS_OK                 = 0x00, /**< Status indicating success. */
  URT_STATUS_DEADLINEVIOLATION  = 0x10, /**< Status indicating a violated deadline. */
  URT_STATUS_JITTERVIOLATION    = 0x11, /**< Status indicating a violated jitter requirement. */
  URT_STATUS_RATEVIOLATION      = 0x12, /**< Status indicating a violated rate requirement. */
  URT_STATUS_SYNC_PENDING       = 0x20, /**< Status indicating a synchronization barrier. */
  URT_STATUS_SYNC_ERROR         = 0x21, /**< Status indicating a failed synchronization. */
  URT_STATUS_NODE_INVALEVTMASK  = 0x22, /**< Status indicating an invalid event mask. */
#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
  URT_STATUS_TOPIC_DUPLICATE    = 0xE0, /**< Status indicating multiple use of the same topic ID. */
  URT_STATUS_PUBLISH_LOCKED     = 0xE1, /**< Status indicating a currently locked topic. */
  URT_STATUS_PUBLISH_BLOCKED    = 0xE2, /**< Status indicating a pending HRT subscriber. */
#if (URT_CFG_OSAL_CONDVAR_TIMEOUT == true) || defined(__DOXYGEN__)
  URT_STATUS_PUBLISH_TIMEOUT    = 0xE3, /**< Status indicating a timeout when publishinh a message. */
#endif /* (URT_CFG_OSAL_CONDVAR_TIMEOUT == true) */
#if (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME) || defined(__DOXYGEN__)
  URT_STATUS_PUBLISH_OBSOLETE   = 0xE4, /**< Status indicating that the message was older than the oldest message in the buffer. */
#endif /* (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME) */
  URT_STATUS_FETCH_NOMESSAGE    = 0xE5, /**< Status indicating that no message could be fetched. */
#endif /* URT_CFG_PUBSUB_ENABLED == true */
#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
  URT_STATUS_SERVICE_DUPLICATE  = 0xF0, /**< Status indicating multiple use of the same service ID. */
  URT_STATUS_REQUEST_BADOWNER   = 0xF1, /**< Status indicating that a request is currently owned by another component. */
  URT_STATUS_REQUEST_LOCKED     = 0xF2, /**< Status indicating a currently locked request. */
  URT_STATUS_REQUEST_PENDING    = 0xF3, /**< Status indicating a pending request. */
  URT_STATUS_REQUEST_OBSOLETE   = 0xF4, /**< Status indicating an obsolete (re-submitted) request, which does not match the original request anymore. */
#endif /* URT_CFG_RPC_ENABLED == true */
} urt_status_t;

#if ((URT_CFG_PUBSUB_ENABLED == true) || (URT_CFG_RPC_ENABLED == true)) || defined(__DOXYGEN__)

/**
 * @brief   Classifyiers of several real-time classes.
 */
typedef enum urt_rtclass {
  URT_NRT,  /**< non real-time */
  URT_SRT,  /**< soft real-time */
  URT_FRT,  /**< firm real-time */
  URT_HRT,  /**< hard real-time */
} urt_rtclass_t;

/**
 * @brief   Usefulness calculation function type.
 *
 * @param[in] dt      Delay to calculate the usefulness for.
 * @param[in] params  Pointer to optional parameters (may be NULL).
 *
 * return   The usefulness (of some data) after the specified delay as value in range [0, 1].
 */
typedef float (*urt_usefulness_f)(urt_delay_t dt, void* params);

#endif  /* (URT_CFG_PUBSUB_ENABLED == true) || (URT_CFG_RPC_ENABLED == true) */

#if ((URT_CFG_PUBSUB_ENABLED == true) && (URT_CFG_PUBSUB_PROFILING == true)) || \
    ((URT_CFG_RPC_ENABLED == true) && (URT_CFG_RPC_PROFILING == true)) ||       \
    defined(__DOXYGEN__)

/**
 * @brief   Counter type used for profiling data.
 */
#if (URT_CFG_PROFILING_COUNTER_WIDTH == 8)
typedef uint8_t urt_profilingcounter_t;
#elif (URT_CFG_PROFILING_COUNTER_WIDTH == 16)
typedef uint16_t urt_profilingcounter_t;
#elif (URT_CFG_PROFILING_COUNTER_WIDTH == 32)
typedef uint32_t urt_profilingcounter_t;
#elif (URT_CFG_PROFILING_COUNTER_WIDTH == 64) || defined(__DOXYGEN__)
typedef uint64_t urt_profilingcounter_t;
#endif

#endif /* ((URT_CFG_PUBSUB_ENABLED == true) && (URT_CFG_PUBSUB_PROFILING == true)) || ((URT_CFG_RPC_ENABLED == true) && (URT_CFG_RPC_PROFILING == true)) */

/*============================================================================*/
/* MACROS                                                                     */
/*============================================================================*/

/*============================================================================*/
/* EXTERN DECLARATIONS                                                        */
/*============================================================================*/

/*============================================================================*/
/* INLINE FUNCTIONS                                                           */
/*============================================================================*/

#endif /* URT_PRIMITIVES_H */

/** @} */
