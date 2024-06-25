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
 * @file    alld_TLC5947.h
 * @brief   LED Driver macros and structures.
 *
 * @defgroup lld_TLC5947_v1 Version 1
 * @brief   Version 1
 * @ingroup lld_TLC5947
 *
 * @addtogroup lld_TLC5947_v1
 * @{
 */

#ifndef AMIROLLD_TLC5947_H
#define AMIROLLD_TLC5947_H

#include <amiro-lld.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief   Number of chained TLC5947 devices.
 */
#if !defined(TLC5947_LLD_CHAINED) || defined(__DOXYGEN__)
#define TLC5947_LLD_CHAINED                     1
#endif

/**
 * @brief   Active state of the BLANK signal.
 */
#define TLC5947_LLD_BLANK_ACTIVE_STATE          APAL_GPIO_ACTIVE_HIGH

/**
 * @brief   Signal edge of the XLAT signal, which causes the SPI registers to be latched to the output.
 */
#define TLC5947_LLD_XLAT_UPDATE_EDGE            APAL_GPIO_EDGE_RISING

/**
 * @brief   Number of PWM channels.
 */
#define TLC5947_LLD_NUM_CHANNELS                (TLC5947_LLD_CHAINED * 24)

/**
 * @brief   Resulotion of the PWM channels in bits.
 */
#define TLC5947_LLD_PWM_RESOLUTION_BITS         12

/**
 * @brief   Size of the TLC grayscale buffer in bytes.
 */
#define TLC5947_LLD_BUFFER_SIZE                 (TLC5947_LLD_NUM_CHANNELS * TLC5947_LLD_PWM_RESOLUTION_BITS / 8)

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
 * @brief The TLC5947Driver struct
 */
typedef struct {
  apalSPIDriver_t* spi_driver;    /**< @brief The SPI Driver.                           */
  const apalControlGpio_t* blank_gpio;  /**< @brief The identifier of the BLANK signal GPIO (may be @p NULL).  */
  const apalControlGpio_t* xlat_gpio;   /**< @brief The identifier of the XLAT signal GPIO (may be @p NULL).   */
} TLC5947Driver;

/**
 * @brief The blank state of the TLC5947 driver.
 */
typedef enum {
  TLC5947_LLD_BLANK_ENABLE = 0,   /**< 'blank on' state of the TLC5947 driver.  */
  TLC5947_LLD_BLANK_DISABLE = 1,  /**< 'blank off' state of the TLC5947 driver. */
} tlc5947_lld_blank_t;


/**
 * @brief TLC5947 buffer struct.
 */
typedef struct {
  uint8_t data[TLC5947_LLD_BUFFER_SIZE]; /**< The raw buffer data. */
} tlc5947_lld_buffer_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
  apalExitStatus_t tlc5947_lld_setBlank(const TLC5947Driver* const tlc5947, const tlc5947_lld_blank_t blank);
  apalExitStatus_t tlc5947_lld_getBlank(const TLC5947Driver* const tlc5947, tlc5947_lld_blank_t* const blank);
  apalExitStatus_t tlc5947_lld_update(const TLC5947Driver* const tlc5947);
  apalExitStatus_t tlc5947_lld_write(const TLC5947Driver* const tlc5947, const tlc5947_lld_buffer_t* const buffer);
  void tlc5947_lld_setBuffer(tlc5947_lld_buffer_t* const buffer, const uint8_t channel, const uint16_t value);
  uint16_t tlc5947_lld_getBuffer(const tlc5947_lld_buffer_t* const buffer, const uint8_t channel);
#ifdef __cplusplus
}
#endif

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROLLD_TLC5947_H */

/** @} */

