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
 * @file    amiroos.h
 * @brief   AMiRo-OS interface header.
 *
 * @addtogroup core
 * @{
 */

#ifndef AMIROOS_H
#define AMIROOS_H

/**
 * @brief   AMiRo-OS identification macro.
 */
#define _AMIRO_OS_

/**
 * @name   AMiRo-OS version and relase information.
 * @{
 */

/**
 * @brief   The type of this version.
 * @note    Is one of "pre-alpha", "alpha", "beta", "release candidate", "release".
 */
#define AMIROOS_RELEASE_TYPE          "release"

/**
 * @brief   The operating system major version.
 * @note    Changes of the major version imply incompatibilities.
 */
#define AMIROOS_VERSION_MAJOR         2

/**
 * @brief   The operating system minor version.
 * @note    A higher minor version implies new functionalty, but all old interfaces are still available.
 */
#define AMIROOS_VERSION_MINOR         3

/**
 * @brief   The operating system patch level.
 */
#define AMIROOS_VERSION_PATCH         0

/** @} */

/******************************************************************************/
/* CONFIGURATION                                                              */
/******************************************************************************/

#include <aosconf.h>
# if !defined(_AMIRO_OS_CFG_)
#   error "invalid AMiRo-OS configuration file"
# elif (AMIRO_OS_CFG_VERSION_MAJOR != AMIROOS_VERSION_MAJOR) ||                 \
       (AMIRO_OS_CFG_VERSION_MINOR < AMIROOS_VERSION_MINOR)
#   error "incompatible AMiRo-OS configuration file"
#endif

/******************************************************************************/
/* SUBSYSTEMS                                                                 */
/******************************************************************************/

/* Bootloader */
#include "bootloader/aos_bootloader.h"

/* System Kernel (ChibiOS) */
#include <hal.h>
#include <ch.h>

/* Low-Level Drivers (AMiRo-LLD) */
#include <amiro-lld.h>

/******************************************************************************/
/* AMiRo-OS CORE                                                              */
/******************************************************************************/

#include "core/inc/aos_confcheck.h"

#include "core/inc/aos_types.h"
#include "core/inc/aos_debug.h"
#include "core/inc/aos_time.h"
#include "core/inc/aos_timer.h"
#include "core/inc/aos_fbcan.h"
#include "core/inc/aos_iostream.h"
#include "core/inc/aos_shell.h"
#include "core/inc/aos_sssp.h"
#include "core/inc/aos_system.h"
#include "core/inc/aos_thread.h"
#include "core/inc/aos_test.h"

/******************************************************************************/
/* MODULE                                                                     */
/******************************************************************************/

#include <module.h>

#endif /* AMIROOS_H */

/** @} */
