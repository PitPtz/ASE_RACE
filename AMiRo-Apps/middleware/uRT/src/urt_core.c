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
 * @file    urt_core.c
 * @brief   Core code.
 */

#include <urt.h>

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

/**
 * @brief   Pointer to the first node.
 * @details Nodes are stored as singly linked list.
 *          This pointer my be NULL if there are no nodes.
 */
urt_node_t* _urt_core_nodes;

/**
 * @brief   Event source for system-wide middleware related events.
 */
urt_osEventSource_t _urt_core_evtSource;

/**
 * @brief   Mutex lock for the µRT core data.
 */
urt_osMutex_t _urt_core_lock;

/**
 * @brief   Status value holding the current µRT status.
 */
urt_status_t _urt_core_status;

/**
 * @brief   Pointer to an external synchronization object.
 * @details Is set during initialization.
 *          May be NULL.
 */
urt_sync_t* _urt_core_sync;

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
/**
 * @brief   Pointer to the first topic.
 * @details Topics are stored as ordered, singly linked list.
 *          Topics are sorted by their individual identifier in ascending order.
 *          This pointer may be NULL if there are no topics.
 */
urt_topic_t* _urt_core_topics;
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
/**
 * @brief   Pointer to the first service.
 * @details Services are stored as ordered, singly linked list.
 *          Services are sorterd by their individual identifier in ascending order.
 *          This pointer may be NULL if there are no services.
 */
urt_service_t*  _urt_core_services;
#endif /* (URT_CFG_RPC_ENABLED == true) */

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

/**
 * @addtogroup kernel_core
 * @{
 */

/**
 * @brief   Initialize the Core.
 *
 * @param[in] sync  Pointer to an external synchronization object.
 *                  May be NULL.
 */
void urtCoreInit(urt_sync_t* sync)
{
  // initialize data
  _urt_core_nodes = NULL;
  urtEventSourceInit(&_urt_core_evtSource);
  urtMutexInit(&_urt_core_lock);
  _urt_core_status = URT_STATUS_OK;
  _urt_core_sync = sync;
#if (URT_CFG_PUBSUB_ENABLED == true)
  _urt_core_topics = NULL;
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
#if (URT_CFG_RPC_ENABLED == true)
  _urt_core_services = NULL;
#endif /* (URT_CFG_RPC_ENABLED == true) */

  return;
}

/**
 * @brief   Get Core status.
 *
 * @return  Current system status.
 */
urt_status_t urtCoreGetStatus(void)
{
  return _urt_core_status;
}

/**
 * @brief   Get Core event source.
 *
 * @return  Current system event source.
 */
urt_osEventSource_t* urtCoreGetEvtSource(void)
{
  return &_urt_core_evtSource;
}

/**
 * @brief   Start threads of all nodes of the Core.
 */
void urtCoreStartNodes(void)
{
  // lock core
  urtMutexLock(&_urt_core_lock);
  // iterate through list of nodes and start the thread of every node
  for (urt_node_t* node = _urt_core_nodes; node != NULL; node = node->next) {
    urtThreadStart(node->thread);
  }
  // unlock core
  urtMutexUnlock(&_urt_core_lock);

  return;
}

/**
 * @brief   Synchronize all nodes of the core.
 *
 * @param[in] node  Pointer to a node to synchronize.
 *                  Must not be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK            All nodes are synchronized and proceed.
 * @retval URT_STATUS_SYNC_ERROR    An faulty stage value has been detected.
 * @retval URT_STATUS_SYNC_PENDING  There are nodes left to synchronize.
 *                                  In this case, the node thread must still wait for the control event (proceed) to synchronize.
 */
urt_status_t urtCoreSynchronizeNodes(urt_node_t* node)
{
  urtDebugAssert(node != NULL);

  // local variables
  urt_status_t syncstatus;

  // lock core
  urtMutexLock(&_urt_core_lock);

  // increment stage of specified node.
  ++node->stage;

  // check all nodes
  const urt_node_t* corenode = _urt_core_nodes;
  while (corenode != NULL && corenode->stage == node->stage){
    corenode = corenode->next;
  }

  // if all nodes are at the same stage
  if (corenode == NULL) {
    syncstatus = URT_STATUS_OK;
  }
  // if the stage of the last checked node was one less than the argument
  else if (corenode->stage == (node->stage - 1)) {
    // wait for remaining node(s) to synchronize
    syncstatus = URT_STATUS_SYNC_PENDING;
  }
  // if a invalid node stage was encountered
  else {
    // initiate panic
    syncstatus = URT_STATUS_SYNC_ERROR;
    urtCorePanic(URT_STATUS_SYNC_ERROR);
  }

  // external synchronization
  if (_urt_core_sync != NULL) {
    // if no error occurred
    if (syncstatus == URT_STATUS_OK || syncstatus == URT_STATUS_SYNC_PENDING) {
      // if this was the first synchronization call
      if (syncstatus == URT_STATUS_SYNC_PENDING && urtSyncGetState(_urt_core_sync) == URT_SYNC_IDLE) {
        syncstatus = urtSyncStart(_urt_core_sync, &_urt_core_evtSource, URT_CORE_EVENTFLAG_EMERGENCY);
      }
      // if local synchronization is complete
      else if (syncstatus == URT_STATUS_OK) {
        syncstatus = urtSyncFinal(_urt_core_sync, &_urt_core_evtSource, URT_CORE_EVENTFLAG_PROCEED, URT_CORE_EVENTFLAG_EMERGENCY);
      }
      // if an error occurred
      if (syncstatus == URT_STATUS_SYNC_ERROR && _urt_core_status == URT_STATUS_OK) {
        _urt_core_status = URT_STATUS_SYNC_ERROR;
        /* emergyncy event was already broadcasted by urtSyncX() call */
      }
    }
  }
  // no external synchronization
  else {
    // if synchronization is complete
    if (syncstatus == URT_STATUS_OK) {
      // let all nodes proceed
      urtEventSourceBroadcast(&_urt_core_evtSource, URT_CORE_EVENTFLAG_PROCEED);
    }
  }

  urtMutexUnlock(&_urt_core_lock);
  return syncstatus;
}

/**
 * @brief   Stop threads of all nodes of the Core.
 *
 * @param[in] reason  The reason why the function was called.
 *                    For normal shutdown URT_STATUS_OK should be used.
 *
 * @return  Returns URT_STATUS_OK if there was no call with another reason than URT_STATUS_OK before.
 *          If the function has been called before with a different reason, that reason is returned.
 */
urt_status_t urtCoreStopNodes(urt_status_t reason)
{
  // if some error occurred earlier, ignore this call
  if (_urt_core_status != URT_STATUS_OK){
    return _urt_core_status;
  }

  // set the reason why nodes are stopped
  _urt_core_status = reason;

  // broadcast an event to trigger all nodes to check for termination requests
  urtEventSourceBroadcast(&_urt_core_evtSource, URT_CORE_EVENTFLAG_TERMINATE);

  return URT_STATUS_OK;
}

/**
 * @brief   Prepend node to core's list of nodes.
 *
 * @param[in] node  The node to prepend.
 *                  Must not be NULL.
 *
 * @return  Always returns URT_STATUS_OK.
 */
urt_status_t urtCoreAddNode(urt_node_t* node)
{
  urtDebugAssert(node != NULL && node->next == NULL);

  // lock core
  urtMutexLock(&_urt_core_lock);

  // prepend node
  node->next = _urt_core_nodes;
  _urt_core_nodes = node;

  // unlock core
  urtMutexUnlock(&_urt_core_lock);

  return URT_STATUS_OK;
}

/**
 * @brief   Execute an emergency shutdown.
 *
 * @param[in] reason  Reason for the emergency.
 *                    The pointer is reinterpreted as urt_status_t value.
 */
void urtCorePanic(urt_status_t reason)
{
  urtDebugAssert(reason != URT_STATUS_OK);

  // retrieve calling thread (may be NULL in ISR context)
  urt_osThread_t* const thread = urtThreadGetSelf();
  urt_osThreadPrio_t oldprio;

  // if the function is called from thread context
  if (thread != NULL) {
    // boost thread priority
    oldprio = urtThreadGetPriority();
    urtThreadSetPriority(URT_THREAD_PRIO_HIGH_MAX);
  }

  // set the reason for first emergency only
  if (_urt_core_status == URT_STATUS_OK) {
    _urt_core_status = reason;
  }

  // broadcast an event to trigger emergency shutdown
  urtEventSourceBroadcast(&_urt_core_evtSource, URT_CORE_EVENTFLAG_EMERGENCY);

  // if the function is called from thread context
  if (thread != NULL) {
    // reset thread priority
    urtThreadSetPriority(oldprio);
  }

  return;
}

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)

/**
 * @brief   Insert topic to core's list of topics.
 * @details Topics are sorted by their identifiers in ascending order.
 *
 * @param[in] topic   The topic to append.
 *                    Must not be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                The topic was sucessfully inserted to the core's list.
 * @retval URT_STATUS_TOPIC_DUPLICATE   A topic with identical identifier has been detected.
 */
urt_status_t urtCoreAddTopic(urt_topic_t* topic)
{
  urtDebugAssert(topic != NULL && topic->next == NULL);

  // local variables
  urt_status_t ret = URT_STATUS_OK;

  // lock core
  urtMutexLock(&_urt_core_lock);

  // insert topic wrt. its identifier (increasing)
  if (_urt_core_topics == NULL) {
    // spcial case if this is the first topic
    _urt_core_topics = topic;
  } else if (_urt_core_topics->id >= topic->id) {
    // special case if the new topic is prepended to the whole list
    if (_urt_core_topics->id > topic->id) {
      topic->next = _urt_core_topics;
      _urt_core_topics = topic;
    }
    // special case if the first element in the list is a duplicate
    else {
      ret = URT_STATUS_TOPIC_DUPLICATE;
    }
  } else {
    // iterate through topic list
    urt_topic_t* coretopic = _urt_core_topics;
    while (coretopic->next != NULL && coretopic->next->id < topic->id) {
      coretopic = coretopic->next;
    }
    // append to the list
    if (coretopic->next == NULL) {
      coretopic->next = topic;
    }
    // check for duplicate
    else if (coretopic->next->id == topic->id) {
      ret = URT_STATUS_TOPIC_DUPLICATE;
    }
    // insert topic
    else {
      topic->next = coretopic->next;
      coretopic->next = topic;
    }
  }

  // unlock core
  urtMutexUnlock(&_urt_core_lock);

  return ret;
}

/**
 * @brief   Get topic of core according to argument.
 *
 * @param[in] id Identifier of the topic to retrieve.
 *
 * @return  Returns a pointer to the requested topic.
 * @retval NULL   No topic mathces the given identifier.
 */
urt_topic_t* urtCoreGetTopic(urt_topicid_t id)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
  urtDebugAssert(id <= URT_PUBSUB_TOPICID_MAXVAL);
#pragma GCC diagnostic pop

  // lock core
  urtMutexLock(&_urt_core_lock);

  // iterate through list of topics
  urt_topic_t* topic = _urt_core_topics;
  while (topic != NULL && topic->id < id) {
    topic = topic->next;
  }
  // check for matching identifer
  if (topic != NULL && topic->id != id) {
    topic = NULL;
  }

  // unlock core
  urtMutexUnlock(&_urt_core_lock);

  return topic;
}

#endif /* (URT_CFG_PUBSUB_ENABLED  == true) */

#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)

/**
 * @brief   Insert service to core's list of services.
 * @details Services are sorted by their identifiers in ascending order.
 *
 * @param[in] service   The service to append.
 *                      Must not be NULL.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK                  The service was sucessfully inserted to the core's list.
 * @retval URT_STATUS_SERVICE_DUPLICATE   A service with identical identifier has been detected.
 */
urt_status_t urtCoreAddService(urt_service_t* service)
{
  urtDebugAssert(service != NULL && service->next == NULL);

  // local variables
  urt_status_t ret = URT_STATUS_OK;

  // lock core
  urtMutexLock(&_urt_core_lock);

  // insert service wrt. its identifier (increasing)
  if (_urt_core_services == NULL) {
    // spcial case if this is the first service
    _urt_core_services = service;
  } else if (_urt_core_services->id >= service->id) {
    // special case if the new service is prepended to the whole list
    if (_urt_core_services->id > service->id) {
      service->next = _urt_core_services;
      _urt_core_services = service;
    }
    // special case if the first element in the list is a duplicate
    else {
      ret = URT_STATUS_SERVICE_DUPLICATE;
    }
  } else {
    // iterate through services list
    urt_service_t* coreservice = _urt_core_services;
    while (coreservice->next != NULL && coreservice->next->id < service->id) {
      coreservice = coreservice->next;
    }
    // append to the list
    if (coreservice->next == NULL) {
      coreservice->next = service;
    }
    // check for duplicate
    else if (coreservice->next->id == service->id) {
      ret = URT_STATUS_SERVICE_DUPLICATE;
    }
    // insert topic
    else {
      service->next = coreservice->next;
      coreservice->next = service;
    }
  }

  // unlock core
  urtMutexUnlock(&_urt_core_lock);

  return ret;
}

/**
 * @brief   Get service of core according to argument.
 *
 * @param[in] id  Identifier of the service to retrieve.
 *
 * @return  Returns a pointer to the requested service.
 * @retval NULL   No service matches the given identifier.
 */
urt_service_t*  urtCoreGetService(urt_serviceid_t id)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
  urtDebugAssert(id <= URT_RPC_SERVICEID_MAXVAL);
#pragma GCC diagnostic pop

  // lock core
  urtMutexLock(&_urt_core_lock);

  // iterate through list of services
  urt_service_t* service = _urt_core_services;
  while (service != NULL && service->id < id) {
    service = service->next;
  }
  // check for matching identifer
  if (service != NULL && service->id != id) {
    service = NULL;
  }

  // unlock core
  urtMutexUnlock(&_urt_core_lock);

  return service;
}

/** @} */

#endif /* (URT_CFG_RPC_ENABLED == true) */
