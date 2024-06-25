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
 * @file
 * @brief   Common AMiRo-LLD configuration file for all modules.
 * @details Contains common AMiRo-LLD settings.
 *
 * @addtogroup modules_configs_amirolld
 * @{
 */

#ifndef AOS_ALLDCONF_H
#define AOS_ALLDCONF_H

#define _AMIRO_LLD_CFG_
#define AMIRO_LLD_CFG_VERSION_MAJOR             1
#define AMIRO_LLD_CFG_VERSION_MINOR             1

#include <aosconf.h>
#include <hal.h>

/**
 * @brief   Width of the apalTime_t data type.
 *
 * @details Possible values are 8, 16, 32, and 64 bits.
 *          By definition time is represented at microsecond precision.
 */
# if !defined(AMIROLLD_CFG_TIME_SIZE)
#   define AMIROLLD_CFG_TIME_SIZE               32
# endif

/**
 * @brief   Flag to enable/disable DBG API.
 */
# if !defined(AMIROLLD_CFG_DBG)
#  define AMIROLLD_CFG_DBG                      AMIROOS_CFG_DBG
# endif

/**
 * @brief   Flag to enable/disable GPIO API.
 */
# if !defined(AMIROLLD_CFG_GPIO)
#   if (HAL_USE_PAL == TRUE)
#     define AMIROLLD_CFG_GPIO                  true
#   else
#     define AMIROLLD_CFG_GPIO                  false
#   endif
# endif

/**
 * @brief   Flag to enable/disable PWM API.
 */
# if !defined(AMIROLLD_CFG_PWM)
#   if (HAL_USE_PWM == TRUE)
#     define AMIROLLD_CFG_PWM                   true
#   else
#     define AMIROLLD_CFG_PWM                   false
#   endif
# endif

/**
 * @brief   Flag to enable/disable QEI API.
 */
# if !defined(AMIROLLD_CFG_QEI)
#   if (HAL_USE_QEI == TRUE)
#     define AMIROLLD_CFG_QEI                   true
#   else
#     define AMIROLLD_CFG_QEI                   false
#   endif
# endif

/**
 * @brief   Flag to enable/disable I2C API.
 */
# if !defined(AMIROLLD_CFG_I2C)
#   if (HAL_USE_I2C == TRUE)
#     define AMIROLLD_CFG_I2C                   true
#   else
#     define AMIROLLD_CFG_I2C                   false
#   endif
# endif

/**
 * @brief   Flag to enable/disable SPI API.
 */
# if !defined(AMIROLLD_CFG_SPI)
#   if (HAL_USE_SPI == TRUE)
#     define AMIROLLD_CFG_SPI                   true
#   else
#     define AMIROLLD_CFG_SPI                   false
#   endif
# endif

/**
 * @brief   Hook macro to insert a custom header to periphAL.h file.
 */
# if !defined(AMIROLLD_CFG_PERIPHAL_HEADER)
#   define AMIROLLD_CFG_PERIPHAL_HEADER          periphery-lld/aos_periphAL.h
# endif

#endif /* AOS_ALLDCONF_H */

/** @} */
