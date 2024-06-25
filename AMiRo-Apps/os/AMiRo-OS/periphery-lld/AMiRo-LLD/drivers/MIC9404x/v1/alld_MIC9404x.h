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
 * @file    alld_MIC9404x.h
 * @brief   MIC9404x power switch macros and structures.
 *
 * @defgroup lld_MIC9404x_v1 Version 1
 * @brief   Version 1
 * @ingroup lld_MIC9404x
 *
 * @addtogroup lld_MIC9404x_v1
 * @{
 */

#ifndef AMIROLLD_MIC9404x_H
#define AMIROLLD_MIC9404x_H

#include <amiro-lld.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief Active state of EN GPIO (active high).
 */
#define MIC9404x_LLD_EN_ACTIVE_STATE            APAL_GPIO_ACTIVE_HIGH

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
 * @brief MIC9404x driver data structure.
 */
typedef struct {
  const apalControlGpio_t* en;  /**< @brief The identifier of the EN GPIO.  */
} MIC9404xDriver;

/**
 * @brief The state of the MIC9404x.
 */
typedef enum {
  MIC9404x_LLD_STATE_OFF = 0x00,   /**< 'on' state of the MIC9404x */
  MIC9404x_LLD_STATE_ON  = 0x01,   /**< 'off' state of the MIC9404x */
} mic9404x_lld_state_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
  apalExitStatus_t mic9404x_lld_set(const MIC9404xDriver* const mic9404x, const mic9404x_lld_state_t state);
  apalExitStatus_t mic9404x_lld_get(const MIC9404xDriver* const mic9404x, mic9404x_lld_state_t* const state);
#ifdef __cplusplus
}
#endif

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROLLD_MIC9404x_H */

/** @} */

