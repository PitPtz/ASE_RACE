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
 * @file    L515_trigger.h
 * @brief   Intel RealSense L515 trigger application.
 *
 * @defgroup apps_l515trigger L515 Trigger
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_l515trigger
 * @{
 */

#ifndef L515_TRIGGER_H
#define L515_TRIGGER_H

#include <urt.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#if !defined(L515TRIGGER_STACKSIZE) || defined(__DOXYGEN__)
/**
 * @brief   Stack size of thread.
 */
#define L515TRIGGER_STACKSIZE                256
#endif /* !defined(L515TRIGGER_STACKSIZE) */

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

typedef struct l515trigger_node {
  URT_THREAD_MEMORY(thread, L515TRIGGER_STACKSIZE);

  urt_node_t node;

  struct {
    urt_osTimer_t timer;
    urt_delay_t interval;
  } timer;

  struct {
    apalControlGpio_t* buffer;
    size_t size;
    size_t current;
  } gpios;

} l515trigger_node_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void L515TriggerInit(l515trigger_node_t* tn, urt_osThreadPrio_t prio, apalControlGpio_t* gpios, size_t num_gpios, urt_delay_t interval);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* L515_TRIGGER_H */


/** @} */
