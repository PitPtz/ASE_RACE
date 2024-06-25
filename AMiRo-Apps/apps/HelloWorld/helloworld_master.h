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
 * @file    helloworld_master.h
 * @brief   Master node of the HelloWorld application.
 *
 * @defgroup apps_helloworld_master Master
 * @ingroup apps_helloworld
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_helloworld_master
 * @{
 */

#ifndef HELLOWORLD_MASTER_H
#define HELLOWORLD_MASTER_H

#include <helloworld.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#if !defined(HELLOWORLD_STACKSIZE_MASTER) || defined(__DOXYGEN__)
/**
 * @brief   Stack size of master threads.
 */
#define HELLOWORLD_STACKSIZE_MASTER             256
#endif /* !defined(HELLOWORLD_STACKSIZE_MASTER) */

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
 * @brief   Master node.
 * @struct  helloworld_master
 */
typedef struct helloworld_master {
  /**
   * @brief   Thread memory.
   */
  URT_THREAD_MEMORY(thread, HELLOWORLD_STACKSIZE_MASTER);

  /**
   * @brief   Node object.
   */
  urt_node_t node;

  /**
   * @brief   Trigger related data.
   */
  struct {
    /**
     * @brief   Pointer to the trigger event source.
     */
    urt_osEventSource_t* source;

    /**
     * @brief   Event listener for trigger events.
     */
    urt_osEventListener_t listener;
  } trigger;

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
  /**
   * @brief   Publisher for publish-subscribe testing.
   */
  urt_publisher_t publisher;
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
  /**
   * @brief   Identifier of the service to use.
   */
  urt_serviceid_t serviceid;

  /**
   * @brief   Deadline constraint to use for HRT and FRT requests.
   */
  urt_delay_t deadline;

  /**
   * @brief   Requests for RPC testing.
   */
  struct {
    /**
     * @brief   NRT request.
     */
    urt_nrtrequest_t nrt;

    /**
     * @brief   SRT request.
     */
    urt_srtrequest_t srt;

    /**
     * @brief   FRT request.
     */
    urt_frtrequest_t frt;

    /**
     * @brief   HRT request.
     */
    urt_hrtrequest_t hrt;
  } requests;
#endif /* (URT_CFG_RPC_ENABLED == true) */

  /**
   * @brief   Flag whether to print outputs.
   */
  bool printOutput;

} helloworld_master_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

void helloWorldMasterInit(helloworld_master_t* master, urt_osThreadPrio_t prio, urt_osEventSource_t* trigger,
#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
                          urt_topicid_t topicid,
#else /* (URT_CFG_PUBSUB_ENABLED == true) */
                          size_t topicid,
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
                          urt_serviceid_t serviceid);
#else /* (URT_CFG_RPC_ENABLED == true) */
                          size_t serviceid);
#endif /* (URT_CFG_RPC_ENABLED == true) */

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* HELLOWORLD_MASTER_H */

/** @} */
