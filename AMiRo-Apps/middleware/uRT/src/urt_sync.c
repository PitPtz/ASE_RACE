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
 * @file    urt_sync.c
 * @brief   Node synchronization code.
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

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

/**
 * @addtogroup kernel_sync
 * @{
 */

/**
 * @brief   Initialie an external synchronization instance.
 *
 * @param[in] sync      Pointer to the instance to initialize.
 *                      Must not be NULL.
 * @param[in] callback  Pointer to a callback function.
 *                      Must not be NULL.
 * @param[in] params    Pointer to optional parameters for the callback function.
 *                      May be NULL.
 */
void urtSyncInit(urt_sync_t* sync, urt_synchronize_f callback, void* params)
{
  urtDebugAssert(sync != NULL);
  urtDebugAssert(callback != NULL);

  sync->callback = callback;
  sync->params = params;
  sync->state = URT_SYNC_IDLE;

  return;
}

/**
 * @brief   Start an external synchronization.
 * @details This function must only be called if the current synchronization
 *          state is URT_SYNC_IDLE.
 *
 * @param[in] sync          Pointer to the synchronization instance.
 *                          Must not be NULL.
 * @param[in] eventsource   Pointer to an event source to signal errors, if any.
 *                          Must not be NULL.
 * @param[in] errorflags    Flags to broadcast on an error event.
 *
 * @return  Indicatiion of the synchronization status.
 * @retval URT_SYNC_PENDING   External synchronization has been started.
 * @retval urt_SYNC_FAILED    An error has occurred.
 *                            In this case, an error event is broadcasted.
 */
urt_status_t urtSyncStart(urt_sync_t* sync, urt_osEventSource_t* eventsource, urt_osEventFlags_t errorflags)
{
  urtDebugAssert(sync != NULL);
  urtDebugAssert(sync->state == URT_SYNC_IDLE);
  urtDebugAssert(eventsource != NULL);

  const urt_status_t status = sync->callback(URT_SYNCSTAGE_INITIAL, NULL, 0, 0, sync->params);
  urtDebugAssert(status == URT_STATUS_SYNC_PENDING || status == URT_STATUS_SYNC_ERROR);

  if (status == URT_STATUS_SYNC_ERROR) {
    sync->state = URT_SYNC_FAILED;
    urtEventSourceBroadcast(eventsource, errorflags);
  } else {
    sync->state = URT_SYNC_PENDING;
  }

  return status;
}

/**
 * @brief   Try to complete an external synchronization.
 *
 * @param[in] sync          Pointer to the synchronization instance.
 *                          Must not be NULL.
 * @param[in] eventsource   Pointer to an event source to signal on succes or
 *                          failure.
 *                          Must not be NULL.
 * @param[in] successflags  Flags to broadcast on a success event.
 * @param[in] errorflags    Flags to broadcast on an error event.
 *
 * @return  Indicatiion of the synchronization status.
 * @retval URT_STATUS_OK            External synchronization was completed successfully.
 *                                  In this case, a success event has been broadcasted.
 * @retval URT_STATUS_SYNC_PENDING  External synchronization still ongoing.
 *                                  In this case, no event is broadcasted.
 *                                  Instead, such event will be emitted
 *                                  asnychronously at a later point in time.
 * @retval URT_STATUS_SYNC_ERROR    An error occurred.
 *                                  In this case, an error event has been broadcasted.
 */
urt_status_t urtSyncFinal(urt_sync_t* sync, urt_osEventSource_t* eventsource, urt_osEventFlags_t successflags, urt_osEventFlags_t errorflags)
{
  urtDebugAssert(sync != NULL);
  urtDebugAssert(sync->state != URT_SYNC_FAILED);
  urtDebugAssert(eventsource != NULL);
  urtDebugAssert((successflags != errorflags) || (successflags == 0 && errorflags == 0));

  const urt_status_t status = sync->callback(URT_SYNCSTAGE_FINAL, eventsource, successflags, errorflags, sync->params);
  urtDebugAssert(status == URT_STATUS_OK || status == URT_STATUS_SYNC_PENDING || status == URT_STATUS_SYNC_ERROR);

  sync->state = (status == URT_STATUS_OK) ? URT_SYNC_IDLE : (status == URT_STATUS_SYNC_PENDING) ? URT_SYNC_PENDING : URT_SYNC_EVENTFLAG_FAILED;

  return status;
}

#if (URT_CFG_SYNCGROUPS_ENABLED == true) || defined(__DOXYGEN__)

/**
 * @brief   Inizialize a synchronization group.
 *
 * @param[in] syncgroup   Pointer to the synchronization group to initialize.
 *                        Must not be NULL.
 * @param[in] sync        Pointer to an external synchronization object.
 *                        May be NULL.
 */
void urtSyncgroupInit(urt_syncgroup_t* syncgroup, urt_sync_t* sync)
{
  urtDebugAssert(syncgroup != NULL);

  syncgroup->nodes = NULL;
  urtMutexInit(&syncgroup->lock);
  urtEventSourceInit(&syncgroup->source);
  syncgroup->sync = sync;

  return;
}

/**
 * @brief   Retrieve whether a synchronization group is currently synchronized.
 *
 * @param[in] syncgroup   Pointer to the synchronization group to check.
 *                        Must not be NULL.
 *
 * @return  Indicator, whether or not the synchronization group is currently synchronized.
 */
bool urtSyncgroupIsSynchronized(urt_syncgroup_t* syncgroup)
{
  urtDebugAssert(syncgroup != NULL);

  // local variables
  bool synchronized = true;

  // lock group
  urtMutexLock(&syncgroup->lock);

  // check external synchronization
  if (syncgroup->sync != NULL) {
    if (urtSyncGetState(syncgroup->sync) != URT_SYNC_IDLE) {
      synchronized = false;
    }
  }

  if (synchronized) {
    // check nodes
    urt_syncnode_t* node = syncgroup->nodes;
    while (node != NULL && node->state == URT_SYNCNODE_SYNCED) {
      node = node->next;
    }
    synchronized = (node == NULL);
  }

  // unlock group
  urtMutexUnlock(&syncgroup->lock);

  return synchronized;
}

/**
 * @brief   Initialize a synchronization node.
 *
 * @param[in] syncnode  Synchronization node to initialize.
 */
void urtSyncnodeInit(urt_syncnode_t* syncnode)
{
  urtDebugAssert(syncnode != NULL);

  syncnode->next = NULL;
  syncnode->group = NULL;
  urtEventListenerInit(&syncnode->listener);
  syncnode->state = URT_SYNCNODE_SYNCED;
}

/**
 * @brief   Join a synchronization group.
 *
 * @param[in] node    Synchronization node to join the group.
 * @param[in] group   Synchronization group to join.
 * @param[in] mask    Event mask to be used for synchronization events.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK            The node successfully joined a synchronized
 *                                  group.
 * @retval URT_STATUS_SYNC_PENDING  The node successfully joined a currently
 *                                  syncing group and is now pending for
 *                                  synchronization.
 */
urt_status_t urtSyncnodeJoinGroup(urt_syncnode_t* node, urt_syncgroup_t* group, urt_osEventMask_t mask)
{
  urtDebugAssert(group != NULL);
  urtDebugAssert(node != NULL && node->group == NULL);

  // lock group
  urtMutexLock(&group->lock);

  // prepend node
  node->next = group->nodes;
  group->nodes = node;

  // associate group
  node->group = group;

  // register event
  urtEventRegister(&group->source, &node->listener, mask, 0);

  // check whether there are pending nodes in the group and set the node state
  bool pending = false;
  for (urt_syncnode_t* n = group->nodes; n != NULL; n = n->next) {
    if (n->state == URT_SYNCNODE_PENDING) {
      pending = true;
      break;
    }
  }
  node->state = pending ? URT_SYNCNODE_PENDING : URT_SYNCNODE_SYNCED;

  // unlock group
  urtMutexUnlock(&group->lock);

  return pending ? URT_STATUS_SYNC_PENDING : URT_STATUS_OK;
}

/**
 * @brief   Leave a synchronization group.
 * @details If all remaining nodes are pending when the node leaves the group,
 *          a synchronization event is emitted.
 *
 * @param[in] node    Synchronization node to be detached.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK  The node successfully left the group.
 */
urt_status_t urtSyncnodeLeaveGroup(urt_syncnode_t* node)
{
  urtDebugAssert(node != NULL && node->group != NULL);

  urt_syncgroup_t* const group = node->group;
  struct {
    uint8_t synced  : 1;
    uint8_t pending : 1;
    uint8_t found   : 1;
  } checks = {false, false, false};

  // lock group
  urtMutexLock(&node->group->lock);

  // if the node is the first element in the list
  if (group->nodes == node) {
    // remove the node
    group->nodes = group->nodes->next;
    checks.found = true;
  }
  // search for the node and check for pending and synched nodes
  for (urt_syncnode_t* n = group->nodes; n != NULL; n = n->next) {
    /* at this point n != node always holds */

    // track node state
    if (n->state == URT_SYNCNODE_SYNCED) {
      checks.synced = true;
    } else {
      checks.pending = true;
    }

    // if the next element is the node to remove
    if (n->next == node) {
      // remove the node
      n->next = n->next->next;
      checks.found = true;
    }

    // break the loop if there were non-pending nodes
    if (checks.found == true && checks.synced == true) {
      break;
    }
  }
#if (URT_CFG_DEBUG_ENABLED == true)
  urtDebugAssert(checks.found == true);
#endif /* (URT_CFG_DEBUG_ENABLED == true) */

  // unregister event
  urtEventUnregister(&group->source, &node->listener);

  // if all remaining nodes are pending
  if (checks.pending == true && checks.synced == false) {
    if (node->group->sync != NULL) {
      // finalize external synchronization
      urtSyncFinal(node->group->sync, &node->group->source, URT_SYNC_EVENTFLAG_PROCEED, URT_SYNC_EVENTFLAG_FAILED);
    } else {
      // broadcast proceed event
      urtEventSourceBroadcast(&node->group->source, URT_SYNC_EVENTFLAG_PROCEED);
    }
  }

  // unlock group
  urtMutexUnlock(&node->group->lock);

  // reset node data
  node->next = NULL;
  node->group = NULL;

  return URT_STATUS_OK;
}

/**
 * @brief   Synchronize all nodes in a synchronization group.
 *
 * @param[in] node    Synchronization node to be synchronized.
 *
 * @return  Status code indicating the result of the function call.
 * @retval URT_STATUS_OK            The whole group is synchronized. An
 *                                  according event hat been emitted.
 * @retval URT_STATUS_SYNC_PENDING  There are node left to synchronize. An event
 *                                  will be emitted when the last node
 *                                  synchronizes.
 */
urt_status_t urtSyncnodeSynchronize(urt_syncnode_t* node)
{
  urtDebugAssert(node != NULL && node->group != NULL);

  // local varibales
  urt_status_t syncstatus = URT_STATUS_OK;
#if (URT_CFG_DEBUG_ENABLED == true)
  struct {
    uint8_t found       : 1;
    uint8_t allpending  : 1;
  } checks = {false, true};
#endif /* (URT_CFG_DEBUG_ENABLED == true) */

  // lock group
  urtMutexLock(&node->group->lock);

  // if this node is already pending
  if (node->state == URT_SYNCNODE_PENDING) {
    // return immediately
    urtMutexUnlock(&node->group->lock);
    return URT_STATUS_SYNC_PENDING;
  } else {
    // set node to be pending
    node->state = URT_SYNCNODE_PENDING;
  }

  // check whether all nodes are pending
  urt_syncnode_t* n = node->group->nodes;
#if (URT_CFG_DEBUG_ENABLED == true)
  while (n != NULL) {
    if (n == node) {
      checks.found = true;
    }
    if (n->state != URT_SYNCNODE_PENDING) {
      checks.allpending = false;
    }
    n = n->next;
  }
  urtDebugAssert(checks.found == true);
#else /* (URT_CFG_DEBUG_ENABLED == true) */
  while (n != NULL && n->state == URT_SYNCNODE_PENDING) {
    n = n->next;
  }
#endif /* (URT_CFG_DEBUG_ENABLED == true) */

  // if all nodes are pending
#if (URT_CFG_DEBUG_ENABLED == true)
  if (checks.allpending == true) {
#else /* (URT_CFG_DEBUG_ENABLED == true) */
  if (n == NULL) {
#endif /* (URT_CFG_DEBUG_ENABLED == true) */
    syncstatus = URT_STATUS_OK;

    // reset all nodes to be synced
    n = node->group->nodes;
    while (n != NULL) {
      n->state = URT_SYNCNODE_SYNCED;
      n = n->next;
    }

    if (node->group->sync != NULL) {
      // finalize external synchonization
      syncstatus = urtSyncFinal(node->group->sync, &node->group->source, URT_SYNC_EVENTFLAG_PROCEED, URT_SYNC_EVENTFLAG_FAILED);
    } else {
      // broadcast proceed event
      urtEventSourceBroadcast(&node->group->source, URT_SYNC_EVENTFLAG_PROCEED);
    }
  }
  // if there are nodes not syncing yet
  else {
    syncstatus = URT_STATUS_SYNC_PENDING;

    if (node->group->sync != NULL) {
      // start external synchronization
      if (urtSyncGetState(node->group->sync) == URT_SYNC_IDLE) {
        syncstatus = urtSyncStart(node->group->sync, &node->group->source, URT_SYNC_EVENTFLAG_FAILED);
      }
    }
  }

  // unlock group
  urtMutexUnlock(&node->group->lock);
  return syncstatus;
}

#endif /* URT_CFG_SYNCGROUPS_ENABLED == true */

/** @} */
