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
 * @file    alld_TPS6211x.h
 * @brief   Step-Down Converter macros and structures.
 *
 * @defgroup lld_TPS6211x_v1 Version 1
 * @brief   Version 1
 * @ingroup lld_TPS6211x
 *
 * @addtogroup lld_TPS6211x_v1
 * @{
 */

#ifndef AMIROLLD_TPS6211X_H
#define AMIROLLD_TPS6211X_H

#include <amiro-lld.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief   Power pin active state.
 */
#define TPS6211x_LLD_POWER_ACTIVE_STATE   APAL_GPIO_ACTIVE_HIGH

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

/**
 * @brief The TPS6211x driver struct.
 */
typedef struct {
  const apalControlGpio_t* gpio_power;  /**< GPIO to turn power on/off */
} TPS6211xDriver;

/**
 * @brief Power enable enum.
 */
typedef enum {
  TPS6211x_LLD_POWER_ENABLED  = 0x01,
  TPS6211x_LLD_POWER_DISABLED = 0x00,
} tps6211x_lld_power_en_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
  apalExitStatus_t tps6211x_lld_get_power_en(const TPS6211xDriver* const tps6211x, tps6211x_lld_power_en_t* const power);
  apalExitStatus_t tps6211x_lld_set_power_en(const TPS6211xDriver* const tps6211x, const tps6211x_lld_power_en_t power);
#ifdef __cplusplus
}
#endif

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROLLD_TPS6211X_H */

/** @} */

