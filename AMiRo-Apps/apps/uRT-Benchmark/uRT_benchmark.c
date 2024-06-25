/*
AMiRo-Apps is a collection of applications and configurations for the
Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2018..2022  Thomas Schöpping et al.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU (Lesser) General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU (Lesser) General Public License for more details.

You should have received a copy of the GNU (Lesser) General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    uRT_benchmark.c
 * @brief   A µRT benchmark application.
 */

#include "uRT_benchmark.h"

#include <string.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

#define TRIGGEREVENT                              (urtCoreGetEventMask() << 1)

#define TOPICEVENT                                (urtCoreGetEventMask() << 2)

#define REQUESTEVENT                              (urtCoreGetEventMask() << 3)

#define SERVICEEVENT                              (urtCoreGetEventMask() << 4)

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

void _ubn_recoverycb(void* params)
{
  urtCorePanic(*(urt_status_t*)params);
  return;
}

/* node setup callback ********************************************************/

urt_osEventMask_t _ubnSetup(urt_node_t* node, void* bn)
{
  urtDebugAssert(bn != NULL);

  // local constants
  urtbenchmark_node_t* const benchmark = (urtbenchmark_node_t*)bn;

  // initialize node data
  benchmark->loopEventMask = 0;
#if (CH_CFG_USE_REGISTRY == TRUE)
  chRegSetThreadName(benchmark->config->node.name);
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
  if (benchmark->config->node.trigger != NULL) {
    urtEventListenerInit(&benchmark->config->node.listener);
    urtEventRegister(benchmark->config->node.trigger, &benchmark->config->node.listener, TRIGGEREVENT, 0);
    benchmark->loopEventMask |= TRIGGEREVENT;
  }

#if (URT_CFG_PUBSUB_ENABLED == true)
  (void)node;

  // initialize publisher data
  for (size_t p = 0; p < benchmark->config->publishers.size; ++p) {
    urtPublisherInit(&benchmark->config->publishers.buffer[p].publisher,
                     urtCoreGetTopic(benchmark->config->publishers.buffer[p].topicid));
  }

  // initialize subscriber data
  for (size_t s = 0; s < benchmark->config->subscribers.size; ++s) {
    switch (benchmark->config->subscribers.buffer[s].rtclass) {
      case URT_NRT:
        urtNrtSubscriberInit(&benchmark->config->subscribers.buffer[s].nrtsubscriber.subscriber);
        urtNrtSubscriberSubscribe(&benchmark->config->subscribers.buffer[s].nrtsubscriber.subscriber,
                                  urtCoreGetTopic(benchmark->config->subscribers.buffer[s].topicid),
                                  TOPICEVENT);
        break;
      case URT_SRT:
        urtSrtSubscriberInit(&benchmark->config->subscribers.buffer[s].srtsubscriber.subscriber);
        urtSrtSubscriberSubscribe(&benchmark->config->subscribers.buffer[s].srtsubscriber.subscriber,
                                  urtCoreGetTopic(benchmark->config->subscribers.buffer[s].topicid),
                                  TOPICEVENT,
                                  benchmark->config->subscribers.buffer[s].srtsubscriber.usefulnesscb,
                                  &benchmark->config->subscribers.buffer[s].srtsubscriber.params);
        break;
      case URT_FRT:
        urtFrtSubscriberInit(&benchmark->config->subscribers.buffer[s].frtsubscriber.subscriber);
        urtFrtSubscriberSubscribe(&benchmark->config->subscribers.buffer[s].frtsubscriber.subscriber,
                                  urtCoreGetTopic(benchmark->config->subscribers.buffer[s].topicid),
                                  TOPICEVENT,
                                  benchmark->config->subscribers.buffer[s].frtsubscriber.deadline,
                                  benchmark->config->subscribers.buffer[s].frtsubscriber.jitter);
        break;
      case URT_HRT:
        urtHrtSubscriberInit(&benchmark->config->subscribers.buffer[s].hrtsubscriber.subscriber);
        urtHrtSubscriberSubscribe(&benchmark->config->subscribers.buffer[s].hrtsubscriber.subscriber,
                                  urtCoreGetTopic(benchmark->config->subscribers.buffer[s].topicid),
                                  TOPICEVENT,
                                  benchmark->config->subscribers.buffer[s].hrtsubscriber.deadline,
                                  benchmark->config->subscribers.buffer[s].hrtsubscriber.jitter,
                                  benchmark->config->subscribers.buffer[s].hrtsubscriber.rate,
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS == true)
                                  _ubn_recoverycb,
                                  &benchmark->config->subscribers.buffer[s].hrtsubscriber.subscriber.recovery.violationCode);
#else /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */
                                  NULL,
                                  NULL);
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */
        break;
    }
    benchmark->loopEventMask |= TOPICEVENT;
  }
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true)
  // initialize service data
  for (size_t s = 0; s < benchmark->config->services.size; ++s) {
    urtServiceInit(&benchmark->config->services.buffer[s].service,
                   benchmark->config->services.buffer[s].serviceid,
                   ((urt_node_t*)node)->thread,
                   REQUESTEVENT);
    benchmark->loopEventMask |= REQUESTEVENT;
  }

  // initialize request data
  for (size_t r = 0; r < benchmark->config->requests.size; ++r) {
    switch (benchmark->config->requests.buffer[r].rtclass) {
      case URT_NRT:
        urtNrtRequestInit(&benchmark->config->requests.buffer[r].nrtrequest.request,
                          benchmark->config->requests.buffer[r].payload.buffer);
        break;
      case URT_SRT:
        urtSrtRequestInit(&benchmark->config->requests.buffer[r].srtrequest.request,
                          benchmark->config->requests.buffer[r].payload.buffer);
        break;
      case URT_FRT:
        urtFrtRequestInit(&benchmark->config->requests.buffer[r].frtrequest.request,
                          benchmark->config->requests.buffer[r].payload.buffer,
                          benchmark->config->requests.buffer[r].frtrequest.jitter);
        break;
      case URT_HRT:
        urtHrtRequestInit(&benchmark->config->requests.buffer[r].hrtrequest.request,
                          benchmark->config->requests.buffer[r].payload.buffer,
                          benchmark->config->requests.buffer[r].hrtrequest.jitter,
#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true) || (URT_CFG_RPC_QOS_JITTERCHECKS == true)
                          _ubn_recoverycb,
                          &benchmark->config->requests.buffer[r].hrtrequest.request.recovery.violationCode);
#else /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) || (URT_CFG_RPC_QOS_JITTERCHECKS == true) */
                                  NULL,
                                  NULL);
#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) || (URT_CFG_RPC_QOS_JITTERCHECKS == true) */
        break;
    }
    // retrieve the service
    while (true) {
      benchmark->config->requests.buffer[r].service = urtCoreGetService(benchmark->config->requests.buffer[r].serviceid);
      if (benchmark->config->requests.buffer[r].service != NULL) {
        break;
      } else {
        urtThreadMSleep(1);
      }
    }
    benchmark->loopEventMask |= SERVICEEVENT;
  }
#endif /* (URT_CFG_RPC_ENABLED == true) */

  return benchmark->loopEventMask;
}

/* node loop callbacks ********************************************************/

urt_osEventMask_t _ubnLoop_node(urt_node_t* node, urt_osEventMask_t events, void* bn)
{
  urtDebugAssert(bn != NULL);
  (void)node;

  // local variables
  urtbenchmark_node_t* const benchmark = (urtbenchmark_node_t*)bn;

  // track latency
  if (events & TRIGGEREVENT) {
    urtDebugAssert(benchmark->config->node.time != NULL);
    urt_osTime_t t;
    urtTimeNow(&t);
    *benchmark->config->node.latencybuffer = urtTimeDiff(benchmark->config->node.time, &t);
    ++benchmark->config->node.latencybuffer;
  }

  return benchmark->loopEventMask;
}

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)

urt_status_t _ubn_publish(urtbenchmark_configdata_publisher_t* const publisher)
{
  urtDebugAssert(publisher != NULL);

  if (publisher->time == NULL) {
    urt_osTime_t t;
    urtTimeNow(&t);
    return urtPublisherPublish(&publisher->publisher,
                               publisher->payload.buffer,
                               publisher->payload.size,
                               &t,
                               publisher->policy);
  } else {
    return urtPublisherPublish(&publisher->publisher,
                               publisher->payload.buffer,
                               publisher->payload.size,
                               publisher->time,
                               publisher->policy);
  }
}

void _ubn_fetch(urtbenchmark_node_t* const benchmark, urtbenchmark_configdata_subscriber_t* const subscriber, void(*action)(urtbenchmark_node_t* const, urtbenchmark_configdata_subscriber_t* const, urt_osTime_t* const))
{
  urtDebugAssert(benchmark != NULL);
  urtDebugAssert(subscriber != NULL);
  urtDebugAssert(action != NULL);

  // local variables
  urt_osTime_t origintime;

  // fetch the next message
  while (urtSubscriberFetchNext(&subscriber->basesubscriber,
                                subscriber->payload.buffer,
                                &subscriber->payload.size,
                                &origintime,
                                subscriber->latencybuffer)
         == URT_STATUS_OK) {
    if (action != NULL) {
      action(benchmark, subscriber, &origintime);
    }
  }

  return;
}

void _ubnfetchcb(urtbenchmark_node_t* const benchmark, urtbenchmark_configdata_subscriber_t* const subscriber, urt_osTime_t* const origintime)
{
  (void)benchmark;
  urtDebugAssert(subscriber != NULL);
  (void)origintime;

  // proceed in latency buffer
  if (subscriber->latencybuffer != NULL) {
    ++subscriber->latencybuffer;
  }
  return;
}

void _ubnfetchcb_hops(urtbenchmark_node_t* const benchmark, urtbenchmark_configdata_subscriber_t* const subscriber, urt_osTime_t* const origintime)
{
  urtDebugAssert(benchmark != NULL);
  urtDebugAssert(subscriber != NULL);
  urtDebugAssert(origintime != NULL);

  // propagate message further if there are üublishers
  if (benchmark->config->publishers.size > 0) {
    urtDebugAssert(benchmark->config->publishers.size == 1);
    benchmark->config->publishers.buffer[0].time = origintime;
    _ubn_publish(&benchmark->config->publishers.buffer[0]);
  }
  // proceed in latency buffer
  if (subscriber->latencybuffer != NULL) {
    ++subscriber->latencybuffer;
  }
  return;
}

urt_osEventMask_t _ubnLoop_pubsub_hops(urt_node_t* node, urt_osEventMask_t events, void* bn)
{
  urtDebugAssert(bn != NULL);
  (void)node;

  // local constants
  urtbenchmark_node_t* const benchmark = (urtbenchmark_node_t*)bn;

  // on a trigger event
  if (events & TRIGGEREVENT) {
    urtDebugAssert(benchmark->config->publishers.size == 1);
    // emit message
    _ubn_publish(&benchmark->config->publishers.buffer[0]);
  }

  // on a topic event
  if (events & TOPICEVENT) {
    urtDebugAssert(benchmark->config->subscribers.size == 1);
    // fetch all pending messages
    _ubn_fetch(benchmark, &benchmark->config->subscribers.buffer[0], _ubnfetchcb_hops);
  }

  return benchmark->loopEventMask;
}

urt_osEventMask_t _ubnLoop_pubsub_scaling(urt_node_t* node, urt_osEventMask_t events, void* bn)
{
  urtDebugAssert(bn != NULL);
  (void)node;

  // local constants
  urtbenchmark_node_t* const benchmark = (urtbenchmark_node_t*)bn;

  // on a trigger event
  if (events & TRIGGEREVENT) {
    // emit messages with all publishers
    for (size_t p = 0; p < benchmark->config->publishers.size; ++p) {
      _ubn_publish(&benchmark->config->publishers.buffer[p]);
    }
  }

  // on a topic event
  if (events & TOPICEVENT) {
    // fetch all pending messages of all subscribers
    for (size_t s = 0; s < benchmark->config->subscribers.size; ++s) {
      _ubn_fetch(benchmark, &benchmark->config->subscribers.buffer[s], _ubnfetchcb);
    }
  }

  return benchmark->loopEventMask;
}

urt_osEventMask_t _ubnLoop_pubsub_qos(urt_node_t* node, urt_osEventMask_t events, void* bn)
{
  urtDebugAssert(bn != NULL);
  (void)node;

  // local constants
  urtbenchmark_node_t* const benchmark = (urtbenchmark_node_t*)bn;

  // on a trigger event
  if (events & TRIGGEREVENT) {
    urtDebugAssert(benchmark->config->publishers.size == 1);
    // emit message
    _ubn_publish(&benchmark->config->publishers.buffer[0]);
  }

  // on a topic event
  if (events & TOPICEVENT) {
    urtDebugAssert(benchmark->config->subscribers.size == 1);
    // fetch all pending messages
    _ubn_fetch(benchmark, &benchmark->config->subscribers.buffer[0], _ubnfetchcb);
  }

  return benchmark->loopEventMask;
}

#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)

void _ubn_submit(urtbenchmark_configdata_request_t* const request)
{
  urtDebugAssert(request != NULL);
  urtDebugAssert(request->service != NULL);

  /* for each request type:
   * 1) acquired request
   * 2) set payload
   * 3) submit request
   */
  if (urtRequestAcquire(&request->baserequest) != URT_STATUS_OK) {
    urtRequestRetrieve(&request->baserequest, URT_REQUEST_RETRIEVE_ENFORCING, NULL, NULL);
  }
  memset(urtRequestGetPayload(&request->baserequest), request->payload.size % 0x0100, request->payload.size);
  switch (request->rtclass) {
    case URT_NRT:
    case URT_SRT:
      urtRequestSubmit(&request->baserequest, request->service, request->payload.size, SERVICEEVENT, 0);
      break;
    case URT_FRT:
      urtRequestSubmit(&request->baserequest, request->service, request->payload.size, SERVICEEVENT, request->frtrequest.deadline);
      break;
    case URT_HRT:
      urtRequestSubmit(&request->baserequest, request->service, request->payload.size, SERVICEEVENT, request->hrtrequest.deadline);
      break;
  }

  return;
}

bool _ubn_dispatch(urtbenchmark_node_t* const benchmark, urtbenchmark_configdata_service_t* const service, urt_service_dispatched_t* const dispatched, void(*action)(urtbenchmark_node_t* const, urtbenchmark_configdata_service_t* const, urt_service_dispatched_t* const, size_t, bool))
{
  urtDebugAssert(benchmark != NULL);
  urtDebugAssert(service != NULL);
  urtDebugAssert(dispatched != NULL);
  urtDebugAssert(action != NULL);

  // local variables
  size_t size;
  bool noreturn;

  // try to dispatch a request
  if (urtServiceDispatch(&service->service, dispatched, service->payload.buffer, &size, &noreturn)) {
    // process request
    action(benchmark, service, dispatched, size, noreturn);
    return true;
  } else {
    return false;
  }
}

void _ubn_retrieve(urtbenchmark_node_t* const benchmark, urtbenchmark_configdata_request_t* const request, void* params, void(*action)(urtbenchmark_node_t* const, urtbenchmark_configdata_request_t* const, void*))
{
  urtDebugAssert(benchmark != NULL);
  urtDebugAssert(request != NULL);
  urtDebugAssert(action != NULL);

  /*
   * 1) check whether request is pending
   * 2) try to retrieve the request
   * 3) check whether the request could be retrieved
   * 4) release request
   */
  size_t size;
  if (urtRequestIsPending(&request->baserequest)) {
    urtRequestRetrieve(&request->baserequest, request->policy, &size, request->latencybuffer);
    if (!urtRequestIsPending(&request->baserequest)) {
      action(benchmark, request, params);
      urtRequestRelease(&request->baserequest);
    }
  }

  return;
}

void _ubndispatchcb(urtbenchmark_node_t* const benchmark, urtbenchmark_configdata_service_t* const service, urt_service_dispatched_t* const dispatched, size_t payloadsize, bool noreturn)
{
  urtDebugAssert(benchmark != NULL);
  urtDebugAssert(service != NULL);
  urtDebugAssert(dispatched != NULL && urtServiceDispatchedIsValid(dispatched));

  // track dispatch latency
  if (service->latencybuffer != NULL) {
    urt_osTime_t t;
    urtTimeNow(&t);
    *service->latencybuffer = urtTimeDiff(&dispatched->submissionTime, &t);
    ++service->latencybuffer;
  }
  // acquire request and respond
  if (!noreturn && urtServiceAcquireRequest(&service->service, dispatched) == URT_STATUS_OK) {
    memcpy(urtRequestGetPayload(dispatched->request), service->payload.buffer, payloadsize);
    urtServiceRespond(dispatched, payloadsize);
  }

  return;
}

void _ubndispatchcb_hops(urtbenchmark_node_t* const benchmark, urtbenchmark_configdata_service_t* const service, urt_service_dispatched_t* const dispatched, size_t payloadsize, bool noreturn)
{
  urtDebugAssert(benchmark != NULL);
  urtDebugAssert(service != NULL);
  urtDebugAssert(dispatched != NULL && urtServiceDispatchedIsValid(dispatched));
  (void)payloadsize;
  (void)noreturn;

  // propagate request further
  if (benchmark->config->requests.buffer != NULL) {
    urtDebugAssert(benchmark->config->requests.size == 1);
    _ubn_submit(&benchmark->config->requests.buffer[0]);
  }
  // acquire request and respond
  else {
    if (urtServiceAcquireRequest(&service->service, dispatched) == URT_STATUS_OK) {
      urtServiceRespond(dispatched, 0);
    }
  }

  return;
}

void _ubndispatchcb_qos(urtbenchmark_node_t* const benchmark, urtbenchmark_configdata_service_t* const service, urt_service_dispatched_t* const dispatched, size_t payloadsize, bool noreturn)
{
  urtDebugAssert(benchmark != NULL);
  urtDebugAssert(service != NULL && service->delay != NULL);
  urtDebugAssert(dispatched != NULL && urtServiceDispatchedIsValid(dispatched));

  // track dispatch latency
  if (service->latencybuffer != NULL) {
    urt_osTime_t t;
    urtTimeNow(&t);
    *service->latencybuffer = urtTimeDiff(&dispatched->submissionTime, &t);
    ++service->latencybuffer;
  }
  // induce artificial delay
  urtThreadUSleep(*service->delay);
  // acquire request and respond
  if (!noreturn && urtServiceAcquireRequest(&service->service, dispatched) == URT_STATUS_OK) {
    memcpy(urtRequestGetPayload(dispatched->request), service->payload.buffer, payloadsize);
    urtServiceRespond(dispatched, payloadsize);
  }

  return;
}

void _ubnretrievecb(urtbenchmark_node_t* const benchmark, urtbenchmark_configdata_request_t* const request, void* params)
{
  urtDebugAssert(benchmark != NULL);
  urtDebugAssert(request != NULL);
  (void)params;

  // proceed in retrieve latency buffer
  if (request->latencybuffer != NULL) {
    ++request->latencybuffer;
  }

  return;
}

void _ubnretrievecb_hops(urtbenchmark_node_t* const benchmark, urtbenchmark_configdata_request_t* const request, void* dispatched)
{
  urtDebugAssert(benchmark != NULL);
  urtDebugAssert(request != NULL);
  urtDebugAssert(dispatched != NULL && urtServiceDispatchedIsValid((urt_service_dispatched_t*)dispatched));

  // respond previous request
  if (benchmark->config->services.buffer != NULL) {
    urtDebugAssert(benchmark->config->services.size == 1);
    // try to acquire dispatched request
    if (urtServiceAcquireRequest(&benchmark->config->services.buffer[0].service, (urt_service_dispatched_t*)dispatched) == URT_STATUS_OK) {
      // respond request
      urtServiceRespond((urt_service_dispatched_t*)dispatched, 0);
    }
  }
  // proceed in retrieve latency buffer
  if (request->latencybuffer != NULL) {
    ++request->latencybuffer;
  }

  return;
}

urt_osEventMask_t _ubnLoop_rpc_hops(urt_node_t* node, urt_osEventMask_t events, void* bn)
{
  urtDebugAssert(bn != NULL);
  (void)node;

  // local constants
  urtbenchmark_node_t* const benchmark = (urtbenchmark_node_t*)bn;

  // on a trigger event
  if (events & TRIGGEREVENT) {
    urtDebugAssert(benchmark->config->requests.size == 1);
    // submit a request
    _ubn_submit(&benchmark->config->requests.buffer[0]);
  }

  // on a request event
  if (events & REQUESTEVENT) {
    urtDebugAssert(benchmark->config->services.size == 1);
    // dispatch and process request
    _ubn_dispatch(benchmark, &benchmark->config->services.buffer[0], &benchmark->config->services.buffer[0].dispatched, _ubndispatchcb_hops);
  }

  // on a service event
  if (events & SERVICEEVENT) {
    urtDebugAssert(benchmark->config->requests.size == 1);
    // retrieve request
    _ubn_retrieve(benchmark, &benchmark->config->requests.buffer[0], &benchmark->config->services.buffer[0].dispatched, _ubnretrievecb_hops);
  }

  return benchmark->loopEventMask;
}

urt_osEventMask_t _ubnLoop_rpc_scaling(urt_node_t* node, urt_osEventMask_t events, void* bn)
{
  urtDebugAssert(bn != NULL);
  (void)node;

  // local constants
  urtbenchmark_node_t* const benchmark = (urtbenchmark_node_t*)bn;

  // on a trigger event
  if (events & TRIGGEREVENT) {
    // submit all requests
    for (size_t r = 0; r < benchmark->config->requests.size; ++r) {
      urtDebugAssert(benchmark->config->requests.buffer[r].service != NULL);
      _ubn_submit(&benchmark->config->requests.buffer[r]);
    }
  }

  // on a request event
  if (events & REQUESTEVENT) {
    // dispatch and process all requests for each service
    for (size_t s = 0; s < benchmark->config->services.size; ++s) {
      while (_ubn_dispatch(benchmark, &benchmark->config->services.buffer[s], &benchmark->config->services.buffer[s].dispatched, _ubndispatchcb)) {
        continue;
      }
    }
  }

  // on a service event
  if (events & SERVICEEVENT) {
    // retrieve all requests
    for (size_t r = 0; r < benchmark->config->requests.size; ++r) {
      _ubn_retrieve(benchmark, &benchmark->config->requests.buffer[r], NULL, _ubnretrievecb);
    }
  }

  return benchmark->loopEventMask;
}

urt_osEventMask_t _ubnLoop_rpc_qos(urt_node_t* node, urt_osEventMask_t events, void* bn)
{
  urtDebugAssert(bn != NULL);
  (void)node;

  // local constants
  urtbenchmark_node_t* const benchmark = (urtbenchmark_node_t*)bn;

  // on a trigger event
  if (events & TRIGGEREVENT) {
    urtDebugAssert(benchmark->config->requests.size == 1);
    // submit request
    _ubn_submit(&benchmark->config->requests.buffer[0]);
  }

  // on a request event
  if (events & REQUESTEVENT) {
    urtDebugAssert(benchmark->config->services.size == 1);
    // dispatch and process all requests
    while (_ubn_dispatch(benchmark, &benchmark->config->services.buffer[0], &benchmark->config->services.buffer[0].dispatched, _ubndispatchcb_qos)) {
      continue;
    }
  }

  // on a service event
  if (events & SERVICEEVENT) {
    urtDebugAssert(benchmark->config->requests.size == 1);
    // retrieve request
    _ubn_retrieve(benchmark, &benchmark->config->requests.buffer[0], NULL, _ubnretrievecb);
  }

  return benchmark->loopEventMask;
}

#endif /* (URT_CFG_RPC_ENABLED == true) */

/* node shutdown callback *****************************************************/

void _ubnShutdown(urt_node_t* node, urt_status_t reason, void* bn)
{
  urtDebugAssert(bn != NULL);
  (void)node;
  (void)reason;

  // local constants
  urtbenchmark_node_t* const benchmark = (urtbenchmark_node_t*)bn;

  // shutdown node
  if (benchmark->config->node.trigger != NULL) {
    urtEventUnregister(benchmark->config->node.trigger, &benchmark->config->node.listener);
  }

#if (URT_CFG_PUBSUB_ENABLED == true)
  // shutdown publishers: nothing to do

  // shutdown subscribers
  for (size_t s = 0; s < benchmark->config->subscribers.size; ++s) {
    urtSubscriberUnsubscribe(&benchmark->config->subscribers.buffer[s].basesubscriber);
  }
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true)
  // shutdown requests: nothing to do

  // shutdown services: nothing to do
#endif /* (URT_CFG_RPC_ENABLED == true) */

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_urtbenchmark
 * @{
 */

void ubnInit(urtbenchmark_node_t* bn, urt_osThreadPrio_t prio, urtbenchmark_config_t* config)
{
  urtDebugAssert(bn != NULL);
  urtDebugAssert(config != NULL);

  // initialize the node
  switch (config->node.type) {
    case URTBENCHMARK_NODE:
      urtNodeInit(&bn->node, (urt_osThread_t*)bn->thread, sizeof(bn->thread), prio,
                  _ubnSetup, bn,
                  _ubnLoop_node, bn,
                  _ubnShutdown, bn);
      break;
#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
    case URTBENCHMARK_PUBSUB_HOPS:
      urtNodeInit(&bn->node, (urt_osThread_t*)bn->thread, sizeof(bn->thread), prio,
                  _ubnSetup, bn,
                  _ubnLoop_pubsub_hops, bn,
                  _ubnShutdown, bn);
      break;
    case URTBENCHMARK_PUBSUB_SCALING:
      urtNodeInit(&bn->node, (urt_osThread_t*)bn->thread, sizeof(bn->thread), prio,
                  _ubnSetup, bn,
                  _ubnLoop_pubsub_scaling, bn,
                  _ubnShutdown, bn);
      break;
    case URTBENCHMARK_PUBSUB_QOS:
      urtNodeInit(&bn->node, (urt_osThread_t*)bn->thread, sizeof(bn->thread), prio,
                  _ubnSetup, bn,
                  _ubnLoop_pubsub_qos, bn,
                  _ubnShutdown, bn);
      break;
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
    case URTBENCHMARK_RPC_HOPS:
      urtNodeInit(&bn->node, (urt_osThread_t*)bn->thread, sizeof(bn->thread), prio,
                  _ubnSetup, bn,
                  _ubnLoop_rpc_hops, bn,
                  _ubnShutdown, bn);
      break;
    case URTBENCHMARK_RPC_SCALING:
      urtNodeInit(&bn->node, (urt_osThread_t*)bn->thread, sizeof(bn->thread), prio,
                  _ubnSetup, bn,
                  _ubnLoop_rpc_scaling, bn,
                  _ubnShutdown, bn);
      break;
    case URTBENCHMARK_RPC_QOS:
      urtNodeInit(&bn->node, (urt_osThread_t*)bn->thread, sizeof(bn->thread), prio,
                  _ubnSetup, bn,
                  _ubnLoop_rpc_qos, bn,
                  _ubnShutdown, bn);
      break;
#endif /* (URT_CFG_RPC_ENABLED == true) */
    default:
      urtDebugAssert(false);
      break;
  }

  // set the configuration
  bn->config = config;

  return;
}

void ubnConfigInit(urtbenchmark_config_t* config)
{
  // reset node data
#if (CH_CFG_USE_REGISTRY == TRUE)
    memset(config->node.name, '\0', sizeof(config->node.name));
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    config->node.trigger = NULL;
    config->node.time = NULL;
    config->node.latencybuffer = NULL;
    config->node.type = URTBENCHMARK_UNDEFINED;

#if (URT_CFG_PUBSUB_ENABLED == true)
    // reset publisher data
    config->publishers.buffer = NULL;
    config->publishers.size = 0;

    // reset subscriber data
    config->subscribers.buffer = NULL;
    config->subscribers.size = 0;
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true)
    // reset request data
    config->requests.buffer = NULL;
    config->requests.size = 0;

    // reset service data
    config->services.buffer = NULL;
    config->services.size = 0;
#endif /* (URT_CFG_RPC_ENABLED == true) */

  return;
}

/** @} */
