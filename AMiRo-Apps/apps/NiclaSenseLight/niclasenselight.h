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
 * @file    niclasenselight.h
 *
 * @defgroup apps_niclasenselight NiclaSenseLight
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_niclasenselight
 * @{
 */

#ifndef NICLASENSELIGHT_H
#define NICLASENSELIGHT_H

#include <urt.h>
#include <alld_NiclaSense.h>
#include "../../messagetypes/ProximitySensordata.h"
#include "../../messagetypes/LightRing_leddata.h"

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#if !defined(LIGHT_STACKSIZE) || defined(__DOXYGEN__)
/**
 * @brief   Stack size of niclasenselight threads.
 */
#define LIGHT_STACKSIZE             512
#endif /* !defined(LIGHT_STACKSIZE) */   


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
* @brief  Data of the light
*/
typedef struct nicla_sense_light {
  urt_nrtrequest_t request;
  urt_service_t* service;
  light_led_data_t data;
}nicla_sense_light_t;



/**
 * @brief   niclasenselight node.
 * @struct  niclasenselight_node
 */
typedef struct niclasenselight_node {
  /**
   * @brief   Thread memory.
   */
  URT_THREAD_MEMORY(thread, LIGHT_STACKSIZE);

  /**
   * @brief   Node object.
   */
  urt_node_t node;
  nicla_sense_light_t light;
  aos_timer_t timer;

    /**
    * @brief   Nicla sense driver.
     */
  struct {
  /**
   * @brief   NICLASENSE driver to use.
   */
  NICLASENSEDRIVER *nicla;

  /**
   * @brief   Timeout delay for certain tests.
   */
  apalTime_t timeout;
} niclasense_data;

#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
  urt_service_t service;
#endif /* (URT_CFG_RPC_ENABLED == true) */
  light_led_data_t values;
} niclasenselight_node_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void niclasenselightInit(niclasenselight_node_t* niclasenselight, urt_osThreadPrio_t prio, urt_serviceid_t serviceID_Light);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/******************************************************************************/
/* SUBSYSTEMS                                                                 */
/******************************************************************************/

#endif /* NICLASENSELIGHT_H */

/** @} */
