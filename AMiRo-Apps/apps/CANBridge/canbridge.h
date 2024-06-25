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
 * @file    canbridge.h
 *
 * @defgroup apps_canbridge CAN Bridge
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_canbridge
 * @{
 */

#ifndef CANBRIDGE_H
#define CANBRIDGE_H

#include <urt.h>

#include "../../messagetypes/DWD_floordata.h"
#include "../../messagetypes/DWD_accodata.h"
#include "../../messagetypes/DWD_compassdata.h"
#include "../../messagetypes/DWD_gyrodata.h"
#include "../../messagetypes/ProximitySensordata.h"
#include "../../messagetypes/LightRing_leddata.h"
#include "../../messagetypes/motiondata.h"
#include "../../messagetypes/positiondata.h"
#include "../../messagetypes/Motor_motordata.h"
#include <AMiRoDefault_services.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#if !defined(CAN_STACKSIZE) || defined(__DOXYGEN__)
/**
 * @brief   Stack size of can threads.
 */
#define CAN_STACKSIZE             512
#endif /* !defined(CAN_STACKSIZE) */

// Because the request could be responded several requests are needed
#define CAN_NUM_REQUESTS   3
#define CAN_NUM_RESPONSES  3

#define CAN_NUM_PAYLOADS 50
#define CAN_NUM_FRAMES 20
#define CAN_PAYLOAD_MAX_SIZE 40 //Maximum size of payload data in bytes
#define CAN_MAX_SUBSCRIBERS 50
/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

typedef enum canBridge_frame_status {
  BLANK,
  DONE,
  WIP, 
  META, //Meta Frame is received
  METATRASH,  // Meta Frame is received but frame status is trash
  TRASH
}canBridge_frame_status_t;

typedef union canBridge_id_map {
  uint32_t raw;
  struct {
    uint32_t cnt : 6;
    uint32_t id : 16;
    uint32_t type : 4;
    uint32_t board_id : 3;
  };
}canBridge_id_map_t;

typedef union canBridge_meta_data{
  uint8_t raw;
  struct {
    uint8_t fireandforget : 1;
    uint8_t total_payloads: 7;
  };
}canBridge_meta_data_t;

typedef struct canBridge_buffer {
  bool used;
  uint8_t cnt;
  uint8_t payload[sizeof(((CANTxFrame*)0)->data8)];
  struct canBridge_buffer* next;
}canBridge_buffer_t;

typedef struct canBridge_frames {
  canBridge_frame_status_t status;
  uint8_t id;
  uint8_t board_id;
  uint8_t type;
  urt_osTime_t timestamp;
  canBridge_meta_data_t metafftp; 
  uint8_t current_payloads;
  size_t bytes;
  canBridge_buffer_t* payload;
}canBridge_frames_t;

typedef struct canBridge_service_list {
  urt_service_t* service;
  size_t payload_size;
  struct canBridge_service_list* next;
}canBridge_service_list_t;

typedef struct canBridge_subscriber_list {
  urt_nrtsubscriber_t* subscriber;
  urt_osEventMask_t* mask;
  size_t payload_size;
  urt_topicid_t topic_id;
  uint8_t transmit_factor; // 1->1, 2->1/2
  struct canBridge_subscriber_list* next;
}canBridge_subscriber_list_t;

/**
 * @brief   canLogic node.
 * @struct  canLogic_node
 */
typedef struct canBridge_node {
  /**
   * @brief   Thread memory.
   */
  URT_THREAD_MEMORY(thread, CAN_STACKSIZE);

  uint8_t board_id;

  uint8_t subscriber_transmit_cnt[CAN_MAX_SUBSCRIBERS];

  /**
   * @brief Filter IDs where the CAN Bus should listen/transmit
   */
  aos_fbcan_filter_t can_filter;

  /**
   * @brief frame ids
   */
  canBridge_id_map_t filter_map;

  /**
   * @brief   Node object.
   */
  urt_node_t node;

  /**
   * @brief Used to signal the node when the frame data are transmitted completely over the CAN Bus
   */
  struct {
    urt_osEventSource_t source;
    urt_osEventListener_t listener;
    urt_osEventMask_t events; //events where the node loop listens to
  }frame_signal;

  canBridge_service_list_t* services;

  urt_service_dispatched_t dispatched[CAN_NUM_RESPONSES];
  uint8_t response_payloads[CAN_NUM_RESPONSES][CAN_PAYLOAD_MAX_SIZE];

  urt_request_t requests[CAN_NUM_REQUESTS]; 
  uint8_t request_payloads[CAN_NUM_REQUESTS][CAN_PAYLOAD_MAX_SIZE];
  urt_serviceid_t dispatched_service_ids[CAN_NUM_REQUESTS];
  urt_serviceid_t* request_service_ids;
  uint8_t num_request;

  canBridge_subscriber_list_t* subscriber;

  urt_publisher_t publisher;
  uint8_t publisher_payloads[CAN_PAYLOAD_MAX_SIZE];
  urt_topicid_t* publisher_topic_ids;
  uint8_t num_publisher;

  canBridge_buffer_t payloads[CAN_NUM_PAYLOADS];
  canBridge_frames_t frames[CAN_NUM_FRAMES]; 
} canBridge_node_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void canBridgeInit(canBridge_node_t* can, 
                    canBridge_subscriber_list_t* subscriber,
                    urt_topicid_t publisher_topic_ids[],
                    uint8_t num_publisher,
                    canBridge_service_list_t* services,
                    urt_serviceid_t request_service_ids[],
                    uint8_t num_request,
                    urt_osThreadPrio_t prio);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/******************************************************************************/
/* SUBSYSTEMS                                                                 */
/******************************************************************************/

#endif /* FLOOR_H */

/** @} */
