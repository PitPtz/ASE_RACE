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
 * @brief   OS configuration file for the DiWheelDrive (v1.1) AMiRo module.
 * @details Contains the module specific AMiRo-OS settings.
 *
 * @addtogroup configs_helloworld_modules_diwheeldrive11
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

/**
 * @brief   Event mask to identify emergency events.
 */
#define MAIN_EMERGENCY_EVENT                    EVENT_MASK(8)

/**
 * @brief   Main loop event hook to handle µRT emergency shutdown events.
 */
#define AMIROOS_CFG_MAIN_LOOP_EVENTHOOK(eventmask)                              \
  aosDbgAssert(eventmask == MAIN_EMERGENCY_EVENT);                              \
  chEvtGetAndClearFlags(&urtEmergencyListener);                                 \
  shutdown = AOS_SHUTDOWN_DEEPSLEEP;

/** @} */

#include <HelloWorld_osconf.h>

#endif /* OSCONF_H */

/** @} */
