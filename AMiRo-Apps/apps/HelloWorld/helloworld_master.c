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
 * @file    helloworld_master.c
 * @brief   Master node of the HelloWorld application.
 */

#include <helloworld.h>
#include <amiroos.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/**
 * @brief   Event mask to set on a trigger event.
 */
#define TRIGGEREVENT                            (urtCoreGetEventMask() << 1)

#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
/**
 * @brief   Event mask to set on a service event.
 */
#define SERVICEEVENT                            (urtCoreGetEventMask() << 2)
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
 * @brief   Name of master nodes.
 */
static const char _helloworld_master_name[] = "HelloWorld_Master";

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   Setup callback function for master nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] master  Pointer to the master structure.
 *                    Must nor be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _helloworld_masterSetup(urt_node_t* node, void* master)
{
  urtDebugAssert(master != NULL);

  (void)node;

  // set thread name
  chRegSetThreadName(_helloworld_master_name);

  // register trigger event
  urtEventRegister(((helloworld_master_t*)master)->trigger.source, &((helloworld_master_t*)master)->trigger.listener, TRIGGEREVENT, 0);

  return TRIGGEREVENT;
}

/**
 * @brief   Loop callback function for master nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] event   The received event(s).
 * @param[in] master  Pointer to the master structure.
 *                    Must nor be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _helloworld_masterLoop(urt_node_t* node, urt_osEventMask_t event, void* master)
{
  urtDebugAssert(master != NULL);

  (void)node;

  // handle trigger events
  if (event & TRIGGEREVENT) {
    // get flags
    urt_osEventFlags_t flags = urtEventListenerClearFlags(&((helloworld_master_t*)master)->trigger.listener, ~0);

    // MESSAGE event
    if (flags & HELLOWORLD_TRIGGERFLAG_MESSAGE) {
      urt_osTime_t t;
      urtTimeNow(&t);

      // print publish indicator
      if (((helloworld_master_t*)master)->printOutput) {
        urtPrintf("[%u] Hello World!\n", (uint32_t)urtTimeGet(&t));
      }

      // clear MESSAGEflag
      flags &= ~HELLOWORLD_TRIGGERFLAG_MESSAGE;
    }

#if (URT_CFG_PUBSUB_ENABLED == true)
    // PUBSUB event
    if (flags & HELLOWORLD_TRIGGERFLAG_PUBSUB) {
      urt_osTime_t t;
      urtTimeNow(&t);

      // print publish indicator
      if (((helloworld_master_t*)master)->printOutput) {
        urtPrintf("[%u] Hello?\n", (uint32_t)urtTimeGet(&t));
      }

      // publish message
      urtPublisherPublish(&((helloworld_master_t*)master)->publisher, NULL, 0, &t, URT_PUBLISHER_PUBLISH_DETERMINED);

      // clear PUBSUB flag
      flags &= ~HELLOWORLD_TRIGGERFLAG_PUBSUB;
    }
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true)
    // RPC event
    if (flags & HELLOWORLD_TRIGGERFLAG_RPC) {
      // retrieve service
      urt_service_t* const service = urtCoreGetService(((helloworld_master_t*)master)->serviceid);
      urtDebugAssert(service != NULL);

      // print request indicator
      if (((helloworld_master_t*)master)->printOutput) {
        urt_osTime_t t;
        urtTimeNow(&t);
        urtPrintf("[%u] Hello?\n", (uint32_t)urtTimeGet(&t));
      }

      // emit HRT request
      if (urtHrtRequestAcquire(&((helloworld_master_t*)master)->requests.hrt) == URT_STATUS_REQUEST_BADOWNER) {
        urtHrtRequestRetrieve(&((helloworld_master_t*)master)->requests.hrt, URT_REQUEST_RETRIEVE_ENFORCING, NULL, NULL);
      }
      urtHrtRequestSubmit(&((helloworld_master_t*)master)->requests.hrt, service, 0, SERVICEEVENT, ((helloworld_master_t*)master)->deadline);

      // emit FRT request
      if (urtFrtRequestAcquire(&((helloworld_master_t*)master)->requests.frt) == URT_STATUS_REQUEST_BADOWNER) {
        urtFrtRequestRetrieve(&((helloworld_master_t*)master)->requests.frt, URT_REQUEST_RETRIEVE_ENFORCING, NULL, NULL);
      }
      urtFrtRequestSubmit(&((helloworld_master_t*)master)->requests.frt, service, 0, SERVICEEVENT, ((helloworld_master_t*)master)->deadline);

      // emit SRT request
      if (urtSrtRequestAcquire(&((helloworld_master_t*)master)->requests.srt) == URT_STATUS_REQUEST_BADOWNER) {
        urtSrtRequestRetrieve(&((helloworld_master_t*)master)->requests.srt, URT_REQUEST_RETRIEVE_ENFORCING, NULL, NULL);
      }
      urtSrtRequestSubmit(&((helloworld_master_t*)master)->requests.srt, service, 0, SERVICEEVENT);

      // emit NRT request
      if (urtNrtRequestAcquire(&((helloworld_master_t*)master)->requests.nrt) == URT_STATUS_REQUEST_BADOWNER) {
        urtNrtRequestRetrieve(&((helloworld_master_t*)master)->requests.nrt, URT_REQUEST_RETRIEVE_ENFORCING, NULL, NULL);
      }
      urtNrtRequestSubmit(&((helloworld_master_t*)master)->requests.nrt, service, 0, SERVICEEVENT);

      // clear RPC flag
      flags &= ~HELLOWORLD_TRIGGERFLAG_RPC;
    }
#endif /* (URT_CFG_RPC_ENABLED == true) */

#if (URT_CFG_DEBUG_ENABLED == true)
    // unknown event
    if (flags) {
      urtErrPrintf("HelloWorld Master: unknown trigger event received\n");
    }
#endif /* (URT_CFG_DEBUG_ENABLED == true) */

#if (URT_CFG_DEBUG_ENABLED == true)
    // clear trigger event
    event &= ~TRIGGEREVENT;
#endif /* (URT_CFG_DEBUG_ENABLED == true) */
  }

#if (URT_CFG_RPC_ENABLED == true)
  // handle service events
  if (event & SERVICEEVENT) {
    // local variables
    urt_status_t status;
    urt_delay_t latency;

    // try to retrieve HRT request
    if (urtHrtRequestIsPending(&((helloworld_master_t*)master)->requests.hrt)) {
      status = urtHrtRequestRetrieve(&((helloworld_master_t*)master)->requests.hrt, URT_REQUEST_RETRIEVE_DETERMINED, NULL, NULL);
      if (status != URT_STATUS_REQUEST_BADOWNER) {
        urtHrtRequestRelease(&((helloworld_master_t*)master)->requests.hrt);
      }
      if (((helloworld_master_t*)master)->printOutput) {
        urtPrintf("\tHRT");
        switch (status) {
          case URT_STATUS_OK:
            urtPrintf("!\n");
            break;
          case URT_STATUS_DEADLINEVIOLATION:
            urtPrintf(": deadline violation\n");
            break;
          case URT_STATUS_JITTERVIOLATION:
            urtPrintf(": jitter violation\n");
            break;
          default:
            urtPrintf(": unexpected status\n");
            break;
        }
      }
    }

    // try to retrieve FRT request
    if (urtFrtRequestIsPending(&((helloworld_master_t*)master)->requests.frt)) {
      status = urtFrtRequestRetrieve(&((helloworld_master_t*)master)->requests.frt, URT_REQUEST_RETRIEVE_DETERMINED, NULL, &latency);
      if (status != URT_STATUS_REQUEST_BADOWNER) {
        urtFrtRequestRelease(&((helloworld_master_t*)master)->requests.frt);
      }
      if (((helloworld_master_t*)master)->printOutput) {
        urtPrintf("\tFRT");
        switch (status) {
          case URT_STATUS_OK:
            urtPrintf("! (%s)\n", urtFrtRequestCalculateValidity(&((helloworld_master_t*)master)->requests.frt, latency) ? "valid" : "invalid");
            break;
          case URT_STATUS_DEADLINEVIOLATION:
            urtPrintf(": deadline violation\n");
            break;
          case URT_STATUS_JITTERVIOLATION:
            urtPrintf(": jitter violation\n");
            break;
          default:
            urtPrintf(": unexpected status\n");
            break;
        }
      }
    }

    // try to retrieve SRT request
    if (urtSrtRequestIsPending(&((helloworld_master_t*)master)->requests.srt)) {
      status = urtSrtRequestRetrieve(&((helloworld_master_t*)master)->requests.srt, URT_REQUEST_RETRIEVE_DETERMINED, NULL, &latency);
      if (status != URT_STATUS_REQUEST_BADOWNER) {
        urtSrtRequestRelease(&((helloworld_master_t*)master)->requests.srt);
      }
      if (((helloworld_master_t*)master)->printOutput && status == URT_STATUS_OK) {
        urtPrintf("\tSRT");
        switch (status) {
          case URT_STATUS_OK:
            urtPrintf("! (usefulness: %f)\n", urtSrtRequestCalculateUsefulness(&((helloworld_master_t*)master)->requests.srt, latency, helloworldSrtUsefulness, NULL));
            break;
          default:
            urtPrintf(": unexpected status\n");
            break;
        }
      }
    }

    // try to retrieve NRT request
    if (urtNrtRequestIsPending(&((helloworld_master_t*)master)->requests.nrt)) {
      status = urtNrtRequestRetrieve(&((helloworld_master_t*)master)->requests.nrt, URT_REQUEST_RETRIEVE_DETERMINED, NULL, NULL);
      if (status != URT_STATUS_REQUEST_BADOWNER) {
        urtNrtRequestRelease(&((helloworld_master_t*)master)->requests.nrt);
      }
      if (((helloworld_master_t*)master)->printOutput && status == URT_STATUS_OK) {
        urtPrintf("\tNRT");
        switch (status) {
          case URT_STATUS_OK:
            urtPrintf("!\n");
            break;
          default:
            urtPrintf(": unexpected status\n");
            break;
        }
      }
    }
#if (URT_CFG_DEBUG_ENABLED == true)
    // clear service event
    event &= ~SERVICEEVENT;
#endif /* (URT_CFG_DEBUG_ENABLED == true) */
  }
#endif /* (URT_CFG_RPC_ENABLED == true) */

#if (URT_CFG_DEBUG_ENABLED == true)
  if (event != 0) {
    urtErrPrintf("HelloWorld Master: unexpected event received: 0x%08X\n", event);
  }
#endif /* (URT_CFG_DEBUG_ENABLED == true) */

  return TRIGGEREVENT
#if (URT_CFG_RPC_ENABLED == true)
         | SERVICEEVENT
#endif /* (URT_CFG_RPC_ENABLED == true) */
         ;
}

/**
 * @brief   Shutdown callback function for master nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] master  Pointer to the master structure.
 *                    Must nor be NULL.
 */
void _helloworld_masterShutdown(urt_node_t* node, urt_status_t reason, void* master)
{
  urtDebugAssert(master != NULL);

  (void)node;
  (void)reason;

  // unregister trigger event
  urtEventUnregister(((helloworld_master_t*)master)->trigger.source, &((helloworld_master_t*)master)->trigger.listener);

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_helloworld_master
 * @{
 */

/**
 * @brief   Initializes a master node.
 *
 * @param[in] master      Master object to initialize.
 *                        Must not be NULL.
 * @param[in] prio        Priority of the node thread.
 * @param[in] trigger     Event source for trigger events.
 *                        Must not be NULL.
 * @param[in] topicid     Identifier of the topic to publish on.
 * @param[in] serviceid   Identifier of the service to call.
 */
void helloWorldMasterInit(helloworld_master_t* master, urt_osThreadPrio_t prio, urt_osEventSource_t* trigger,
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
  urtDebugAssert(master != NULL);
  urtDebugAssert(trigger != NULL);

  // initialize the node
  urtNodeInit(&master->node, (urt_osThread_t*)master->thread, sizeof(master->thread), prio,
              _helloworld_masterSetup, master,
              _helloworld_masterLoop, master,
              _helloworld_masterShutdown, master);

  // set/initialize event data
  master->trigger.source = trigger;
  urtEventListenerInit(&master->trigger.listener);

  // set output flag
  master->printOutput = true;

#if (URT_CFG_PUBSUB_ENABLED == true)
  // initialize the publisher
  urtPublisherInit(&master->publisher, urtCoreGetTopic(topicid));
#else /* (URT_CFG_PUBSUB_ENABLED == true) */
  (void)topicid;
  (void)messages;
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true)
  // store service ID
  master->serviceid = serviceid;
  // set timing constraints
  master->deadline = 0;
  // initialize requests
  urtNrtRequestInit(&master->requests.nrt, NULL);
  urtSrtRequestInit(&master->requests.srt, NULL);
  urtFrtRequestInit(&master->requests.frt, NULL, 0);
  urtHrtRequestInit(&master->requests.hrt, NULL, 0, NULL, NULL);
#else /* (URT_CFG_RPC_ENABLED == true) */
  (void)serviceid;
#endif /* (URT_CFG_RPC_ENABLED == true) */

  return;
}

/** @} */
