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
 * @file    L515_trigger.c
 * @brief   Intel RealSense L515 trigger application.
 */

#include "L515_trigger.h"

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

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

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

void _l515triggerTimerCallback(void* thd)
{
//  urtEventSignal((urt_osThread_t*)thd, TRIGGEREVENT);
  chEvtSignalI((urt_osThread_t*)thd, TRIGGEREVENT);
}

urt_osEventMask_t _l515triggerSetup(urt_node_t* node, void* tn)
{
  urtDebugAssert(tn != NULL);
  (void)node;

  // local constants
  l515trigger_node_t* const trigger = (l515trigger_node_t*)tn;

  // initialize timer
  urtTimerInit(&trigger->timer.timer);

  // activate first GPIO
  apalControlGpioSet(&trigger->gpios.buffer[trigger->gpios.current], APAL_GPIO_ON);
  aosprintf("GPIO %u active\n", trigger->gpios.current);

  // start timer
  urtTimerSetPeriodic(&trigger->timer.timer, trigger->timer.interval, _l515triggerTimerCallback, trigger->node.thread);

  return TRIGGEREVENT;
}

urt_osEventMask_t _l515triggerLoop(urt_node_t* node, urt_osEventMask_t events, void* tn)
{
  urtDebugAssert(tn != NULL);
  (void)node;

  // local constants
  l515trigger_node_t* const trigger = (l515trigger_node_t*)tn;

  if (events & TRIGGEREVENT) {
    // deactivate current GPIO and activate next GPIO
    apalControlGpioSet(&trigger->gpios.buffer[trigger->gpios.current], APAL_GPIO_OFF);
    trigger->gpios.current = (trigger->gpios.current + 1) % trigger->gpios.size;
    apalControlGpioSet(&trigger->gpios.buffer[trigger->gpios.current], APAL_GPIO_ON);
    // print some information
    urt_osTime_t t;
    urtTimeNow(&t);
    aosprintf("[%u] GPIO %u active\n", (uint32_t)urtTimeGet(&t), trigger->gpios.current);
  }

  return TRIGGEREVENT;
}

void _l515triggerShutdown(urt_node_t* node, urt_status_t reason, void* tn)
{
  urtDebugAssert(tn != NULL);
  (void)node;
  (void)reason;

  // local constants
  l515trigger_node_t* const trigger = (l515trigger_node_t*)tn;

  // deactivate timer
  urtTimerReset(&trigger->timer.timer);

  // deactivate current GPIO
  apalControlGpioSet(&trigger->gpios.buffer[trigger->gpios.current], APAL_GPIO_OFF);

  aosprintf("terminated\n");

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_l515trigger
 * @{
 */

void L515TriggerInit(l515trigger_node_t* tn, urt_osThreadPrio_t prio, apalControlGpio_t* gpios, size_t num_gpios, urt_delay_t interval)
{
  urtDebugAssert(tn != NULL);
  urtDebugAssert(gpios != NULL);
  urtDebugAssert(num_gpios > 0);
  urtDebugAssert(interval > URT_DELAY_IMMEDIATE && interval < URT_DELAY_INFINITE);

  // set configuration data
  tn->gpios.buffer = gpios;
  tn->gpios.size = num_gpios;
  tn->gpios.current = 0;
  tn->timer.interval = interval;

  // initialize the node
  urtNodeInit(&tn->node, (urt_osThread_t*)tn->thread, sizeof(tn->thread), prio,
              _l515triggerSetup, tn,
              _l515triggerLoop, tn,
              _l515triggerShutdown, tn);

  // configure GPIOs
  for (size_t gpio = 0; gpio < num_gpios; ++gpio) {
    osalSysLock();
    palSetLineMode(gpios[gpio].gpio->line, PAL_MODE_OUTPUT_PUSHPULL);
    osalSysUnlock();
    apalControlGpioSet(&gpios[gpio], APAL_GPIO_ON);
  }

  return;
}

/** @} */
