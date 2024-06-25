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
 * @file    remoteexample.h
 * @brief   An application that learns motor control with reinforcement learning.
 *
 * @defgroup apps_remoteexample RemoteExample
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_remoteexample
 * @{
 */

#ifndef REMOTEEXAMPLE_H
#define REMOTEEXAMPLE_H

#include <urt.h>
#include "../../messagetypes/remotedata.h"
#include "../../messagetypes/LightRing_leddata.h"
#include "../../messagetypes/motiondata.h"


/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#if !defined(REMOTEEXAMPLE_STACKSIZE) || defined(__DOXYGEN__)
/**
 * @brief   Stack size of remoteexample threads.
 */
#define REMOTEEXAMPLE_STACKSIZE             512
#endif /* !defined(REMOTEEXAMPLE_STACKSIZE) */

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
* @brief   Trigger related to remote data
*/
typedef struct re_remote {
 urt_topicid_t topicid;
 urt_nrtsubscriber_t nrt;
 remote_data_t data;
}re_remote_t;

/**
 * @brief   RemoteExample node.
 * @struct  remoteexample_node
 */
typedef struct remoteexample_node {

  URT_THREAD_MEMORY(thread, REMOTEEXAMPLE_STACKSIZE);
  urt_node_t node;

  re_remote_t remote;
} remoteexample_node_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void remoteexampleInit(remoteexample_node_t* remoteexample,urt_osThreadPrio_t prio);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/******************************************************************************/
/* SUBSYSTEMS                                                                 */
/******************************************************************************/

#endif /* REMOTEEXAMPLE_H */

/** @} */
