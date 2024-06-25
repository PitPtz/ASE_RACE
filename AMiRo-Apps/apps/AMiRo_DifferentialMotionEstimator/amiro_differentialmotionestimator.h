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
 * @file    amiro_differentialmotionestimator.h
 * @brief   Motion estimator for differential kinematics.
 *
 * @defgroup apps_amiro_dme Differential Motion Estimator
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_amiro_dme
 * @{
 */

#ifndef AMIRO_DifferentialMotionEstimator_H
#define AMIRO_DifferentialMotionEstimator_H

#include <motiondata.h>
#include <amiroos.h>
#include <urt.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief   Size of the DME node thread stack.
 */
#define AMIRO_DME_STACKSIZE                             256

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

#if (MOTIONDATA_AXES < 3)
# error "MOTIONDATA_AXES is expected to be 3 or greater."
#endif

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

/**
 * @brief   Payload type for motion messages.
 */
typedef motion_ev_csi dme_motionpayload_t;

/**
 * @brief   QEI descriptor type.
 */
struct amiro_dme_qeidescriptor {
  /**
   * @brief   Pointer to QEI driver.
   */
  apalQEIDriver_t* driver;

  /**
   * @brief   QEI increments per wheel revolution.
   */
  apalQEICount_t increments_per_revolution;
};

/**
 * @brief   Wheel descriptor type.
 */
struct amiro_dme_wheeldescriptor {
  /**
   * @brief   Cirumference of the wheel in µm.
   */
  uint32_t circumference;

  /**
   * @brief   Spacial offset of the wheel from the center of motion in µm.
   */
  uint32_t offset;

  /**
   * @brief   QEI related information.
   */
  struct amiro_dme_qeidescriptor qei;
};

/**
 * @brief   DME configuration structure.
 *
 * @struct amiro_dme_config
 */
typedef struct amiro_dme_config {
  /**
   * @brief   Configuration for the left wheel.
   */
  struct amiro_dme_wheeldescriptor left;

  /**
   * @brief   Configuration for the right wheel.
   */
  struct amiro_dme_wheeldescriptor right;

  /**
   * @brief   Frequency as which to measure QEI input.
   */
  urt_delay_t interval;
} amiro_dme_config_t;

/**
 * @brief   Differential Motion Estimator (DME) structure.
 *
 * @struct amiro_dme
 */
typedef struct amiro_dme {
  /**
   * @brief   Node thread memory.
   */
  URT_THREAD_MEMORY(thread, AMIRO_DME_STACKSIZE);

  /**
   * @brief   Node object.
   */
  urt_node_t node;

  /**
   * @brief   Publisher to privide motion information.
   */
  urt_publisher_t publisher;

  /**
   * @brief   Timer to trigger periodic measurements.
   */
  aos_timer_t trigger;

  /**
   * @brief   QEI related data.
   */
  struct {
    /**
     * @brief   Time of the most recent measurement.
     */
    urt_osTime_t time;
  } qei;

  /**
   * @brief   Pointer to the associated configuration.
   */
  const amiro_dme_config_t* config;

  float callback; //Callback for testing
} amiro_dme_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined (__cplusplus)
extern "C" {
#endif
  void amiroDmeInit(amiro_dme_t* dme, const amiro_dme_config_t* config, urt_topicid_t topic, urt_osThreadPrio_t prio);
#if defined (__cplusplus)
}
#endif

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/******************************************************************************/
/* SUBSYSTEMS                                                                 */
/******************************************************************************/

#endif /* AMIRO_DifferentialMotionEstimator_H */

/** @} */
