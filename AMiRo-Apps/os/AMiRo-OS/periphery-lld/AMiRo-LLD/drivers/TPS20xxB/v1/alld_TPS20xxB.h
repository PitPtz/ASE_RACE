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
 * @file    alld_TPS20xxB.h
 * @brief   Power-Distribution Switch macros and structures.
 *
 * @defgroup lld_TPS20xxB_v1 Version 1
 * @brief   Version 1
 * @ingroup lld_TPS20xxB
 *
 * @addtogroup lld_TPS20xxB_v1
 * @{
 */

#ifndef AMIROLLD_TPS20XXB_H
#define AMIROLLD_TPS20XXB_H

#include <amiro-lld.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief   Enable pin active state.
 */
#define TPS20xxB_LLD_ENABLE_ACTIVE_STATE        APAL_GPIO_ACTIVE_HIGH

/**
 * @brief   Overcurrent pin active state.
 */
#define TPS20xxB_LLD_OVERCURRENT_ACTIVE_STATE   APAL_GPIO_ACTIVE_LOW

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
 * @brief The TPS20xxB driver struct.
 */
typedef struct {
  const apalControlGpio_t* enable;        /**< @brief GPIO to enable/disable the tps2051b. */
  const apalControlGpio_t* overcurrent;   /**< @brief GPIO to indicate overcurrent */
} TPS20xxBDriver;

/**
 * @brief Enable/Disable enum.
 */
typedef enum {
  TPS20xxB_LLD_ENABLE = 0x01,
  TPS20xxB_LLD_DISABLE = 0x00,
} tps20xxb_lld_enable_t;

/**
 * @brief Overcurrent enum.
 */
typedef enum {
  TPS20xxB_LLD_OVERCURRENT = 0x01,
  TPS20xxB_LLD_NO_OVERCURRENT = 0x00,
} tps20xxb_lld_overcurrent_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
  apalExitStatus_t tps20xxb_lld_set_enable(const TPS20xxBDriver* const tps20xxb, const tps20xxb_lld_enable_t enable);
  apalExitStatus_t tps20xxb_lld_read_enable(const TPS20xxBDriver* const tps20xxb, tps20xxb_lld_enable_t* const enable);
  apalExitStatus_t tps20xxb_lld_read_overcurrent(const TPS20xxBDriver* const tps20xxb, tps20xxb_lld_overcurrent_t* const oc);
#ifdef __cplusplus
}
#endif

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROLLD_TPS20XXB_H */

/** @} */

