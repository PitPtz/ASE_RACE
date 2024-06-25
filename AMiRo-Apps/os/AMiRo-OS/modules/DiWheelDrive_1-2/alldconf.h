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
 * @brief   AMiRo-LLD configuration file for the DiWheelDrive v1.2 module.
 * @details Contains the application specific AMiRo-LLD settings.
 *
 * @defgroup diwheeldrive12_cfg_amirolld AMiRo-LLD
 * @ingroup diwheeldrive12_cfg
 *
 * @brief   Todo
 * @details Todo
 *
 * @addtogroup diwheeldrive12_cfg_amirolld
 * @{
 */

#ifndef ALLDCONF_H
#define ALLDCONF_H

/* 
 * common configuration
 */
#include "../aos_alldconf.h"
#include "../../../core/inc/aos_time.h"


/*
 * Driver configuration of the BNO055
 */
#define BNO055I2CDRIVER  &I2CD1
#define BNO055I2CTIMEOUT MICROSECONDS_PER_SECOND

#endif /* ALLDCONF_H */

/** @} */
