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
 * @file    lightfloordata.h
 * @brief   A application that let the AMiRo follow a line.
 *
 * @defgroup apps_lightfloordata Line Following
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_lightfloordata
 * @{
 */

#ifndef LIGHTFLOORDATA_H
#define LIGHTFLOORDATA_H

#include <urt.h>
#include "../../messagetypes/DWD_floordata.h"
#include "../../messagetypes/LightRing_leddata.h"


/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#if !defined(LIGHTFLOORDATA_STACKSIZE) || defined(__DOXYGEN__)
/**
 * @brief   Stack size of lightfloordata threads.
 */
#define LIGHTFLOORDATA_STACKSIZE             512
#endif /* !defined(LIGHTFLOORDATA_STACKSIZE) */

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
* @brief   Trigger related data of the floor sensors.
*/
typedef struct lfd_floor{
 urt_topicid_t topicid;
 urt_nrtsubscriber_t nrt;
 floor_proximitydata_t data;
 size_t data_size;
}lfd_floor_t;

/**
* @brief  Data of the light
*/
typedef struct lfd_light {
  urt_nrtrequest_t request;
  light_led_data_t data;
  urt_service_t* service;
}lfd_light_t;


/**
 * @brief   LineFollowing node.
 * @struct  lightfloordata_node
 */
typedef struct lightfloordata_node {

  URT_THREAD_MEMORY(thread, LIGHTFLOORDATA_STACKSIZE);
  urt_node_t node;

  lfd_floor_t floor_prox;
  lfd_light_t light_led;
} lightfloordata_node_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void lightfloordataInit(lightfloordata_node_t* lightfloordata, 
                          urt_serviceid_t led_light_serviceid,
                          urt_topicid_t proximity_topicid,
                          urt_osThreadPrio_t prio);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
int lightfloordata_ShellCallback_state(BaseSequentialStream* stream, int argc, const char* argv[], lightfloordata_node_t* lfd);
#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */
/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/******************************************************************************/
/* SUBSYSTEMS                                                                 */
/******************************************************************************/

#endif /* LIGHTFLOORDATA_H */

/** @} */
