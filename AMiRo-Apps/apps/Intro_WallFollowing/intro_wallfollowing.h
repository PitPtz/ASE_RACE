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
 * @file    intro_wallfollowing.h
 *
 * @defgroup apps_intro WallFollowing
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_intro
 * @{
 */

#ifndef INTRO_WALLFOLLOWING_H
#define INTRO_WALLFOLLOWING_H

#include <urt.h>
#include "../../messagetypes/ProximitySensordata.h"
#include "../../messagetypes/LightRing_leddata.h"
#include "../../messagetypes/motiondata.h"
#include "../../messagetypes/DWD_floordata.h"
#include "../../messagetypes/amirostatus.h"

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#if !defined(INTRO_WALLFOLLOWING_STACKSIZE) || defined(__DOXYGEN__)
/**
 * @brief   Stack size of intro_wallfollowing threads.
 */
#define INTRO_WALLFOLLOWING_STACKSIZE             256
#endif /* !defined(INTRO_WALLFOLLOWING_STACKSIZE) */


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
 * @brief Intro_WallFollowing states
 */
typedef enum intro_wl_state {
  WL_IDLE,
  WL_WALLFOLLOWING,
  WL_PRINTING,
  WL_LINEFOLLOWING
}intro_wl_state_t;

/**
* @brief   Trigger related data of the floor sensors.
*/
typedef struct intro_wl_floor{
 urt_topicid_t topicid;
 urt_nrtsubscriber_t nrt;
 floor_proximitydata_t data;
} intro_wl_floor_t;

/**
* @brief  Data of the motor
*/
typedef struct intro_wl_motor {
  motion_ev_csi data;
  urt_nrtrequest_t request;
  urt_service_t* service;
}intro_wl_motor_t;


/**
 * @brief  Data of the light
 */
typedef struct intro_wl_light {
  urt_nrtrequest_t request;
  urt_service_t* service;
  light_led_data_t data;
}intro_wl_light_t;


/**
 * @brief   Trigger related data of the ring sensors.
 */
typedef struct intro_wl_ring {
  urt_topicid_t topicid;
  urt_nrtsubscriber_t nrt;
  proximitysensor_proximitydata_t data;
}intro_wl_ring_t;


/**
 * @brief   intro_wallfollowing node.
 * @struct  intro_wallfollowing
 */
typedef struct intro_wallfollowing_node {
  /**
   * @brief   Thread memory.
   */
  URT_THREAD_MEMORY(thread, INTRO_WALLFOLLOWING_STACKSIZE);

  /**
   * @brief   Node object.
   */
  urt_node_t node;

  urt_publisher_t status_publisher;
  status_data_t status_data;

  intro_wl_floor_t floor;
  intro_wl_light_t light;
  intro_wl_motor_t motor;
  intro_wl_ring_t ring;

  intro_wl_state_t state;
} intro_wallfollowing_node_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void intro_wl_Init(intro_wallfollowing_node_t* intro_wl,
                     urt_osThreadPrio_t prio,
                     urt_serviceid_t serviceID_Light,
                     urt_serviceid_t serviceID_Motor,
                     urt_topicid_t topicID_floorProx,
                     urt_topicid_t topicID_ringProx,
                     urt_topicid_t topicID_status);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/******************************************************************************/
/* SUBSYSTEMS                                                                 */
/******************************************************************************/

#endif /* INTRO_WALLFOLLOWING_H */

/** @} */
