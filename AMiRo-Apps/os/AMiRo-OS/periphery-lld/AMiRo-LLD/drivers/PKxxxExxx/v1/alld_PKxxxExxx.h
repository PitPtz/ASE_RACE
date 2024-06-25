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
 * @file    alld_PKxxxExxx.h
 * @brief   Buzzer macros.
 *
 * @defgroup lld_PKxxxExxx_v1 Version 1
 * @brief   Version 1
 * @ingroup lld_PKxxxExxx
 *
 * @addtogroup lld_PKxxxExxx_v1
 * @{
 */

#ifndef AMIROLLD_PKXXXEXXX_H
#define AMIROLLD_PKXXXEXXX_H

#include <amiro-lld.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

# ifndef PKxxxExxx_LLD_FREQUENCY_SPEC
#   error "PKxxxExxx_LLD_FREQUENCY_SPEC not defined"
# elif !(PKxxxExxx_LLD_FREQUENCY_SPEC > 0)
#   error "PKxxxExxx_LLD_FREQUENCY_SPEC set to invalid value"
# endif

# ifndef PKxxxExxx_LLD_FREQUENCY_MIN
#   warning "PKxxxExxx_LLD_FREQUENCY_MIN not defined (recommended)"
# elif !(PKxxxExxx_LLD_FREQUENCY_MIN > 0)
#   error "PKxxxExxx_LLD_FREQUENCY_MIN set to invalid value"
# endif

# ifndef PKxxxExxx_LLD_FREQUENCY_MAX
#   warning "PKxxxExxx_LLD_FREQUENCY_MAX not defined (recommended)"
# elif !(PKxxxExxx_LLD_FREQUENCY_MAX > 0)
#   error "PKxxxExxx_LLD_FREQUENCY_MAX set to invalid value"
# endif

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
  apalExitStatus_t pkxxxexxx_lld_checkPWMconfiguration(apalPWMDriver_t* pwm);
  apalExitStatus_t pkxxxexxx_lld_enable(apalPWMDriver_t* pwm, const apalPWMchannel_t channel, const bool enable);
#ifdef __cplusplus
}
#endif

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROLLD_PKXXXEXXX_H */

/** @} */

