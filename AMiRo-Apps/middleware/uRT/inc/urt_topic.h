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
 * @file    urt_topic.h
 * @brief   Topic structure and interfaces.
 *
 * @defgroup pubsub_topic Topic
 * @ingroup pubsub
 * @brief   Topics mediate communication.
 * @details Topics act as mediators for publish-subscribe communication.
 *          Publishers pass their message to a topic, which is buffered there.
 *          Then, the topic informs all associated subscribers about the new
 *          message, but does not pass on the data.
 *          Instead, the subscribers have to fetch the data from the buffer
 *          ansynchronously.
 *
 * @addtogroup pubsub_topic
 * @{
 */

#ifndef URT_TOPIC_H
#define URT_TOPIC_H

#include <urt.h>

#if (URT_CFG_PUBSUB_ENABLED == true || defined(__DOXYGEN__))

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

#if (URT_CFG_PUBSUB_TOPIC_BUFFERORDER != URT_PUBSUB_TOPIC_BUFFERORDER_PUBTIME) && \
    (URT_CFG_PUBSUB_TOPIC_BUFFERORDER != URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME)
#error "URT_CFG_PUBSUB_TOPIC_BUFFERORDER set to invalid value."
#endif

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

#if (URT_CFG_PUBSUB_PROFILING  == true) || defined (__DOXYGEN__)
/**
 * @brief   Topic profiling data structure.
 */
typedef struct urt_topic_profilingdata {
  /**
   * @brief   Counter of overall published messages.
   */
  urt_profilingcounter_t numMessagesPublished;

  /**
   * @brief   Value to count how often (non-HRT) subscribers did not fetch a message before it was reused.
   */
  urt_profilingcounter_t numMessagesDiscarded;

  /**
   * @brief   Number of overall subscribers.
   */
  size_t numSubscribers;

} urt_topic_profilingdata_t;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

/**
 * @brief   Type to uniquely identify topics.
 */
#if (URT_CFG_PUBSUB_TOPICID_WIDTH > 0 && URT_CFG_PUBSUB_TOPICID_WIDTH <= 8)
typedef uint8_t urt_topicid_t;
#elif (URT_CFG_PUBSUB_TOPICID_WIDTH > 0 && URT_CFG_PUBSUB_TOPICID_WIDTH <= 16) || defined(__DOXYGEN__)
typedef uint16_t urt_topicid_t;
#elif (URT_CFG_PUBSUB_TOPICID_WIDTH > 0 && URT_CFG_PUBSUB_TOPICID_WIDTH <= 32)
typedef uint32_t urt_topicid_t;
#elif (URT_CFG_PUBSUB_TOPICID_WIDTH > 0 && URT_CFG_PUBSUB_TOPICID_WIDTH <= 64)
typedef uint64_t urt_topicid_t;
#else
#error "URT_CFG_PUBSUB_TOPICID_WIDTH must be in range [1,64]."
#endif

/**
 * @brief   Maximum topic identifier value.
 * @note    Minimum value is always 0.
 */
#if (URT_CFG_PUBSUB_TOPICID_WIDTH < 64) || defined(__DOXYGEN__)
#define URT_PUBSUB_TOPICID_MAXVAL               (((urt_topicid_t)1 << URT_CFG_PUBSUB_TOPICID_WIDTH) - 1)
#else /* (URT_CFG_PUBSUB_TOPICID_WIDTH < 64) */
#define URT_PUBSUB_TOPICID_MAXVAL               ((urt_topicid_t)~0)
#endif /* (URT_CFG_PUBSUB_TOPICID_WIDTH < 64) */

/**
 * @brief   Data structure representing topics.
 */
typedef struct urt_topic
{
  /**
   * @brief   Pointer to the next topic in a singly-linked list.
   */
  struct urt_topic* next;

  /**
   * @brief   Unique Identifier of the topic.
   */
  urt_topicid_t id;

  /**
   * @brief   Mutex lock for exclusive access.
   */
  urt_osMutex_t lock;

  /**
   * @brief   Event source to inform subscribers on new data.
   */
  urt_osEventSource_t evtSource;

  /**
   * @brief   Counter of HRT subscribers currently subscribed to the topic.
   */
  size_t numHrtSubscribers;

  /**
   * @brief   Condition variable to inform any waiting publishers that publishing is possible.
   * @details Publishing new messages is prevented if not all HRT subscribers have processed the oldest message in the list.
   *          Only when a message was processed by all HRT subscribers, it can be reused.
   */
  urt_osCondvar_t hrtReleased;

  /**
   * @brief   First message for the circular message buffer.
   */
  urt_message_t mandatoryMessage;

  /**
   * @brief   Pointer to the message published most recently.
   * @note    This pointer is initialized to the mandatory message and thus will never be NULL.
   */
  urt_message_t* latestMessage;

#if (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME) || defined(__DOXYGEN__)

  /**
   * @brief   Counter of the number of message published via the topic.
   */
  urt_messageid_t messageCounter;

#endif /* (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME) */


#if ((URT_CFG_PUBSUB_QOS_DEADLINECHECKS  == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS  == true)) || defined (__DOXYGEN__)

  /**
   * @brief   Pointer to the first element in a list of associated HRT subscribers.
   * @details If URT_CFG_PUBSUB_QOS_RATECHECKS is true, HRT subscribers are ordered by their expected rate (most critical first).
   *          Otherwise, HRT subscribers are ordered by their deadline (most critical first).
   *          This pointer is NULL if there are no HRT subscribers associated to the topic.
   */
  struct urt_hrtsubscriber* hrtSubscribers;

#if (URT_CFG_PUBSUB_QOS_RATECHECKS == true) || defined(__DOXYGEN__)

#if (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_PUBTIME) || defined(__DOXYGEN__)
  /**
   * @brief   Pointer to the message holding the most recent information.
   * @details Initially, this pointer may be NULL.
   */
  urt_message_t* latestInformation;
#endif /* (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_PUBTIME) */

  /**
   * @brief   QoS timer to check for rate violations.
   */
  urt_osTimer_t qosTimer;
#endif /* (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */

#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS  == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS  == true) */

#if (URT_CFG_PUBSUB_PROFILING  == true) || defined (__DOXYGEN__)
  /**
   * @brief   Profiling data.
   */
  urt_topic_profilingdata_t profiling;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

} urt_topic_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  urt_status_t urtTopicInit(urt_topic_t* topic, urt_topicid_t id, void* payload);
  urt_status_t urtTopicAddMessages(urt_topic_t* topic, urt_message_t* messages);
  size_t urtTopicBufferSize(urt_topic_t* topic);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
#endif /* URT_TOPIC_H */

/** @} */
