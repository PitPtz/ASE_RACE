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
 * @file    alld_DW1000.h
 * @brief   UWB transceiver mocros and structures.
 *
 * @defgroup lld_DW1000_v2 Version 2
 * @brief   Version 2
 * @ingroup lld_DW1000
 *
 * @addtogroup lld_DW1000_v2
 * @{
 */

#ifndef AMIROLLD_DW1000_H
#define AMIROLLD_DW1000_H

#include <amiro-lld.h>

#include "decadriver/deca_regs.h"
#include "decadriver/deca_device_api.h"

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#define DW1000_LLD_SPIBUFLEN                    32

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

typedef struct {
  apalSPIDriver_t* spid;
  const apalControlGpio_t* gpio_exti;
  const apalControlGpio_t* gpio_reset;
} DW1000Driver;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

extern DW1000Driver dw1000;

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROLLD_DW1000_H */

/** @} */

