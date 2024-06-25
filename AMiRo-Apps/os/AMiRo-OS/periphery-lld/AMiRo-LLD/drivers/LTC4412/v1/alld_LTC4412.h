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
 * @file    alld_LTC4412.h
 * @brief   PowerPath Controller macros and structures.
 *
 * @defgroup lld_LTC4412_v1 Version 1
 * @brief   Version 1
 * @ingroup lld_LTC4412
 *
 * @addtogroup lld_LTC4412_v1
 * @{
 */

#ifndef AMIROLLD_LTC4412_H
#define AMIROLLD_LTC4412_H

#include <amiro-lld.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief CTRL GPIO active state.
 */
#define LTC4412_LLD_CTRL_ACTIVE_STATE   APAL_GPIO_ACTIVE_LOW

/**
 * @brief STAT GPIO active state.
 */
#define LTC4412_LLD_STAT_ACTIVE_STATE   APAL_GPIO_ACTIVE_HIGH

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
 * @brief LTC4412 driver struct.
 */
typedef struct {
  const apalControlGpio_t* gpio_ctrl;   /**< @brief Control GPIO */
  const apalControlGpio_t* gpio_stat;   /**< @brief Status GPIO */
} LTC4412Driver;

typedef enum {
  LTC4412_LLD_CTRL_INACTIVE = 0x0,
  LTC4412_LLD_CTRL_ACTIVE   = 0x1,
} ltc4412_lld_ctrl_t;

typedef enum {
  LTC4412_LLD_STAT_INACTIVE = 0x0,
  LTC4412_LLD_STAT_ACTIVE   = 0x1,
} ltc4412_lld_stat_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
  apalExitStatus_t ltc4412_lld_get_ctrl(const LTC4412Driver* const ltc4412, ltc4412_lld_ctrl_t* const ctrl);
  apalExitStatus_t ltc4412_lld_set_ctrl(const LTC4412Driver* const ltc4412, const ltc4412_lld_ctrl_t ctrl);
  apalExitStatus_t ltc4412_lld_get_stat(const LTC4412Driver* const ltc4412, ltc4412_lld_stat_t* const stat);
#ifdef __cplusplus
}
#endif

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROLLD_LTC4412_H */

/** @} */

