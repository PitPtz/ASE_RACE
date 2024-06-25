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
 * @file    linefollowing.h
 * @brief   A application that let the AMiRo follow a line.
 *
 * @defgroup apps_linefollowing Line Following
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_linefollowing
 * @{
 */

#ifndef LINEFOLLOWING_H
#define LINEFOLLOWING_H

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

#if !defined(LINEFOLLOWING_STACKSIZE) || defined(__DOXYGEN__)
/**
 * @brief   Stack size of linefollowing threads.
 */
#define LINEFOLLOWING_STACKSIZE             512
#endif /* !defined(LINEFOLLOWING_STACKSIZE) */

#define RAND_TRESH 20000
#define MAX_CORRECTED_SPEED 1000000*100

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
 * @brief Line Following states
 */
typedef enum lf_state {
  IDLE,
  LINEFOLLOWING,
  EDGEFOLLOWING
}lf_state_t;

typedef enum LineFollowStrategy{
  NONE,           // Deactivate any movement
  CENTER,         // Classic line following using the Fuzzy strategy
  EDGE_LEFT,      // Edge following: driving on the left edge of a black line
  EDGE_RIGHT     // Edge following: driving on the right edge of a black line
} lf_strategy_t;

/**
* @brief   Trigger related data of the floor sensors.
*/
typedef struct lf_floor{
 urt_topicid_t topicid;
 urt_nrtsubscriber_t nrt;
 floor_proximitydata_t data;
 size_t data_size;
}lf_floor_t;

/**
* @brief  Data of the motor
*/
typedef struct lf_motor {
  motion_ev_csi data;
  urt_nrtrequest_t request;
  urt_service_t* service;
}lf_motor_t;

/**
* @brief  Data of the light
*/
typedef struct lf_light {
  urt_nrtrequest_t request;
  light_led_data_t data;
  urt_serviceid_t serviceid;
}lf_light_t;

/**
* @brief   Data of the environment sensors.
*/
typedef struct lf_environment{
 urt_topicid_t topicid;
 urt_nrtsubscriber_t nrt;
 proximitysensor_proximitydata_t data;
 size_t data_size;
}lf_environment_t;

/**
 * @brief   LineFollowing node.
 * @struct  linefollowing_node
 */
typedef struct linefollowing_node {

  URT_THREAD_MEMORY(thread, LINEFOLLOWING_STACKSIZE);
  urt_node_t node;

  lf_floor_t floor_data;
  lf_motor_t motor_data;
  lf_light_t light_data;
  lf_environment_t environment_data;
  lf_state_t state;
  lf_strategy_t strategy;
  aos_timer_t timer;
} linefollowing_node_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void linefollowingInit(linefollowing_node_t* linefollowing,
                 urt_serviceid_t dmc_target_serviceid,
                 urt_serviceid_t led_light_serviceid,
                 urt_topicid_t proximity_topicid,
                 urt_topicid_t environment_topicid,
                 urt_osThreadPrio_t prio);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
int linefollowing_ShellCallback_strategy(BaseSequentialStream* stream, int argc, const char* argv[], linefollowing_node_t* lf);
#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

void updateCorrectionSpeed(linefollowing_node_t* linefollowing);
void lineFollowing(linefollowing_node_t* linefollowing);

/******************************************************************************/
/* SUBSYSTEMS                                                                 */
/******************************************************************************/

#endif /* LINEFOLLOWING_H */

/** @} */
