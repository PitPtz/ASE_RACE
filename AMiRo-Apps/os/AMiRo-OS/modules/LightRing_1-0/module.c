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
 * @brief   Structures and constant for the LightRing v1.0 module.
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

I2CConfig moduleHalI2cEepromConfig = {
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

ROMCONST SPIConfig moduleHalSpiWlConfig = {
  /* circular buffer mode             */ false,
  /* slave mode                       */ false,
  /* data callback function pointer   */ NULL,
  /* error callback function pointer  */ NULL,
  /* chip select line                 */ LINE_WL_SS_N,
  /* CR1                              */ SPI_CR1_BR_0,
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

/** @} */

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
 * @brief   LASER_EN output signal GPIO.
 */
static apalGpio_t _gpioLaserEn = {
  /* line */ LINE_LASER_EN,
};
ROMCONST apalControlGpio_t moduleGpioLaserEn = {
  /* GPIO */ &_gpioLaserEn,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ TPS20xxB_LLD_ENABLE_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   LASER_OC input signal GPIO.
 */
static apalGpio_t _gpioLaserOc = {
  /* line */ LINE_LASER_OC_N,
};
ROMCONST apalControlGpio_t moduleGpioLaserOc = {
  /* GPIO */ &_gpioLaserOc,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ TPS20xxB_LLD_OVERCURRENT_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
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
 * @brief   WL_GDO2 input signal GPIO.
 */
static apalGpio_t _gpioWlGdo2 = {
  /* line */ LINE_WL_GDO2,
};
ROMCONST apalControlGpio_t moduleGpioWlGdo2 = {
  /* GPIO */ &_gpioWlGdo2,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   WL_GDO0 input signal GPIO.
 */
static apalGpio_t _gpioWlGdo0= {
  /* line */ LINE_WL_GDO0,
};
ROMCONST apalControlGpio_t moduleGpioWlGdo0 = {
  /* GPIO */ &_gpioWlGdo0,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
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
  // LASER_OC
  palSetLineCallback(moduleGpioLaserOc.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioLaserOc.gpio->line);
  palEnableLineEvent(moduleGpioLaserOc.gpio->line, APAL2CH_EDGE(moduleGpioLaserOc.meta.edge));

  // WL_GDO2
  palSetLineCallback(moduleGpioWlGdo2.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioWlGdo2.gpio->line);
  palEnableLineEvent(moduleGpioWlGdo2.gpio->line, APAL2CH_EDGE(moduleGpioWlGdo2.meta.edge));

  // WL_GDO0
  palSetLineCallback(moduleGpioWlGdo0.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioWlGdo0.gpio->line);
  /*palEnableLineEvent(moduleGpioWlGdo0.gpio->line, APAL2CH_EDGE(moduleGpioWlGdo0.meta.edge)); // this is broken for some reason*/

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

  // I2C
  moduleHalI2cEepromConfig.clock_speed = (AT24C01B_LLD_I2C_MAXFREQUENCY < moduleHalI2cEepromConfig.clock_speed) ? AT24C01B_LLD_I2C_MAXFREQUENCY : moduleHalI2cEepromConfig.clock_speed;
  moduleHalI2cEepromConfig.duty_cycle = (moduleHalI2cEepromConfig.clock_speed <= 100000) ? STD_DUTY_CYCLE : FAST_DUTY_CYCLE_2;
  i2cStart(&MODULE_HAL_I2C_EEPROM, &moduleHalI2cEepromConfig);

  // SPI
  spiStart(&MODULE_HAL_SPI_LIGHT, &moduleHalSpiLightConfig);
  spiStart(&MODULE_HAL_SPI_WL, &moduleHalSpiWlConfig);

  // CAN
  aosFBCanInit(&MODULE_HAL_CAN, NULL, NULL);
  aosFBCanStart(&MODULE_HAL_CAN, &moduleHalCanConfig);
  sdcanInit();
  sdcanObjectInit(&moduleHalSerialCAN);
  sdcanStart(&moduleHalSerialCAN, &moduleHalSerialCanConfig);

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
  aosShellAddCommand(&moduleTestTlc5947ShellCmd);
  aosShellAddCommand(&moduleTestTps2051bdbvShellCmd);
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
  // CA
  sdcanStop(&moduleHalSerialCAN);
  aosFBCanStop(&MODULE_HAL_CAN);

  // SPI
  spiStop(&MODULE_HAL_SPI_LIGHT);
  spiStop(&MODULE_HAL_SPI_WL);

  // I2C
  i2cStop(&MODULE_HAL_I2C_EEPROM);

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
  /* I2C driver   */ &MODULE_HAL_I2C_EEPROM,
  /* I2C address  */ 0x00u,
};

TLC5947Driver moduleLldLedPwm = {
  /* SPI driver         */ &MODULE_HAL_SPI_LIGHT,
  /* BLANK signal GPIO  */ &moduleGpioLightBlank,
  /* XLAT signal GPIO   */ &moduleGpioLightXlat,
};

TPS20xxBDriver moduleLldPowerSwitchLaser = {
  /* laser enable GPIO      */ &moduleGpioLaserEn,
  /* laser overcurrent GPIO */ &moduleGpioLaserOc,
};

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
 * TLC5947 (24 channel PWM LED driver)
 */
#include "test/TLC5947/module_test_TLC5947.h"
static int _moduleTestTlc5947ShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestTlc5947ShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestTlc5947ShellCmd, "test:Lights", _moduleTestTlc5947ShellCmdCb);

/*
 * TPS2051BDBV (Current-limited power switch)
 */
#include "test/TPS20xxB/module_test_TPS20xxB.h"
static int _moduleTestTps2051bdbvShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestTps20xxbShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestTps2051bdbvShellCmd, "test:PowerSwitch", _moduleTestTps2051bdbvShellCmdCb);

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

  /* TLC5947 (24 channel LED PWM driver) */
  status |= moduleTestTlc5947ShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

  /* TPS2051BDBV (Current-limited power switch) */
  status |= moduleTestTps20xxbShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

#endif /* (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

  // print total result
  chprintf(stream, "\n");
  aosTestResultPrintSummary(stream, &result_total, "entire module");

  return status;
}
AOS_SHELL_COMMAND(moduleTestAllShellCmd, "test:all", _moduleTestAllShellCmdCb);

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */

/** @} */
