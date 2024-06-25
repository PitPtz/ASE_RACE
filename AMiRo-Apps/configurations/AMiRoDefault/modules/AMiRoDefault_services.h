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
 * @file    AMiRoDefault_services.h
 * @brief   AMiRoDefault µRT service identifiers.
 *
 * @addtogroup configs_amirodefault_modules
 * @{
 */

#ifndef AMIRODEFAULT_SERVICES_H
#define AMIRODEFAULT_SERVICES_H

#include <urt.h>
#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

//Ids need to be bitwise for the CANBridge filter and not 0
//Maximum number of IDs is 16

/**
 * @brief   Service identifiers.
 */
enum services {
  SERVICEID_LIGHT,                           /**< Light service. */ /*payload=LightRing_leddata.h:light_led_data_t*/
  SERVICEID_DMC_TARGET,                      /**< DifferentialMotorControl target motion service. */ /*payload=motiondata.h:motion_ev_csi*/
  SERVICEID_DMC_CALIBRATION,                 /**< DifferentialMotorControl calibration service. */ /*payload=*/
  SERVICEID_FLOOR_CALIBRATION,               /**< Floor proximity sensor calibration service. */ /*payload=*/
  SERVICEID_PROXIMITYSENSOR_CALIBRATION,     /**< ProximitySensor proximity sensor calibration service. */ /*payload=*/
  SERVICEID_ODOMETRY                         /**< Odometry reset values service. */ /*payload=*/
};



/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* URT_CFG_RPC_ENABLED == true */
#endif /* AMIRODEFAULT_SERVICES_H */

/** @} */
