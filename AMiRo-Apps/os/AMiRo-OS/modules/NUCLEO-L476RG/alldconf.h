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
 * @file    alldconf.h
 * @brief   AMiRo-LLD configuration file for the NUCLEO-L476RG module.
 * @details Contains the application specific AMiRo-LLD settings.
 *
 * @defgroup nucleol476rg_cfg_amirolld AMiRo-LLD
 * @ingroup nucleol476rg_cfg
 *
 * @brief   Todo
 * @details Todo
 *
 * @addtogroup nucleol476rg_cfg_amirolld
 * @{
 */

#ifndef ALLDCONF_H
#define ALLDCONF_H

/*
 * common configuration
 */
#include "../aos_alldconf.h"

#if defined(BOARD_VL53L1X_CONNECTED) || defined(__DOXYGEN__)

/**
 * @brief   VL53L1X I2C voltage level configuration.
 * @details VL53L1X assumes 1.8V I2C communication by default.
 *          This flag triggers the API to set the device to 2.8V mode.
 */
#define VL53L1X_LLD_I2C_2V8                     true

#endif

#endif /* ALLDCONF_H */

/** @} */
