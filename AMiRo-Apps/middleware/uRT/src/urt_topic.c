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
 * @file    urt_topic.c
 * @brief   Topic code.
 */

#include <urt.h>

#if (URT_CFG_PUBSUB_ENABLED == true || defined(__DOXYGEN__))

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED VARIABLES                                                         */
/******************************************************************************/

/******************************************************************************/
/* LOCAL TYPES                                                                */
/******************************************************************************/

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

/**
 * @addtogroup pubsub_topic
 * @{
 */

/**
 * @brief   Initialize the topic.
 *
 * @param[in] topic     The topic to initialize.
 *                      Must not be NULL.
 * @param[in] id        Numeric identifier of the topic.
 *                      Must be unique.
 * @param[in] payload   Payload for the first mandatory message.
 *                      May be NULL for messages without payload.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Topic was sucessfully added to the µRT core.
 * @retval URT_STATUS_TOPIC_DUPLICATE   Another topic with the same identifier already exists.
 */
urt_status_t urtTopicInit(urt_topic_t* topic, urt_topicid_t id, void* payload)
{
  urtDebugAssert(topic != NULL);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
  urtDebugAssert(id <= URT_PUBSUB_TOPICID_MAXVAL);
#pragma GCC diagnostic pop

  // initialize topic data
  topic->next = NULL;
  topic->id = id;
  urtMutexInit(&topic->lock);
  urtEventSourceInit(&topic->evtSource);
  topic->numHrtSubscribers = 0;
  urtCondvarInit(&topic->hrtReleased);
  urtMessageInit(&topic->mandatoryMessage, payload);
  topic->latestMessage = &topic->mandatoryMessage;
#if (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME)
  topic->messageCounter = 0;
#endif /* (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME) */
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS  == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS == true)
  topic->hrtSubscribers = NULL;
#if (URT_CFG_PUBSUB_QOS_RATECHECKS == true)
#if (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_PUBTIME)
  topic->latestInformation = NULL;
#endif /* (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_PUBTIME) */
  urtTimerInit(&topic->qosTimer);
#endif /* (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS  == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */
#if (URT_CFG_PUBSUB_PROFILING == true)
  topic->profiling.numMessagesPublished = 0;
  topic->profiling.numMessagesDiscarded = 0;
  topic->profiling.numSubscribers = 0;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

  // close circular message buffer
  topic->latestMessage->next = topic->latestMessage;

  // add topic to core
  return urtCoreAddTopic(topic);
}

/**
 * @brief   Add messages to the message buffer.
 *
 * @param[in] topic     Topic to which the messages shall be added.
 *                      Must not be NULL.
 * @param[in] messages  NULL terminated list of messages to be added to the circular buffer.
 *                      Messages must not be associated to another topic.
 *                      Once a message has been added, it can not be removed later.
 *                      Must not be NULL.
 *
 * @return  Always returns URT_STATUS_OK.
 */
urt_status_t urtTopicAddMessages(urt_topic_t* topic, urt_message_t* messages)
{
  urtDebugAssert(topic != NULL);
  urtDebugAssert(messages != NULL);

  // find last message in the list
  urt_message_t* last = messages;
  urtDebugAssert(last->id == URT_MESSAGEID_INVALID);
  while (last->next != NULL) {
    last = last->next;
    urtDebugAssert(last->id == URT_MESSAGEID_INVALID);
    urtDebugAssert(last != messages);
  }

  // insert messages in circular message buffer
  urtMutexLock(&topic->lock);
  last->next = topic->latestMessage->next;
  topic->latestMessage->next = messages;
  urtMutexUnlock(&topic->lock);

  return URT_STATUS_OK;
}

/**
 * @brief   Retrieve the size of the circular message buffer.
 *
 * @param[in] topic   Topic to retrieve the buffer size of.
 *                    Must not be NULL.
 *
 * @return  Size of the circular message buffer.
 */
size_t urtTopicBufferSize(urt_topic_t* topic)
{
  urtDebugAssert(topic != NULL);

  // local variables
  size_t size = 1;

  // lock topic
  urtMutexLock(&topic->lock);

  // count messages in the circular buffer
  for (urt_message_t* msg = topic->latestMessage->next; msg != topic->latestMessage; msg = msg->next) {
    ++size;
  }

  // unlock topic
  urtMutexUnlock(&topic->lock);

  return size;
}

/** @} */

#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
