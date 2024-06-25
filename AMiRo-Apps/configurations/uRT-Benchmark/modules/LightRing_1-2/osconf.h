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
 * @brief   OS configuration file for the LightRing v1.2 module.
 * @details Contains the module specific AMiRo-OS settings.
 *
 * @addtogroup configs_urtbenchmark_modules_lightring12
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

/**
 * @brief   Set time delta (in system ticks = microseconds) as low as possible.
 * @note    Number of µRT nodes increases locked regions significantly. However,
 *          due to the very controlled setup, this setting can be kept rather
 *          low nevertheless.
 */
#if !defined(CH_CFG_ST_TIMEDELTA)
#define CH_CFG_ST_TIMEDELTA                     250
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

/** @} */

/*===========================================================================*/
/**
 * @name main function hooks
 * @{
 */
/*===========================================================================*/

/** @} */

#include <uRT_benchmark_osconf.h>

#endif /* OSCONF_H */

/** @} */
