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
 * @file    apps.c
 * @brief   AMiRoDefault configuration application container.
 */

#include "apps.h"
#include <AMiRoDefault_topics.h>
#include <AMiRoDefault_services.h>
#include <amiro_light.h>
#include <canbridge.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED VARIABLES                                                         */
/******************************************************************************/

/******************************************************************************/
/* LOCAL TYPES                                                                */
/******************************************************************************/

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/

/**
 * @brief Light node instance. 
 */
static amiro_light_node_t _amiro_light;

/**
 * @brief CAN node instance. 
 */
static canBridge_node_t _can;

/******************************************************************************/
/* CAN DATA                                                                   */
/******************************************************************************/

#define NUM_REQUESTS 1
urt_serviceid_t service_IDS[NUM_REQUESTS];

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup configs_amirodefault_modules_lightring12
 * @{
 */

/**
 * @brief   Initializes all data applications for the AMiRo default configuration.
 */
void appsInit(void)
{
  // initialize common data
  appsCommonInit();

  amiroLightInit(&_amiro_light, URT_THREAD_PRIO_HIGH_MIN, SERVICEID_LIGHT);

  service_IDS[0] = SERVICEID_LIGHT;

  // initialize CAN app
  canBridgeInit(&_can,
                NULL,
                NULL,
                (uint8_t)0,
                NULL,
                service_IDS,
                (uint8_t)NUM_REQUESTS,
                URT_THREAD_PRIO_NORMAL_MAX);

  return;
}

/** @} */
