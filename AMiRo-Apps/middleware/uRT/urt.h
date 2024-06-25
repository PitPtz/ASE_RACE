/*
µRT is a lightweight publish-subscribe & RPC middleware for real-time
applications. It was developed as part of the software habitat for the
Autonomous Mini Robot (AMiRo) but can be used for other purposes as well.

Copyright (C) 2018..2022  Thomas Schöpping et al.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    urt.h
 * @brief   µRT interface header.
 *
 * @addtogroup kernel
 * @{
 */

#ifndef URT_H
#define URT_H

/**
 * @brief   µRT identification macro.
 */
#define _uRT_

/*============================================================================*/
/**
 * @name    µRT version and release information.
 * @{
 */
/*============================================================================*/

/**
 * @brief   Release type of this version.
 * @note    Possible values are "pre-alpha", "alpha", "beta", "release candidate", and "release".
 */
#define URT_RELEASE_TYPE          "release"

/**
 * @brief   The µRT major version.
 * @note    Changes of the major version imply incompatibilities.
 */
#define URT_VERSION_MAJOR         1

/**
 * @brief   The µRT minor version.
 * @note    A higher minor version implies new functionalities, but all old interfaces are still available.
 */
#define URT_VERSION_MINOR         2

/**
 * @brief   The µRT patch level.
 */
#define URT_VERSION_PATCH         0

/**
 * @brief   The µRT operating system abstraction layer interface required major version.
 * @note    Any other major version is assumed to be incompatible.
 */
#define URT_OSAL_REQUIRED_MAJOR   1

/**
 * @brief   The µRT operating system abstraction layer interface required minor version.
 * @note    Higher minor version values are assumed to be compatible, too.
 */
#define URT_OSAL_REQUIRED_MINOR   0

/** @} */

/*============================================================================*/
/* CONFIGURATION & VERIFICATION                                               */
/*============================================================================*/

// load config
#include <urt_confoptions.h>
#include <urtconf.h>
// check config version compatibility
#if !defined(_URT_CFG_)
# error "invalid µRT configuration file"
#elif (URT_CFG_VERSION_MAJOR != URT_VERSION_MAJOR) || (URT_CFG_VERSION_MINOR < URT_VERSION_MINOR)
# error "incompatible µRT configuration file"
#endif
// check configuration
#include <urt_confcheck.h>

// load primitive data types
#include <urt_primitives.h>

// load OSAL interface header
#include <urt_osal.h>
// check OSAL version compatibility
#if !defined(URT_OSAL_VERSION_MAJOR) || (URT_OSAL_VERSION_MAJOR != URT_OSAL_REQUIRED_MAJOR) || \
    !defined(URT_OSAL_VERSION_MINOR) || (URT_OSAL_VERSION_MINOR < URT_OSAL_REQUIRED_MINOR)
# error "incompatible µRT OSAL implementation"
#endif

// load µRT components
#include <urt_node.h>
#include <urt_sync.h>

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
#include <urt_message.h>
#include <urt_topic.h>
#include <urt_publisher.h>
#include <urt_subscriber.h>
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
#include <urt_request.h>
#include <urt_service.h>
#endif /* (URT_CFG_RPC_ENABLED == true) */

#include <urt_core.h>

#endif /* URT_H */

/** @} */
