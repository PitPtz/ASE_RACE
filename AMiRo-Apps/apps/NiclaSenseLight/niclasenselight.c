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
 * @file    light.c
 */

#include <niclasenselight.h>
#include <amiroos.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

#ifdef BOARD_LIGHTRING_1_2
  #define NUM_LEDS  24
#elif BOARD_LIGHTRING_1_1
  #define NUM_LEDS  8
#endif

/**
 * @brief   Event mask to be set on service events.
 */
#define TIMEREVENT                    (urt_osEventMask_t)(1<< 1)      

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
 * @brief   Name of light nodes.
 */
static const char _light_name[] = "NiclaSenseLight";

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   Callback function for the trigger timer.
 *
 * @param[in] flags   Flags to emit for the trigger event.
 *                    Pointer is reinterpreted as integer value.
 */
static void _nicla_triggercb(virtual_timer_t* timer, void* params)
{
  // local constants
  niclasenselight_node_t* const ns = (niclasenselight_node_t*)params;
//   urtPrintf("callback called");
  // signal node thread to read timer data
  urtEventSignal(ns->node.thread, TIMEREVENT);
  return;
}

//Signal the light service with the new colors for each LED
void signalLightService(niclasenselight_node_t* intro_lf) {
  urtDebugAssert(intro_lf != NULL);
  
  //Try to acquire the light service
  if (urtNrtRequestTryAcquire(&intro_lf->light.request) == URT_STATUS_OK) {
    //Signal the light service
    urtNrtRequestSubmit(&intro_lf->light.request,
                        intro_lf->light.service,
                        sizeof(intro_lf->light.data),
                        0);
  }

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_niclasenselight
 * @{
 */

/**
 * @brief   Setup callback function for niclasenselight nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] lights  Pointer to the light structure.
 *                    Must not be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _niclasenselightSetup(urt_node_t* node, void* lights)
{
  urtDebugAssert(lights != NULL);
  (void)node;
  urtPrintf("Setup called");
  // set thread name
  chRegSetThreadName(_light_name);

  // local constants
  niclasenselight_node_t* const light = (niclasenselight_node_t*)lights;
  aosTimerPeriodicInterval(&light->timer, MICROSECONDS_PER_SECOND , _nicla_triggercb, light);
  return TIMEREVENT;
}

/**
 * @brief   Loop callback function for light nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] lights  Pointer to the light structure.
 *                    Must not be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _niclasenselightLoop(urt_node_t* node, urt_osEventMask_t event, void* lights)
{
  urtDebugAssert(lights != NULL);
  (void)node;

  // local constants
  niclasenselight_node_t* const light = (niclasenselight_node_t*)lights;
  
  // local variables
  light_led_data_t light_data;
  const size_t bytes = sizeof(light_data);
  apalExitStatus_t status;
  urtPrintf("loop called");

  status = niclasense_lld_get(light->niclasense_data.nicla, 
                              light_data.color, bytes, light->niclasense_data.timeout);
  urtPrintf("get called");
  if (status == APAL_STATUS_OK || status == APAL_STATUS_SUCCESS) {
    urtPrintf("Status OK: 0x%08X \n", status); 
  } else if (status == APAL_STATUS_ERROR || status == APAL_STATUS_FAILURE) {
    urtPrintf("Status ERROR: 0x%08X \n", status); 
  } else if (status == APAL_STATUS_TIMEOUT) {
    urtPrintf( "Status TIMEOUT: 0x%08X \n", status); 
  } else if (status == APAL_STATUS_INVALIDARGUMENTS) {
    urtPrintf("Status INVALID: 0x%08X \n", status); 
  }else if (status == APAL_STATUS_UNAVAILABLE) {
    urtPrintf("Status unavailable: 0x%08X \n", status); 
  }else if (status == APAL_STATUS_IO) {
    urtPrintf("Status IO: 0x%08X \n", status); 
  } else {
    urtPrintf("other: 0x%08X \n", status);
  }
  for (uint8_t led = 0; led< 24; led++) {
    urtPrintf("LED:%u Value:%i \n", led, light_data.color[led]);                      
  }
  if (event & TIMEREVENT) {
    for (uint8_t led = 0; led< 24; led++) {
        urtPrintf("LED:%u Value:%i \n", led, light_data.color[led]);     
        light->light.data.color[led] = light_data.color[led];                 
    }

  }
  signalLightService(light);
  return TIMEREVENT;
}

/**
 * @brief   Shutdown callback function for light nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] light  Pointer to the light structure.
 *                    Must nor be NULL.
 */
void _niclasenselightShutdown(urt_node_t* node, urt_status_t reason, void* light)
{
  urtDebugAssert(light != NULL);

  (void)node;
  (void)reason;

  // set light off
//   tlc5947_lld_setBlank(((niclasenselight_node_t*)light)->driver_data.driver, TLC5947_LLD_BLANK_ENABLE);

  return;
}

void niclasenselightInit(niclasenselight_node_t* light, urt_osThreadPrio_t prio, urt_serviceid_t serviceID_Light)
{
  urtDebugAssert(light != NULL);

//   light->driver_data.driver = &moduleLldLedPwm;
  light->light.service = urtCoreGetService(serviceID_Light);
  urtNrtRequestInit(&light->light.request, &light->light.data);
  aosTimerInit(&light->timer);
  light->niclasense_data.nicla = &moduleLldNicla;
  light->niclasense_data.timeout = TIME_INFINITE;

  // initialize the node
  urtNodeInit(&light->node, (urt_osThread_t*)light->thread, sizeof(light->thread), prio,
              _niclasenselightSetup, light,
              _niclasenselightLoop, light,
              _niclasenselightShutdown, light);

// #if (URT_CFG_RPC_ENABLED == true)
//   urtServiceInit(&light->service, id, light->node.thread, TIMEREVENT);
// #endif /* (URT_CFG_RPC_ENABLED == true) */

  return;
}

/** @} */
