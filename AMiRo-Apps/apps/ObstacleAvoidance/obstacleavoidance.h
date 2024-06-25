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
 * @file    obstacleavoidance.h
 * @brief   A application that let the AMiRo avoid obstacles.
 *
 * @defgroup apps_obstacleavoidance
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_obstacleavoidance
 * @{
 */

#ifndef OBSTACLEAVOIDANCE_H
#define OBSTACLEAVOIDANCE_H

#include <urt.h>
#include "../../messagetypes/DWD_floordata.h"
#include "../../messagetypes/motiondata.h"
#include "../../messagetypes/chargedata.h"
#include "../../messagetypes/LightRing_leddata.h"
#include "../../messagetypes/positiondata.h"
#include "../../messagetypes/ProximitySensordata.h"


/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#if !defined(OBSTACLEAVOIDANCE_STACKSIZE) || defined(__DOXYGEN__)
/**
 * @brief   Stack size of obstacleavoidance threads.
 */
#define OBSTACLEAVOIDANCE_STACKSIZE             512
#endif /* !defined(OBSTACLEAVOIDANCE_STACKSIZE) */

#define OBSTACLEAVOIDANCE_PROX_THRESHOLD_LOW  0x0000
#define OBSTACLEAVOIDANCE_PROX_THRESHOLD_HIGH 0x4000
#define OBSTACLEAVOIDANCE_PROX_THRESHOLD_RANGE (OBSTACLEAVOIDANCE_PROX_THRESHOLD_HIGH-OBSTACLEAVOIDANCE_PROX_THRESHOLD_LOW)

#define OBSTACLEAVOIDANCE_BASE_TRANSLATION 0.1f
#define OBSTACLEAVOIDANCE_MAX_ADDITIONAL_TRANSLATION 0.5f

#define OBSTACLEAVOIDANCE_BASE_ROTATION 0.0f
#define OBSTACLEAVOIDANCE_MAX_ADDITIONAL_ROTATION 4.0f

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/***********************
*******************************************************/

/**
 * @brief ObstacleAvoidance states
 */
typedef enum oa_state {
  OA_IDLE,
  OBSTACLE_AVOIDANCE
}oa_state_t;

typedef enum ObstacleAvoidanceStrategy{
  OA_OFF,
  OA_ON,
} oa_strategy_t;

/**
* @brief  Data of the motor
*/
typedef struct oa_motor {
  motion_ev_csi data;
  urt_nrtrequest_t request;
  urt_service_t* service;
}oa_motor_t;

/**
* @brief  Data of the light
*/
typedef struct oa_light {
  urt_nrtrequest_t request;
  light_led_data_t data;
  urt_serviceid_t serviceid;
}oa_light_t;

/**
* @brief   Data of the environment sensors.
*/
typedef struct oa_environment{
 urt_topicid_t topicid;
 urt_nrtsubscriber_t nrt;
 proximitysensor_proximitydata_t data;
 size_t data_size;
}oa_environment_t;

/**
 * @brief   ObstacleAvoidance node.
 * @struct  ObstacleAvoidance_node
 */
typedef struct obstacleavoidance_node {

  URT_THREAD_MEMORY(thread, OBSTACLEAVOIDANCE_STACKSIZE);
  urt_node_t node;
  oa_strategy_t strategy;
  oa_motor_t motor_data;
  oa_light_t light_data;
  oa_environment_t environment_data;
  oa_state_t state;
  aos_timer_t timer;
} obstacleavoidance_node_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void obstacleavoidanceInit(obstacleavoidance_node_t* obstacleavoidance,
                 urt_serviceid_t dmc_target_serviceid,
                 urt_serviceid_t led_light_serviceid,
                 urt_topicid_t environment_topicid,
                 urt_osThreadPrio_t prio);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
int obstacleavoidance_ShellCallback_strategy(BaseSequentialStream* stream, int argc, const char* argv[], obstacleavoidance_node_t* lf);
#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */


/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

void obstacleAvoidance(obstacleavoidance_node_t* obstacleavoidance);

/******************************************************************************/
/* SUBSYSTEMS                                                                 */
/******************************************************************************/

#endif /* OBSTACLEAVOIDANCE_H */

/** @} */
