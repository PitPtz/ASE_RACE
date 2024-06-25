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
 * @file    chconf.h
 * @brief   ChibiOS Configuration file for the DiWheelDrive v1.2 module.
 * @details Contains the application specific kernel settings.
 *
 * @defgroup diwheeldrive12_cfg_chibiosrt ChibiOS/RT
 * @ingroup diwheeldrive12_cfg
 *
 * @brief   Todo
 * @details Todo
 *
 * @addtogroup diwheeldrive12_cfg_chibiosrt
 * @{
 */

#ifndef CHCONF_H
#define CHCONF_H

#include <aosconf.h>

/*===========================================================================*/
/**
 * @name System settings
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Handling of instances.
 * @note    If enabled then threads assigned to various instances can
 *          interact each other using the same synchronization objects.
 *          If disabled then each OS instance is a separate world, no
 *          direct interactions are handled by the OS.
 */
#if !defined(CH_CFG_SMP_MODE)
#define CH_CFG_SMP_MODE                         FALSE
#endif

/** @} */

/*===========================================================================*/
/**
 * @name System timers settings
 * @{
 */
/*===========================================================================*/

/**
 * @brief   System time counter resolution.
 * @note    Allowed values are 16 or 32 bits.
 */
#if !defined(CH_CFG_ST_RESOLUTION)
#define CH_CFG_ST_RESOLUTION                    16
#endif

/**
 * @brief   Time delta constant for the tick-less mode.
 * @note    If this value is zero then the system uses the classic
 *          periodic tick. This value represents the minimum number
 *          of ticks that is safe to specify in a timeout directive.
 *          The value one is not valid, timeouts are rounded up to
 *          this value.
 */
#if !defined(CH_CFG_ST_TIMEDELTA)
#define CH_CFG_ST_TIMEDELTA                     50
#endif

// more common definitions in aos_chconf.h

/** @} */

/*===========================================================================*/
/**
 * @name Kernel parameters and options
 * @{
 */
/*===========================================================================*/

// common definitions in aos_chconf.h

/** @} */

/*===========================================================================*/
/**
 * @name Performance options
 * @{
 */
/*===========================================================================*/

// common definitions in aos_chconf.h

/** @} */

/*===========================================================================*/
/**
 * @name Subsystem options
 * @{
 */
/*===========================================================================*/

// common definitions in aos_chconf.h

/** @} */

/*===========================================================================*/
/**
 * @name OSLIB options
 * @{
 */
/*===========================================================================*/

// common definitions in aos_chconf.h

/** @} */

/*===========================================================================*/
/**
 * @name Objects factory options
 * @{
 */
/*===========================================================================*/

// common definitions in aos_chconf.h

/** @} */

/*===========================================================================*/
/**
 * @name Debug options
 * @{
 */
/*===========================================================================*/

// common definitions in aos_chconf.h

/** @} */

/*===========================================================================*/
/**
 * @name Kernel hooks
 * @{
 */
/*===========================================================================*/

// common definitions in aos_chconf.h

/** @} */

/*===========================================================================*/
/**
 * @name Port-specific settings (override port settings defaulted in chcore.h).
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Flag to enable/disable sleep mode when the system is idle.
 */
#define CORTEX_ENABLE_WFI_IDLE                  TRUE

/** @} */

/*===========================================================================*/
/**
 * @name other
 * @{
 */
/*===========================================================================*/

// common definitions in aos_chconf.h

/** @} */

#include "../aos_chconf.h"

#endif  /* CHCONF_H */

/** @} */
