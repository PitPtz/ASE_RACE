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
 * @file    urt_service.h
 * @brief   Service structure and interfaces.
 *
 * @defgroup rpc_service Service
 * @ingroup rpc
 * @brief   Services provide a function, which is callable remotely via requests.
 * @details When a request is submitted, is is enqueued at the service and the
 *          node thread the service resides in is signaled.
 *          The service then dispatches the requests, performes the according
 *          processing and eventually returns a response.
 *
 * @addtogroup rpc_service
 * @{
 */

#ifndef URT_SERVICE_H
#define URT_SERVICE_H

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
typedef struct urt_baserequest urt_baserequest_t;
typedef struct urt_nrtrequest urt_nrtrequest_t;
typedef struct urt_srtrequest urt_srtrequest_t;
typedef struct urt_frtrequest urt_frtrequest_t;
typedef struct urt_hrtrequest urt_hrtrequest_t;
/** @endcond */

#if (URT_CFG_RPC_PROFILING == true) || defined (__DOXYGEN__)
/**
 * @brief   Service profiling data structure.
 * @ingroup µRT_service
 */
typedef struct urt_service_profilingdata
{
  /**
   * @brief   Counter how often this service is called.
   */
  urt_profilingcounter_t numCalls;

  /**
   * @brief   Counter how often ownership was lost when servicing a request.
   */
  urt_profilingcounter_t numOwnershipLost;

} urt_service_profilingdata_t;
#endif /* (URT_CFG_RPC_PROFILING == true) */

/**
 * @brief   Type to uniquely identify services.
 */
#if (URT_CFG_RPC_SERVICEID_WIDTH > 0 && URT_CFG_RPC_SERVICEID_WIDTH <= 8)
typedef uint8_t urt_serviceid_t;
#elif (URT_CFG_RPC_SERVICEID_WIDTH > 0 && URT_CFG_RPC_SERVICEID_WIDTH <= 16) || defined(__DOXYGEN__)
typedef uint16_t urt_serviceid_t;
#elif (URT_CFG_RPC_SERVICEID_WIDTH > 0 && URT_CFG_RPC_SERVICEID_WIDTH <= 32)
typedef uint32_t urt_serviceid_t;
#elif (URT_CFG_RPC_SERVICEID_WIDTH > 0 && URT_CFG_RPC_SERVICEID_WIDTH <= 64)
typedef uint64_t urt_serviceid_t;
#else
#error "URT_CFG_RPC_SERVICEID_WIDTH must be in range [1,64]."
#endif

/**
 * @brief   Maximum service identifier value.
 * @note    Minimum value is always 0.
 */
#if (URT_CFG_RPC_SERVICEID_WIDTH < 64) || defined(__DOXYGEN__)
#define URT_RPC_SERVICEID_MAXVAL                (((urt_serviceid_t)1 << URT_CFG_RPC_SERVICEID_WIDTH) - 1)
#else /* (URT_CFG_RPC_SERVICEID_WIDTH < 64) */
#define URT_RPC_SERVICEID_MAXVAL                ((urt_serviceid_t)~0)
#endif /* (URT_CFG_RPC_SERVICEID_WIDTH < 64) */

/**
 * @brief   µRT service structure.
 * @ingroup µRT_service
 */
typedef struct urt_service
{
  /**
   * @brief   Pointer to the next service i a singly-linked list.
   */
  struct urt_service* next;

  /**
   * @brief   Unique identifier of the service.
   */
  urt_serviceid_t id;

  /**
   * @brief   Mutex lock for exclusive access.
   */
  urt_osMutex_t lock;

  /**
   * @brief   Request queue related data.
   */
  struct {
    /**
     * @brief   Pointer to the first pending request in a queue.
     * @details If NULL there are no pending requests (queue is empty).
     */
    urt_baserequest_t* front;

    /**
     * @brief   Pointer to the last pending HRT request in a queue.
     * @details If NULL there are no pending HRT requests.
     */
    urt_hrtrequest_t* hrtBack;

    /**
     * @brief   Pointer to the last pending FRT/SRT request in a queue.
     * @details If NULL there are no pending SRT or FRT requests.
     */
    urt_baserequest_t* fsrtBack;

    /**
     * @brief   Pointer to the last pending request in a queue.
     * @details If NULL there are no pending requests (queue is empty).
     */
    urt_baserequest_t* back;
  } queue;

  /**
   * @brief   Information required to notify the thread this service resides in.
   */
  struct {
    /**
     * @brief   Pointer to the thread which provides this service.
     */
    urt_osThread_t* thread;

    /**
     * @brief   Event mask to set when signaling the servicing thread.
     */
    urt_osEventMask_t mask;
  } notification;

#if (URT_CFG_RPC_PROFILING == true) || defined (__DOXYGEN__)
  /**
   * @brief   Profiling data.
   */
  urt_service_profilingdata_t profiling;
#endif /* (URT_CFG_RPC_PROFILING == true) */

} urt_service_t;

/**
 * @brief   Data structure representing a dispatched request.
 */
typedef struct urt_service_dispatched
{
  /**
   * @brief   Pointer to the dispatched request.
   */
  urt_baserequest_t* request;

  /**
   * @brief   Submission time of the dispatched request.
   * @details This is required to ensure that a request was not aborted and re-submitted while the service is processing the original submission.
   */
  urt_osTime_t submissionTime;

} urt_service_dispatched_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

  urt_status_t urtServiceInit(urt_service_t* service, urt_serviceid_t id, urt_osThread_t* thread, urt_osEventMask_t mask);
  bool urtServiceDispatch(urt_service_t* service, urt_service_dispatched_t* dispatched, void* payload, size_t* bytes, bool* noreturn);
  urt_status_t urtServiceAcquireRequest(urt_service_t* service, const urt_service_dispatched_t* dispatched);
  urt_status_t urtServiceTryAcquireRequest(urt_service_t* service, const urt_service_dispatched_t* dispatched);
  urt_status_t urtServiceRespond(urt_service_dispatched_t* dispatched, size_t bytes);
  size_t urtServiceQueueLength(urt_service_t* service, size_t* hrt, size_t* fsrt, size_t* nrt);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/**
 * @brief   Check whether a request has been dispatched.
 *
 * @param[in] dispatched  Dispatched request.
 *                        Must not be NULL.
 *
 * @return  Indicator whether a request has been dispatched.
 * @retval true   A request has been dispatched.
 * @retval flase  No request has been dispatched.
 */
static inline bool urtServiceDispatchedIsValid(urt_service_dispatched_t* const dispatched)
{
  return (dispatched->request != NULL);
}

/**
 * @brief   Retrieve the request object from a dispatched request.
 *
 * @param[in] dispatched  Dispatched request.
 *                        Must not be NULL.
 *
 * @return  Pointer to request object which has been dispatched.
 */
static inline urt_baserequest_t* urtServiceDispatchedGetRequest(urt_service_dispatched_t* const dispatched)
{
  return dispatched->request;
}

/**
 * @brief   Retrieve the submission time of a dispatched request.
 *
 * @param[in] dispatched  Dispatched request to retrieve the submission time from.
 *                        Must not be NULL.
 *
 * @return  Pointer to the submission time.
 */
static inline urt_osTime_t const* urtServiceDispatchedGetSubmissionTime(urt_service_dispatched_t* const dispatched)
{
  urtDebugAssert(dispatched != NULL);

  return &dispatched->submissionTime;
}

#endif /* (URT_CFG_RPC_ENABLED == true) */
#endif /* URT_SERVICE_H */

/** @} */
