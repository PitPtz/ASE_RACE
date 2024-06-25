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
 * @file    helloworld_slave.c
 * @brief   Slave node of the HelloWorld application.
 */

#include <helloworld.h>
#include <amiroos.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
/**
 * @brief   Event mask to set on a publish event.
 */
#define PUBLISHEVENT                            (urtCoreGetEventMask() << 1)
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
/**
 * @brief   Event mask to set on a request event.
 */
#define REQUESTEVENT                            (urtCoreGetEventMask() << 2)
#endif /* (URT_CFG_RPC_ENABLED == true) */

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
 * @brief   Name of slave nodes.
 */
static const char _helloworld_slave_name[] = "HelloWorld_Slave";

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   Setup callback function for slave nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] slave   Pointer to the slave structure.
 *                    Must nor be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _helloworld_slaveSetup(urt_node_t* node, void* slave)
{
  urtDebugAssert(slave != NULL);

  (void)node;

  // set thread name
  chRegSetThreadName(_helloworld_slave_name);

#if (URT_CFG_PUBSUB_ENABLED == true)
  // subscribe to the topic
  urt_topic_t* const topic = urtCoreGetTopic(((helloworld_slave_t*)slave)->topicid);
  urtDebugAssert(topic != NULL);
  urtNrtSubscriberSubscribe(&((helloworld_slave_t*)slave)->subscribers.nrt, topic, PUBLISHEVENT);
  urtSrtSubscriberSubscribe(&((helloworld_slave_t*)slave)->subscribers.srt, topic, PUBLISHEVENT, helloworldSrtUsefulness, NULL);
  urtFrtSubscriberSubscribe(&((helloworld_slave_t*)slave)->subscribers.frt, topic, PUBLISHEVENT, ((helloworld_slave_t*)slave)->deadline, ((helloworld_slave_t*)slave)->jitter);
  urtHrtSubscriberSubscribe(&((helloworld_slave_t*)slave)->subscribers.hrt, topic, PUBLISHEVENT, ((helloworld_slave_t*)slave)->deadline, ((helloworld_slave_t*)slave)->jitter, ((helloworld_slave_t*)slave)->rate, NULL, NULL);
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

  return 0
#if (URT_CFG_PUBSUB_ENABLED == true)
         | PUBLISHEVENT
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
#if (URT_CFG_RPC_ENABLED == true)
         | REQUESTEVENT
#endif /* (URT_CFG_RPC_ENABLED == true) */
         ;
}

/**
 * @brief   Loop callback function for slave nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] event   The received event(s).
 * @param[in] slave   Pointer to the slave structure.
 *                    Must nor be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _helloworld_slaveLoop(urt_node_t* node, urt_osEventMask_t event, void* slave)
{
  urtDebugAssert(slave != NULL);

  (void)node;
#if (URT_CFG_PUBSUB_ENABLED != true) && (URT_CFG_RPC_ENABLED != true)
  (void)event;
#endif /* (URT_CFG_PUBSUB_ENABLED != true) && (URT_CFG_RPC_ENABLED != true) */

#if (URT_CFG_PUBSUB_ENABLED == true)
  // handle pub-sub event
  if (event & PUBLISHEVENT) {
    // local variables
    urt_status_t status;
    urt_delay_t latency;

    // fetch HRT
    do {
      status = urtHrtSubscriberFetchNext(&((helloworld_slave_t*)slave)->subscribers.hrt, NULL, NULL, NULL, NULL);

      // print subscriber indicator
      if (((helloworld_slave_t*)slave)->printOutput && status != URT_STATUS_FETCH_NOMESSAGE) {
        urtPrintf("\tHRT!");
        if (status != URT_STATUS_OK) {
          urtPrintf(" -> jitter violation");
        }
        urtPrintf("\n");
      }
    } while (status != URT_STATUS_FETCH_NOMESSAGE);

    // fetch FRT
    do {
      status = urtFrtSubscriberFetchNext(&((helloworld_slave_t*)slave)->subscribers.frt, NULL, NULL, NULL, &latency);

      // print subscriber indicator
      if (((helloworld_slave_t*)slave)->printOutput && status != URT_STATUS_FETCH_NOMESSAGE) {
        urtPrintf("\tFRT! (%s)", urtFrtSubscriberCalculateValidity(&((helloworld_slave_t*)slave)->subscribers.frt, latency) ? "valid" : "invalid");
        if (status != URT_STATUS_OK) {
          urtPrintf(" -> %s violation", (status == URT_STATUS_DEADLINEVIOLATION) ? "deadline" : "jitter");
        }
        urtPrintf("\n");
      }
    } while (status != URT_STATUS_FETCH_NOMESSAGE);

    // fetch SRT
    do {
      status = urtSrtSubscriberFetchNext(&((helloworld_slave_t*)slave)->subscribers.srt, NULL, NULL, NULL, &latency);

      // print subscriber indicator
      if (((helloworld_slave_t*)slave)->printOutput && status != URT_STATUS_FETCH_NOMESSAGE) {
        urtPrintf("\tSRT! (usefulness: %f)\n", urtSrtSubscriberCalculateUsefulness(&((helloworld_slave_t*)slave)->subscribers.srt, latency));
      }
    } while (status != URT_STATUS_FETCH_NOMESSAGE);

    // fetch NRT
    do {
      status = urtNrtSubscriberFetchNext(&((helloworld_slave_t*)slave)->subscribers.nrt, NULL, NULL, NULL, NULL);

      // print subscriber indicator
      if (((helloworld_slave_t*)slave)->printOutput && status != URT_STATUS_FETCH_NOMESSAGE) {
        urtPrintf("\tNRT!\n");
      }
    } while (status != URT_STATUS_FETCH_NOMESSAGE);

#if (URT_CFG_DEBUG_ENABLED == true)
    // clear publish event
    event &= ~PUBLISHEVENT;
#endif /* (URT_CFG_DEBUG_ENABLED == true) */
  }
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true)
  // handle request event
  if (event & REQUESTEVENT) {
    // dispatch all available requests
    urt_service_dispatched_t dispatched;
    while (true) {
      // break if there are no pending requests
      if (urtServiceDispatch(&((helloworld_slave_t*)slave)->service, &dispatched, NULL, NULL, NULL) == false) {
        break;
      }

      /* do some processing
       * For the sake of simplicity, this node does nothing.
       * Also, Printing any message at this point would corrupt output due to
       * the multi-threaded nature of the HelloWorld application.
       */

      // acquire request and respond
      if (urtServiceAcquireRequest(&((helloworld_slave_t*)slave)->service, &dispatched) == URT_STATUS_OK) {
        // respond
        urtServiceRespond(&dispatched, 0);
      }
    }

#if (URT_CFG_DEBUG_ENABLED == true)
    // clear request event
    event &= ~REQUESTEVENT;
#endif /* (URT_CFG_DEBUG_ENABLED == true) */
  }
#endif /* (URT_CFG_RPC_ENABLED == true) */

#if (URT_CFG_DEBUG_ENABLED == true)
  if (event != 0) {
    urtErrPrintf("HelloWorld Slave: unexpected event received: 0x%08X\n", event);
  }
#endif /* (URT_CFG_DEBUG_ENABLED == true) */

  return 0
#if (URT_CFG_PUBSUB_ENABLED == true)
         | PUBLISHEVENT
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
#if (URT_CFG_RPC_ENABLED == true)
         | REQUESTEVENT
#endif /* (URT_CFG_RPC_ENABLED == true) */
         ;
}

/**
 * @brief   Shutdown callback function for slave nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] slave   Pointer to the slave structure.
 *                    Must nor be NULL.
 */
void _helloworld_slaveShutdown(urt_node_t* node, urt_status_t reason, void* slave)
{
  urtDebugAssert(slave != NULL);

  (void)node;
  (void)reason;

#if (URT_CFG_PUBSUB_ENABLED == true)
  // unsubscribe topics
  urtNrtSubscriberUnsubscribe(&((helloworld_slave_t*)slave)->subscribers.nrt);
  urtSrtSubscriberUnsubscribe(&((helloworld_slave_t*)slave)->subscribers.srt);
  urtFrtSubscriberUnsubscribe(&((helloworld_slave_t*)slave)->subscribers.frt);
  urtHrtSubscriberUnsubscribe(&((helloworld_slave_t*)slave)->subscribers.hrt);
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_helloworld_slave
 * @{
 */

/**
 * @brief   Initializes a slave node.
 *
 * @param[in] slave       Slave object to initialize.
 *                        Must not be NULL.
 * @param[in] prio        Priority of the node thread.
 * @param[in] topicid     Identifier of the topic to subscribe to.
 * @param[in] serviceid   Identifier of the service provide.
 */
void helloWorldSlaveInit(helloworld_slave_t* slave, urt_osThreadPrio_t prio,
#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
                         urt_topicid_t topicid,
#else /* (URT_CFG_PUBSUB_ENABLED == true) */
                         size_t topicid,
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
                         urt_serviceid_t serviceid)
#else /* (URT_CFG_RPC_ENABLED == true) */
                         size_t serviceid)
#endif /* (URT_CFG_RPC_ENABLED == true) */
{
  urtDebugAssert(slave != NULL);

  // initialize the node
  urtNodeInit(&slave->node, (urt_osThread_t*)slave->thread, sizeof(slave->thread), prio,
              _helloworld_slaveSetup, slave,
              _helloworld_slaveLoop, slave,
              _helloworld_slaveShutdown, slave);

  // set output flag
  slave->printOutput = true;

#if (URT_CFG_PUBSUB_ENABLED == true)
  // store topic ID
  slave->topicid = topicid;
  // set timing constraints
  slave->deadline = 0;
  slave->jitter = 0;
  slave->rate = 0;
  // initialize the subscribers
  urtNrtSubscriberInit(&slave->subscribers.nrt);
  urtSrtSubscriberInit(&slave->subscribers.srt);
  urtFrtSubscriberInit(&slave->subscribers.frt);
  urtHrtSubscriberInit(&slave->subscribers.hrt);
#else /* (URT_CFG_PUBSUB_ENABLED == true) */
  (void)topicid;
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true)
  urtServiceInit(&slave->service, serviceid, slave->node.thread, REQUESTEVENT);
#else /* (URT_CFG_RPC_ENABLED == true) */
  (void)serviceid;
#endif /* (URT_CFG_RPC_ENABLED == true) */

  return;
}

/** @} */
