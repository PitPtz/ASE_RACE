/*
AMiRo-Apps is a collection of applications and configurations for the
Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2018..2022  Thomas Schöpping et al.

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
 * @file    osconf.h
 * @brief   OS configuration file for the LightRing (v1.2) AMiRo module.
 * @details Contains the module specific AMiRo-OS settings.
 *
 * @addtogroup configs_amirodefault_modules_lightring12
 * @{
 */

#ifndef OSCONF_H
#define OSCONF_H

/*===========================================================================*/
/**
 * @name main function hooks
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Custom header to be included in aos_main.cpp file.
 */
#define AMIROOS_CFG_MAIN_EXTRA_INCLUDE_HEADER   apps.h

/** @} */

#include <AMiRoDefault_osconf.h>

#endif /* OSCONF_H */

/** @} */
