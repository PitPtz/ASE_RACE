/*
AMiRo-OS is an operating system designed for the Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2016..2022  Thomas Schöpping et al.

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
 * @file    aos_timer.h
 * @brief   Timer macros and structures.
 *
 * @addtogroup core_timer
 * @{
 */

#ifndef AMIROOS_TIMER_H
#define AMIROOS_TIMER_H

#include <ch.h>
#include "aos_time.h"

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief   Maximum timer interval that can be set in system ticks.
 */
#define AOS_TIMER_MAX_INTERVAL_ST     TIME_MAX_INTERVAL

/**
 * @brief   Maximum timer interval that can be set in seconds.
 */
#define AOS_TIMER_MAX_INTERVAL_S      (chTimeI2S(AOS_TIMER_MAX_INTERVAL_ST) - 1)

/**
 * @brief   Maximum timer interval that can be set in milliseconds.
 */
#define AOS_TIMER_MAX_INTERVAL_MS     (chTimeI2MS(AOS_TIMER_MAX_INTERVAL_ST) - 1)

/**
 * @brief   Maximum timer interval that can be set in microseconds.
 */
#define AOS_TIMER_MAX_INTERVAL_US     (chTimeI2US(AOS_TIMER_MAX_INTERVAL_ST) - 1)

/**
 * @brief   Minimum timer interval for periodic timers in microseconds.
 */
#define AOS_TIMER_MIN_PRIOD_US        chTimeI2US(CH_CFG_ST_TIMEDELTA)

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

/**
 * @brief   Timer structure.
 */
typedef struct aos_timer {
  /**
   * @brief   ChibiOS virtual timer.
   */
  virtual_timer_t vt;

  /**
   * @brief   Absolute time to trigger.
   */
  aos_timestamp_t triggertime;

  /**
   * @brief   Relative interval for periodic timers.
   * @note    A value of 0 indicates single shot timing.
   */
  aos_longinterval_t interval;

  /**
   * @brief   Pointer to a callback function.
   */
  vtfunc_t callback;

  /**
   * @brief   Pointer to a parameter for the callback function.
   */
  void* cbparam;
} aos_timer_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void aosTimerInit(aos_timer_t* timer);
  void aosTimerSetAbsoluteI(aos_timer_t* timer, const aos_timestamp_t time, vtfunc_t cb, void* par);
  void aosTimerSetIntervalI(aos_timer_t* timer, const aos_interval_t offset, vtfunc_t cb, void* par);
  void aosTimerSetLongIntervalI(aos_timer_t* timer, const aos_longinterval_t offset, vtfunc_t cb, void* par);
  void aosTimerPeriodicIntervalI(aos_timer_t* timer, const aos_interval_t interval, vtfunc_t cb, void* par);
  void aosTimerPeriodicLongIntervalI(aos_timer_t* timer, const aos_longinterval_t interval, vtfunc_t cb, void* par);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/**
 * @brief   Set timer to trigger at an absolute system time.
 *
 * @param[in] timer   Pointer to the timer to set.
 * @param[in] time    Absolute uptime for the timer to trigger.
 * @param[in] cb      Pointer to a callback function to be called.
 *                    This callback function is always executed from locked ISR
 *                    context.
 * @param[in] par     Pointer to a parameter for the callback function.
 *                    Mmay be NULL.
 */
static inline void aosTimerSetAbsolute(aos_timer_t* timer, const aos_timestamp_t time, vtfunc_t cb, void* par)
{
  chSysLock();
  aosTimerSetAbsoluteI(timer, time, cb, par);
  chSysUnlock();

  return;
}

/**
 * @brief   Set timer to trigger after a relative interval.
 *
 * @param[in] timer   Pointer to the timer to set.
 * @param[in] offset  Relative interval to set for the timer to trigger.
 * @param[in] cb      Pointer to a callback function to be called.
 *                    This callback function is always executed from locked ISR
 *                    context.
 * @param[in] par     Pointer to a parameter for the callback function.
 *                    May be NULL.
 */
static inline void aosTimerSetInterval(aos_timer_t* timer, const aos_interval_t offset, vtfunc_t cb, void* par)
{
  chSysLock();
  aosTimerSetIntervalI(timer, offset, cb, par);
  chSysUnlock();
}

/**
 * @brief   Set timer to trigger after a long relative interval.
 *
 * @param[in] timer   Pointer to the timer to set.
 * @param[in] offset  Long interval value to set for the timer to trigger.
 * @param[in] cb      Pointer to a callback function to be called.
 *                    This callback function is always executed from locked ISR
 *                    context.
 * @param[in] par     Pointer to a parameter for the callback function.
 *                    May be NULL.
 */
static inline void aosTimerSetLongInterval(aos_timer_t* timer, const aos_longinterval_t offset, vtfunc_t cb, void* par)
{
  chSysLock();
  aosTimerSetLongIntervalI(timer, offset, cb, par);
  chSysUnlock();
}

/**
 * @brief   Set timer to trigger periodically in the specified interval.
 *
 * @param[in] timer     Pointer to the timer to set.
 * @param[in] interval  Interval for the timer to trigger periodically.
 * @param[in] cb        Pointer to a callback function to be called.
 *                      This callback function is always executed from locked
 *                      ISR context.
 * @param[in] par       Pointer to a parameter for the callback function.
 *                      May be NULL.
 */
static inline void aosTimerPeriodicInterval(aos_timer_t *timer, const aos_interval_t interval, vtfunc_t cb, void *par)
{
  chSysLock();
  aosTimerPeriodicIntervalI(timer, interval, cb, par);
  chSysUnlock();

  return;
}

/**
 * @brief   Set timer to trigger periodically in the specified interval.
 *
 * @param[in] timer     Pointer to the timer to set.
 * @param[in] interval  Long interval value for the periodic timer to trigger
 *                      periodically.
 * @param[in] cb        Pointer to a callback function to be called.
 *                      This callback function is always executed from locked
 *                      ISR context.
 * @param[in] par       Pointer to a parameter for the callback function.
 *                      May be NULL.
 */
static inline void aosTimerPeriodicLongInterval(aos_timer_t* timer, const aos_longinterval_t interval, vtfunc_t cb, void* par)
{
  chSysLock();
  aosTimerPeriodicLongIntervalI(timer, interval, cb, par);
  chSysUnlock();

  return;
}

/**
 * @brief   Determine whether a timer is armed.
 *
 * @param[in] timer   Pointer to the timer to check.
 *
 * @return    true if the timer is armed, false otherwise.
 */
static inline bool aosTimerIsArmedI(aos_timer_t* timer)
{
  return chVTIsArmedI(&(timer->vt));
}

/**
 * @brief   Determine whether a timer is armed.
 *
 * @param[in] timer   Pointer to the timer to check.
 *
 * @return    true if the timer is armed, false otherwise.
 */
static inline bool aosTimerIsArmed(aos_timer_t* timer)
{
  bool b;

  chSysLock();
  b = aosTimerIsArmedI(timer);
  chSysUnlock();

  return b;
}

/**
 * @brief   Reset a timer.
 *
 * @param[in] timer   Pointer to the timer to reset.
 */
static inline void aosTimerResetI(aos_timer_t* timer)
{
  chVTResetI(&(timer->vt));
  timer->interval = 0;

  return;
}

/**
 * @brief   Reset a timer.
 *
 * @param[in] timer   Pointer to the timer to reset.
 */
static inline void aosTimerReset(aos_timer_t* timer)
{
  chSysLock();
  aosTimerResetI(timer);
  chSysUnlock();

  return;
}

#endif /* AMIROOS_TIMER_H */

/** @} */
