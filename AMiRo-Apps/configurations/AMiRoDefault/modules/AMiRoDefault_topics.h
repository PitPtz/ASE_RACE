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
 * @file    AMiRoDefault_topics.h
 * @brief   AMiRoDefault µRT topic identifiers.
 *
 * @addtogroup configs_amirodefault_modules
 * @{
 */

#ifndef AMIRODEFAULT_TOPICS_H
#define AMIRODEFAULT_TOPICS_H

#include <urt.h>
#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)

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

enum {
  TOPICID_FLOOR_PROXIMITY,                      /**< Floor proximity topic. */ /*payload=DWD_floordata.h:floor_proximitydata_t*/
  TOPICID_FLOOR_AMBIENT,                        /**< Floor ambient light topic. */ /*payload=DWD_floordata.h:floor_ambientdata_t*/
  TOPICID_DME,                                  /**< DifferentialMotioEstimator motion topic. */ /*payload=motiondata.h:motion_ev_csi*/
  TOPICID_PROXIMITYSENSOR_AMBIENT,              /**< ProximitySensor ambient light topic. */ /*payload=ProximitySensordata.h:proximitysensor_ambientdata_t*/
  TOPICID_PROXIMITYSENSOR_PROXIMITY,            /**< ProximitySensor proximity topic. */ /*payload=ProximitySensordata.h:proximitysensor_proximitydata_t*/
  TOPICID_TOUCH,                                /**< Touch topic. */ /*payload=TouchSensordata.h:touch_data_t*/
  TOPICID_ACCELEROMETER,                        /**< Accelerometer motion topic. */ /*payload=DWD_accodata.h:acco_converted_data_t*/
  TOPICID_GYROSCOPE,                            /**< Gyroscope motion topic. */ /*payload=DWD_gyrodata.h:gyro_converted_data_t*/
  TOPICID_COMPASS,                              /**< Gravity direction topic. */ /*payload=DWD_gravitydata.h:gravity_data_t*/
  TOPICID_MAGNETOMETER,                         /**< TBD */ /*payload=DWD_magnodata.h:magno_converted_data_t*/
  TOPICID_ODOMETRY,                             /**< Odometry values topic. */ /*payload=positiondata.h:position_cv_si*/
  TOPICID_BATTERY,                              /**< Battery values topic. */ /*payload=chargedata.h:battery_data_t*/
  TOPICID_STATUS                                /**< Amiro Status topic. */ /*payload=amirostatus.h:status_data_t*/
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

#endif /* URT_CFG_PUBSUB_ENABLED == true */
#endif /* AMIRODEFAULT_TOPICS_H */

/** @} */
