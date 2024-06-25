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
 * @file    aos_timer.c
 * @brief   Timer code.
 * @details Implementation of aos_timer_t and aos_periodictimer_t functions.
 */

#include <amiroos.h>

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

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/*
 * forward declarations
 */
static void _aosTimerIntermediateCb(virtual_timer_t* vt, void* timer);
static void _aosTimerTriggerCb(virtual_timer_t* vt, void *timer);

/**
 * @brief   Setup a timer according to its configuration.
 *
 * @param[in] timer   Pointer to the timer to setup.
 */
void _aosTimerSetup(aos_timer_t* const timer)
{
  // local variables
  aos_timestamp_t uptime;

  // loop required in case of periodic timer that fires immediately (avoid recursion)
  while(true) {
    // get current system uptime
    aosSysGetUptimeX(&uptime);

    // if this is a periodic timer, set trigger time from interval
    if (timer->interval > 0) {
      // if the periodic timer is not initialized yet
      if (aosTimestampGet(timer->triggertime) == 0) {
        aosTimestampSet(timer->triggertime, aosTimestampGet(uptime) + timer->interval);
      }
      // if this is not an intermediate interrupt but a new iteration
      else if (!(aosTimestampDiff(uptime, timer->triggertime) > 0)) {
        aosTimestampAddLongInterval(timer->triggertime, timer->interval);
      }
    }

    // calculate the time delta
    const aos_longinterval_t timedelta = aosTimestampDiff(uptime, timer->triggertime);

    // if the trigger time is more than TIME_IMMEDIATE in the future
    if (timedelta > chTimeI2US(TIME_IMMEDIATE)) {
      // split the time delta if necessary
      if (timedelta > AOS_TIMER_MAX_INTERVAL_US) {
        chVTSetI(&(timer->vt), chTimeUS2I(AOS_TIMER_MAX_INTERVAL_US), _aosTimerIntermediateCb, timer);
        break;
      } else {
        chVTSetI(&(timer->vt), chTimeUS2I(timedelta), _aosTimerTriggerCb, timer);
        break;
      }
    } else {
      // trigger immediately
      timer->callback(NULL, timer->cbparam);
      // reenable periodic timers
      /* Note:
       * Intuitively periodic timers would call this function recursively, but
       * recursion is not desired, therefore the while-loop.
       */
      if (timer->interval == 0) {
        break;
      }
    }
  }

  return;
}

/**
 * @brief   Callback function for intermediate interrupts.
 * @details This is required if the desired time to trigger is too far in the
 *          future so that the interval must be split.
 *
 * @param[in] vt      Pointer to the calling virtual timer.
 * @param[in] timer   Pointer to a aos_timer_t to reactivate.
 */
static void _aosTimerIntermediateCb(virtual_timer_t* vt, void* timer)
{
  (void)vt;

  chSysLockFromISR();
  _aosTimerSetup((aos_timer_t*)timer);
  chSysUnlockFromISR();
}

/**
 * @brief   Callback function for the trigger event of the timer.
 *
 * @param[in] vt      Pointer to the calling virtual timer.
 * @param[in] timer   Pointer to a aos_timer_t to call its callback.
 */
static void _aosTimerTriggerCb(virtual_timer_t* vt, void *timer)
{
  chSysLockFromISR();
  ((aos_timer_t*)timer)->callback(vt, ((aos_timer_t*)timer)->cbparam);
  // reenable periodic timers
  if (((aos_timer_t*)timer)->interval > 0) {
    _aosTimerSetup((aos_timer_t*)timer);
  }
  chSysUnlockFromISR();
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup core_timer
 * @{
 */

/**
 * @brief   Initialize a aos_timer_t object.
 *
 * @param[in] timer   The timer to initialize.
 */
void aosTimerInit(aos_timer_t* timer)
{
  aosDbgAssert(timer != NULL);

  chVTObjectInit(&(timer->vt));
  aosTimestampSet(timer->triggertime, 0);
  timer->interval = 0;
  timer->callback = NULL;
  timer->cbparam = NULL;

  return;
}

/**
 * @brief   Set timer to trigger at an absolute system time.
 *
 * @param[in] timer   Pointer to the timer to set.
 * @param[in] time    Absolute system time for the timer to trigger.
 * @param[in] cb      Pointer to a callback function to be called.
 *                    This callback function is always executed from locked ISR
 *                    context.
 * @param[in] par     Pointer to a parameter for the callback function.
 *                    May be NULL.
 */
void aosTimerSetAbsoluteI(aos_timer_t *timer, const aos_timestamp_t time, vtfunc_t cb, void *par)
{
  aosDbgCheck(timer != NULL);
  aosDbgCheck(cb != NULL);

  timer->triggertime = time;
  timer->interval = 0;
  timer->callback = cb;
  timer->cbparam = par;
  _aosTimerSetup(timer);

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
void aosTimerSetIntervalI(aos_timer_t *timer, const aos_interval_t offset, vtfunc_t cb, void *par)
{
  aosDbgCheck(timer != NULL);
  aosDbgCheck(cb != NULL);

  aos_timestamp_t uptime;

  aosSysGetUptimeX(&uptime);
  aosTimestampSet(timer->triggertime, aosTimestampGet(uptime) + offset);
  timer->interval = 0;
  timer->callback = cb;
  timer->cbparam = par;
  _aosTimerSetup(timer);

  return;
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
void aosTimerSetLongIntervalI(aos_timer_t *timer, const aos_longinterval_t offset, vtfunc_t cb, void *par)
{
  aosDbgCheck(timer != NULL);
  aosDbgCheck(cb != NULL);

  aos_timestamp_t uptime;

  aosSysGetUptimeX(&uptime);
  aosTimestampSet(timer->triggertime, aosTimestampGet(uptime) + offset);
  timer->interval = 0;
  timer->callback = cb;
  timer->cbparam = par;
  _aosTimerSetup(timer);

  return;
}

/**
 * @brief   Set timer to trigger periodically in the specified interval.
 *
 * @param[in] timer     Pointer to the periodic timer to set.
 * @param[in] interval  Interval for the periodic timer to trigger periodically.
 * @param[in] cb        Pointer to a callback function to be called.
 *                      This callback function is always executed from locked
 *                      ISR context.
 * @param[in] par       Pointer to a parameter for the callback function.
 *                      May be NULL.
 */
void aosTimerPeriodicIntervalI(aos_timer_t* timer, const aos_interval_t interval, vtfunc_t cb, void* par)
{
  aosDbgCheck(timer != NULL);
  aosDbgCheck(interval >= AOS_TIMER_MIN_PRIOD_US);
  aosDbgCheck(cb != NULL);

  aosTimestampSet(timer->triggertime, 0);
  timer->interval = interval;
  timer->callback = cb;
  timer->cbparam = par;
  _aosTimerSetup(timer);

  return;
}

/**
 * @brief   Set timer to trigger periodically in the specified interval.
 *
 * @param[in] timer     Pointer to the periodic timer to set.
 * @param[in] interval  Long interval value for the periodic timer to trigger
 *                      periodically.
 * @param[in] cb        Pointer to a callback function to be called.
 *                      This callback function is always executed from locked
 *                      ISR context.
 * @param[in] par       Pointer to a parameter for the callback function.
 *                      May be NULL.
 */
void aosTimerPeriodicLongIntervalI(aos_timer_t* timer, const aos_longinterval_t interval, vtfunc_t cb, void* par)
{
  aosDbgCheck(timer != NULL);
  aosDbgCheck(interval >= AOS_TIMER_MIN_PRIOD_US);
  aosDbgCheck(cb != NULL);

  aosTimestampSet(timer->triggertime, 0);
  timer->interval = interval;
  timer->callback = cb;
  timer->cbparam = par;
  _aosTimerSetup(timer);

  return;
}

/** @} */
