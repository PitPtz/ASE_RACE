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
 * @file    urt_core.h
 * @brief   Core interfaces.
 *
 * @defgroup kernel_core Core
 * @ingroup kernel
 * @brief   Core logic and API of µRT.
 * @details The core logic consists of some global memory and several API
 *          functions.
 *
 * @addtogroup kernel_core
 * @{
 */

#ifndef URT_CORE_H
#define URT_CORE_H

#include <urt.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief   Event flag to signal an emegerncy shutdown.
 */
#define URT_CORE_EVENTFLAG_EMERGENCY            (urt_osEventFlags_t)(1 << 0)

/**
 * @brief   Event flag for node termination requests.
 */
#define URT_CORE_EVENTFLAG_TERMINATE            (urt_osEventFlags_t)(1 << 1)

/**
 * @brief   Event flag to proceed after node synchronization.
 */
#define URT_CORE_EVENTFLAG_PROCEED              (urt_osEventFlags_t)(1 << 2)

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/**
 * @brief   Retrieve the event mask used for core events.
 */
#define urtCoreGetEventMask()                   URT_EVENTMASK_MAXPRIO

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

  void urtCoreInit(urt_sync_t* sync);
  urt_status_t urtCoreGetStatus(void);
  urt_osEventSource_t* urtCoreGetEvtSource(void);
  void urtCoreStartNodes(void);
  urt_status_t urtCoreSynchronizeNodes(urt_node_t* node);
  urt_status_t urtCoreStopNodes(urt_status_t reason);
  urt_status_t urtCoreAddNode(urt_node_t* node);
  void urtCorePanic(urt_status_t reason);

#if (URT_CFG_PUBSUB_ENABLED == true) || defined (__DOXYGEN__)
  urt_status_t urtCoreAddTopic(urt_topic_t* topic);
  urt_topic_t* urtCoreGetTopic(urt_topicid_t id);
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true) || defined (__DOXYGEN__)
  urt_status_t urtCoreAddService(urt_service_t* service);
  urt_service_t* urtCoreGetService(urt_serviceid_t id);
#endif /* (URT_CFG_RPC_ENABLED == true) */

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* URT_CORE_H */

/** @} */
