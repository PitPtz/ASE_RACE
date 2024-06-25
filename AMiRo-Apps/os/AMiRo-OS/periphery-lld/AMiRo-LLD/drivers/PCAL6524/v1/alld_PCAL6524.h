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
 * @file    alld_PCAL6524.h
 * @brief   GPIO extender macros and structures.
 *
 * @defgroup lld_PCAL6524_v1 Version 1
 * @brief   Version 1
 * @ingroup lld_PCAL6524
 *
 * @addtogroup lld_PCAL6524_v1
 * @{
 */

#ifndef AMIROLLD_PCAL6524_H
#define AMIROLLD_PCAL6524_H

#include <amiro-lld.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief   Maximum I2C frequency.
 */
#define PCAL6524_LLD_I2C_MAXFREQUENCY           1000000

/**
 * @brief   A falling edge indicats an interrupt.
 */
#define PCAL6524_LLD_INT_EDGE                   APAL_GPIO_EDGE_FALLING

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
 * @brief   The PCAL6524Driver sruct.
 */
typedef struct {
  apalI2CDriver_t* i2cd;
  apalI2Caddr_t addr;
} PCAL6524Driver;

/**
 * @brief   Possible I2C address configurations.
 */
enum {
  PCAL6524_LLD_I2C_ADDR_FIXED     = 0x0020u,  /**< Fixed part of the I2C address. */
  PCAL6524_LLD_I2C_ADDR_SCL       = 0x0020u,  /**< ADDR pin connected to SCL. */
  PCAL6524_LLD_I2C_ADDR_SDA       = 0x0021u,  /**< ADDR pin connected to SDA. */
  PCAL6524_LLD_I2C_ADDR_VSS       = 0x0022u,  /**< ADDR pin connected to VSS. */
  PCAL6524_LLD_I2C_ADDR_VDD       = 0x0023u,  /**< ADDR pin connected to VDD. */
  PCAL6524_LLD_I2C_ADDR_DEVICEID  = 0x007Cu,  /**< Special address to read device ID information. */
};

/**
 * @brief   Command bit to enable auto-incrementation of command value.
 * @details Can be added (ORed) to any command value.
 */
#define PCAL6524_LLD_CMD_AUTOINCREMENT          0x80u

/**
 * @brief   The total number of registers that can be accessed.
 * @note    This is the maximum number of bytes that may be read or written continuously.
 */
#define PCAL6524_LLD_NUM_REGISTERS              52

typedef union {
  uint8_t raw[3];
  struct {
    uint16_t name : 12;
    uint16_t part : 9;
    uint8_t revision : 3;
  };
} pcal6524_lld_deviceid_t;

/**
 * @brief   Control commands for the PCAL6524.
 */
typedef enum {
  PCAL6524_LLD_CMD_INPUT_P0                             = 0x00u,  /**<  read only */
  PCAL6524_LLD_CMD_INPUT_P1                             = 0x01u,  /**<  read only */
  PCAL6524_LLD_CMD_INPUT_P2                             = 0x02u,  /**<  read only */
  PCAL6524_LLD_CMD_OUTPUT_P0                            = 0x04u,  /**< read/write */
  PCAL6524_LLD_CMD_OUTPUT_P1                            = 0x05u,  /**< read/write */
  PCAL6524_LLD_CMD_OUTPUT_P2                            = 0x06u,  /**< read/write */
  PCAL6524_LLD_CMD_POLARITYINVERSION_P0                 = 0x08u,  /**< read/write */
  PCAL6524_LLD_CMD_POLARITYINVERSION_P1                 = 0x09u,  /**< read/write */
  PCAL6524_LLD_CMD_POLARITYINVERSION_P2                 = 0x0Au,  /**< read/write */
  PCAL6524_LLD_CMD_CONFIGURATION_P0                     = 0x0Cu,  /**< read/write */
  PCAL6524_LLD_CMD_CONFIGURATION_P1                     = 0x0Du,  /**< read/write */
  PCAL6524_LLD_CMD_CONFIGURATION_P2                     = 0x0Eu,  /**< read/write */
  PCAL6524_LLD_CMD_OUTPUTDRIVESTRENGTH_P0A              = 0x40u,  /**< read/write */
  PCAL6524_LLD_CMD_OUTPUTDRIVESTRENGTH_P0B              = 0x41u,  /**< read/write */
  PCAL6524_LLD_CMD_OUTPUTDRIVESTRENGTH_P1A              = 0x42u,  /**< read/write */
  PCAL6524_LLD_CMD_OUTPUTDRIVESTRENGTH_P1B              = 0x43u,  /**< read/write */
  PCAL6524_LLD_CMD_OUTPUTDRIVESTRENGTH_P2A              = 0x44u,  /**< read/write */
  PCAL6524_LLD_CMD_OUTPUTDRIVESTRENGTH_P2B              = 0x45u,  /**< read/write */
  PCAL6524_LLD_CMD_INPUTLATCH_P0                        = 0x48u,  /**< read/write */
  PCAL6524_LLD_CMD_INPUTLATCH_P1                        = 0x49u,  /**< read/write */
  PCAL6524_LLD_CMD_INPUTLATCH_P2                        = 0x4Au,  /**< read/write */
  PCAL6524_LLD_CMD_PUPDENABLE_P0                        = 0x4Cu,  /**< read/write */
  PCAL6524_LLD_CMD_PUPDENABLE_P1                        = 0x4Du,  /**< read/write */
  PCAL6524_LLD_CMD_PUPDENABLE_P2                        = 0x4Eu,  /**< read/write */
  PCAL6524_LLD_CMD_PUPDSELECTION_P0                     = 0x50u,  /**< read/write */
  PCAL6524_LLD_CMD_PUPDSELECTION_P1                     = 0x51u,  /**< read/write */
  PCAL6524_LLD_CMD_PUPDSELECTION_P2                     = 0x52u,  /**< read/write */
  PCAL6524_LLD_CMD_INTERRUPTMASK_P0                     = 0x54u,  /**< read/write */
  PCAL6524_LLD_CMD_INTERRUPTMASK_P1                     = 0x55u,  /**< read/write */
  PCAL6524_LLD_CMD_INTERRUPTMASK_P2                     = 0x56u,  /**< read/write */
  PCAL6524_LLD_CMD_INTERRUPTSTATUS_P0                   = 0x58u,  /**<  read only */
  PCAL6524_LLD_CMD_INTERRUPTSTATUS_P1                   = 0x59u,  /**<  read only */
  PCAL6524_LLD_CMD_INTERRUPTSTATUS_P2                   = 0x5Au,  /**<  read only */
  PCAL6524_LLD_CMD_OUTPUTCONFIGURATION                  = 0x5Cu,  /**< read/write */
  PCAL6524_LLD_CMD_INTERRUPTEDGE_P0A                    = 0x60u,  /**< read/write */
  PCAL6524_LLD_CMD_INTERRUPTEDGE_P0B                    = 0x61u,  /**< read/write */
  PCAL6524_LLD_CMD_INTERRUPTEDGE_P1A                    = 0x62u,  /**< read/write */
  PCAL6524_LLD_CMD_INTERRUPTEDGE_P1B                    = 0x63u,  /**< read/write */
  PCAL6524_LLD_CMD_INTERRUPTEDGE_P2A                    = 0x64u,  /**< read/write */
  PCAL6524_LLD_CMD_INTERRUPTEDGE_P2B                    = 0x65u,  /**< read/write */
  PCAL6524_LLD_CMD_INTERRUPTCLEAR_P0                    = 0x68u,  /**< write only */
  PCAL6524_LLD_CMD_INTERRUPTCLEAR_P1                    = 0x69u,  /**< write only */
  PCAL6524_LLD_CMD_INTERRUPTCLEAR_P2                    = 0x6Au,  /**< write only */
  PCAL6524_LLD_CMD_INPUTSTATUS_P0                       = 0x6Cu,  /**<  read only */
  PCAL6524_LLD_CMD_INPUTSTATUS_P1                       = 0x6Du,  /**<  read only */
  PCAL6524_LLD_CMD_INPUTSTATUS_P2                       = 0x6Eu,  /**<  read only */
  PCAL6524_LLD_CMD_INDIVIDUALPINOUTPUTCONFIGURATION_P0  = 0x70u,  /**< read/write */
  PCAL6524_LLD_CMD_INDIVIDUALPINOUTPUTCONFIGURATION_P1  = 0x71u,  /**< read/write */
  PCAL6524_LLD_CMD_INDIVIDUALPINOUTPUTCONFIGURATION_P2  = 0x72u,  /**< read/write */
  PCAL6524_LLD_CMD_SWITCHDEBOUNCEENABLE_P0              = 0x74u,  /**< read/write */
  PCAL6524_LLD_CMD_SWITCHDEBOUNCEENABLE_P1              = 0x75u,  /**< read/write */
  PCAL6524_LLD_CMD_SWITCHDEBOUNCECOUNT                  = 0x76u,  /**< read/write */
} pcal6524_lld_cmd_t;

/**
 * @brief   Input register bit values.
 * @details The bits in the input register reflect the incoming logic levels per pin.
 *          If a pin is configured as ouput, the bit reflects th set value or is forced to 0 in case the outpus is configured as open-drain.
 *          If a pin is configured as input with latched interrupts, reading the according port will reset the input value and clear the interrupt.
 */
typedef enum {
  PCAL6524_LLD_INPUT_LOW  = 0b0,
  PCAL6524_LLD_INPUT_HIGH = 0b1,
} pcal6524_lld_input_t;

/**
 * @brief   Output register bit values.
 * @details Defines the logic level to be driven by output pins.
 *          The default value (after reset) is 0b1 (high),
 */
typedef enum {
  PCAL6524_LLD_OUTPUT_LOW     = 0b0,
  PCAL6524_LLD_OUTPUT_HIGH    = 0b1,
} pcal6524_lld_output_t;

/**
 * @brief   Polarity inversion register values.
 * @details Allows to inverse the logic values written to the input register.
 *          The default value (after reset) is 0b0 (disabled).
 */
typedef enum {
  PCAL6524_LLD_POLARITYINVERSION_DISABLED = 0b0,
  PCAL6524_LLD_POLARITYINVERSION_ENABLED  = 0b1,
} pcal6524_lld_polarityinversion_t;

/**
 * @brief   Configuration regsiter bit values.
 * @details Configures the direction of the I/O pins to either high-impedance input or output.
 *          The default value (after reset) is 0b1 (input).
 */
typedef enum {
  PCAL6524_LLD_CONFIGURATION_INPUT    = 0b1,
  PCAL6524_LLD_CONFIGURATION_OUTPUT   = 0b0,
} pcal6524_lld_configuration_t;

/**
 * @brief   Output drive strength register mask values.
 * @details Configures maximum current of output pins can be defined via a 2 bit mask per pin.
 *          Toggling multiple output pins simultaneously a peak current may induce noise to supply voltage and ground.
 *          By lowering the maximum current per pin. this effect can be minimized.
 *          The default value (after reset) is 0b11 (factor 1x)
 */
typedef enum {
  PCAL6524_LL_OUTPUTDRIVESTRENGTH_0_25    = 0b00,
  PCAL6524_LL_OUTPUTDRIVESTRENGTH_0_5     = 0b01,
  PCAL6524_LL_OUTPUTDRIVESTRENGTH_0_75    = 0b10,
  PCAL6524_LL_OUTPUTDRIVESTRENGTH_1       = 0b11,
} pcal6524_lld_outputdrivestrength_t;

/**
 * @brief   Input latch register bit values.
 * @details Allows to latch interrupt and input states per pin, if an interrupt occurred.
 *          The default value (after reset) is 0b0 (disabled).
 */
typedef enum {
  PCAL6524_LLD_INPUTLATCH_ENABLED   = 0b1,
  PCAL6524_LLD_INPUTLATCH_DISABLED  = 0b0,
} pcal6524_lld_inputlatch_t;

/**
 * @brief   Pull-up/Pull-down enable register bis values.
 * @details Configures per pin whether the pull-up/pull-down resistors shall be enabled.
 *          If a pin is configured as open-drain output, the setting in this register are overridden and the resistors are disconnected.
 *          The default value (after reset) is 0b0 (disabled).
 */
typedef enum {
  PCAL6524_LLD_PUPD_ENABLED   = 0b1,
  PCAL6524_LLD_PUPD_DISABLED  = 0b0,
} pcal6524_lld_pupdenable_t;

/**
 * @brief   Pull-up/Pull-dpwn selection register bit values.
 * @details Selects between pull-up and pull-down resistor (100 kΩ) per pin.
 *          Has no effect if the according bit is the pull-up/pull-down enable register is disabled.
 *          The default value (after reset) is 0b1 (pull-up).
 */
typedef enum {
  PCAL6524_LLD_PUPDSELECTION_PULLUP   = 0b1,
  PCAL6524_LLD_PUPDSELECTION_PULLDOWN = 0b0,
} pcal6524_lld_pupdselection_t;

/**
 * @brief   Interrupt mask register bit values.
 * @details Allows to enable (value 0) or disable (value 1) interrupts per pin.
 *          The default value (after reset) is 0b1 (interrupt disabled).
 */
typedef enum {
  PCAL6524_LLD_INTERRUPTMASK_ENABLED  = 0b0,
  PCAL6524_LLD_INTERRUPTMASK_DSIABLED = 0b1,
} pcal6524_lld_interruptmask_t;

/**
 * @brief   Interrupt status register bit value.
 * @details Indicates whether an interrupt occurred per pin.
 *          After reset the register is initialized with 0b0 (no interrupt occurred yet).
 */
typedef enum {
  PCAL6524_LLD_INTERRUPTSTATUS_ACTIVE   = 0b1,
  PCAL6524_LLD_INTERRUPTSTATUS_INACTIVE = 0b0,
} pcal6542_lld_interruptstatus_t;

/**
 * @brief   Output port configuration register mask of valid bits.
 */
#define PCAL6524_LLD_OUTPUTCONFIGURATION_MASK       0x07u

/**
 * @brief   Output port configuration register mask for I/O port 0.
 */
#define PCAL6524_LLD_OUTPUTCONFIGURATION_MASK_PORT0 0x01u

/**
 * @brief   Output port configuration register mask for I/O port 1.
 */
#define PCAL6524_LLD_OUTPUTCONFIGURATION_MASK_PORT1 0x02u

/**
 * @brief   Output port configuration register mask for I/O port 2.
 */
#define PCAL6524_LLD_OUTPUTCONFIGURATION_MASK_PORT2 0x04u

/**
 * @brief   Output port configuration register bit values.
 * @details Configures all ouput pins per port to be push-pull or open-drain.
 *          The default value (after reset) is 0b0 (push-pull).
 */
typedef enum {
  PCAL6524_LLD_OUTPUTCONFIGURATION_PUSHPULL   = 0b0,
  PCAL6524_LLD_OUTPUTCONFIGURATION_OPENDRAIN  = 0b1,
} pcal6524_lld_outputconfiguration_t;

/**
 * @brief   Interrupt edge register mask values.
 * @details Configures the type of event that would cause an interrupt per pin.
 *          The default value (after reset) is 0b00 (level triggered).
 */
typedef enum {
  PCAL6524_LLD_INTERRUPTEDGE_LEVELTRIGGERED = 0b00,
  PCAL6524_LLD_INTERRUPTEDGE_RISINGEDGE     = 0b01,
  PCAL6524_LLD_INTERRUPTEDGE_FALLINGEDGE    = 0b10,
  PCAL6524_LLD_INTERRUPTEDGE_ANYEDGE        = 0b11,
} pcal6524_lld_interruptedge_t;

/**
 * @brief   Input status register bit values.
 * @details Reflects the current logic level per pin similar to the input register.
 *          However, values are not latched and reading the register will not reset interrupts.
 */
typedef enum {
  PCAL6524_LLD_INPUTSTATUS_LOW  = 0b0,
  PCAL6524_LLD_INPUTSTATUS_HIGH = 0b1,
} pcal6524_lld_inputstatus_t;

/**
 * @brief   Individual pin output configuration register bit values.
 * @details Can be used to invert the port-wide push-pull/open-drain configuration via the ouput port configuration register per pin.
 *          the default value (after reset) is 0b0 (not inverted).
 */
typedef enum {
  PCAL6524_LLD_INDIVIDUALPINOUTPUTCONFIGURATION_PORT      = 0b0,
  PCAL6524_LLD_INDIVIDUALPINOUTPUTCONFIGURATION_INVERTED  = 0b1,
} pcal6524_lld_individualpinoutputconfiguration_t;

/**
 * @brief   Switch debounce enable register bit values.
 * @details Allows to enable debounce functionality for I/O ports 0 and 1.
 *          In order to use the debounce feature, an oscillator signal must be applied to pin 0 of port 0.
 *          The default value (after reset) is 0b0 (disabled).
 */
typedef enum {
  PCAL6524_LLD_SWITCHDEBOUNCE_ENABLED   = 0b1,
  PCAL6524_LLD_SWITCHDEBOUNCE_DISABLED  = 0b0,
} pcal6524_lld_switchdebounceenable_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
  uint8_t pcal6524_lld_cmd_groupsize(const pcal6524_lld_cmd_t cmd);

//  apalExitStatus_t pcal6524_lld_read_id(const PCAL6524Driver* const pcal6524d, uint8_t* const data, const apalTime_t timeout);
  apalExitStatus_t pcal6524_lld_read_reg(const PCAL6524Driver* const pcal6524d, const pcal6524_lld_cmd_t reg, uint8_t* const data, const apalTime_t timeout);
  apalExitStatus_t pcal6524_lld_write_reg(const PCAL6524Driver* const pcal6524d, const pcal6524_lld_cmd_t reg, const uint8_t data, const apalTime_t timeout);
  apalExitStatus_t pcal6524_lld_read_group(const PCAL6524Driver* const pcal6524d, const pcal6524_lld_cmd_t reg, uint8_t* const data, const apalTime_t timeout);
  apalExitStatus_t pcal6524_lld_write_group(const PCAL6524Driver* const pcal6524d, const pcal6524_lld_cmd_t reg, const uint8_t* const data, const apalTime_t timeout);
  apalExitStatus_t pcal6524_lld_read_continuous(const PCAL6524Driver* const pcal6524d, const pcal6524_lld_cmd_t reg, uint8_t* const data, const uint8_t length, const apalTime_t timeout);
  apalExitStatus_t pcal6524_lld_write_continuous(const PCAL6524Driver* const pcal6524d, const pcal6524_lld_cmd_t reg, const uint8_t* const data, const uint8_t length, const apalTime_t timeout);
#ifdef __cplusplus
}
#endif

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROLLD_PCAL6524_H */

/** @} */

