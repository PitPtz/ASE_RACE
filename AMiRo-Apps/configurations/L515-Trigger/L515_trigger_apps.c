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
 * @file    L515_trigger_apps.c
 * @brief   L515 trigger configuration application container.
 */

#include "L515_trigger_apps.h"

#include <l515triggerconf.h>
#include <amiroos.h>
#include <L515_trigger.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED VARIABLES                                                         */
/******************************************************************************/

/******************************************************************************/
/* LOCAL TYPES                                                                */
/******************************************************************************/

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/

l515trigger_node_t _l515trigger_node;

apalControlGpio_t _l515trigger_cgpios[sizeof(l515trigger_gpios) / sizeof(apalGpio_t)];

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup configs_l515trigger
 * @{
 */

/**
 * @brief   Initializes all data and applications for the L515 trigger configuration.
 */
void L515TriggerAppsInit(void)
{
  // configure control GPIOS
  const apalGpioMeta_t meta = {
    .direction = APAL_GPIO_DIRECTION_OUTPUT,
    .active = APAL_GPIO_ACTIVE_LOW,
    .edge = APAL_GPIO_EDGE_NONE,
  };
  for (size_t gpio = 0; gpio < sizeof(l515trigger_gpios)/sizeof(l515trigger_gpios[0]); ++gpio) {
    _l515trigger_cgpios[gpio].gpio = &l515trigger_gpios[gpio];
    _l515trigger_cgpios[gpio].meta = meta;
  }

  // initialize trigger node
  L515TriggerInit(&_l515trigger_node, URT_THREAD_PRIO_RT_MIN, _l515trigger_cgpios, sizeof(_l515trigger_cgpios) / sizeof(_l515trigger_cgpios[0]), L515TRIGGER_CFG_INTERVAL);

  return;
}

/** @} */
