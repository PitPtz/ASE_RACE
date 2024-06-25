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
 * @brief   Structures and constant for the DiWheelDrive v1.2 module.
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

I2CConfig moduleHalI2cImuConfig = {
  /* I²C mode   */ OPMODE_I2C,
  /* frequency  */ 400000,
  /* duty cycle */ FAST_DUTY_CYCLE_2,
};

I2CConfig moduleHalI2cProxEepromPwrmtrConfig = {
  /* I²C mode   */ OPMODE_I2C,
  /* frequency  */ 400000,
  /* duty cycle */ FAST_DUTY_CYCLE_2,
};

ROMCONST PWMConfig moduleHalPwmDriveConfig = {
  /* frequency              */ 72000000,
  /* period                 */ 4096,  // 72MHz / 4096 = 17.6kHz motor noise
  /* callback               */ NULL,
  /* channel configurations */ {
    /* channel 0              */ {
      /* mode                   */ PWM_OUTPUT_ACTIVE_HIGH,
      /* callback               */ NULL
    },
    /* channel 1              */ {
      /* mode                   */ PWM_OUTPUT_ACTIVE_HIGH,
      /* callback               */ NULL
    },
    /* channel 2              */ {
      /* mode                   */ PWM_OUTPUT_ACTIVE_HIGH,
      /* callback               */ NULL
    },
    /* channel 3              */ {
      /* mode                   */ PWM_OUTPUT_ACTIVE_HIGH,
      /* callback               */ NULL
    },
  },
  /* TIM CR2 register       */ 0,
  /* TIM BDTR register      */ 0,
  /* TIM DIER register      */ 0
};

ROMCONST QEIConfig moduleHalQeiConfig = {
  .mode = QEI_MODE_QUADRATURE,
  .resolution = QEI_BOTH_EDGES,
  .dirinv = QEI_DIRINV_FALSE,
  .overflow = QEI_OVERFLOW_WRAP,
  .min = QEI_COUNT_MIN,
  .max = QEI_COUNT_MAX,
  .notify_cb = NULL,
  .overflow_cb = NULL,
};

ROMCONST SerialConfig moduleHalSerialConfig = {
  /* bit rate */ 115200,
  /* CR1      */ 0,
  /* CR1      */ 0,
  /* CR1      */ 0,
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

/** @} */

/*===========================================================================*/
/**
 * @name GPIO definitions
 * @{
 */
/*===========================================================================*/

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
 * @brief   POWER_EN output signal GPIO.
 */
static apalGpio_t _gpioPowerEn = {
  /* line */ LINE_POWER_EN,
};
ROMCONST apalControlGpio_t moduleGpioPowerEn = {
  /* GPIO */ &_gpioPowerEn,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   IR_INT input signal GPIO.
 */
static apalGpio_t _gpioIrInt = {
  /* line */ LINE_IR_INT,
};
ROMCONST apalControlGpio_t moduleGpioIrInt = {
  /* GPIO */ &_gpioIrInt,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ (VCNL4020_LLD_INT_EDGE == APAL_GPIO_EDGE_RISING) ? APAL_GPIO_ACTIVE_HIGH : APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ VCNL4020_LLD_INT_EDGE,
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
 * @brief   IMU_INT input signal GPIO.
 */
static apalGpio_t _gpioImuInt = {
  /* line */ LINE_IMU_INT,
};
ROMCONST apalControlGpio_t moduleGpioImuInt = {
  /* GPIO */ &_gpioImuInt,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH, // TODO
    /* interrupt edge */ APAL_GPIO_EDGE_RISING, // TODO
  },
};

/**
 * @brief   SYS_SNYC bidirectional signal GPIO.
 */
static apalGpio_t _gpioSysSync = {
  /* line */ LINE_SYS_INT_N,
};
ROMCONST apalControlGpio_t  moduleGpioSysSync = {
  /* GPIO */ &_gpioSysSync,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   IMU_RESET output signal GPIO.
 */
static apalGpio_t _gpioImuReset = {
  /* line */ LINE_IMU_RESET_N,
};
ROMCONST apalControlGpio_t  moduleGpioImuReset = {
  /* GPIO */ &_gpioImuReset,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_LOW, // TODO
    /* interrupt edge */ APAL_GPIO_EDGE_NONE, // TODO
  },
};

/**
 * @brief   PATH_DCSTAT input signal GPIO.
 */
static apalGpio_t _gpioPathDcStat = {
  /* line */ LINE_PATH_DCSTAT,
};
ROMCONST apalControlGpio_t moduleGpioPathDcStat = {
  /* GPIO */ &_gpioPathDcStat,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ LTC4412_LLD_STAT_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   PATH_DCEN output signal GPIO.
 */
static apalGpio_t _gpioPathDcEn = {
  /* line */ LINE_PATH_DCEN,
};
ROMCONST apalControlGpio_t moduleGpioPathDcEn = {
  /* GPIO */ &_gpioPathDcEn,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ LTC4412_LLD_CTRL_ACTIVE_STATE,
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
 * @brief   SYS_REG_EN input signal GPIO.
 */
static apalGpio_t _gpioSysRegEn = {
  /* line */ LINE_SYS_REG_EN,
};
ROMCONST apalControlGpio_t moduleGpioSysRegEn = {
  /* GPIO */ &_gpioSysRegEn,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   IMU_BOOT_LOAD output signal GPIO.
 */
static apalGpio_t _gpioImuBootLoad = {
  /* line */ LINE_IMU_BOOT_LOAD_N,
};
ROMCONST apalControlGpio_t moduleGpioImuBootLoad = {
  /* GPIO */ &_gpioImuBootLoad,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_LOW, // TODO
    /* interrupt edge */ APAL_GPIO_EDGE_NONE, // TODO
  },
};

/**
 * @brief   IMU_BL_IND input signal GPIO.
 */
static apalGpio_t _gpioImuBlInd = {
  /* line */ LINE_IMU_BL_IND,
};
ROMCONST apalControlGpio_t moduleGpioImuBlInd = {
  /* GPIO */ &_gpioImuBlInd,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH, // TODO
    /* interrupt edge */ APAL_GPIO_EDGE_NONE, // TODO
  },
};

/**
 * @brief   SYS_WARMRST bidirectional signal GPIO.
 */
static apalGpio_t _gpioSysWarmrst = {
  /* line */ LINE_SYS_WARMRST_N,
};
ROMCONST apalControlGpio_t moduleGpioSysWarmrst = {
  /* GPIO */ &_gpioSysWarmrst,
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
  // IR_INT
  palSetLineCallback(moduleGpioIrInt.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioIrInt.gpio->line);
  palEnableLineEvent(moduleGpioIrInt.gpio->line, APAL2CH_EDGE(moduleGpioIrInt.meta.edge));

  // IMU_INT
  palSetLineCallback(moduleGpioImuInt.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioImuInt.gpio->line);
  palEnableLineEvent(moduleGpioImuInt.gpio->line, APAL2CH_EDGE(moduleGpioImuInt.meta.edge));

  // PATH_DCSTAT
  palSetLineCallback(moduleGpioPathDcStat.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioPathDcStat.gpio->line);
  palEnableLineEvent(moduleGpioPathDcStat.gpio->line, APAL2CH_EDGE(moduleGpioPathDcStat.meta.edge));

  // SYS_REG_EN
  palSetLineCallback(moduleGpioSysRegEn.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioSysRegEn.gpio->line);
  palEnableLineEvent(moduleGpioSysRegEn.gpio->line, APAL2CH_EDGE(moduleGpioSysRegEn.meta.edge));

  // SYS_WARMRST
  palSetLineCallback(moduleGpioSysWarmrst.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioSysWarmrst.gpio->line);
  palEnableLineEvent(moduleGpioSysWarmrst.gpio->line, APAL2CH_EDGE(moduleGpioSysWarmrst.meta.edge));

  return;
}

/**
 * @brief   Initializes periphery communication interfaces.
 */
void moduleInitPeripheryInterfaces()
{
  // serial driver
  sdStart(&MODULE_HAL_SERIAL, &moduleHalSerialConfig);

  // I2C 1
  /* TODO: calculcate config depending on BNO055 */
  moduleHalI2cImuConfig.clock_speed = (BNO055_LLD_I2C_MAXFREQUENCY < moduleHalI2cImuConfig.clock_speed) ? BNO055_LLD_I2C_MAXFREQUENCY : moduleHalI2cImuConfig.clock_speed;
  moduleHalI2cImuConfig.duty_cycle = (moduleHalI2cImuConfig.clock_speed <= 100000) ? STD_DUTY_CYCLE : FAST_DUTY_CYCLE_2;
  i2cStart(&MODULE_HAL_I2C_IMU, &moduleHalI2cImuConfig);
  // I2C 2
  moduleHalI2cProxEepromPwrmtrConfig.clock_speed = (PCA9544A_LLD_I2C_MAXFREQUENCY < moduleHalI2cProxEepromPwrmtrConfig.clock_speed) ? PCA9544A_LLD_I2C_MAXFREQUENCY : moduleHalI2cProxEepromPwrmtrConfig.clock_speed;
  moduleHalI2cProxEepromPwrmtrConfig.clock_speed = (VCNL4020_LLD_I2C_MAXFREQUENCY < moduleHalI2cProxEepromPwrmtrConfig.clock_speed) ? VCNL4020_LLD_I2C_MAXFREQUENCY : moduleHalI2cProxEepromPwrmtrConfig.clock_speed;
  moduleHalI2cProxEepromPwrmtrConfig.clock_speed = (AT24C01B_LLD_I2C_MAXFREQUENCY < moduleHalI2cProxEepromPwrmtrConfig.clock_speed) ? AT24C01B_LLD_I2C_MAXFREQUENCY : moduleHalI2cProxEepromPwrmtrConfig.clock_speed;
  moduleHalI2cProxEepromPwrmtrConfig.clock_speed = (INA219_LLD_I2C_MAXFREQUENCY < moduleHalI2cProxEepromPwrmtrConfig.clock_speed) ? INA219_LLD_I2C_MAXFREQUENCY : moduleHalI2cProxEepromPwrmtrConfig.clock_speed;
  moduleHalI2cProxEepromPwrmtrConfig.duty_cycle = (moduleHalI2cProxEepromPwrmtrConfig.clock_speed <= 100000) ? STD_DUTY_CYCLE : FAST_DUTY_CYCLE_2;
  i2cStart(&MODULE_HAL_I2C_PROX_EEPROM_PWRMTR, &moduleHalI2cProxEepromPwrmtrConfig);

  // PWM
  pwmStart(&MODULE_HAL_PWM_DRIVE, &moduleHalPwmDriveConfig);

  // QEI
  qeiStart(&MODULE_HAL_QEI_LEFT_WHEEL, &moduleHalQeiConfig);
  qeiStart(&MODULE_HAL_QEI_RIGHT_WHEEL, &moduleHalQeiConfig);
  qeiEnable(&MODULE_HAL_QEI_LEFT_WHEEL);
  qeiEnable(&MODULE_HAL_QEI_RIGHT_WHEEL);

  // CAN
  aosFBCanInit(&MODULE_HAL_CAN, NULL, NULL);
  aosFBCanStart(&MODULE_HAL_CAN, &moduleHalCanConfig);
  sdcanInit();
  sdcanObjectInit(&moduleHalSerialCAN);
  sdcanStart(&moduleHalSerialCAN, &moduleHalSerialCanConfig);

  #if (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_ESP) || defined(__DOXYGEN__)
    aosThdSSleep(4);
  #endif /* (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_ESP) */

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
  aosShellAddCommand(&moduleTestA3906ShellCmd);
  aosShellAddCommand(&moduleTestAt24c01bShellCmd);
  aosShellAddCommand(&moduleTestBno055ShellCmd);
  aosShellAddCommand(&moduleTestIna219ShellCmd);
  aosShellAddCommand(&moduleTestLedShellCmd);
  aosShellAddCommand(&moduleTestLtc4412ShellCmd);
  aosShellAddCommand(&moduleTestPca9544aShellCmd);
  aosShellAddCommand(&moduleTestTps62113ShellCmd);
  aosShellAddCommand(&moduleTestVcnl4020ShellCmd);
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

  // PWM
  pwmStop(&MODULE_HAL_PWM_DRIVE);

  // QEI
  qeiDisable(&MODULE_HAL_QEI_LEFT_WHEEL);
  qeiDisable(&MODULE_HAL_QEI_RIGHT_WHEEL);
  qeiStop(&MODULE_HAL_QEI_LEFT_WHEEL);
  qeiStop(&MODULE_HAL_QEI_RIGHT_WHEEL);

  // I2C
  i2cStop(&MODULE_HAL_I2C_IMU);
  i2cStop(&MODULE_HAL_I2C_PROX_EEPROM_PWRMTR);

  // serial driver
  /* don't stop the serial driver so messages can still be printed */

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

ROMCONST char* moduleShellPrompt = "DiWheelDrive";

AosShellChannel moduleShellSerialCanChannel;

#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */
/** @} */

/*===========================================================================*/
/**
 * @name Low-level drivers
 * @{
 */
/*===========================================================================*/

A3906Driver moduleLldMotors = {
  /* power enable GPIO  */ &moduleGpioPowerEn,
};

AT24C01BDriver moduleLldEeprom = {
  /* I2C driver   */ &MODULE_HAL_I2C_PROX_EEPROM_PWRMTR,
  /* I²C address  */ AT24C01B_LLD_I2C_ADDR_FIXED,
};

BNO055DRIVER moduleLldBno = {
  /* I2C driver        */ &MODULE_HAL_I2C_IMU,
  /* I²C address       */ BNO055_I2C_ADDR1,
  /* BNO Driver struct */ {},
};

INA219Driver moduleLldPowerMonitorVdd = {
  /* I2C Driver       */ &MODULE_HAL_I2C_PROX_EEPROM_PWRMTR,
  /* I²C address      */ INA219_LLD_I2C_ADDR_FIXED,
  /* current LSB (uA) */ 0x00u,
  /* configuration    */ NULL,
};

LEDDriver moduleLldStatusLed = {
  /* LED enable Gpio */ &moduleGpioLed,
};

LTC4412Driver moduleLldPowerPathController = {
  /* Control GPIO */ &moduleGpioPathDcEn,
  /* Status GPIO  */ &moduleGpioPathDcStat,
};

PCA9544ADriver moduleLldI2cMultiplexer = {
  /* I²C driver   */ &MODULE_HAL_I2C_PROX_EEPROM_PWRMTR,
  /* I²C address  */ PCA9544A_LLD_I2C_ADDR_FIXED | PCA9544A_LLD_I2C_ADDR_A0 | PCA9544A_LLD_I2C_ADDR_A1 | PCA9544A_LLD_I2C_ADDR_A2,
};

TPS6211xDriver moduleLldStepDownConverterVdrive = {
  /* Power enable Gpio */ &moduleGpioPowerEn,
};

VCNL4020Driver moduleLldProximity = {
  /* I²C Driver */ &MODULE_HAL_I2C_PROX_EEPROM_PWRMTR,
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
 * A3906 (motor driver)
 */
#include "test/A3906/module_test_A3906.h"
static int _moduleTestA3906ShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestA3906ShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestA3906ShellCmd, "test:MotorDriver", _moduleTestA3906ShellCmdCb);

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
 * BNO055 (IMU)
 */
#include "test/BNO055/module_test_BNO055.h"
static int _moduleTestBno055ShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestBno055ShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestBno055ShellCmd, "test:IMU", _moduleTestBno055ShellCmdCb);

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
 * LTC4412 (power path controller)
 */
#include "test/LTC4412/module_test_LTC4412.h"
static int _moduleTestLtc4412ShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestLtc4412ShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestLtc4412ShellCmd, "test:PowerPathController", _moduleTestLtc4412ShellCmdCb);

/*
 * PCA9544A (I2C multiplexer)
 */
#include "test/PCA9544A/module_test_PCA9544A.h"
static int _moduleTestPca9544aShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestPca9544aShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestPca9544aShellCmd, "test:I2CMultiplexer", _moduleTestPca9544aShellCmdCb);

/*
 * TPS62113 (step-down converter)
 */
#include "test/TPS6211x/module_test_TPS6211x.h"
static int _moduleTestTps6211xShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestTps6211xShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestTps62113ShellCmd, "test:StepDownConverter", _moduleTestTps6211xShellCmdCb);

/*
 * VCNL4020 (proximity sensor)
 */
#include "test/VCNL4020/module_test_VCNL4020.h"
static int _moduleTestVcnl4020ShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestVcnl4020ShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestVcnl4020ShellCmd, "test:ProximitySensor", _moduleTestVcnl4020ShellCmdCb);

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

  /* A3906 (motor driver) */
  status |= moduleTestA3906ShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

  /* AT24C01B (EEPROM) */
  status |= moduleTestAt24c01bShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

  /* BNO055 (IMU) */
  status |= moduleTestBno055ShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

  /* INA219 (power monitor) */
  status |= moduleTestIna219ShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

  /* Status LED */
  status |= moduleTestLedShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

  /* LTC4412 (power path controller) */
  status |= moduleTestLtc4412ShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

  /* PCA9544A (I2C multiplexer) */
  status |= moduleTestPca9544aShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

  /* TPS62113 (step-down converter) */
  status |= moduleTestTps6211xShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

  /* VCNL4020 (proximity sensor) */
  // wheel left
  targv[1] = "-wl";
  status |= moduleTestVcnl4020ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  // front left
  targv[1] = "-fl";
  status |= moduleTestVcnl4020ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  // front right
  targv[1] = "-fr";
  status |= moduleTestVcnl4020ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  // wheel right
  targv[1] = "-wr";
  status |= moduleTestVcnl4020ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  targv[1] = "";

#endif /* (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

  // print total result
  chprintf(stream, "\n");
  aosTestResultPrintSummary(stream, &result_total, "entire module");

  return status;
}
AOS_SHELL_COMMAND(moduleTestAllShellCmd, "test:all", _moduleTestAllShellCmdCb);

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */

/** @} */
