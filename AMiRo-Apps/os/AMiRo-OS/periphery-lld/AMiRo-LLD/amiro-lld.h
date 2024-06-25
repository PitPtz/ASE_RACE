/*
AMiRo-LLD is a compilation of low-level hardware drivers for the Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2016..2022  Thomas Schöpping et al.

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
 * @file    amiro-lld.h
 * @brief   AMiRo-LLD interface header.
 */

#ifndef AMIROLLD_H
#define AMIROLLD_H

/**
 * @brief   AMIRO-LLD identification macro.
 */
#define _AMIRO_LLD_

/*===========================================================================*/
/**
 * @name   AMiRo-LLD version and relase information.
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Realease type of this version.
 * @note    Possible values are "pre-alpha", "alpha", "beta", "release
 *          candidate", and "release".
 */
#define AMIROLLD_RELEASE_TYPE                   "release"

/**
 * @brief   The AMiRo-LLD major version.
 * @note    Changes of the major version imply incompatibilities.
 */
#define AMIROLLD_VERSION_MAJOR                  1

/**
 * @brief   The AMiRo-LLD minor version.
 * @note    A higher minor version implies new functionalty, but all old
 *          interfaces are still available.
 */
#define AMIROLLD_VERSION_MINOR                  1

/**
 * @brief   The AMiRo-LLD patch level.
 */
#define AMIROLLD_VERSION_PATCH                  0

/** @} */

/******************************************************************************/
/* CONFIGURATION & VERIFICATION                                               */
/******************************************************************************/

#include <alldconf.h>

# if !defined(_AMIRO_LLD_CFG_)
#   error "invalid AMiRo-LLD configuration file"
# elif (AMIRO_LLD_CFG_VERSION_MAJOR != AMIROLLD_VERSION_MAJOR) ||               \
       (AMIRO_LLD_CFG_VERSION_MINOR < AMIROLLD_VERSION_MINOR)
#   error "incompatible AMiRo-LLD configuration file"
# endif

# if !defined(AMIROLLD_CFG_TIME_SIZE)
#   error "AMIROLLD_CFG_TIME_SIZE not defined in alldconf.h"
# endif /* !defined(AMIROLLD_CFG_TIME_SIZE) */

# if !defined(AMIROLLD_CFG_DBG)
#   error "AMIROLLD_CFG_DBG not defined in alldconf.h"
# endif /* !defined(AMIROLLD_CFG_DBG) */

# if !defined(AMIROLLD_CFG_GPIO)
#   error "AMIROLLD_CFG_GPIO not defined in alldconf.h"
# endif /* !defined(AMIROLLD_CFG_GPIO) */

# if !defined(AMIROLLD_CFG_PWM)
#   error "AMIROLLD_CFG_PWM not defined in alldconf.h"
# endif /* !defined(AMIROLLD_CFG_PWM) */

# if !defined(AMIROLLD_CFG_QEI)
#   error "AMIROLLD_CFG_QEI not defined in alldconf.h"
# endif /* !defined(AMIROLLD_CFG_QEI) */

# if !defined(AMIROLLD_CFG_I2C)
#   error "AMIROLLD_CFG_I2C not defined in alldconf.h"
# endif /* !defined(AMIROLLD_CFG_I2C) */

# if !defined(AMIROLLD_CFG_SPI)
#   error "AMIROLLD_CFG_SPI not defined in alldconf.h"
# endif /* !defined(AMIROLLD_CFG_SPI) */

# if ((AMIROLLD_CFG_GPIO == true) || \
      (AMIROLLD_CFG_PWM == true)  || \
      (AMIROLLD_CFG_QEI == true)  || \
      (AMIROLLD_CFG_I2C == true)  || \
      (AMIROLLD_CFG_SPI == true)) && \
     !defined(AMIROLLD_CFG_PERIPHAL_HEADER)
#   error "AMIROLLD_CFG_PERIPHAL_HEADER required but not defined in alldconf.h"
# endif

/******************************************************************************/
/* PERIPHERY ABSTRACTION LAYER (periphAL)                                     */
/******************************************************************************/

#include <periphAL.h>

#endif /* AMIROLLD_H */
