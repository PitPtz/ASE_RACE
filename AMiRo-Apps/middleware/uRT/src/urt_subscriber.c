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
 * @file    urt_subscriber.c
 * @brief   Subscriber code.
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

/*
 * forward declarations
 */
urt_status_t _urt_nrtsubscriber_fetchNext(urt_nrtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* origin_time, urt_delay_t* latency);
urt_status_t _urt_nrtsubscriber_fetchLatest(urt_nrtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* origin_time, urt_delay_t* latency);
urt_status_t _urt_nrtsubscriber_unsubscribe(urt_nrtsubscriber_t* subscriber);
urt_status_t _urt_srtsubscriber_fetchNext(urt_srtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* origin_time, urt_delay_t* latency);
urt_status_t _urt_srtsubscriber_fetchLatest(urt_srtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* origin_time, urt_delay_t* latency);
urt_status_t _urt_srtsubscriber_unsubscribe(urt_srtsubscriber_t* subscriber);
urt_status_t _urt_frtsubscriber_fetchNext(urt_frtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* origin_time, urt_delay_t* latency);
urt_status_t _urt_frtsubscriber_fetchLatest(urt_frtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* origin_time, urt_delay_t* latency);
urt_status_t _urt_frtsubscriber_unsubscribe(urt_frtsubscriber_t* subscriber);
urt_status_t _urt_hrtsubscriber_fetchNext(urt_hrtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* origin_time, urt_delay_t* latency);
urt_status_t _urt_hrtsubscriber_fetchLatest(urt_hrtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* origin_time, urt_delay_t* latency);
urt_status_t _urt_hrtsubscriber_unsubscribe(urt_hrtsubscriber_t* subscriber);

/**
 * @brief   Virtual method table for NRT subscribers.
 */
const struct urt_basesubscriber_VMT _urt_nrtsubscriber_vmt = {
  .fetchNext    = (urt_status_t(*)(void*, void*, size_t*, urt_osTime_t*, urt_delay_t*))_urt_nrtsubscriber_fetchNext,
  .fetchLatest  = (urt_status_t(*)(void*, void*, size_t*, urt_osTime_t*, urt_delay_t*))_urt_nrtsubscriber_fetchLatest,
  .unsubscribe  = (urt_status_t(*)(void*))_urt_nrtsubscriber_unsubscribe,
};

/**
 * @brief   Virtual method table for SRT subscribers.
 */
const struct urt_basesubscriber_VMT _urt_srtsubscriber_vmt = {
  .fetchNext    = (urt_status_t(*)(void*, void*, size_t*, urt_osTime_t*, urt_delay_t*))_urt_srtsubscriber_fetchNext,
  .fetchLatest  = (urt_status_t(*)(void*, void*, size_t*, urt_osTime_t*, urt_delay_t*))_urt_srtsubscriber_fetchLatest,
  .unsubscribe  = (urt_status_t(*)(void*))_urt_srtsubscriber_unsubscribe,
};

/**
 * @brief   Virtual method table for FRT subscribers.
 */
const struct urt_basesubscriber_VMT _urt_frtsubscriber_vmt = {
  .fetchNext    = (urt_status_t(*)(void*, void*, size_t*, urt_osTime_t*, urt_delay_t*))_urt_frtsubscriber_fetchNext,
  .fetchLatest  = (urt_status_t(*)(void*, void*, size_t*, urt_osTime_t*, urt_delay_t*))_urt_frtsubscriber_fetchLatest,
  .unsubscribe  = (urt_status_t(*)(void*))_urt_frtsubscriber_unsubscribe,
};

/**
 * @brief   Virtual method table for HRT subscribers.
 */
const struct urt_basesubscriber_VMT _urt_hrtsubscriber_vmt = {
  .fetchNext    = (urt_status_t(*)(void*, void*, size_t*, urt_osTime_t*, urt_delay_t*))_urt_hrtsubscriber_fetchNext,
  .fetchLatest  = (urt_status_t(*)(void*, void*, size_t*, urt_osTime_t*, urt_delay_t*))_urt_hrtsubscriber_fetchLatest,
  .unsubscribe  = (urt_status_t(*)(void*))_urt_hrtsubscriber_unsubscribe,
};

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   Initialize base subscriber memory.
 *
 * @param[in] subscriber  Base subscriber to initialize.
 */
void _urt_basesubscriber_init(urt_basesubscriber_t* const subscriber)
{
  urtDebugAssert(subscriber != NULL);

  subscriber->topic = NULL;
  urtEventListenerInit(subscriber->evtListener);
  subscriber->lastMessage = NULL;
  subscriber->lastMessageID = URT_MESSAGEID_INVALID;
#if (URT_CFG_PUBSUB_PROFILING == true)
  subscriber->profiling.minLatency = URT_DELAY_INFINITE;
  subscriber->profiling.maxLatency = URT_DELAY_IMMEDIATE;
  subscriber->profiling.sumLatencies = 0;
  subscriber->profiling.numMessagesReceived = 0;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

  return;
}

/**
 * @brief   Subscribe to a topic.
 *
 * @param[in] subscriber  The subscriber which shall subscribe to a topic.
 *                        Must not be NULL.
 *                        Must not be associated to a topic.
 * @param[in] topic       The topic to subscribe to.
 *                        Must not be NULL.
 * @param[in] mask        Mask of event where the subscriber registers to.
 *                        Must not be 0.
 */
void _urt_basesubscriber_subscribe(urt_basesubscriber_t* const subscriber, urt_topic_t* const topic, urt_osEventMask_t const mask)
{
  urtDebugAssert(subscriber != NULL && subscriber->topic == NULL);

  // set topic
  subscriber->topic = topic;

  // lock topic
  urtMutexLock(&topic->lock);

  // set last message information
  subscriber->lastMessage = topic->latestMessage;
  subscriber->lastMessageID = topic->latestMessage->id;

  // register to topic's event source
  urtEventRegister(&topic->evtSource, &subscriber->evtListener, mask, 0);

#if (URT_CFG_PUBSUB_PROFILING == true)
  ++topic->profiling.numSubscribers;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

  // unlock topic
  urtMutexUnlock(&topic->lock);

  return;
}

#if (URT_CFG_PUBSUB_PROFILING == true) || defined(__DOXYGEN__)

/**
 * @brief   Track message latencies for profiling.
 *
 * @param[in] subscriber  Subscriber to track the latency for.
 *                        Must not be NULL.
 * @param[in] latency     Latency to track.
 */
void _urt_basesubscriber_trackLatencies(urt_basesubscriber_t* const subscriber, urt_delay_t const latency)
{
  urtDebugAssert(subscriber != NULL);

  // accumulate latencies
  subscriber->profiling.sumLatencies += latency;

  // update min/max latency
  if (subscriber->profiling.minLatency > subscriber->profiling.maxLatency ||
      latency < subscriber->profiling.minLatency) {
    subscriber->profiling.minLatency = latency;
  }
  if (subscriber->profiling.minLatency > subscriber->profiling.maxLatency ||
      latency > subscriber->profiling.maxLatency) {
    subscriber->profiling.maxLatency = latency;
  }

  return;
}

#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

/**
 * @brief   Fetch the next message.
 *
 * @param[in]  subscriber   The subscriber that shall fetch the message.
 *                          Must not be NULL.
 *                          Must be associatied to a topic.
 * @param[out] payload      Pointer where to copy the payload to.
 *                          May be NULL.
 * @param[out] bytes        Pointer where to store payload size in bytes.
 *                          May be NULL.
 * @param[out] time         Pointer where to store the information origin time to.
 *                          May be NULL.
 * @param[out] latency      The latency to be returned by reference (optional).
 *                          Value is only modified if a message was fetched.
 *                          May be NULL.
 *
 * @return  Indicator whether a message has been fetched.
 * @retval true   Successfully fetched a message.
 * @retval false  There is no new message to fetch.
 */
bool _urt_basesubscriber_fetchNext(urt_basesubscriber_t* const subscriber, void* const payload, size_t* const bytes, urt_osTime_t* const time, urt_delay_t* const latency)
{
  urtDebugAssert(subscriber != NULL && subscriber->topic != NULL);

  // lock topic
  urtMutexLock(&subscriber->topic->lock);

  // find the message to fetch
  urt_message_t* message = subscriber->lastMessage;
  // if the identifier of last read message equals local copy
  if (message->id == subscriber->lastMessageID) {
    // if this is still the latest message
    if (subscriber->topic->latestMessage == message) {
      // unlock topic and return
      urtMutexUnlock(&subscriber->topic->lock);
      return false;
    } else {
      // proceed to next message
      message = message->next;
    }
  }
  // the last message has been reused
  else {
    // retrieve the oldest message
    message = subscriber->topic->latestMessage->next;
  }

  // fetch the message
  subscriber->lastMessage = message;
  subscriber->lastMessageID = message->id;
  if (time) *time = message->time;
  if (payload) memcpy(payload, message->payload, message->size);
  if (bytes) *bytes = message->size;
#if (URT_CFG_PUBSUB_PROFILING == true)
  // track profiling information
  urt_osTime_t t;
  urtTimeNow(&t);
  const urt_delay_t lat = (urt_delay_t)urtTimeDiff(&message->time, &t);
  --message->profiling.numConsumersLeft;
  ++subscriber->profiling.numMessagesReceived;
#else /* (URT_CFG_PUBSUB_PROFILING == true) */
  // calculate latency if requested
  if (latency) {
    urt_osTime_t t;
    urtTimeNow(&t);
    *latency = (urt_delay_t)urtTimeDiff(&message->time, &t);
  }
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

  // unlock topic
  urtMutexUnlock(&subscriber->topic->lock);

#if (URT_CFG_PUBSUB_PROFILING == true)
  // track latencies
  _urt_basesubscriber_trackLatencies(subscriber, lat);
  // set optional latency output argument
  if (latency) *latency = lat;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

  return true;
}

/**
 * @brief   Fetch the latest message.
 *
 * @param[in]  subscriber   The subscriber that shall fetch the message.
 *                          Must not be NULL.
 *                          Must be associatied to a topic.
 * @param[out] payload      Pointer where to copy the payload to.
 *                          May be NULL.
 * @param[out] bytes        Pointer where to store payload size in bytes.
 *                          May be NULL.
 * @param[out] time         Pointer where to store the information origin time to.
 *                          May be NULL.
 * @param[out] latency      The latency to be returned by reference (optional).
 *                          Value is only modified if a message was fetched.
 *                          May be NULL.
 *
 * @return  Indicator whether a message has been fetched.
 * @retval true   Successfully fetched a message.
 * @retval false  There is no new message to fetch.
 */
bool _urt_basesubscriber_fetchLatest(urt_basesubscriber_t* const subscriber, void* const payload, size_t* const bytes, urt_osTime_t* const time, urt_delay_t* const latency)
{
  urtDebugAssert(subscriber != NULL && subscriber->topic != NULL);

  // lock topic
  urtMutexLock(&subscriber->topic->lock);

  // check whether there is a new message to fetch
  if (subscriber->topic->latestMessage->id == URT_MESSAGEID_INVALID ||
#if (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME)
    subscriber->lastMessageID == subscriber->topic->messageCounter) {
#elif (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_PUBTIME)
    subscriber->lastMessageID == subscriber->topic->latestMessage->id) {
#endif
    // unlock topic and return
    urtMutexUnlock(&subscriber->topic->lock);
    return false;
  }

  // proceed to the latest message
#if (URT_CFG_PUBSUB_PROFILING == true)
  urt_message_t* message = subscriber->lastMessage->next;
  while (true) {
    --message->profiling.numConsumersLeft;
    ++subscriber->profiling.numMessagesReceived;

    // if there is a more recent message
    if (message != subscriber->topic->latestMessage) {
      // iterate to next message
      message = message->next;
    } else {
      // break the loop
      break;
    }
  }
#else /* (URT_CFG_PUBSUB_PROFILING == true) */
  urt_message_t* message = subscriber->topic->latestMessage;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

  // fetch the message
  subscriber->lastMessage = message;
  subscriber->lastMessageID = message->id;
  if (time) *time = message->time;
  if (payload) memcpy(payload, message->payload, message->size);
  if (bytes) *bytes = message->size;
#if (URT_CFG_PUBSUB_PROFILING == true)
  // track profiling information
  urt_osTime_t t;
  urtTimeNow(&t);
  const urt_delay_t lat = (urt_delay_t)urtTimeDiff(&message->time, &t);
#else /* (URT_CFG_PUBSUB_PROFILING == true) */
  // calculate latency if requested
  if (latency) {
    urt_osTime_t t;
    urtTimeNow(&t);
    *latency = (urt_delay_t)urtTimeDiff(&message->time, &t);
  }
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

  // unlock topic
  urtMutexUnlock(&subscriber->topic->lock);

#if (URT_CFG_PUBSUB_PROFILING == true)
  // track latencies
  _urt_basesubscriber_trackLatencies(subscriber, lat);
  // set optional latency output argument
  if (latency) *latency = lat;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

  return true;
}

/**
 * @brief   Unsubscribe from a topic.
 *
 * @param[in,out] subscriber  The subscriber to be unsubscribed.
 *                            Must not be NULL.
 *                            Must be associatied to a topic.
 */
void _urt_basesubscriber_unsubscribe(urt_basesubscriber_t* const subscriber)
{
  urtDebugAssert(subscriber != NULL && subscriber->topic != NULL);

  // unregister from topic event
  urtEventUnregister(&subscriber->topic->evtSource, &subscriber->evtListener);

#if (URT_CFG_PUBSUB_PROFILING == true)
  urtMutexLock(&subscriber->topic->lock);
  --subscriber->topic->profiling.numSubscribers;
  // decrement subscriber counter for all pending messages
  if (subscriber->lastMessageID != subscriber->topic->latestMessage->id) {
    urt_message_t* message = subscriber->lastMessage;
    do {
      message = message->next;
      --message->profiling.numConsumersLeft;
    } while (message != subscriber->topic->latestMessage);
  }
  urtMutexUnlock(&subscriber->topic->lock);
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

  // reset data
  subscriber->topic = NULL;
  subscriber->lastMessage = NULL;
  subscriber->lastMessageID = URT_MESSAGEID_INVALID;

  return;
}

/* NRT ************************************************************************/

/**
 * @brief   Fetch the next message.
 *
 * @param[in]  subscriber   The NRT subscriber that shall fetch the message.
 *                          Must not be NULL.
 *                          Must be associatied to a topic.
 * @param[out] payload      Pointer where to copy the payload to.
 *                          May be NULL.
 * @param[out] bytes        Pointer where to store payload size in bytes.
 *                          May be NULL.
 * @param[out] time         Pointer where to store the information origin time to.
 *                          May be NULL.
 * @param[out] latency      The latency to be returned by reference (optional).
 *                          Value is only modified if a message was fetched.
 *                          May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Successfully fetched a message.
 * @retval URT_STATUS_FETCH_NOMESSAGE   There is no new message to fetch.
 */
urt_status_t _urt_nrtsubscriber_fetchNext(urt_nrtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* time, urt_delay_t* latency)
{
  urtDebugAssert(subscriber != NULL && subscriber->topic != NULL);

  // fetch next message
  if (_urt_basesubscriber_fetchNext((urt_basesubscriber_t*)subscriber, payload, bytes, time, latency)) {
    return URT_STATUS_OK;
  } else {
    return URT_STATUS_FETCH_NOMESSAGE;
  }
}

/**
 * @brief   Fetch the latest message.
 *
 * @param[in]  subscriber   The NRT subscriber that shall fetch the message.
 *                          Must not be NULL.
 *                          Must be associatied to a topic.
 * @param[out] payload      Pointer where to copy the payload to.
 *                          May be NULL.
 * @param[out] bytes        Pointer where to store payload size in bytes.
 *                          May be NULL.
 * @param[out] time         Pointer where to store the information origin time to.
 *                          May be NULL.
 * @param[out] latency      The latency to be returned by reference (optional).
 *                          Value is only modified if a message was fetched.
 *                          May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Successfully fetched a message.
 * @retval URT_STATUS_FETCH_NOMESSAGE   There is no new message to fetch.
 */
urt_status_t _urt_nrtsubscriber_fetchLatest(urt_nrtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* time, urt_delay_t* latency)
{
  urtDebugAssert(subscriber != NULL && subscriber->topic != NULL);

  // fetch latest message
  if (_urt_basesubscriber_fetchLatest((urt_basesubscriber_t*)subscriber, payload, bytes, time, latency)) {
    return URT_STATUS_OK;
  } else {
    return URT_STATUS_FETCH_NOMESSAGE;
  }
}

/**
 * @brief   Unsubscribe from a topic.
 *
 * @param[in,out] subscriber  The NRT subscriber to be unsubscribed.
 *                            Must not be NULL.
 *                            Must be associatied to a topic.
 *
 * @return  Always returns URT_STATUS_OK.
 */
urt_status_t _urt_nrtsubscriber_unsubscribe(urt_nrtsubscriber_t* subscriber)
{
  urtDebugAssert(subscriber != NULL && subscriber->topic != NULL);

  // unsubscribe from topic
  _urt_basesubscriber_unsubscribe((urt_basesubscriber_t*)subscriber);

  return URT_STATUS_OK;
}

/* SRT ************************************************************************/

/**
 * @brief   Fetch the next message.
 *
 * @param[in]  subscriber   The SRT subscriber that shall fetch the message.
 *                          Must not be NULL.
 *                          Must be associatied to a topic.
 * @param[out] payload      Pointer where to copy the payload to.
 *                          May be NULL.
 * @param[out] bytes        Pointer where to store payload size in bytes.
 *                          May be NULL.
 * @param[out] time         Pointer where to store the information origin time to.
 *                          May be NULL.
 * @param[out] latency      The latency to be returned by reference (optional).
 *                          Value is only modified if a message was fetched.
 *                          May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Successfully fetched a message.
 * @retval URT_STATUS_FETCH_NOMESSAGE   There is no new message to fetch.
 */
urt_status_t _urt_srtsubscriber_fetchNext(urt_srtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* time, urt_delay_t* latency)
{
  urtDebugAssert(subscriber != NULL && subscriber->topic != NULL);

  // fetch next message
  if (_urt_basesubscriber_fetchNext((urt_basesubscriber_t*)subscriber, payload, bytes, time, latency)) {
    return URT_STATUS_OK;
  } else {
    return URT_STATUS_FETCH_NOMESSAGE;
  }
}

/**
 * @brief   Fetch the latest message.
 *
 * @param[in]  subscriber   The SRT subscriber that shall fetch the message.
 *                          Must not be NULL.
 *                          Must be associatied to a topic.
 * @param[out] payload      Pointer where to copy the payload to.
 *                          May be NULL.
 * @param[out] bytes        Pointer where to store payload size in bytes.
 *                          May be NULL.
 * @param[out] time         Pointer where to store the information origin time to.
 *                          May be NULL.
 * @param[out] latency      The latency to be returned by reference (optional).
 *                          Value is only modified if a message was fetched.
 *                          May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Successfully fetched a message.
 * @retval URT_STATUS_FETCH_NOMESSAGE   There is no new message to fetch.
 */
urt_status_t _urt_srtsubscriber_fetchLatest(urt_srtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* time, urt_delay_t* latency)
{
  urtDebugAssert(subscriber != NULL && subscriber->topic != NULL);

  // fetch latest message
  if (_urt_basesubscriber_fetchLatest((urt_basesubscriber_t*)subscriber, payload, bytes, time, latency)) {
    return URT_STATUS_OK;
  } else {
    return URT_STATUS_FETCH_NOMESSAGE;
  }
}

/**
 * @brief   Unsubscribe from a topic.
 *
 * @param[in,out] subscriber  The SRT subscriber to be unsubscribed.
 *                            Must not be NULL.
 *                            Must be associatied to a topic.
 *
 * @return  Always returns URT_STATUS_OK.
 */
urt_status_t _urt_srtsubscriber_unsubscribe(urt_srtsubscriber_t* subscriber)
{
  urtDebugAssert(subscriber != NULL && subscriber->topic != NULL);

  // unsubscribe from topic
  _urt_basesubscriber_unsubscribe((urt_basesubscriber_t*)subscriber);

  return URT_STATUS_OK;
}

/* FRT ************************************************************************/

/**
 * @brief   Fetch the next message.
 *
 * @param[in]  subscriber   The FRT subscriber that shall fetch the message.
 *                          Must not be NULL.
 *                          Must be associatied to a topic.
 * @param[out] payload      Pointer where to copy the payload to.
 *                          May be NULL.
 * @param[out] bytes        Pointer where to store payload size in bytes.
 *                          May be NULL.
 * @param[out] time         Pointer where to store the information origin time to.
 *                          May be NULL.
 * @param[out] latency      The latency to be returned by reference (optional).
 *                          Value is only modified if a message was fetched.
 *                          May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                  Successfully fetched a message.
 * @retval URT_STATUS_FETCH_NOMESSAGE     There is no new message to fetch.
 * @retval URT_STATUS_DEADLINEVIOLATION   Latency violated the maximum expected latency.
 *                                        Deadline violations are checked before jitter violations.
 * @retval URT_STATUS_JITTERVIOLATION     Latency violated the maximum expected jitter.
 *                                        Jitter violations are only checked if no deadline violation was detected.
 */
urt_status_t _urt_frtsubscriber_fetchNext(urt_frtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* time, urt_delay_t* latency)
{
  urtDebugAssert(subscriber != NULL && subscriber->topic != NULL);

  // local variables
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
  urt_delay_t lat;
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

  // fetch next message
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
  if (!_urt_basesubscriber_fetchNext((urt_basesubscriber_t*)subscriber, payload, bytes, time, &lat)) {
#else /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */
  if (!_urt_basesubscriber_fetchNext((urt_basesubscriber_t*)subscriber, payload, bytes, time, latency)) {
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */
    return URT_STATUS_FETCH_NOMESSAGE;
  }

#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true)
  // check for deadline violation
  if (subscriber->deadlineOffset != 0 &&
      lat > subscriber->deadlineOffset) {
    return URT_STATUS_DEADLINEVIOLATION;
  }
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) */

#if (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
  // track jitter and check for violation
  if (subscriber->maxJitter != 0) {
    // update min/max latency only if valid
    if ((subscriber->minLatency > subscriber->maxLatency) ||
        (lat < subscriber->minLatency && subscriber->maxLatency - lat <= subscriber->maxJitter)) {
      subscriber->minLatency = lat;
    }
    if ((subscriber->minLatency > subscriber->maxLatency) ||
        (lat > subscriber->maxLatency && lat - subscriber->minLatency <= subscriber->maxJitter)) {
      subscriber->maxLatency = lat;
    }
    // check for violation
    if (lat < subscriber->minLatency || lat > subscriber->maxLatency) {
      return URT_STATUS_JITTERVIOLATION;
    }
  }
#endif /* (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
  // set optional latency output argument
  if (latency) {
    *latency = lat;
  }
#endif /*(URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */
  return URT_STATUS_OK;
}

/**
 * @brief   Fetch the latest message.
 *
 * @param[in]  subscriber   The FRT subscriber that shall fetch the message.
 *                          Must not be NULL.
 *                          Must be associatied to a topic.
 * @param[out] payload      Pointer where to copy the payload to.
 *                          May be NULL.
 * @param[out] bytes        Pointer where to store payload size in bytes.
 *                          May be NULL.
 * @param[out] time         Pointer where to store the information origin time to.
 *                          May be NULL.
 * @param[out] latency      The latency to be returned by reference (optional).
 *                          Value is only modified if a message was fetched.
 *                          May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                  Successfully fetched a message.
 * @retval URT_STATUS_FETCH_NOMESSAGE     There is no new message to fetch.
 * @retval URT_STATUS_DEADLINEVIOLATION   Latency violated the maximum expected latency.
 *                                        Deadline violations are checked before jitter violations.
 * @retval URT_STATUS_JITTERVIOLATION     Latency violated the maximum expected jitter.
 *                                        Jitter violations are only checked if no deadline violation was detected.
 */
urt_status_t _urt_frtsubscriber_fetchLatest(urt_frtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* time, urt_delay_t* latency)
{
  urtDebugAssert(subscriber != NULL && subscriber->topic != NULL);

  // local variables
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
  urt_delay_t lat;
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

  // fetch latest message
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
  if (!_urt_basesubscriber_fetchLatest((urt_basesubscriber_t*)subscriber, payload, bytes, time, &lat)) {
#else /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */
  if (!_urt_basesubscriber_fetchLatest((urt_basesubscriber_t*)subscriber, payload, bytes, time, latency)) {
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */
    return URT_STATUS_FETCH_NOMESSAGE;
  }

#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true)
  // check for deadline violation
  if (subscriber->deadlineOffset != 0 &&
      lat > subscriber->deadlineOffset) {
    return URT_STATUS_DEADLINEVIOLATION;
  }
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) */

#if (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
  // track jitter and check for violation
  if (subscriber->maxJitter != 0) {
    // update min/max latency only if valid
    if ((subscriber->minLatency > subscriber->maxLatency) ||
        (lat < subscriber->minLatency && subscriber->maxLatency - lat <= subscriber->maxJitter)) {
      subscriber->minLatency = lat;
    }
    if ((subscriber->minLatency > subscriber->maxLatency) ||
        (lat > subscriber->maxLatency && lat - subscriber->minLatency <= subscriber->maxJitter)) {
      subscriber->maxLatency = lat;
    }
    // check for violation
    if (lat < subscriber->minLatency || lat > subscriber->maxJitter) {
      return URT_STATUS_JITTERVIOLATION;
    }
  }
#endif /* (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
  // set optional latency output argument
  if (latency) {
    *latency = lat;
  }
#endif /*(URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

  return URT_STATUS_OK;
}

/**
 * @brief   Unsubscribe from a topic.
 *
 * @param[in,out] subscriber  The FRT subscriber to be unsubscribed.
 *                            Must not be NULL.
 *                            Must be associatied to a topic.
 *
 * @return  Always returns URT_STATUS_OK.
 */
urt_status_t _urt_frtsubscriber_unsubscribe(urt_frtsubscriber_t* subscriber)
{
  urtDebugAssert(subscriber != NULL && subscriber->topic != NULL);

  // unsubscribe from topic
  _urt_basesubscriber_unsubscribe((urt_basesubscriber_t*)subscriber);

  return URT_STATUS_OK;
}

/* HRT ************************************************************************/

#if ((URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)) || defined(__DOXYGEN__)
/**
 * @brief   Wrapper function for executing custom recovery callbacks of HRT
 *          subscribers in case of a deadline or jitter violation.
 *
 * @param[in] subscriber  Pointer to the HRT subscriber of which a timing
 *                        constraint has been violated.
 */
void _urt_hrtsubscriber_deadlinejitterrecoverywrapper(urt_hrtsubscriber_t* subscriber)
{
  // check current violation code
  if (subscriber->recovery.violationCode == URT_STATUS_OK) {
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) && (URT_CFG_PUBSUB_QOS_JITTERCHECKS != true)
    subscriber->recovery.violationCode = URT_STATUS_DEADLINEVIOLATION;
#elif (URT_CFG_PUBSUB_QOS_DEADLINECHECKS != true) && (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
    subscriber->recovery.violationCode = URT_STATUS_JITTERVIOLATION;
#elif (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) && (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
    {
      // calculate critical delay
      const urt_delay_t delay = (subscriber->maxJitter != 0 &&
                                 subscriber->minLatency <= subscriber->maxLatency &&
                                 (subscriber->deadlineOffset == 0 ||
                                  (subscriber->minLatency + subscriber->maxJitter) < subscriber->deadlineOffset)
                                ) ? subscriber->minLatency + subscriber->maxJitter : subscriber->deadlineOffset;
      // set violation code an execute recovery callback
      subscriber->recovery.violationCode = (delay == subscriber->deadlineOffset) ? URT_STATUS_DEADLINEVIOLATION : URT_STATUS_JITTERVIOLATION;
    }
#endif
    subscriber->recovery.callback(subscriber->recovery.params);
  }

  return;
}

/**
 * @brief   Initialize emergency shutdown or recovery attempt from HRT subscriber context.
 *
 * @param[in] subscriber  Pointer to the HRT subscriber of which a timing constraint has been violated.
 *                        Must not be NULL.
 * @param[in] reason      Reson for the emergency shutdown.
 *                        Must be a violation status.
 * @param[in] mutex       Mutex lock to release.
 *                        Must not be NULL.
 * @param[in] condvar     Optional condition variable to signal.
 *                        May be NULL.
 */
void _urt_hrtsubscriber_initPanicOrRecovery(urt_hrtsubscriber_t* subscriber, urt_status_t const reason, urt_osMutex_t* const mutex, urt_osCondvar_t* const condvar)
{
  urtDebugAssert(subscriber != NULL);
  urtDebugAssert(reason == URT_STATUS_DEADLINEVIOLATION || reason == URT_STATUS_JITTERVIOLATION || reason == URT_STATUS_RATEVIOLATION);
  urtDebugAssert(mutex != NULL);

  // check for recovery callback
  if (subscriber->recovery.callback == NULL) {
    // initialize emergency shutdown
    urtCorePanic(reason);
  } else if (subscriber->recovery.violationCode == URT_STATUS_OK) {
    // initialize recovery
    subscriber->recovery.violationCode = reason;
    subscriber->recovery.callback(subscriber->recovery.params);
  }

  // signal condition variable, if any
  if (condvar != NULL) {
    urtCondvarBroadcast(condvar);
  }

  // unlock mutex
  urtMutexUnlock(mutex);

  return;
}
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

#if (URT_CFG_PUBSUB_QOS_RATECHECKS == true) || defined(__DOXYGEN__)
/**
 * @brief   Wrapper function for executing of recovery callbacks of HRT
 *          subscribers in case of a rate violation.
 *
 * @param[in] subscriber  Pointer to the HRT subscriber of which the rate
 *                        constraint has been violated.
 */
void _urt_hrtsubscriber_raterecoverywrapper(urt_hrtsubscriber_t* subscriber)
{
  // restart the QoS rate timer according the next HRT subscriber in the queue
  if (subscriber->next != NULL) {
#if (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME)
    urt_osTime_t time = subscriber->topic->latestMessage->time;
#elif (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_PUBTIME)
    urt_osTime_t time = subscriber->topic->latestInformation->time;
#endif
    urtTimeAdd(&time, subscriber->next->expectedRate);
    if (subscriber->next->recovery.callback == NULL) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
      urtTimerSet(&subscriber->topic->qosTimer, &time, (urt_osTimerCallback_t)urtCorePanic, (void*)URT_STATUS_RATEVIOLATION);
#pragma GCC diagnostic pop
    } else {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
      urtTimerSet(&subscriber->topic->qosTimer, &time, (urt_osTimerCallback_t)_urt_hrtsubscriber_raterecoverywrapper, (void*)(subscriber->next));
#pragma GCC diagnostic pop
    }
  }

  // check current violation code
  if (subscriber->recovery.violationCode == URT_STATUS_OK) {
    // set violation code and execute recovery callback
    subscriber->recovery.violationCode = URT_STATUS_RATEVIOLATION;
    subscriber->recovery.callback(subscriber->recovery.params);
  }

  return;
}
#endif /* (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */

/**
 * @brief   Fetch the next message.
 *
 * @param[in]  subscriber   The HRT subscriber that shall fetch the message.
 *                          Must not be NULL.
 *                          Must be associatied to a topic.
 * @param[out] payload      Pointer where to copy the payload to.
 *                          May be NULL for messages without payload.
 * @param[out] bytes        Pointer where to store payload size in bytes.
 *                          May be NULL.
 * @param[out] time         Pointer where to store the information origin time to.
 *                          May be NULL.
 * @param[out] latency      The latency to be returned by reference (optional).
 *                          Value is only modified if a message was fetched.
 *                          May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                  Successfully fetched a message.
 * @retval URT_STATUS_FETCH_NOMESSAGE     There is no new message to fetch.
 * @retval URT_STATUS_JITTERVIOLATION     Latency violated the maximum expected jitter.
 *                                        Jitter violations are only checked if no deadline violation was detected.
 */
urt_status_t _urt_hrtsubscriber_fetchNext(urt_hrtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* time, urt_delay_t* latency)
{
  urtDebugAssert(subscriber != NULL && subscriber->topic != NULL);

  // lock topic
  urtMutexLock(&subscriber->topic->lock);

  // check whether the last message still is the latest message
  if (subscriber->lastMessageID == subscriber->topic->latestMessage->id) {
    // unlock topic and return
    urtMutexUnlock(&subscriber->topic->lock);
    return URT_STATUS_FETCH_NOMESSAGE;
  }

#if (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME)
  urt_message_t* message;
  // if the last message is still valid
  if (subscriber->lastMessage->id == subscriber->lastMessageID) {
    // iterate to the next message
    message = subscriber->lastMessage->next;
  } else {
    // find the oldest message in the queue
    message = subscriber->topic->latestMessage->next;
    while (message->id == URT_MESSAGEID_INVALID) {
      message = message->next;
    }
  }
#elif (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_PUBTIME)
  // iterate to the next message (may point to itself)
  urt_message_t* const message = subscriber->lastMessage->next;
#endif
  urtDebugAssert(message->numHrtConsumersLeft > 0);

  // fetch the message
  subscriber->lastMessage = message;
  subscriber->lastMessageID = message->id;
  if (time) *time = message->time;
  if (payload) memcpy(payload, message->payload, message->size);
  if (bytes) *bytes = message->size;
  --message->numHrtConsumersLeft;

  // calculate latency
  urt_delay_t lat;
  {
    urt_osTime_t t;
    urtTimeNow(&t);
    lat = (urt_delay_t)urtTimeDiff(&message->time, &t);
  }

#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
  // if the QoS timer was set to the fetched message
  if (subscriber->qosTimerMessage == message) {
    // if there are no further pending messages
    if (message == subscriber->topic->latestMessage) {
      // reset the QoS timer
      urtTimerReset(&subscriber->qosTimer);
      subscriber->qosTimerMessage = NULL;
    }
    // if there are further pending messages
    else {
      // find the pending message with oldest information
      subscriber->qosTimerMessage = message->next;
#if (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_PUBTIME)
      {
        urt_message_t* msg = subscriber->qosTimerMessage;
        while (msg != subscriber->topic->latestMessage) {
          msg = msg->next;
          if (urtTimeGet(&msg->time) < urtTimeGet(&subscriber->qosTimerMessage->time)) {
            subscriber->qosTimerMessage = msg;
          }
        }
      }
#endif /* (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_PUBTIME) */
      /* calculate critical delay
       * NOTE:
       * In this case 'delay' is always greater than 0, because the timer
       * would not have been actiavted in the first place if timing
       * constraints were of no concern.
       */
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) && (URT_CFG_PUBSUB_QOS_JITTERCHECKS != true)
      const urt_delay_t delay = subscriber->deadlineOffset;
#elif (URT_CFG_PUBSUB_QOS_DEADLINECHECKS != true) && (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
      const urt_delay_t delay = (subscriber->maxJitter != 0 && subscriber->minLatency <= subscriber->maxLatency) ? subscriber->minLatency + subscriber->maxJitter : 0;
#elif (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) && (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
      const urt_delay_t delay = (subscriber->maxJitter != 0 &&
                                 subscriber->minLatency <= subscriber->maxLatency &&
                                 (subscriber->deadlineOffset == 0 ||
                                  (subscriber->minLatency + subscriber->maxJitter) < subscriber->deadlineOffset)
                                ) ? subscriber->minLatency + subscriber->maxJitter : subscriber->deadlineOffset;
#endif
      // update QoS time wrt. critical delay
      urt_osTime_t t = subscriber->qosTimerMessage->time;
      urtTimeAdd(&t, delay);
      if (subscriber->recovery.callback == NULL) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) && (URT_CFG_PUBSUB_QOS_JITTERCHECKS != true)
        urtTimerSet(&subscriber->qosTimer, &t, (urt_osTimerCallback_t)urtCorePanic, (void*)URT_STATUS_DEADLINEVIOLATION);
#elif (URT_CFG_PUBSUB_QOS_DEADLINECHECKS != true) && (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
        urtTimerSet(&subscriber->qosTimer, &t, (urt_osTimerCallback_t)urtCorePanic, (void*)URT_STATUS_JITTERVIOLATION);
#elif (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) && (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
        urtTimerSet(&subscriber->qosTimer, &t, (urt_osTimerCallback_t)urtCorePanic, (delay == subscriber->deadlineOffset) ? (void*)URT_STATUS_DEADLINEVIOLATION : (void*)URT_STATUS_JITTERVIOLATION);
#endif
#pragma GCC diagnostic pop
      } else {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
        urtTimerSet(&subscriber->qosTimer, &t, (urt_osTimerCallback_t)_urt_hrtsubscriber_deadlinejitterrecoverywrapper, subscriber);
#pragma GCC diagnostic pop
      }
    }
  }
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

  // set optional latency output argument
  if (latency != NULL) *latency = lat;

#if (URT_CFG_PUBSUB_PROFILING == true)
  // track profiling information
  --message->profiling.numConsumersLeft;
  _urt_basesubscriber_trackLatencies((urt_basesubscriber_t*)subscriber, lat);
  ++subscriber->profiling.numMessagesReceived;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

#if (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
  // track jitter and check for violation
  if (subscriber->maxJitter != 0) {
    // update min/max latency only if valid
    if ((subscriber->minLatency > subscriber->maxLatency) ||
        (lat < subscriber->minLatency && subscriber->maxLatency - lat <= subscriber->maxJitter)) {
      subscriber->minLatency = lat;
    }
    if ((subscriber->minLatency > subscriber->maxLatency) ||
        (lat > subscriber->maxLatency && lat - subscriber->minLatency <= subscriber->maxJitter)) {
      subscriber->maxLatency = lat;
    }
    // check for lower deviation
    if (lat < subscriber->minLatency || lat > subscriber->maxLatency) {
      _urt_hrtsubscriber_initPanicOrRecovery(subscriber, URT_STATUS_JITTERVIOLATION, &subscriber->topic->lock, (message->numHrtConsumersLeft == 0) ? &subscriber->topic->hrtReleased : NULL);
      return URT_STATUS_JITTERVIOLATION;
    }
  }
#endif /* (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)*/

  // signal condition variable if this was the last HRT subscriber to consume the message
  if (message->numHrtConsumersLeft == 0) {
    urtCondvarBroadcast(&subscriber->topic->hrtReleased);
  }

  // unlock topic
  urtMutexUnlock(&subscriber->topic->lock);

  return URT_STATUS_OK;
}

/**
 * @brief   Fetch the latest message.
 *
 * @param[in]  subscriber   The HRT subscriber that shall fetch the message.
 *                          Must not be NULL.
 *                          Must be associatied to a topic.
 * @param[out] payload      Pointer where to copy the payload to.
 *                          May be NULL for messages without payload.
 * @param[out] bytes        Pointer where to store payload size in bytes.
 *                          May be NULL.
 * @param[out] time         Pointer where to store the information origin time to.
 *                          May be NULL.
 * @param[out] latency      The latency to be returned by reference (optional).
 *                          Value is only modified if a message was fetched.
 *                          May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                  Successfully fetched a message.
 * @retval URT_STATUS_FETCH_NOMESSAGE     There is no new message to fetch.
 * @retval URT_STATUS_JITTERVIOLATION     Latency violated the maximum expected jitter.
 *                                        Jitter violations are only checked if no deadline violation was detected.
 */
urt_status_t _urt_hrtsubscriber_fetchLatest(urt_hrtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* time, urt_delay_t* latency)
{
  urtDebugAssert(subscriber != NULL && subscriber->topic != NULL);

  // lock topic
  urtMutexLock(&subscriber->topic->lock);

  // check whether the last message still is the latest message
  if (subscriber->lastMessageID == subscriber->topic->latestMessage->id) {
    // unlock topic and return
    urtMutexUnlock(&subscriber->topic->lock);
    return URT_STATUS_FETCH_NOMESSAGE;
  }

  // proceed to latest message
#if (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME)
  urt_message_t* message;
  // if the last message is still valid
  if (subscriber->lastMessage->id == subscriber->lastMessageID) {
    // iterate to the next message
    message = subscriber->lastMessage->next;
  } else {
    // find the oldest message in the queue
    message = subscriber->topic->latestMessage->next;
    while (message->id == URT_MESSAGEID_INVALID) {
      message = message->next;
    }
  }
#elif (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_PUBTIME)
  urt_message_t* message = subscriber->lastMessage->next;
#endif
  urt_delay_t lat;
  bool hrtreleased = false;
  while (true) {
    urtDebugAssert(message->numHrtConsumersLeft > 0);

    // if this is the latest message
    if (message == subscriber->topic->latestMessage) {
      // fetch the message
      subscriber->lastMessage = message;
      subscriber->lastMessageID = message->id;
      if (time) *time = message->time;
      if (payload) memcpy(payload, message->payload, message->size);
      if (bytes) *bytes = message->size;
    }

    // decrease counter of HRT subscribers
    hrtreleased = (--message->numHrtConsumersLeft == 0) ? true : hrtreleased;

    // calculate latency
    {
      urt_osTime_t t;
      urtTimeNow(&t);
      lat = (urt_delay_t)urtTimeDiff(&message->time, &t);
    }

#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
    // if the QoS timer was set wrt. this message
    if (subscriber->qosTimerMessage == message) {
      // reset the QoS timer
      urtTimerReset(&subscriber->qosTimer);
      subscriber->qosTimerMessage = NULL;
    }
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

    // set optional latency output argument
    if (latency != NULL) *latency = lat;

#if (URT_CFG_PUBSUB_PROFILING == true)
    // track profiling information
    --message->profiling.numConsumersLeft;
    _urt_basesubscriber_trackLatencies((urt_basesubscriber_t*)subscriber, lat);
    ++subscriber->profiling.numMessagesReceived;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

#if (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
    // track jitter and check for violation
    if (subscriber->maxJitter != 0) {
      // update min/max latency only if valid
      if ((subscriber->minLatency > subscriber->maxLatency) ||
          (lat < subscriber->minLatency && subscriber->maxLatency - lat <= subscriber->maxJitter)) {
        subscriber->minLatency = lat;
      }
      if ((subscriber->minLatency > subscriber->maxLatency) ||
          (lat > subscriber->maxLatency && lat - subscriber->minLatency <= subscriber->maxJitter)) {
        subscriber->maxLatency = lat;
      }
      // check for violation
      if (lat < subscriber->minLatency || lat > subscriber->maxLatency) {
        _urt_hrtsubscriber_initPanicOrRecovery(subscriber, URT_STATUS_JITTERVIOLATION, &subscriber->topic->lock, (message->numHrtConsumersLeft == 0) ? &subscriber->topic->hrtReleased : NULL);
        return URT_STATUS_JITTERVIOLATION;
      }
    }
#endif /* (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)*/

    if (message != subscriber->topic->latestMessage) {
      message = message->next;
    } else {
      break;
    }
  }

  // signal condition varibale if this was the last HRT subscriber to consume any message
  if (hrtreleased) {
    urtCondvarBroadcast(&subscriber->topic->hrtReleased);
  }

  // unlock topic
  urtMutexUnlock(&subscriber->topic->lock);

  return URT_STATUS_OK;
}

/**
 * @brief   Unsubscribe from a topic.
 *
 * @param[in,out] subscriber  The HRT subscriber to be unsubscribed.
 *                            Must not be NULL.
 *                            Must be associatied to a topic.
 *
 * @return  Always returns URT_STATUS_OK-
 */
urt_status_t _urt_hrtsubscriber_unsubscribe(urt_hrtsubscriber_t* subscriber)
{
  urtDebugAssert(subscriber != NULL && subscriber->topic != NULL);

  // unregister from topic event
  urtEventUnregister(&subscriber->topic->evtSource, &subscriber->evtListener);

  // lock topic
  urtMutexLock(&subscriber->topic->lock);

#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
     // reset the QoS timer
     urtTimerReset(&subscriber->qosTimer);
     subscriber->qosTimerMessage = NULL;
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

#if (URT_CFG_PUBSUB_QOS_RATECHECKS)
  {
    // remove self from HRT subscribers list
    urt_hrtsubscriber_t* sub = subscriber->topic->hrtSubscribers;
    // this was the most critical HRT subscriber in wrt. rate
    if (sub == subscriber) {
      subscriber->topic->hrtSubscribers = subscriber->next;
      // update QoS rate timer wrt. next most critical HRT subscriber
      if (subscriber->topic->hrtSubscribers != NULL && subscriber->topic->hrtSubscribers->expectedRate != 0) {
#if (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME)
        urt_osTime_t t = subscriber->topic->latestMessage->time;
#elif (URT_CFG_PUBSUB_TOPIC_BUFFERORDER == URT_PUBSUB_TOPIC_BUFFERORDER_PUBTIME)
        urt_osTime_t t = subscriber->topic->latestInformation->time;
#endif
        urtTimeAdd(&t, subscriber->topic->hrtSubscribers->expectedRate);
        if (subscriber->recovery.callback == NULL) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
          urtTimerSet(&subscriber->topic->qosTimer, &t, (urt_osTimerCallback_t)urtCorePanic, (void*)URT_STATUS_RATEVIOLATION);
#pragma GCC diagnostic pop
        } else {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
          urtTimerSet(&subscriber->topic->qosTimer, &t, (urt_osTimerCallback_t)_urt_hrtsubscriber_raterecoverywrapper, (void*)subscriber);
#pragma GCC diagnostic pop
        }
      } else {
        urtTimerReset(&subscriber->topic->qosTimer);
      }
    }
    // this was not the most critical HRT subscriber in wrt. rate
    else {
      // find self in topic's list of HRT subscribers
      while (sub->next != subscriber) {
        sub = sub->next;
        urtDebugAssert(sub != NULL);
      }
      sub->next = subscriber->next;
    }
    subscriber->next = NULL;
  }
#endif /* (URT_CFG_PUBSUB_QOS_RATECHECKS) */

  // consume any remaining messages
  if (subscriber->lastMessageID != subscriber->topic->latestMessage->id) {
    urt_message_t* message = subscriber->lastMessage;
    bool hrtreleased = false;
    do {
      message = message->next;
      urtDebugAssert(message->numHrtConsumersLeft > 0);

      // decrement counter of HRT subscribers
      hrtreleased = (--message->numHrtConsumersLeft == 0) ? true : hrtreleased;

#if (URT_CFG_PUBSUB_PROFILING == true)
     // track profiling information
      --message->profiling.numConsumersLeft;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */
    } while (message != subscriber->topic->latestMessage);

    // signal condition varibale if this was the last HRT subscriber to consume any message
    if (hrtreleased) {
      urtCondvarBroadcast(&subscriber->topic->hrtReleased);
    }
  }

  // update topic meta information
  --subscriber->topic->numHrtSubscribers;
#if (URT_CFG_PUBSUB_PROFILING == true)
  --subscriber->topic->profiling.numSubscribers;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

  // unlock topic
  urtMutexUnlock(&subscriber->topic->lock);

  // reset data
  subscriber->topic = NULL;
  subscriber->lastMessage = NULL;
  subscriber->lastMessageID = URT_MESSAGEID_INVALID;
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS == true)
  subscriber->recovery.callback = NULL;
  subscriber->recovery.params = NULL;
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */

  return URT_STATUS_OK;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

/**
 * @addtogroup pubsub_subscriber_base
 * @{
 */

/**
 * @brief   Retrieve the real-time class of a base subscriber.
 *
 * @param[in] subscriber  The base subscriber to retrieve the real-time class from.
 *                        Must not be NULL.
 *
 * @return  Real-time class of the base subscriber.
 */
urt_rtclass_t urtSubscriberGetRtClass(const urt_basesubscriber_t* const subscriber)
{
  urtDebugAssert(subscriber != NULL);

  if (subscriber->vmt == &_urt_hrtsubscriber_vmt) {
    return URT_HRT;
  } else if (subscriber->vmt == &_urt_frtsubscriber_vmt) {
    return URT_FRT;
  } else if (subscriber->vmt == &_urt_srtsubscriber_vmt) {
    return URT_SRT;
  } else if (subscriber->vmt == &_urt_nrtsubscriber_vmt) {
    return URT_NRT;
  } else {
    urtDebugAssert(false);
    return -1;
  }
}

/** @} */

/* NRT ************************************************************************/

/**
 * @addtogroup pubsub_subscriber_nrt
 * @{
 */

/**
 * @brief   Initialize an NRT subscriber.
 *
 * @param[in] subscriber  The NRT subscriber to initialize.
 *                        Must not be NULL.
 */
void urtNrtSubscriberInit(urt_nrtsubscriber_t* subscriber)
{
  urtDebugAssert(subscriber != NULL);

  // initialize base data
  subscriber->vmt = &_urt_nrtsubscriber_vmt;
  _urt_basesubscriber_init((urt_basesubscriber_t*)subscriber);

  // initialize NRT data
  /* nothing to initialize */

  return;
}

/**
 * @brief   Subscribe to a topic.
 *
 * @param[in] subscriber  The NRT subscriber which shall subscribe to a topic.
 *                        Must not be NULL.
 *                        Must not be associated to a topic.
 * @param[in] topic       The topic to subscribe to.
 *                        Must not be NULL.
 * @param[in] mask        Mask of event where the subscriber registers to.
 *                        Must not be 0.
 *
 * @return  Always returns URT_STATUS_OK.
 */
urt_status_t urtNrtSubscriberSubscribe(urt_nrtsubscriber_t* subscriber, urt_topic_t* topic, urt_osEventMask_t mask)
{
  urtDebugAssert(subscriber != NULL && subscriber->topic == NULL);
  urtDebugAssert(topic != NULL);
  urtDebugAssert(mask != 0);

  // initialize/reset NRT data
  /* nothing to initialize/reset */

  // initialize/reset base data and subscribe
  _urt_basesubscriber_subscribe((urt_basesubscriber_t*)subscriber, topic, mask);

  return URT_STATUS_OK;
}

/** @} */

/* SRT ************************************************************************/

/**
 * @addtogroup pubsub_subscriber_srt
 * @{
 */

/**
 * @brief   Initialize an SRT subscriber.
 *
 * @param[in] subscriber  The SRT subscriber to initialize.
 *                        Must not be NULL.
 */
void urtSrtSubscriberInit(urt_srtsubscriber_t* subscriber)
{
  urtDebugAssert(subscriber != NULL);

  // initialize base data
  subscriber->vmt = &_urt_srtsubscriber_vmt;
  _urt_basesubscriber_init((urt_basesubscriber_t*)subscriber);

  // initialize SRT data
  subscriber->usefulnesscb = NULL;
  subscriber->cbparams = NULL;

  return;
}

/**
 * @brief   Subscribe to a topic.
 *
 * @param[in] subscriber    The SRT subscriber which shall subscribe to a topic.
 *                          Must not be NULL.
 *                          Must not be associated to a topic.
 * @param[in] topic         The topic to subscribe to.
 *                          Must not be NULL.
 * @param[in] mask          Mask of event where the subscriber registers to.
 *                          Must not be 0.
 * @param[in] usefulnesscb  Pointer to a function to calculate the usefulness of a message.
 *                          Must not be NULL.
 * @param[in] cbparams      Optional parameters for the usefulness callback.
 *                          May be NULL if the callback expects no parameters.
 *
 * @return  Always returns URT_STATUS_OK.
 */
urt_status_t urtSrtSubscriberSubscribe(urt_srtsubscriber_t* subscriber,
                                       urt_topic_t* topic,
                                       urt_osEventMask_t mask,
                                       urt_usefulness_f usefulnesscb,
                                       void* cbparams)
{
  urtDebugAssert(subscriber != NULL && subscriber->topic == NULL);
  urtDebugAssert(topic != NULL);
  urtDebugAssert(mask != 0);
  urtDebugAssert(usefulnesscb != NULL);

  // initialize/reset SRT data
  subscriber->usefulnesscb = usefulnesscb;
  subscriber->cbparams = cbparams;

  // initialize/reset base data and subscribe
  _urt_basesubscriber_subscribe((urt_basesubscriber_t*)subscriber, topic, mask);
  return URT_STATUS_OK;
}

/** @} */

/* FRT ************************************************************************/

/**
 * @addtogroup pubsub_subscriber_frt
 * @{
 */

/**
 * @brief   Initialize an FRT subscriber.
 *
 * @param[in] subscriber  The FRT subscriber to initialize.
 *                        Must not be NULL.
 */
void urtFrtSubscriberInit(urt_frtsubscriber_t* subscriber)
{
  urtDebugAssert(subscriber != NULL);

  // initialize base data
  subscriber->vmt = &_urt_frtsubscriber_vmt;
  _urt_basesubscriber_init((urt_basesubscriber_t*)subscriber);

  // initialize FRT data
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true)
  subscriber->deadlineOffset = 0;
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) */
#if (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
  subscriber->maxJitter = 0;
  subscriber->minLatency = URT_DELAY_INFINITE;
  subscriber->maxLatency = URT_DELAY_IMMEDIATE;
#endif /* (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

  return;
}

/**
 * @brief   Subscribe to a topic.
 *
 * @param[in] subscriber  The FRT subscriber which shall subscribe to a topic.
 *                        Must not be NULL.
 *                        Must not be associated to a topic.
 * @param[in] topic       The topic to subscribe to.
 *                        Must not be NULL.
 * @param[in] mask        Mask of event where the subscriber registers to.
 *                        Must not be 0.
 * @param[in] deadline    Maximum latency to consume messages.
 *                        A value of 0 indicates that latency is of no concern.
 * @param[in] jitter      Maximum allowed jitter (difference between maximum and minimum latency) when consuming messages.
 *                        A value of 0 indicates that jitter is of no concern.
 *
 * @return  Always returns URT_STATUS_OK.
 */
urt_status_t urtFrtSubscriberSubscribe(urt_frtsubscriber_t* subscriber, urt_topic_t* topic,
                                       urt_osEventMask_t mask,
                                       urt_delay_t deadline, urt_delay_t jitter)
{
  urtDebugAssert(subscriber != NULL && subscriber->topic == NULL);
  urtDebugAssert(topic != NULL);
  urtDebugAssert(mask != 0);

  // initialize/reset FRT data
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true)
  subscriber->deadlineOffset = deadline;
#else /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) */
  (void)deadline;
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) */
#if (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
  subscriber->maxJitter = jitter;
  subscriber->minLatency = URT_DELAY_INFINITE;
  subscriber->maxLatency = URT_DELAY_IMMEDIATE;
#else /* (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */
  (void)jitter;
#endif /* (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

  // initialize/reset base data and subscribe
  _urt_basesubscriber_subscribe((urt_basesubscriber_t*)subscriber, topic, mask);

  return URT_STATUS_OK;
}

/**
 * @brief   Calculate validity of a message.
 *
 * @param[in] subscriber  The FRT subscriber to calculate a validity for.
 *                        Must not be NULL.
 * @param[in] latency     Latency (of a message) as argument to calculate validity.
 *
 * @return  Returns a boolean indicator whether the latency is fine.
 */
bool urtFrtSubscriberCalculateValidity(urt_frtsubscriber_t* subscriber, urt_delay_t latency)
{
  urtDebugAssert(subscriber != NULL);

#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)

  bool validity = true;

#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true)
  if (subscriber->deadlineOffset != 0 &&
      latency > subscriber->deadlineOffset) {
    validity = false;
  }
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) */

#if (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
  if ((subscriber->maxJitter != 0) && (subscriber->minLatency <= subscriber->maxLatency) &&
      (((latency > subscriber->minLatency) && ((latency - subscriber->minLatency) > subscriber->maxJitter)) ||
       ((latency < subscriber->maxLatency) && ((subscriber->maxLatency - latency) > subscriber->maxJitter)))) {
    validity = false;
  }
#endif /* (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

  return validity;

#else /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

  (void)latency;
  return true;

#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */
}

/** @} */

/* HRT ************************************************************************/

/**
 * @addtogroup pubsub_subscriber_hrt
 * @{
 */

/**
 * @brief   Initialize an HRT subscriber.
 *
 * @param[in] subscriber  The HRT subscriber to initialize.
 *                        Must not be NULL.
 */
void urtHrtSubscriberInit(urt_hrtsubscriber_t* subscriber)
{
  urtDebugAssert(subscriber != NULL);

  // initialize base data
  subscriber->vmt = &_urt_hrtsubscriber_vmt;
  _urt_basesubscriber_init((urt_basesubscriber_t*)subscriber);

  // initialize HRT data
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS == true)
  subscriber->next = NULL;
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
  urtTimerInit(&subscriber->qosTimer);
  subscriber->qosTimerMessage = NULL;
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true)
  subscriber->deadlineOffset = 0;
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) */
#if (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
  subscriber->maxJitter = 0;
  subscriber->minLatency = URT_DELAY_INFINITE;
  subscriber->maxLatency = URT_DELAY_IMMEDIATE;
#endif /* (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */
#if (URT_CFG_PUBSUB_QOS_RATECHECKS == true)
  subscriber->expectedRate = 0;
#endif /* (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS == true)
  subscriber->recovery.callback = NULL;
  subscriber->recovery.params = NULL;
  subscriber->recovery.violationCode = URT_STATUS_OK;
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */

  return;
}

/**
 * @brief   Subscribe to a topic.
 *
 * @param[in] subscriber  The HRT subscriber which shall subscribe to a topic.
 *                        Must not be NULL.
 *                        Must not be associated to a topic.
 * @param[in] topic       The topic to subscribe to.
 *                        Must not be NULL.
 * @param[in] mask        Mask of event where the subscriber registers to.
 *                        Must not be 0.
 * @param[in] deadline    Maximum latency to consume messages.
 *                        A value of 0 indicates that latency is of no concern.
 * @param[in] jitter      Maximum allowed jitter (difference between maximum and minimum latency) when consuming messages.
 *                        A value of 0 indicates that jitter is of no concern.
 * @param[in] rate        Expected minimum rate of new messages (= maximum time between consecutive messages).
 *                        A value of 0 indicates, that rate is of no concern.
 * @param[in] recoverycb  Callback to be executed when a timing constraint has been violated.
 *                        If NULL, urtCorePanic() will be called instead.
 * @param[in] params      Optional parameters to be passed to the recovery callback.
 *
 * @return  Always returns URT_STATUS_OK.
 */
urt_status_t urtHrtSubscriberSubscribe(urt_hrtsubscriber_t* subscriber, urt_topic_t* topic,
                                       urt_osEventMask_t mask,
                                       urt_delay_t deadline, urt_delay_t jitter, urt_delay_t rate,
                                       urt_hrtsubscriber_recovery_f recoverycb, void* params)
{
  urtDebugAssert(subscriber != NULL && subscriber->topic == NULL);
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS == true)
  urtDebugAssert(subscriber->next == NULL);
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
  urtDebugAssert(subscriber->qosTimerMessage == NULL);
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */
  urtDebugAssert(topic != NULL);
  urtDebugAssert(mask != 0);

  // initialize/reset HRT data
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true)
  subscriber->deadlineOffset = deadline;
#else /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) */
  (void)deadline;
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) */
#if (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
  subscriber->maxJitter = jitter;
  subscriber->minLatency = URT_DELAY_INFINITE;
  subscriber->maxLatency = URT_DELAY_IMMEDIATE;
#else /* (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */
  (void)jitter;
#endif /* (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */
#if (URT_CFG_PUBSUB_QOS_RATECHECKS == true)
  subscriber->expectedRate = rate;
#else /* (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */
  (void)rate;
#endif /* (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS == true)
  subscriber->recovery.callback = recoverycb;
  subscriber->recovery.params = params;
  subscriber->recovery.violationCode = URT_STATUS_OK;
#else /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */
  (void)recoverycb;
  (void)params;
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */

  // initialize/reset base data
  subscriber->topic = topic;

  // lock topic
  urtMutexLock(&topic->lock);

  // set last message information
  subscriber->lastMessage = topic->latestMessage;
  subscriber->lastMessageID = topic->latestMessage->id;

#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS  == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS  == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS  == true)
  // enqueue in HRT subscriber list
  urt_hrtsubscriber_t* sub = topic->hrtSubscribers;
  if (sub == NULL) {
    topic->hrtSubscribers = subscriber;
  } else {
#if (URT_CFG_PUBSUB_QOS_RATECHECKS == true)
    while (sub->next != NULL && sub->next->expectedRate < subscriber->expectedRate) {
#elif (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true)
    while (sub->next != NULL && sub->next->deadlineOffset < subscriber->deadlineOffset) {
#elif (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
    while (sub->next != NULL && sub->next->maxJitter < subscriber->maxJitter) {
#endif
      sub = sub->next;
    }
    subscriber->next = sub->next;
    sub->next = subscriber;
  }
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS  == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS  == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS  == true) */

  // register to topic's event source
  urtEventRegister(&topic->evtSource, &subscriber->evtListener, mask, 0);

  ++topic->numHrtSubscribers;
#if (URT_CFG_PUBSUB_PROFILING == true)
  ++topic->profiling.numSubscribers;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

  // unlock topic
  urtMutexUnlock(&topic->lock);

  return URT_STATUS_OK;
}

/** @} */

#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
