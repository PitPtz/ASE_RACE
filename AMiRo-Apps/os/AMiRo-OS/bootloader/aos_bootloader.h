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
 * @file    aos_bootloader.h
 * @brief   Bootloader wrapper header.
 *
 * @addtogroup bootloader
 * @{
 */

#ifndef AMIROOS_BOOTLOADER_H
#define AMIROOS_BOOTLOADER_H

#include <aosconf.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief   Identifier for the case that no bootloader is installed/available.
 */
#define AOS_BOOTLOADER_NONE                     -1

/**
 * @brief   Identifier for AMiRo-BLT
 * @note    The version of the installed AMiRo-BLT can be retrieved via the
 *          AMiRo-BLT interface.
 */
#define AOS_BOOTLOADER_AMiRoBLT                 1

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

#if (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_AMiRoBLT)
#include <amiroblt.h>
#endif

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

#if (AMIROOS_CFG_BOOTLOADER != AOS_BOOTLOADER_NONE) &&                          \
    (AMIROOS_CFG_BOOTLOADER != AOS_BOOTLOADER_AMiRoBLT)
# error "AMIROOS_CFG_BOOTLOADER set to invalid value in aosconf.h"
#endif

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROOS_BOOTLOADER_H */

/** @} */
