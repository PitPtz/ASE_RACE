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
 * @file    urt_sync.h
 * @brief   Synchronization interfaces.
 *
 * @defgroup kernel_sync Synchronization
 * @ingroup kernel
 * @brief   The synchronization subsystem allows to synchronize groups of nodes
 *          and with external components.
 * @details In multithreaded environments, barriers are a common feature to
 *          synchronize mutiple threads.
 *          In µRT this is realized by the synchronization subsystem.
 *          All nodes within a synchronization group can be synchronized via a
 *          barrier.
 *          Each node can be part of multiple synchronization groups at the same
 *          time.
 *          Nodes can join and leave synchronization groups any time.
 *          Furtermore, the entire µRT instance can be synchronized with other
 *          external components, e.g. further µRT instances.
 *
 * @note    Node sysnchronization subsystem can be disabled via the
 *          configuration.
 *
 * @addtogroup kernel_sync
 * @{
 */

#ifndef URT_SYNC_H
#define URT_SYNC_H

#include <urt.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief   Event flag to proceed after synchronization.
 */
#define URT_SYNC_EVENTFLAG_PROCEED              (urt_osEventFlags_t)(1 << 0)

/**
 * @brief   Event flag to indicate a failed synchronization.
 */
#define URT_SYNC_EVENTFLAG_FAILED               (urt_osEventFlags_t)(1 << 1)

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
typedef struct urt_sync urt_sync_t;
#if (URT_CFG_SYNCGROUPS_ENABLED == true) || defined(__DOXYGEN__)
typedef struct urt_syncgroup urt_syncgroup_t;
typedef struct urt_syncnode urt_syncnode_t;
#endif /* (URT_CFG_SYNCGROUPS_ENABLED == true) */
/** @endcond */

/**
 * @brief   Synchronization stage for external synchronization.
 */
typedef enum urt_syncstage {
  URT_SYNCSTAGE_INITIAL = 0,  /**< Initial synchronization call. */
  URT_SYNCSTAGE_FINAL   = 1,  /**< Final synchronization call. */
} urt_syncstage_t;

/**
 * @brief   Callback function for external synchronization.
 * @details This function shall initiate or check external synchronization in a
 *          non-blocking manner.
 *          When called with URT_SYNCSTAGE_INITIAL as first argument, the call
 *          must not broadcast an event and may only return
 *          URT_STATUS_SYNC_PENDING or URT_STATUS_SYNC_ERROR.
 *          When called with URT_SYNCSTAGE_FINAL as first argument, the call
 *          must broadcast an event in case of a completed synchronization or an
 *          error and return URT_STATUS_OK or URT_STATUS_SYNC_ERROR respectively.
 *          If synchronization is not completed yet, the function must return
 *          URT_STATUS_SYNC_PENDING and the event must be broadcasted externally
 *          as soon as the synchronization completes or fails.
 *
 * @param[in] stage         Synchronization stage of this call.
 * @param[in] eventsource   Event source to broadcast.
 * @param[in] successflags  Flags to broadcast as soon as the synchronization is completed successfully.
 * @param[in] errorflags    Flags to broadcats if an error is encountered.
 * @param[in] params        Pointer to optional parameters.
 *
 * @return  Status indicating the success of the call.
 * @retval URT_STATUS_OK            Synchronization completed successfully.
 *                                  Must only be returned if the stage argument was URT_SYNCSTAGE_FINAL.
 * @retval URT_STATUS_SYNC_PENDING  External synchronization is pending.
 * @retval URT_STATUS_SYNC_ERROR    External synchronization failed.
 */
typedef urt_status_t (*urt_synchronize_f)(urt_syncstage_t stage, urt_osEventSource_t* eventsource, urt_osEventFlags_t successflags, urt_osEventFlags_t errorflags, void* params);

/**
 * @brief   State of a urt_sync_t object.
 */
typedef enum urt_syncstate {
  URT_SYNC_IDLE,      /**< No synchronization process ongoing. */
  URT_SYNC_PENDING,   /**< Synchronization process has been initialized but not completed yet. */
  URT_SYNC_FAILED,    /**< Synchronization failed. */
} urt_syncstate_t;

/**
 * @brief   Data structure for external synchronization.
 */
typedef struct urt_sync {
  /**
   * @brief   Pointer to a callback function to be executed by urtSyncStart()
   *          and urtSyncFinal() methods.
   */
  urt_synchronize_f callback;

  /**
   * @brief   Pointer to optional parameters to be passed to the callback.
   */
  void* params;

  /**
   * @brief   Synchronization state.
   */
  urt_syncstate_t state;
} urt_sync_t;

#if (URT_CFG_SYNCGROUPS_ENABLED == true) || defined(__DOXYGEN__)

/**
 * @brief   Synchronization group data structure.
 */
struct urt_syncgroup {
  /**
   * @brief   Pointer to the first node in the group.
   */
  urt_syncnode_t* nodes;

  /**
   * @brief   Mutex lock for exclusive access.
   */
  urt_osMutex_t lock;

  /**
   * @brief   Event source for synchronization events.
   */
  urt_osEventSource_t source;

  /**
   * @brief   Pointer to an external synchronization object.
   */
  urt_sync_t* sync;
};

/**
 * @brief   Synchronization group list element type.
 */
struct urt_syncnode {
  /**
   * @brief   Pointer to the next node in a singly-linked list.
   */
  urt_syncnode_t* next;

  /**
   * @brief   Pointer to an associated synchronization group.
   */
  urt_syncgroup_t* group;

  /**
   * @brief   Event listener for synchronization events.
   */
  urt_osEventListener_t listener;

  /**
   * @brief   Current synchronization state of the node.
   */
  enum {
    URT_SYNCNODE_SYNCED,
    URT_SYNCNODE_PENDING,
  } state;
};

#endif /* (URT_CFG_SYNCGROUPS_ENABLED == true) */

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

  void urtSyncInit(urt_sync_t* sync, urt_synchronize_f callback, void* params);
  urt_status_t urtSyncStart(urt_sync_t* sync, urt_osEventSource_t* eventsource, urt_osEventFlags_t errorflags);
  urt_status_t urtSyncFinal(urt_sync_t* sync, urt_osEventSource_t* eventsource, urt_osEventFlags_t successflags, urt_osEventFlags_t errorflags);

#if (URT_CFG_SYNCGROUPS_ENABLED == true) || defined(__DOXYGEN__)

  void urtSyncgroupInit(urt_syncgroup_t* syncgroup, urt_sync_t* sync);
  bool urtSyncgroupIsSynchronized(urt_syncgroup_t* syncgroup);

  void urtSyncnodeInit(urt_syncnode_t* syncnode);
  urt_status_t urtSyncnodeJoinGroup(urt_syncnode_t* node, urt_syncgroup_t* group, urt_osEventMask_t mask);
  urt_status_t urtSyncnodeLeaveGroup(urt_syncnode_t* node);
  urt_status_t urtSyncnodeSynchronize(urt_syncnode_t* node);

#endif /* (URT_CFG_SYNCGROUPS_ENABLED == true) */

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/**
 * @brief   Retrieve the current synchronization state.
 *
 * @param[in] sync  Synchronization object to retrieve the state from.
 *                  Must not be NULL.
 *
 * @return  Current synchronization state.
 */
inline bool urtSyncGetState(urt_sync_t* sync)
{
  urtDebugAssert(sync != NULL);

  return sync->state;
}

#endif /* URT_SYNC_H */

/** @} */
