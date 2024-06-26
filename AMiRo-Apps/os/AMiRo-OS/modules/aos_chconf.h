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
 * @file    os/modules/common/chconf.h
 * @brief   Common ChibiOS Configuration file for the all modules.
 * @details Contains the application specific kernel settings.
 *
 * @addtogroup modules_configs_chibios
 * @{
 */

#ifndef AOS_CHCONF_H
#define AOS_CHCONF_H

#define _CHIBIOS_RT_CONF_
#define _CHIBIOS_RT_CONF_VER_7_0_

/*===========================================================================*/
/**
 * @name System settings
 * @{
 */
/*===========================================================================*/

// These settings are specific to each module.

/** @} */

/*===========================================================================*/
/**
 * @name System timers settings
 * @{
 */
/*===========================================================================*/

/**
 * @brief   System tick frequency.
 * @details Frequency of the system timer that drives the system ticks. This
 *          setting also defines the system tick time unit.
 */
# if !defined(CH_CFG_ST_FREQUENCY)
#   define CH_CFG_ST_FREQUENCY                  1000000UL
# endif

/**
 * @brief   Time intervals data size.
 * @note    Allowed values are 16, 32 or 64 bits.
 */
# if !defined(CH_CFG_INTERVALS_SIZE)
#   define CH_CFG_INTERVALS_SIZE                32
# endif

/**
 * @brief   Time types data size.
 * @note    Allowed values are 16 or 32 bits.
 */
# if !defined(CH_CFG_TIME_TYPES_SIZE)
#   define CH_CFG_TIME_TYPES_SIZE               32
# endif

/**
 * @brief   Time delta constant for the tick-less mode.
 * @note    If this value is zero then the system uses the classic
 *          periodic tick. This value represents the minimum number
 *          of ticks that is safe to specify in a timeout directive.
 *          The value one is not valid, timeouts are rounded up to
 *          this value.
 */
# if !defined(CH_CFG_ST_TIMEDELTA)
#   define CH_CFG_ST_TIMEDELTA                  1000
# endif

/** @} */

/*===========================================================================*/
/**
 * @name Kernel parameters and options
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Round robin interval.
 * @details This constant is the number of system ticks allowed for the
 *          threads before preemption occurs. Setting this value to zero
 *          disables the preemption for threads with equal priority and the
 *          round robin becomes cooperative. Note that higher priority
 *          threads can still preempt, the kernel is always preemptive.
 * @note    Disabling the round robin preemption makes the kernel more compact
 *          and generally faster.
 * @note    The round robin preemption is not supported in tickless mode and
 *          must be set to zero in that case.
 */
# if !defined(CH_CFG_TIME_QUANTUM)
#   define CH_CFG_TIME_QUANTUM                  0
# endif

/**
 * @brief   Idle thread automatic spawn suppression.
 * @details When this option is activated the function @p chSysInit()
 *          does not spawn the idle thread. The application @p main()
 *          function becomes the idle thread and must implement an
 *          infinite loop.
 */
# if !defined(CH_CFG_NO_IDLE_THREAD)
#   define CH_CFG_NO_IDLE_THREAD                FALSE
# endif

/** @} */

/*===========================================================================*/
/**
 * @name Performance options
 * @{
 */
/*===========================================================================*/

/**
 * @brief   OS optimization.
 * @details If enabled then time efficient rather than space efficient code
 *          is used when two possible implementations exist.
 *
 * @note    This is not related to the compiler optimization options.
 * @note    The default is @p TRUE.
 */
# if !defined(CH_CFG_OPTIMIZE_SPEED)
#   define CH_CFG_OPTIMIZE_SPEED                TRUE
# endif

/** @} */

/*===========================================================================*/
/**
 * @name Subsystem options
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Time Measurement APIs.
 * @details If enabled then the time measurement APIs are included in
 *          the kernel.
 *
 * @note    The default is @p TRUE.
 */
# if !defined(CH_CFG_USE_TM)
#   if ((AMIROOS_CFG_DBG == true) || (AMIROOS_CFG_PROFILE == true)) || defined(__DOXYGEN__)
#     define CH_CFG_USE_TM                      TRUE
#   else
#     define CH_CFG_USE_TM                      FALSE
#   endif
# endif

/**
 * @brief   Time Stamps APIs.
 * @details If enabled then the time stamps APIs are included in the kernel.
 *
 * @note    The default is @p TRUE.
 */
#if !defined(CH_CFG_USE_TIMESTAMP)
#define CH_CFG_USE_TIMESTAMP                    FALSE
#endif

/**
 * @brief   Threads registry APIs.
 * @details If enabled then the registry APIs are included in the kernel.
 *
 * @note    The default is @p TRUE.
 */
# if !defined(CH_CFG_USE_REGISTRY)
#   if ((AMIROOS_CFG_DBG == true) || (AMIROOS_CFG_PROFILE == true)) ||          \
       defined(__DOXYGEN__)
#     define CH_CFG_USE_REGISTRY                TRUE
#   else
#     define CH_CFG_USE_REGISTRY                FALSE
#   endif
# endif

/**
 * @brief   Thread hierarchy APIs.
 * @details Id enabled then the thread hierarchy APIs are included in the kernel.
 *
 * @note    The default is @p FALSE.
 */
# if !defined(CH_CFG_USE_THREADHIERARCHY)
#   if ((AMIROOS_CFG_DBG == true) || (AMIROOS_CFG_PROFILE == true)) ||          \
       defined(__DOXYGEN__)
#     define CH_CFG_USE_THREADHIERARCHY         TRUE
#   else
#     define CH_CFG_USE_THREADHIERARCHY         FALSE
#   endif
# endif

/**
 * @brief   Enable ordering of child lists.
 * @details Children will be ordered by their priority (descending).
 *          If sibliblings have identical priority, they are ordered by age
 *          (descending).
 */
# if !defined(CH_CFG_THREADHIERARCHY_ORDERED)
#   define CH_CFG_THREADHIERARCHY_ORDERED       TRUE
# endif

/**
 * @brief   Threads synchronization APIs.
 * @details If enabled then the @p chThdWait() function is included in
 *          the kernel.
 *
 * @note    The default is @p TRUE.
 */
# if !defined(CH_CFG_USE_WAITEXIT)
#   define CH_CFG_USE_WAITEXIT                  TRUE
# endif

/**
 * @brief   Semaphores APIs.
 * @details If enabled then the Semaphores APIs are included in the kernel.
 *
 * @note    The default is @p TRUE.
 */
# if !defined(CH_CFG_USE_SEMAPHORES)
#   define CH_CFG_USE_SEMAPHORES                TRUE
# endif

/**
 * @brief   Semaphores queuing mode.
 * @details If enabled then the threads are enqueued on semaphores by
 *          priority rather than in FIFO order.
 *
 * @note    The default is @p FALSE. Enable this if you have special
 *          requirements.
 * @note    Requires @p CH_CFG_USE_SEMAPHORES.
 */
# if !defined(CH_CFG_USE_SEMAPHORES_PRIORITY)
#   define CH_CFG_USE_SEMAPHORES_PRIORITY       FALSE
# endif

/**
 * @brief   Mutexes APIs.
 * @details If enabled then the mutexes APIs are included in the kernel.
 *
 * @note    The default is @p TRUE.
 */
# if !defined(CH_CFG_USE_MUTEXES)
#   define CH_CFG_USE_MUTEXES                   TRUE
# endif

/**
 * @brief   Enables recursive behavior on mutexes.
 * @note    Recursive mutexes are heavier and have an increased
 *          memory footprint.
 *
 * @note    The default is @p FALSE.
 * @note    Requires @p CH_CFG_USE_MUTEXES.
 */
# if !defined(CH_CFG_USE_MUTEXES_RECURSIVE)
#   define CH_CFG_USE_MUTEXES_RECURSIVE         FALSE
# endif

/**
 * @brief   Conditional Variables APIs.
 * @details If enabled then the conditional variables APIs are included
 *          in the kernel.
 *
 * @note    The default is @p TRUE.
 * @note    Requires @p CH_CFG_USE_MUTEXES.
 */
# if !defined(CH_CFG_USE_CONDVARS)
#   define CH_CFG_USE_CONDVARS                  TRUE
# endif

/**
 * @brief   Conditional Variables APIs with timeout.
 * @details If enabled then the conditional variables APIs with timeout
 *          specification are included in the kernel.
 *
 * @note    The default is @p FALSE.
 * @note    Requires @p CH_CFG_USE_CONDVARS.
 */
# if !defined(CH_CFG_USE_CONDVARS_TIMEOUT)
#   define CH_CFG_USE_CONDVARS_TIMEOUT          FALSE
# endif

/**
 * @brief   Events Flags APIs.
 * @details If enabled then the event flags APIs are included in the kernel.
 *
 * @note    The default is @p TRUE.
 */
# if !defined(CH_CFG_USE_EVENTS)
#   define CH_CFG_USE_EVENTS                    TRUE
# endif

/**
 * @brief   Events Flags APIs with timeout.
 * @details If enabled then the events APIs with timeout specification
 *          are included in the kernel.
 *
 * @note    The default is @p TRUE.
 * @note    Requires @p CH_CFG_USE_EVENTS.
 */
# if !defined(CH_CFG_USE_EVENTS_TIMEOUT)
#   define CH_CFG_USE_EVENTS_TIMEOUT            TRUE
# endif

/**
 * @brief   Synchronous Messages APIs.
 * @details If enabled then the synchronous messages APIs are included
 *          in the kernel.
 *
 * @note    The default is @p TRUE.
 */
# if !defined(CH_CFG_USE_MESSAGES)
#   define CH_CFG_USE_MESSAGES                  FALSE
# endif

/**
 * @brief   Synchronous Messages queuing mode.
 * @details If enabled then messages are served by priority rather than in
 *          FIFO order.
 *
 * @note    The default is @p FALSE. Enable this if you have special
 *          requirements.
 * @note    Requires @p CH_CFG_USE_MESSAGES.
 */
# if !defined(CH_CFG_USE_MESSAGES_PRIORITY)
#   define CH_CFG_USE_MESSAGES_PRIORITY         FALSE
# endif

/**
 * @brief   Dynamic Threads APIs.
 * @details If enabled then the dynamic threads creation APIs are included
 *          in the kernel.
 *
 * @note    The default is @p TRUE.
 * @note    Requires @p CH_CFG_USE_WAITEXIT.
 * @note    Requires @p CH_CFG_USE_HEAP and/or @p CH_CFG_USE_MEMPOOLS.
 */
# if !defined(CH_CFG_USE_DYNAMIC)
#   define CH_CFG_USE_DYNAMIC                   FALSE
# endif

/** @} */

/*===========================================================================*/
/**
 * @name OSLIB options
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Mailboxes APIs.
 * @details If enabled then the asynchronous messages (mailboxes) APIs are
 *          included in the kernel.
 *
 * @note    The default is @p TRUE.
 * @note    Requires @p CH_CFG_USE_SEMAPHORES.
 */
# if !defined(CH_CFG_USE_MAILBOXES)
#   define CH_CFG_USE_MAILBOXES                 FALSE
# endif

/**
 * @brief   Core Memory Manager APIs.
 * @details If enabled then the core memory manager APIs are included
 *          in the kernel.
 *
 * @note    The default is @p TRUE.
 */
# if !defined(CH_CFG_USE_MEMCORE)
#   define CH_CFG_USE_MEMCORE                   FALSE
# endif

/**
 * @brief   Managed RAM size.
 * @details Size of the RAM area to be managed by the OS. If set to zero
 *          then the whole available RAM is used. The core memory is made
 *          available to the heap allocator and/or can be used directly through
 *          the simplified core memory allocator.
 *
 * @note    In order to let the OS manage the whole RAM the linker script must
 *          provide the @p __heap_base__ and @p __heap_end__ symbols.
 * @note    Requires @p CH_CFG_USE_MEMCORE.
 */
# if !defined(CH_CFG_MEMCORE_SIZE)
#   define CH_CFG_MEMCORE_SIZE                  0
# endif

/**
 * @brief   Heap Allocator APIs.
 * @details If enabled then the memory heap allocator APIs are included
 *          in the kernel.
 *
 * @note    The default is @p TRUE.
 * @note    Requires @p CH_CFG_USE_MEMCORE and either @p CH_CFG_USE_MUTEXES or
 *          @p CH_CFG_USE_SEMAPHORES.
 * @note    Mutexes are recommended.
 */
# if !defined(CH_CFG_USE_HEAP)
#   define CH_CFG_USE_HEAP                      FALSE
# endif

/**
 * @brief   Memory Pools Allocator APIs.
 * @details If enabled then the memory pools allocator APIs are included
 *          in the kernel.
 *
 * @note    The default is @p TRUE.
 */
# if !defined(CH_CFG_USE_MEMPOOLS)
#   define CH_CFG_USE_MEMPOOLS                  FALSE
# endif

/**
 * @brief   Objects FIFOs APIs.
 * @details If enabled then the objects FIFOs APIs are included
 *          in the kernel.
 *
 * @note    The default is @p TRUE.
 */
# if !defined(CH_CFG_USE_OBJ_FIFOS)
#   define CH_CFG_USE_OBJ_FIFOS                 FALSE
# endif

/**
 * @brief   Pipes APIs.
 * @details If enabled then the pipes APIs are included
 *          in the kernel.
 *
 * @note    The default is @p TRUE.
 */
# if !defined(CH_CFG_USE_PIPES)
#   define CH_CFG_USE_PIPES                     FALSE
# endif

/**
 * @brief   Objects Caches APIs.
 * @details If enabled then the objects caches APIs are included
 *          in the kernel.
 *
 * @note    The default is @p TRUE.
 */
# if !defined(CH_CFG_USE_OBJ_CACHES)
#   define CH_CFG_USE_OBJ_CACHES                FALSE
# endif

/**
 * @brief   Delegate threads APIs.
 * @details If enabled then the delegate threads APIs are included
 *          in the kernel.
 *
 * @note    The default is @p TRUE.
 */
# if !defined(CH_CFG_USE_DELEGATES)
#   define CH_CFG_USE_DELEGATES                 FALSE
# endif

/**
 * @brief   Jobs Queues APIs.
 * @details If enabled then the jobs queues APIs are included
 *          in the kernel.
 *
 * @note    The default is @p TRUE.
 */
# if !defined(CH_CFG_USE_JOBS)
#   define CH_CFG_USE_JOBS                      FALSE
# endif

/** @} */

/*===========================================================================*/
/**
 * @name Objects factory options
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Objects Factory APIs.
 * @details If enabled then the objects factory APIs are included in the
 *          kernel.
 *
 * @note    The default is @p FALSE.
 */
# if !defined(CH_CFG_USE_FACTORY)
#   define CH_CFG_USE_FACTORY                   FALSE
# endif

/**
 * @brief   Maximum length for object names.
 * @details If the specified length is zero then the name is stored by
 *          pointer but this could have unintended side effects.
 */
# if !defined(CH_CFG_FACTORY_MAX_NAMES_LENGTH)
#   define CH_CFG_FACTORY_MAX_NAMES_LENGTH      8
# endif

/**
 * @brief   Enables the registry of generic objects.
 */
# if !defined(CH_CFG_FACTORY_OBJECTS_REGISTRY)
#   define CH_CFG_FACTORY_OBJECTS_REGISTRY      TRUE
# endif

/**
 * @brief   Enables factory for generic buffers.
 */
# if !defined(CH_CFG_FACTORY_GENERIC_BUFFERS)
#   define CH_CFG_FACTORY_GENERIC_BUFFERS       TRUE
# endif

/**
 * @brief   Enables factory for semaphores.
 */
# if !defined(CH_CFG_FACTORY_SEMAPHORES)
#   define CH_CFG_FACTORY_SEMAPHORES            TRUE
# endif

/**
 * @brief   Enables factory for mailboxes.
 */
# if !defined(CH_CFG_FACTORY_MAILBOXES)
#   define CH_CFG_FACTORY_MAILBOXES             TRUE
# endif

/**
 * @brief   Enables factory for objects FIFOs.
 */
# if !defined(CH_CFG_FACTORY_OBJ_FIFOS)
#   define CH_CFG_FACTORY_OBJ_FIFOS             TRUE
# endif

/**
 * @brief   Enables factory for Pipes.
 */
# if !defined(CH_CFG_FACTORY_PIPES) || defined(__DOXYGEN__)
#   define CH_CFG_FACTORY_PIPES                 TRUE
# endif

/** @} */

/*===========================================================================*/
/**
 * @name Debug options
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Debug option, kernel statistics.
 *
 * @note    The default is @p FALSE.
 */
# if !defined(CH_DBG_STATISTICS)
#   if ((CH_CFG_USE_TM == TRUE) &&                                              \
        ((AMIROOS_CFG_DBG == true) || (AMIROOS_CFG_PROFILE == true))) ||        \
       defined(__DOXYGEN__)
#     define CH_DBG_STATISTICS                  TRUE
#   else
#     define CH_DBG_STATISTICS                  FALSE
#   endif
# endif

/**
 * @brief   Debug option, system state check.
 * @details If enabled the correct call protocol for system APIs is checked
 *          at runtime.
 *
 * @note    The default is @p FALSE.
 */
# if !defined(CH_DBG_SYSTEM_STATE_CHECK)
#   if (AMIROOS_CFG_DBG == true) || defined(__DOXYGEN__)
#     define CH_DBG_SYSTEM_STATE_CHECK          TRUE
#   else
#     define CH_DBG_SYSTEM_STATE_CHECK          TRUE
#   endif
# endif

/**
 * @brief   Debug option, parameters checks.
 * @details If enabled then the checks on the API functions input
 *          parameters are activated.
 *
 * @note    The default is @p FALSE.
 */
# if !defined(CH_DBG_ENABLE_CHECKS)
#   if (AMIROOS_CFG_DBG == true) || defined(__DOXYGEN__)
#     define CH_DBG_ENABLE_CHECKS               TRUE
#   else
#     define CH_DBG_ENABLE_CHECKS               TRUE
#   endif
# endif

/**
 * @brief   Debug option, consistency checks.
 * @details If enabled then all the assertions in the kernel code are
 *          activated. This includes consistency checks inside the kernel,
 *          runtime anomalies and port-defined checks.
 *
 * @note    The default is @p FALSE.
 */
# if !defined(CH_DBG_ENABLE_ASSERTS)
#   if (AMIROOS_CFG_DBG == true) || defined(__DOXYGEN__)
#     define CH_DBG_ENABLE_ASSERTS              TRUE
#   else
#     define CH_DBG_ENABLE_ASSERTS              TRUE
#   endif
# endif

/**
 * @brief   Debug option, trace buffer.
 * @details If enabled then the trace buffer is activated.
 *
 * @note    The default is @p CH_DBG_TRACE_MASK_DISABLED.
 */
# if !defined(CH_DBG_TRACE_MASK)
#   define CH_DBG_TRACE_MASK                    CH_DBG_TRACE_MASK_DISABLED
# endif

/**
 * @brief   Trace buffer entries.
 * @note    The trace buffer is only allocated if @p CH_DBG_TRACE_MASK is
 *          different from @p CH_DBG_TRACE_MASK_DISABLED.
 */
# if !defined(CH_DBG_TRACE_BUFFER_SIZE)
#   define CH_DBG_TRACE_BUFFER_SIZE             128
# endif

/**
 * @brief   Debug option, stack checks.
 * @details If enabled then a runtime stack check is performed.
 *
 * @note    The default is @p FALSE.
 * @note    The stack check is performed in a architecture/port dependent way.
 *          It may not be implemented or some ports.
 * @note    The default failure mode is to halt the system with the global
 *          @p panic_msg variable set to @p NULL.
 */
# if !defined(CH_DBG_ENABLE_STACK_CHECK)
#   if (AMIROOS_CFG_DBG == true) || defined(__DOXYGEN__)
#     define CH_DBG_ENABLE_STACK_CHECK          TRUE
#   else
#     define CH_DBG_ENABLE_STACK_CHECK          TRUE
#   endif
# endif

/**
 * @brief   Debug option, stacks initialization.
 * @details If enabled then the threads working area is filled with a byte
 *          value when a thread is created. This can be useful for the
 *          runtime measurement of the used stack.
 *
 * @note    The default is @p FALSE.
 */
# if !defined(CH_DBG_FILL_THREADS)
#   if (AMIROOS_CFG_PROFILE == true) || defined(__DOXYGEN__)
#     define CH_DBG_FILL_THREADS                TRUE
#   else
#     define CH_DBG_FILL_THREADS                FALSE
#   endif
# endif

/**
 * @brief   Debug option, threads profiling.
 * @details If enabled then a field is added to the @p thread_t structure that
 *          counts the system ticks occurred while executing the thread.
 *
 * @note    The default is @p FALSE.
 * @note    This debug option is not currently compatible with the
 *          tickless mode.
 */
# if !defined(CH_DBG_THREADS_PROFILING)
#   if ((CH_CFG_ST_TIMEDELTA == 0) && (AMIROOS_CFG_PROFILE == true)) ||         \
       defined(__DOXYGEN__)
#     define CH_DBG_THREADS_PROFILING           TRUE
#   else
#     define CH_DBG_THREADS_PROFILING           FALSE
#   endif
# endif

/** @} */

/*===========================================================================*/
/**
 * @name Kernel hooks
 * @{
 */
/*===========================================================================*/

/**
 * @brief   System structure extension.
 * @details User fields added to the end of the @p ch_system_t structure.
 */
# if !defined(CH_CFG_SYSTEM_EXTRA_FIELDS)
#   define CH_CFG_SYSTEM_EXTRA_FIELDS                                           \
      /* Add threads custom fields here.*/                                      \
      size_t threadcount;
# endif

/**
 * @brief   System initialization hook.
 * @details User initialization code added to the @p chSysInit() function
 *          just before interrupts are enabled globally.
 */
# if !defined(CH_CFG_SYSTEM_INIT_HOOK)
#   define CH_CFG_SYSTEM_INIT_HOOK() {                                          \
      /* Add threads initialization code here.*/                                \
      ch_system.threadcount = 0;                                                \
    }
# endif

/**
 * @brief   OS instance structure extension.
 * @details User fields added to the end of the @p os_instance_t structure.
 */
#define CH_CFG_OS_INSTANCE_EXTRA_FIELDS                                         \
  /* Add OS instance custom fields here.*/

/**
 * @brief   OS instance initialization hook.
 *
 * @param[in] oip       pointer to the @p os_instance_t structure
 */
#define CH_CFG_OS_INSTANCE_INIT_HOOK(oip) {                                     \
  /* Add OS instance initialization code here.*/                                \
}

/**
 * @brief   Threads descriptor structure extension.
 * @details User fields added to the end of the @p thread_t structure.
 */
# if !defined(CH_CFG_THREAD_EXTRA_FIELDS)
#   define CH_CFG_THREAD_EXTRA_FIELDS                                           \
      /* Add threads custom fields here.*/
# endif

/**
 * @brief   Threads initialization hook.
 * @details User initialization code added to the @p _thread_init() function.
 *
 * @note    It is invoked from within @p _thread_init() and implicitly from all
 *          the threads creation APIs.
 */
# if !defined(CH_CFG_THREAD_INIT_HOOK)
#   define CH_CFG_THREAD_INIT_HOOK(tp) {                                        \
      /* Add threads initialization code here.*/                                \
      ++ch_system.threadcount;                                                  \
    }
# endif

/**
 * @brief   Threads finalization hook.
 * @details User finalization code added to the @p chThdExit() API.
 */
# if !defined(CH_CFG_THREAD_EXIT_HOOK)
#   define CH_CFG_THREAD_EXIT_HOOK(tp) {                                        \
      /* Add threads finalization code here.*/                                  \
      --ch_system.threadcount;                                                  \
    }
# endif

/**
 * @brief   Context switch hook.
 * @details This hook is invoked just before switching between threads.
 */
# if !defined(CH_CFG_CONTEXT_SWITCH_HOOK)
#   define CH_CFG_CONTEXT_SWITCH_HOOK(ntp, otp) {                               \
      /* Context switch code here.*/                                            \
    }
# endif

/**
 * @brief   ISR enter hook.
 */
# if !defined(CH_CFG_IRQ_PROLOGUE_HOOK)
#   define CH_CFG_IRQ_PROLOGUE_HOOK() {                                         \
      /* IRQ prologue code here.*/                                              \
    }
# endif

/**
 * @brief   ISR exit hook.
 */
# if !defined(CH_CFG_IRQ_EPILOGUE_HOOK)
#   define CH_CFG_IRQ_EPILOGUE_HOOK() {                                         \
      /* IRQ epilogue code here.*/                                              \
    }
# endif

/**
 * @brief   Idle thread enter hook.
 * @note    This hook is invoked within a critical zone, no OS functions
 *          should be invoked from here.
 * @note    This macro can be used to activate a power saving mode.
 */
# if !defined(CH_CFG_IDLE_ENTER_HOOK)
#   define CH_CFG_IDLE_ENTER_HOOK() {                                           \
      /* Idle-enter code here.*/                                                \
    }
# endif

/**
 * @brief   Idle thread leave hook.
 * @note    This hook is invoked within a critical zone, no OS functions
 *          should be invoked from here.
 * @note    This macro can be used to deactivate a power saving mode.
 */
# if !defined(CH_CFG_IDLE_LEAVE_HOOK)
#   define CH_CFG_IDLE_LEAVE_HOOK() {                                           \
      /* Idle-leave code here.*/                                                \
    }
# endif

/**
 * @brief   Idle Loop hook.
 * @details This hook is continuously invoked by the idle thread loop.
 */
# if !defined(CH_CFG_IDLE_LOOP_HOOK)
#   define CH_CFG_IDLE_LOOP_HOOK() {                                            \
      /* Idle loop code here.*/                                                 \
    }
# endif

/**
 * @brief   System tick event hook.
 * @details This hook is invoked in the system tick handler immediately
 *          after processing the virtual timers queue.
 */
# if !defined(CH_CFG_SYSTEM_TICK_HOOK)
#   define CH_CFG_SYSTEM_TICK_HOOK() {                                          \
      /* System tick event code here.*/                                         \
    }
# endif

/**
 * @brief   System halt hook.
 * @details This hook is invoked in case to a system halting error before
 *          the system is halted.
 */
# if !defined(CH_CFG_SYSTEM_HALT_HOOK)
#   define CH_CFG_SYSTEM_HALT_HOOK(reason) {                                    \
  while (true) {                                                                \
      USART1->DR='$';                                                           \
      for (char* volatile tmp = reason; *tmp != '\0'; tmp++) {                  \
          while (!(USART1->SR & USART_SR_TXE)) {};                              \
          USART1->DR = *tmp;                                                    \
      }                                                                         \
      /*USART2->DR='~';     */                      \
      /*USART3->DR='^';   */                        \
      /* System halt code here.*/                                               \
    }                                             \
    }
# endif

/**
 * @brief   Trace hook.
 * @details This hook is invoked each time a new record is written in the
 *          trace buffer.
 */
# if !defined(CH_CFG_TRACE_HOOK)
#   define CH_CFG_TRACE_HOOK(tep) {                                             \
      /* Trace code here.*/                                                     \
    }
# endif


/**
 * @brief   Runtime Faults Collection Unit hook.
 * @details This hook is invoked each time new faults are collected and stored.
 */
# if !defined(CH_CFG_RUNTIME_FAULTS_HOOK)
#   define CH_CFG_RUNTIME_FAULTS_HOOK(mask) {                                   \
      /* Faults handling code here.*/                                           \
    }
# endif

/** @} */

/*===========================================================================*/
/**
 * @name Port-specific settings (override port settings defaulted in chcore.h).
 * @{
 */
/*===========================================================================*/

// These settings are specific to each module.

/** @} */

/*===========================================================================*/
/**
 * @name other
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Flag to enable/disable floating point support in chprintf() and all deriviates.
 */
#define CHPRINTF_USE_FLOAT                  TRUE

/**
 * @brief   Flag to enable/disable long long support in chprintf() and all derivatives.
 */
#define CHPRINTF_SUPPORT_LONGLONG           TRUE

/** @} */

#endif  /* AOS_CHCONF_H */

/** @} */
