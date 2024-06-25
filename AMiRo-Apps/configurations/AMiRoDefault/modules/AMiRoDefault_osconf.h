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
 * @file    AMiRoDefault_osconf.h
 * @brief   OS configuration file for the AMiRoDefault configuration.
 *
 * @addtogroup configs_amirodefault_modules
 * @{
 */

#ifndef AMiRoDefault_OSCONF_H
#define AMiRoDefault_OSCONF_H

/*
 * compatibility guards
 */
#if !defined(_AMIRO_OS_CFG_) ||                                                 \
    !defined(AMIRO_OS_CFG_VERSION_MAJOR) || (AMIRO_OS_CFG_VERSION_MAJOR != 2) ||\
    !defined(AMIRO_OS_CFG_VERSION_MINOR) || (AMIRO_OS_CFG_VERSION_MAJOR < 1)
# error "No or incompatible AMiRo-OS configuration detected"
#endif

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
#define OS_CFG_DBG                              true
#endif

/**
 * @brief   Flag to enable/disable unit tests.
 * @note    Setting this flag will implicitely enable the shell.
 */
#if !defined(OS_CFG_TESTS_ENABLE)
#define OS_CFG_TESTS_ENABLE                     false
#endif

/** @} */

/*===========================================================================*/
/**
 * @name Bootloader configuration
 * @{
 */
/*===========================================================================*/

/** @} */

/*===========================================================================*/
/**
 * @name SSSP (Startup Shutdown Synchronization Protocol) configuration.
 * @{
 */
/*===========================================================================*/

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
#define OS_CFG_SHELL_ENABLE                     true
#endif

/** @} */

/*===========================================================================*/
/**
 * @name main function hooks
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Event mask to identify emergency events.
 */
#define MAIN_EMERGENCY_EVENT                    EVENT_MASK(8)

/**
 * @brief   Control thread hook to initialize µRT and all apps.
 */
#define AMIROOS_CFG_MAIN_INIT_HOOK_2()                                          \
  urtCoreInit(&robotsync);                                                      \
  appsInit();

/**
 * @brief   Control thread hook to start all µRT nodes.
 */
#define AMIROOS_CFG_MAIN_INIT_HOOK_5()                                          \
  event_listener_t urtEmergencyListener;                                        \
  chEvtRegisterMaskWithFlags(urtCoreGetEvtSource(), &urtEmergencyListener, MAIN_EMERGENCY_EVENT, URT_CORE_EVENTFLAG_EMERGENCY); \
  urtCoreStartNodes();                                                          \
  AMiRoDefault_MAIN_INIT_HOOK();
#if !defined(AMiRoDefault_MAIN_INIT_HOOK)
# define AMiRoDefault_MAIN_INIT_HOOK()
#endif

/**
 * @brief   Main loop event hook to handle µRT emergency shutdown events.
 */
#define AMIROOS_CFG_MAIN_LOOP_EVENTHOOK(eventmask)                              \
  aosDbgAssert(eventmask == MAIN_EMERGENCY_EVENT);                              \
  chEvtGetAndClearFlags(&urtEmergencyListener);                                 \
  shutdown = AOS_SHUTDOWN_DEEPSLEEP;

/**
 * @brief   Control thread hook to stop all µRT nodes.
 */
#define AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1()                                      \
  AMiRoDefault_MAIN_SHUTDOWN_HOOK();                                            \
  urtCoreStopNodes(URT_STATUS_OK);                                              \
  chEvtUnregister(urtCoreGetEvtSource(), &urtEmergencyListener);                \
  if (urtCoreGetStatus() != URT_STATUS_OK) {                                    \
    aosprintf("uRT status: %u\n", urtCoreGetStatus());                          \
  }
#if !defined(AMiRoDefault_MAIN_SHUTDOWN_HOOK)
# define AMiRoDefault_MAIN_SHUTDOWN_HOOK()
#endif

/** @} */

#endif /* AMiRoDefault_OSCONF_H */

/** @} */
