/*
µRT is a lightweight publish-subscribe & RPC middleware for real-time
applications. It was developed as part of the software habitat for the
Autonomous Mini Robot (AMiRo) but can be used for other purposes as well.

Copyright (C) 2018..2022  Thomas Schöpping et al.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    urt_osal.h
 * @brief   µRT operating system abstraction layer (SOAL) definition.
 *
 * @defgroup kernel_osal OSAL
 * @ingroup kernel
 * @brief   Operating system abstraction layer (OSAL).
 * @details µRT OSAL specifies an API for interaction with the operating system.
 *          That API must be implemented by the software, which employs µRT.
 *
 * @addtogroup kernel_osal
 * @{
 */

#ifndef URT_OSAL_H
#define URT_OSAL_H

#include <urt.h>
#include <stdint.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/* VERSION ********************************************************************/

/*
 * The following macros must be defined by the implementation:
 */
#if defined(__DOXYGEN__)
/**
 * @brief   Major version of the implemented OSAL.
 * @note    Values that conflict with the requirements (see urt.h) are
 *          incompatible.
 */
#define URT_OSAL_VERSION_MAJOR

/**
 * @brief   Minor version of the implemented OSAL.
 * @note    Values smaller than the requirements (see urt.h) are incompatible.
 */
#define URT_OSAL_VERSION_MINOR
#endif

/* EVENTS *********************************************************************/

/*
 * The following constants must be defined by the implementation:
 */
#if defined(__DOXYGEN__)
/**
 * @brief   Constant of type urt_osEventMask_t to specify the event mask that is
 *          handled with maximum priority.
 * @details This constant is typically either 0x0..1 (low values have high
 *          priority) or 0x8..0 (high values have high priority).
 *          A value of 0 indicates that priorities are not supported.
 */
#define URT_EVENTMASK_MAXPRIO

/**
 * @brief   Constant of type urt_osEventMask_t to filter no events at all.
 *          Typically is 0xF..F.
 */
#define URT_EVENTMASK_ALL
#endif

/* THREAD *********************************************************************/

/*
 * The following constants must be defined by the implementation:
 */
#if defined(__DOXYGEN__)
/**
 * @brief   Constant of type urt_osThreadPrio_t, specifying the minimum priority
 *          for low-priority threads.
 *          Should be greater than the idle thread priority.
 */
#define URT_THREAD_PRIO_LOW_MIN

/**
 * @brief   Constant of type urt_osThreadPrio_t, specifying the maximum priority
 *          for low-priority threads.
 *          Should be greater or equal URT_THREAD_PRIO_LOW_MIN.
 */
#define URT_THREAD_PRIO_LOW_MAX

/**
 * @brief   Constant of type urt_osThreadPrio_t, specifying the minimum priority
 *          for normal-priority threads.
 *          Should be greater than URT_THREAD_PRIO_LOW_MAX.
 */
#define URT_THREAD_PRIO_NORMAL_MIN

/**
 * @brief   Constant of type urt_osThreadPrio_t, specifying the maximum priority
 *          for normal-priority threads.
 *          Should be greater or equal URT_THREAD_PRIO_NORMAL_MIN.
 */
#define URT_THREAD_PRIO_NORMAL_MAX

/**
 * @brief   Constant of type urt_osThreadPrio_t, specifying the minimum priority
 *          for high-priority threads.
 *          Should be greater than URT_THREAD_PRIO_NORMAL_MAX.
 */
#define URT_THREAD_PRIO_HIGH_MIN

/**
 * @brief   Constant of type urt_osThreadPrio_t, specifying the maximum priority
 *          for high-priority threads.
 *          Should be greater or equal URT_THREAD_PRIO_HIGH_MIN.
 */
#define URT_THREAD_PRIO_HIGH_MAX

/**
 * @brief   Constant of type urt_osThreadPrio_t, specifying the minimum priority
 *          for real-time threads.
 *          Should be greater than URT_THREAD_PRIO_HIGH_MAX.
 */
#define URT_THREAD_PRIO_RT_MIN

/**
 * @brief   Constant of type urt_osThreadPrio_t, specifying the maximum priority
 *          for real-time threads.
 *          Should be greater or equal URT_THREAD_PRIO_HIGH_MIN but lower than
 *          the control thread priority.
 */
#define URT_THREAD_PRIO_RT_MAX

/**
 * @brief   Constant of type float specifying the maximum time (in seconds) a
 *          thread may sleep via the urtThreadSleep() function.
 */
#define URT_THREAD_SLEEP_MAX

/**
 * @brief   Constant of type urt_delay_t specifying the maximum number of
 *          seconds a thread may sleep via the urtThreadSSleep() function.
 */
#define URT_THREAD_SSLEEP_MAX

/**
 * @brief   Constant of type urt_delay_t specifying the maximum number of
 *          milliseconds a thread may sleep via the urtThreadMSleep() function.
 */
#define URT_THREAD_MSLEEP_MAX

/**
 * @brief   Constant of type urt_delay_t specifying the maximum number of
 *          microseconds a thread may sleep via the urtThreadUSleep() function.
 */
#define URT_THREAD_USLEEP_MAX
#endif

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

/* MUTEX **********************************************************************/

/*
 * The following type must be defined by the implementation:
 */
#if defined(__DOXYGEN__)
/**
 * @brief   Type to represent a mutex lock object.
 * @note    Is only used via pointers by the API.
 */
struct urt_osMutex_t {};
#endif

/* CONDITION VARIABLE *********************************************************/

/*
 * The following type must be defined by the implementation:
 */
#if defined(__DOXYGEN__)
/**
 * @brief   Type to represent a condition variable object.
 * @note    Is only used via pointers by the API.
 */
struct urt_osCondvar_t {};
#endif

/**
 * @brief   Status type of condition variables.
 */
typedef enum {
  URT_CONDVAR_WAITSTATUS_SIGNAL = 1,    /**< The condition variable has been signaled individually. */
  URT_CONDVAR_WAITSTATUS_BROADCAST = 2, /**< The condition variable has been signal via broadcast. */
  URT_CONDVAR_WAITSTATUS_TIMEOUT = 0,   /**< Waiting for the condition variable timed out. */
} urt_osCondvarWaitStatus_t;

/* EVENTS *********************************************************************/

/*
 * The following types must be defined by the implementation:
 */
#if defined(__DOXYGEN__)
/**
 * @brief   Type to represent an event source object.
 *          Is only used via pointers by the API.
 */
struct urt_osEventSource_t {};

/**
 * @brief   Type to represent an event listener object.
 *          Is only used via pointers by the API.
 */
struct urt_osEventListener_t {};

/**
 * @brief   Integer type to represent event masks.
 */
struct urt_osEventMask_t {};

/**
 * @brief   Integer type to represent event flags.
 */
struct urt_osEventFlags_t {};
#endif

/**
 * @brief   Enum type to distinguish differnt variants how to wait for events.
 */
typedef enum {
  URT_EVENT_WAIT_ONE = 0,   /**< Wait for exactly one of the specified events. */
  URT_EVENT_WAIT_ANY = 1,   /**< Wait for any (one or more) of the specified events. */
  URT_EVENT_WAIT_ALL = 2,   /**< Wait for all of the specified events. */
} urt_osEventWait_t;

/* TIME ***********************************************************************/

/*
 * The following type must be defined by the implementation:
 */
#if defined(__DOXYGEN__)
/**
 * @brief   Type to represent a time object.
 *          Internal time representation may be of any precision.
 * @note    Is only used via pointers by the API.
 */
struct urt_osTime_t {};
#endif

/* THREAD *********************************************************************/

/*
 * The following types must be defined by the implementation:
 */
#if defined(__DOXYGEN__)
/**
 * @brief   Type to represent an thread object.
 * @note    Is only used via pointers by the API.
 */
struct urt_osThread_t {};

/**
 * @brief   Integer type to represent thread priorities.
 */
struct urt_osThreadPrio_t {};
#endif

/**
 * @brief   Enum type for various thread states.
 */
typedef enum {
  URT_THREAD_STATE_INACTIVE = 0,    /**< The thread has not been started yet. */
  URT_THREAD_STATE_RUNNING = 1,     /**< The thread is currently being executed on a CPU. */
  URT_THREAD_STATE_READY = 2,       /**< The thread is ready to be scheduled for execution. */
  URT_THREAD_STATE_SLEEPING = 3,    /**< The thread is currently sleeping for a defined amount of time. */
  URT_THREAD_STATE_SUSPENDED = 4,   /**< Execution of the thread has been suspended. */
  URT_THREAD_STATE_WAITING = 5,     /**< The thread is currently waiting for an event. */
  URT_THREAD_STATE_TERMINATED = 6,  /**< The thread has been terminated. */
} urt_osThreadState_t;

/**
 * @brief   Enum type for various termination signals.
 */
typedef enum {
  URT_THREAD_TERMINATE_REQUEST = 15,  /**< Request a thread to terminate in a cooperative manner. */
  URT_THREAD_TERMINATE_KILL = 9,      /**< Terminate a thread in a non-cooperative manner (force). */
} urt_osThreadTerminateSignal_t;

/**
 * @brief   Thread function.
 *
 * @param[in] arg   Pointer to optional arguments.
 *                  May be NULL.
 */
typedef void (*urt_osThreadFunction_t)(void* arg);

/* TIMER **********************************************************************/

/*
 * The following type must be defined by the implementation:
 */
#if defined(__DOXYGEN__)
/**
 * @brief   Type to represent a timer object.
 * @note    Is only used via pointers by the API.
 */
struct urt_osTimer_t {};
#endif

/**
 * @brief   Timer callback function type.
 * @details A timer executes a callback when its time ran out and it fires.
 *
 * @param[in] parameter Pointer to optional parameters for the callback function.
 *                      May be NULL.
 */
typedef void (*urt_osTimerCallback_t)(void* parameter);

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/* THREAD *********************************************************************/

/*
 * The following macro must be defined by the implementation:
 */
#if defined(__DOXYGEN__)
/**
 * @brief   Macro function to staically allocate a variable 'varname', which
 *          holds all memory required to hold a thread object with the specified
 *          stacksize.
 */
#define URT_THREAD_MEMORY(varname, stacksize)
#endif

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

// load OSAL implementation header, if specified
#if defined(URT_CFG_OSAL_HEADER)
# define __include(file) <file>
# define _include(file) __include(file)
# include _include(URT_CFG_OSAL_HEADER)
# undef _include
# undef __include
#endif

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

/* DEBUG **********************************************************************/

#if !defined(urtDebugAssert) || defined(__DOXYGEN__)
  /**
   * @brief Assert function to check a given condition.
   *
   * @param[in] condition   The condition to check.
   */
  void urtDebugAssert(const bool condition);
#endif /* !defined(urtDebugAssert) */

/* MUTEX **********************************************************************/

#if !defined(urtMutexInit) || defined(__DOXYGEN__)
  /**
   * @brief   Initialize a mutex lock object.
   *
   * @param[in] mutex   Pointer to the mutex lock object to initialize.
   *                    Must not be NULL.
   */
  void urtMutexInit(urt_osMutex_t* mutex);
#endif /* !defined(urtMutexInit) */

#if !defined(urtMutexLock) || defined(__DOXYGEN__)
  /**
   * @brief   Lock a mutex lock.
   * @details The calling thread is blocked until the mutex lock could be acquired.
   *
   * @param[in] mutex   Pointer to the mutex lock to acquire.
   *                    Must not be NULL.
   */
  void urtMutexLock(urt_osMutex_t* mutex);
#endif /* !defined(urtMutexLock) */

#if !defined(urtMutexTryLock) || defined(__DOXYGEN__)
  /**
   * @brief   Try to lock a mutex lock.
   * @details This function does not block but returns immediately.
   *
   * @param[in] mutex   Pointer to the mutex lock to acquire.
   *
   * @return  Indicator whether the lock was acquired.
   * @retval true   The lock was acquired successfully.
   * @retval false  The lock could not be acquired.
   */
  bool urtMutexTryLock(urt_osMutex_t* mutex);
#endif /* !defined(urtMutexTryLock) */

#if !defined(urtMutexUnlock) || defined(__DOXYGEN__)
  /**
   * @brief   Unlock an owned mutex lock.
   *
   * @param[in] mutex   Pointer to the mutex lock to unlock.
   *                    Must not be NULL.
   */
  void urtMutexUnlock(urt_osMutex_t* mutex);
#endif /* !defined(urtMutexUnlock) */

/* CONDITION VARIABLE *********************************************************/

#if !defined(urtCondvarInit) || defined(__DOXYGEN__)
  /**
   * @brief   Initialize a condition variable object.
   *
   * @param[in] condvar   Pointer to the object to initialize.
   *                      Must not be NULL.
   */
  void urtCondvarInit(urt_osCondvar_t* condvar);
#endif /* !defined(urtCondvarInit) */

#if !defined(urtCondvarSignal) || defined(__DOXYGEN__)
  /**
   * @brief   Signal a condition variable (wake one waiting thread).
   *
   * @param[in] condvar   Pointer to the condition variable to signal.
   *                      Must not be NULL.
   */
  void urtCondvarSignal(urt_osCondvar_t* condvar);
#endif /* !defined(urtCondvarSignal) */

#if !defined(urtCondvarBroadcast) || defined(__DOXYGEN__)
  /**
   * @brief   Signal a condition variable via broadcast (wake all waiting threads).
   *
   * @param[in] condvar   Pointer to the condition variable to signal via broadcast.
   *                      Must not be NULL.
   */
  void urtCondvarBroadcast(urt_osCondvar_t* condvar);
#endif /* !defined(urtCondvarBroadcast) */

#if (!defined(urtCondvarWait) && (URT_CFG_OSAL_CONDVAR_TIMEOUT == true)) || defined(__DOXYGEN__)
  /**
   * @brief   Wait for a condition variable to be signaled or timeout.
   *
   * @param[in] condvar   Pointer to the condition variabel to wait for.
   *                      Must not be NULL.
   * @param[in] mutex     Pointer to the associated mutex lock.
   *                      Must not be NULL.
   *                      Mutex must be acquired when this function is called.
   * @param[in] timeout   Timeout (in microseconds) when the function will return even without the condition variable being signaled.
   *                      Use URT_DELAY_INFINITE to deactivate timeout.
   * @return
   */
  urt_osCondvarWaitStatus_t urtCondvarWait(urt_osCondvar_t* condvar, urt_osMutex_t* mutex, urt_delay_t timeout);
#else /* !defined(urtCondvarWait) && (URT_CFG_OSAL_CONDVAR_TIMEOUT == true) */
  urt_osCondvarWaitStatus_t urtCondvarWait(urt_osCondvar_t* condvar, urt_osMutex_t* mutex);
#endif /* !defined(urtCondvarWait) && (URT_CFG_OSAL_CONDVAR_TIMEOUT == true) */

/* EVENTS *********************************************************************/

#if !defined(urtEventSignal) || defined(__DOXYGEN__)
  /**
   * @brief   Signal a specific thread with a event mask.
   *
   * @param[in] thread  Pointer to the thread to be signaled.
   *                    Must not be NULL.
   * @param[in] mask    Mask to be set at the signaled thread.
   */
  void urtEventSignal(urt_osThread_t* thread, urt_osEventMask_t mask);
#endif /* !defined(urtEventSignal) */

#if !defined(urtEventSourceInit) || defined(__DOXYGEN__)
  /**
   * @brief   Initialize an event source object.
   *
   * @param[in] source  Pointer to the event source object to initialize.
   *                    Must not be NULL.
   */
  void urtEventSourceInit(urt_osEventSource_t* source);
#endif /* !defined(urtEventSourceInit) */

#if !defined(urtEventSourceBroadcast) || defined(__DOXYGEN__)
  /**
   * @brief   Broadcast an event with specified flags.
   *
   * @param[in] source  Pointer to the event source to broadcast the event.
   *                    Must not be NULL.
   * @param[in] flags   Flags to be set at the receiveing listeners.
   */
  void urtEventSourceBroadcast(urt_osEventSource_t* source, urt_osEventFlags_t flags);
#endif /* !defined(urtEventSourceBroadcast) */

#if !defined(urtEventListenerInit) || defined(__DOXYGEN__)
  /**
   * @brief   Initialize an event listener object.
   *          Must not be NULL.
   *
   * @param[in] listener  Pointer to the event listener object to initialize.
   */
  void urtEventListenerInit(urt_osEventListener_t* listener);
#endif /* !defined(urtEventListenerInit) */

#if !defined(urtEventListenerGetFlags) || defined(__DOXYGEN__)
  /**
   * @brief   Retrieve the currently set flags of an event listener.
   * @details Flags are not cleared in the listener.
   *
   * @param[in] listener  Pointer to the event listener to receive the flags from.
   *                      Must not be NULL.
   *
   * @return  Currently set flags.
   */
  urt_osEventFlags_t urtEventListenerGetFlags(urt_osEventListener_t* listener);
#endif /* !defined(urtEventListenerGetFlags) */

#if !defined(urtEventListenerClearFlags) || defined(__DOXYGEN__)
  /**
   * @brief   Retrieve and clear the currently set flags from an event listener.
   *
   * @param[in] listener  Pointer to the event listener to receive the flags from.
   *                      Must not be NULL.
   * @param[in] flags     Flags to be cleared.
   *
   * @return  The flags that had been set at the listener but have been cleared.
   */
  urt_osEventFlags_t urtEventListenerClearFlags(urt_osEventListener_t* listener, urt_osEventFlags_t flags);
#endif /* !defined(urtEventListenerClearFlags) */

#if !defined(urtEventRegister) || defined(__DOXYGEN__)
  /**
   * @brief   Register an event source to a listener.
   *
   * @param[in] source    Pointer to the event source to register.
   *                      Must not be NULL.
   * @param[in] listener  Pointer to the event listener to register.
   *                      Must not be NULL.
   * @param[in] mask      Mask to be set at the listening thread on an event.
   * @param[in] flags     Flags to be set by default at the listener on an event,
   */
  void urtEventRegister(urt_osEventSource_t* source, urt_osEventListener_t* listener, urt_osEventMask_t mask, urt_osEventFlags_t flags);
#endif /* !defined(urtEventRegister) */

#if !defined(urtEventUnregister) || defined(__DOXYGEN__)
  /**
   * @brief   Unregister an associated event source and listener.
   *
   * @param[in] source    Pointer to the event source to unregister.
   *                      Must not be NULL.
   * @param[in] listener  Pointer to the event listener to unregister.
   *                      Must not be NULL.
   */
  void urtEventUnregister(urt_osEventSource_t* source, urt_osEventListener_t* listener);
#endif /* !defined(urtEventUnregister) */

#if !defined(urtEventWait) || defined(__DOXYGEN__)
  /**
   * @brief   Wait for one/any/all event(s) to be received or a timeout.
   *
   * @param[in] mask      The mask of event to wait for.
   * @param[in] type      Specificator whether to wait for exactly one, any combination or all of the specified mask flags.
   * @param[in] timeout   Timeout when the function shall return even if no event was received.
   *
   * @return  The received event mask.
   */
  urt_osEventMask_t urtEventWait(urt_osEventMask_t mask, urt_osEventWait_t type, urt_delay_t timeout);
#endif /* !defined(urtEventWait) */

/* STREAMS ********************************************************************/

#if !defined(urtPrintf) || defined(__DOXYGEN__)
  /**
   * @brief   Prints a formatted string to standard output.
   *
   * @param[in] fmt   The formatted string to be printed.
   *
   * @return  Number of characters printed.
   */
  int urtPrintf(char* fmt, ...);
#endif /* !defined(urtPrintf) */

#if !defined(urtErrPrintf) || defined(__DOXYGEN__)
  /**
   * @brief   Prints a formatted string to error output.
   *
   * @param[in] fmt   The formatted string to be printed.
   *
   * @return  Number of characters printed.
   */
  int urtErrPrintf(char* fmt, ...);
#endif /* !defined(urtErrPrintf) */

/* TIME ***********************************************************************/

#if !defined(urtTimeSet) || defined(__DOXYGEN__)
  /**
   * @brief   Set an OS time to a specified value (in microseconds).
   *
   * @param[in, out] time   The time to set.
   *                        Must not be NULL.
   * @param[in]      us     Value to set (in microseconds).
   */
  void urtTimeSet(urt_osTime_t* time, uint64_t us);
#endif /* !defined(urtTimeSet) */

#if !defined(urtTimeGet) || defined(__DOXYGEN__)
  /**
   * @brief   Retrieve the microsecond equivalent value from an OS time.
   *
   * @param[in] t   The time to convert.
   *                Must not be NULL
   *
   * @return  The equivalent time in microseconds.
   */
  uint64_t urtTimeGet(urt_osTime_t* t);
#endif /* !defined(urtTime2Us) */

#if !defined(urtTimeNow) || defined(__DOXYGEN__)
  /**
   * @brief   Get the current system time.
   *
   * @param[in,out] t   Time object to store the system time to.
   *                    Must not be NULL.
   */
  void urtTimeNow(urt_osTime_t* t);
#endif /* !defined(urtTimeNow) */

#if !defined(urtTimeAdd) || defined(__DOXYGEN__)
  /**
   * @brief   Add microseconds to a specified system time.
   *
   * @param[in] time    Pointer to the time value to be incremented.
   *                    Must not be NULL.
   * @param[in] offset  Amount of microseconds to add.
   */
  void urtTimeAdd(urt_osTime_t* time, urt_delay_t offset);
#endif /* !defined(urtTimeAddUs) */

#if !defined(urtTimeDiff) || defined(__DOXYGEN__)
  /**
   * @brief   Calculate difference between two OS times in microseconds.
   *
   * @param[in] from  The earlier time.
   *                  Must not be NULL.
   * @param[in] to    The later time.
   *                  Must not be NULL.
   *
   * @return  Time difference between the two times.
   */
  uint64_t urtTimeDiff(urt_osTime_t* from, urt_osTime_t* to);
#endif /* !defined(urtTimeDiff) */

/* THREAD *********************************************************************/

#if !defined(urtThreadInit) || defined(__DOXYGEN__)
  /**
   * @brief   Initialize a thread object.
   * @details Initializes the memory but does not start execution of the thread.
   *          The thread will only be started when urtThreadStart() is called.
   *          The new thread becomes a child of the calling thread.
   *
   * @param[in] memory  Pointer to the memory where to create the thread object.
   *                    Must not be NULL.
   * @param[in] size    Size of the memory.
   * @param[in] prio    Initial priority of the thread.
   * @param[in] func    Pointer to the thread function the thread shall execute.
   *                    Must not be NULL.
   * @param[in] arg     Pointer to optional function arguments.
   *                    May be NULL.
   *
   * @return  Pointer to the created thread object.
   */
  urt_osThread_t* urtThreadInit(void* memory, size_t size, urt_osThreadPrio_t prio, urt_osThreadFunction_t func, void* arg);
#endif /* !defined(urtThreadInit) */

#if !defined(urtThreadStart) || defined(__DOXYGEN__)
  /**
   * @brief   Start execution of a thread.
   *
   * @param[in] thread  Pointer to the thread to start.
   *                    Must not be NULL.
   */
  void urtThreadStart(urt_osThread_t* thread);
#endif /* !defined(urtThreadStart) */

#if !defined(urtThreadYield) || defined(__DOXYGEN__)
  /**
   * @brief   Yield execution of the calling thread.
   */
  void urtThreadYield(void);
#endif /* !defined(urtThreadYield) */

#if !defined(urtThreadGetPriority) || defined(__DOXYGEN__)
  /**
   * @brief   Retrieve the current priority of the calling thread.
   *
   * @return  Current priority.
   */
  urt_osThreadPrio_t urtThreadGetPriority(void);
#endif /* !defined(urtThreadGetPriority) */

#if !defined(urtThreadSetPriority) || defined(__DOXYGEN__)
  /**
   * @brief   Set the priority of the calling thread.
   *
   * @param[in] prio  The new priority to set.
   */
  void urtThreadSetPriority(urt_osThreadPrio_t prio);
#endif /* !defined(urtThreadSetPriority) */

#if !defined(urtThreadSleep) || defined(__DOXYGEN__)
  /**
   * @brief   The calling thread sleeps for the given amount of time.
   *
   * @param[in] seconds   Time in seconds to sleep.
   *                      Must not be greater than URT_THREAD_SLEEP_MAX.
   */
  void urtThreadSleep(float seconds);
#endif /* !defined(urtThreadSleep) */

#if !defined(urtThreadSSleep) || defined(__DOXYGEN__)
  /**
   * @brief   The calling thread sleeps for the given amount of seconds.
   *
   * @param[in] seconds   Time in seconds to sleep.
   *                      Must not be greater than URT_THREAD_SSLEP_MAX.
   */
  void urtThreadSSleep(unsigned int seconds);
#endif /* !defined(urtThreadSSleep) */

#if !defined(urtThreadMSleep) || defined(__DOXYGEN__)
  /**
   * @brief   The calling thread sleeps for the given amount of milliseconds.
   *
   * @param[in] milliseconds  Time in milliseconds to sleep.
   *                          Must not be greater than URT_THREAD_MSLEP_MAX.
   */
  void urtThreadMSleep(unsigned int milliseconds);
#endif /* !defined(urtThreadMSleep) */

#if !defined(urtThreadUSleep) || defined(__DOXYGEN__)
  /**
   * @brief   The calling thread sleeps for the gven amount of microseconds.
   *
   * @param[in] microseconds  Time in microseconds to sleep.
   *                          Must not be greater than URT_THREAD_USLEP_MAX.
   */
  void urtThreadUSleep(urt_delay_t microseconds);
#endif /* !defined(urtThreadUSleep) */

#if !defined(urtThreadSleepUntil) || defined(__DOXYGEN__)
  /**
   * @brief   The calling thread sleeps until the given point in time.
   *
   * @param[in] time  Time when the thread shall wake up.
   *                  If the current is already beyond the argument, the thread will not sleep at all.
   *                  Must not be NULL.
   */
  void urtThreadSleepUntil(urt_osTime_t* time);
#endif /* !defined(urtThreadSleepUntil) */

#if !defined(urtThreadExit) || defined(__DOXYGEN__)
  /**
   * @brief   The calling thread stops execution and terminates.
   */
  void urtThreadExit(void);
#endif /* !defined(urtThreadExit) */

#if !defined(urtThreadTerminate) || defined(__DOXYGEN__)
  /**
   * @brief   Send a termination signal to a thread.
   *
   * @param[in] thread  Thread to be signaled.
   *                    Must not be NULL.
   * @param[in] sig     Type of termination.
   */
  void urtThreadTerminate(urt_osThread_t* thread, urt_osThreadTerminateSignal_t sig);
#endif /* !defined(urtThreadTerminate) */

#if !defined(urtThreadShouldTerminate) || defined(__DOXYGEN__)
  /**
   * @brief   Retrieve whether the calling thread has been requested to terminate.
   *
   * @return  Indicator, whether the thread shoud terminate.
   */
  bool urtThreadShouldTerminate(void);
#endif /* !defined(urtThreadShouldTerminate) */

#if !defined(urtThreadJoin) || defined(__DOXYGEN__)
  /**
   * @brief   Wait for a thread to terminate.
   *
   * @param[in] thread  Pointer to the thread to wait for.
   *                    Must not be NULL.
   */
  void urtThreadJoin(urt_osThread_t* thread);
#endif /* !defined(urtThreadJoin) */

#if !defined(urtThreadGetState) || defined(__DOXYGEN__)
  /**
   * @brief   Get the current execution status of a thread.
   *
   * @param[in] thread  Pointer to the thread to get the status for.
   *                    Must not be NULL.
   *
   * @return  Execution status of the secified thread.
   */
  urt_osThreadState_t urtThreadGetState(urt_osThread_t* thread);
#endif /* !defined(urtThreadGetState) */

#if !defined(urtThreadGetSelf) || defined(__DOXYGEN__)
  /**
   * @brief   Retrieve the thread object of the calling thread.
   *
   * @return  Pointer to the thread that is currrently executed.
   *          May be NULL if called from ISR context.
   */
  urt_osThread_t* urtThreadGetSelf(void);
#endif /* !defined(urtThreadGetSelf) */

/* TIMER **********************************************************************/

#if !defined(urtTimerInit) || defined(__DOXYGEN__)
  /**
   * @brief   Initialize a timer object.
   *
   * @param[in] timer   Pointer to the timer object to initialize.
   *                    Must not be NULL.
   */
  void urtTimerInit(urt_osTimer_t* timer);
#endif /* !defined(urtTimerInit) */

#if !defined(urtTimerSet) || defined(__DOXYGEN__)
  /**
   * @brief   Set a timer in single-shot mode.
   *
   * @param[in] timer     Pointer to the timer to be armed.
   *                      Must not be NULL.
   * @param[in] time      Absolute time at which the timer shall fire.
   *                      Must not be NULL.
   * @param[in] callback  Pointer to the function to be called when the timer fires.
   *                      Must not be NULL.
   * @param[in] cbparams  Optional parameters for the callback function.
   *                      May be NULL.
   */
  void urtTimerSet(urt_osTimer_t* timer, urt_osTime_t* time, urt_osTimerCallback_t callback, void* cbparams);
#endif /* !defined(urtTimerSet) */

#if !defined(urtTimerSetPeriodic) || defined(__DOXYGEN__)
  /**
   * @brief   Set a timer in periodic mode.
   *
   * @param[in] timer     Pointer to the timer to be armed.
   *                      Must not be NULL.
   * @param[in] period    Time after which the timer shall fire periodically.
   * @param[in] callback  Pointer to the function to be called each time the timer fires.
   *                      Must not be NULL.
   * @param[in] cbparams  Optional parameters for the callback function.
   *                      May be NULL.
   */
  void urtTimerSetPeriodic(urt_osTimer_t* timer, urt_delay_t period, urt_osTimerCallback_t callback, void* cbparams);
#endif /* !defined(urtTimerSetPeriodic) */

#if !defined(urtTimerReset) || defined(__DOXYGEN__)
  /**
   * @brief   Reset and disarm a timer.
   *
   * @param[in] timer   Pointer to the timer to be reset.
   *                    Must not be NULL.
   */
  void urtTimerReset(urt_osTimer_t* timer);
#endif /* !defined(urtTimerReset) */

#if !defined(urtTimerIsArmed) || defined(__DOXYGEN__)
  /**
   * @brief   Retrieve the current status of a timer.
   *
   * @param[in] timer   Pointer to the timer to inspect.
   *                    Must not be NULL.
   *
   * @return  Flag, indicating whether or not the timer is currently armed.
   */
  bool urtTimerIsArmed(urt_osTimer_t* timer);
#endif /* !defined(urtTimerIsArmed) */

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* URT_OSAL_H */

/** @} */
