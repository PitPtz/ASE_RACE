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
 * @file    apps_urtosal.h
 *
 * @defgroup mw_urt_osal OSAL
 * @ingroup mw_urt
 * @brief   todo
 * @details todo
 *
 * @addtogroup mw_urt_osal
 * @{
 */

#ifndef APPS_URTOSAL_H
#define APPS_URTOSAL_H

/*============================================================================*/
/* VERSION                                                                    */
/*============================================================================*/

/**
 * @brief   The µRT operating system abstraction layer interface major version.
 * @note    Changes of the major version imply incompatibilities.
 */
#define URT_OSAL_VERSION_MAJOR                  1

/**
 * @brief   The µRT operating system abstraction layer interface minor version.
 * @note    A higher minor version implies new functionalty, but all old interfaces are still available.
 */
#define URT_OSAL_VERSION_MINOR                  0

/*============================================================================*/
/* DEPENDENCIES                                                               */
/*============================================================================*/

#include <amiroos.h>
#include <urt.h>

/*============================================================================*/
/* DEBUG                                                                      */
/*============================================================================*/

#if (URT_CFG_DEBUG_ENABLED == true)
  #define urtDebugAssert(condition)               aosDbgAssert(condition)
#else
  #define urtDebugAssert(condition)               (void)(condition)
#endif

/*============================================================================*/
/* MUTEX                                                                      */
/*============================================================================*/

/**
 * @brief   Mutex lock type.
 */
typedef mutex_t urt_osMutex_t;

#define urtMutexInit(mutex)                     chMtxObjectInit(mutex)

#define urtMutexLock(mutex)                     chMtxLock(mutex)

#define urtMutexTryLock(mutex)                  chMtxTryLock(mutex)

#define urtMutexUnlock(mutex)                   chMtxUnlock(mutex)

/*============================================================================*/
/* CONDITION VARIABLE                                                         */
/*============================================================================*/

/**
 * @brief   Condition variable type.
 */
typedef condition_variable_t urt_osCondvar_t;

#define urtCondvarInit(condvar)                 chCondObjectInit(condvar)

#define urtCondvarSignal(condvar)               chCondSignal(condvar)

#define urtCondvarBroadcast(condvar)            chCondBroadcast(condvar)

/*============================================================================*/
/* EVENTS                                                                     */
/*============================================================================*/

/**
 * @brief   Type representing an event source object.
 */
typedef event_source_t urt_osEventSource_t;

/**
 * @brief   Type representing an event listener object.
 */
typedef event_listener_t urt_osEventListener_t;

/**
 * @brief   Type representing event masks.
 * @details Event masks can be used to distinguish and filter events.
 */
typedef eventmask_t  urt_osEventMask_t;

/**
 * @brief   Type representing event flags.
 * @details Event flags can be used to represent rudimentary metadata or to filter events.
 */
typedef eventflags_t urt_osEventFlags_t;

/**
 * @brief   Event mask, which is handled with highest priority.
 */
#define URT_EVENTMASK_MAXPRIO                   EVENT_MASK(0)

/**
 * @brief   Event mask to select all events,
 */
#define URT_EVENTMASK_ALL                       ALL_EVENTS

#define urtEventSourceInit(source)              chEvtObjectInit(source)

#define urtEventListenerInit(listener)          (void)listener

#define urtEventListenerGetFlags(listener)      (urt_osEventFlags_t)(listener->flags)

#define urtEventRegister(source, listener, mask, flags) chEvtRegisterMaskWithFlags(source, listener, mask, flags ? flags : (eventflags_t)-1)

#define urtEventUnregister(source, listener)    chEvtUnregister(source, listener)

/*============================================================================*/
/* STREAMS                                                                    */
/*============================================================================*/

#define urtPrintf(fmt, ...)                     aosprintf(fmt, ##__VA_ARGS__)

#define urtErrPrintf(fmt, ...)                  aosprintf(fmt, ##__VA_ARGS__)

/*============================================================================*/
/* TIME                                                                       */
/*============================================================================*/

/**
 * @brief   The urt_osTime_t represents time at an arbitrary precision.
 */
typedef aos_timestamp_t urt_osTime_t;

#define urtTimeSet(time, us)                  aosTimestampSet(*(time), us)

#define urtTimeGet(time)                      aosTimestampGet(*(time))

#define urtTimeNow(time)                      aosSysGetUptime(time)

#define urtTimeAdd(time, offset)              aosTimestampAddInterval(*(time), offset)

#define urtTimeDiff(from, to)                 aosTimestampDiff(*(from), *(to))

/*============================================================================*/
/* THREAD                                                                     */
/*============================================================================*/

/**
 * @brief   Thread object type.
 */
typedef thread_t urt_osThread_t;

/**
 * @brief   Thread priority data type.
 */
typedef tprio_t urt_osThreadPrio_t;

/**
 * @brief   Minimum thread priority.
 */
#define URT_THREAD_PRIO_LOW_MIN                 AOS_THD_LOWPRIO_MIN

/**
 * @brief   Maximum thread priority for low priority class threads.
 */
#define URT_THREAD_PRIO_LOW_MAX                 AOS_THD_LOWPRIO_MAX

/**
 * @brief   Minimum thread priority for normal priority class threads.
 */
#define URT_THREAD_PRIO_NORMAL_MIN              AOS_THD_NORMALPRIO_MIN

/**
 * @brief   Maximum thread priority for normal priority class threads.
 */
#define URT_THREAD_PRIO_NORMAL_MAX              AOS_THD_NORMALPRIO_MAX

/**
 * @brief   Minimum thread priority for high priority class threads.
 */
#define URT_THREAD_PRIO_HIGH_MIN                AOS_THD_HIGHPRIO_MIN

/**
 * @brief   Maximum thread priority for high priority class threads.
 */
#define URT_THREAD_PRIO_HIGH_MAX                AOS_THD_HIGHPRIO_MAX

/**
 * @brief   Minimum thread priority for real-time class threads.
 */
#define URT_THREAD_PRIO_RT_MIN                  AOS_THD_RTPRIO_MIN

/**
 * @brief   maximum thread priority for real-time class threads.
 */
#define URT_THREAD_PRIO_RT_MAX                  AOS_THD_RTPRIO_MAX

/**
 * @brief   Maximum number of seconds a thread can sleep.
 */
#define URT_THREAD_SLEEP_MAX                    (float)AOS_THD_MAX_SLEEP_S

/**
 * @brief   Maximum number of seconds a thread can sleep.
 */
#define URT_THREAD_SSLEEP_MAX                   (unsigned int)AOS_THD_MAX_SLEEP_S

/**
 * @brief   Maximum number of milliseconds a thread can sleep.
 */
#define URT_THREAD_MSLEEP_MAX                   (unsigned int)AOS_THD_MAX_SLEEP_MS

/**
 * @brief   Maximum number of microseconds a thread can sleep.
 */
#define URT_THREAD_USLEEP_MAX                   (urt_delay_t)AOS_THD_MAX_SLEEP_US

/**
 * @brief   Macro function to layout a thread memory.
 * @details The overall memory required for a thread usually is a combination of the stack and some further data (i.e. the thread object itself).
 *          The layout in memory as well as the metadata may differ between kernels and event hardware ports.
 *
 * @param[in]   varname     The name of the resulting buffer variable.
 * @param[in]   stacksize   The size of the thread's stack.
 */
#define URT_THREAD_MEMORY(varname, stacksize)   THD_WORKING_AREA(varname, stacksize)

#define urtThreadStart(thread)                  chThdStart(thread)

#define urtThreadYield()                        chThdYield()

#define urtThreadGetPriority()                  chThdGetPriorityX()

#define urtThreadSetPriority(prio) {                                              \
  aosDbgCheck(prio >= URT_THREAD_PRIO_LOW_MIN && prio <= URT_THREAD_PRIO_RT_MAX); \
  if (port_irq_enabled(port_get_irq_status())) chThdSetPriority(prio);            \
}

#define urtThreadSleep(seconds)                 aosThdSleep(seconds)

#define urtThreadSSleep(seconds)                aosThdSSleep(seconds)

#define urtThreadMSleep(milliseconds)           aosThdMSleep(milliseconds)

#define urtThreadUSleep(microseconds)           aosThdUSleep(microseconds)

#define urtThreadSleepUntil(time)               aosThdSleepUntil(*time)

#define urtThreadExit()                         chThdExit(MSG_OK)

#define urtThreadShouldTerminate()              chThdShouldTerminateX()

#define urtThreadJoin(thread)                   (void)chThdWait(thread)

#define urtThreadGetSelf()                      (port_is_isr_context() ? NULL : chThdGetSelfX())

/*============================================================================*/
/* TIMER                                                                      */
/*============================================================================*/

/**
 * @brief   Timer object type.
 */
typedef struct {
  aos_timer_t timer;
  urt_osTimerCallback_t callback;
  void* cbparams;
} urt_osTimer_t;

#define urtTimerIsArmed(t)                      aosTimerIsArmed(&(t)->timer)

#endif /* APPS_URTOSAL_H */

/** @} */
