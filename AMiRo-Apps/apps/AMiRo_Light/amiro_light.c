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
 * @file    amiro_light.c
 */

#include <amiro_light.h>
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
#define SERVICEEVENT                  (urt_osEventMask_t)(1<< 1)
#define TIMEREVENT                    (urt_osEventMask_t)(1<< 2)      

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
static const char _amiro_light_name[] = "Amiro_Light";

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_amiro_light
 * @{
 */

/**
 * @brief   Setup callback function for light nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] lights  Pointer to the light structure.
 *                    Must not be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _amiro_lightSetup(urt_node_t* node, void* lights)
{
  urtDebugAssert(lights != NULL);
  (void)node;

  // set thread name
  chRegSetThreadName(_amiro_light_name);

  // local constants
  amiro_light_node_t* const light = (amiro_light_node_t*)lights;

  memset(light->driver_data.buffer.data, 0, TLC5947_LLD_BUFFER_SIZE);
  tlc5947_lld_write(light->driver_data.driver, &light->driver_data.buffer);
  tlc5947_lld_update(light->driver_data.driver);

  // set light on
  tlc5947_lld_setBlank(light->driver_data.driver, TLC5947_LLD_BLANK_DISABLE);

  return SERVICEEVENT;
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
urt_osEventMask_t _amiro_lightLoop(urt_node_t* node, urt_osEventMask_t event, void* lights)
{
  urtDebugAssert(lights != NULL);
  (void)node;

  // local constants
  amiro_light_node_t* const light = (amiro_light_node_t*)lights;

  if (event & SERVICEEVENT) {
    // local variables
    urt_service_dispatched_t dispatched;    
    uint8_t value[3];

    // get the request              
    while(urtServiceDispatch(&light->service,
                          &dispatched,
                          &light->values,
                          NULL,
                          NULL)) {
    
      for (uint8_t led = 0; led < NUM_LEDS; led++) {
        value[0] = LIGHT_COLORS[light->values.color[led]][0];
        value[1] = LIGHT_COLORS[light->values.color[led]][1];
        value[2] = LIGHT_COLORS[light->values.color[led]][2];

        for (uint8_t channel = led*3; channel < led*3+3; ++channel) {
          tlc5947_lld_setBuffer(&light->driver_data.buffer, channel, value[channel%3]);
        }
      }
      tlc5947_lld_write(light->driver_data.driver, &light->driver_data.buffer);
      tlc5947_lld_update(light->driver_data.driver);

      // signal the request
      if (dispatched.request != NULL) {
        if (urtServiceTryAcquireRequest(&light->service, &dispatched) == URT_STATUS_OK) {
          urtServiceRespond(&dispatched, 0);
        }
      }
    }
    event &= ~SERVICEEVENT;
  }

  return SERVICEEVENT;
}

/**
 * @brief   Shutdown callback function for light nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] light  Pointer to the light structure.
 *                    Must nor be NULL.
 */
void _amiro_lightShutdown(urt_node_t* node, urt_status_t reason, void* light)
{
  urtDebugAssert(light != NULL);

  (void)node;
  (void)reason;

  // set light off
  tlc5947_lld_setBlank(((amiro_light_node_t*)light)->driver_data.driver, TLC5947_LLD_BLANK_ENABLE);

  return;
}

void amiroLightInit(amiro_light_node_t* light, urt_osThreadPrio_t prio, urt_serviceid_t id)
{
  urtDebugAssert(light != NULL);

  light->driver_data.driver = &moduleLldLedPwm;

  // initialize the node
  urtNodeInit(&light->node, (urt_osThread_t*)light->thread, sizeof(light->thread), prio,
              _amiro_lightSetup, light,
              _amiro_lightLoop, light,
              _amiro_lightShutdown, light);

#if (URT_CFG_RPC_ENABLED == true)
  urtServiceInit(&light->service, id, light->node.thread, SERVICEEVENT);
#endif /* (URT_CFG_RPC_ENABLED == true) */

  return;
}

/** @} */
