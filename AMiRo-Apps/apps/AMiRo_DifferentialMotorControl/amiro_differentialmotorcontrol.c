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
 * @file    amiro_differentialmotorcontrol.c
 * @brief   Motor controller for differential kinematics.
 */

#include "amiro_differentialmotorcontrol.h"

#include <math.h>
#include <stdlib.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

#if !defined(M_PI)
#define M_PI                                      3.14159265358979323846
#endif

/**
 * @brief   Event mask to be set on subscriber events.
 */
#define MOTIONDATAEVENT                           (urtCoreGetEventMask() << 1)

/**
 * @brief   Event mask to be set on service events.
 */
#define TARGETUPDATEEVENT                         (urtCoreGetEventMask() << 2)

#if (DMC_CALIBRATION_ENABLE == true) || defined(__DOXYGEN__)

/**
 * @brief   Event mask to be set on service events.
 */
#define CALIBRATIONEVENT                          (urtCoreGetEventMask() << 3)

#endif /* (DMC_CALIBRATION_ENABLE == true) */

/******************************************************************************/
/* EXPORTED VARIABLES                                                         */
/******************************************************************************/

/******************************************************************************/
/* LOCAL TYPES                                                                */
/******************************************************************************/

#if (DMC_CALIBRATION_ENABLE == true) || defined(__DOXYGEN__)

/**
 * @brief   Data used during auto calibration.
 */
struct calibrationdata {
  /**
   * @brief   Initial delay before actually performing auto calibration (in
   *          seconds).
   */
  float delay;

  /**
   * @brief   Duration of each calibration step (in seconds).
   */
  float interval;

  /**
   * @brief   Timer used during calibration.
   */
  aos_timer_t timer;

  /**
   * @brief   Stream to print status messages to.
   *          May be NULL.
   */
  BaseSequentialStream* stream;
};

#endif /* (DMC_CALIBRATION_ENABLE == true) */

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/

#if (CH_CFG_USE_REGISTRY == TRUE) || defined(__DOXYGEN__)
/**
 * @brief   Name of the node thread.
 */
static const char _amiro_dmc_name[] = "Amiro_DMC";
#endif

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)

/**
 * @brief   Request to be used for shell commands to set target velocity.
 */
urt_nrtrequest_t _amiro_dmc_shelltargetrequest;

/**
 * @brief   Payload for the shell target velocity request.
 */
motor_data_t _amiro_dmc_shelltargetrequestpayload;

#if (DMC_CALIBRATION_ENABLE == true) || defined(__DOXYGEN__)

/**
 * @brief   Request to be used for shell comands to initiate auto calibration.
 */
urt_nrtrequest_t _amiro_dmc_shellcalibrequest;

/**
 * @brief   Payload for the shell calibration request.
 */
struct calibrationdata _amiro_dmc_shellcalibrequestpayload;

#endif /* (DMC_CALIBRATION_ENABLE == true) */

#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__) */

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   Encode multi-DoF motion data to a more efficient representation.
 *
 * @param[out] dst  Variable to store the efficient representation to.
 * @param[in]  src  Source data to be encoded.
 */
void _amiro_dmcEncodeMotion(struct amiro_dmc_motion* dst, const dmc_motionpayload_t* src)
{
  // only x-axis is relevant
  dst->translation = src->translation.axes[0];

  // calculate length of rotation vector (encodes angle in radians)
  dst->rotation = 0.0f;
  for (size_t axis = 0; axis < MOTIONDATA_AXES; ++axis) {
    dst->rotation += src->rotation.vector[axis] * src->rotation.vector[axis];
  }
  dst->rotation = sqrtf(dst->rotation);

  // scale rotation wrt. rotation by z-axis
  if (dst->rotation > 0.0f) {
    dst->rotation *= src->rotation.vector[2] / dst->rotation;
  }

  return;
}

/**
 * @brief   DMC setup callback.
 *
 * @param[in] node  Execution node of the DMC.
 * @param[in] dmc   Pointer to the DMC instance.
 *
 * @return  Event mask to listen to.
 */
urt_osEventMask_t _amiro_dmcSetup(urt_node_t* node, void* dmc)
{
  urtDebugAssert(dmc != NULL);
  (void)node;

#if CH_CFG_USE_REGISTRY == TRUE
  // set thread name
  chRegSetThreadName(_amiro_dmc_name);
#endif

  // subscribe to motion toptic
  {
    // kind of a hack to save memory
    urt_topic_t* const topic = ((amiro_dmc_t*)dmc)->subscriber_motion.topic;
    ((amiro_dmc_t*)dmc)->subscriber_motion.topic = NULL;
    urtHrtSubscriberSubscribe(&((amiro_dmc_t*)dmc)->subscriber_motion, topic, MOTIONDATAEVENT,
                              MICROSECONDS_PER_MILLISECOND, MICROSECONDS_PER_MILLISECOND, 0,
                              NULL, NULL);
  }

#if (DMC_CALIBRATION_ENABLE == true)
  return MOTIONDATAEVENT | TARGETUPDATEEVENT | CALIBRATIONEVENT;
#else
  return MOTIONDATAEVENT | TARGETUPDATEEVENT;
#endif
}

/**
 * @brief   DMC loop callback.
 *
 * @param[in] node  Execution node of the DMC.
 * @param[in] event Received event.
 * @param[in] dmc   Pointer to the MDC instance.
 *
 * @return  Event mask to listen to.
 */
urt_osEventMask_t _amiro_dmcLoop(urt_node_t* node, urt_osEventMask_t event, void* dmc)
{
  urtDebugAssert(dmc != NULL);
  (void)node;

  // handle motion information events
  while (event & MOTIONDATAEVENT) {
    // local variables
    struct amiro_dmc_motion motion;
    urt_osTime_t motion_time;

    // fetch latest motion message
    {
      dmc_motionpayload_t m;
      if (urtHrtSubscriberFetchLatest(&((amiro_dmc_t*)dmc)->subscriber_motion, &m, NULL, &motion_time, NULL) != URT_STATUS_OK) {
        break;
      }
      _amiro_dmcEncodeMotion(&motion, &m);
    }

    // do not calculate the PID if any target speed is invalid
    if (isnan(((amiro_dmc_t*)dmc)->control.target.translation) || isnan(((amiro_dmc_t*)dmc)->control.target.rotation)) {
      // set PWM to neutral state
      apalPWMSet(((amiro_dmc_t*)dmc)->config->motors.left.forward.driver, ((amiro_dmc_t*)dmc)->config->motors.left.forward.channel, APAL_PWM_WIDTH_OFF);
      apalPWMSet(((amiro_dmc_t*)dmc)->config->motors.right.forward.driver, ((amiro_dmc_t*)dmc)->config->motors.right.forward.channel, APAL_PWM_WIDTH_OFF);
      apalPWMSet(((amiro_dmc_t*)dmc)->config->motors.left.reverse.driver, ((amiro_dmc_t*)dmc)->config->motors.left.reverse.channel, APAL_PWM_WIDTH_OFF);
      apalPWMSet(((amiro_dmc_t*)dmc)->config->motors.right.reverse.driver, ((amiro_dmc_t*)dmc)->config->motors.right.reverse.channel, APAL_PWM_WIDTH_OFF);

      // update static values
      ((amiro_dmc_t*)dmc)->control.time = motion_time;
      ((amiro_dmc_t*)dmc)->control.steering.error.last = 0.0f;
      ((amiro_dmc_t*)dmc)->control.left.error.last = 0.0f;
      ((amiro_dmc_t*)dmc)->control.right.error.last = 0.0f;
    }
    // all target speeds are valid -> execute PID
    else {
      // calculate some common values
      const float time_diff = (urtTimeGet(&((amiro_dmc_t*)dmc)->control.time) > 0) ?
                                ((float)urtTimeDiff(&((amiro_dmc_t*)dmc)->control.time, &motion_time) / (float)MICROSECONDS_PER_SECOND) :
                                INFINITY;
      const float lpf_factor = fminf(((amiro_dmc_t*)dmc)->config->lpf.factor * time_diff, 1.0f);



      // calculate steering PID
      const float steering_error = ((amiro_dmc_t*)dmc)->control.target.rotation - motion.rotation;
      const float steering_derror = (steering_error - ((amiro_dmc_t*)dmc)->control.steering.error.last) / time_diff;
      ((amiro_dmc_t*)dmc)->control.steering.error.lpf = (lpf_factor * steering_error) +
                                                  ((1.0f - lpf_factor) * ((amiro_dmc_t*)dmc)->control.steering.error.lpf);
      if (((amiro_dmc_t*)dmc)->config->lpf.max.steering > 0.0f &&
          fabs(((amiro_dmc_t*)dmc)->control.steering.error.lpf) > ((amiro_dmc_t*)dmc)->config->lpf.max.steering) {
        ((amiro_dmc_t*)dmc)->control.steering.error.lpf  = ((amiro_dmc_t*)dmc)->config->lpf.max.steering *
                                                     ((((amiro_dmc_t*)dmc)->control.steering.error.lpf >= 0) ? 1.0f : -1.0f);
      }
      const float u_steering = (((amiro_dmc_t*)dmc)->control.steering.gains.p * steering_error) +
                               (((amiro_dmc_t*)dmc)->control.steering.gains.i * ((amiro_dmc_t*)dmc)->control.steering.error.lpf) +
                               (((amiro_dmc_t*)dmc)->control.steering.gains.d * steering_derror);

      // calculate left motor PID
      const float left_error = (((amiro_dmc_t*)dmc)->control.target.translation - u_steering) - motion.translation;
      const float left_derror = (left_error - ((amiro_dmc_t*)dmc)->control.left.error.last) / time_diff;
      ((amiro_dmc_t*)dmc)->control.left.error.lpf = (lpf_factor * left_error) +
                                              ((1.0f - lpf_factor) * ((amiro_dmc_t*)dmc)->control.left.error.lpf);
      if (((amiro_dmc_t*)dmc)->config->lpf.max.left > 0.0f &&
          fabs(((amiro_dmc_t*)dmc)->control.left.error.lpf) > ((amiro_dmc_t*)dmc)->config->lpf.max.left) {
        ((amiro_dmc_t*)dmc)->control.left.error.lpf  = ((amiro_dmc_t*)dmc)->config->lpf.max.left *
                                                 ((((amiro_dmc_t*)dmc)->control.left.error.lpf >= 0) ? 1.0f : -1.0f);
      }
      float u_left = (((amiro_dmc_t*)dmc)->control.left.gains.p * left_error) +
                     (((amiro_dmc_t*)dmc)->control.left.gains.i * ((amiro_dmc_t*)dmc)->control.left.error.lpf) +
                     (((amiro_dmc_t*)dmc)->control.left.gains.d * left_derror);

      // calculate right motor PID
      const float right_error = (((amiro_dmc_t*)dmc)->control.target.translation + u_steering) - motion.translation;
      const float right_derror = (right_error - ((amiro_dmc_t*)dmc)->control.right.error.last) / time_diff;
      ((amiro_dmc_t*)dmc)->control.right.error.lpf = (lpf_factor * right_error) +
                                               ((1.0f - lpf_factor) * ((amiro_dmc_t*)dmc)->control.right.error.lpf);
      if (((amiro_dmc_t*)dmc)->config->lpf.max.right > 0.0f &&
          fabs(((amiro_dmc_t*)dmc)->control.right.error.lpf) > ((amiro_dmc_t*)dmc)->config->lpf.max.right) {
        ((amiro_dmc_t*)dmc)->control.right.error.lpf  = ((amiro_dmc_t*)dmc)->config->lpf.max.right *
                                                  ((((amiro_dmc_t*)dmc)->control.right.error.lpf >= 0) ? 1.0f : -1.0f);
      }
      float u_right = (((amiro_dmc_t*)dmc)->control.right.gains.p * right_error) +
                      (((amiro_dmc_t*)dmc)->control.right.gains.i * ((amiro_dmc_t*)dmc)->control.right.error.lpf) +
                      (((amiro_dmc_t*)dmc)->control.right.gains.d * right_derror);

      // limit and normalize left and right output
      if (fabs(u_left) > 1.0f || fabs(u_right) > 1.0f) {
        const float reduction_factor = 1.0f / fmaxf(fabs(u_left), fabs(u_right));
        u_left *= reduction_factor;
        u_right *= reduction_factor;
      }

      // set the PWM values
      apalPWMSet(((amiro_dmc_t*)dmc)->config->motors.left.forward.driver, ((amiro_dmc_t*)dmc)->config->motors.left.forward.channel,
                 (u_left > 0.0f) ? (u_left * APAL_PWM_WIDTH_MAX) : APAL_PWM_WIDTH_OFF);
      apalPWMSet(((amiro_dmc_t*)dmc)->config->motors.right.forward.driver, ((amiro_dmc_t*)dmc)->config->motors.right.forward.channel,
                 (u_right > 0.0f) ? (u_right * APAL_PWM_WIDTH_MAX) : APAL_PWM_WIDTH_OFF);
      apalPWMSet(((amiro_dmc_t*)dmc)->config->motors.left.reverse.driver, ((amiro_dmc_t*)dmc)->config->motors.left.reverse.channel,
                 (u_left < 0.0f) ? (-1.0f * u_left * APAL_PWM_WIDTH_MAX) : APAL_PWM_WIDTH_OFF);
      apalPWMSet(((amiro_dmc_t*)dmc)->config->motors.right.reverse.driver, ((amiro_dmc_t*)dmc)->config->motors.right.reverse.channel,
                 (u_right < 0.0f) ? (-1.0f * u_right * APAL_PWM_WIDTH_MAX) : APAL_PWM_WIDTH_OFF);

      // update static values
      ((amiro_dmc_t*)dmc)->control.time = motion_time;
      ((amiro_dmc_t*)dmc)->control.steering.error.last = steering_error;
      ((amiro_dmc_t*)dmc)->control.left.error.last = left_error;
      ((amiro_dmc_t*)dmc)->control.right.error.last = right_error;
    }

    break;
  }

  // handle target velocity update events
  while (event & TARGETUPDATEEVENT) {
    // local variables
    dmc_motionpayload_t motion;

    // dispatch request
    urt_service_dispatched_t dispatched;
    bool new_request;
    new_request = urtServiceDispatch(&((amiro_dmc_t*)dmc)->service_targetspeed, &dispatched, &motion, NULL, NULL);
    if (new_request) {
      // parse payload
      _amiro_dmcEncodeMotion(&((amiro_dmc_t*)dmc)->control.target, &motion);

      // try to acquire and respond the request
      if (dispatched.request != NULL) {
        if (urtServiceTryAcquireRequest(&((amiro_dmc_t*)dmc)->service_targetspeed, &dispatched) == URT_STATUS_OK) {
          urtServiceRespond(&dispatched, sizeof(motion));
        }
      }
    }

    break;
  }

#if (DMC_CALIBRATION_ENABLE == true)
  while (event & CALIBRATIONEVENT) {
    // local variables
    struct calibrationdata calibdata;

    // dispatch request
    urt_service_dispatched_t dispatched;
    urtServiceDispatch(&((amiro_dmc_t*)dmc)->service_calibration, &dispatched, &calibdata, NULL, NULL);
    if (dispatched.request != NULL) {
      // print message
      if (calibdata.stream) {
        chprintf(calibdata.stream, "TODO\n");
      }

      // try to acquire and respond the request
      if (urtServiceAcquireRequest(&((amiro_dmc_t*)dmc)->service_calibration, &dispatched) == URT_STATUS_OK) {
        urtServiceRespond(&dispatched, 0);
      }
    }

    break;
  }
#endif /* (DMC_CALIBRATION_ENABLE == true) */

#if (DMC_CALIBRATION_ENABLE == true)
  return MOTIONDATAEVENT | TARGETUPDATEEVENT | CALIBRATIONEVENT;
#else
  return MOTIONDATAEVENT | TARGETUPDATEEVENT;
#endif
}

/**
 * @brief   DMC shutdown callback.
 *
 * @param[in] node    Execution node of the DMC.
 * @param[in] reason  Reason for the shutdown.
 * @param[in] dmc     Pointer to the DMC instance.
 */
void _amiro_dmcShutdown(urt_node_t* node, urt_status_t reason, void* dmc)
{
  urtDebugAssert(dmc != NULL);
  (void)node;
  (void)reason;

  // unsubscribe from motion topic
  urtHrtSubscriberUnsubscribe(&((amiro_dmc_t*)dmc)->subscriber_motion);

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_amiro_dmc
 * @{
 */

/**
 * @brief   Differential Motor Control (DMC) initialization function.
 *
 * @param[in] dmc                 DMC object to initialize.
 * @param[in] config              Configuration to set for the DMC.
 * @param[in] motiontopic         Topic ID to listen for motion information.
 * @param[in] targetservice       Service ID to listen to target information.
 * @param[in] calibrationservice  Service ID to listen for calibration requests.
 * @param[in] prio                Priority of the execution thread.
 */
#if (DMC_CALIBRATION_ENABLE == true) || defined(__DOXYGEN__)
  void amiroDmcInit(amiro_dmc_t* dmc, const amiro_dmc_config_t* config, urt_topicid_t motiontopic, urt_serviceid_t targetservice, urt_serviceid_t calibrationservice, urt_osThreadPrio_t prio)
#else /* (DMC_CALIBRATION_ENABLE == true) */
  void amiroDmcInit(amiro_dmc_t* dmc, const amiro_dmc_config_t* config, urt_topicid_t motiontopic, urt_serviceid_t targetservice, urt_osThreadPrio_t prio)
#endif /* (DMC_CALIBRATION_ENABLE == true) */
{
  urtDebugAssert(dmc != NULL);
  urtDebugAssert(config != NULL);
  urtDebugAssert(config->motors.left.forward.driver != NULL && config->motors.left.reverse.driver != NULL);
  urtDebugAssert(config->motors.right.forward.driver != NULL && config->motors.right.reverse.driver != NULL);
  urtDebugAssert(config->lpf.factor > 0.0f);
  urtDebugAssert(config->lpf.max.steering >= 0.0f &&
                 config->lpf.max.left >= 0.0f &&
                 config->lpf.max.right >= 0.0f);

  // initialize variables
  dmc->control.target.translation = 0.0f;
  dmc->control.target.rotation = 0.0f;
  urtTimeSet(&dmc->control.time, 0);
  dmc->control.steering.error.last = 0.0f;
  dmc->control.steering.error.lpf = 0.0f;
  dmc->control.steering.gains.p = 0.06f;
  dmc->control.steering.gains.i = 0.0f;
  dmc->control.steering.gains.d = 0.0f;
  dmc->control.left.error.last = 0.0f;
  dmc->control.left.error.lpf = 0.0f;
  dmc->control.left.gains.p = 2.0f;
  dmc->control.left.gains.i = 16.0f;
  dmc->control.left.gains.d = 0.0f;
  dmc->control.right.error.last = 0.0f;
  dmc->control.right.error.lpf = 0.0f;
  dmc->control.right.gains.p = 2.0f;
  dmc->control.right.gains.i = 16.0f;
  dmc->control.right.gains.d = 0.0f;

  // associate the configuration
  dmc->config = config;

  // initialize the node
  urtNodeInit(&dmc->node, (urt_osThread_t*)dmc->thread, sizeof(dmc->thread), prio,
              _amiro_dmcSetup, dmc,
              _amiro_dmcLoop, dmc,
              _amiro_dmcShutdown, dmc);

  // initialize subscriber
  urtHrtSubscriberInit(&dmc->subscriber_motion);
  dmc->subscriber_motion.topic = urtCoreGetTopic(motiontopic); // kind of a hack to save memory

  // initialize service
  urtServiceInit(&dmc->service_targetspeed, targetservice, dmc->node.thread, TARGETUPDATEEVENT);
#if (DMC_CALIBRATION_ENABLE == true)
  urtServiceInit(&dmc->service_calibration, calibrationservice, dmc->node.thread, CALIBRATIONEVENT);
#endif /* (DMC_CALIBRATION_ENABLE == true) */

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
  // initialize request for shell interaction
  urtNrtRequestInit(&_amiro_dmc_shelltargetrequest, &_amiro_dmc_shelltargetrequestpayload);
#if (DMC_CALIBRATION_ENABLE == true)
  aosTimerInit(&_amiro_dmc_shellcalibrequestpayload.timer);
  urtNrtRequestInit(&_amiro_dmc_shellcalibrequest, &_amiro_dmc_shellcalibrequestpayload);
#endif /* (DMC_CALIBRATION_ENABLE == true) */
#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__) */

  return;
}

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)

/**
 * @brief   DMC shell command callback function to set target velocity.
 *
 * @param[in] stream      Shell stream for I/O.
 * @param[in] argc        Number of command arguments.
 * @param[in] argv        Argument list.
 * @param[in] dmc_service Service to use for submitting new target velocity.
 *
 * @return  The operation result.
 */
int dmcShellCallback_setVelocity(BaseSequentialStream* stream, int argc, const char* argv[], urt_service_t* dmc_service)
{
  urtDebugAssert(dmc_service != NULL);

  // if too few arguments
  if (argc < 3) {
    // print help text
    chprintf(stream, "Usage: %s <translation> <rotation>\n", argv[0]);
    chprintf(stream, "translation:\n");
    chprintf(stream, "  Target translation velocity in m/s.\n");
    chprintf(stream, "rotation:\n");
    chprintf(stream, "  Target rotation velocity in rad/s.\n");
    return AOS_INVALIDARGUMENTS;
  }

  // acquire request
  if (urtNrtRequestAcquire(&_amiro_dmc_shelltargetrequest) == URT_STATUS_REQUEST_BADOWNER) {
    urtNrtRequestRetrieve(&_amiro_dmc_shelltargetrequest, URT_REQUEST_RETRIEVE_ENFORCING, NULL, NULL);
  }

  // set payload
  ((dmc_motionpayload_t*)urtNrtRequestGetPayload(&_amiro_dmc_shelltargetrequest))->translation.axes[0] = atof(argv[1]);
  ((dmc_motionpayload_t*)urtNrtRequestGetPayload(&_amiro_dmc_shelltargetrequest))->rotation.vector[2] = atof(argv[2]);

  // submit request
  urtNrtRequestSubmit(&_amiro_dmc_shelltargetrequest, dmc_service, sizeof(dmc_motionpayload_t), 0);

  return AOS_OK;
}

/**
 * @brief   DMC shell command callback function to retrieve PID gains.
 *
 * @param[in] stream  Shell I/O stream.
 * @param[in] argc    Number of arguments.
 * @param[in] argv    Argument list.
 * @param[in] dmc     DMC instance to retrieve gains of.
 *                    Must not be NULL.
 *
 * @return  The operation result.
 */
int dmcShellCallback_getGains(BaseSequentialStream* stream, int argc, const char* argv[], amiro_dmc_t* dmc)
{
  urtDebugAssert(dmc != NULL && dmc->config != NULL);

  (void)argc;
  (void)argv;

  chprintf(stream, "left motor:\n");
  chprintf(stream, "  P: %f\n", dmc->control.left.gains.p);
  chprintf(stream, "  I: %f (anti-windup: ", dmc->control.left.gains.i);
  if (dmc->config->lpf.max.left > 0.0f) {
    chprintf(stream, "%f m/s", dmc->config->lpf.max.left);
  } else {
    chprintf(stream, "disabled");
  }
  chprintf(stream, ")\n");
  chprintf(stream, "  D: %f\n", dmc->control.left.gains.d);
  chprintf(stream, "right motor:\n");
  chprintf(stream, "  P: %f\n", dmc->control.right.gains.p);
  chprintf(stream, "  I: %f (anti-windup: ", dmc->control.right.gains.i);
  if (dmc->config->lpf.max.right > 0.0f) {
    chprintf(stream, "%f m/s", dmc->config->lpf.max.right);
  } else {
    chprintf(stream, "disabled");
  }
  chprintf(stream, ")\n");
  chprintf(stream, "  D: %f\n", dmc->control.right.gains.d);
  chprintf(stream, "steering:\n");
  chprintf(stream, "  P: %f\n", dmc->control.steering.gains.p);
  chprintf(stream, "  I: %f (anti-windup: ", dmc->control.steering.gains.i);
  if (dmc->config->lpf.max.steering > 0.0f) {
    chprintf(stream, "%f rad/s", dmc->config->lpf.max.steering);
  } else {
    chprintf(stream, "disabled");
  }
  chprintf(stream, ")\n");
  chprintf(stream, "  D: %f\n", dmc->control.steering.gains.d);

  return AOS_OK;
}


/**
 * @brief   DMC shell command callback function to set PID gains manually.
 *
 * @param[in] stream  Shell I/O stream.
 * @param[in] argc    Number of arguments.
 * @param[in] argv    Argument list.
 * @param[in] dmc     DMC instance to set gains for.
 *                    Must not be NULL.
 *
 * @return  The operation result.
 */
int dmcShellCallback_setGains(BaseSequentialStream* stream, int argc, const char* argv[], amiro_dmc_t* dmc)
{
  urtDebugAssert(dmc != NULL);

  // local variables
  bool print_help = (argc < 3) || (argc > 5);
  float gains[3] = {NAN};

  // parse arguments to floats
  if (!print_help) {
    for (uint8_t g = 0; g < argc - 2; ++g) {
      gains[g] = atof(argv[g+2]);
      // if the parsed value is invalid, break and print help
      if (!isfinite(gains[g]) || gains[g] < 0.0f) {
        print_help = true;
        break;
      }
    }
  }

  // parse selector argument
  if (!print_help) {
    if (strcmp(argv[1], "-lp") == 0) {
      dmc->control.left.gains.p = gains[0];
    }
    else if (strcmp(argv[1], "-li") == 0) {
      dmc->control.left.gains.i = gains[0];
    }
    else if (strcmp(argv[1], "-ld") == 0) {
      dmc->control.left.gains.d = gains[0];
    }
    else if (strcmp(argv[1], "-rp") == 0) {
      dmc->control.right.gains.p = gains[0];
    }
    else if (strcmp(argv[1], "-ri") == 0) {
      dmc->control.right.gains.i = gains[0];
    }
    else if (strcmp(argv[1], "-rd") == 0) {
      dmc->control.right.gains.d = gains[0];
    }
    else if (strcmp(argv[1], "-sp") == 0) {
      dmc->control.steering.gains.p = gains[0];
    }
    else if (strcmp(argv[1], "-si") == 0) {
      dmc->control.steering.gains.i = gains[0];
    }
    else if (strcmp(argv[1], "-sd") == 0) {
      dmc->control.steering.gains.d = gains[0];
    }
    else if (strcmp(argv[1], "-l") == 0) {
      dmc->control.left.gains.p = gains[0];
      if (argc > 3) {
        dmc->control.left.gains.i = gains[1];
      }
      if (argc > 4) {
        dmc->control.left.gains.d = gains[2];
      }
    }
    else if (strcmp(argv[1], "-r") == 0) {
      dmc->control.right.gains.p = gains[0];
      if (argc > 3) {
        dmc->control.right.gains.i = gains[1];
      }
      if (argc > 4) {
        dmc->control.right.gains.d = gains[2];
      }
    }
    else if (strcmp(argv[1], "-s") == 0) {
      dmc->control.steering.gains.p = gains[0];
      if (argc > 3) {
        dmc->control.steering.gains.i = gains[1];
      }
      if (argc > 4) {
        dmc->control.steering.gains.d = gains[2];
      }
    }
    else {
      print_help = true;
    }
  }

  if (print_help) {
    // print help text
    chprintf(stream, "Usage: %s GAIN VAL...\n", argv[0]);
    chprintf(stream, "GAIN:\n");
    chprintf(stream, "  Gain to set. must be one of the following:\n");
    chprintf(stream, "  -lp, -li, -ld\n");
    chprintf(stream, "    Set P, I or D gain for the left motor.\n");
    chprintf(stream, "  -rp, -ri, -rd\n");
    chprintf(stream, "    Set P, I or D gain for the right motor.\n");
    chprintf(stream, "  -sp, -si, -sd\n");
    chprintf(stream, "    Set P, I or D gain for steering.\n");
    chprintf(stream, "  -l, -r, -s\n");
    chprintf(stream, "    Set all gains for the left motor, the right motor or steering, respectively.\n");
    chprintf(stream, "    The first value of VAL is set as P-gain, the second as I-gain and the third as D-gain.\n");
    chprintf(stream, "    If only one or two values are specified, the unspecified gains remain unchanged.\n");

    return AOS_INVALIDARGUMENTS;
  }

  return AOS_OK;
}

#if (DMC_CALIBRATION_ENABLE == true) || defined(__DOXYGEN__)

/**
 * @brief   DMC shell command callback function to perform auto calibration.
 *
 * @param[in] stream        Shell I/O stream.
 * @param[in] argc          Number of arguments.
 * @param[in] argv          Argument list.
 * @param[in] dmc_service   Service to submit the auto clibration request to.
 *
 * @return  The operation result.
 */
int dmcShellCallback_autoCalibration(BaseSequentialStream* stream, int argc, const char* argv[], urt_service_t* dmc_service)
{
  urtDebugAssert(dmc_service != NULL);

  // check user input and print help text if required
  {
    // local variables
    bool printhelp = (aosShellcmdCheckHelp(argc, argv, true) == AOS_SHELL_HELPREQUEST_EXPLICIT);
    bool argsok = true;

    // check arguments (if any)
    if (!printhelp) {
      if (argc > 1) {
        const float delay = atof(argv[1]);
        argsok = argsok && isfinite(delay) && (delay > 0.0f);
      }
      if (argc > 2) {
        const float interval = atof(argv[2]);
        argsok = argsok && isfinite(interval) && (interval > 0.0f);
      }
    }

    // print help text
    if (printhelp || !argsok) {
      chprintf(stream, "Usage: %s [-help|-h] [DELAY] [INTERVAL]\n", argv[0]);
      chprintf(stream, "--help, -h\n");
      chprintf(stream, "  Print this help text.\n");
      chprintf(stream, "DELAY\n");
      chprintf(stream, "  Time to delay auto calibration in seconds.\n");
      chprintf(stream, "  If not specified, calibration is delayed by 5 seconds.\n");
      chprintf(stream, "INTERVAL\n");
      chprintf(stream, "  Duration of each calibration phase in seconds.\n");
      chprintf(stream, "  During calibration procedure, the system will toggle between active and stationary phases, each taking INTERVAL seconds.\n");
      chprintf(stream, "  If not specified, this value defaults to 0.5 seconds.\n");

      return argsok ? AOS_OK : AOS_INVALIDARGUMENTS;
    }
  }

  // acquire request
  if (urtNrtRequestAcquire(&_amiro_dmc_shellcalibrequest) != URT_STATUS_OK) {
    chprintf(stream, "Calibration currently in progress.\n");
    return AOS_WARNING;
  }

  // set payload (calibration data)
  {
    struct calibrationdata* cd = urtNrtRequestGetPayload(&_amiro_dmc_shellcalibrequest);
    cd->delay = (argc > 1) ? atof(argv[1]) : 5.0f;
    cd->interval = (argc > 2) ? atof(argv[2]) : 0.5f;
    cd->stream = stream;
  }

  // submit calibration request
  urtDebugAssert(AOS_SHELL_EVENTSMASK_COMMAND != 0);
  urt_osEventMask_t eventmask = 1;
  while (eventmask & AOS_SHELL_EVENTSMASK_PROHIBITED) {
    eventmask <<= 1;
  }
  urtNrtRequestSubmit(&_amiro_dmc_shellcalibrequest, dmc_service, sizeof(struct calibrationdata), eventmask);

  // wait for event
  eventmask = chEvtWaitOne(AOS_SHELL_EVENT_OS | eventmask);

  // if an OS event has been received
  if (eventmask == AOS_SHELL_EVENT_OS) {
    // abort the calibration request
    urtNrtRequestRetrieve(&_amiro_dmc_shellcalibrequest, URT_REQUEST_RETRIEVE_ENFORCING, NULL, NULL);
    urtNrtRequestRelease(&_amiro_dmc_shellcalibrequest);
    // signal the shell thread again
    chEvtSignal(chThdGetSelfX(), AOS_SHELL_EVENT_OS);
    // return with an error
    return AOS_ERROR;
  }

  // retrieve request
  urtNrtRequestRetrieve(&_amiro_dmc_shellcalibrequest, URT_REQUEST_RETRIEVE_DETERMINED, NULL, NULL);

  // release request and return
  urtNrtRequestRelease(&_amiro_dmc_shellcalibrequest);
  return AOS_OK;
}

#endif /* (DMC_CALIBRATION_ENABLE == true) */

#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__) */

/** @} */
