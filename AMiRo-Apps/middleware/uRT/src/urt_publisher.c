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
 * @file    urt_publisher.c
 * @brief   Publisher code.
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

#if ((URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)) || defined(__DOXYGEN__)
extern void _urt_hrtsubscriber_deadlinejitterrecoverywrapper(urt_hrtsubscriber_t* subscriber);
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

#if (URT_CFG_PUBSUB_QOS_RATECHECKS == true) || defined(__DOXYGEN__)
extern void _urt_hrtsubscriber_raterecoverywrapper(urt_hrtsubscriber_t* subscriber);
#endif /* (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */

/**
 * @brief   Publish a message.
 *
 * @param[in] topic     Pointer to the topic to publish on.
 *                      Must not be NULL.
 * @param[in] payload   Pointer to the data to be published.
 *                      May be NULL for messages without payload.
 * @param[in] bytes     Size of the payload in bytes.
 * @param[in] t         Timestamp to be set for the message information.
 *                      Must not be NULL.
 */
void _urt_publisher_publish(urt_topic_t* const topic, void const* const payload, size_t const bytes, urt_osTime_t const* t)
{
  // local variables
  urt_message_t* const message = topic->latestMessage->next;
  urtDebugAssert((payload != NULL && bytes > 0) || (payload == NULL && bytes == 0));
  urtDebugAssert(payload == NULL || (payload != NULL && message->payload != NULL));

#if (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME)
  // if the new message will become the latest message
  if (topic->latestMessage->id == URT_MESSAGEID_INVALID || urtTimeGet(t) >= urtTimeGet(&topic->latestMessage->time)) {
    // proceed topic's message pointer
    topic->latestMessage = message;
  }
  else {
    // detach oldest message and enqueue it according to the specified timestamp
    topic->latestMessage->next = message->next;
    urt_message_t* msg = topic->latestMessage;
    while (msg->next->id == URT_MESSAGEID_INVALID || urtTimeGet(t) >= urtTimeGet(&msg->next->time)) {
      msg = msg->next;
    }
    // enqueue the message begind 'msg'
    message->next = msg->next;
    msg->next = message;
  }
#endif /* (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME) */

#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS == true)
  // get first HRT subscriber from topic (may be NULL)
  urt_hrtsubscriber_t* hrtsubscriber = topic->hrtSubscribers;

#if (URT_CFG_PUBSUB_QOS_RATECHECKS == true)
  // if the content of the message is newer than the newest so far
#if (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME)
  if (topic->latestMessage == message) {
#elif (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_PUBTIME)
  if (topic->latestInformation == NULL || urtTimeGet(t) >= urtTimeGet(&topic->latestInformation->time)) {
    // update topic'c information pointer
    topic->latestInformation = message;
#endif
    // update QoS rate time wrt. most critical HRT subscriber (first in the list)
    if (hrtsubscriber != NULL && hrtsubscriber->expectedRate != 0) {
      urt_osTime_t time = *t;
      urtTimeAdd(&time, hrtsubscriber->expectedRate);
      if (hrtsubscriber->recovery.callback == NULL) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
        urtTimerSet(&topic->qosTimer, &time, (urt_osTimerCallback_t)urtCorePanic, (void*)URT_STATUS_RATEVIOLATION);
#pragma GCC diagnostic pop
      } else {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
        urtTimerSet(&topic->qosTimer, &time, (urt_osTimerCallback_t)_urt_hrtsubscriber_raterecoverywrapper, hrtsubscriber);
#pragma GCC diagnostic pop
      }
    }
  }
#endif /* (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) ||(URT_CFG_PUBSUB_QOS_RATECHECKS == true) */

  // setup the message
#if (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME)
  ++topic->messageCounter;
  if (topic->messageCounter == URT_MESSAGEID_INVALID) {
    ++topic->messageCounter;
  }
  message->id = topic->messageCounter;
#elif (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_PUBTIME)
  message->id = topic->latestMessage->id + 1;
  if (message->id == URT_MESSAGEID_INVALID) {
    ++message->id;
  }
#endif
  message->time = *t;
  memcpy(message->payload, payload, bytes);
  message->size = bytes;
#if (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME)
  message->numHrtConsumersLeft = (message == topic->latestMessage) ? topic->numHrtSubscribers : message->next->numHrtConsumersLeft;
#elif (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_PUBTIME)
  message->numHrtConsumersLeft = topic->numHrtSubscribers;
#endif
#if (URT_CFG_PUBSUB_PROFILING == true)
  ++message->profiling.utilizations;
  // track whether any (non-HRT) subscribers did not process the message yet.
  if (message->profiling.numConsumersLeft > 0) {
    ++topic->profiling.numMessagesDiscarded;
  }
  message->profiling.numConsumersLeft = topic->profiling.numSubscribers;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

#if (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_PUBTIME)
  // proceed topic's message pointer
  topic->latestMessage = message;
#endif /* (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_PUBTIME) */

#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
  // update QoS timers for all HRT subscribers
  for (; hrtsubscriber != NULL; hrtsubscriber = hrtsubscriber->next) {
    // calculate critical delay
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) && (URT_CFG_PUBSUB_QOS_JITTERCHECKS != true)
    const urt_delay_t delay = hrtsubscriber->deadlineOffset;
#elif (URT_CFG_PUBSUB_QOS_DEADLINECHECKS != true) && (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
    const urt_delay_t delay = (hrtsubscriber->maxJitter != 0 && hrtsubscriber->minLatency <= hrtsubscriber->maxLatency) ? hrtsubscriber->minLatency + hrtsubscriber->maxJitter : 0;
#elif (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) && (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
    const urt_delay_t delay = (hrtsubscriber->maxJitter != 0 &&
                               hrtsubscriber->minLatency <= hrtsubscriber->maxLatency &&
                               (hrtsubscriber->deadlineOffset == 0 ||
                                (hrtsubscriber->minLatency + hrtsubscriber->maxJitter) < hrtsubscriber->deadlineOffset)
                              ) ? hrtsubscriber->minLatency + hrtsubscriber->maxJitter : hrtsubscriber->deadlineOffset;
#endif
    /* only update the timer if
     * - there are any timing constraints and
     * - either
     *   - the timer is not armed (no critical message set) or
     *   - the new message is older than the most critical message so far
     */
    if (delay != 0 &&
        (hrtsubscriber->qosTimerMessage == NULL || urtTimeGet(&hrtsubscriber->qosTimerMessage->time) > urtTimeGet(t))) {
      urt_osTime_t time = *t;
      urtTimeAdd(&time, delay);
      if (hrtsubscriber->recovery.callback == NULL) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) && (URT_CFG_PUBSUB_QOS_JITTERCHECKS != true)
        urtTimerSet(&hrtsubscriber->qosTimer, &time, (urt_osTimerCallback_t)urtCorePanic, (void*)URT_STATUS_DEADLINEVIOLATION);
#elif (URT_CFG_PUBSUB_QOS_DEADLINECHECKS != true) && (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
        urtTimerSet(&hrtsubscriber->qosTimer, &time, (urt_osTimerCallback_t)urtCorePanic, (void*)URT_STATUS_JITTERVIOLATION);
#elif (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) && (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
        urtTimerSet(&hrtsubscriber->qosTimer, &time, (urt_osTimerCallback_t)urtCorePanic, (delay == hrtsubscriber->deadlineOffset) ? (void*)URT_STATUS_DEADLINEVIOLATION : (void*)URT_STATUS_JITTERVIOLATION);
#endif
#pragma GCC diagnostic pop
      } else {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
        urtTimerSet(&hrtsubscriber->qosTimer, &time, (urt_osTimerCallback_t)_urt_hrtsubscriber_deadlinejitterrecoverywrapper, hrtsubscriber);
#pragma GCC diagnostic pop
      }
      hrtsubscriber->qosTimerMessage = message;
    }
  }
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

#if (URT_CFG_PUBSUB_PROFILING == true)
  // track the number of messages published on the topic
  ++topic->profiling.numMessagesPublished;
#endif /* (URT_CFG_PUBSUB_PROFILING == true)*/

  // fire topic event
  urtEventSourceBroadcast(&topic->evtSource, 0);

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

/**
 * @addtogroup pubsub_publisher
 * @{
 */

/**
 * @brief   Initialize the publisher.
 *
 * @param[in] publisher   The publisher to initialize.
 *                        Must not be NULL.
 * @param[in] topic       The topic, this publisher is associated to.
 *                        Must not be NULL.
 */
void urtPublisherInit(urt_publisher_t* publisher, urt_topic_t* topic)
{
  urtDebugAssert(publisher!= NULL);
  urtDebugAssert(topic != NULL);

  // initialize publisher data
  publisher->topic = topic;
#if (URT_CFG_PUBSUB_PROFILING == true)
  publisher->profiling.publishAttempts = 0;
  publisher->profiling.publishFails = 0;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

  return;
}

/**
 * @brief   Publish a message.
 *
 * @param[in] publisher   Pointer to the publisher to use.
 *                        Must not be NULL.
 * @param[in] payload     Pointer to the data to be published.
 *                        May be NULL for messages without payload.
 * @param[in] bytes       Size of the payload in bytes.
 * @param[in] t           Timestamp to be set for the message information.
 *                        Must not be NULL.
 * @param[in] policy      Policy how to publish the message.
 * @param[in] timeout     Timeout delay in case the message cannot be published.
 *                        Not available if URT_PUBSUB_PUBLISHER_PUBLISH_TIMEOUT is false.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Message was published successfully.
 * @retval URT_STATUS_PUBLISH_LOCKED    Associated topic is currently locked.
 *                                      Only occurs if the policy was set to URT_PUBLISHER_PUBLISH_LAZY.
 * @retval URT_STATUS_PUBLISH_BLOCKED   A pending HRT subscriber prohibits reutilization of the next message.
 *                                      Only occurs if the policy was not set to URT_PUBLISHER_PUBLISH_ENFORCING.
 * @retval URT_STATUS_PUBLISH_TIMEOUT   Timeout occurred.
 *                                      Can only occur if URT_PUBSUB_PUBLISHER_PUBLISH_TIMEOUT is true.
 * @retval URT_STATUS_PUBLISH_OBSOLETE  The information carried by the message is too old.
 *                                      Can only occur if URT_CFG_PUBSUB_TOPIC_BUFFERORDER is URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME.
 */
#if (URT_PUBSUB_PUBLISHER_PUBLISH_TIMEOUT == true) || defined(__DOXYGEN__)
  urt_status_t urtPublisherPublish(urt_publisher_t* publisher, void const* payload, size_t bytes, urt_osTime_t const* t, urt_publisher_publishpolicy_t policy, urt_delay_t timeout)
#else /* (URT_PUBSUB_PUBLISHER_PUBLISH_TIMEOUT == true) */
  urt_status_t urtPublisherPublish(urt_publisher_t* publisher, void const* payload, size_t bytes, urt_osTime_t const* t, urt_publisher_publishpolicy_t policy)
#endif /* (URT_PUBSUB_PUBLISHER_PUBLISH_TIMEOUT == true) */
{
  urtDebugAssert(publisher != NULL);
  urtDebugAssert(payload != NULL || (payload == NULL && bytes == 0));
  urtDebugAssert(t != NULL);

#if (URT_CFG_PUBSUB_PROFILING == true)
  ++publisher->profiling.publishAttempts;
#endif /* (URT_CFG_PUBSUB_PROFILING  == true) */

  // lock topic
  if (policy != URT_PUBLISHER_PUBLISH_LAZY) {
    urtMutexLock(&publisher->topic->lock);
  } else {
    if (!urtMutexTryLock(&publisher->topic->lock)) {
#if (URT_CFG_PUBSUB_PROFILING == true)
      ++publisher->profiling.publishFails;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */
      return URT_STATUS_PUBLISH_LOCKED;
    }
  }

  // access next message of the topic (i.e. message to be published on)
  {
    urt_message_t* message = publisher->topic->latestMessage->next;
    urt_messageid_t messageid = message->id;

    // check for HRT consumers left for the message
    if (message->numHrtConsumersLeft > 0 && policy != URT_PUBLISHER_PUBLISH_ENFORCING) {
      // abort publish attempt
      urtMutexUnlock(&publisher->topic->lock);
#if (URT_CFG_PUBSUB_PROFILING == true)
      ++publisher->profiling.publishFails;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */
      return URT_STATUS_PUBLISH_BLOCKED;
    }
    while (message->numHrtConsumersLeft > 0) {
#if (URT_PUBSUB_PUBLISHER_PUBLISH_TIMEOUT == true)
      // wait for topic condition variable to be signaled or time out
      if (urtCondvarWait(&publisher->topic->hrtReleased, &publisher->topic->lock, timeout) == URT_CONDVAR_WAITSTATUS_TIMEOUT) {
        // abort publish attempt on timeout
        urtMutexUnlock(&publisher->topic->lock);
#if (URT_CFG_PUBSUB_PROFILING == true)
        ++publisher->profiling.publishFails;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */
        return URT_STATUS_PUBLISH_TIMEOUT;
      }
#else /* (URT_PUBSUB_PUBLISHER_PUBLISH_TIMEOUT == true) */
      // wait for topic condition variable to be signaled
      urtCondvarWait(&publisher->topic->hrtReleased, &publisher->topic->lock);
#endif /* (URT_PUBSUB_PUBLISHER_PUBLISH_TIMEOUT == true) */

      // check for a modified identifier of the current message (another publisher used this message)
      if (message->id != messageid) {
        // access next message of the topic (i.e. message to be published on)
        message = publisher->topic->latestMessage->next;
        messageid = message->id;
      }
    }

#if (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME)
    // if the timestamp of the information to be published is older than the oldest message in the queue
    if (message->id != URT_MESSAGEID_INVALID && urtTimeGet(t) < urtTimeGet(&message->time)) {
      // abort publish attempt
      urtMutexUnlock(&publisher->topic->lock);
#if (URT_CFG_PUBSUB_PROFILING == true)
      ++publisher->profiling.publishFails;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */
      return URT_STATUS_PUBLISH_OBSOLETE;
    }
#endif /* (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME) */
  }

  // publish message
  _urt_publisher_publish(publisher->topic, payload, bytes, t);

  // release topic
  urtMutexUnlock(&publisher->topic->lock);

  return URT_STATUS_OK;
}

/** @} */

#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
