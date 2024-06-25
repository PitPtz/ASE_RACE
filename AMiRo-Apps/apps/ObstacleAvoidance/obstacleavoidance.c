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
 * @file    obstacleavoidance.c
 */

#include <obstacleavoidance.h>
#include <amiroos.h>
#include <stdlib.h>
#include <math.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

#define TIMER_TRIGGEREVENT            (urtCoreGetEventMask() << 1)
#define PROXFLOOREVENT                (urtCoreGetEventMask() << 2)
#define PROXENVEVENT                  (urtCoreGetEventMask() << 3)

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
 * @brief   Name of obstacleavoidance nodes.
 */
static const char _obstacleavoidance_name[] = "ObstacleAvoidance";

/**
 * Obstacle Avoidance Setup
 */
float oa_rpmSpeed[2] = {0};


float namMatrix[8][2] = {
    /*            x     w_z */
    /* NNW */{-0.75f, -1.00f},
    /* WNW */{-0.75f, -0.50f},
    /* WSW */{ 0.25f, -0.25f},
    /* SSW */{ 0.00f,  0.00f},
    /* SSE */{ 0.00f,  0.00f},
    /* ESE */{ 0.25f,  0.25f},
    /* ENE */{-0.75f,  0.50f},
    /* NNE */{-0.75f,  1.00f}
};


/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

//Signal the led light service to update current colors
void oa_signalLightService(void* obstacleavoidance) {
  urtDebugAssert(obstacleavoidance != NULL);

  if (urtNrtRequestTryAcquire(&((obstacleavoidance_node_t*)obstacleavoidance)->light_data.request) == URT_STATUS_OK) {
    urtNrtRequestSubmit(&((obstacleavoidance_node_t*)obstacleavoidance)->light_data.request,
                        urtCoreGetService(((obstacleavoidance_node_t*)obstacleavoidance)->light_data.serviceid),
                        sizeof(((obstacleavoidance_node_t*)obstacleavoidance)->light_data.data),
                        0);
  } else {
    urtPrintf("Could not acquire urt while signaling light service!");
  }
  return;
}

//Signal the service with the new motor data
void oa_signalMotorService(obstacleavoidance_node_t* obstacleavoidance, float trans, float rot) {
  urtDebugAssert(obstacleavoidance != NULL);

  obstacleavoidance->motor_data.data.translation.axes[0] = trans;
  obstacleavoidance->motor_data.data.rotation.vector[2] = rot;

  urt_status_t status = urtNrtRequestTryAcquire(&obstacleavoidance->motor_data.request);
  if (status == URT_STATUS_OK) {
    urtNrtRequestSubmit(&obstacleavoidance->motor_data.request,
                        obstacleavoidance->motor_data.service,
                        sizeof(obstacleavoidance->motor_data.data),
                        0);
  } else {
    urtPrintf("Could not acquire urt while signaling motor service! Reason:%i\n", status);
  }
  return;
}

void oa_getData(void* obstacleavoidance, urt_osEventMask_t event) {
  urtDebugAssert(obstacleavoidance != NULL);

  //local variables
  urt_status_t status;

  if (event & PROXENVEVENT) {
    // fetch NRT of the touch sensor data
    do {
      status = urtNrtSubscriberFetchNext(&((obstacleavoidance_node_t*)obstacleavoidance)->environment_data.nrt,
                                         &((obstacleavoidance_node_t*)obstacleavoidance)->environment_data.data,
                                         &((obstacleavoidance_node_t*)obstacleavoidance)->environment_data.data_size,
                                         NULL, NULL);
    } while (status != URT_STATUS_FETCH_NOMESSAGE);
    event &= ~PROXENVEVENT;
  }

  return;
}

/**
 * @brief   Callback function for the trigger timer.
 *
 * @param[in] flags   Flags to emit for the trigger event.
 *                    Pointer is reinterpreted as integer value.
 */
static void _obstacleavoidance_triggercb(virtual_timer_t* timer, void* params)
{
  // local constants
  obstacleavoidance_node_t* const oan = (obstacleavoidance_node_t*)params;

  // signal node thread to read proximity data
  if (timer == &oan->timer.vt) {
    urtEventSignal(oan->node.thread, TIMER_TRIGGEREVENT);
  }

  return;
}

static uint8_t proxId2LedId(uint8_t proxId) {
    return (7-proxId)*3+1;
    //return (proxId < 4) ? proxId+4 : proxId-4;
}

color_t prox2Color(float prox) {
  /*float p = 0.0f;
  if (prox < 0.5f) {
    p = 2.0f * prox;
    return color_t(0x00, p*0xFF, (1.0f-p)*0xFF);
  } else {
    p = 2.0f * (prox - 0.5f);
    return Color(p*0xFF, (1.0f-p)*0xFF, 0x00);
  }*/
  if (prox > 0.55f) //0.4f)
    return RED;
  if (prox > 0.15f) // 0.17f) //orange
    return ORANGE;
  if (prox > 0.04f)
    return YELLOW;
//  if (prox <= 1.f)
  return GREEN;
}


void obstacleAvoidance(obstacleavoidance_node_t* obstacleavoidance)
{
    urtDebugAssert(obstacleavoidance != NULL);
    
    // Do state action
    switch (obstacleavoidance->state) {
      case OA_IDLE:
        urtThreadSSleep(1);
        break;
      case OBSTACLE_AVOIDANCE:
        ; //empty statement because some compiler doesn't allow decls after labels
        // normalize proximity values
        float proxNormalized[8];
        for (int sensor = 0; sensor < 8; ++sensor) {
          register uint16_t prox = obstacleavoidance->environment_data.data.data[sensor];
          // limit to high treshold
          if (prox > OBSTACLEAVOIDANCE_PROX_THRESHOLD_HIGH)
              prox = OBSTACLEAVOIDANCE_PROX_THRESHOLD_HIGH;
          // limit to low threshold
          else if (prox < OBSTACLEAVOIDANCE_PROX_THRESHOLD_LOW)
              prox = OBSTACLEAVOIDANCE_PROX_THRESHOLD_LOW;
          // apply low threshold
          prox -= OBSTACLEAVOIDANCE_PROX_THRESHOLD_LOW;
          // normalize to [0, 1]
          proxNormalized[sensor] = 1.0f*prox / (OBSTACLEAVOIDANCE_PROX_THRESHOLD_RANGE);
        }
        //urtPrintf("OA activated %.2f - %u\n", proxNormalized[0], obstacleavoidance->environment_data.data.data[0]);
        // map the sensor values to the top LEDs
        for (int sensor = 0; sensor < 8; ++sensor) {
          obstacleavoidance->light_data.data.color[proxId2LedId(sensor)] = prox2Color(proxNormalized[sensor]);
        }
	float factor_x=0.0f;
	float factor_wz=0.0f;
        // evaluate NAM
        for (int sensor = 0; sensor < 8; ++sensor) {
          factor_x += proxNormalized[sensor] * namMatrix[sensor][0];
          factor_wz += proxNormalized[sensor] * namMatrix[sensor][1];
        }
        //-1 < factor < 1
        float x   = OBSTACLEAVOIDANCE_BASE_TRANSLATION + factor_x * OBSTACLEAVOIDANCE_MAX_ADDITIONAL_TRANSLATION;
        float w_z = OBSTACLEAVOIDANCE_BASE_ROTATION    + factor_wz * OBSTACLEAVOIDANCE_MAX_ADDITIONAL_ROTATION;
	urtPrintf("fx %.2f fwz %0.2f x %.2f wz %.2f\n", factor_x, factor_wz, x,w_z);
        //update light
        oa_signalLightService(obstacleavoidance);
        oa_signalMotorService(obstacleavoidance, x, w_z);
        break;
      default:
        break;
    }
}

urt_osEventMask_t _obstacleavoidance_Setup(urt_node_t* node, void* obstacleavoidance)
{
  urtDebugAssert(obstacleavoidance != NULL);
  (void)node;

  // set thread name
  chRegSetThreadName(_obstacleavoidance_name);

#if (URT_CFG_PUBSUB_ENABLED == true)
  //Subscribe to the environment sensor topic
  urt_topic_t* const environment_sensor_topic = urtCoreGetTopic(((obstacleavoidance_node_t*)obstacleavoidance)->environment_data.topicid);
  urtDebugAssert(environment_sensor_topic != NULL);
  urtNrtSubscriberSubscribe(&((obstacleavoidance_node_t*)obstacleavoidance)->environment_data.nrt, environment_sensor_topic, PROXENVEVENT);
#endif /* URT_CFG_PUBSUB_ENABLED == true */

  // Start timer
  aosTimerPeriodicInterval(&((obstacleavoidance_node_t*)obstacleavoidance)->timer, (1.0f / 100.0f) * MICROSECONDS_PER_SECOND + 0.5f,
                         _obstacleavoidance_triggercb, ((obstacleavoidance_node_t*)obstacleavoidance));

  return PROXFLOOREVENT | PROXENVEVENT;
}

/**
 * @brief   Loop callback function for obstacleavoidance nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] obstacleavoidance    Pointer to the maze structure.
 *                    Must nor be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _obstacleavoidance_Loop(urt_node_t* node, urt_osEventMask_t event, void* obstacleavoidance)
{
  urtDebugAssert(obstacleavoidance != NULL);
  (void)node;

  // local constants
  obstacleavoidance_node_t* const oan = (obstacleavoidance_node_t*)obstacleavoidance;
  
  if (event & (PROXENVEVENT | PROXFLOOREVENT)) {
    // get the proximity and ambient data of the ring and floor sensors
    oa_getData(oan, event);
  }

  if (event & TIMER_TRIGGEREVENT) {
    switch (oan->strategy)
    {
        case OA_OFF:
            urtThreadSSleep(1);
            break;
        case OA_ON:
            obstacleAvoidance(obstacleavoidance);
            break;
        default:
            urtDebugAssert(0);
    }
    event &= ~TIMER_TRIGGEREVENT;
  }

  return PROXFLOOREVENT | PROXENVEVENT | TIMER_TRIGGEREVENT;
}

/**
 * @brief   Shutdown callback function for obstacleavoidance nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] reason  Reason of the termination.
 * @param[in] obstacleavoidance  Pointer to the obstacleavoidance structure
 *                           Must not be NULL.
 */
void _obstacleavoidance_Shutdown(urt_node_t* node, urt_status_t reason, void* obstacleavoidance)
{
  urtDebugAssert(obstacleavoidance != NULL);

  (void)node;
  (void)reason;

#if (URT_CFG_PUBSUB_ENABLED == true)
  // unsubscribe from topics
  urtNrtSubscriberUnsubscribe(&((obstacleavoidance_node_t*)obstacleavoidance)->environment_data.nrt);
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_obstacleavoidance
 * @{
 */

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
int obstacleavoidance_ShellCallback_strategy(BaseSequentialStream* stream, int argc, const char* argv[], obstacleavoidance_node_t* oa) {
  
  urtDebugAssert(oa != NULL);
  (void)argc;
  (void)argv;

  bool print_help = (argc < 2) || (argc > 3);
  if (!print_help) {
    int set_value = atoi(argv[1]);
    if (set_value == 0) {
      oa->strategy = (oa_strategy_t) set_value;
      chprintf(stream, "ObstacleAvoidance: set ObstacleAvoidance off.\n");
    } else if (set_value == 1){
      oa->strategy = (oa_strategy_t) set_value;
      chprintf(stream, "ObstacleAvoidance: set ObstacleAvoidance on.\n");
      // Will be activated immediately
      //chprintf(stream, "!You must touch the ? side sensors to start/end the ObstacleAvoidance!\n");
    } else {
      print_help = true;
    }
  }

  if (print_help) {
    chprintf(stream, "Usage: %s STRATEGY\n", argv[0]);
    chprintf(stream, "STRATEGY:\n");
    chprintf(stream, "  Strategy to be set. Must be one of the following:\n");
    chprintf(stream, "  0\n");
    chprintf(stream, "    Deactivate all ObstacleAvoidance.\n");
    chprintf(stream, "  1\n");
    chprintf(stream, "    Activate ObstacleAvoidance.\n");
    return AOS_INVALIDARGUMENTS;
  }
  return AOS_OK;
}
#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */


void obstacleavoidanceInit(obstacleavoidance_node_t* obstacleavoidance,
                    urt_serviceid_t dmc_target_serviceid,
                    urt_serviceid_t led_light_serviceid,
                    urt_topicid_t environment_topicid,
                    urt_osThreadPrio_t prio)
{
  urtDebugAssert(obstacleavoidance != NULL);

  obstacleavoidance->state = OBSTACLE_AVOIDANCE;
  obstacleavoidance->strategy = OA_OFF;

  obstacleavoidance->environment_data.topicid = environment_topicid;

  obstacleavoidance->environment_data.data_size = sizeof(((obstacleavoidance_node_t*)obstacleavoidance)->environment_data.data);

  // initialize the node
  urtNodeInit(&obstacleavoidance->node, (urt_osThread_t*)obstacleavoidance->thread, sizeof(obstacleavoidance->thread), prio,
              _obstacleavoidance_Setup, obstacleavoidance,
              _obstacleavoidance_Loop, obstacleavoidance,
              _obstacleavoidance_Shutdown, obstacleavoidance);

  // initialize service requests
#if (URT_CFG_RPC_ENABLED == true)
  urtNrtRequestInit(&obstacleavoidance->motor_data.request, &obstacleavoidance->motor_data.data);
  urtNrtRequestInit(&obstacleavoidance->light_data.request, &obstacleavoidance->light_data.data);
  obstacleavoidance->motor_data.service = urtCoreGetService(dmc_target_serviceid);
  obstacleavoidance->light_data.serviceid = led_light_serviceid;
#endif /* (URT_CFG_RPC_ENABLED == true) */

  // initialize subscriber
#if (URT_CFG_PUBSUB_ENABLED == true)
  urtNrtSubscriberInit(&obstacleavoidance->environment_data.nrt);
#endif /* URT_CFG_PUBSUB_ENABLED == true */

  // initialize trigger timer
  aosTimerInit(&obstacleavoidance->timer);

  return;
}

/** @} */
