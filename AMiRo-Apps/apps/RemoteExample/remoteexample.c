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
 * @file    remoteexample.c
 */

#include <remoteexample.h>
#include <amiroos.h>
#include <stdlib.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/**
 * @brief   Event mask to be set on service events.
 */
#define REMOTEEVENT                  (urt_osEventMask_t)(1<< 1)

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
 * @brief   Name of remoteexample nodes.
 */
static const char _remoteexample_name[] = "RemoteExample";

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

void re_getData(void* remoteexample, urt_osEventMask_t event) {
  urtDebugAssert(remoteexample != NULL);

  //local variables
  urt_status_t status;

  if (event & REMOTEEVENT) {
    // fetch NRT of the floor proximity data
    do {
      size_t size = sizeof(((remoteexample_node_t*)remoteexample)->remote.data);
      status = urtNrtSubscriberFetchNext(&((remoteexample_node_t*)remoteexample)->remote.nrt,
                                         &((remoteexample_node_t*)remoteexample)->remote.data,
                                         &size,
                                         NULL, NULL);
    } while (status != URT_STATUS_FETCH_NOMESSAGE);
  }

  return;
}

urt_osEventMask_t _remoteexample_Setup(urt_node_t* node, void* remoteexample)
{
  urtDebugAssert(remoteexample != NULL);
  (void)node;

  // set thread name
  chRegSetThreadName(_remoteexample_name);

#if (URT_CFG_PUBSUB_ENABLED == true)
  urt_topic_t* const remote_topic = urtCoreGetTopic(((remoteexample_node_t*)remoteexample)->remote.topicid);
  urtDebugAssert(remote_topic != NULL);
  urtNrtSubscriberSubscribe(&((remoteexample_node_t*)remoteexample)->remote.nrt, remote_topic, REMOTEEVENT);
#endif /* URT_CFG_PUBSUB_ENABLED == true */

  return REMOTEEVENT;
}

/**
 * @brief   Loop callback function for remoteexample nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] remoteexample    Pointer to the maze structure.
 *                    Must nor be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _remoteexample_Loop(urt_node_t* node, urt_osEventMask_t event, void* remoteexample)
{
  urtDebugAssert(remoteexample != NULL);
  (void)node;


  re_getData(remoteexample, event);

  remoteexample_node_t* re = (remoteexample_node_t*)remoteexample;

  urtPrintf("New data from ESP %u %u %u %u \n", re->remote.data.bytes[0], re->remote.data.bytes[1], re->remote.data.bytes[2], re->remote.data.bytes[3]);

  return REMOTEEVENT;
}

/**
 * @brief   Shutdown callback function for maze nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] reason  Reason of the termination.
 * @param[in] remoteexample  Pointer to the maze structure
 *                           Must not be NULL.
 */
void _remoteexample_Shutdown(urt_node_t* node, urt_status_t reason, void* remoteexample)
{
  urtDebugAssert(remoteexample != NULL);

  (void)node;
  (void)reason;

#if (URT_CFG_PUBSUB_ENABLED == true)
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
// unsubscribe from topics
  urtNrtSubscriberUnsubscribe(&((remoteexample_node_t*)remoteexample)->remote.nrt);
  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_remoteexample
 * @{
 */

void remoteexampleInit(remoteexample_node_t* remoteexample,
                    urt_osThreadPrio_t prio)
{
  urtDebugAssert(remoteexample != NULL);

#if (URT_CFG_PUBSUB_ENABLED == true)
  urtNrtSubscriberInit(&remoteexample->remote.nrt);
#endif /* URT_CFG_PUBSUB_ENABLED == true */

  // initialize the node
  urtNodeInit(&remoteexample->node, (urt_osThread_t*)remoteexample->thread, sizeof(remoteexample->thread), prio,
              _remoteexample_Setup, remoteexample,
              _remoteexample_Loop, remoteexample,
              _remoteexample_Shutdown, remoteexample);

  return;
}

/** @} */
