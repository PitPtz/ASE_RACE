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
 * @file    apps_urtosal.c
 */

#include <urt.h>

/*============================================================================*/
/* DEPENDENCIES                                                               */
/*============================================================================*/

/*============================================================================*/
/* DEBUG                                                                      */
/*============================================================================*/

/*============================================================================*/
/* MUTEX                                                                      */
/*============================================================================*/

/*============================================================================*/
/* CONDITION VARIABLE                                                         */
/*============================================================================*/

#if (URT_CFG_OSAL_CONDVAR_TIMEOUT == true) || defined(__DOXYGEN__)
/**
 * @cond
 * @details Due to limitations of ChibiOS, the 'mutex' argument is ignored in
 *          this implementation. Instead, the mutex, which was locked by the
 *          calling thread most recently is used.
 *          In debug builds, however, the argument is still checked to be
 *          identical to implicitely selected mutex.
 * @endcond
 */
urt_osCondvarWaitStatus_t urtCondvarWait(urt_osCondvar_t* condvar, urt_osMutex_t* mutex, urt_delay_t timeout)
{
  aosDbgCheck(condvar != NULL);
  aosDbgCheck(mutex != NULL);
  aosDbgCheck(chMtxGetNextMutexX() == mutex); // due to limitation of ChibiOS

  (void)mutex;

  switch (chCondWaitTimeout(condvar, timeout)) {
    case MSG_OK:
      return URT_CONDVAR_WAITSTATUS_SIGNAL;
    case MSG_RESET:
      return URT_CONDVAR_WAITSTATUS_BROADCAST;
    case MSG_TIMEOUT:
      return URT_CONDVAR_WAITSTATUS_TIMEOUT;
    default:
      chSysHalt(__func__);
      return URT_CONDVAR_WAITSTATUS_TIMEOUT;
  }
}
#else
urt_osCondvarWaitStatus_t urtCondvarWait(urt_osCondvar_t* condvar, urt_osMutex_t* mutex)
{
  aosDbgCheck(condvar != NULL);
  aosDbgCheck(mutex != NULL);
  aosDbgCheck(chMtxGetNextMutexX() == mutex); // due to limitation of ChibiOS

  (void)mutex;

  switch (chCondWait(condvar)) {
    case MSG_OK:
      return URT_CONDVAR_WAITSTATUS_SIGNAL;
    case MSG_RESET:
      return URT_CONDVAR_WAITSTATUS_BROADCAST;
    default:
      chSysHalt(__func__);
      return URT_CONDVAR_WAITSTATUS_TIMEOUT;
  }
}
#endif

/*============================================================================*/
/* EVENTS                                                                     */
/*============================================================================*/

void urtEventSignal(urt_osThread_t* thread, urt_osEventMask_t mask)
{
  // use chEvtSignal?() function depending on context
  if (port_is_isr_context()) {
    // calls from ISR context are always in a locked state
    chEvtSignalI(thread, mask);
  } else if (!port_irq_enabled(port_get_irq_status())) {
    chEvtSignalI(thread, mask);
    chSchRescheduleS();
  } else {
    chEvtSignal(thread, mask);
  }

  return;
}

void urtEventSourceBroadcast(urt_osEventSource_t* source, urt_osEventFlags_t flags)
{
  // use chEvtBroadcastFlags?() function depending on context
  if (port_is_isr_context()) {
    // calls from ISR context are always in a locked state
    chEvtBroadcastFlagsI(source, flags);
  } else if (!port_irq_enabled(port_get_irq_status())) {
    chEvtBroadcastFlagsI(source, flags);
    chSchRescheduleS();
  } else {
    chEvtBroadcastFlags(source, flags);
  }

  return;
}

urt_osEventFlags_t urtEventListenerClearFlags(urt_osEventListener_t* listener, urt_osEventFlags_t flags)
{
  chSysLock();
  const urt_osEventFlags_t reset = listener->flags & flags;
  listener->flags &= ~flags;
  chSysUnlock();

  return reset;
}

urt_osEventMask_t urtEventWait(urt_osEventMask_t mask, urt_osEventWait_t type, urt_delay_t timeout)
{
  switch (type) {
    case URT_EVENT_WAIT_ONE:
      return chEvtWaitOneTimeout(mask, timeout);
    case URT_EVENT_WAIT_ANY:
      return chEvtWaitAnyTimeout(mask, timeout);
    case URT_EVENT_WAIT_ALL:
      return chEvtWaitAllTimeout(mask, timeout);
    default:
      return 0;
  }
}

/*============================================================================*/
/* STREAMS                                                                    */
/*============================================================================*/

/*============================================================================*/
/* TIME                                                                       */
/*============================================================================*/

/*============================================================================*/
/* THREAD                                                                     */
/*============================================================================*/

urt_osThread_t* urtThreadInit(void* memory, size_t size, urt_osThreadPrio_t prio, urt_osThreadFunction_t func, void* arg)
{
  aosDbgCheck(memory != NULL);
  aosDbgCheck(size > 0);
  aosDbgCheck(prio >= URT_THREAD_PRIO_LOW_MIN && prio <= URT_THREAD_PRIO_RT_MAX);
  aosDbgCheck(func != NULL);

  const thread_descriptor_t descriptor = {
    /* name                         */ NULL,
    /* pointer to working area base */ (stkalign_t*)memory,
    /* end of the working area      */ (stkalign_t*)((uint8_t*)memory + size),
    /* thread priority              */ prio,
    /* thread function pointer      */ func,
    /* thread argument              */ arg,
  };

  return chThdCreateSuspended(&descriptor);
}

void urtThreadTerminate(urt_osThread_t* thread, urt_osThreadTerminateSignal_t sig)
{
  aosDbgCheck(thread != NULL);

  switch (sig) {
    case URT_THREAD_TERMINATE_REQUEST:
      chThdTerminate(thread);
      return;
    case URT_THREAD_TERMINATE_KILL:
      /*
       * TODO: implement kill functionality
       */
      chThdTerminate(thread);
      return;
  }
}

urt_osThreadState_t urtThreadGetState(urt_osThread_t* thread)
{
  aosDbgCheck(thread != NULL);

  switch (thread->state) {
    case CH_STATE_CURRENT:
      return URT_THREAD_STATE_RUNNING;
    case CH_STATE_READY:
      return URT_THREAD_STATE_READY;
    case CH_STATE_SLEEPING:
      return URT_THREAD_STATE_SLEEPING;
    case CH_STATE_WTSTART:
    case CH_STATE_SUSPENDED:
      return URT_THREAD_STATE_SUSPENDED;
    case CH_STATE_FINAL:
      return URT_THREAD_STATE_TERMINATED;
    case CH_STATE_QUEUED:
    case CH_STATE_WTSEM:
    case CH_STATE_WTMTX:
    case CH_STATE_WTCOND:
    case CH_STATE_WTEXIT:
    case CH_STATE_WTOREVT:
    case CH_STATE_WTANDEVT:
    case CH_STATE_SNDMSGQ:
    case CH_STATE_SNDMSG:
    case CH_STATE_WTMSG:
    default:
      return URT_THREAD_STATE_WAITING;
  }
}

/*============================================================================*/
/* TIMER                                                                      */
/*============================================================================*/

static void _urtosalTimerCallback(virtual_timer_t* timer, void* urtOsTimer)
{
  (void)timer;
  ((urt_osTimer_t*)urtOsTimer)->callback(((urt_osTimer_t*)urtOsTimer)->cbparams);
}

void urtTimerInit(urt_osTimer_t* timer)
{
  aosTimerInit(&timer->timer);
  timer->callback = NULL;
  timer->cbparams = NULL;
  return;
}

void urtTimerSet(urt_osTimer_t* timer, urt_osTime_t* time, urt_osTimerCallback_t callback, void* cbparams)
{
  timer->callback = callback;
  timer->cbparams = cbparams;
  aosTimerSetAbsolute(&timer->timer, *time, _urtosalTimerCallback, timer);
  return;
}

void urtTimerSetPeriodic(urt_osTimer_t* timer, urt_delay_t period, urt_osTimerCallback_t callback, void* cbparams)
{
  aosDbgCheck(timer != NULL);
  aosDbgCheck(callback != NULL);

  timer->callback = callback;
  timer->cbparams = cbparams;
  if (sizeof(urt_delay_t) > sizeof(aos_interval_t)) {
    aosTimerPeriodicLongInterval(&timer->timer, (aos_longinterval_t)period, _urtosalTimerCallback, timer);
  } else {
    aosTimerPeriodicInterval(&timer->timer, period, _urtosalTimerCallback, timer);
  }
  return;
}

void urtTimerReset(urt_osTimer_t* timer)
{
  aosTimerReset(&timer->timer);
  timer->callback = NULL;
  timer->cbparams = NULL;
}
