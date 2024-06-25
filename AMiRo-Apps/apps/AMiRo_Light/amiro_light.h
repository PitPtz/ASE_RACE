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
 * @file    amiro_light.h
 *
 * @defgroup apps_light Light
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_light
 * @{
 */

#ifndef AMIRO_LIGHT_H
#define AMIRO_LIGHT_H

#include <urt.h>
#include "../../messagetypes/ProximitySensordata.h"
#include "../../messagetypes/LightRing_leddata.h"

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#if !defined(AMIRO_LIGHT_STACKSIZE) || defined(__DOXYGEN__)
/**
 * @brief   Stack size of light threads.
 */
#define AMIRO_LIGHT_STACKSIZE             512
#endif /* !defined(AMIRO_LIGHT_STACKSIZE) */   


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
 * @brief   light node.
 * @struct  light_node
 */
typedef struct light_node {
  /**
   * @brief   Thread memory.
   */
  URT_THREAD_MEMORY(thread, AMIRO_LIGHT_STACKSIZE);

  /**
   * @brief   Node object.
   */
  urt_node_t node;

  /**
   * @brief   Driver related data.
   */
  struct {
    /**
     * @brief   24 channel PWM LED driver.
     */
    TLC5947Driver* driver;

    /**
     * @brief buffer for the LED driver
     */
    tlc5947_lld_buffer_t buffer;
  } driver_data;

#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
  urt_service_t service;
#endif /* (URT_CFG_RPC_ENABLED == true) */
  light_led_data_t values;
} amiro_light_node_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void amiroLightInit(amiro_light_node_t* light, urt_osThreadPrio_t prio, urt_serviceid_t id);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/******************************************************************************/
/* SUBSYSTEMS                                                                 */
/******************************************************************************/

#endif /* AMIRO_LIGHT_H */

/** @} */
