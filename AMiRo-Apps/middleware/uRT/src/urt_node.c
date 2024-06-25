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
 * @file    urt_node.c
 * @brief   Node code.
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
/**
 * @name Local Functions
 * @{
 */

/**
 * @brief  Main function of a node thread.
 *
 * @param[in] node  Pointer to the node object associated to this thread.
 */
void _urt_node_main(urt_node_t* const node)
{
  // local variables
  urt_osEventMask_t waitmask;

  /*
   * SETUP
   */
  // register to core event
  urtEventRegister(urtCoreGetEvtSource(), &(node->listener), urtCoreGetEventMask(), 0);
  // if setup callback is set
  if (node->setupcallback != NULL) {
    // execute setup callback
    waitmask = urtCoreGetEventMask() | node->setupcallback(node, node->setupparams);
  }
  // if no setup callback is set
  else {
    // act as if setup callback returned 0xF..F
    waitmask = URT_EVENTMASK_ALL;
  }

  // synchronize nodes if core status is URT_STATUS_OK
  if (urtCoreGetStatus() == URT_STATUS_OK) {
    // synchronize
    urtCoreSynchronizeNodes(node);
    // wait for core event
    urtEventWait(urtCoreGetEventMask(), URT_EVENT_WAIT_ALL, URT_DELAY_INFINITE);
    // clear flags
    urtEventListenerClearFlags(&node->listener, URT_CORE_EVENTFLAG_PROCEED);
  }

  /*
   * LOOP
   */
  // loop as long as no termination request has been received
  while (!urtThreadShouldTerminate()) {
    // wait for a specified or a core event
    const urt_osEventMask_t event = urtEventWait(waitmask, URT_EVENT_WAIT_ANY, URT_DELAY_INFINITE);
    // if a core event was received
    if (event & urtCoreGetEventMask()) {
      // get and evaluate core event flags
      const urt_osEventFlags_t flags = urtEventListenerClearFlags(&node->listener, ~0);
      if (flags & URT_CORE_EVENTFLAG_EMERGENCY) {
        urtThreadTerminate(node->thread, URT_THREAD_TERMINATE_KILL);
      } else if (flags & URT_CORE_EVENTFLAG_TERMINATE) {
        urtThreadTerminate(node->thread, URT_THREAD_TERMINATE_REQUEST);
      }
    }
    // if a non core event was received
    else {
      // execute loop callback
      waitmask = urtCoreGetEventMask() | node->loopcallback(node, event, node->loopparams);
#if (URT_CFG_PUBSUB_ENABLED == true && URT_CFG_PUBSUB_PROFILING == true) || (URT_CFG_RPC_ENABLED == true && URT_CFG_RPC_PROFILING == true)
      ++node->profiling.loops;
#endif /* (URT_CFG_PUBSUB_ENABLED == true && URT_CFG_PUBSUB_PROFILING == true) || (URT_CFG_RPC_ENABLED == true && URT_CFG_RPC_PROFILING == true) */
    }
  }

  /*
   * OUTRO
   */
  // propagate termination request to all child threads
  urtThreadTerminate(node->thread, URT_THREAD_TERMINATE_REQUEST);

  // if shutdown callback is set
  if (node->shutdowncallback != NULL) {
    // execute shutdown callback
    node->shutdowncallback(node, urtCoreGetStatus(), node->shutdownparams);
  }

  // unregister from core event
  urtEventUnregister(urtCoreGetEvtSource(), &node->listener);

  // terminate node thread
  urtThreadExit();

  return;
}

/** @} */

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

/**
 * @addtogroup kernel_node
 * @{
 */

/**
 * @brief  Initalize a node.
 *
 * @param[in] node              The node to initialize. Must not be NULL.
 * @param[in] thread            The thread to intialize.
 * @param[in] threadsize        Size of the thread object.
 * @param[in] prio              Priority of this node
 * @param[in] setupcallback     Callback function to be executed during setup.
 *                              May be NULL if no custom setup is required.
 * @param[in] setupparams       Parameters for the setup callback function.
 *                              Must be NULL if no setup callback is specified.
 *                              May be NULL if the specified setup callback does not expect parameters.
 * @param[in] loopcallback      Callback function to be executed in a loop.
 *                              Must not be NULL.
 * @param[in] loopparams        Parameters for the loop callback function.
 *                              May be NULL if the specified loop callback does not expect parameters.
 * @param[in] shutdowncallback  Callback function to be executed during shutdown.
 *                              May be NULL if no custom shutdown is required.
 * @param[in] shutdownparams    Parameters for the loop callback function.
 *                              Must be NULL if no shutdown callback is specified.
 *                              May be NULL if the specified shutdown callback does not expect parameters.
 */
void urtNodeInit(urt_node_t* node, urt_osThread_t* thread, size_t threadsize, urt_osThreadPrio_t prio,
                 urt_nodeSetupCallback_t setupcallback, void* setupparams,
                 urt_nodeLoopCallback_t loopcallback, void* loopparams,
                 urt_nodeShutdownCallback_t shutdowncallback, void* shutdownparams)
{
  urtDebugAssert(node != NULL);
  urtDebugAssert(thread != NULL);
  urtDebugAssert(threadsize > 0);
  urtDebugAssert(setupcallback != NULL || (setupcallback == NULL && setupparams == NULL));
  urtDebugAssert(loopcallback != NULL);
  urtDebugAssert(shutdowncallback != NULL || (shutdowncallback == NULL && shutdownparams == NULL));

  // initialize node data
  node->next = NULL;
  node->thread = urtThreadInit((void*)thread, threadsize, prio, (urt_osThreadFunction_t) _urt_node_main, (void*)node);
  node->setupcallback = setupcallback;
  node->setupparams = setupparams;
  node->loopcallback = loopcallback;
  node->loopparams = loopparams;
  node->shutdowncallback = shutdowncallback;
  node->shutdownparams = shutdownparams;
  node->stage = 0;
  urtEventListenerInit(node->listener);
#if (URT_CFG_PUBSUB_ENABLED == true && URT_CFG_PUBSUB_PROFILING == true) || (URT_CFG_RPC_ENABLED == true && URT_CFG_RPC_PROFILING == true)
  node->profiling.loops = 0;
#endif /* (URT_CFG_PUBSUB_ENABLED == true && URT_CFG_PUBSUB_PROFILING == true) || (URT_CFG_RPC_ENABLED == true && URT_CFG_RPC_PROFILING == true) */

  // instert node to core list
  urtCoreAddNode(node);

  return;
}

/** @} */
