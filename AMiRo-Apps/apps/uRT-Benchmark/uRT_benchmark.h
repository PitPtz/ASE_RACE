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
 * @file    uRT_benchmark.h
 * @brief   A µRT benchmark application.
 *
 * @defgroup apps_urtbenchmark µRT Benchmark
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_urtbenchmark
 * @{
 */

#ifndef URT_BENCHMARK_H
#define URT_BENCHMARK_H

#include <urtbenchmarkconf.h>
#include <urt.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#if !defined(URTBENCHMARK_STACKSIZE) || defined(__DOXYGEN__)
/**
 * @brief   Stack size of threads.
 */
#define URTBENCHMARK_CFG_STACKSIZE                256
#endif /* !defined(URTBENCHMARK_STACKSIZE) */

#if ((CH_CFG_USE_REGISTRY == TRUE) && !defined(URTBENCHMARK_NODENAMESIZE)) || defined(__DOXYGEN__)
/**
 * @brief   Maximum length of thread names.
 */
#define URTBENCHMARK_CFG_THREADNAMELENGTH         16
#endif /* ((CH_CFG_USE_REGISTRY == TRUE) && !defined(URTBENCHMARK_NODENAMESIZE)) */

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

typedef enum urtbenchmark_configdata_nodetype {
  URTBENCHMARK_UNDEFINED,
  URTBENCHMARK_NODE,
#if (HAL_USE_CAN == TRUE) || defined(__DOXYGEN__)
  URTBENCHMARK_TRANSPORT_CAN_EMITTER,
  URTBENCHMARK_TRANSPORT_CAN_RESPONDER,
#endif /* (HAL_USE_CAN == TRUE) */
#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
  URTBENCHMARK_PUBSUB_HOPS,
  URTBENCHMARK_PUBSUB_SCALING,
  URTBENCHMARK_PUBSUB_QOS,
#if (HAL_USE_CAN == TRUE) || defined(__DOXYGEN__)
  URTBENCHMARK_PUBSUB_CAN_EMITTER,
  URTBENCHMARK_PUBSUB_CAN_BRIDGE,
  URTBENCHMARK_PUBSUB_CAN_RESPONDER,
#endif /* (HAL_USE_CAN == TRUE) */
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
  URTBENCHMARK_RPC_HOPS,
  URTBENCHMARK_RPC_SCALING,
  URTBENCHMARK_RPC_QOS,
#if (HAL_USE_CAN == TRUE) || defined(__DOXYGEN__)
  URTBENCHMARK_RPC_CAN_EMITTER,
  URTBENCHMARK_RPC_CAN_BRIDGE_SERVICE,
  URTBENCHMARK_RPC_CAN_BRIDGE_REQUEST,
  URTBENCHMARK_RPC_CAN_RESPONDER,
#endif /* (HAL_USE_CAN == TRUE) */
#endif /* (URT_CFG_RPC_ENABLED == true) */
} urtbenchmark_configdata_nodetype_t;

typedef struct urtbenchmark_configdata_payload {
  uint8_t* buffer;
  size_t size;
} urtbenchmark_configdata_payload_t;

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)

typedef struct urtbenchmark_configdata_publisher {
  urt_publisher_t publisher;
  urt_osTime_t* time;
  urt_publisher_publishpolicy_t policy;
  urtbenchmark_configdata_payload_t payload;
  urt_topicid_t topicid;
} urtbenchmark_configdata_publisher_t;

typedef struct urtbenchmark_configdata_subscriber {
  union {
    urt_basesubscriber_t basesubscriber;
    struct {
      urt_nrtsubscriber_t subscriber;
    } nrtsubscriber;
    struct {
      urt_srtsubscriber_t subscriber;
      urt_usefulness_f usefulnesscb;
      void* params;
    } srtsubscriber;
    struct {
      urt_frtsubscriber_t subscriber;
      urt_delay_t deadline;
      urt_delay_t jitter;
    } frtsubscriber;
    struct {
      urt_hrtsubscriber_t subscriber;
      urt_delay_t deadline;
      urt_delay_t jitter;
      urt_delay_t rate;
    } hrtsubscriber;
  };
  urt_delay_t* latencybuffer;
  urtbenchmark_configdata_payload_t payload;
  urt_topicid_t topicid;
  urt_rtclass_t rtclass;
} urtbenchmark_configdata_subscriber_t;

#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)

typedef struct urtbenchmark_configdata_request {
  union {
    urt_baserequest_t baserequest;
    struct {
      urt_nrtrequest_t request;
    } nrtrequest;
    struct {
      urt_srtrequest_t request;
      urt_usefulness_f usefulnesscb;
      void* params;
    } srtrequest;
    struct {
      urt_frtrequest_t request;
      urt_delay_t deadline;
      urt_delay_t jitter;
    } frtrequest;
    struct {
      urt_hrtrequest_t request;
      urt_delay_t deadline;
      urt_delay_t jitter;
    } hrtrequest;
  };
  urt_service_t* service;
  urt_delay_t* latencybuffer;
  urtbenchmark_configdata_payload_t payload;
  urt_serviceid_t serviceid;
  urt_request_retrievepolicy_t policy;
  urt_rtclass_t rtclass;
} urtbenchmark_configdata_request_t;

typedef struct urtbenchmark_configdata_service {
  urt_service_t service;
  urt_service_dispatched_t dispatched;
  urtbenchmark_configdata_payload_t payload;
  urt_delay_t* delay;
  urt_delay_t* latencybuffer;
  urt_serviceid_t serviceid;
} urtbenchmark_configdata_service_t;

#endif /* (URT_CFG_RPC_ENABLED == true) */

typedef struct urtbenchmark_config {
  struct {
#if (CH_CFG_USE_REGISTRY == TRUE)
    char name[URTBENCHMARK_CFG_THREADNAMELENGTH];
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    urt_osEventSource_t* trigger;
    urt_osEventListener_t listener;
    urt_osTime_t* time;
    urt_delay_t* latencybuffer;
    urtbenchmark_configdata_nodetype_t type;
  } node;

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)

  struct {
    urtbenchmark_configdata_publisher_t* buffer;
    size_t size;
  } publishers;

  struct {
    urtbenchmark_configdata_subscriber_t* buffer;
    size_t size;
  } subscribers;

#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)

  struct {
    urtbenchmark_configdata_request_t* buffer;
    size_t size;
  } requests;

  struct {
    urtbenchmark_configdata_service_t* buffer;
    size_t size;
  } services;

#endif /* (URT_CFG_RPC_ENABLED == true) */

} urtbenchmark_config_t;

typedef struct urtbenchmark_node {
  URT_THREAD_MEMORY(thread, URTBENCHMARK_CFG_STACKSIZE);

  urt_node_t node;

  urtbenchmark_config_t* config;

  urt_osEventMask_t loopEventMask;

} urtbenchmark_node_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void ubnInit(urtbenchmark_node_t* bn, urt_osThreadPrio_t prio, urtbenchmark_config_t* config);
  void ubnConfigInit(urtbenchmark_config_t* config);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* URT_BENCHMARK_H */

/** @} */
