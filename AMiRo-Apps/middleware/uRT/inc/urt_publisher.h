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
 * @file    urt_publisher.h
 * @brief   Publisher structure and interfaces.
 *
 * @defgroup pubsub_publisher Publisher
 * @ingroup pubsub
 * @brief   Publishers emit messages.
 * @details Publishers emit messages via an associated topic, thereby setting
 *          all meta information (e.g. time) and payload.
 *
 * @addtogroup pubsub_publisher
 * @{
 */

#ifndef URT_PUBLISHER_H
#define URT_PUBLISHER_H

#include <urt.h>

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)

#include <string.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief   Flag indicating whether the urtPublisherPublish() function supports a timeout argument.
 */
#define URT_PUBSUB_PUBLISHER_PUBLISH_TIMEOUT    URT_CFG_OSAL_CONDVAR_TIMEOUT

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

#if (URT_CFG_PUBSUB_PROFILING == true) || defined (__DOXYGEN__)
/**
 * @brief   Publisher profiling data structure.
 */
typedef struct urt_publisher_profilingdata {
  /**
   * @brief   Counter of attempts to publish a message.
   */
  urt_profilingcounter_t publishAttempts;

  /**
   * @brief   Counter of failed publish attempts.
   */
  urt_profilingcounter_t publishFails;

} urt_publisher_profilingdata_t;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

/**
 * @brief   Publisher data structure.
 */
typedef struct urt_publisher
{
  /**
   * @brief   Pointer to the associated topic.
   */
  urt_topic_t* topic;

#if (URT_CFG_PUBSUB_PROFILING == true) || defined (__DOXYGEN__)
  /**
   * @brief   Profiling data.
   */
  urt_publisher_profilingdata_t profiling;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

} urt_publisher_t;

/**
 * @brief   Policies for publishing messages.
 */
typedef enum urt_publisher_publishpolicy
{
  URT_PUBLISHER_PUBLISH_LAZY,         /**< Abort if a locked mutex or a blocking HRT subscriber is detected. */
  URT_PUBLISHER_PUBLISH_DETERMINED,   /**< Abort only if a blocking HRT subscriber is detected. */
  URT_PUBLISHER_PUBLISH_ENFORCING,    /**< Always publish, even if it takes a while. */
} urt_publisher_publishpolicy_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void urtPublisherInit(urt_publisher_t* publisher, urt_topic_t* topic);
#if (URT_PUBSUB_PUBLISHER_PUBLISH_TIMEOUT == true) || defined(__DOXYGEN__)
  urt_status_t urtPublisherPublish(urt_publisher_t* publisher, void const* payload, size_t bytes, urt_osTime_t const* t, urt_publisher_publishpolicy_t policy, urt_delay_t timeout);
#else /* (URT_PUBSUB_PUBLISHER_PUBLISH_TIMEOUT == true) */
  urt_status_t urtPublisherPublish(urt_publisher_t* publisher, void const* payload, size_t bytes, urt_osTime_t const* t, urt_publisher_publishpolicy_t policy);
#endif /* (URT_PUBSUB_PUBLISHER_PUBLISH_TIMEOUT == true) */
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
#endif /* URT_PUBLISHER_H */

/** @} */
