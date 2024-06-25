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
 * @file    helloworld_slave.h
 * @brief   Slave node of the HelloWorld application.
 *
 * @defgroup apps_helloworld_slave Slave
 * @ingroup apps_helloworld
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_helloworld_slave
 * @{
 */

#ifndef HELLOWORLD_SLAVE_H
#define HELLOWORLD_SLAVE_H

#include <helloworld.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#if !defined(HELLOWORLD_STACKSIZE_SLAVE) || defined(__DOXYGEN__)
/**
 * @brief   Stack size of slave threads.
 */
#define HELLOWORLD_STACKSIZE_SLAVE              256
#endif /* !defined(HELLOWORLD_STACKSIZE_SLAVE) */

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
 * @brief   Slave node.
 * @struct  helloworld_slave
 */
typedef struct helloworld_slave {
  /**
   * @brief   Thread memory.
   */
  URT_THREAD_MEMORY(thread, HELLOWORLD_STACKSIZE_SLAVE);

  /**
   * @brief   Node object.
   */
  urt_node_t node;

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
  /**
   * @brief   Identifier of the topic to subscribe to.
   */
  urt_topicid_t topicid;

  /**
   * @brief   Deadline constraint for HRT and FRT subscribers.
   */
  urt_delay_t deadline;

  /**
   * @brief   Jitter constraint to use for HRT and FRT subscribers.
   */
  urt_delay_t jitter;

  /**
   * @brief   Rate constraint to use for HRT subscribers.
   */
  urt_delay_t rate;

  /**
   * @brief The   Subscribers for publish-subscribe testing.
   */
  struct {
    /**
     * @brief   NRT subscriber.
     */
    urt_nrtsubscriber_t nrt;

    /**
     * @brief   SRT subscriber.
     */
    urt_srtsubscriber_t srt;

    /**
     * @brief   FRT subscriber.
     */
    urt_frtsubscriber_t frt;

    /**
     * @brief   HRT subscriber.
     */
    urt_hrtsubscriber_t hrt;
  } subscribers;
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
  /**
   * @brief   Service to provide for RPC testing.
   */
  urt_service_t service;
#endif /* (URT_CFG_RPC_ENABLED == true) */

  /**
   * @brief   Flag whether to print outputs.
   */
  bool printOutput;

} helloworld_slave_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

void helloWorldSlaveInit(helloworld_slave_t* slave, urt_osThreadPrio_t prio,
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

#endif /* HELLOWORLD_SLAVE_H */

/** @} */
