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
 * @file    urt_node.h
 * @brief   Node structure and interfaces.
 *
 * @defgroup kernel_node Node
 * @ingroup kernel
 * @brief   Nodes are a fundamental building block in µRT.
 * @details Nodes basically represent threads in a system.
 *          The execution function of those threads is defined by µRT, though.
 *          Custom code is injected to node threads via callbacks, which are
 *          executed by the main function.
 *
 *          While a node is always a thread, it may spawn/consist of further
 *          threads internally (each node is a thread, but not each thread is a
 *          thread necessarily).
 *          That said, if a node holds further threads, those are unknown to µRT
 *          core and thus are not influenced by its control API.
 *          It is the responsibility of the nores themselves to manage any
 *          subthreads.
 *
 * @addtogroup kernel_node
 * @{
 */

#ifndef URT_NODE_H
#define URT_NODE_H

#include <urt.h>

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
 * forward declaration
 * @cond
 */
typedef struct urt_node urt_node_t;
/** @endcond */

/**
 * @brief   Function type for setup callbacks.
 */
typedef urt_osEventMask_t (*urt_nodeSetupCallback_t)(urt_node_t* node, void* arg);

/**
 * @brief   Function type for loop callbacks
 */
typedef urt_osEventMask_t (*urt_nodeLoopCallback_t)(urt_node_t* node, urt_osEventMask_t events, void* arg);

/**
 * @brief   Function type for shutdown callbacks.
 */
typedef void (*urt_nodeShutdownCallback_t)(urt_node_t* node, urt_status_t reason, void* arg);

#if (URT_CFG_PUBSUB_ENABLED == true && URT_CFG_PUBSUB_PROFILING == true) ||     \
    (URT_CFG_RPC_ENABLED == true && URT_CFG_RPC_PROFILING == true) ||           \
    defined(__DOXYGEN__)
/**
 * @brief   Node profiling data structure.
 */
typedef struct urt_node_profilingdata
{
  /**
   * @brief   Loop counter.
   */
  urt_profilingcounter_t loops;

} urt_node_profilingdata_t;
#endif /* (URT_CFG_PUBSUB_ENABLED == true && URT_CFG_PUBSUB_PROFILING == true) || (URT_CFG_RPC_ENABLED == true && URT_CFG_RPC_PROFILING == true) */

/**
 * @brief   Type to use for the node stage value.
 * @details Use wide types only if you need to synchronize the system very often.
 */
#if (URT_CFG_NODESTAGE_WIDTH == 8)
typedef uint8_t urt_nodestage_t;
#elif (URT_CFG_NODESTAGE_WIDTH == 16) || defined(__DOXYGEN__)
typedef uint16_t urt_nodestage_t;
#elif (URT_CFG_NODESTAGE_WIDTH == 32)
typedef uint32_t urt_nodestage_t;
#elif (URT_CFG_NODESTAGE_WIDTH == 64)
typedef uint64_t urt_nodestage_t;
#else
# error "URT_CFG_NODESTAGE_WIDTH must be set to 8, 16, 32 or 64."
#endif

/**
 * @brief  Node data structure.
 */
typedef struct urt_node
{
  /**
   * @brief   Pointer to the next node in a list.
   */
  struct urt_node* next;

  /**
   * @brief   Pointer to the associated thread object.
   */
  urt_osThread_t* thread;

  /**
   * @brief   Pointer to an optional setup callback.
   */
  urt_nodeSetupCallback_t setupcallback;

  /**
   * @brief   Pointer to optional parameters for the setup callback.
   */
  void* setupparams;

  /**
   * @brief    Pointer to the mandatory loop callback.
   */
  urt_nodeLoopCallback_t loopcallback;

  /**
   * @brief   Pointer to optional parameters for the loop callback.
   */
  void* loopparams;

  /**
   * @brief   Pointer to an optional shutdown callback.
   */
  urt_nodeShutdownCallback_t shutdowncallback;

  /**
   * @brief   Pointer to optional parameters for the shutdown callback.
   */
  void* shutdownparams;

  /**
   * @brief   Current execution stage of the node.
   */
  urt_nodestage_t stage;

  /**
   * @brief   Event listener for core events.
   */
  urt_osEventListener_t listener;

#if (URT_CFG_PUBSUB_ENABLED == true && URT_CFG_PUBSUB_PROFILING == true) ||     \
    (URT_CFG_RPC_ENABLED == true && URT_CFG_RPC_PROFILING == true) ||           \
    defined(__DOXYGEN__)
  /**
   * @brief   Profiling data.
   */
  urt_node_profilingdata_t profiling;
#endif /* (URT_CFG_PUBSUB_ENABLED == true && URT_CFG_PUBSUB_PROFILING == true) || (URT_CFG_RPC_ENABLED == true && URT_CFG_RPC_PROFILING == true) */

} urt_node_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void urtNodeInit(urt_node_t* node, urt_osThread_t* thread, size_t threadsize, urt_osThreadPrio_t prio,
                   urt_nodeSetupCallback_t setupcallback, void* setupparams,
                   urt_nodeLoopCallback_t loopcallback, void* loopparams,
                   urt_nodeShutdownCallback_t shutdowncallback, void* shutdownparams);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* URT_NODE_H */

/** @} */
