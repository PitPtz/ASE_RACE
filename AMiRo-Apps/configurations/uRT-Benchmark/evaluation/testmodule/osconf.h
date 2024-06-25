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
 * @file    osconf.h
 * @brief   OS configuration file for the NUCLEO-L476RG module.
 * @details Contains the module specific AMiRo-OS settings.
 *
 * @addtogroup configs_urtbenchmark_modules_testmodule
 * @{
 */

#ifndef OSCONF_H
#define OSCONF_H

/*===========================================================================*/
/**
 * @name Kernel parameters and options
 * @{
 */
/*===========================================================================*/

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

/** @} */

/*===========================================================================*/
/**
 * @name main function hooks
 * @{
 */
/*===========================================================================*/

#if !defined(TESTMODULE_OSONLY)

/**
 * @brief   Custom header to be included in aos_main.cpp file.
 */
#define AMIROOS_CFG_MAIN_EXTRA_INCLUDE_HEADER   urt.h

/**
 * @brief   Control thread hook to initialize µRT and all apps.
 */
#define AMIROOS_CFG_MAIN_INIT_HOOK_2() {                                        \
  urtCoreInit(NULL);                                                            \
}

/**
 * @brief   Control thread hook to stop all µRT nodes.
 */
#define AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1() {                                    \
  urtCoreStopNodes(URT_STATUS_OK);                                              \
}

#endif /* !defined(TESTMODULE_OSONLY) */

/** @} */

#endif /* OSCONF_H */

/** @} */
