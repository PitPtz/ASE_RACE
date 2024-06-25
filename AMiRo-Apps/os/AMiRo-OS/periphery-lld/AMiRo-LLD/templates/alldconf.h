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
 * @file    alldconf.h
 * @brief   AMiRo-LLD configuration header template.
 *
 * @defgroup config Configuration
 * @brief   AMiRo-LLD configuration.
 * @details AMiRo-LLD can be configured via several feature flags and settings.
 *
 * @addtogroup config
 * @{
 */

#ifndef ALLDCONF_H
#define ALLDCONF_H

/*
 * Compatibility guards.
 */
#define _AMIRO_LLD_CFG_
#define AMIRO_LLD_CFG_VERSION_MAJOR             1
#define AMIRO_LLD_CFG_VERSION_MINOR             1

/**
 * @brief   Width of the apalTime_t data type.
 *
 * @details Possible values are 8, 16, 32, and 64 bits.
 *          By definition time is represented ot a microsecond precision.
 */
#define AMIROLLD_CFG_TIME_SIZE                  32

/**
 * @brief   Flag to enable/disable DBG API.
 */
#define AMIROLLD_CFG_DBG                        true

/**
 * @brief   Flag to enable/disable GPIO API.
 */
#define AMIROLLD_CFG_GPIO                       true

/**
 * @brief   Flag to enable/disable PWM API.
 */
#define AMIROLLD_CFG_PWM                        true

/**
 * @brief   Flag to enable/disable QEI API.
 */
#define AMIROLLD_CFG_QEI                        true

/**
 * @brief   Flag to enable/disable I2C API.
 */
#define AMIROLLD_CFG_I2C                        true

/**
 * @brief   Flag to enable/disable SPI API.
 */
#define AMIROLLD_CFG_SPI                        true

/**
 * @brief   Hook macro to insert a custom header to periphAL.h file.
 */
#define AMIROLLD_CFG_PERIPHAL_HEADER            custom_periphAL.h

#endif /* ALLDCONF_H */

/** @} */
