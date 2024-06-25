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
 * @brief   AMiRo-LLD configuration file for the PowerManagement v1.1 module.
 * @details Contains the application specific AMiRo-LLD settings.
 *
 * @defgroup powermanagement11_cfg_amirolld AMiRo-LLD
 * @ingroup powermanagement11_cfg
 *
 * @brief   Todo
 * @details Todo
 *
 * @addtogroup powermanagement11_cfg_amirolld
 * @{
 */

#ifndef ALLDCONF_H
#define ALLDCONF_H

/**
 * @brief   BQ27500 hardware version
 */
#define BQ27500_LLD_HWVERSION                   120

/**
 * @brief   PKLCS1212E4001 frequency specification
 */
#define PKxxxExxx_LLD_FREQUENCY_MIN             2000
#define PKxxxExxx_LLD_FREQUENCY_SPEC            4000
#define PKxxxExxx_LLD_FREQUENCY_MAX             6000

/*
 * common configuration
 */
#include "../aos_alldconf.h"

#endif /* ALLDCONF_H */

/** @} */
