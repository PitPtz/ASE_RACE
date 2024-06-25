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
 * @file    odometry.h
 *
 * @defgroup apps_odometry Odometry
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_odometry
 * @{
 */

#ifndef ODOMETRY_H
#define ODOMETRY_H

#include <urt.h>
#include "../../messagetypes/motiondata.h"
#include "../../messagetypes/positiondata.h"

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#if !defined(ODOMETRY_STACKSIZE) || defined(__DOXYGEN__)
/**
 * @brief   Stack size of odometry threads.
 */
#define ODOMETRY_STACKSIZE             512
#endif /* !defined(ODOMETRY_STACKSIZE) */

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/


typedef struct odometry_node {
  URT_THREAD_MEMORY(thread, ODOMETRY_STACKSIZE);
  urt_node_t node;

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
  /**
   * @brief   Publisher to publish the odometry data
   */
  urt_publisher_t odom_publisher;
  /**
   * @brief Subscriber to get the motion data
   */
  urt_nrtsubscriber_t nrt;
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true) 
  /**
   * @brief   Service to reset the odometry values.
   */
  urt_service_t service;
#endif /* (URT_CFG_RPC_ENABLED == true) */

  position_cv_ccsi odomData;

  motion_ev_csi motionData;
  motion_ev_csi old_motionData;
  urt_osTime_t lastTime;
}odometry_node_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void odometryInit(odometry_node_t *odom, 
                    urt_osThreadPrio_t prio, 
                    urt_topicid_t motion_topicid, 
                    urt_topicid_t odom_topicid,
                    urt_serviceid_t reset_serviceid);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/******************************************************************************/
/* SUBSYSTEMS                                                                 */
/******************************************************************************/

#endif /* ODOMETRY_H */

/** @} */
