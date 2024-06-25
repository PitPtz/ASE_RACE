/*
AMiRo-OS is an operating system designed for the Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2016..2022  Thomas Schöpping et al.

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
 * @file    module.c
 * @brief   Structures and constant for the LightRing v1.2 module.
 */

#include <amiroos.h>

#include <string.h>

/*===========================================================================*/
/**
 * @name Module specific functions
 * @{
 */
/*===========================================================================*/

/** @} */

/*===========================================================================*/
/**
 * @name ChibiOS/HAL configuration
 * @{
 */
/*===========================================================================*/

ROMCONST CANConfig moduleHalCanConfig = {
  /* mcr  */ CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
  /* btr  */ CAN_BTR_SJW(1) | CAN_BTR_TS2(2) | CAN_BTR_TS1(13) | CAN_BTR_BRP(1),
};

I2CConfig moduleHalI2cEepromPwrmtrBreakoutConfig = {
  /* I²C mode   */ OPMODE_I2C,
  /* frequency  */ 400000, // TODO: replace with some macro (-> ChibiOS/HAL)
  /* duty cycle */ FAST_DUTY_CYCLE_2,
};

ROMCONST SPIConfig moduleHalSpiLightConfig = {
  /* circular buffer mode             */ false,
  /* slave mode                       */ false,
  /* data callback function pointer   */ NULL,
  /* error callback function pointer  */ NULL,
  /* chip select line                 */ LINE_LIGHT_XLAT,
  /* CR1                              */ SPI_CR1_BR_0 | SPI_CR1_BR_1,
  /* CR2                              */ SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN,
};

SerialCANDriver moduleHalSerialCAN;

ROMCONST SerialCANConfig moduleHalSerialCanConfig = {
  /* FBCAN driver   */ &MODULE_HAL_CAN,
  /* CAN identifer  */ {.std={
                          /* rsvd */ 0,
                          /* id   */ 0x7FF,
                          /* ide  */ CAN_IDE_STD,
                          /* rtr  */ CAN_RTR_DATA,
                        }},
};

#if (AMIROOS_CFG_DBG == true) || defined(__DOXYGEN__)

ROMCONST SerialConfig moduleHalSerialConfig = {
  /* bit rate */ 115200,
  /* CR1      */ 0,
  /* CR1      */ 0,
  /* CR1      */ 0,
};

#endif /* (AMIROOS_CFG_DBG == true) */

#if (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_UWBv10)

ROMCONST SPIConfig moduleHalSpiUwbHsConfig = {
  /* circular buffer mode        */ false,
  /* callback function pointer   */ NULL,
  /* chip select line port       */ PAL_PORT(LINE_SPI_SS_N),
  /* chip select line pad number */ PAL_PAD(LINE_SPI_SS_N),
  /* CR1                         */ 0,                                  // 36/2 Mbps
  /* CR2                         */ SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN,  // 0
};

ROMCONST SPIConfig moduleHalSpiUwbLsConfig = {
  /* circular buffer mode        */ false,
  /* callback function pointer   */ NULL,
  /* chip select line port       */ PAL_PORT(LINE_SPI_SS_N),
  /* chip select line pad number */ PAL_PAD(LINE_SPI_SS_N),
  /* CR1                         */ SPI_CR1_BR_1 | SPI_CR1_BR_0,        // 36/16 Mbps
  /* CR2                         */ SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN,  // 0
};

#endif /* (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_UWBv10) */

/*===========================================================================*/
/**
 * @name GPIO definitions
 * @{
 */
/*===========================================================================*/

/**
 * @brief   LIGHT_BANK output signal GPIO.
 */
static apalGpio_t _gpioLightBlank = {
  /* line */ LINE_LIGHT_BLANK,
};
ROMCONST apalControlGpio_t moduleGpioLightBlank = {
  /* GPIO */ &_gpioLightBlank,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ TLC5947_LLD_BLANK_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   RS232_R_EN_N output signal GPIO.
 */
static apalGpio_t _gpioRs232En = {
  /* line */ LINE_RS232_R_EN_N,
};
ROMCONST apalControlGpio_t moduleGpioRs232En = {
  /* GPIO */ &_gpioRs232En,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_LOW, //TODO
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   SW_V33_EN output signal GPIO.
 */
static apalGpio_t _gpioSwV33En = {
  /* line */ LINE_SW_V33_EN,
};
ROMCONST apalControlGpio_t moduleGpioSwV33En = {
  /* GPIO */ &_gpioSwV33En,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ MIC9404x_LLD_EN_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

// The 4.2V switch is disabled due to a hardware bug.
///**
// * @brief   SW_V42_EN output signal GPIO.
// */
//static apalGpio_t _gpioSwV42En = {
//  /* line */ LINE_SW_V42_EN,
//};
//ROMCONST apalControlGpio_t moduleGpioSwV42En = {
//  /* GPIO */ &_gpioSwV42En,
//  /* meta */ {
//    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
//    /* active state   */ MIC9404x_LLD_EN_ACTIVE_STATE,
//    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
//  },
//};

/**
 * @brief   SW_V50_EN output signal GPIO.
 */
static apalGpio_t _gpioSwV50En = {
  /* line */ LINE_SW_V50_EN,
};
ROMCONST apalControlGpio_t moduleGpioSwV50En = {
  /* GPIO */ &_gpioSwV50En,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ MIC9404x_LLD_EN_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   SYS_UART_DN bidirectional signal GPIO.
 */
static apalGpio_t _gpioSysUartDn = {
  /* line */ LINE_SYS_UART_DN,
};
ROMCONST apalControlGpio_t moduleGpioSysUartDn = {
  /* GPIO */ &_gpioSysUartDn,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   LED output signal GPIO.
 */
static apalGpio_t _gpioLed = {
  /* line */ LINE_LED,
};
ROMCONST apalControlGpio_t moduleGpioLed = {
  /* GPIO */ &_gpioLed,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   LIGHT_XLAT output signal GPIO.
 */
static apalGpio_t _gpioLightXlat = {
  /* line */ LINE_LIGHT_XLAT,
};
ROMCONST apalControlGpio_t moduleGpioLightXlat = {
  /* GPIO */ &_gpioLightXlat,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ (TLC5947_LLD_XLAT_UPDATE_EDGE == APAL_GPIO_EDGE_RISING) ? APAL_GPIO_ACTIVE_HIGH : APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   SW_V18_EN output signal GPIO.
 */
static apalGpio_t _gpioSwV18En = {
  /* line */ LINE_SW_V18_EN,
};
ROMCONST apalControlGpio_t moduleGpioSwV18En = {
  /* GPIO */ &_gpioSwV18En,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ MIC9404x_LLD_EN_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   SW_VSYS_EN output signal GPIO.
 */
static apalGpio_t _gpioSwVsysEn = {
  /* line */ LINE_SW_VSYS_EN,
};
ROMCONST apalControlGpio_t moduleGpioSwVsysEn = {
  /* GPIO */ &_gpioSwVsysEn,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   SYS_UART_UP bidirectional signal GPIO.
 */
static apalGpio_t _gpioSysUartUp = {
  /* line */ LINE_SYS_UART_UP,
};
ROMCONST apalControlGpio_t moduleGpioSysUartUp = {
  /* GPIO */ &_gpioSysUartUp,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   SYS_PD bidirectional signal GPIO.
 */
static apalGpio_t _gpioSysPd = {
  /* line */ LINE_SYS_PD_N,
};
ROMCONST apalControlGpio_t moduleGpioSysPd = {
  /* GPIO */ &_gpioSysPd,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   SYS_SYNC bidirectional signal GPIO.
 */
static apalGpio_t _gpioSysSync = {
  /* line */ LINE_SYS_INT_N,
};
ROMCONST apalControlGpio_t moduleGpioSysSync = {
  /* GPIO */ &_gpioSysSync,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   IO_1 breakout signal GPIO.
 */
static apalGpio_t _gpioBreakoutIo1 = {
  /* line */ LINE_IO_1,
};

/**
 * @brief   IO_2 breakout signal GPIO.
 */
static apalGpio_t _gpioBreakoutIo2 = {
  /* line */ LINE_IO_2,
};

/**
 * @brief   IO_3 breakout signal GPIO.
 */
static apalGpio_t _gpioBreakoutIo3 = {
  /* line */ LINE_IO_3,
};

/**
 * @brief   IO_4 breakout signal GPIO.
 */
static apalGpio_t _gpioBreakoutIo4 = {
  /* line */ LINE_IO_4,
};

/**
 * @brief   IO_5 breakout signal GPIO.
 */
static apalGpio_t _gpioBreakoutIo5 = {
  /* line */ LINE_IO_5,
};

/**
 * @brief   IO_6 breakout signal GPIO.
 */
static apalGpio_t _gpioBreakoutIo6 = {
  /* line */ LINE_IO_6,
};

/**
 * @brief   IO_7 breakout signal GPIO.
 */
static apalGpio_t _gpioBreakoutIo7 = {
  /* line */ LINE_IO_7,
};

/**
 * @brief   IO_8 breakout signal GPIO.
 */
static apalGpio_t _gpioBreakoutIo8 = {
  /* line */ LINE_IO_8,
};

#if (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_NONE || BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_ESP || BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_NICLASENSE)

apalControlGpio_t moduleGpioBreakoutIo1 = {
  /* GPIO */ &_gpioBreakoutIo1,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_UNDEFINED,
    /* active state   */ APAL_GPIO_ACTIVE_NONE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

apalControlGpio_t moduleGpioBreakoutIo2 = {
  /* GPIO */ &_gpioBreakoutIo2,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_UNDEFINED,
    /* active state   */ APAL_GPIO_ACTIVE_NONE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

apalControlGpio_t moduleGpioBreakoutIo3 = {
  /* GPIO */ &_gpioBreakoutIo3,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_UNDEFINED,
    /* active state   */ APAL_GPIO_ACTIVE_NONE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

apalControlGpio_t moduleGpioBreakoutIo4 = {
  /* GPIO */ &_gpioBreakoutIo4,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_UNDEFINED,
    /* active state   */ APAL_GPIO_ACTIVE_NONE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

apalControlGpio_t moduleGpioBreakoutIo5 = {
  /* GPIO */ &_gpioBreakoutIo5,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_UNDEFINED,
    /* active state   */ APAL_GPIO_ACTIVE_NONE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

apalControlGpio_t moduleGpioBreakoutIo6 = {
  /* GPIO */ &_gpioBreakoutIo6,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_UNDEFINED,
    /* active state   */ APAL_GPIO_ACTIVE_NONE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

apalControlGpio_t moduleGpioBreakoutIo7 = {
  /* GPIO */ &_gpioBreakoutIo7,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_UNDEFINED,
    /* active state   */ APAL_GPIO_ACTIVE_NONE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

apalControlGpio_t moduleGpioBreakoutIo8 = {
  /* GPIO */ &_gpioBreakoutIo8,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_UNDEFINED,
    /* active state   */ APAL_GPIO_ACTIVE_NONE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

#elif (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_UWBv10)

apalControlGpio_t moduleGpioBreakoutIo1 = {
  /* GPIO */ &_gpioBreakoutIo1,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_UNDEFINED,
    /* active state   */ APAL_GPIO_ACTIVE_NONE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

apalControlGpio_t moduleGpioBreakoutIo2 = {
  /* GPIO */ &_gpioBreakoutIo2,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

apalControlGpio_t moduleGpioBreakoutIo3 = {
  /* GPIO */ &_gpioBreakoutIo3,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_UNDEFINED,
    /* active state   */ APAL_GPIO_ACTIVE_NONE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

apalControlGpio_t moduleGpioBreakoutIo4 = {
  /* GPIO */ &_gpioBreakoutIo4,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_UNDEFINED,
    /* active state   */ APAL_GPIO_ACTIVE_NONE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

apalControlGpio_t moduleGpioBreakoutIo5 = {
  /* GPIO */ &_gpioBreakoutIo5,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_UNDEFINED,
    /* active state   */ APAL_GPIO_ACTIVE_NONE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

apalControlGpio_t moduleGpioBreakoutIo6 = {
  /* GPIO */ &_gpioBreakoutIo6,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_UNDEFINED,
    /* active state   */ APAL_GPIO_ACTIVE_NONE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

apalControlGpio_t moduleGpioBreakoutIo7 = {
  /* GPIO */ &_gpioBreakoutIo7,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

apalControlGpio_t moduleGpioBreakoutIo8 = {
  /* GPIO */ &_gpioBreakoutIo8,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_RISING,
  },
};

#endif

/** @} */

/*===========================================================================*/
/**
 * @name AMiRo-OS core configurations
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Initializes interrupts.
 */
void moduleInitInterrupts()
{
  // breakout
#if (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_UWBv10)
  palSetLineCallback(moduleLldDw1000.gpio_exti->gpio->line, aosSysGetStdGpioCallback(), &moduleLldDw1000.gpio_exti->gpio->line);
  palEnableLineEvent(moduleLldDw1000.gpio_exti->gpio->line, APAL2CH_EDGE(moduleLldDw1000.gpio_exti->meta.edge));
#endif

  return;
}

/**
 * @brief   Initializes periphery communication interfaces.
 */
void moduleInitPeripheryInterfaces()
{
  // serial driver
#if (AMIROOS_CFG_DBG == true)
  sdStart(&MODULE_HAL_SERIAL, &moduleHalSerialConfig);
#endif /* (AMIROOS_CFG_DBG == true) */

#if (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_NICLASENSE) || defined(__DOXYGEN__)
  // Power on the MIC9404x (power switch)
  // V50
  mic9404x_lld_set(&moduleLldPowerSwitchV50, MIC9404x_LLD_STATE_ON);

  //Sleep a little bit so that the supply voltage is enabled before the I2C
  aosThdMSleep(1000);
#endif /* (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_NICLASENSE)*/

  // I2C
  moduleHalI2cEepromPwrmtrBreakoutConfig.clock_speed = (AT24C01B_LLD_I2C_MAXFREQUENCY < moduleHalI2cEepromPwrmtrBreakoutConfig.clock_speed) ? AT24C01B_LLD_I2C_MAXFREQUENCY : moduleHalI2cEepromPwrmtrBreakoutConfig.clock_speed;
  moduleHalI2cEepromPwrmtrBreakoutConfig.clock_speed = (INA219_LLD_I2C_MAXFREQUENCY < moduleHalI2cEepromPwrmtrBreakoutConfig.clock_speed) ? INA219_LLD_I2C_MAXFREQUENCY : moduleHalI2cEepromPwrmtrBreakoutConfig.clock_speed;
  moduleHalI2cEepromPwrmtrBreakoutConfig.duty_cycle = (moduleHalI2cEepromPwrmtrBreakoutConfig.clock_speed <= 100000) ? STD_DUTY_CYCLE : FAST_DUTY_CYCLE_2;
  i2cStart(&MODULE_HAL_I2C_EEPROM_PWRMTR_BREAKOUT, &moduleHalI2cEepromPwrmtrBreakoutConfig);

  // SPI
  spiStart(&MODULE_HAL_SPI_LIGHT, &moduleHalSpiLightConfig);
#if (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_UWBv10)
  spiStart(&MODULE_HAL_SPI_UWB, &moduleHalSpiUwbLsConfig);
#endif

  // CAN
  aosFBCanInit(&MODULE_HAL_CAN, NULL, NULL);
  aosFBCanStart(&MODULE_HAL_CAN, &moduleHalCanConfig);
  sdcanInit();
  sdcanObjectInit(&moduleHalSerialCAN);
  sdcanStart(&moduleHalSerialCAN, &moduleHalSerialCanConfig);

#if (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_ESP) || defined(__DOXYGEN__)
  palWriteLine(LINE_IO_1, PAL_LOW);
  palSetLineMode(LINE_IO_1, PAL_MODE_OUTPUT_OPENDRAIN);

  // Power on the MIC9404x (power switch)
  // V33
  mic9404x_lld_set(&moduleLldPowerSwitchV33, MIC9404x_LLD_STATE_ON);

  aosThdSSleep(1);

  palWriteLine(LINE_IO_1, PAL_HIGH);

  aosThdSSleep(3);
#endif /* (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_ESP)*/

  return;
}

#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)
/**
 * @brief   Initializes tests.
 */
void moduleInitTests()
{
  // add test commands to shell
#if (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)
  aosShellAddCommand(&moduleTestAt24c01bShellCmd);
  aosShellAddCommand(&moduleTestIna219ShellCmd);
  aosShellAddCommand(&moduleTestLedShellCmd);
  aosShellAddCommand(&moduleTestMic9404xShellCmd);
  aosShellAddCommand(&moduleTestTlc5947ShellCmd);
#if (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_UWBv10)
  aosShellAddCommand(&moduleTestDw1000ShellCmd);
#endif
#endif /* (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */
  aosShellAddCommand(&moduleTestAllShellCmd);

  return;
}
#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */

/**
 * @brief   Deinitializes periphery communication interfaces.
 */
void moduleShutdownPeripheryInterfaces()
{
  // CAN
  sdcanStop(&moduleHalSerialCAN);
  aosFBCanStop(&MODULE_HAL_CAN);

  // SPI
#if (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_UWBv10)
  spiStop(&MODULE_HAL_SPI_UWB);
#endif
  spiStop(&MODULE_HAL_SPI_LIGHT);

  // I2C
  i2cStop(&MODULE_HAL_I2C_EEPROM_PWRMTR_BREAKOUT);

  // serial driver
#if (AMIROOS_CFG_DBG == true)
  /* don't stop the serial driver so messages can still be printed */
#endif /* (AMIROOS_CFG_DBG == true) */

  return;
}

/** @} */

/*===========================================================================*/
/**
 * @name Startup Shutdown Synchronization Protocol (SSSP)
 * @{
 */
/*===========================================================================*/
#if ((AMIROOS_CFG_SSSP_ENABLE == true) && (AMIROOS_CFG_SSSP_MSI == true)) ||    \
    defined(__DOXYGEN__)

/* some local definitions */
// maximum number of bytes per CAN frame
#define CAN_BYTES_PER_FRAME                     8
// identifier (as dominant as possible)
#define MSI_BCBMSG_CANID                        0

static void _moduleSsspBCBfilterCallback(const CANRxFrame* frame, void* params)
{
  // copy the frame to the buffer
  *((CANRxFrame*)params) = *frame;
  return;
}

void moduleSsspBcbSetup(void* data)
{
  aosDbgCheck(data != NULL);

  // set mask
  ((MODULE_SSSP_BCBDATA*)data)->filter.mask.std.id = ~0;
  ((MODULE_SSSP_BCBDATA*)data)->filter.mask.std.ide = 1;
  ((MODULE_SSSP_BCBDATA*)data)->filter.mask.std.rtr = 1;

  // set filter
  ((MODULE_SSSP_BCBDATA*)data)->filter.filter.std.id = MSI_BCBMSG_CANID;
  ((MODULE_SSSP_BCBDATA*)data)->filter.filter.std.ide = CAN_IDE_STD;
  ((MODULE_SSSP_BCBDATA*)data)->filter.filter.std.rtr = CAN_RTR_DATA;

  // set callback and parameter
  ((MODULE_SSSP_BCBDATA*)data)->filter.callback = _moduleSsspBCBfilterCallback;
  ((MODULE_SSSP_BCBDATA*)data)->filter.cbparams = &((MODULE_SSSP_BCBDATA*)data)->frame;

  // invalidate frame
  ((MODULE_SSSP_BCBDATA*)data)->frame.SID = ~MSI_BCBMSG_CANID;

  // add filter
  aosFBCanAddFilter(&MODULE_HAL_CAN, &((MODULE_SSSP_BCBDATA*)data)->filter);

  return;
}

void moduleSsspBcbRelease(void* data)
{
  // remove filter
  const aos_status_t status = aosFBCanRemoveFilter(&MODULE_HAL_CAN, &((MODULE_SSSP_BCBDATA*)data)->filter);
  aosDbgAssertMsg(status == AOS_SUCCESS, "SSSP BCB failed to remove filter");

  return;
}

aos_ssspbcbstatus_t moduleSsspBcbTransmit(const uint8_t* buffer, size_t length, void* data)
{
  aosDbgCheck(buffer != NULL);
  aosDbgCheck(length > 0 && length <= CAN_BYTES_PER_FRAME);

  (void)data;

  // local variables
  CANTxFrame frame;

  // setup the common parts of the message frame
  frame.DLC = (uint8_t)length;
  frame.RTR = CAN_RTR_DATA;
  frame.IDE = CAN_IDE_STD;
  frame.SID = MSI_BCBMSG_CANID;
  memcpy(frame.data8, buffer, length);

  // sent the frame and return
  return (aosFBCanTransmitTimeout(&MODULE_HAL_CAN, &frame, TIME_IMMEDIATE) != AOS_TIMEOUT) ? AOS_SSSP_BCB_SUCCESS : AOS_SSSP_BCB_ERROR;
}

aos_ssspbcbstatus_t moduleSsspBcbReceive(uint8_t* buffer, size_t length, void* data)
{
  aosDbgCheck(buffer != NULL);
  aosDbgCheck(length > 0 && length <= CAN_BYTES_PER_FRAME);

  // check buffer for new frame
  if (((MODULE_SSSP_BCBDATA*)data)->frame.SID == MSI_BCBMSG_CANID) {
    // verify frame
    if (((MODULE_SSSP_BCBDATA*)data)->frame.DLC == length) {
      // success: fetch, invalidate buffer and return
      memcpy(buffer, ((MODULE_SSSP_BCBDATA*)data)->frame.data8, length);
      ((MODULE_SSSP_BCBDATA*)data)->frame.SID = ~MSI_BCBMSG_CANID;
      return AOS_SSSP_BCB_SUCCESS;
    }
    // an unexpected frame was received
    else {
      return AOS_SSSP_BCB_INVALIDMSG;
    }
  }
  else {
    // failure: return with error
    return AOS_SSSP_BCB_ERROR;
  }
}

#undef MSI_BCBMSG_CANID
#undef CAN_BYTES_PER_FRAME

#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) && (AMIROOS_CFG_SSSP_MSI == true) */
/** @} */

/*===========================================================================*/
/**
 * @name AMiRo-OS shell configurations
 * @{
 */
/*===========================================================================*/
#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)

ROMCONST char* moduleShellPrompt = "LightRing";

AosShellChannel moduleShellSerialCanChannel;

#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */
/** @} */

/*===========================================================================*/
/**
 * @name Low-level drivers
 * @{
 */
/*===========================================================================*/

AT24C01BDriver moduleLldEeprom = {
  /* I2C driver   */ &MODULE_HAL_I2C_EEPROM_PWRMTR_BREAKOUT,
  /* I2C address  */ 0x00u,
};

INA219Driver moduleLldPowerMonitorVled = {
  /* I2C Driver       */ &MODULE_HAL_I2C_EEPROM_PWRMTR_BREAKOUT,
  /* I²C address      */ INA219_LLD_I2C_ADDR_FIXED,
  /* current LSB (uA) */ 0x00u,
  /* configuration    */ NULL,
};

LEDDriver moduleLldStatusLed = {
  /* LED enable Gpio */ &moduleGpioLed,
};

MIC9404xDriver moduleLldPowerSwitchV18 = {
  /* power enable GPIO  */ &moduleGpioSwV18En,
};

MIC9404xDriver moduleLldPowerSwitchV33 = {
  /* power enable GPIO  */ &moduleGpioSwV33En,
};

// The 4.2V switch is disabled due to a hardware bug.
//MIC9404xDriver moduleLldPowerSwitchV42 = {
//  /* power enable GPIO  */ &moduleGpioSwV42En,
//};

MIC9404xDriver moduleLldPowerSwitchV50 = {
  /* power enable GPIO  */ &moduleGpioSwV50En,
};

MIC9404xDriver moduleLldPowerSwitchVsys = {
  /* power enable GPIO  */ &moduleGpioSwVsysEn,
};

// TODO: add SNx5C3221E

TLC5947Driver moduleLldLedPwm = {
  /* SPI driver         */ &MODULE_HAL_SPI_LIGHT,
  /* BLANK signal GPIO  */ &moduleGpioLightBlank,
  /* XLAT signal GPIO   */ &moduleGpioLightXlat,
};

#if (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_UWBv10)

DW1000Driver moduleLldDw1000 = {
  /* SPI driver     */ &MODULE_HAL_SPI_UWB,
  /* interrupt GPIO */ &moduleGpioBreakoutIo8,
  /* reset GPIO     */ &moduleGpioBreakoutIo7,
};

#endif /* (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_UWBv10) */

/** @} */

/*===========================================================================*/
/**
 * @name Tests
 * @{
 */
/*===========================================================================*/
#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)
#if (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) || defined(__DOXYGEN__)

/*
 * AT24C01BN-SH-B (EEPROM)
 */
#include "test/AT24C01B/module_test_AT24C01B.h"
static int _moduleTestAt24co1bShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestAt24c01bShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestAt24c01bShellCmd, "test:EEPROM", _moduleTestAt24co1bShellCmdCb);

/*
 * INA219 (power monitor)
 */
#include "test/INA219/module_test_INA219.h"
static int _moduleTestIna219ShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestIna219ShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestIna219ShellCmd, "test:PowerMonitor", _moduleTestIna219ShellCmdCb);

/*
 * Status LED
 */
#include "test/LED/module_test_LED.h"
static int _moduleTestLedShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestLedShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestLedShellCmd, "test:StatusLED", _moduleTestLedShellCmdCb);

/*
 * MIC9404x (power switch)
 */
#include "test/MIC9404x/module_test_MIC9404x.h"
static int _moduleTestMic9404xShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestMic9404xShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestMic9404xShellCmd, "test:PowerSwitch", _moduleTestMic9404xShellCmdCb);

// TODO: add SNx5C3221E

/*
 * TLC5947 (24 channel PWM LED driver)
 */
#include "test/TLC5947/module_test_TLC5947.h"
static int _moduleTestTlc5947ShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestTlc5947ShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestTlc5947ShellCmd, "test:Lights", _moduleTestTlc5947ShellCmdCb);

#if (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_UWBv10)

/*
 * UwB Driver (DW1000)
 */
#include "test/DW1000/module_test_DW1000.h"
static int _moduleTestDw1000ShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestDw1000ShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestDw1000ShellCmd, "test:DW1000", _moduleTestDw1000ShellCmdCb);

#endif /* (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_UWBv10) */

#endif /* (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/*
 * entire module
 */
static int _moduleTestAllShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  (void)argc;
  (void)argv;

  int status = AOS_OK;
  aos_testresult_t result_total = {0, 0};
#if (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)
  char* targv[AMIROOS_CFG_SHELL_MAXARGS] = {NULL};
  aos_testresult_t result_test = {0, 0};
#endif /* (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

#if (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)

  /* AT24C01B (EEPROM) */
  status |= moduleTestAt24c01bShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

  /* INA219 (power monitor) */
  status |= moduleTestIna219ShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

  /* Status LED */
  status |= moduleTestLedShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

  /* MIC9404x (power switch) */
  // 1.8V
  targv[1] = "1.8V";
  status |= moduleTestMic9404xShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  // 3.3V
  targv[1] = "3.3V";
  status |= moduleTestMic9404xShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
// The 4.2V switch is disabled due to a hardware bug.
//  // 4.2V
//  targv[1] = "4.2V";
//  status |= moduleTestMic9404xShellCb(stream, 2, (const char**)targv, &result_test);
//  result_total = aosTestResultAdd(result_total, result_test);
  // 5.0V
  targv[1] = "5.0V";
  status |= moduleTestMic9404xShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  //VSYS
  targv[1] = "VSYS";
  status |= moduleTestMic9404xShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  targv[1] = "";

  // TODO: add SNx5C3221E

  /* TLC5947 (24 channel PWM LED driver) */
  status |= moduleTestTlc5947ShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

#if (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_UWBv10)
  /* DW1000 (UWB transceiver) */
  status |= moduleTestDw1000ShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
#endif /* (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_UWBv10) */

#endif /* (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

  // print total result
  chprintf(stream, "\n");
  aosTestResultPrintSummary(stream, &result_total, "entire module");

  return status;
}
AOS_SHELL_COMMAND(moduleTestAllShellCmd, "test:all", _moduleTestAllShellCmdCb);

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */

/** @} */
