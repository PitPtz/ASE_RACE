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
 * @file    alld_AT24C01B.h
 * @brief   EEPROM macros and structures.
 *
 * @defgroup lld_AT24C01B_v1 Version 1
 * @brief   Version 1
 * @ingroup lld_AT24C01B
 *
 * @addtogroup lld_AT24C01B_v1
 * @{
 */

#ifndef AMIROLLD_AT24C01B_H
#define AMIROLLD_AT24C01B_H

#include <amiro-lld.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief Memory size of the EEPROM in bits.
 */
#define AT24C01B_LLD_SIZE_BITS              1024

/**
 * @brief Memory size of the EEPROM in bytes
 */
#define AT24C01B_LLD_SIZE_BYTES             128


/**
 * @brief Size of a page in bytes
 */
#define AT24C01B_LLD_PAGE_SIZE_BYTES        8

/**
 * @brief  Time in microseconds a write operation takes to complete (I2C will not respond).
 * @note   The system should wait slightly longer.
 */
#define AT24C01B_LLD_WRITECYCLETIME_US      5000

/**
 * @brief Maximum I2C frequency.
 */
#define AT24C01B_LLD_I2C_MAXFREQUENCY       400000

/**
 * @brief Maximum I2C frequency at 5V.
 */
#define AT24C01B_LLD_I2C_MAXFREQUENCY_5V    1000000


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
 * @brief The AT24C01B driver struct
 */
typedef struct {
  apalI2CDriver_t* i2cd;  /**< @brief The I2C Driver */
  apalI2Caddr_t addr;     /**< @brief The address of the AT24C01B for I2C communication, which is defined by the wiring of the A0, A1, A2 pins */
} AT24C01BDriver;

/**
 * @brief Bitmasks for the I2C address, including the wiring of the A0, A1, A2 pins.
 */
enum {
  AT24C01B_LLD_I2C_ADDR_FIXED       = 0x0050u,
  AT24C01B_LLD_I2C_ADDR_A0          = 0x0001u,
  AT24C01B_LLD_I2C_ADDR_A1          = 0x0002u,
  AT24C01B_LLD_I2C_ADDR_A2          = 0x0004u,
};

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
  apalExitStatus_t at24c01b_lld_poll_ack(const AT24C01BDriver* const at24c01b, const apalTime_t timeout);
  apalExitStatus_t at24c01b_lld_read_current_address(const AT24C01BDriver* const at24c01b, uint8_t* const data, const uint8_t num, const apalTime_t timeout);
  apalExitStatus_t at24c01b_lld_read(const AT24C01BDriver* const at24c01b, const uint8_t addr, uint8_t* const data, const uint8_t num, const apalTime_t timeout);
  apalExitStatus_t at24c01b_lld_write_byte(const AT24C01BDriver* const at24c01b, const uint8_t addr, const uint8_t data, const apalTime_t timeout);
  apalExitStatus_t at24c01b_lld_write_page(const AT24C01BDriver* const at24c01b, const uint8_t addr, const uint8_t* const data, const uint8_t num, const apalTime_t timeout);
#ifdef __cplusplus
}
#endif

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROLLD_AT24C01BN_H */

/** @} */
