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

#if (URT_CFG_RPC_ENABLED == true || defined(__DOXYGEN__))

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
urt_status_t _urt_nrtequest_submit_vmtcb(urt_nrtrequest_t* request, urt_service_t* service, size_t bytes, urt_osEventMask_t mask, urt_delay_t deadline);
urt_status_t _urt_nrtrequest_retrieve(urt_nrtrequest_t* request, urt_request_retrievepolicy_t policy, size_t* bytes, urt_delay_t* latency);
urt_status_t _urt_srtequest_submit_vmtcb(urt_srtrequest_t* request, urt_service_t* service, size_t bytes, urt_osEventMask_t mask, urt_delay_t deadline);
urt_status_t _urt_srtrequest_retrieve(urt_srtrequest_t* request, urt_request_retrievepolicy_t policy, size_t* bytes, urt_delay_t* latency);
urt_status_t _urt_frtrequest_submit(urt_frtrequest_t* request, urt_service_t* service, size_t bytes, urt_osEventMask_t mask, urt_delay_t deadline);
urt_status_t _urt_frtrequest_retrieve(urt_frtrequest_t* request, urt_request_retrievepolicy_t policy, size_t* bytes, urt_delay_t* latency);
urt_status_t _urt_hrtrequest_submit(urt_hrtrequest_t* request, urt_service_t* service, size_t bytes, urt_osEventMask_t mask, urt_delay_t deadline);
urt_status_t _urt_hrtrequest_retrieve(urt_hrtrequest_t* request, urt_request_retrievepolicy_t policy, size_t* bytes, urt_delay_t* latency);

/**
 * @brief   Virtual method table for NRT requests.
 */
const struct urt_baserequest_VMT _urt_nrtrequest_vmt = {
  .submit             = (urt_status_t(*)(void*, urt_service_t*, size_t, urt_osEventMask_t, urt_delay_t))_urt_nrtequest_submit_vmtcb,
  .retrieve           = (urt_status_t(*)(void*, urt_request_retrievepolicy_t, size_t*, urt_delay_t*))_urt_nrtrequest_retrieve,
};

/**
 * @brief   Virtual method table for SRT requests.
 */
const struct urt_baserequest_VMT _urt_srtrequest_vmt = {
  .submit             = (urt_status_t (*)(void*, urt_service_t*, size_t, urt_osEventMask_t, urt_delay_t))_urt_srtequest_submit_vmtcb,
  .retrieve           = (urt_status_t (*)(void*, urt_request_retrievepolicy_t, size_t*, urt_delay_t*))_urt_srtrequest_retrieve,
};

/**
 * @brief   Virtual method table for FRT requests.
 */
const struct urt_baserequest_VMT _urt_frtrequest_vmt = {
  .submit             = (urt_status_t (*)(void*, urt_service_t*, size_t, urt_osEventMask_t, urt_delay_t))_urt_frtrequest_submit,
  .retrieve           = (urt_status_t (*)(void*, urt_request_retrievepolicy_t, size_t*, urt_delay_t*))_urt_frtrequest_retrieve,
};

/**
 * @brief   Virtual method table for HRT requests.
 */
const struct urt_baserequest_VMT _urt_hrtrequest_vmt = {
  .submit             = (urt_status_t (*)(void*, urt_service_t*, size_t, urt_osEventMask_t, urt_delay_t))_urt_hrtrequest_submit,
  .retrieve           = (urt_status_t (*)(void*, urt_request_retrievepolicy_t, size_t*, urt_delay_t*))_urt_hrtrequest_retrieve,
};

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   Initialize a base request.
 *
 * @param[in] request   The base request to initialize.
 *                      Must not be NULL.
 * @param[in] payload   May be NULL for requests without payload.
 */
void _urt_baserequest_init(urt_baserequest_t* const request, void* const payload)
{
  urtDebugAssert(request != NULL);

  request->prev = NULL;
  request->next = NULL;
  urtMutexInit(&request->lock);
  request->service = NULL;
  urtTimeSet(&request->submissionTime, 0);
  request->notification.thread = NULL;
  request->notification.mask = 0;
  request->payload = payload;
  request->size = 0;

#if (URT_CFG_RPC_PROFILING == true)
  request->profiling.sumLatencies = 0;
  request->profiling.numCalls = 0;
  request->profiling.numFails = 0;
  request->profiling.maxLatency = URT_DELAY_IMMEDIATE;
  request->profiling.minLatency = URT_DELAY_INFINITE;
#endif /* (URT_CFG_RPC_PROFILING == true) */

  return;
}

/**
 * @brief   Prepare a request for submission.
 * @details Most data of the request is set by this function.
 *
 * @param[in] request   Request to prepare.
 *                      Must not be NULL.
 * @param[in] service   Service this request is about to be submitted to.
 *                      Must not be NULL.
 * @param[in] bytes     Size of the payload in bytes.
 * @param[in] mask      Event mask to be notified when the request is answered.
 *                      Set this to 0 for "fire and forget" requests.
 */
void _urt_baserequest_prepareForSubmission(urt_baserequest_t* const request, urt_service_t* const service, size_t const bytes, urt_osEventMask_t const mask)
{
  urtDebugAssert(request != NULL);
  urtDebugAssert(service != NULL);

  // reset queue pointers
  request->prev = NULL;
  request->next = NULL;

  // associate service
  request->service = service;

  // set submission time
  urtTimeNow(&request->submissionTime);

  // set notification information
  if (mask != 0) {
    request->notification.thread = urtThreadGetSelf();
    request->notification.mask = mask;
  }

  // set payload size
  request->size = bytes;

  return;
}

/**
 * @brief   Signal a servicing node thread and unlock the request.
 *
 * @param[in] request   Request to unlock.
 *                      Must not be NULL.
 */
void _urt_baserequest_signalServiceAndUnlock(urt_baserequest_t* const request)
{
  urtDebugAssert(request != NULL);

#if (URT_CFG_RPC_PROFILING == true)
  // increment call counters
  ++request->service->profiling.numCalls;
  ++request->profiling.numCalls;
#endif /* (URT_CFG_RPC_PROFILING == true) */

  // signal service node thread
  urtEventSignal(request->service->notification.thread, request->service->notification.mask);
  // unlock service
  urtMutexUnlock(&request->service->lock);
  // unlock request
  urtMutexUnlock(&request->lock);

  return;
}

/**
 * @brief   Detach a request from its associated service.
 *
 * @param[in] request   Request to detach.
 *                      Must be associated to a service.
 */
void _urt_baserequest_detach(urt_baserequest_t* const request)
{
  urtDebugAssert(request != NULL);

  // lock associated service
  urtMutexLock(&request->service->lock);

  // if this was the first request in the queue
  if (request->prev == NULL) {
    request->service->queue.front = request->next;
    if (request->next != NULL) {
      request->next->prev = NULL;
    }
  } else {
    request->prev->next = request->next;
  }

  // if this was the hindmost HRT request in the queue
  if ((urt_baserequest_t*)request->service->queue.hrtBack == request) {
    request->service->queue.hrtBack = (urt_hrtrequest_t*)request->prev;
  }
  // if this was the hindmost SRT/FRT request in the queue
  else if (request->service->queue.fsrtBack == request) {
    request->service->queue.fsrtBack = (request->prev == (urt_baserequest_t*)request->service->queue.hrtBack) ? NULL : request->prev;
  }

  // if this was the hindmost request in the queue
  if (request->next == NULL) {
    request->service->queue.back = request->prev;
    if (request->prev != NULL) {
      request->prev->next = request->next;
    }
  } else {
    request->next->prev = request->prev;
  }

#if (URT_CFG_RPC_PROFILING == true)
  // increment conter of lost ownerships
  ++request->service->profiling.numOwnershipLost;
#endif /* (URT_CFG_RPC_PROFILING == true) */

  // unlock associated service
  urtMutexUnlock(&request->service->lock);

  // reset request
  request->prev = NULL;
  request->next = NULL;
  request->service = NULL;
  // explicitely do not reset submission time
  request->notification.thread = NULL;
  // notification mask does not need to be reseted

#if (URT_CFG_RPC_PROFILING == true)
  // increment conter of failed calls
  ++request->profiling.numFails;
#endif /* (URT_CFG_RPC_PROFILING == true) */

  return;
}

#if (URT_CFG_RPC_PROFILING == true) || defined(__DOXYGEN__)
/**
 * @brief   Track latencies of a request for profiling.
 *
 * @param[in] request   Request to track latency for.
 * @param[in] latency   Latency to track.
 */
void _urt_baserequest_trackLatencies(urt_baserequest_t* const request, urt_delay_t const latency)
{
  urtDebugAssert(request != NULL);

  // accumulate latencies
  request->profiling.sumLatencies += latency;

  // update min/max latency
  if (request->profiling.minLatency > request->profiling.maxLatency ||
      latency < request->profiling.minLatency) {
    request->profiling.minLatency = latency;
  }
  if (request->profiling.minLatency > request->profiling.maxLatency ||
      latency > request->profiling.maxLatency) {
    request->profiling.maxLatency = latency;
  }

  return;
}
#endif /* (URT_CFG_RPC_PROFILING == true) */

/* NRT ************************************************************************/

/**
 * @brief   Appends an NRT request to its associated service's queue.
 *
 * @param[in] request   Request to append.
 */
void _urt_nrtrequest_append(urt_nrtrequest_t* const request)
{
  urtDebugAssert(request != NULL);

  // service already holds a request?
  if (request->service->queue.back != NULL) {
    // append request at the very end
    request->prev = request->service->queue.back;
    request->prev->next = (urt_baserequest_t*)request;
  }
  // request queue is empty
  else {
    // insert first element
    request->service->queue.front = (urt_baserequest_t*)request;
  }

  // set queue back pointer
  request->service->queue.back = (urt_baserequest_t*)request;

  return;
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
urt_status_t _urt_nrtrequest_submit(urt_nrtrequest_t* request, urt_service_t* service, size_t bytes, urt_osEventMask_t mask)
{
  urtDebugAssert(request != NULL);
  urtDebugAssert(service != NULL);
  urtDebugAssert((request->payload == NULL && bytes == 0) || request->payload != NULL);

  // set request data
  _urt_baserequest_prepareForSubmission((urt_baserequest_t*)request, service, bytes, mask);

  // append request to the service's queue
  urtMutexLock(&service->lock);
  _urt_nrtrequest_append(request);

  // signal servicing thread and unlock service and request
  _urt_baserequest_signalServiceAndUnlock((urt_baserequest_t*)request);

  return URT_STATUS_OK;
}

/**
 * @brief   NRT request VMT wrapper for the 'submit' method.
 *
 * @param[in] request   The request to be used for this call.
 *                      Must not be NULL.
 *                      Must have been acquired before.
 * @param[in] service   The service to be called.
 *                      Must not be NULL.
 * @param[in] bytes     Size of the payload in bytes.
 * @param[in] mask      Event mask to be notified when the request is answered.
 * @param[in] deadline  Deadline does not apply for NRT requests and is ignored.
 *
 * @return  Always returns URT_STATUS_OK.
 */
urt_status_t _urt_nrtequest_submit_vmtcb(urt_nrtrequest_t* request, urt_service_t* service, size_t bytes, urt_osEventMask_t mask, urt_delay_t deadline)
{
  (void)deadline;
  return _urt_nrtrequest_submit(request, service, bytes, mask);
}

/**
 * @brief   Retrieve an NRT request.
 *
 * @param[in]  request  The request to retrieve.
 *                      Must not be NULL.
 * @param[in]  policy   Policy how to retrieve the request.
 * @param[out] bytes    Size of the payload to be returned by reference (optional).
 *                      May be NULL.
 * @param[out] latency  The latency to be returned by reference (optional).
 *                      May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Request was retrieved normally.
 *                                      The request is locked.
 * @retval URT_STATUS_REQUEST_BADOWNER  Request had to be removed from a service.
 *                                      If policy was set to URT_REQUEST_RETRIEVE_ENFORCING, the request is locked.
 * @retval URT_STATUS_REQUEST_LOCKED    Request could not be acquired due to a locked mutex.
 *                                      Only occurs if the policy was set to URT_REQUEST_RETRIEVE_LAZY.
 *                                      Request is not locked.
 */
urt_status_t _urt_nrtrequest_retrieve(urt_nrtrequest_t* request, urt_request_retrievepolicy_t policy, size_t* bytes, urt_delay_t* latency)
{
  urtDebugAssert(request != NULL);

  // local variables
  urt_status_t status = URT_STATUS_OK;

  // lock request
  if (policy != URT_REQUEST_RETRIEVE_LAZY) {
    urtMutexLock(&request->lock);
  } else {
    if (!urtMutexTryLock(&request->lock)) {
      return URT_STATUS_REQUEST_LOCKED;
    }
  }

  // request is associated to a service?
  if (request->service != NULL) {
    if (policy == URT_REQUEST_RETRIEVE_ENFORCING) {
      // remove request from service 'by force'
      _urt_baserequest_detach((urt_baserequest_t*)request);
      status = URT_STATUS_REQUEST_BADOWNER;
    } else {
      // unlock and return
      urtMutexUnlock(&request->lock);
      return URT_STATUS_REQUEST_BADOWNER;
    }
  }

  // reset notification thread
  request->notification.thread = NULL;

  // store payload size
  if (bytes) *bytes = request->size;

  // calculate latency if requested
#if (URT_CFG_RPC_PROFILING == true)
  urt_osTime_t t;
  urtTimeNow(&t);
  const urt_delay_t lat = (urt_delay_t)urtTimeDiff(&request->submissionTime, &t);
  // track latencies
  _urt_baserequest_trackLatencies((urt_baserequest_t*)request, lat);
  // set optional latency output argument
  if (latency) *latency = lat;
#else /* (URT_CFG_RPC_PROFILING == true) */
  if (latency) {
    urt_osTime_t t;
    urtTimeNow(&t);
    *latency = (urt_delay_t)urtTimeDiff(&request->submissionTime, &t);
  }
#endif /* (URT_CFG_RPC_PROFILING == true) */

  return status;
}

/* SRT ************************************************************************/

/**
 * @brief   Enqueues an SRT or FRT request in its associated service's queue.
 *
 * @param[in] request   Request to enqueue.
 */
void _urt_sfrtrequest_append(urt_baserequest_t* const request)
{
  urtDebugAssert(request != NULL);
  urtDebugAssert(urtRequestGetRtClass(request) == URT_SRT || urtRequestGetRtClass(request) == URT_FRT);

  // service already holds an SRT or FRT request?
  if (request->service->queue.fsrtBack != NULL) {
    // enqeue after last SRT/FRT request
    request->prev = request->service->queue.fsrtBack;
    request->next = request->prev->next;
    request->prev->next = request;
    if (request->next) {
      request->next->prev = request;
    } else {
      request->service->queue.back = request;
    }
  }
  // service holds an HRT request?
  else if (request->service->queue.hrtBack != NULL) {
    // enqueue after last HRT request
    request->prev = (urt_baserequest_t*)request->service->queue.hrtBack;
    request->next = request->prev->next;
    request->prev->next = request;
    if (request->next) {
      request->next->prev = request;
    } else {
      request->service->queue.back = request;
    }
  }
  // service holds an NRT request?
  else if (request->service->queue.front != NULL) {
    // prepend request
    request->next = request->service->queue.front;
    request->service->queue.front = request;
    request->next->prev = request;
  }
  // request queue is empty
  else {
    // insert first element
    request->service->queue.front = request;
    request->service->queue.back = request;
  }

  // set pointer to last SRT/FRT request
  request->service->queue.fsrtBack = request;

  return;
}

/**
 * @brief   Submit an SRT request.
 *
 * @param[in] request   The SRT request to be used for this call.
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
urt_status_t _urt_srtrequest_submit(urt_srtrequest_t* request, urt_service_t* service, size_t bytes, urt_osEventMask_t mask)
{
  urtDebugAssert(request != NULL);
  urtDebugAssert(service != NULL);
  urtDebugAssert((request->payload == NULL && bytes == 0) || request->payload != NULL);

  // set request data
  _urt_baserequest_prepareForSubmission((urt_baserequest_t*)request, service, bytes, mask);

  // enqueue request in the service's queue
  urtMutexLock(&service->lock);
  _urt_sfrtrequest_append((urt_baserequest_t*)request);

  // signal servicing thread and unlock service and request
  _urt_baserequest_signalServiceAndUnlock((urt_baserequest_t*)request);

  return URT_STATUS_OK;
}

/**
 * @brief   SRT request VMT wrapper for the 'submit' method.
 *
 * @param[in] request   The SRT request to be used for this call.
 *                      Must not be NULL.
 *                      Must have been acquired before.
 * @param[in] service   The service to be called.
 *                      Must not be NULL.
 * @param[in] bytes     Size of the payload in bytes.
 * @param[in] mask      Event mask to be notified when the request is answered.
 *                      Set this to 0 for "fire and forget" requests.
 * @param deadline      Deadline does not apply for SRT requests and is ignored.
 *
 * @return  Always returns URT_STATUS_OK.
 */
urt_status_t _urt_srtequest_submit_vmtcb(urt_srtrequest_t* request, urt_service_t* service, size_t bytes, urt_osEventMask_t mask, urt_delay_t deadline)
{
  (void)deadline;
  return _urt_srtrequest_submit(request, service, bytes, mask);
}

/**
 * @brief   Retrieve an SRT request.
 *
 * @param[in] request   The SRT request to retrieve.
 *                      Must not be NULL.
 * @param[in]  policy   Policy how to retrieve the request.
 * @param[out] bytes    Size of the payload to be returned by reference (optional).
 *                      May be NULL.
 * @param[out] latency  The latency to be returned by reference (optional).
 *                      May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Request was retrieved normally.
 *                                      The request is locked.
 * @retval URT_STATUS_REQUEST_BADOWNER  Request had to be removed from a service.
 *                                      If policy was set to URT_REQUEST_RETRIEVE_ENFORCING, the request is locked.
 * @retval URT_STATUS_REQUEST_LOCKED    Request could not be acquired due to a locked mutex.
 *                                      Only occurs if the policy was set to URT_REQUEST_RETRIEVE_LAZY.
 *                                      Request is not locked.
 */
urt_status_t _urt_srtrequest_retrieve(urt_srtrequest_t* request, urt_request_retrievepolicy_t policy, size_t* bytes, urt_delay_t* latency)
{
  urtDebugAssert(request != NULL);

  // local variables
  urt_status_t status = URT_STATUS_OK;

  // lock request
  if (policy != URT_REQUEST_RETRIEVE_LAZY) {
    urtMutexLock(&request->lock);
  } else {
    if (!urtMutexTryLock(&request->lock)) {
      return URT_STATUS_REQUEST_LOCKED;
    }
  }

  // request is associated to a service?
  if (request->service != NULL) {
    if (policy == URT_REQUEST_RETRIEVE_ENFORCING) {
      // remove request from service 'by force'
      _urt_baserequest_detach((urt_baserequest_t*)request);
      status = URT_STATUS_REQUEST_BADOWNER;
    } else {
      // unlock and return
      urtMutexUnlock(&request->lock);
      return URT_STATUS_REQUEST_BADOWNER;
    }
  }

  // reset notification thread
  request->notification.thread = NULL;

  // store payload size
  if (bytes) *bytes = request->size;

  // calculate latency if requested
#if (URT_CFG_RPC_PROFILING == true)
  urt_osTime_t t;
  urtTimeNow(&t);
  const urt_delay_t lat = (urt_delay_t)urtTimeDiff(&request->submissionTime, &t);
  // track latencies
  _urt_baserequest_trackLatencies((urt_baserequest_t*)request, lat);
  // set optional latency output argument
  if (latency) *latency = lat;
#else /* (URT_CFG_RPC_PROFILING == true) */
  if (latency) {
    urt_osTime_t t;
    urtTimeNow(&t);
    *latency = (urt_delay_t)urtTimeDiff(&request->submissionTime, &t);
  }
#endif /* (URT_CFG_RPC_PROFILING == true) */

  return status;
}

/* FRT ************************************************************************/

/**
 * @brief   Submit an FRT request.
 *
 * @param[in] request   The FRT request to be used for this call.
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
urt_status_t _urt_frtrequest_submit(urt_frtrequest_t* request, urt_service_t* service, size_t bytes, urt_osEventMask_t mask, urt_delay_t deadline)
{
  urtDebugAssert(request != NULL);
  urtDebugAssert(service != NULL);
  urtDebugAssert((request->payload == NULL && bytes == 0) || request->payload != NULL);

  // set request data
  _urt_baserequest_prepareForSubmission((urt_baserequest_t*)request, service, bytes, mask);

#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true)
  // set deadline
  request->deadlineOffset = deadline;
#else /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) */
  (void)deadline;
#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) */

  // enqueue request in the service's queue
  urtMutexLock(&service->lock);
  _urt_sfrtrequest_append((urt_baserequest_t*)request);

  // signal servicing thread and unlock service and request
  _urt_baserequest_signalServiceAndUnlock((urt_baserequest_t*)request);

  return URT_STATUS_OK;
}

/**
 * @brief   Retrieve an FRT request.
 *
 * @param[in] request   The FRT request to retrieve.
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
 * @retval URT_STATUS_DEADLINEVIOLATION   Request was retrieved, but deadline has been violated.
 *                                        The request is locked.
 * @retval URT_STATUS_JITTERVIOLATION     Request was retrieved, but jitter has been jiolated.
 *                                        The request is locked.
 * @retval URT_STATUS_REQUEST_BADOWNER    Request had to be removed from a service.
 *                                        If policy was set to URT_REQUEST_RETRIEVE_ENFORCING, the request is locked.
 * @retval URT_STATUS_REQUEST_LOCKED      Request could not be acquired due to a locked mutex.
 *                                        Only occurs if the policy was set to URT_REQUEST_RETRIEVE_LAZY.
 *                                        Request is not locked.
 */
urt_status_t _urt_frtrequest_retrieve(urt_frtrequest_t* request, urt_request_retrievepolicy_t policy, size_t* bytes, urt_delay_t* latency)
{
  urtDebugAssert(request != NULL);

  // local variables
  urt_status_t status = URT_STATUS_OK;

  // lock request
  if (policy != URT_REQUEST_RETRIEVE_LAZY) {
    urtMutexLock(&request->lock);
  } else {
    if (!urtMutexTryLock(&request->lock)) {
      return URT_STATUS_REQUEST_LOCKED;
    }
  }

  // request is associated to a service?
  if (request->service != NULL) {
    if (policy == URT_REQUEST_RETRIEVE_ENFORCING) {
      // remove request from service 'by force'
      _urt_baserequest_detach((urt_baserequest_t*)request);
      status = URT_STATUS_REQUEST_BADOWNER;
    } else {
      // unlock and return
      urtMutexUnlock(&request->lock);
      return URT_STATUS_REQUEST_BADOWNER;
    }
  }

#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true) || (URT_CFG_RPC_QOS_JITTERCHECKS == true) || (URT_CFG_RPC_PROFILING == true)
  // calculate latency
  urt_delay_t lat;
  {
    urt_osTime_t t;
    urtTimeNow(&t);
    lat = (urt_delay_t)urtTimeDiff(&request->submissionTime, &t);
  }

  // set optional output argument
  if (latency) *latency = lat;

#if (URT_CFG_RPC_PROFILING == true)
  // track latencies
  _urt_baserequest_trackLatencies((urt_baserequest_t*)request, lat);
#endif /* (URT_CFG_RPC_PROFILING == true) */

#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true)
  // check for deadline violation (only if nothing went wrong so far)
  if (status == URT_STATUS_OK &&
      request->deadlineOffset != 0 && lat > request->deadlineOffset) {
    status = URT_STATUS_DEADLINEVIOLATION;
  }
#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) */

#if (URT_CFG_RPC_QOS_JITTERCHECKS == true)
  // track jitter and check for violation (only if nothing went wrong so far)
  if (status == URT_STATUS_OK &&
      request->maxJitter != 0) {
    // update min/max latency (only if valid)
    if ((request->minLatency > request->maxLatency) ||
        (lat < request->minLatency && request->maxLatency - lat <= request->maxJitter)) {
      request->minLatency = lat;
    }
    if ((request->minLatency > request->maxLatency) ||
        (lat > request->maxLatency && lat - request->minLatency <= request->maxJitter)) {
      request->maxLatency = lat;
    }
    // check for violation
    if (lat < request->minLatency || lat > request->maxLatency) {
      status = URT_STATUS_JITTERVIOLATION;
    }
  }
#endif /* (URT_CFG_RPC_QOS_JITTERCHECKS == true) */

  // reset notification thread
  request->notification.thread = NULL;

  // store payload size
  if (bytes) *bytes = request->size;

#else /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) || (URT_CFG_RPC_QOS_JITTERCHECKS == true) || (URT_CFG_RPC_PROFILING == true) */
  // set optional output arguments
  if (bytes) *bytes = request->size;
  if (latency) {
    urt_osTime_t t;
    urtTimeNow(&t);
    *latency = (urt_delay_t)urtTimeDiff(&request->submissionTime, &t);
  }
#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) || (URT_CFG_RPC_QOS_JITTERCHECKS == true) || (URT_CFG_RPC_PROFILING == true) */

  return status;
}

/* HRT ************************************************************************/

#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true || URT_CFG_RPC_QOS_JITTERCHECKS == true) || defined(__DOXYGEN__)

/**
 * @brief   Calculate the absolute deadline of a HRT request.
 *
 * @param[in]  request  HRT request to calculate the deadline for.
 *                      Must not be NULL.
 * @param[out] t        Time obejct to store the deadline to.
 *                      Must not be NULL.
 */
void _urt_hrtrequest_calculateDeadline(urt_hrtrequest_t const* const request, urt_osTime_t* const t)
{
  urtDebugAssert(request != NULL);
  urtDebugAssert(t != NULL);

#if (URT_CFG_RPC_QOS_JITTERCHECKS == true)
  const urt_delay_t jitter_deadline = (request->maxJitter != 0 && request->minLatency <= request->maxLatency) ? (request->minLatency + request->maxJitter) : URT_DELAY_INFINITE;
#endif /* (URT_CFG_RPC_QOS_JITTERCHECKS == true) */

#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true) && (URT_CFG_RPC_QOS_JITTERCHECKS == true)
  const urt_delay_t deadline = (request->deadlineOffset != 0) ? request->deadlineOffset : URT_DELAY_INFINITE;
  const urt_delay_t min_deadline = (deadline < jitter_deadline) ? deadline : jitter_deadline;
  if (min_deadline != URT_DELAY_INFINITE) {
    *t = request->submissionTime;
    urtTimeAdd(t, min_deadline);
  } else {
    urtTimeSet(t, -1);
  }
#elif (URT_CFG_RPC_QOS_DEADLINECHECKS == true)
  if (request->deadlineOffset != 0) {
    *t = request->submissionTime;
    urtTimeAdd(t, request->deadlineOffset);
  } else {
    urtTimeSet(t, -1);
  }
#elif (URT_CFG_RPC_QOS_JITTERCHECKS == true)
  if (jitter_deadline != URT_DELAY_INFINITE) {
    *t = request->submissionTime;
    urtTimeAdd(t, jitter_deadline);
  } else {
    urtTimeSet(t, -1);
  }
#else
# error "invalid configuration"
#endif
  return;
}

/**
 * @brief _urt_hrtrequest_deadlinejitterrecoverywrapper
 * @param request
 */
void _urt_hrtrequest_deadlinejitterrecoverywrapper(urt_hrtrequest_t* request)
{
  // check current violation code
  if (request->recovery.violationCode == URT_STATUS_OK) {
    // calculate QoS timer delay (either limited by either deadline or jitter)
#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true) && (URT_CFG_RPC_QOS_JITTERCHECKS != true)
    request->recovery.violationCode = URT_STATUS_DEADLINEVIOLATION;
#elif (URT_CFG_RPC_QOS_DEADLINECHECKS != true) && (URT_CFG_RPC_QOS_JITTERCHECKS == true)
    request->recovery.violationCode = URT_STATUS_JITTERVIOLATION;
#elif (URT_CFG_RPC_QOS_DEADLINECHECKS == true) && (URT_CFG_RPC_QOS_JITTERCHECKS == true)
    {
     // calculate QoS timer delay (either limited by either deadline or jitter)
     const urt_delay_t delay = (request->maxJitter != 0 &&
                                request->minLatency <= request->maxLatency &&
                                (request->deadlineOffset == 0 ||
                                 (request->minLatency + request->maxJitter) < request->deadlineOffset)
                               ) ? request->minLatency + request->maxJitter : request->deadlineOffset;
     // set violation code and execute recovery callback
     request->recovery.violationCode = (delay == request->deadlineOffset) ? URT_STATUS_DEADLINEVIOLATION : URT_STATUS_JITTERVIOLATION;
   }
#endif
    request->recovery.callback(request->recovery.params);
  }

  return;
}

#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true || URT_CFG_RPC_QOS_JITTERCHECKS == true) */

/**
 * @brief   Enqueues an HRT request in its associated service's queue.
 *
 * @param[in] request   Request to enqueue.
 *                      Must not be NULL.
 */
void _urt_hrtrequest_enqueue(urt_hrtrequest_t* const request)
{
  urtDebugAssert(request != NULL);

  // service already holds an HRT request?
  if (request->service->queue.hrtBack != NULL) {
#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true || URT_CFG_RPC_QOS_JITTERCHECKS == true)
    // enqueue request depending on timing constraints
    urt_osTime_t request_deadline;
    _urt_hrtrequest_calculateDeadline(request, &request_deadline);
    // search for the first less critical request
    urt_hrtrequest_t* hrt_req = (urt_hrtrequest_t*)request->service->queue.front;
    urt_osTime_t dl;
    _urt_hrtrequest_calculateDeadline(hrt_req, &dl);
    while (urtTimeGet(&dl) <= urtTimeGet(&request_deadline)) {
      if (hrt_req != request->service->queue.hrtBack) {
        hrt_req = (urt_hrtrequest_t*)hrt_req->next;
        _urt_hrtrequest_calculateDeadline(hrt_req, &dl);
      } else {
        hrt_req = NULL;
        break;
      }
    }
    // if such a request was found
    if (hrt_req != NULL) {
      // insert before
      request->next = (urt_baserequest_t*)hrt_req;
      if (hrt_req->prev != NULL) {
        request->prev = (urt_baserequest_t*)hrt_req->prev;
        request->prev->next = (urt_baserequest_t*)request;
      } else {
        request->service->queue.front = (urt_baserequest_t*)request;
      }
      hrt_req->prev = (urt_baserequest_t*)request;
    } else {
      // insert after last HRT
      request->prev = (urt_baserequest_t*)request->service->queue.hrtBack;
      request->service->queue.hrtBack = request;
      if (request->prev->next != NULL) {
        request->next = request->prev->next;
        request->next->prev = (urt_baserequest_t*)request;
      } else {
        request->service->queue.back = (urt_baserequest_t*)request;
      }
      request->prev->next = (urt_baserequest_t*)request;
    }
#else /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true || URT_CFG_RPC_QOS_JITTERCHECKS == true) */
    // insert after last HRT
    request->prev = (urt_baserequest_t*)request->service->queue.hrtBack;
    request->service->queue.hrtBack = request;
    if (request->prev->next != NULL) {
      request->next = request->prev->next;
      request->next->prev = (urt_baserequest_t*)request;
    } else {
      request->service->queue.back = (urt_baserequest_t*)request;
    }
    request->prev->next = (urt_baserequest_t*)request;
#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true || URT_CFG_RPC_QOS_JITTERCHECKS == true) */
  }
  // service holds any non-HRT requests
  else if (request->service->queue.front != NULL) {
    // prepend request
    request->next = request->service->queue.front;
    request->service->queue.front = (urt_baserequest_t*)request;
    request->service->queue.hrtBack = request;
    request->next->prev = (urt_baserequest_t*)request;
  }
  // request queue is empty
  else {
    // insert first element
    request->service->queue.front = (urt_baserequest_t*)request;
    request->service->queue.back = (urt_baserequest_t*)request;
    request->service->queue.hrtBack = request;
  }

  return;
}

/**
 * @brief   Submit an HRT request.
 *
 * @param[in] request   The HRT request to be used for this call.
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
urt_status_t _urt_hrtrequest_submit(urt_hrtrequest_t* request, urt_service_t* service, size_t bytes, urt_osEventMask_t mask, urt_delay_t deadline)
{
  urtDebugAssert(request != NULL);
  urtDebugAssert(service != NULL);
  urtDebugAssert((request->payload == NULL && bytes == 0) || request->payload != NULL);

  // set request data
  _urt_baserequest_prepareForSubmission((urt_baserequest_t*)request, service, bytes, mask);

#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true)
  // set deadline
  request->deadlineOffset = deadline;
#else /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) */
  (void) deadline;
#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) */

#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true || URT_CFG_RPC_QOS_JITTERCHECKS == true)
  // calculate QoS timer delay (either limited by either deadline or jitter)
#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true) && (URT_CFG_RPC_QOS_JITTERCHECKS != true)
  const urt_delay_t delay = request->deadlineOffset;
#elif (URT_CFG_RPC_QOS_DEADLINECHECKS != true) && (URT_CFG_RPC_QOS_JITTERCHECKS == true)
  const urt_delay_t delay = (request->maxJitter != 0 && request->minLatency <= request->maxLatency) ? request->minLatency + request->maxJitter : 0;
#elif (URT_CFG_RPC_QOS_DEADLINECHECKS == true) && (URT_CFG_RPC_QOS_JITTERCHECKS == true)
  const urt_delay_t delay = (request->maxJitter != 0 &&
                             request->minLatency <= request->maxLatency &&
                             (request->deadlineOffset == 0 ||
                              (request->minLatency + request->maxJitter) < request->deadlineOffset)
                            ) ? request->minLatency + request->maxJitter : request->deadlineOffset;
#endif
  // arm the QoS timer
  if (delay != 0) {
    urt_osTime_t t = request->submissionTime;
    urtTimeAdd(&t, delay);
    if (request->recovery.callback == NULL) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true) && (URT_CFG_RPC_QOS_JITTERCHECKS != true)
      urtTimerSet(&request->qosTimer, &t, (urt_osTimerCallback_t)urtCorePanic, (void*)URT_STATUS_DEADLINEVIOLATION);
#elif (URT_CFG_RPC_QOS_DEADLINECHECKS != true) && (URT_CFG_RPC_QOS_JITTERCHECKS == true)
      urtTimerSet(&request->qosTimer, &t, (urt_osTimerCallback_t)urtCorePanic, (void*)URT_STATUS_JITTERVIOLATION);
#elif (URT_CFG_RPC_QOS_DEADLINECHECKS == true) && (URT_CFG_RPC_QOS_JITTERCHECKS == true)
      urtTimerSet(&request->qosTimer, &t, (urt_osTimerCallback_t)urtCorePanic, (delay == request->deadlineOffset) ? (void*)URT_STATUS_DEADLINEVIOLATION : (void*)URT_STATUS_JITTERVIOLATION);
#endif
#pragma GCC diagnostic pop
    } else {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
      urtTimerSet(&request->qosTimer, &t, (urt_osTimerCallback_t)_urt_hrtrequest_deadlinejitterrecoverywrapper, (void*)request);
#pragma GCC diagnostic pop
    }
  }
#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true || URT_CFG_RPC_QOS_JITTERCHECKS == true) */

  // enqueue request is ther service's queue
  urtMutexLock(&service->lock);
  _urt_hrtrequest_enqueue(request);

  // signal servicing thread and unlock service and request
  _urt_baserequest_signalServiceAndUnlock((urt_baserequest_t*)request);

  return URT_STATUS_OK;
}

/**
 * @brief   Retrieve an HRT request.
 *
 * @param[in] request   The HRT request to retrieve.
 *                      Must not be NULL.
 * @param[out] bytes    Size of the payload to be returned by reference (optional).
 *                      May be NULL.
 * @param[in]  policy   Policy how to retrieve the request.
 * @param[out] latency  The latency to be returned by reference (optional).
 *                      May be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                  Request was retrieved normally.
 *                                        The request is locked.
 * @retval URT_STATUS_DEADLINEVIOLATION   Request was retrieved, but deadline has been violated.
 *                                        The request is locked.
 * @retval URT_STATUS_JITTERVIOLATION     Request was retrieved, but jitter has been jiolated.
 *                                        The request is locked.
 * @retval URT_STATUS_REQUEST_BADOWNER    Request had to be removed from a service.
 *                                        If policy was set to URT_REQUEST_RETRIEVE_ENFORCING, the request is locked.
 * @retval URT_STATUS_REQUEST_LOCKED      Request could not be acquired due to a locked mutex.
 *                                        Only occurs if the policy was set to URT_REQUEST_RETRIEVE_LAZY.
 *                                        Request is not locked.
 */
urt_status_t _urt_hrtrequest_retrieve(urt_hrtrequest_t* request, urt_request_retrievepolicy_t policy, size_t* bytes, urt_delay_t* latency)
{
  urtDebugAssert(request != NULL);

  // local variables
  urt_status_t status = URT_STATUS_OK;

  // lock request
  if (policy != URT_REQUEST_RETRIEVE_LAZY) {
    urtMutexLock(&request->lock);
  } else {
    if (!urtMutexTryLock(&request->lock)) {
      return URT_STATUS_REQUEST_LOCKED;
    }
  }

  // request is associated to a service?
  if (request->service != NULL) {
    if (policy == URT_REQUEST_RETRIEVE_ENFORCING) {
      // remove request from service 'by force'
      _urt_baserequest_detach((urt_baserequest_t*)request);
      status = URT_STATUS_REQUEST_BADOWNER;
    } else {
      // unlock and return
      urtMutexUnlock(&request->lock);
      return URT_STATUS_REQUEST_BADOWNER;
    }
  }

#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true) || (URT_CFG_RPC_QOS_JITTERCHECKS == true) || (URT_CFG_RPC_PROFILING == true)
  // calculate latency
  urt_delay_t lat;
  {
    urt_osTime_t t;
    urtTimeNow(&t);
    lat = (urt_delay_t)urtTimeDiff(&request->submissionTime, &t);
  }

#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true || URT_CFG_RPC_QOS_JITTERCHECKS == true)
  // reset the QoS timer
  urtTimerReset(&request->qosTimer);
#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true || URT_CFG_RPC_QOS_JITTERCHECKS == true) */

  // set optional output arguments
  if (bytes) *bytes = request->size;
  if (latency) *latency = lat;

#if (URT_CFG_RPC_PROFILING == true)
  // track latencies
  _urt_baserequest_trackLatencies((urt_baserequest_t*)request, lat);
#endif /* (URT_CFG_RPC_PROFILING == true) */

#if (URT_CFG_RPC_QOS_JITTERCHECKS == true)
  // track jitter and check for violation (only if nothing went wrong so far)
  if (status == URT_STATUS_OK &&
      request->maxJitter != 0) {
    // update min/max latency (only if valid)
    if ((request->minLatency > request->maxLatency) ||
        (lat < request->minLatency && request->maxLatency - lat <= request->maxJitter)) {
      request->minLatency = lat;
    }
    if ((request->minLatency > request->maxLatency) ||
        (lat > request->maxLatency && lat - request->minLatency <= request->maxJitter)) {
      request->maxLatency = lat;
    }
    // check for violation
    if (lat < request->minLatency || lat > request->maxLatency) {
      if (request->recovery.callback == NULL) {
        urtCorePanic(URT_STATUS_JITTERVIOLATION);
      } else if (request->recovery.violationCode == URT_STATUS_OK) {
        request->recovery.violationCode = URT_STATUS_JITTERVIOLATION;
        request->recovery.callback(request->recovery.params);
      }
      // return violation status
      status = URT_STATUS_JITTERVIOLATION;
    }
  }
#endif /* (URT_CFG_RPC_QOS_JITTERCHECKS == true) */
#else /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) || (URT_CFG_RPC_QOS_JITTERCHECKS == true) || (URT_CFG_RPC_PROFILING == true) */
  // set optional output arguments
  if (bytes) *bytes = request->size;
  if (latency) {
    urt_osTime_t t;
    urtTimeNow(&t);
    *latency = (urt_delay_t)urtTimeDiff(&request->submissionTime, &t);
  }
#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) || (URT_CFG_RPC_QOS_JITTERCHECKS == true) || (URT_CFG_RPC_PROFILING == true) */

  // reset notification thread
  request->notification.thread = NULL;

  return status;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

/**
 * @addtogroup rpc_request_base
 * @{
 */

/**
 * @brief   Retrieve the real-time class of a base request.
 *
 * @param[in] request   The base request to retrieve the real-time class from.
 *                      Must not be NULL.
 *
 * @return  Real-time class of the base request.
 */
urt_rtclass_t urtRequestGetRtClass(const urt_baserequest_t* const request)
{
  urtDebugAssert(request != NULL);

  if (request->vmt == &_urt_hrtrequest_vmt) {
    return URT_HRT;
  } else if (request->vmt == &_urt_frtrequest_vmt) {
    return URT_FRT;
  } else if (request->vmt == &_urt_srtrequest_vmt) {
    return URT_SRT;
  } else if (request->vmt == &_urt_nrtrequest_vmt) {
    return URT_NRT;
  } else {
    urtDebugAssert(false);
    return -1;
  }
}

/**
 * @brief   Acquire a base request.
 *
 * @param[in] request   The base request to acquire.
 *                      Must not be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Request was acquired sucessfully.
 * @retval URT_STATUS_REQUEST_BADOWNER  Request is currently associated to a service.
 * @retval URT_STATUS_REQUEST_PENDING   Request could not be acquired because it is pending.
 */
urt_status_t urtRequestAcquire(urt_baserequest_t* const request)
{
  urtDebugAssert(request != NULL);

  // lock request
  urtMutexLock(&request->lock);

  // request is owned by a service?
  if (request->service != NULL) {
    urtMutexUnlock(&request->lock);
    return URT_STATUS_REQUEST_BADOWNER;
  }

  // request is pending?
  if (request->notification.thread != NULL) {
    urtMutexUnlock(&request->lock);
    return URT_STATUS_REQUEST_PENDING;
  }

  return URT_STATUS_OK;
}

/**
 * @brief   Try to acquire a base request.
 *
 * @param[in] request   The base request to be acquired.
 *                      Must not be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Request was acquired sucessfully.
 * @retval URT_STATUS_REQUEST_BADOWNER  Request is currently associated to a service.
 * @retval URT_STATUS_REQUEST_LOCKED    Request could not be acquired due to a locked mutex.
 * @retval URT_STATUS_REQUEST_PENDING   Request could not be acquired because it is pending.
 */
urt_status_t urtRequestTryAcquire(urt_baserequest_t* const request)
{
  urtDebugAssert(request != NULL);

  // request could be locked?
  if (!urtMutexTryLock(&request->lock)) {
    return URT_STATUS_REQUEST_LOCKED;
  }

  // request is owned by a service?
  if (request->service != NULL) {
    urtMutexUnlock(&request->lock);
    return URT_STATUS_REQUEST_BADOWNER;
  }

  // request is pending?
  if (request->notification.thread != NULL) {
    urtMutexUnlock(&request->lock);
    return URT_STATUS_REQUEST_PENDING;
  }

  return URT_STATUS_OK;
}

/**
 * @brief   Retrieve the age of a base request (since submission).
 *
 * @param[in] request   Request of which the age shall be retrieved.
 *
 * @return  Age of the request (since submission).
 * @retval 0  The request was not submitted yet.
 */
urt_delay_t urtRequestGetAge(const urt_baserequest_t* const request)
{
  urtDebugAssert(request != NULL);

  // copy atomically instead of locking overhead
  const urt_osTime_t submissionTime = request->submissionTime;

  if (urtTimeGet(&submissionTime) != 0) {
    urt_osTime_t t;
    urtTimeNow(&t);
    return urtTimeDiff(&submissionTime, &t);
  } else {
    return 0;
  }
}

/** @} */

/* NRT ************************************************************************/

/**
 * @addtogroup rpc_request_nrt
 * @{
 */

/**
 * @brief   Initialize an NRT request.
 *
 * @param[in] request   The NRT request to initialize.
 *                      Must not be NULL.
 * @param[in] payload   May be NULL for requests without payload.
 */
void urtNrtRequestInit(urt_nrtrequest_t* request, void* payload)
{
  urtDebugAssert(request != NULL);

  // initialize base data
  request->vmt = &_urt_nrtrequest_vmt;
  _urt_baserequest_init((urt_baserequest_t*)request, payload);

  // initialize NRT data
  /* nothing to initialize */

  return;
}

/** @} */

/* SRT ************************************************************************/

/**
 * @addtogroup rpc_request_srt
 * @{
 */

/**
 * @brief   Initialize an SRT request.
 *
 * @param[in] request   The SRT request to initialize.
 *                      Must not be NULL.
 * @param[in] payload   May be NULL for requests without payload.
 */
void urtSrtRequestInit(urt_srtrequest_t* request, void* payload)
{
  urtDebugAssert(request != NULL);

  // initialize base data
  request->vmt = &_urt_srtrequest_vmt;
  _urt_baserequest_init((urt_baserequest_t*)request, payload);

  // initialize SRT data
  /* nothing to initialize */

  return;
}

/** @} */

/* FRT ************************************************************************/

/**
 * @addtogroup rpc_request_frt
 * @{
 */

/**
 * @brief   Initialize an FRT request.
 *
 * @param[in] request   The FRT request to initialize.
 *                      Must not be NULL.
 * @param[in] payload   May be NULL for requests without payload.
 * @param[in] jitter    Maximum expected jitter.
 *                      A value of 0 indicates that jitter is of no concern.
 *                      Ignored if URT_CFG_RPC_QOS_JITTERCHECKS is false.
 */
void urtFrtRequestInit(urt_frtrequest_t* request, void* payload, urt_delay_t jitter)
{
  urtDebugAssert(request != NULL);

  // initialize base data
  request->vmt = &_urt_frtrequest_vmt;
  _urt_baserequest_init((urt_baserequest_t*)request, payload);

  // initialize FRT data
#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true)
  request->deadlineOffset = 0;
#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) */

#if (URT_CFG_RPC_QOS_JITTERCHECKS == true)
  request->maxJitter = jitter;
  request->minLatency = URT_DELAY_INFINITE;
  request->maxLatency = URT_DELAY_IMMEDIATE;
#else /* (URT_CFG_RPC_QOS_JITTERCHECKS == true) */
  (void)jitter;
#endif /* (URT_CFG_RPC_QOS_JITTERCHECKS == true) */

  return;
}

/**
 * @brief   Calculate validity of a request.
 *
 * @param[in] request   The FRT request to calculate the validity for.
 *                      Must not be NULL.
 * @param[in] latency   Latency (of the response) as argument to calculate validity.
 *
 * @return  Returns a booleas indicator whether the latency is fine.
 */
bool urtFrtRequestCalculateValidity(const urt_frtrequest_t* request, urt_delay_t latency)
{
  urtDebugAssert(request != NULL);

#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true) || (URT_CFG_RPC_QOS_JITTERCHECKS == true)

  bool validity = true;

#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true)
  if (request->deadlineOffset != 0 &&
      latency > request->deadlineOffset) {
    validity = false;
  }
#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) */

#if (URT_CFG_RPC_QOS_JITTERCHECKS == true)
  if ((request->maxJitter != 0) && (request->minLatency <= request->maxLatency) &&
      (((latency > request->minLatency) && ((latency - request->minLatency) > request->maxJitter)) ||
       ((latency < request->maxLatency) && ((request->maxLatency - latency) > request->maxJitter)))) {
    validity = false;
  }
#endif /* (URT_CFG_RPC_QOS_JITTERCHECKS == true) */

  return validity;

#else /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) || (URT_CFG_RPC_QOS_JITTERCHECKS == true) */

  (void)latency;
  return true;

#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) || (URT_CFG_RPC_QOS_JITTERCHECKS == true) */
}

/** @} */

/* HRT ************************************************************************/

/**
 * @addtogroup rpc_request_hrt
 * @{
 */

/**
 * @brief   Initialize an HRT request.
 *
 * @param[in] request   The HRT request to initialize.
 *                      Must not be NULL.
 * @param[in] payload   May be NULL for requests without payload.
 * @param[in] jitter    Maximum expected jitter.
 *                      A value of 0 indicates that jitter is of no concern.
 *                      Ignored if URT_CFG_RPC_QOS_JITTERCHECKS is false.
 */
void urtHrtRequestInit(urt_hrtrequest_t* request, void* payload, urt_delay_t jitter, urt_hrtrequest_recovery_f recoverycb, void* params)
{
  urtDebugAssert(request != NULL);

  // initialize base data
  request->vmt = &_urt_hrtrequest_vmt;
  _urt_baserequest_init((urt_baserequest_t*)request, payload);

  // initialize HRT data
#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true)
  request->deadlineOffset = 0;
#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) */

#if (URT_CFG_RPC_QOS_JITTERCHECKS == true)
  request->maxJitter = jitter;
  request->minLatency = URT_DELAY_INFINITE;
  request->maxLatency = URT_DELAY_IMMEDIATE;
#else /* (URT_CFG_RPC_QOS_JITTERCHECKS == true) */
  (void)jitter;
#endif /* (URT_CFG_RPC_QOS_JITTERCHECKS == true) */

#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true || URT_CFG_RPC_QOS_JITTERCHECKS == true)
  urtTimerInit(&request->qosTimer);
  request->recovery.callback = recoverycb;
  request->recovery.params = params;
  request->recovery.violationCode = URT_STATUS_OK;
#else /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true || URT_CFG_RPC_QOS_JITTERCHECKS == true) */
  (void)recoverycb;
  (void)params;
#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true || URT_CFG_RPC_QOS_JITTERCHECKS == true) */

  return;
}

/** @} */

#endif /* (URT_CFG_RPC_ENABLED == true) */
