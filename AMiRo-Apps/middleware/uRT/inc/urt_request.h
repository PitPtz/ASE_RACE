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
 * @file    urt_request.h
 * @brief   Request interface.
 *
 * @defgroup rpc_request Request
 * @ingroup rpc
 * @brief   Requests trigger services and can hold return data.
 * @details Requests trigger services to do some processing.
 *          Therefore, additional data can be passed to the service via the
 *          request and the service can return information, using the the same
 *          request, that triggered it.
 *
 * @defgroup rpc_request_base Base Request
 * @ingroup rpc_request
 * @brief   Request base type.
 * @details All specialized requests share this base type.
 *
 * @defgroup rpc_request_nrt NRT Request
 * @ingroup rpc_request
 * @brief   Request with no real-time constraints.
 * @details NRT requests hold no real-time constraints at all.
 *          This makes them the smallest and most performant option, but they
 *          offer no timing guarantiees at all.
 *
 * @defgroup rpc_request_srt SRT Request
 * @ingroup rpc_request
 * @brief   Request with soft real-time constraints.
 * @details Similarly efficient as NRT requests, SRT requests provide an API to
 *          calculate the usefulness of a response, depending on its age.
 *
 * @defgroup rpc_request_frt FRT Request
 * @ingroup rpc_request
 * @brief   Request with firm real-time constraints.
 * @details Slightly less efficient than NRT and SRT requests, strict timing
 *          constraints can be set for FRT requests.
 *          Whenever these are violated, the response is considered invalid.
 *
 * @defgroup rpc_request_hrt HRT Request
 * @ingroup rpc_request
 * @brief   Request with hard real-time constraints.
 * @details This is the least efficient request type, which behaves similarly to
 *          FRT requests.
 *          If timing constraints are violated, however, this is considered a
 *          severe system failure and a µRT panic is provoked immediately.
 *
 * @defgroup rpc_request_xrt Request
 * @ingroup rpc_request
 * @brief   General request type.
 * @details This general request type encompasses an NRT, SRT, FRT and HRT
 *          request and thus offers high flexibility.
 *          In terms of memory utilization, however, the specialized types are
 *          typically more efficient.
 */

#ifndef URT_REQUEST_H
#define URT_REQUEST_H

#include <urt.h>

#if (URT_CFG_RPC_ENABLED == true || defined(__DOXYGEN__))

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
typedef struct urt_service urt_service_t;
typedef struct urt_baserequest urt_baserequest_t;
typedef struct urt_nrtrequest urt_nrtrequest_t;
typedef struct urt_srtrequest urt_srtrequest_t;
typedef struct urt_frtrequest urt_frtrequest_t;
typedef struct urt_hrtrequest urt_hrtrequest_t;
typedef union urt_request urt_request_t;
/** @endcond */

/**
 * @brief   Recovery callback function type for HRT requests.
 *
 * @details   Recovery callbacks can be used to initiate recovery attempts in
 *            case some timing violation occurred. However, callbacks are only
 *            executed if the current value of the request's violationCode
 *            member is URT_STATUS_OK.
 *            If a recovery has been initiated, it is the responsibility of the
 *            recovery logic to reset that variable to this value.
 *
 * @param[in] params  Pointer to optional parameters.
 */
typedef void (*urt_hrtrequest_recovery_f)(void* params);

/**
 * @brief   Policies for retrieving previously submitted requests.
 * @ingroup rpc_request_base
 */
typedef enum urt_request_retrievepolicy
{
  URT_REQUEST_RETRIEVE_LAZY,        /**< Abort retrieval if a locked mutex or a bad owner is detected. */
  URT_REQUEST_RETRIEVE_DETERMINED,  /**< Abort retrieval only if a bad owner is detected. */
  URT_REQUEST_RETRIEVE_ENFORCING,   /**< Always retrieve, even if this cancels the request. */
} urt_request_retrievepolicy_t;

/* base request ***************************************************************/

#if (URT_CFG_RPC_PROFILING == true) || defined(__DOXYGEN__)
/**
 * @brief   Base request profiling data structure.
 * @details Common to all request types (NRT, SRT, FRT and HRT).
 * @ingroup rpc_request_base
 */
typedef struct urt_baserequest_profilingdata
{
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
   * @brief   Counter how often the request has been submitted.
   */
  urt_profilingcounter_t numCalls;

  /**
   * @brief   Counter how often this request was not answered in time.
   */
  urt_profilingcounter_t numFails;

} urt_baserequest_profilingdata_t;
#endif /* (URT_CFG_RPC_PROFILING == true) */

/**
 * @brief   Base request specific methods.
 */
#define _urt_baserequest_methods                                                \
  /* Submit a request. */                                                       \
  urt_status_t (*submit)(void* instance, urt_service_t* service, size_t bytes, urt_osEventMask_t mask, urt_delay_t deadline); \
  /* Retrieve a request. */                                                     \
  urt_status_t (*retrieve)(void* instance, urt_request_retrievepolicy_t policy, size_t* bytes, urt_delay_t* latency);

/**
 * @brief   Base request virtual method table.
 *
 * @ingroup rpc_request_base
 */
struct urt_baserequest_VMT {
  _urt_baserequest_methods
};

/**
 * @brief   Base request data.
 * @details Common to all request types (NRT, SRT, FRT and HRT).
 */
#define _urt_baserequest_data                                                   \
  /* Pointer to the previous request in a doubly-linked list. */                \
  urt_baserequest_t* prev;                                                      \
  /* Pointer to the next request in a doubly-linked list. */                    \
  urt_baserequest_t* next;                                                      \
  /* Mutex lock for exclusive access. */                                        \
  urt_osMutex_t lock;                                                           \
  /* Pointer to the owning service of this request. */                          \
  urt_service_t* service;                                                       \
  /* Time when the request has been submitted. */                               \
  urt_osTime_t submissionTime;                                                  \
  /* notification related data */                                               \
  struct {                                                                      \
    /* Submitting thread to be notified by the service. */                      \
    urt_osThread_t* thread;                                                     \
    /* Event mask to be set on notification. */                                 \
    urt_osEventMask_t mask;                                                     \
  } notification;                                                               \
  /* Pointer to an optional payload of the request. */                          \
  void* payload;                                                                \
  /* Size of the current payload */                                             \
  size_t size;                                                                  \
  _urt_baserequest_profilingdata;

#if (URT_CFG_RPC_PROFILING == true) || defined (__DOXYGEN__)
/**
 * @brief   Base request profiling related data.
 */
#define _urt_baserequest_profilingdata                                          \
   urt_baserequest_profilingdata_t profiling;
#else /* (URT_CFG_RPC_PROFILING == true) */
#define _urt_baserequest_profilingdata
#endif /* (URT_CFG_RPC_PROFILING == true) */

/**
 * @brief   Base request structure, common to all request types (NRT, SRT, FRT and HRT).
 *
 * @ingroup rpc_request_base
 */
typedef struct urt_baserequest {
  const struct urt_baserequest_VMT* vmt;  /**< Virtual method table. */
  _urt_baserequest_data
} urt_baserequest_t;

/**
 * @brief   NRT request structure.
 *
 * @extends urt_baserequest
 * @ingroup rpc_request_nrt
 */
typedef struct urt_nrtrequest
{
  const struct urt_baserequest_VMT* vmt;  /**< Virtual method table. */
  _urt_baserequest_data
} urt_nrtrequest_t;

/**
 * @brief   SRT request structure.
 *
 * @extends urt_baserequest
 * @ingroup rpc_request_srt
 */
typedef struct urt_srtrequest
{
  const struct urt_baserequest_VMT* vmt;  /**< Virtual method table. */
  _urt_baserequest_data
} urt_srtrequest_t;

/**
 * @brief   FRT request structure.
 *
 * @extends urt_baserequest
 * @ingroup rpc_request_frt
 */
typedef struct urt_frtrequest
{
  const struct urt_baserequest_VMT* vmt;  /**< Virtual method table. */
  _urt_baserequest_data

#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true) || defined (__DOXYGEN__)
  /**
   * @brief   Maximum expected latency.
   * @details A value of 0 indicates, that latency is of no concern.
   */
  urt_delay_t deadlineOffset;
#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) */

#if (URT_CFG_RPC_QOS_JITTERCHECKS == true) || defined (__DOXYGEN__)
  /**
   * @brief   Maximum expected jitter.
   * @details A value of 0 indicates, that jitter is of no concern.
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
#endif /* (URT_CFG_RPC_QOS_JITTERCHECKS == true) */

} urt_frtrequest_t;

/**
 * @brief   HRT request structure.
 *
 * @extends urt_baserequest
 * @ingroup rpc_request_hrt
 */
typedef struct urt_hrtrequest
{
  const struct urt_baserequest_VMT* vmt;  /**< Virtual method table. */
  _urt_baserequest_data

#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true) || defined (__DOXYGEN__)
  /**
   * @brief   Maximum expected latency.
   * @details A value of 0 indicates, that latency is of no concern.
   */
  urt_delay_t deadlineOffset;
#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) */

#if (URT_CFG_RPC_QOS_JITTERCHECKS == true) || defined (__DOXYGEN__)
  /**
   * @brief   Maximum expected jitter.
   * @details A value of 0 indicates, that jitter is of no concern.
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
#endif /* (URT_CFG_RPC_QOS_JITTERCHECKS == true) */

#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true || URT_CFG_RPC_QOS_JITTERCHECKS == true) || defined (__DOXYGEN__)
  /**
   * @brief   QoS timer to detect latency or jitter violations.
   */
  urt_osTimer_t qosTimer;

  /**
   * @brief   Recovery related data.
   */
  struct {
    /**
     * @brief   Callback to be executed in case a timing violation has occurred.
     * @details If set to NULL, urtCorePanic() will be executed as default.
     *          The callback will only be executed if the current value of the
     *          violationCode is URT_STATUS_OK.
     */
    urt_hrtrequest_recovery_f callback;

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
#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true || URT_CFG_RPC_QOS_JITTERCHECKS == true) */

} urt_hrtrequest_t;

/**
 * @brief   Generic request.
 *
 * @extends urt_baserequest
 * @ingroup rpc_request_xrt
 */
typedef union urt_request
{
  /**
   * @brief   Base request view.
   */
  urt_baserequest_t base;

  /**
   * @brief   NRT request view.
   */
  urt_nrtrequest_t nrt;

  /**
   * @brief   SRT request view.
   */
  urt_srtrequest_t srt;

  /**
   * @brief   FRT request view.
   */
  urt_frtrequest_t frt;

  /**
   * @brief   HRT request view.
   */
  urt_hrtrequest_t hrt;
} urt_request_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

  urt_rtclass_t urtRequestGetRtClass(const urt_baserequest_t* request);
  urt_status_t urtRequestAcquire(urt_baserequest_t* request);
  urt_status_t urtRequestTryAcquire(urt_baserequest_t* request);
  urt_delay_t urtRequestGetAge(const urt_baserequest_t* request);

  void urtNrtRequestInit(urt_nrtrequest_t* request, void* payload);

  void urtSrtRequestInit(urt_srtrequest_t* request, void* payload);

  void urtFrtRequestInit(urt_frtrequest_t* request, void* payload, urt_delay_t jitter);
  bool urtFrtRequestCalculateValidity(const urt_frtrequest_t* request, urt_delay_t latency);

  void urtHrtRequestInit(urt_hrtrequest_t* request, void* payload, urt_delay_t jitter, urt_hrtrequest_recovery_f recoverycb, void* params);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/**
 * @addtogroup rpc_request_base
 * @{
 */

/**
 * @brief   Submit a request.
 *
 * @param[in] request   The request to be used for this call.
 *                      Must not be NULL.
 *                      Must have been acquired before.
 * @param[in] service   The service to be called.
 *                      Must not be NULL.
 * @param[in] bytes     Size of the payload in bytes.
 * @param[in] mask      Event mask to be notified when the request is answered.
 *                      Set this to 0 for "fire and forget" requests.
 * @param[in] deadline  Deadline offset until when this request is valid.
 *                      A value of 0 indicates that there is no deadline.
 *                      Only applies to FRT and HRT requests.
 *                      Ignored if URT_CFG_RPC_QOS_DEADLINECHECKS false.
 *
 * @return  Always returns URT_STATUS_OK.
 */
inline urt_status_t urtRequestSubmit(urt_baserequest_t* request, urt_service_t* service, size_t bytes, urt_osEventMask_t mask, urt_delay_t deadline)
{
  return request->vmt->submit(request, service, bytes, mask, deadline);
}

/**
 * @brief   Retrieve whether a base request was submitted but not retrieved yet.
 *
 * @param[in] request   Base request to check.
 *
 * @return  Indicator whether the request is pending.
 */
inline bool urtRequestIsPending(const urt_baserequest_t* const request)
{
  return request->notification.thread != NULL;
}

/**
 * @brief   Retrieve a request.
 *
 * @param[in] request   The request to retrieve.
 *                      Must not be NULL.
 * @param[in]  policy   Policy how to retrieve the request.
 * @param[out] bytes    Size of the payload to be returned by reference (optional).
 *                      May be NULL.
 * @param[out] latency  The latency to be returned by reference (optional).
 *                      May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                  Request was retrieved normally.
 *                                        The request is locked.
 * @retval URT_STATUS_REQUEST_BADOWNER    Request had to be removed from a service.
 *                                        If policy was set to URT_REQUEST_RETRIEVE_ENFORCING, the request is locked.
 * @retval URT_STATUS_REQUEST_LOCKED      Request could not be acquired due to a locked mutex.
 *                                        Only occurs if the policy was set to URT_REQUEST_RETRIEVE_LAZY.
 *                                        Request is not locked.
 * @retval URT_STATUS_DEADLINEVIOLATION   Request was retrieved, but deadline has been violated.
 *                                        The request is locked.
 *                                        Only occurs for FRT and HRT requests.
 * @retval URT_STATUS_JITTERVIOLATION     Request was retrieved, but jitter has been jiolated.
 *                                        The request is locked.
 *                                        Only occurs for FRT and HRT requests.
 */
inline urt_status_t urtRequestRetrieve(urt_baserequest_t* request, urt_request_retrievepolicy_t policy, size_t* bytes, urt_delay_t* latency)
{
  return request->vmt->retrieve(request, policy, bytes, latency);
}

/**
 * @brief   Release a base request.
 *
 * @param[in] request  Request to release.
 *                     Must not be NULL.
 *                     Must have been acquired before.
 *
 * @return  Always returns URT_STATUS_OK.
 */
inline urt_status_t urtRequestRelease(urt_baserequest_t* const request)
{
  urtMutexUnlock(&request->lock);
  return URT_STATUS_OK;
}

/**
 * @brief   Retrieve the pointer to the associated payload of a base request.
 *
 * @param[in] request   Request to retrieve the payload from.
 *
 * @return  Pointer to a payload object or NULL.
 */
inline void* urtRequestGetPayload(urt_baserequest_t* const request)
{
  return request->payload;
}

/**
 * @brief   Retrieve the current payload size of a base request.
 *
 * @param[in] request   Request to retrieve the payload size from.
 *
 * @return  Size of the payload in bytes.
 */
inline size_t urtRequestGetPayloadSize(const urt_baserequest_t* const request)
{
  return request->size;
}

/**
 * @brief   Retrieve the submission time of a base request.
 *
 * @param[in] request   Request of which the submission time shall be retrieved.
 *
 * @return  Pointer to the submission time of the request.
 */
inline const urt_osTime_t* urtRequestGetSubmissionTime(const urt_baserequest_t* const request)
{
  return &request->submissionTime;
}

/**
 * @brief   Retrieve whether the request was submitted with fire-and-forget
 *          intentions.
 *
 * @param[in] request   Request to check for fire-and-forget intentions.
 *
 * @return  Indicator, whether the request was submitted with fire-and-forget
 *          intentions.
 * @retval true   Fire-and-forget request: no need to respond to.
 * @retval false  No fire-and-forget request: must be responded to.
 */
inline bool urtRequestIsFireAndForget(const urt_baserequest_t* const request)
{
  return request->notification.mask == 0;
}

/** @} */

/* NRT ************************************************************************/

/**
 * @addtogroup rpc_request_nrt
 * @{
 */

/**
 * @brief   Acquire an NRT request.
 *
 * @param[in] request   The request to acquire.
 *                      Must not be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Request was acquired sucessfully.
 * @retval URT_STATUS_REQUEST_BADOWNER  Request is currently associated to a service.
 */
inline urt_status_t urtNrtRequestAcquire(urt_nrtrequest_t* request)
{
  return urtRequestAcquire((urt_baserequest_t*)request);
}

/**
 * @brief   Try to acquire an NRT request.
 *
 * @param[in] request   The request to be acquired.
 *                      Must not be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Request was acquired sucessfully.
 * @retval URT_STATUS_REQUEST_BADOWNER  Request is currently associated to a service.
 * @retval URT_STATUS_REQUEST_LOCKED    Request could not be acquired due to a locked mutex.
 */
inline urt_status_t urtNrtRequestTryAcquire(urt_nrtrequest_t* request)
{
  return urtRequestTryAcquire((urt_baserequest_t*)request);
}

/**
 * @brief   Submit an NRT request.
 *
 * @param[in] request   The request to be used for this call.
 *                      Must not be NULL.
 *                      Must have been acquired before.
 * @param[in] service   The service to be called.
 *                      Must not be NULL.
 * @param[in] bytes     Size of the payload in bytes.
 * @param[in] mask      Event mask to be notified when the request is answered.
 *                      Set this to 0 for "fire and forget" requests.
 *
 * @return  Always returns URT_STATUS_OK.
 */
inline urt_status_t urtNrtRequestSubmit(urt_nrtrequest_t* request, urt_service_t* service, size_t bytes, urt_osEventMask_t mask)
{
  return request->vmt->submit(request, service, bytes, mask, 0);
}

/**
 * @brief   Retrieve whether an NRT request was submitted but not retrieved yet.
 *
 * @param[in] request   Request to check.
 *
 * @return  Indicator whether the request is pending.
 */
inline bool urtNrtRequestIsPending(const urt_nrtrequest_t* request)
{
  return urtRequestIsPending((const urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve an NRT request.
 *
 * @param[in] request   The request to retrieve.
 *                      Must not be NULL.
 * @param[in]  policy   Policy how to retrieve the request.
 * @param[out] bytes    Size of the payload to be returned by reference (optional).
 *                      May be NULL.
 * @param[out] latency  The latency to be returned by reference (optional).
 *                      May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                  Request was retrieved normally.
 *                                        The request is locked.
 * @retval URT_STATUS_REQUEST_BADOWNER    Request had to be removed from a service.
 *                                        If policy was set to URT_REQUEST_RETRIEVE_ENFORCING, the request is locked.
 * @retval URT_STATUS_REQUEST_LOCKED      Request could not be acquired due to a locked mutex.
 *                                        Only occurs if the policy was set to URT_REQUEST_RETRIEVE_LAZY.
 *                                        Request is not locked.
 * @retval URT_STATUS_DEADLINEVIOLATION   Request was retrieved, but deadline has been violated.
 *                                        The request is locked.
 *                                        Only occurs for FRT and HRT requests.
 * @retval URT_STATUS_JITTERVIOLATION     Request was retrieved, but jitter has been jiolated.
 *                                        The request is locked.
 *                                        Only occurs for FRT and HRT requests.
 */
inline urt_status_t urtNrtRequestRetrieve(urt_nrtrequest_t* request, urt_request_retrievepolicy_t policy, size_t* bytes, urt_delay_t* latency)
{
  return request->vmt->retrieve(request, policy, bytes, latency);
}

/**
 * @brief   Release an NRT request.
 *
 * @param[in] request  Request to release.
 *                     Must not be NULL.
 *                     Must have been acquired before.
 *
 * @return  Always returns URT_STATUS_OK.
 */
inline urt_status_t urtNrtRequestRelease(urt_nrtrequest_t* request)
{
  return urtRequestRelease((urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve the pointer to the associated payload of an NRT request.
 *
 * @param[in] request   Request to retrieve the payload from.
 *
 * @return  Pointer to a payload object or NULL.
 */
inline void* urtNrtRequestGetPayload(urt_nrtrequest_t* request)
{
  return urtRequestGetPayload((urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve the current payload size of an NRT request.
 *
 * @param[in] request   Request to retrieve the payload size from.
 *
 * @return  Size of the payload in bytes.
 */
inline size_t urtNrtRequestGetPayloadSize(const urt_nrtrequest_t* request)
{
  return urtRequestGetPayloadSize((const urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve the submission time of an NRT request.
 *
 * @param[in] request   Request of which the submission time shall be retrieved.
 *
 * @return  Pointer to the submission time of the request.
 */
inline const urt_osTime_t* urtNrtRequestGetSubmissionTime(const urt_nrtrequest_t* request)
{
  return urtRequestGetSubmissionTime((const urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve the age of an NRT request (since submission).
 *
 * @param[in] request   Request of which the age shall be retrieved.
 *
 * @return  Age of the request (since submission).
 * @retval 0  The request was not submitted yet.
 */
inline urt_delay_t urtNrtRequestGetAge(const urt_nrtrequest_t* request)
{
  return urtRequestGetAge((const urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve whether the NRT request was submitted with fire-and-forget
 *          intentions.
 *
 * @param[in] request   Request to check for fire-and-forget intentions.
 *
 * @return  Indicator, whether the request was submitted with fire-and-forget
 *          intentions.
 * @retval true   Fire-and-forget request: no need to respond to.
 * @retval false  No fire-and-forget request: must be responded to.
 */
inline bool urtNrtRequestIsFireAndForget(const urt_nrtrequest_t* const request)
{
  return request->notification.mask == 0;
}

/** @} */

/* SRT ************************************************************************/

/**
 * @addtogroup rpc_request_srt
 * @{
 */

/**
 * @brief   Acquire an SRT request.
 *
 * @param[in] request   The request to acquire.
 *                      Must not be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Request was acquired sucessfully.
 * @retval URT_STATUS_REQUEST_BADOWNER  Request is currently associated to a service.
 */
inline urt_status_t urtSrtRequestAcquire(urt_srtrequest_t* request)
{
  return urtRequestAcquire((urt_baserequest_t*)request);
}

/**
 * @brief   Try to acquire an SRT request.
 *
 * @param[in] request   The request to be acquired.
 *                      Must not be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Request was acquired sucessfully.
 * @retval URT_STATUS_REQUEST_BADOWNER  Request is currently associated to a service.
 * @retval URT_STATUS_REQUEST_LOCKED    Request could not be acquired due to a locked mutex.
 */
inline urt_status_t urtSrtRequestTryAcquire(urt_srtrequest_t* request)
{
  return urtRequestTryAcquire((urt_baserequest_t*)request);
}

/**
 * @brief   Submit an SRT request.
 *
 * @param[in] request   The request to be used for this call.
 *                      Must not be NULL.
 *                      Must have been acquired before.
 * @param[in] service   The service to be called.
 *                      Must not be NULL.
 * @param[in] bytes     Size of the payload in bytes.
 * @param[in] mask      Event mask to be notified when the request is answered.
 *                      Set this to 0 for "fire and forget" requests.
 *
 * @return  Always returns URT_STATUS_OK.
 */
inline urt_status_t urtSrtRequestSubmit(urt_srtrequest_t* request, urt_service_t* service, size_t bytes, urt_osEventMask_t mask)
{
  return request->vmt->submit(request, service, bytes, mask, 0);
}

/**
 * @brief   Retrieve whether an SRT request was submitted but not retrieved yet.
 *
 * @param[in] request   Request to check.
 *
 * @return  Indicator whether the request is pending.
 */
inline bool urtSrtRequestIsPending(const urt_srtrequest_t* request)
{
  return urtRequestIsPending((const urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve an SRT request.
 *
 * @param[in] request   The request to retrieve.
 *                      Must not be NULL.
 * @param[in]  policy   Policy how to retrieve the request.
 * @param[out] bytes    Size of the payload to be returned by reference (optional).
 *                      May be NULL.
 * @param[out] latency  The latency to be returned by reference (optional).
 *                      May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                  Request was retrieved normally.
 *                                        The request is locked.
 * @retval URT_STATUS_REQUEST_BADOWNER    Request had to be removed from a service.
 *                                        If policy was set to URT_REQUEST_RETRIEVE_ENFORCING, the request is locked.
 * @retval URT_STATUS_REQUEST_LOCKED      Request could not be acquired due to a locked mutex.
 *                                        Only occurs if the policy was set to URT_REQUEST_RETRIEVE_LAZY.
 *                                        Request is not locked.
 * @retval URT_STATUS_DEADLINEVIOLATION   Request was retrieved, but deadline has been violated.
 *                                        The request is locked.
 *                                        Only occurs for FRT and HRT requests.
 * @retval URT_STATUS_JITTERVIOLATION     Request was retrieved, but jitter has been jiolated.
 *                                        The request is locked.
 *                                        Only occurs for FRT and HRT requests.
 */
inline urt_status_t urtSrtRequestRetrieve(urt_srtrequest_t* request, urt_request_retrievepolicy_t policy, size_t* bytes, urt_delay_t* latency)
{
  return request->vmt->retrieve(request, policy, bytes, latency);
}

/**
 * @brief   Release an SRT request.
 *
 * @param[in] request  Request to release.
 *                     Must not be NULL.
 *                     Must have been acquired before.
 *
 * @return  Always returns URT_STATUS_OK.
 */
inline urt_status_t urtSrtRequestRelease(urt_srtrequest_t* request)
{
  return urtRequestRelease((urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve the pointer to the associated payload of an SRT request.
 *
 * @param[in] request   Request to retrieve the payload from.
 *
 * @return  Pointer to a payload object or NULL.
 */
inline void* urtSrtRequestGetPayload(urt_srtrequest_t* request)
{
  return urtRequestGetPayload((urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve the current payload size of an SRT request.
 *
 * @param[in] request   Request to retrieve the payload size from.
 *
 * @return  Size of the payload in bytes.
 */
inline size_t urtSrtRequestGetPayloadSize(const urt_srtrequest_t* request)
{
  return urtRequestGetPayloadSize((const urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve the submission time of an SRT request.
 *
 * @param[in] request   Request of which the submission time shall be retrieved.
 *
 * @return  Pointer to the submission time of the request.
 */
inline const urt_osTime_t* urtSrtRequestGetSubmissionTime(const urt_srtrequest_t* request)
{
  return urtRequestGetSubmissionTime((const urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve the age of an SRT request (since submission).
 *
 * @param[in] request   Request of which the age shall be retrieved.
 *
 * @return  Age of the request (since submission).
 * @retval 0  The request was not submitted yet.
 */
inline urt_delay_t urtSrtRequestGetAge(const urt_srtrequest_t* request)
{
  return urtRequestGetAge((const urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve whether the SRT request was submitted with fire-and-forget
 *          intentions.
 *
 * @param[in] request   Request to check for fire-and-forget intentions.
 *
 * @return  Indicator, whether the request was submitted with fire-and-forget
 *          intentions.
 * @retval true   Fire-and-forget request: no need to respond to.
 * @retval false  No fire-and-forget request: must be responded to.
 */
inline bool urtSrtRequestIsFireAndForget(const urt_srtrequest_t* const request)
{
  return request->notification.mask == 0;
}

/**
 * @brief   Calculate the usefulness for an SRT request.
 *
 * @param[in] request       The request to calculate validity for.
 *                          Must not be NULL.
 *                          Must have been acquired before.
 * @param[in] latency       Latency (of a response) as argument to calculate usefulness.
 * @param[in] usefulnesscb  Callback function that calculates a usefulness.
 *                          Must not be NULL.
 * @param[in] cbparams      Optional arguments for the callback function.
 *                          May be NULL if the callback does not expect any parameters.
 *
 * @return  Value in range [0,1], indicating the usefulness of the request.
 */
inline float urtSrtRequestCalculateUsefulness(const urt_srtrequest_t* const request, urt_delay_t const latency, urt_usefulness_f const usefulnesscb, void* const cbparams)
{
  urtDebugAssert(request != NULL);
  urtDebugAssert(usefulnesscb != NULL);

  return usefulnesscb(latency, cbparams);
}

/** @} */

/* FRT ************************************************************************/

/**
 * @addtogroup rpc_request_frt
 * @{
 */

/**
 * @brief   Acquire an FRT request.
 *
 * @param[in] request   The request to acquire.
 *                      Must not be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Request was acquired sucessfully.
 * @retval URT_STATUS_REQUEST_BADOWNER  Request is currently associated to a service.
 */
inline urt_status_t urtFrtRequestAcquire(urt_frtrequest_t* request)
{
  return urtRequestAcquire((urt_baserequest_t*)request);
}

/**
 * @brief   Try to acquire an FRT request.
 *
 * @param[in] request   The request to be acquired.
 *                      Must not be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Request was acquired sucessfully.
 * @retval URT_STATUS_REQUEST_BADOWNER  Request is currently associated to a service.
 * @retval URT_STATUS_REQUEST_LOCKED    Request could not be acquired due to a locked mutex.
 */
inline urt_status_t urtFrtRequestTryAcquire(urt_frtrequest_t* request)
{
  return urtRequestTryAcquire((urt_baserequest_t*)request);
}

/**
 * @brief   Submit an FRT request.
 *
 * @param[in] request   The request to be used for this call.
 *                      Must not be NULL.
 *                      Must have been acquired before.
 * @param[in] service   The service to be called.
 *                      Must not be NULL.
 * @param[in] bytes     Size of the payload in bytes.
 * @param[in] mask      Event mask to be notified when the request is answered.
 *                      Set this to 0 for "fire and forget" requests.
 * @param[in] deadline  Deadline offset until when this request is valid.
 *                      A value of 0 indicates that there is no deadline.
 *                      Ignored if URT_CFG_RPC_QOS_DEADLINECHECKS false.
 *
 * @return  Always returns URT_STATUS_OK.
 */
inline urt_status_t urtFrtRequestSubmit(urt_frtrequest_t* request, urt_service_t* service, size_t bytes, urt_osEventMask_t mask, urt_delay_t deadline)
{
  return request->vmt->submit(request, service, bytes, mask, deadline);
}

/**
 * @brief   Retrieve whether an FRT request was submitted but not retrieved yet.
 *
 * @param[in] request   Request to check.
 *
 * @return  Indicator whether the request is pending.
 */
inline bool urtFrtRequestIsPending(const urt_frtrequest_t* request)
{
  return urtRequestIsPending((const urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve an FRT request.
 *
 * @param[in] request   The request to retrieve.
 *                      Must not be NULL.
 * @param[in]  policy   Policy how to retrieve the request.
 * @param[out] bytes    Size of the payload to be returned by reference (optional).
 *                      May be NULL.
 * @param[out] latency  The latency to be returned by reference (optional).
 *                      May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                  Request was retrieved normally.
 *                                        The request is locked.
 * @retval URT_STATUS_REQUEST_BADOWNER    Request had to be removed from a service.
 *                                        If policy was set to URT_REQUEST_RETRIEVE_ENFORCING, the request is locked.
 * @retval URT_STATUS_REQUEST_LOCKED      Request could not be acquired due to a locked mutex.
 *                                        Only occurs if the policy was set to URT_REQUEST_RETRIEVE_LAZY.
 *                                        Request is not locked.
 * @retval URT_STATUS_DEADLINEVIOLATION   Request was retrieved, but deadline has been violated.
 *                                        The request is locked.
 *                                        Only occurs for FRT and HRT requests.
 * @retval URT_STATUS_JITTERVIOLATION     Request was retrieved, but jitter has been jiolated.
 *                                        The request is locked.
 *                                        Only occurs for FRT and HRT requests.
 */
inline urt_status_t urtFrtRequestRetrieve(urt_frtrequest_t* request, urt_request_retrievepolicy_t policy, size_t* bytes, urt_delay_t* latency)
{
  return request->vmt->retrieve(request, policy, bytes, latency);
}

/**
 * @brief   Release an FRT request.
 *
 * @param[in] request  Request to release.
 *                     Must not be NULL.
 *                     Must have been acquired before.
 *
 * @return  Always returns URT_STATUS_OK.
 */
inline urt_status_t urtFrtRequestRelease(urt_frtrequest_t* request)
{
  return urtRequestRelease((urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve the pointer to the associated payload of an FRT request.
 *
 * @param[in] request   Request to retrieve the payload from.
 *
 * @return  Pointer to a payload object or NULL.
 */
inline void* urtFrtRequestGetPayload(urt_frtrequest_t* request)
{
  return urtRequestGetPayload((urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve the current payload size of an FRT request.
 *
 * @param[in] request   Request to retrieve the payload size from.
 *
 * @return  Size of the payload in bytes.
 */
inline size_t urtFrtRequestGetPayloadSize(const urt_frtrequest_t* request)
{
  return urtRequestGetPayloadSize((const urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve the submission time of an FRT request.
 *
 * @param[in] request   Request of which the submission time shall be retrieved.
 *
 * @return  Pointer to the submission time of the request.
 */
inline const urt_osTime_t* urtFrtRequestGetSubmissionTime(const urt_frtrequest_t* request)
{
  return urtRequestGetSubmissionTime((const urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve the age of an FRT request (since submission).
 *
 * @param[in] request   Request of which the age shall be retrieved.
 *
 * @return  Age of the request (since submission).
 * @retval 0  The request was not submitted yet.
 */
inline urt_delay_t urtFrtRequestGetAge(const urt_frtrequest_t* request)
{
  return urtRequestGetAge((const urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve whether the FRT request was submitted with fire-and-forget
 *          intentions.
 *
 * @param[in] request   Request to check for fire-and-forget intentions.
 *
 * @return  Indicator, whether the request was submitted with fire-and-forget
 *          intentions.
 * @retval true   Fire-and-forget request: no need to respond to.
 * @retval false  No fire-and-forget request: must be responded to.
 */
inline bool urtFrtRequestIsFireAndForget(const urt_frtrequest_t* const request)
{
  return request->notification.mask == 0;
}

/** @} */

/* HRT ************************************************************************/

/**
 * @addtogroup rpc_request_hrt
 * @{
 */

/**
 * @brief   Acquire an HRT request.
 *
 * @param[in] request   The request to acquire.
 *                      Must not be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Request was acquired sucessfully.
 * @retval URT_STATUS_REQUEST_BADOWNER  Request is currently associated to a service.
 */
inline urt_status_t urtHrtRequestAcquire(urt_hrtrequest_t* request)
{
  return urtRequestAcquire((urt_baserequest_t*)request);
}

/**
 * @brief   Try to acquire an HRT request.
 *
 * @param[in] request   The request to be acquired.
 *                      Must not be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Request was acquired sucessfully.
 * @retval URT_STATUS_REQUEST_BADOWNER  Request is currently associated to a service.
 * @retval URT_STATUS_REQUEST_LOCKED    Request could not be acquired due to a locked mutex.
 */
inline urt_status_t urtHrtRequestTryAcquire(urt_hrtrequest_t* request)
{
  return urtRequestTryAcquire((urt_baserequest_t*)request);
}

/**
 * @brief   Submit an HRT request.
 *
 * @param[in] request   The request to be used for this call.
 *                      Must not be NULL.
 *                      Must have been acquired before.
 * @param[in] service   The service to be called.
 *                      Must not be NULL.
 * @param[in] bytes     Size of the payload in bytes.
 * @param[in] mask      Event mask to be notified when the request is answered.
 *                      Set this to 0 for "fire and forget" requests.
 * @param[in] deadline  Deadline offset until when this request is valid.
 *                      A value of 0 indicates that there is no deadline.
 *                      Ignored if URT_CFG_RPC_QOS_DEADLINECHECKS false.
 *
 * @return  Always returns URT_STATUS_OK.
 */
inline urt_status_t urtHrtRequestSubmit(urt_hrtrequest_t* request, urt_service_t* service, size_t bytes, urt_osEventMask_t mask, urt_delay_t deadline)
{
  return request->vmt->submit(request, service, bytes, mask, deadline);
}

/**
 * @brief   Retrieve whether an HRT request was submitted but not retrieved yet.
 *
 * @param[in] request   Request to check.
 *
 * @return  Indicator whether the request is pending.
 */
inline bool urtHrtRequestIsPending(const urt_hrtrequest_t* request)
{
  return urtRequestIsPending((const urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve an HRT request.
 *
 * @param[in] request   The request to retrieve.
 *                      Must not be NULL.
 * @param[in]  policy   Policy how to retrieve the request.
 * @param[out] bytes    Size of the payload to be returned by reference (optional).
 *                      May be NULL.
 * @param[out] latency  The latency to be returned by reference (optional).
 *                      May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                  Request was retrieved normally.
 *                                        The request is locked.
 * @retval URT_STATUS_REQUEST_BADOWNER    Request had to be removed from a service.
 *                                        If policy was set to URT_REQUEST_RETRIEVE_ENFORCING, the request is locked.
 * @retval URT_STATUS_REQUEST_LOCKED      Request could not be acquired due to a locked mutex.
 *                                        Only occurs if the policy was set to URT_REQUEST_RETRIEVE_LAZY.
 *                                        Request is not locked.
 * @retval URT_STATUS_DEADLINEVIOLATION   Request was retrieved, but deadline has been violated.
 *                                        The request is locked.
 *                                        Only occurs for FRT and HRT requests.
 * @retval URT_STATUS_JITTERVIOLATION     Request was retrieved, but jitter has been jiolated.
 *                                        The request is locked.
 *                                        Only occurs for FRT and HRT requests.
 */
inline urt_status_t urtHrtRequestRetrieve(urt_hrtrequest_t* request, urt_request_retrievepolicy_t policy, size_t* bytes, urt_delay_t* latency)
{
  return request->vmt->retrieve(request, policy, bytes, latency);
}

/**
 * @brief   Release an HRT request.
 *
 * @param[in] request  Request to release.
 *                     Must not be NULL.
 *                     Must have been acquired before.
 *
 * @return  Always returns URT_STATUS_OK.
 */
inline urt_status_t urtHrtRequestRelease(urt_hrtrequest_t* request)
{
  return urtRequestRelease((urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve the pointer to the associated payload of an HRT request.
 *
 * @param[in] request   Request to retrieve the payload from.
 *
 * @return  Pointer to a payload object or NULL.
 */
inline void* urtHrtRequestGetPayload(urt_hrtrequest_t* request)
{
  return urtRequestGetPayload((urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve the current payload size of an HRT request.
 *
 * @param[in] request   Request to retrieve the payload size from.
 *
 * @return  Size of the payload in bytes.
 */
inline size_t urtHrtRequestGetPayloadSize(const urt_hrtrequest_t* request)
{
  return urtRequestGetPayloadSize((const urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve the submission time of an HRT request.
 *
 * @param[in] request   Request of which the submission time shall be retrieved.
 *
 * @return  Pointer to the submission time of the request.
 */
inline const urt_osTime_t* urtHrtRequestGetSubmissionTime(const urt_hrtrequest_t* request)
{
  return urtRequestGetSubmissionTime((const urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve the age of an HRT request (since submission).
 *
 * @param[in] request   Request of which the age shall be retrieved.
 *
 * @return  Age of the request (since submission).
 * @retval 0  The request was not submitted yet.
 */
inline urt_delay_t urtHrtRequestGetAge(const urt_hrtrequest_t* request)
{
  return urtRequestGetAge((const urt_baserequest_t*)request);
}

/**
 * @brief   Retrieve whether the HRT request was submitted with fire-and-forget
 *          intentions.
 *
 * @param[in] request   Request to check for fire-and-forget intentions.
 *
 * @return  Indicator, whether the request was submitted with fire-and-forget
 *          intentions.
 * @retval true   Fire-and-forget request: no need to respond to.
 * @retval false  No fire-and-forget request: must be responded to.
 */
inline bool urtHrtRequestIsFireAndForget(const urt_hrtrequest_t* const request)
{
  return request->notification.mask == 0;
}

/** @} */

#endif /* (URT_CFG_RPC_ENABLED == true) */
#endif /* URT_REQUEST_H */

