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
 * @file    urt_service.c
 * @brief   Service code.
 */

#include <urt.h>

#if (URT_CFG_RPC_ENABLED == true || defined(__DOXYGEN__))

#include <string.h>

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

/**
 * @brief   Check whether the request to be acquired is valid.
 *
 * @param[in] service     Service to acquire the request.
 *                        Must not be NULL.
 * @param[in] dispatched  Dispatched request to be acquired.
 *                        Must have been locked before.
 *                        Must not be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Request was acquired successfully.
 * @retval URT_STATUS_REQUEST_BADOWNER  Service has lost ownership of the request.
 * @retval URT_STATUS_REQUEST_OBSOLETE  Request has been aborted and re-submitted.
 */
urt_status_t _urt_service_acquireRequestCheck(urt_service_t const* const service, urt_service_dispatched_t const* const dispatched)
{
  // this service is still owner of the request and the request is still valid?
  if (dispatched->request->service == service &&
      urtTimeGet(&dispatched->request->submissionTime) == urtTimeGet(&dispatched->submissionTime)) {
    return URT_STATUS_OK;
  } else {
    urtMutexUnlock(&dispatched->request->lock);
    return (dispatched->request->service != service) ? URT_STATUS_REQUEST_BADOWNER : URT_STATUS_REQUEST_OBSOLETE;
  }
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

/**
 * @addtogroup rpc_service
 * @{
 */

/**
 * @brief   Initalize a service.
 *
 * @param[in] service   The service to initialize.
 *                      Must not be NULL.
 * @param[in] id        Unique, numeric identifier of the service.
 * @param[in] thread    Pointer to the thread providing this service.
 *                      Must not be NULL.
 * @param[in] mask      Mask of event where the service registers to.
 *                      Must not be 0.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                  Service was successfully added to the µRT core.
 * @retval URT_STATUS_SERVICE_DUPLICATE   Another service with the same identifier already exists.
 */
urt_status_t urtServiceInit(urt_service_t* service, urt_serviceid_t id, urt_osThread_t* thread, urt_osEventMask_t mask)
{
  urtDebugAssert(service != NULL);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
  urtDebugAssert(id <= URT_RPC_SERVICEID_MAXVAL);
#pragma GCC diagnostic pop
  urtDebugAssert(thread != NULL);
  urtDebugAssert(mask != 0);

  service->next = NULL;
  service->id = id;
  urtMutexInit(&service->lock);
  service->queue.front = NULL;
  service->queue.hrtBack = NULL;
  service->queue.fsrtBack = NULL;
  service->queue.back = NULL;
  service->notification.thread = thread;
  service->notification.mask = mask;

#if (URT_CFG_RPC_PROFILING == true) || defined (__DOXYGEN__)
  service->profiling.numCalls = 0;
  service->profiling.numOwnershipLost = 0;
#endif /* (URT_CFG_RPC_PROFILING == true) */

  // add service to core
  const urt_status_t status = urtCoreAddService(service);
  if (status != URT_STATUS_OK) {
    service->notification.thread = NULL;
    service->notification.mask = 0;
  }
  return status;
}

/**
 * @brief   Dispatch a request for a given service.
 *
 * @param[in] service         The service to dispatch a request for.
 *                            Must not be NULL.
 * @param[in,out] dispatched  Pointer where to store information about the dispatched request.
 *                            If no request is dispatched, that data will be set to an invalid state.
 *                            Must not be NULL.
 * @param[in,out] payload     Pointer where to store the payload (if any).
 *                            May be NULL if the service expects no payload.
 * @param[in,out] bytes       Optional return argument to store the size of the payload.
 *                            May be NULL.
 * @param[in,out] noreturn    Optional return argument to indicate whether the dispatched request is 'fire-and-forget'.
 *                            May be NULL.
 *
 * @return  Indicator whether a request has been dispatched.
 * @retval true   A request has been dispatched.
 * @retval flase  No request has been dispatched.
 */
bool urtServiceDispatch(urt_service_t* service, urt_service_dispatched_t* dispatched, void* payload, size_t* bytes, bool* noreturn)
{
  urtDebugAssert(service != NULL);
  urtDebugAssert(dispatched != NULL);

  // lock service
  urtMutexLock(&service->lock);

  // request list empty?
  if (service->queue.front == NULL) {
    urtMutexUnlock(&service->lock);
    // set dispatched data to invalid state
    dispatched->request = NULL;
    return false;
  }

  // dispatch request
  dispatched->request = service->queue.front;
  urtMutexLock(&dispatched->request->lock);
  service->queue.front = dispatched->request->next;
  dispatched->request->next = NULL;

  // this was the hindmost HRT request?
  if (dispatched->request == (urt_baserequest_t*)service->queue.hrtBack) {
    // reset pointer
    service->queue.hrtBack = NULL;
  }
  // this was the hindmost SRT/FRT request?
  else if (dispatched->request == service->queue.fsrtBack) {
    // reset pointer
    service->queue.fsrtBack = NULL;
  }

  // this was the hindmost request in the queue?
  if (dispatched->request == service->queue.back) {
    // reset pointer
    service->queue.back = NULL;
  } else {
    service->queue.front->prev = NULL;
  }

  // copy the payload
  urtDebugAssert(payload == NULL || (payload != NULL && dispatched->request->payload != NULL));
  if (payload) memcpy(payload, dispatched->request->payload, dispatched->request->size);

  // store relevant meta information
  dispatched->submissionTime = dispatched->request->submissionTime;

  // set optional return arguments
  if (bytes) *bytes = dispatched->request->size;
  if (noreturn) *noreturn = (dispatched->request->notification.mask == 0);

  // unlock request
  urtMutexUnlock(&dispatched->request->lock);
  // unlock service
  urtMutexUnlock(&service->lock);

  return true;
}

/**
 * @brief   Acquire a request for a given service to reply.
 *
 * @param[in] service     The service that shall acquire the request.
 *                        Must not be NULL.
 * @param[in] dispatched  The request to be acquired.
 *                        Must not be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Request was acquired successfully.
 * @retval URT_STATUS_REQUEST_BADOWNER  Service has lost ownership of the request.
 * @retval URT_STATUS_REQUEST_OBSOLETE  Request has been aborted and re-submitted.
 */
urt_status_t urtServiceAcquireRequest(urt_service_t* service, const urt_service_dispatched_t* dispatched)
{
  urtDebugAssert(service != NULL);
  urtDebugAssert(dispatched != NULL && dispatched->request != NULL);

  // lock request
  urtMutexLock(&dispatched->request->lock);

  return _urt_service_acquireRequestCheck(service, dispatched);
}

/**
 * @brief   Try to acquire a request for a given service to reply.
 *
 * @param[in] service     The service that shall acquire the request.
 *                        Must not be NULL.
 * @param[in] dispatched  The request to be acquired.
 *                        Must not be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                Request was acquired sucessfully.
 * @retval URT_STATUS_REQUEST_BADOWNER  Service has lost ownership of the request.
 * @retval URT_STATUS_REQUEST_LOCKED    Request could not be acquired due to a locked mutex.
 * @retval URT_STATUS_REQUEST_OBSOLETE  Request has been aborted and re-submitted.
 */
urt_status_t urtServiceTryAcquireRequest(urt_service_t* service, const urt_service_dispatched_t* dispatched)
{
  urtDebugAssert(service != NULL);
  urtDebugAssert(dispatched != NULL && dispatched->request != NULL);

  // locking request failed?
  if (!urtMutexTryLock(&dispatched->request->lock)) {
    return URT_STATUS_REQUEST_LOCKED;
  }

  return _urt_service_acquireRequestCheck(service, dispatched);
}

/**
 * @brief   Respond to a given request
 *
 * @param[in] dispatched  Request to respond to.
 *                        Must not be NULL.
 *                        Must have been acquired before.
 * @param[in] bytes       Payload size of return data.
 *                        Can be 0.
 *
 * @return  Always returns URT_STATUS_OK.
 */
urt_status_t urtServiceRespond(urt_service_dispatched_t* dispatched, size_t bytes)
{
  urtDebugAssert(dispatched != NULL && dispatched->request != NULL);

  // reset associated service
  dispatched->request->service = NULL;

  // set payload size of return data
  dispatched->request->size = bytes;

  // notify calling thread
  if (dispatched->request->notification.mask != 0) {
    urtEventSignal(dispatched->request->notification.thread, dispatched->request->notification.mask);
  }

  // unlock request
  urtMutexUnlock(&dispatched->request->lock);

  // invalidate dispatched and return
  dispatched->request = NULL;
  return URT_STATUS_OK;
}

/**
 * @brief   Retrieve the length of the queue of pending requests.
 *
 * @param[in]  service  Service to retrieve the queue length of.
 * @param[out] hrt      Optional pointer to return the number of HRT requests.
 * @param[out] fsrt     Optional pointer to return the number of FRT/SRT requests.
 * @param[out] nrt      Optional pointer to return the number of NRT requests.
 *
 * @return  Overall length of the request queue.
 */
size_t urtServiceQueueLength(urt_service_t* service, size_t* hrt, size_t* fsrt, size_t* nrt)
{
  urtDebugAssert(service != NULL);

  // local variables
  size_t length = 0;

  // reset optional arguments
  if (hrt != NULL) {
    *hrt = 0;
  }
  if (fsrt != NULL) {
    *fsrt = 0;
  }
  if (nrt != NULL) {
    *nrt = 0;
  }

  // lock service
  urtMutexLock(&service->lock);

  // count the requests in the queue
  for (urt_baserequest_t* req = service->queue.front; req != NULL; req = req->next) {
    ++length;
    // count HRT requests
    if (req == (urt_baserequest_t*)service->queue.hrtBack) {
      if (hrt != NULL) {
        *hrt = length;
      }
      if (fsrt != NULL) {
        *fsrt = length;
      }
      if (nrt != NULL) {
        *nrt = length;
      }
    }
    // count FRT/SRT requests
    else if (req == service->queue.fsrtBack) {
      if (fsrt != NULL) {
        *fsrt = length - *fsrt;
      }
      if (nrt != NULL) {
        *nrt = length;
      }
    }
  }

  // unlock service
  urtMutexUnlock(&service->lock);

  // count NRT requests
  if (nrt != NULL) {
    *nrt = length - *nrt;
  }

  return length;
}

/** @} */

#endif /* (URT_CFG_RPC_ENABLED == true) */
