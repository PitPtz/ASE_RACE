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
 * @file    lightfloordata.c
 */

#include <lightfloordata.h>
#include <amiroos.h>
#include <stdlib.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

#define PROXFLOOREVENT                (urtCoreGetEventMask() << 1)

#define NUM_LIGHTS   24

#define BLUE_LIGHT         100
#define GREEN_LIGHT        5000
#define YELLOW_LIGHT       60000

#define BLACK_LIGHT        9000
#define GREY_LIGHT         17000
#define WHITE_LIGHT        25000

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
 * @brief   Name of lightfloordata nodes.
 */
static const char _lightfloordata_name[] = "LightFloorData";

/**
 * @brief Light Floor Data states
 */
typedef enum _lightfloordata_state {
  LFD_OFF,
  LFD_DEFAULT,
  LFD_LINEFOLLOW_VISUALIZATION
}_lightfloordata_state_t;

// state of the light floor data node
_lightfloordata_state_t _lightfloordata_currentstate;

// Boolean to represent whether the lights should shine
bool _lightfloordata_on = false;
// Old color of the LEDs
light_led_data_t _lightfloordata_oldData;


/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

//Signal the led light service to update current colors
void _lightfloordata_signalLightService(lightfloordata_node_t* lightfloordata) {
  urtDebugAssert(lightfloordata != NULL);

  urt_status_t status = urtNrtRequestTryAcquire(&lightfloordata->light_led.request);
  if (status == URT_STATUS_OK) {
    urtNrtRequestSubmit(&lightfloordata->light_led.request,
                        lightfloordata->light_led.service,
                        sizeof(lightfloordata->light_led.data),
                        0);
  } else {
    urtPrintf("Could not acquire urt while signaling light service! Reason: %i\n", status);
  }
  return;
}

void _lightfloordata_getData(lightfloordata_node_t* lfd, urt_osEventMask_t event) {
  urtDebugAssert(lfd != NULL);

  //local variables
  urt_status_t status;
  size_t floor_data_size = sizeof(lfd->floor_prox.data);

  if (event & PROXFLOOREVENT) {
    // fetch NRT of the floor proximity data
    do {
      status = urtNrtSubscriberFetchNext(&lfd->floor_prox.nrt,
                                         &lfd->floor_prox.data.data,
                                         &floor_data_size,
                                         NULL, NULL);
    } while (status != URT_STATUS_FETCH_NOMESSAGE);
  }

  return;
}

/**
 * @brief Sends a service call to set all lights off.
*/
void _lightfloordata_resetLights(lightfloordata_node_t* lightfloordata) {
  // Assume BOARD_LIGHTRING_1_2 with 24 lights
  for (int l = 0; l < 24; l++) {
    lightfloordata->light_led.data.color[l] = OFF;
  }
  _lightfloordata_signalLightService(lightfloordata);
}

urt_osEventMask_t _lightfloordata_Setup(urt_node_t* node, void* lightfloordata)
{
  urtDebugAssert(lightfloordata != NULL);
  (void)node;

  // set thread name
  chRegSetThreadName(_lightfloordata_name);

#if (URT_CFG_PUBSUB_ENABLED == true)
  // subscribe to the floor proximity topic
  urt_topic_t* const floor_prox_topic = urtCoreGetTopic(((lightfloordata_node_t*)lightfloordata)->floor_prox.topicid);
  urtDebugAssert(floor_prox_topic != NULL);
  urtNrtSubscriberSubscribe(&((lightfloordata_node_t*)lightfloordata)->floor_prox.nrt, floor_prox_topic, PROXFLOOREVENT);
#endif /* URT_CFG_PUBSUB_ENABLED == true */

  return PROXFLOOREVENT;
}

/**
 * @brief   Loop callback function for lightfloordata nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] lightfloordata    Pointer to the lightfloordata structure.
 *                              Must not be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _lightfloordata_Loop(urt_node_t* node, urt_osEventMask_t event, void* lightfloordata)
{
  urtDebugAssert(lightfloordata != NULL);
  (void)node;

  // local constants
  lightfloordata_node_t* const lfd = (lightfloordata_node_t*)lightfloordata;

  if (_lightfloordata_on && (event & PROXFLOOREVENT)) {
    // get the proximity data of the floor sensors 
    _lightfloordata_getData(lfd, event);

    // lokal variables
    // 4 floor sensors (0: wheel_left, 1: front_left, 2: front_right, 3: wheel_right) 
    color_t sensor_colors[4];

    switch(_lightfloordata_currentstate) {
      case LFD_DEFAULT: {
        for (int c = 0; c < 4; c++) {
          if (lfd->floor_prox.data.data[c] < BLUE_LIGHT) {
            sensor_colors[c] = BLUE;
          } else if (lfd->floor_prox.data.data[c] < GREEN_LIGHT) {
            sensor_colors[c] = GREEN;
          } else if (lfd->floor_prox.data.data[c] < YELLOW_LIGHT) {
            sensor_colors[c] = YELLOW;
          } else {
            sensor_colors[c] = RED;
          }
        }
        lfd->light_led.data.color[20] = sensor_colors[0];
        lfd->light_led.data.color[23] = sensor_colors[1];
        lfd->light_led.data.color[0] = sensor_colors[2];
        lfd->light_led.data.color[3] = sensor_colors[3];
        break;
      }
      case LFD_LINEFOLLOW_VISUALIZATION: {
          for (int c = 0; c < 4; c++) {
          if (lfd->floor_prox.data.data[c] < BLACK_LIGHT) {
            sensor_colors[c] = BLUE;
          } else if (lfd->floor_prox.data.data[c] < GREY_LIGHT) {
            sensor_colors[c] = TURQUOISE;
          } else if (lfd->floor_prox.data.data[c] < WHITE_LIGHT) {
            sensor_colors[c] = YELLOW;
          } else {
            sensor_colors[c] = RED;
          }
        }
        lfd->light_led.data.color[20] = sensor_colors[0];
        lfd->light_led.data.color[21] = sensor_colors[0];
        lfd->light_led.data.color[22] = sensor_colors[1];
        lfd->light_led.data.color[23] = sensor_colors[1];
        lfd->light_led.data.color[0] = sensor_colors[2];
        lfd->light_led.data.color[1] = sensor_colors[2];
        lfd->light_led.data.color[2] = sensor_colors[3];
        lfd->light_led.data.color[3] = sensor_colors[3];
        break;
      }
      case LFD_OFF:{
        _lightfloordata_resetLights(lfd);
        _lightfloordata_on = false;
        break;
      }
      default:break;
    }

    // Only signal the light service if lights changed. Otherwise the light flicker
    if (memcmp(_lightfloordata_oldData.color, lfd->light_led.data.color, sizeof(_lightfloordata_oldData.color)) != 0) {
      _lightfloordata_signalLightService(lfd);    
    }

    memcpy(_lightfloordata_oldData.color, lfd->light_led.data.color, sizeof(_lightfloordata_oldData.color));
  } 

  return PROXFLOOREVENT;
}

/**
 * @brief   Shutdown callback function for maze nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] reason  Reason of the termination.
 * @param[in] lightfloordata  Pointer to the maze structure
 *                           Must not be NULL.
 */
void _lightfloordata_Shutdown(urt_node_t* node, urt_status_t reason, void* lightfloordata)
{
  urtDebugAssert(lightfloordata != NULL);

  (void)node;
  (void)reason;

#if (URT_CFG_PUBSUB_ENABLED == true)
  // unsubscribe from topics
  urtNrtSubscriberUnsubscribe(&((lightfloordata_node_t*)lightfloordata)->floor_prox.nrt);
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
int lightfloordata_ShellCallback_state(BaseSequentialStream* stream, int argc, const char* argv[], lightfloordata_node_t* lfd) {
  
  urtDebugAssert(lfd != NULL);
  (void)argc;
  (void)argv;

  bool print_help = (argc < 2) || (argc > 3);
  if (!print_help) {
    int set_value = atoi(argv[1]);
    if (set_value == 0) {
      _lightfloordata_currentstate = (_lightfloordata_state_t) set_value;
      chprintf(stream, "LightFloorData: Turn all visualizations off.\n");
    } else if (set_value == 1){
      _lightfloordata_currentstate = (_lightfloordata_state_t) set_value;
      _lightfloordata_on = true;
      chprintf(stream, "LightFloorData: Set state to default visualization.\n");
    } else if (set_value == 2){
      _lightfloordata_currentstate = (_lightfloordata_state_t) set_value;
      _lightfloordata_on = true;
      chprintf(stream, "LightFloorData: Set state to LineFollowing visualization.\n");
    } else {
      print_help = true;
    }
  }
  if (!print_help) {
    _lightfloordata_resetLights(lfd);
  }
  if (print_help) {
    chprintf(stream, "Usage: %s STATE\n", argv[0]);
    chprintf(stream, "Sets the lights of the LightRing according to the floor data values.\nDifferent Visualizations are available:\n");
    chprintf(stream, "STATE:\n");
    chprintf(stream, "  State to be set. Must be one of the following:\n");
    chprintf(stream, "  0\n");
    chprintf(stream, "    Deactivate all visualization.\n");
    chprintf(stream, "  1\n");
    chprintf(stream, "    Activate default visualization.\n");
    chprintf(stream, "  2\n");
    chprintf(stream, "    Activate LineFollowing visualization.\n");
    return AOS_INVALIDARGUMENTS;
  }
  return AOS_OK;
}
#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */

/**
 * @addtogroup apps_lightfloordata
 * @{
 */

void lightfloordataInit(lightfloordata_node_t* lightfloordata,
                        urt_serviceid_t led_light_serviceid,
                        urt_topicid_t proximity_topicid,
                        urt_osThreadPrio_t prio)
{
  urtDebugAssert(lightfloordata != NULL);

  _lightfloordata_currentstate = LFD_OFF;
  _lightfloordata_on = false;

  lightfloordata->floor_prox.topicid = proximity_topicid;

  lightfloordata->floor_prox.data_size = sizeof(((lightfloordata_node_t*)lightfloordata)->floor_prox.data);

  // initialize the node
  urtNodeInit(&lightfloordata->node, (urt_osThread_t*)lightfloordata->thread, sizeof(lightfloordata->thread), prio,
              _lightfloordata_Setup, lightfloordata,
              _lightfloordata_Loop, lightfloordata,
              _lightfloordata_Shutdown, lightfloordata);

  // initialize service requests
#if (URT_CFG_RPC_ENABLED == true)
  urtNrtRequestInit(&lightfloordata->light_led.request, &lightfloordata->light_led.data);
  lightfloordata->light_led.service = urtCoreGetService(led_light_serviceid);
#endif /* (URT_CFG_RPC_ENABLED == true) */

  // initialize subscriber
#if (URT_CFG_PUBSUB_ENABLED == true)
  urtNrtSubscriberInit(&lightfloordata->floor_prox.nrt);
#endif /* URT_CFG_PUBSUB_ENABLED == true */

  return;
}

/** @} */
