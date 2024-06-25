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
 * @file    aos_thread.c
 * @brief   Thread code.
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

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup core_thread
 * @{
 */

/**
 * @brief   Lets the calling thread sleep until the specifide system uptime.
 *
 * @param[in] t     Deadline until the thread will sleep.
 */
void aosThdSleepUntilS(const aos_timestamp_t* t)
{
  aosDbgCheck(t != NULL);

  aos_timestamp_t uptime;

  // get the current system uptime
  aosSysGetUptimeX(&uptime);

  // while the remaining time is too long, it must be split into multiple sleeps
  while (aosTimestampDiff(uptime, *t) > AOS_THD_MAX_SLEEP_US) {
    chThdSleepS(chTimeUS2I(AOS_THD_MAX_SLEEP_US));
    aosSysGetUptimeX(&uptime);
  }

  // sleep the remaining time
  const sysinterval_t rest = chTimeUS2I(aosTimestampDiff(uptime, *t));
  if (rest > TIME_IMMEDIATE) {
    chThdSleepS(rest);
  }

  return;
}

#if ((AMIROOS_CFG_DBG == true) && (CH_DBG_FILL_THREADS == TRUE)) ||             \
    defined(__DOXYGEN__)
/**
 * @brief   Calculate the peak stack utilization for a specific thread so far in
 *          bytes.
 *
 * @param[in] thread    Thread to calculate the stack utilization for.
 *
 * @return  Absolute peak stack utilization in bytes.
 */
size_t aosThdGetStackPeakUtilization(thread_t* thread)
{
  aosDbgCheck(thread != NULL);

  // iterator through the stack
  // note: since the stack is filled from top to bottom, the loop searches for the first irregular byte from the bottom (stack end).
  for (uint8_t* ptr = (uint8_t*)thread->wabase; ptr < (uint8_t*)thread->wabase + aosThdGetStacksize(thread); ++ptr) {
    if (*ptr != CH_DBG_STACK_FILL_VALUE) {
      return aosThdGetStacksize(thread) - (size_t)(--ptr - (uint8_t*)thread->wabase);
    }
  }

  return 0;
}

size_t aosThdGetStackPeakUtilization2(thread_t* thread)
{
  aosDbgCheck(thread != NULL);

  uint32_t num_bytes = 0; 
  for (uint8_t* ptr = (uint8_t*)thread->wabase; ptr < (uint8_t*)thread->wabase + aosThdGetStacksize(thread); ++ptr) { 
      if (*ptr != CH_DBG_STACK_FILL_VALUE) { 
        num_bytes++; 
      } 
  } 
  return num_bytes;
}
#endif /* (AMIROOS_CFG_DBG == true) && (CH_DBG_FILL_THREADS == TRUE) */

#if (CH_CFG_USE_THREADHIERARCHY == TRUE) || defined(__DOXYGEN__)

/**
 * @brief   Retrieve the root thread in the system.
 *
 * @return  Pointer to the root thread.
 */
thread_t* aosThdHierarchyGetRoot(void)
{
  thread_t* tp = chThdGetSelfX();

  chSysLock();
  // move up as long as there is a parent
  while (tp->parent) {
    tp = tp->parent;
  }
  chSysUnlock();

  return tp;
}

/**
 * @brief   Retrieve the next thread in the hierarchy.
 * @details Threads are retrieved depth-first.
 *          If there is an additional idle thread (which is not part of the
 *          hierarchy), it is retrieved last.
 *
 * @param[in] thread  Current thread to retrieve the 'successor' to.
 *
 * @return  Pointer to the next thread, or NULL if there are no further threads.
 */
thread_t* aosThdHierarchyGetNext(thread_t* thread)
{
  aosDbgCheck(thread != NULL);

#if (CH_CFG_NO_IDLE_THREAD == FALSE)
  // idle thread is last (see below)
  if (thread == chSysGetIdleThreadX()) {
    return NULL;
  }
#endif /* (CH_CFG_NO_IDLE_THREAD == FALSE) */

  // iterate through threads in a depth-first approach
  chSysLock();
  if (thread->children != NULL) {
    // if we can go deeper, return the first child.
    thread = thread->children;
    chSysUnlock();
    return thread;
  } else {
    // while there are no siblings, move back up
    while (thread != NULL && thread->sibling == NULL) {
      thread = thread->parent;
    }
    // return next sibling or NULL/idle thread.
#if (CH_CFG_NO_IDLE_THREAD == FALSE)
    // append idle thread explicitely as it is not part of the hierarchy tree
    thread = (thread != NULL) ? thread->sibling : chSysGetIdleThreadX();
#else /* (CH_CFG_NO_IDLE_THREAD == FALSE) */
    thread = (thread != NULL) ? thread->sibling : NULL;
#endif /* (CH_CFG_NO_IDLE_THREAD == FALSE) */
    chSysUnlock();

    return thread;
  }
}

#endif /* (CH_CFG_USE_THREADHIERARCHY == TRUE) */

/** @} */
