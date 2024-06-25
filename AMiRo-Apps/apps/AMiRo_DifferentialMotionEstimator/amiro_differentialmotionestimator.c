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
 * @file    amiro_differentialmotionestimator.c
 * @brief   Motion estimator for differential kinematics.
 */

#include "amiro_differentialmotionestimator.h"

#include <stdlib.h>
#include <math.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

#if !defined(M_PI)
#define M_PI                                      3.14159265358979323846
#endif

/**
 * @brief   Event maks to be set on trigger events.
 */
#define TRIGGEREVENT                              (urtCoreGetEventMask() << 1)

/******************************************************************************/
/* EXPORTED VARIABLES                                                         */
/******************************************************************************/

/******************************************************************************/
/* LOCAL TYPES                                                                */
/******************************************************************************/

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/

#if (CH_CFG_USE_REGISTRY == TRUE) || defined(__DOXYGEN__)
/**
 * @brief   Name of the node thread.
 */
static const char _amiro_dme_name[] = "Amiro_DME";
#endif

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   Callback function for the trigger timer.
 *
 * @param[in] dme   Pointer to the DME instance.
 */
static void _amiro_dmeTriggerCb(virtual_timer_t* timer, void* dme)
{
  (void)timer;

  chEvtSignalI(((amiro_dme_t*)dme)->node.thread, TRIGGEREVENT);
  return;
}

/**
 * @brief   DME setup callback.
 *
 * @param[in] node  Execution node of the DME.
 * @param[in] dme   Pointer to the DME object.
 *
 * @return  Event mask to listen to.
 */
urt_osEventMask_t _amiro_dmeSetup(urt_node_t* node, void* dme)
{
  urtDebugAssert(dme != NULL);
  (void)node;

#if CH_CFG_USE_REGISTRY == TRUE
  // set thread name
  chRegSetThreadName(_amiro_dme_name);
#endif

  // read out both QEIs
  urtTimeNow(&((amiro_dme_t*)dme)->qei.time);
  {
    apalQEIDiff_t qei_delta;
    apalQEIGetDelta(((amiro_dme_t*)dme)->config->left.qei.driver, &qei_delta);
    apalQEIGetDelta(((amiro_dme_t*)dme)->config->right.qei.driver, &qei_delta);
  }

  // activate trigger timer
  aosTimerPeriodicInterval(&((amiro_dme_t*)dme)->trigger, ((amiro_dme_t*)dme)->config->interval, _amiro_dmeTriggerCb, dme);

  return TRIGGEREVENT;
}

/**
 * @brief   DME loop callback.
 *
 * @param[in] node  Execution node of the DME.
 * @param[in] event Received event.
 * @param[in] dme   Pointer to the DME instance.
 *
 * @return  Event mask to listen to.
 */
urt_osEventMask_t _amiro_dmeLoop(urt_node_t* node, urt_osEventMask_t event, void* dme)
{
  urtDebugAssert(dme != NULL);
  (void)node;

  // handle trigger timer event
  while (event & TRIGGEREVENT) {
    // local variables
    apalQEIDiff_t qei_left, qei_right;
    dme_motionpayload_t motion = {
      .translation = {
        .axes = {0.0f},
      },
      .rotation = {
        .vector = {0.0f},
      }
    };

    // get current time
    urt_osTime_t t_measure;
    urtTimeNow(&t_measure);

    /* handle special situation:
     * Two subsequent iterations are executed without measureable temporal
     * interval. Even though not desired, this situation can actually occur
     * and is valid. The reason, however, is NOT that the current iteration is
     * executed too early, but the previous iteration was very late (right
     * before the next timer trigger fired). Hence, there is no information to
     * process in this iterations, thus it is skipped.
     */
    if (!(urtTimeGet(&t_measure) > urtTimeGet(&((amiro_dme_t*)dme)->qei.time))) {
      break;
    }

    // read QEIs
    apalQEIGetDelta(((amiro_dme_t*)dme)->config->left.qei.driver, &qei_left);
    apalQEIGetDelta(((amiro_dme_t*)dme)->config->right.qei.driver, &qei_right);

    // calculate motion
    const urt_delay_t time_diff = urtTimeDiff(&((amiro_dme_t*)dme)->qei.time, &t_measure);
    const float time_factor = (float)time_diff / (float)MICROSECONDS_PER_SECOND;
    const float dist_left = (float)qei_left / (float)((amiro_dme_t*)dme)->config->left.qei.increments_per_revolution * ((float)((amiro_dme_t*)dme)->config->left.circumference / 1e6);
    const float dist_right = (float)qei_right / (float)((amiro_dme_t*)dme)->config->right.qei.increments_per_revolution * ((float)((amiro_dme_t*)dme)->config->right.circumference / 1e6);
    const uint32_t wheel_dist = ((amiro_dme_t*)dme)->config->left.offset + ((amiro_dme_t*)dme)->config->right.offset;
    const float prop_left = (float)((amiro_dme_t*)dme)->config->left.offset / (float)wheel_dist;
    const float prop_right = 1.0f - prop_left;
    const float dist_center = (prop_left * dist_left) + (prop_right * dist_right);
    motion.translation.axes[0] = dist_center / time_factor;
    motion.rotation.vector[2] = atanf((dist_right - dist_left) / (((float)wheel_dist) / 1e6)) / time_factor;

    // publish message
    urtPublisherPublish(&((amiro_dme_t*)dme)->publisher, &motion, sizeof(motion), &t_measure, URT_PUBLISHER_PUBLISH_ENFORCING);

    // update static values
    ((amiro_dme_t*)dme)->qei.time = t_measure;

    break;
  }

  return TRIGGEREVENT;
}

/**
 * @brief @brief   DME shutdown callback.
 *
 * @param[in] node    Execution node of the DME.
 * @param[in] reason  Reason for the shutdown.
 * @param[in] dme     Pointer to the DME instance.
 */
void _amiro_dmeShutdown(urt_node_t* node, urt_status_t reason, void* dme)
{
  urtDebugAssert(dme != NULL);
  (void)node;
  (void)reason;

  // stop trigger timer
  aosTimerReset(&((amiro_dme_t*)dme)->trigger);

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_amiro_dme
 * @{
 */

/**
 * @brief   Differential motion estimator (DME) initialization function.
 *
 * @param[in] dme     DME object to initialize.
 * @param[in] config  Configuration to set for the DME.
 * @param[in] topic   Topic ID to broadcast motion information on.
 * @param[in] prio    Priority of the execution thread.
 */
void amiroDmeInit(amiro_dme_t* dme, const amiro_dme_config_t* config, urt_topicid_t topic, urt_osThreadPrio_t prio)
{
  urtDebugAssert(dme != NULL);
  urtDebugAssert(config != NULL);
  urtDebugAssert(config->left.circumference > 0 &&
                 config->left.qei.driver != NULL &&
                 config->left.qei.increments_per_revolution > 0);
  urtDebugAssert(config->right.circumference > 0 &&
                 config->right.qei.driver != NULL &&
                 config->right.qei.increments_per_revolution > 0);
  urtDebugAssert(config->left.offset + config->right.offset > 0);
  urtDebugAssert(config->interval != URT_DELAY_IMMEDIATE &&
                 config->interval != URT_DELAY_INFINITE);

  // associate the configuration
  dme->config = config;

  // initialize the node
  urtNodeInit(&dme->node, (urt_osThread_t*)dme->thread, sizeof(dme->thread), prio,
              _amiro_dmeSetup, dme,
              _amiro_dmeLoop, dme,
              _amiro_dmeShutdown, dme);

  // intialite trigger timer
  aosTimerInit(&dme->trigger);

  // initialize publisher
  urtPublisherInit(&dme->publisher, urtCoreGetTopic(topic));

  return;
}

/** @} */
