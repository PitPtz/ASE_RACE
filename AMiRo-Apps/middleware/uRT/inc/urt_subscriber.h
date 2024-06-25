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
 * @file    urt_subscriber.h
 * @brief   Subscriber interface.
 *
 * @defgroup pubsub_subscriber Subscriber
 * @ingroup pubsub
 * @brief   Subscribers receive messages.
 * @details When subscribed to a topic, subscribers are informed about new
 *          messages and the node thread the subscriber resides in is signaled.
 *          Actual fetching of messages from the topics message buffer, however,
 *          is executed asynchronously.
 *
 * @defgroup pubsub_subscriber_base Base Subscriber
 * @ingroup pubsub_subscriber
 * @brief   Subscriber base type.
 * @details All specialized subscribers share this base type.
 *
 * @defgroup pubsub_subscriber_nrt NRT Subscriber
 * @ingroup pubsub_subscriber
 * @brief   Subscriber with no real-time constraints.
 * @details NRT Subscribers hold no real-time constraints at all.
 *          This makes them the smallest and most performant option, but they
 *          offer no timing guarantiees at all.
 *
 * @defgroup pubsub_subscriber_srt SRT Subscriber
 * @ingroup pubsub_subscriber
 * @brief   Subscriber with soft real-time constraints.
 * @details Similarly efficient as NRT subscribers, SRT subscribers provide an
 *          API to calculate the usefulness of a message, depending on its age.
 *
 * @defgroup pubsub_subscriber_frt FRT Subscriber
 * @ingroup pubsub_subscriber
 * @brief   Subscribers with firm real-time constraints.
 * @details Slightly less efficient than NRT and SRT subscribers, strict timing
 *          constraints can be set for FRT subscribers.
 *          Whenever these are violated, the message is considered invalid.
 *
 * @defgroup pubsub_subscriber_hrt HRT Subscriber
 * @ingroup pubsub_subscriber
 * @brief   Subscribers with hard real-time constraints.
 * @details This is the least efficient subscriber type, which behaves similarly
 *          to FRT subscribers.
 *          If timing constraints are violated, however, this is considered a
 *          severe system failure and a µRT panic is provoked immediately.
 *
 * @defgroup pubsub_subscriber_xrt Subscriber
 * @ingroup pubsub_subscriber
 * @brief   General subscriber type.
 * @details This general subscriber type encompasses an NRT, SRT, FRT and HRT
 *          subscriber and thus offers high flexibility.
 *          In terms of memory utilization, however, the specialized types are
 *          typically more efficient.
 */

#ifndef URT_SUBSCRIBER_H
#define URT_SUBSCRIBER_H

#include <urt.h>

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)

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
 * forward declarations
 * @cond
 */
typedef struct urt_topic urt_topic_t;
typedef struct urt_basesubscriber urt_basesubscriber_t;
typedef struct urt_nrtsubscriber urt_nrtsubscriber_t;
typedef struct urt_srtsubscriber urt_srtsubscriber_t;
typedef struct urt_frtsubscriber urt_frtsubscriber_t;
typedef struct urt_hrtsubscriber urt_hrtsubscriber_t;
typedef union urt_subscriber urt_subscriber_t;
/** @endcond */

/**
 * @brief   Recovery callback function type for HRT subscribers.
 *
 * @details   Recovery callbacks can be used to initiate recovery attempts in
 *            case some timing violation occurred. However, callbacks are only
 *            executed if the current value of the subscriber's violationCode
 *            member is URT_STATUS_OK.
 *            If a recovery has been initiated, it is the responsibility of the
 *            recovery logic to reset that variable to this value.
 *
 * @param[in] params  Pointer to optional parameters.
 */
typedef void (*urt_hrtsubscriber_recovery_f)(void* params);

#if (URT_CFG_PUBSUB_PROFILING == true) || defined (__DOXYGEN__)
/**
 * @brief   Base subscriber profiling data structure.
 * @ingroup pubsub_subscriber_base
 */
typedef struct urt_basesubscriber_profilingdata {
  /**
   * @brief   Minimum latency ever detected.
   */
  urt_delay_t minLatency;

  /**
   * @brief   Maximum latency ever detected.
   */
  urt_delay_t maxLatency;

  /**
   * @brief   Sum of all latencies.
   */
  uint64_t sumLatencies;

  /**
   * @brief   Counter of the number of messages received.
   */
  urt_profilingcounter_t numMessagesReceived;

} urt_basesubscriber_profilingdata_t;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

/**
 * @brief   Base subscriber specific methods.
 */
#define _urt_basesubscriber_methods                                             \
  /* Fetch the next pending message. */                                         \
  urt_status_t (*fetchNext)(void* instance, void* payload, size_t* bytes, urt_osTime_t* origin_time, urt_delay_t* latency); \
  /* Fetch the latest pending message. */                                       \
  urt_status_t (*fetchLatest)(void* instance, void* payload, size_t* bytes, urt_osTime_t* origin_time, urt_delay_t* latency); \
  /* Unsubscribe from a topic. */                                               \
  urt_status_t (*unsubscribe)(void* istance);

/**
 * @brief   Base subscriber virtual method table.
 * @ingroup pubsub_subscriber_base
 */
struct urt_basesubscriber_VMT {
  _urt_basesubscriber_methods
};

/**
 * @brief   Base subscriber data.
 * @details Common to all subscriber types (NRT, SRT, FRT and HRT).
 */
#define _urt_basesubscriber_data                                                \
  /* Pointer to the associated topic or NULL. */                                \
  struct urt_topic* topic;                                                      \
  /* Event listener for topic events. */                                        \
  urt_osEventListener_t evtListener;                                            \
  /* Pointer to the last message read. */                                       \
  urt_message_t* lastMessage;                                                   \
  /* Copy of the identifier of the last message read. */                        \
  urt_messageid_t lastMessageID;                                                \
  /* profiling data */                                                          \
  _urt_basesubscriber_profilingdata;

#if (URT_CFG_PUBSUB_PROFILING == true) || defined (__DOXYGEN__)
/**
 * @brief   Base subscriber profiling related data.
 */
#define _urt_basesubscriber_profilingdata                                       \
  urt_basesubscriber_profilingdata_t profiling;
#else /* (URT_CFG_PUBSUB_PROFILING == true) */
#define _urt_basesubscriber_profilingdata
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

/**
 * @brief   Base subscriber structure, common to all subscriber types (NRT, SRT, FRT and HRT).
 * @struct  urt_basesubscriber
 * @ingroup pubsub_subscriber_base
 */
struct urt_basesubscriber {
  const struct urt_basesubscriber_VMT* vmt; /**< Virtual method table. */
  _urt_basesubscriber_data
};

/**
 * @extends urt_basesubscriber
 *
 * @brief   NRT subscriber structure.
 * @ingroup pubsub_subscriber_nrt
 */
struct urt_nrtsubscriber
{
  const struct urt_basesubscriber_VMT* vmt; /**< Virtual method table. */
  _urt_basesubscriber_data
};

/**
 * @extends urt_basesubscriber
 *
 * @brief   SRT subscriber structure.
 * @ingroup pubsub_subscriber_srt
 */
struct urt_srtsubscriber
{
  const struct urt_basesubscriber_VMT* vmt; /**< Virtual method table. */
  _urt_basesubscriber_data

  /**
   * @brief   Calback function to calculate usefulness of messages.
   */
  urt_usefulness_f usefulnesscb;

  /**
   * @brief   Pointer to optional parameters for the usefullness callback.
   */
  void* cbparams;

};

/**
 * @extends urt_basesubscriber
 *
 * @brief   FRT subscriber structure.
 * @ingroup pubsub_subscriber_frt
 */
struct urt_frtsubscriber
{
  const struct urt_basesubscriber_VMT* vmt; /**< Virtual method table. */
  _urt_basesubscriber_data

#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS) || defined (__DOXYGEN__)
  /**
   * @brief   Maximum expected latency.
   * @note    A value of 0 indicates, that latency is of no concern.
   */
  urt_delay_t deadlineOffset;
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) */

#if (URT_CFG_PUBSUB_QOS_JITTERCHECKS) || defined (__DOXYGEN__)
  /**
   * @brief   Maximum expected jitter.
   * @note    A value of 0 indicates, that jitter is of no concern.
   */
  urt_delay_t maxJitter;

  /**
   * @brief   Minimum valid latency ever detected.
   */
  urt_delay_t minLatency;

  /**
   * @brief   Maximum valid latency ever detected.
   */
  urt_delay_t maxLatency;
#endif /* (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

};

/**
 * @extends urt_basesubscriber
 *
 * @brief   HRT subscriber structure.
 * @ingroup pubsub_subscriber_hrt
 */
struct urt_hrtsubscriber
{
  const struct urt_basesubscriber_VMT* vmt; /**< Virtual method table. */
  _urt_basesubscriber_data

#if ((URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS == true)) || defined (__DOXYGEN__)
  /**
   * @brief   Pointer to the next HRT subscriber in a singly-linked list.
   */
  struct urt_hrtsubscriber* next;
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */

#if ((URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)) || defined (__DOXYGEN__)
  /**
   * @brief   QoS timer to detect latency or jitter violations.
   */
  urt_osTimer_t qosTimer;

  /**
   * @brief   Pointer to the message, which defined the currently set timer.
   * @note    Is NULL if the timer is not armed.
   */
  urt_message_t* qosTimerMessage;
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || defined (__DOXYGEN__)
  /**
   * @brief   Maximum expected latency.
   * @note    A value of 0 indicates, that latency is of no concern.
   */
  urt_delay_t deadlineOffset;
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) */

#if (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || defined (__DOXYGEN__)
  /**
   * @brief   Maximum expected jitter.
   * @note    A value of 0 indicates, that jitter is of no concern.
   */
  urt_delay_t maxJitter;

  /**
   * @brief   Minimum valid latency ever detected.
   */
  urt_delay_t minLatency;

  /**
   * @brief   Maximum valid latency ever detected.
   */
  urt_delay_t maxLatency;
#endif /* (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

#if (URT_CFG_PUBSUB_QOS_RATECHECKS == true) || defined (__DOXYGEN__)
  /**
   * @brief   Minimum expected data rate.
   * @note    Value is given as time per period in microseconds.
   *          A value of 0 indicates, that rate is of no concern.
   */
  urt_delay_t expectedRate;
#endif /* (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */

#if ((URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS == true)) || defined (__DOXYGEN__)
  /**
   * @brief   Recovery related data.
   */
  struct {
    /**
     * @brief   Callback to be executed in case a timing violation has occurred.
     * @details If set to NULL, urtCorePanic() will be executed as default.
     *          The callback will only be executed if the current value of
     *          violationCode is URT_STATUS_OK.
     */
    urt_hrtsubscriber_recovery_f callback;

    /**
     * @brief   Custom parameter to be passed to the callback function.
     */
    void* params;

    /**
     * @brief   Status code indicating the reason for the first timing violation.
     * @details Its value is initialized with URT_STATUS_OK.
     *          Whenever a recovery attempt is initiated, this value is set to
     *          the status code, indicating the cause of the violation (i.e.
     *          deadline, jitter or rate). As long as its value is not reset to
     *          URT_STATUS_OK, no further attempts will be performed.
     *          It is thus th resposnibility of the recovery logic to reset this
     *          variable on success.
     */
    urt_status_t violationCode;
  } recovery;

#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */

};

/**
 * @extends urt_basesubscriber
 *
 * @brief   Generic subscriber.
 * @ingroup pubsub_subscriber_xrt
 */
union urt_subscriber
{
  /**
   * @brief   Base subscriber view.
   */
  urt_basesubscriber_t base;

  /**
   * @brief   NRT subscriber view.
   */
  urt_nrtsubscriber_t nrt;

  /**
   * @brief   SRT subscriber view.
   */
  urt_srtsubscriber_t srt;

  /**
   * @brief   FRT subscriber view.
   */
  urt_frtsubscriber_t frt;

  /**
   * @brief   HRT subscriber view.
   */
  urt_hrtsubscriber_t hrt;
};

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

  urt_rtclass_t urtSubscriberGetRtClass(const urt_basesubscriber_t* subscriber);

  void urtNrtSubscriberInit(urt_nrtsubscriber_t* subscriber);
  urt_status_t urtNrtSubscriberSubscribe(urt_nrtsubscriber_t* subscriber, urt_topic_t* topic, urt_osEventMask_t mask);

  void urtSrtSubscriberInit(urt_srtsubscriber_t* subscriber);
  urt_status_t urtSrtSubscriberSubscribe(urt_srtsubscriber_t* subscriber, urt_topic_t* topic, urt_osEventMask_t mask, urt_usefulness_f usefulnesscb, void* cbparams);

  void urtFrtSubscriberInit(urt_frtsubscriber_t* subscriber);
  urt_status_t urtFrtSubscriberSubscribe(urt_frtsubscriber_t* subscriber, urt_topic_t* topic, urt_osEventMask_t mask, urt_delay_t deadline, urt_delay_t jitter);
  bool urtFrtSubscriberCalculateValidity(urt_frtsubscriber_t* subscriber, urt_delay_t latency);

  void urtHrtSubscriberInit(urt_hrtsubscriber_t* subscriber);
  urt_status_t urtHrtSubscriberSubscribe(urt_hrtsubscriber_t* subscriber, urt_topic_t* topic, urt_osEventMask_t mask, urt_delay_t deadline, urt_delay_t jitter, urt_delay_t rate, urt_hrtsubscriber_recovery_f recoverycb, void* params);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/**
 * @addtogroup pubsub_subscriber_base
 * @{
 */

/**
 * @brief   Fetch the next pending message.
 *
 * @param[in]  subscriber   The subscriber that shall fetch the message.
 *                          Must not be NULL.
 *                          Must be associatied to a topic.
 * @param[out] payload      Pointer where to copy the payload to.
 *                          May be NULL.
 * @param[out] bytes        Pointer where to store payload size in bytes.
 *                          May be NULL.
 * @param[out] origin_time  Pointer where to store the information origin time to.
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
 *                                        Only occurs for FRT and HRT subscribers.
 * @retval URT_STATUS_JITTERVIOLATION     Latency violated the maximum expected jitter.
 *                                        Jitter violations are only checked if no deadline violation was detected.
 *                                        Only occurs for FRT and HRT subscribers.
 */
inline urt_status_t urtSubscriberFetchNext(urt_basesubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* origin_time, urt_delay_t* latency)
{
  return subscriber->vmt->fetchNext(subscriber, payload, bytes, origin_time, latency);
}

/**
 * @brief   Fetch the latest pending message.
 *
 * @param[in]  subscriber   The subscriber that shall fetch the message.
 *                          Must not be NULL.
 *                          Must be associatied to a topic.
 * @param[out] payload      Pointer where to copy the payload to.
 *                          May be NULL for messages without payload.
 * @param[out] bytes        Pointer where to store payload size in bytes.
 *                          May be NULL.
 * @param[out] origin_time  Pointer where to store the information origin time to.
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
 *                                        Only occurs for FRT and HRT subscribers.
 * @retval URT_STATUS_JITTERVIOLATION     Latency violated the maximum expected jitter.
 *                                        Jitter violations are only checked if no deadline violation was detected.
 *                                        Only occurs for FRT and HRT subscribers.
 */
inline urt_status_t urtSubscriberFetchLatest(urt_basesubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* origin_time, urt_delay_t* latency)
{
  return subscriber->vmt->fetchLatest(subscriber, payload, bytes, origin_time, latency);
}

/**
 * @brief   Unsubscribe from a topic.
 *
 * @param[in,out] subscriber  The subscriber to be unsubscribed.
 *                            Must not be NULL.
 *                            Must be associatied to a topic.
 *
 * @return  Always returns URT_STATUS_OK.
 */
inline urt_status_t urtSubscriberUnsubscribe(urt_basesubscriber_t* subscriber)
{
  return subscriber->vmt->unsubscribe(subscriber);
}

/** @} */

/* NRT ************************************************************************/

/**
 * @addtogroup pubsub_subscriber_nrt
 * @{
 */

/**
 * @brief   Fetch the next pending message.
 *
 * @param[in]  subscriber   The NRT subscriber that shall fetch the message.
 *                          Must not be NULL.
 *                          Must be associatied to a topic.
 * @param[out] payload      Pointer where to copy the payload to.
 *                          May be NULL.
 * @param[out] bytes        Pointer where to store payload size in bytes.
 *                          May be NULL.
 * @param[out] origin_time  Pointer where to store the information origin time to.
 *                          May be NULL.
 * @param[out] latency      The latency to be returned by reference (optional).
 *                          Value is only modified if a message was fetched.
 *                          May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Successfully fetched a message.
 * @retval URT_STATUS_FETCH_NOMESSAGE   There is no new message to fetch.
 */
inline urt_status_t urtNrtSubscriberFetchNext(urt_nrtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* origin_time, urt_delay_t* latency)
{
  return subscriber->vmt->fetchNext(subscriber, payload, bytes, origin_time, latency);
}

/**
 * @brief   Fetch the latest pending message.
 *
 * @param[in]  subscriber   The NRT subscriber that shall fetch the message.
 *                          Must not be NULL.
 *                          Must be associatied to a topic.
 * @param[out] payload      Pointer where to copy the payload to.
 *                          May be NULL for messages without payload.
 * @param[out] bytes        Pointer where to store payload size in bytes.
 *                          May be NULL.
 * @param[out] origin_time  Pointer where to store the information origin time to.
 *                          May be NULL.
 * @param[out] latency      The latency to be returned by reference (optional).
 *                          Value is only modified if a message was fetched.
 *                          May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Successfully fetched a message.
 * @retval URT_STATUS_FETCH_NOMESSAGE   There is no new message to fetch.
 */
inline urt_status_t urtNrtSubscriberFetchLatest(urt_nrtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* origin_time, urt_delay_t* latency)
{
  return subscriber->vmt->fetchLatest(subscriber, payload, bytes, origin_time, latency);
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
inline urt_status_t urtNrtSubscriberUnsubscribe(urt_nrtsubscriber_t* subscriber)
{
  return subscriber->vmt->unsubscribe(subscriber);
}

/** @} */

/* SRT ************************************************************************/

/**
 * @addtogroup pubsub_subscriber_srt
 * @{
 */

/**
 * @brief   Fetch the next pending message.
 *
 * @param[in]  subscriber   The SRT subscriber that shall fetch the message.
 *                          Must not be NULL.
 *                          Must be associatied to a topic.
 * @param[out] payload      Pointer where to copy the payload to.
 *                          May be NULL.
 * @param[out] bytes        Pointer where to store payload size in bytes.
 *                          May be NULL.
 * @param[out] origin_time  Pointer where to store the information origin time to.
 *                          May be NULL.
 * @param[out] latency      The latency to be returned by reference (optional).
 *                          Value is only modified if a message was fetched.
 *                          May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Successfully fetched a message.
 * @retval URT_STATUS_FETCH_NOMESSAGE   There is no new message to fetch.
 */
inline urt_status_t urtSrtSubscriberFetchNext(urt_srtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* origin_time, urt_delay_t* latency)
{
  return subscriber->vmt->fetchNext(subscriber, payload, bytes, origin_time, latency);
}

/**
 * @brief   Fetch the latest pending message.
 *
 * @param[in]  subscriber   The SRT subscriber that shall fetch the message.
 *                          Must not be NULL.
 *                          Must be associatied to a topic.
 * @param[out] payload      Pointer where to copy the payload to.
 *                          May be NULL for messages without payload.
 * @param[out] bytes        Pointer where to store payload size in bytes.
 *                          May be NULL.
 * @param[out] origin_time  Pointer where to store the information origin time to.
 *                          May be NULL.
 * @param[out] latency      The latency to be returned by reference (optional).
 *                          Value is only modified if a message was fetched.
 *                          May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Successfully fetched a message.
 * @retval URT_STATUS_FETCH_NOMESSAGE   There is no new message to fetch.
 */
inline urt_status_t urtSrtSubscriberFetchLatest(urt_srtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* origin_time, urt_delay_t* latency)
{
  return subscriber->vmt->fetchLatest(subscriber, payload, bytes, origin_time, latency);
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
inline urt_status_t urtSrtSubscriberUnsubscribe(urt_srtsubscriber_t* subscriber)
{
  return subscriber->vmt->unsubscribe(subscriber);
}

/**
 * @brief   Calculate the usefulness of a message.
 *
 * @param[in] subscriber  The SRT subscriber to calculate the usefulness for.
 *                        Must not be NULL.
 * @param[in] latency     Latency (of a message) as argument to calculate usefulness.
 *
 * @return  Returns the usefulness as a value in range [0,1].
 */
static inline float urtSrtSubscriberCalculateUsefulness(const urt_srtsubscriber_t* subscriber, urt_delay_t latency)
{
  urtDebugAssert(subscriber != NULL);

  return subscriber->usefulnesscb(latency, subscriber->cbparams);
}

/** @} */

/* FRT ************************************************************************/

/**
 * @addtogroup pubsub_subscriber_frt
 * @{
 */

/**
 * @brief   Fetch the next pending message.
 *
 * @param[in]  subscriber   The FRT subscriber that shall fetch the message.
 *                          Must not be NULL.
 *                          Must be associatied to a topic.
 * @param[out] payload      Pointer where to copy the payload to.
 *                          May be NULL.
 * @param[out] bytes        Pointer where to store payload size in bytes.
 *                          May be NULL.
 * @param[out] origin_time  Pointer where to store the information origin time to.
 *                          May be NULL.
 * @param[out] latency      The latency to be returned by reference (optional).
 *                          Value is only modified if a message was fetched.
 *                          May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Successfully fetched a message.
 * @retval URT_STATUS_FETCH_NOMESSAGE   There is no new message to fetch.
 * @retval URT_STATUS_JITTERVIOLATION   Latency violated the maximum expected jitter.
 *                                      Jitter violations are only checked if no deadline violation was detected.
 */
inline urt_status_t urtFrtSubscriberFetchNext(urt_frtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* origin_time, urt_delay_t* latency)
{
  return subscriber->vmt->fetchNext(subscriber, payload, bytes, origin_time, latency);
}

/**
 * @brief   Fetch the latest pending message.
 *
 * @param[in]  subscriber   The FRT subscriber that shall fetch the message.
 *                          Must not be NULL.
 *                          Must be associatied to a topic.
 * @param[out] payload      Pointer where to copy the payload to.
 *                          May be NULL for messages without payload.
 * @param[out] bytes        Pointer where to store payload size in bytes.
 *                          May be NULL.
 * @param[out] origin_time  Pointer where to store the information origin time to.
 *                          May be NULL.
 * @param[out] latency      The latency to be returned by reference (optional).
 *                          Value is only modified if a message was fetched.
 *                          May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                  Successfully fetched a message.
 * @retval URT_STATUS_FETCH_NOMESSAGE     There is no new message to fetch.
 */
inline urt_status_t urtFrtSubscriberFetchLatest(urt_frtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* origin_time, urt_delay_t* latency)
{
  return subscriber->vmt->fetchLatest(subscriber, payload, bytes, origin_time, latency);
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
inline urt_status_t urtFrtSubscriberUnsubscribe(urt_frtsubscriber_t* subscriber)
{
  return subscriber->vmt->unsubscribe(subscriber);
}

/** @} */

/* HRT ************************************************************************/

/**
 * @addtogroup pubsub_subscriber_hrt
 * @{
 */

/**
 * @brief   Fetch the next pending message.
 *
 * @param[in]  subscriber   The HRT subscriber that shall fetch the message.
 *                          Must not be NULL.
 *                          Must be associatied to a topic.
 * @param[out] payload      Pointer where to copy the payload to.
 *                          May be NULL.
 * @param[out] bytes        Pointer where to store payload size in bytes.
 *                          May be NULL.
 * @param[out] origin_time  Pointer where to store the information origin time to.
 *                          May be NULL.
 * @param[out] latency      The latency to be returned by reference (optional).
 *                          Value is only modified if a message was fetched.
 *                          May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Successfully fetched a message.
 * @retval URT_STATUS_FETCH_NOMESSAGE   There is no new message to fetch.
 * @retval URT_STATUS_JITTERVIOLATION   Latency violated the maximum expected jitter.
 *                                      Jitter violations are only checked if no deadline violation was detected.
 */
inline urt_status_t urtHrtSubscriberFetchNext(urt_hrtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* origin_time, urt_delay_t* latency)
{
  return subscriber->vmt->fetchNext(subscriber, payload, bytes, origin_time, latency);
}

/**
 * @brief   Fetch the latest pending message.
 *
 * @param[in]  subscriber   The HRT subscriber that shall fetch the message.
 *                          Must not be NULL.
 *                          Must be associatied to a topic.
 * @param[out] payload      Pointer where to copy the payload to.
 *                          May be NULL for messages without payload.
 * @param[out] bytes        Pointer where to store payload size in bytes.
 *                          May be NULL.
 * @param[out] origin_time  Pointer where to store the information origin time to.
 *                          May be NULL.
 * @param[out] latency      The latency to be returned by reference (optional).
 *                          Value is only modified if a message was fetched.
 *                          May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                  Successfully fetched a message.
 * @retval URT_STATUS_FETCH_NOMESSAGE     There is no new message to fetch.
 */
inline urt_status_t urtHrtSubscriberFetchLatest(urt_hrtsubscriber_t* subscriber, void* payload, size_t* bytes, urt_osTime_t* origin_time, urt_delay_t* latency)
{
  return subscriber->vmt->fetchLatest(subscriber, payload, bytes, origin_time, latency);
}

/**
 * @brief   Unsubscribe from a topic.
 *
 * @param[in,out] subscriber  The HRT subscriber to be unsubscribed.
 *                            Must not be NULL.
 *                            Must be associatied to a topic.
 *
 * @return  Always returns URT_STATUS_OK.
 */
inline urt_status_t urtHrtSubscriberUnsubscribe(urt_hrtsubscriber_t* subscriber)
{
  return subscriber->vmt->unsubscribe(subscriber);
}

/** @} */

#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
#endif /* URT_SUBSCRIBER_H */
