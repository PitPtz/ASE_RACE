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
 * @file    amiro_differentialmotorcontrol.h
 * @brief   Motor controller for differential kinematics.
 *
 * @defgroup apps_amiro_dmc Differential Motor Control
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_amiro_dmc
 * @{
 */

#ifndef AMIRO_DifferentialMotorControl_H
#define AMIRO_DifferentialMotorControl_H

#include <motiondata.h>
#include <amiroos.h>
#include <urt.h>
#include "../../messagetypes/Motor_motordata.h"

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief   Size of the DMC node thread stack.
 */
#define AMIRO_DMC_STACKSIZE                             512

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

#if !defined(DMC_CALIBRATION_ENABLE) || defined(__DOXYGEN__)
/**
 * @brief   Flag to enable auto calibration feature.
 */
#define DMC_CALIBRATION_ENABLE          true
#endif

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
typedef motion_ev_csi dmc_motionpayload_t;

/**
 * @brief   Efficient data structure to represent relevant motions.
 */
struct amiro_dmc_motion {
  /**
   * @brief   Translation along the x-axis in m/s.
   */
  float translation;

  /**
   * @brief   Rotation about the z-axis in rad/s.
   */
  float rotation;
};

/**
 * @brief   PWM descriptor type.
 */
struct amiro_dmc_pwmdescriptor {
  /**
   * @brief   Pointer to the PWM driver.
   */
  apalPWMDriver_t* driver;

  /**
   * @brief   Channel to use at the driver.
   */
  apalPWMchannel_t channel;
};

/**
 * @brief   Motor descriptor type.
 */
struct amiro_dmc_motordescriptor {
  /**
   * @brief   PWM to forward acceleration.
   */
  struct amiro_dmc_pwmdescriptor forward;

  /**
   * @brief   PWM for reverse acceleration.
   */
  struct amiro_dmc_pwmdescriptor reverse;
};

/**
 * @brief   DMC configuration structure.
 *
 * @struct amiro_dmc_config
 */
typedef struct amiro_dmc_config {

  /**
   * @brief   Motors related data.
   */
  struct {
    /**
   * @brief   Configuration for the left motor.
   */
    struct amiro_dmc_motordescriptor left;

    /**
   * @brief   Configuration for the right motor.
   */
    struct amiro_dmc_motordescriptor right;
  } motors;

  /**
   * @brief   Low-pass filer related data.
   */
  struct {
    /**
     * @brief   Factor to add new data points to the low-pass filtered error
     *          history, normalized to a time delta of one second.
     *          Must be positive.
     * @details With each iteration, the low-pass filter 'lpf' is updated with
     *          the latest error value 'e' wrt. the time delta between iterations
     *          'dt' and this factor 'f' according to this formula:
     *            lpf = (max(f * dt, 1) * e) + ((1 - max(f * dt, 1)) * lpf)
     */
    float factor;

    /**
     * @brief   Limiter values for the low-pass filters (anti-windup).
     */
    struct {
      /**
       * @brief   LPF limit for the steering controller in rad/s.
       *          If set to zero, anti-windup is disabled.
       */
      float steering;

      /**
       * @brief   LPF limit for the left motor controller in m/s.
       *          If set to zero, anti-windup is disabled.
       */
      float left;

      /**
       * @brief   LPF limit for the right motor controller in m/s.
       *          If set to zero, anti-windup is disabled.
       */
      float right;
    } max;
  } lpf;

} amiro_dmc_config_t;

/**
 * @brief   PID controller.
 */
struct amiro_dmc_pid {
  /**
   * @brief   Error information.
   */
  struct {
    /**
     * @brief   Error of the last iteration.
     */
    float last;

    /**
     * @brief   Low-pass filtered error.
     */
    float lpf;
  } error;

  struct {
    /**
   * @brief   Gain for the proportional part.
   */
    float p;

    /**
   * @brief   Gain for the integral part.
   */
    float i;

    /**
   * @brief   Gain for the differential part.
   */
    float d;
  } gains;
};

/**
 * @brief   Differential Motor Control (DMC) structure.
 */
typedef struct amiro_dmc {

  /**
   * @brief   Node thread memory.
   */
  URT_THREAD_MEMORY(thread, AMIRO_DMC_STACKSIZE);

  /**
   * @brief   Node object.
   */
  urt_node_t node;

  /**
   * @brief   Subscriber to receive motion information.
   */
  urt_hrtsubscriber_t subscriber_motion;
  dmc_motionpayload_t motion;

  /**
   * @brief   Service to set the target speed.
   */
  urt_service_t service_targetspeed;

#if (DMC_CALIBRATION_ENABLE == true) || defined(__DOXYGEN__)
  urt_service_t service_calibration;
#endif /* (DMC_CALIBRATION_ENABLE == true) */

  /**
   * @brief   Controller related data.
   */
  struct {
    /**
     * @brief   Target motion.
     */
    struct amiro_dmc_motion target;

    /**
     * @brief   Time of the last iteration.
     */
    urt_osTime_t time;

    /**
     * @brief   PID controll for steering.
     */
    struct amiro_dmc_pid steering;

    /**
     * @brief   PID controll for the left motor.
     */
    struct amiro_dmc_pid left;

    /**
     * @brief   PID controll for the right motor.
     */
    struct amiro_dmc_pid right;
  } control;

  /**
   * @brief   Pointer to the associated configuration.
   */
  const amiro_dmc_config_t* config;

  float callback; //callback for testing
} amiro_dmc_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined (__cplusplus)
extern "C" {
#endif
#if (DMC_CALIBRATION_ENABLE == true) || defined(__DOXYGEN__)
  void amiroDmcInit(amiro_dmc_t* dmc, const amiro_dmc_config_t* config, urt_topicid_t motiontopic, urt_serviceid_t targetservice, urt_serviceid_t calibrationservice, urt_osThreadPrio_t prio);
#else /* (DMC_CALIBRATION_ENABLE == true) */
  void amiroDmcInit(amiro_dmc_t* dmc, const amiro_dmc_config_t* config, urt_topicid_t motiontopic, urt_serviceid_t targetservice, urt_osThreadPrio_t prio);
#endif /* (DMC_CALIBRATION_ENABLE == true) */
#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
  int dmcShellCallback_setVelocity(BaseSequentialStream* stream, int argc, const char* argv[], urt_service_t* dmc_service);
  int dmcShellCallback_getGains(BaseSequentialStream* stream, int argc, const char* argv[], amiro_dmc_t* dmc);
  int dmcShellCallback_setGains(BaseSequentialStream* stream, int argc, const char* argv[], amiro_dmc_t* dmc);
#if (DMC_CALIBRATION_ENABLE == true) || defined(__DOXYGEN__)
  int dmcShellCallback_autoCalibration(BaseSequentialStream* stream, int argc, const char* argv[], urt_service_t* dmc_service);
#endif /* (DMC_CALIBRATION_ENABLE == true) */
#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */
#if defined (__cplusplus)
}
#endif

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/******************************************************************************/
/* SUBSYSTEMS                                                                 */
/******************************************************************************/

#endif /* AMIRO_DifferentialMotorControl_H */

/** @} */
