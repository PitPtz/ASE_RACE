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
 * @file    HelloWorld_osconf.h
 * @brief   OS configuration file for the HelloWorld configuration.
 *
 * @addtogroup configs_helloworld_modules
 * @{
 */

#ifndef HELLOWOLRD_OSCONF_H
#define HELLOWOLRD_OSCONF_H

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
#define OS_CFG_TESTS_ENABLE                     true
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
 * @brief   Custom header to be included in aos_main.cpp file.
 */
#if !defined(AMIROOS_CFG_MAIN_EXTRA_INCLUDE_HEADER)
#define AMIROOS_CFG_MAIN_EXTRA_INCLUDE_HEADER   HelloWorld_apps.h
#endif

/**
 * @brief   Control thread hook to initialize µRT and all apps.
 */
#if !defined(AMIROOS_CFG_MAIN_INIT_HOOK_2)
#define AMIROOS_CFG_MAIN_INIT_HOOK_2()                                          \
  urtCoreInit(NULL);                                                            \
  helloworldAppsInit();
#endif

/**
 * @brief   Control thread hook to start all µRT nodes.
 */
#if !defined(AMIROOS_CFG_MAIN_INIT_HOOK_3)
#define AMIROOS_CFG_MAIN_INIT_HOOK_3()                                          \
  event_listener_t urtEmergencyListener;                                        \
  chEvtRegisterMaskWithFlags(urtCoreGetEvtSource(), &urtEmergencyListener, MAIN_EMERGENCY_EVENT, URT_CORE_EVENTFLAG_EMERGENCY); \
  urtCoreStartNodes();
#endif

/**
 * @brief   Control thread hook to stop all µRT nodes.
 */
#if !defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1)
#define AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1()                                      \
  urtCoreStopNodes(URT_STATUS_OK);                                              \
  chEvtUnregister(urtCoreGetEvtSource(), &urtEmergencyListener);
#endif

/** @} */

#endif /* HELLOWOLRD_OSCONF_H */

/** @} */
