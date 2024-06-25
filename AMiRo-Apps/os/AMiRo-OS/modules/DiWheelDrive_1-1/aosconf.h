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
 * @file    aosconf.h
 * @brief   AMiRo-OS Configuration file for the DiWheelDrive v1.1 module.
 * @details Contains the application specific AMiRo-OS settings.
 *
 * @defgroup diwheeldrive11_cfg_amiroos AMiRo-OS
 * @ingroup diwheeldrive11_cfg
 *
 * @brief   Todo
 * @details Todo
 *
 * @addtogroup diwheeldrive11_cfg_amiroos
 * @{
 */

#ifndef AOSCONF_H
#define AOSCONF_H

/*
 * compatibility guards
 */
#define _AMIRO_OS_CFG_
#define AMIRO_OS_CFG_VERSION_MAJOR              2
#define AMIRO_OS_CFG_VERSION_MINOR              3

#include <stdbool.h>

/*
 * Include an external configuration file to override the following default
 * settings only if required.
 */
#if defined(AMIRO_APPS) && (AMIRO_APPS == true)
  #include <osconf.h>
#endif /* defined(AMIRO_APPS) && (AMIRO_APPS == true) */

/*===========================================================================*/
/**
 * @name Kernel parameters and options
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Flag to enable/disable debug API and logic.
 */
#if !defined(OS_CFG_DBG)
  #define AMIROOS_CFG_DBG                       true
#else /* !defined(OS_CFG_DBG) */
  #define AMIROOS_CFG_DBG                       OS_CFG_DBG
#endif /* !defined(OS_CFG_DBG) */

/**
 * @brief   Flag to enable/disable unit tests.
 * @note    Setting this flag will implicitely enable the shell.
 */
#if !defined(OS_CFG_TESTS_ENABLE)
  #define AMIROOS_CFG_TESTS_ENABLE              true
#else /* !defined(OS_CFG_TESTS_ENABLE) */
  #define AMIROOS_CFG_TESTS_ENABLE              OS_CFG_TESTS_ENABLE
#endif /* !defined(OS_CFG_TESTS_ENABLE) */

/**
 * @brief   Flag to enable/disable kernel tests.
 * @note    This flag is only relevant when AMIROOS_CFG_TESTS_ENABLE is true.
 */
#if !defined(OS_CFG_TESTS_ENABLE_KERNEL)
  #define AMIROOS_CFG_TESTS_ENABLE_KERNEL       true
#else /* !defined(OS_CFG_TESTS_ENABLE_KERNEL) */
  #define AMIROOS_CFG_TESTS_ENABLE_KERNEL       OS_CFG_TESTS_ENABLE_KERNEL
#endif /* !defined(OS_CFG_TESTS_ENABLE_KERNEL) */

/**
 * @brief   Flag to enable/disable periphery tests.
 * @note    This flag is only relevant when AMIROOS_CFG_TESTS_ENABLE is true.
 */
#if !defined(OS_CFG_TESTS_ENABLE_PERIPHERY)
  #define AMIROOS_CFG_TESTS_ENABLE_PERIPHERY    true
#else /* !defined(OS_CFG_TESTS_ENABLE_PERIPHERY) */
  #define AMIROOS_CFG_TESTS_ENABLE_PERIPHERY    OS_CFG_TESTS_ENABLE_PERIPHERY
#endif /* !defined(OS_CFG_TESTS_ENABLE_PERIPHERY) */

/**
 * @brief   Flag to enable/disable profiling API and logic.
 */
#if !defined(OS_CFG_PROFILE)
  #define AMIROOS_CFG_PROFILE                   true
#else /* !defined(OS_CFG_PROFILE) */
  #define AMIROOS_CFG_PROFILE                   OS_CFG_PROFILE
#endif /* !defined(OS_CFG_PROFILE) */

/**
 * @brief   Mask for the control thread to listen to certain GPIO events.
 * @note    Any mandatory events (e.g. for SSSP) are enabled implicitely
 *          despite this configuration.
 */
#if !defined(OS_CFG_MAIN_LOOP_GPIOEVENT_FLAGSMASK)
  #define AMIROOS_CFG_MAIN_LOOP_GPIOEVENT_FLAGSMASK 0
#else /* !defined(OS_CFG_MAIN_LOOP_GPIOEVENT_FLAGSMASK) */
  #define AMIROOS_CFG_MAIN_LOOP_GPIOEVENT_FLAGSMASK OS_CFG_MAIN_LOOP_GPIOEVENT_FLAGSMASK
#endif /* !defined(OS_CFG_MAIN_LOOP_GPIOEVENT_FLAGSMASK) */

/**
 * @brief   Timeout value when waiting for events in the main loop in
 *          microseconds.
 * @details A value of 0 deactivates the timeout.
 */
#if !defined(OS_CFG_MAIN_LOOP_TIMEOUT)
  #define AMIROOS_CFG_MAIN_LOOP_TIMEOUT         0
#else /* !defined(OS_CFG_MAIN_LOOP_TIMEOUT) */
  #define AMIROOS_CFG_MAIN_LOOP_TIMEOUT         OS_CFG_MAIN_LOOP_TIMEOUT
#endif /* !defined(OS_CFG_MAIN_LOOP_TIMEOUT) */

/** @} */

/*===========================================================================*/
/**
 * @name Bootloader configuration
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Identifier of the instaled bootloader (if any).
 * @note    See aos_bootloader.h file for a list of available settings.
 */
#if !defined(OS_CFG_BOOTLOADER)
  #define AMIROOS_CFG_BOOTLOADER                AOS_BOOTLOADER_AMiRoBLT
#else /* !defined(OS_CFG_BOOTLOADER) */
  #define AMIROOS_CFG_BOOTLOADER                OS_CFG_BOOTLOADER
#endif /* !defined(OS_CFG_BOOTLOADER) */

/** @} */

/*===========================================================================*/
/**
 * @name Filtered & Buffered CAN (FBCAN) configuration
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Flag to enable FBCAN for the CAN1 interface.
 */
#if !defined(OS_CFG_FBCAN1_ENABLE)
  #define AMIROOS_CFG_FBCAN1_ENABLE             true
#else /* !defined(OS_CFG_FBCAN1_ENABLE) */
  #define AMIROOS_CFG_FBCAN1_ENABLE             OS_CFG_FBCAN1_ENABLE
#endif /* !defined(OS_CFG_FBCAN1_ENABLE) */

/**
 * @brief   Flag to enable FBCAN for the CAN2 interface.
 */
#if !defined(OS_CFG_FBCAN2_ENABLE)
  #define AMIROOS_CFG_FBCAN2_ENABLE             false
#else /* !defined(OS_CFG_FBCAN2_ENABLE) */
  #define AMIROOS_CFG_FBCAN2_ENABLE             OS_CFG_FBCAN2_ENABLE
#endif /* !defined(OS_CFG_FBCAN2_ENABLE) */

/**
 * @brief   Flag to enable FBCAN for the CAN3 interface.
 */
#if !defined(OS_CFG_FBCAN3_ENABLE)
  #define AMIROOS_CFG_FBCAN3_ENABLE             false
#else /* !defined(OS_CFG_FBCAN3_ENABLE) */
  #define AMIROOS_CFG_FBCAN3_ENABLE             OS_CFG_FBCAN3_ENABLE
#endif /* !defined(OS_CFG_FBCAN3_ENABLE) */

/**
 * @brief   Size of input buffers for the SerialCANDriver.
 * @details Setting this value to 0 diables Serial over CAN API.
 */
#if !defined(OS_CFG_FBCAN_SERIAL_BUFFERSIZE)
  #define AMIROOS_CFG_FBCAN_SERIAL_BUFFERSIZE   AMIROOS_CFG_SHELL_LINEWIDTH
#else /* !defined(OS_CFG_FBCAN_SERIAL_BUFFERSIZE) */
  #define AMIROOS_CFG_FBCAN_SERIAL_BUFFERSIZE   OS_CFG_FBCAN_SERIAL_BUFFERSIZE
#endif /* !defined(OS_CFG_FBCAN_SERIAL_BUFFERSIZE) */

/** @} */

/*===========================================================================*/
/**
 * @name SSSP (Startup Shutdown Synchronization Protocol) configuration.
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Flag to enable SSSP.
 */
#if !defined(OS_CFG_SSSP_ENABLE)
  #define AMIROOS_CFG_SSSP_ENABLE               true
#else /* !defined(OS_CFG_SSSP_ENABLE) */
  #define AMIROOS_CFG_SSSP_ENABLE               OS_CFG_SSSP_ENABLE
#endif /* !defined(OS_CFG_SSSP_ENABLE) */

/**
 * @brief   Flag to indicate, whether the SSSP startup sequence shall be
 *          executed by AMiRo-OS.
 * @details This setting must be false in case another software (e.g. a
 *          bootloader) handles the initial stages of the SSSP startup phase.
 */
#if !defined(OS_CFG_SSSP_STARTUP)
  #define AMIROOS_CFG_SSSP_STARTUP              false
#else
  #define AMIROOS_CFG_SSSP_STARTUP              OS_CFG_SSSP_STARTUP
#endif

/**
 * @brief   Flag to indicate, whether the SSSP shutdown sequence shall be
 *          executed by AMiRo-OS.
 * @details This setting should be false in case another software (e.g. a
 *          bootloader) handles the final stages of the SSSP shutdown phase.
 */
#if !defined(OS_CFG_SSSP_SHUTDOWN)
  #define AMIROOS_CFG_SSSP_SHUTDOWN             false
#else
  #define AMIROOS_CFG_SSSP_SHUTDOWN             OS_CFG_SSSP_SHUTDOWN
#endif

/**
 * @brief   Flag to indicate, whether the module stack initialization (MSI;
 *          stage 3 of the SSSP startup phase) shall be executed.
 * @brief   In order to execute MSI, a broadcast bus (BCB) and according
 *          interfaces must be defined.
 */
#if !defined(OS_CFG_SSSP_MSI)
  #define AMIROOS_CFG_SSSP_MSI                  true
#else
  #define AMIROOS_CFG_SSSP_MSI                  OS_CFG_SSSP_MSI
#endif

/**
 * @brief   Width of the @p aos_sssp_moduleid_t type.
 * @details Possible settings are 8, 16, 32 and 64.
 */
#if !defined(OS_CFG_SSSP_MODULEIDWIDTH)
  #define AMIROOS_CFG_SSSP_MODULEIDWIDTH        16
#else
  #define AMIROOS_CFG_SSSP_MODULEIDWIDTH        OS_CFG_SSSP_MODULEIDWIDTH
#endif

/**
 * @brief   Flag to set the module as SSSP master.
 * @details There must be only one module with this flag set to true in a
 *          system.
 */
#if !defined(OS_CFG_SSSP_MASTER)
  #define AMIROOS_CFG_SSSP_MASTER               false
#else /* !defined(OS_CFG_SSSP_MASTER) */
  #define AMIROOS_CFG_SSSP_MASTER               OS_CFG_SSSP_MASTER
#endif /* !defined(OS_CFG_SSSP_MASTER) */

/**
 * @brief   Flag to set the module to be the first in the stack.
 * @details There must be only one module with this flag set to true in a
 *          system.
 */
#if !defined(OS_CFG_SSSP_STACK_START)
  #define AMIROOS_CFG_SSSP_STACK_START          true
#else /* !defined(OS_CFG_SSSP_STACK_START) */
  #define AMIROOS_CFG_SSSP_STACK_START          OS_CFG_SSSP_STACK_START
#endif /* !defined(OS_CFG_SSSP_STACK_START) */

/**
 * @brief   Flag to set the module to be the last in the stack.
 * @details There must be only one module with this flag set to true in a
 *          system.
 */
#if !defined(OS_CFG_SSSP_STACK_END)
  #define AMIROOS_CFG_SSSP_STACK_END            false
#else /* !defined(OS_CFG_SSSP_STACK_END) */
  #define AMIROOS_CFG_SSSP_STACK_END            OS_CFG_SSSP_STACK_END
#endif /* !defined(OS_CFG_SSSP_STACK_END) */

/**
 * @brief   Delay time (in microseconds) how long a SSSP signal must be active.
 */
#if !defined(OS_CFG_SSSP_SIGNALDELAY)
  #define AMIROOS_CFG_SSSP_SIGNALDELAY          1000
#else /* !defined(OS_CFG_SSSP_SIGNALDELAY) */
  #define AMIROOS_CFG_SSSP_SIGNALDELAY          OS_CFG_SSSP_SIGNALDELAY
#endif /* !defined(OS_CFG_SSSP_SIGNALDELAY) */

/**
 * @brief   Time boundary for robot wide clock synchronization in microseconds.
 * @details Whenever the SSSP S (snychronization) signal gets logically
 *          deactivated, all modules need to align their local uptime to the
 *          nearest multiple of this value.
 *          Setting this value to 0 deactivates clock synchronization.
 */
#if !defined(OS_CFG_SSSP_SYSSYNCPERIOD)
  #define AMIROOS_CFG_SSSP_SYSSYNCPERIOD        1000000
#else /* !defined(OS_CFG_SSSP_SYSSYNCPERIOD) */
  #define AMIROOS_CFG_SSSP_SYSSYNCPERIOD        OS_CFG_SSSP_SYSSYNCPERIOD
#endif /* !defined(OS_CFG_SSSP_SYSSYNCPERIOD) */

/** @} */

/*===========================================================================*/
/**
 * @name System shell options
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Shell enable flag.
 */
#if !defined(OS_CFG_SHELL_ENABLE)
  #define AMIROOS_CFG_SHELL_ENABLE              true
#else /* !defined(OS_CFG_SHELL_ENABLE) */
  #define AMIROOS_CFG_SHELL_ENABLE              OS_CFG_SHELL_ENABLE
#endif /* !defined(OS_CFG_SHELL_ENABLE) */

/**
 * @brief   Shell thread stack size.
 */
#if !defined(OS_CFG_SHELL_STACKSIZE)
  #define AMIROOS_CFG_SHELL_STACKSIZE           1024
#else /* !defined(OS_CFG_SHELL_STACKSIZE) */
  #define AMIROOS_CFG_SHELL_STACKSIZE           OS_CFG_SHELL_STACKSIZE
#endif /* !defined(OS_CFG_SHELL_STACKSIZE) */

/**
 * @brief   Shell thread priority.
 * @details Thread priorities are specified as an integer value.
 *          Predefined ranges are:
 *            lowest  ┌ THD_LOWPRIO_MIN
 *                    │ ...
 *                    └ THD_LOWPRIO_MAX
 *                    ┌ THD_NORMALPRIO_MIN
 *                    │ ...
 *                    └ THD_NORMALPRIO_MAX
 *                    ┌ THD_HIGHPRIO_MIN
 *                    │ ...
 *                    └ THD_HIGHPRIO_MAX
 *                    ┌ THD_RTPRIO_MIN
 *                    │ ...
 *            highest └ THD_RTPRIO_MAX
 */
#if !defined(OS_CFG_SHELL_THREADPRIO)
  #define AMIROOS_CFG_SHELL_THREADPRIO          AOS_THD_NORMALPRIO_MIN
#else /* !defined(OS_CFG_SHELL_THREADPRIO) */
  #define AMIROOS_CFG_SHELL_THREADPRIO          OS_CFG_SHELL_THREADPRIO
#endif /* !defined(OS_CFG_SHELL_THREADPRIO) */

/**
 * @brief   Shell maximum input line length.
 */
#if !defined(OS_CFG_SHELL_LINEWIDTH)
  #define AMIROOS_CFG_SHELL_LINEWIDTH           64
#else /* !defined(OS_CFG_SHELL_LINEWIDTH) */
  #define AMIROOS_CFG_SHELL_LINEWIDTH           OS_CFG_SHELL_LINEWIDTH
#endif /* !defined(OS_CFG_SHELL_LINEWIDTH) */

/**
 * @brief   Shell maximum number of arguments.
 */
#if !defined(OS_CFG_SHELL_MAXARGS)
  #define AMIROOS_CFG_SHELL_MAXARGS             8
#else /* !defined(OS_CFG_SHELL_MAXARGS) */
  #define AMIROOS_CFG_SHELL_MAXARGS             OS_CFG_SHELL_MAXARGS
#endif /* !defined(OS_CFG_SHELL_MAXARGS) */

/**
 * @brief   Shell number of history entries.
 * @details A value of 0 disables shell history.
 */
#if !defined(OS_CFG_SHELL_HISTLENGTH)
  #define AMIROOS_CFG_SHELL_HISTLENGTH          3
#else /* !defined(OS_CFG_SHELL_HISTLENGTH) */
  #define AMIROOS_CFG_SHELL_HISTLENGTH          OS_CFG_SHELL_HISTLENGTH
#endif /* !defined(OS_CFG_SHELL_HISTLENGTH) */

/**
 * @brief   Shell remote access enable flag.
 * @details If enabled, AMiRo-OS shells support remote access.
 */
#if !defined(OS_CFG_SHELL_REMOTE_ENABLE)
  #define AMIROOS_CFG_SHELL_REMOTE_ENABLE       true
#else /* !defined(OS_CFG_SHELL_REMOTE_ENABLE) */
  #define AMIROOS_CFG_SHELL_REMOTE_ENABLE       OS_CFG_SHELL_REMOTE_ENABLE
#endif /* !defined(OS_CFG_SHELL_REMOTE_ENABLE) */

/** @} */

#endif /* AOSCONF_H */

/** @} */
