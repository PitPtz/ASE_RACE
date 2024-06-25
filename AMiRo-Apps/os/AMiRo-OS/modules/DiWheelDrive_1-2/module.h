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
 * @file    module.h
 * @brief   Structures and constant for the DiWheelDrive v1.2 module.
 *
 * @addtogroup diwheeldrive12
 * @{
 */

#ifndef AMIROOS_MODULE_H
#define AMIROOS_MODULE_H

#include <amiroos.h>

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

/**
 * @brief   CAN driver to use.
 */
#define MODULE_HAL_CAN                          FBCAND1

/**
 * @brief   Configuration for the CAN driver.
 */
extern ROMCONST CANConfig moduleHalCanConfig;

/**
 * @brief   I2C driver to access the bno055.
 */
#define MODULE_HAL_I2C_IMU                      I2CD1

/**
 * @brief   Configuration for the bno055 I2C driver.
 */
extern I2CConfig moduleHalI2cImuConfig;

/**
 * @brief   I2C driver to access multiplexer, proximity sensors, EEPROM and
 *          power monitor.
 */
#define MODULE_HAL_I2C_PROX_EEPROM_PWRMTR       I2CD2

/**
 * @brief   Configuration for the multiplexer, proximity, EEPROM and power
 *          monitor I2C driver.
 */
extern I2CConfig moduleHalI2cProxEepromPwrmtrConfig;

/**
 * @brief   PWM driver to use.
 */
#define MODULE_HAL_PWM_DRIVE                    PWMD2

/**
 * @brief   Configuration for the PWM driver.
 */
extern ROMCONST PWMConfig moduleHalPwmDriveConfig;

/**
 * @brief   Drive PWM channel for the left wheel forward direction.
 */
#define MODULE_HAL_PWM_DRIVE_CHANNEL_LEFT_FORWARD     ((apalPWMchannel_t)0)

/**
 * @brief   Drive PWM channel for the left wheel backward direction.
 */
#define MODULE_HAL_PWM_DRIVE_CHANNEL_LEFT_BACKWARD    ((apalPWMchannel_t)1)

/**
 * @brief   Drive PWM channel for the right wheel forward direction.
 */
#define MODULE_HAL_PWM_DRIVE_CHANNEL_RIGHT_FORWARD    ((apalPWMchannel_t)2)

/**
 * @brief   Drive PWM channel for the right wheel backward direction.
 */
#define MODULE_HAL_PWM_DRIVE_CHANNEL_RIGHT_BACKWARD   ((apalPWMchannel_t)3)

/**
 * @brief   Quadrature encooder for the left wheel.
 */
#define MODULE_HAL_QEI_LEFT_WHEEL               QEID3

/**
 * @brief   Quadrature encooder for the right wheel.
 */
#define MODULE_HAL_QEI_RIGHT_WHEEL              QEID4

/**
 * @brief   Configuration for both quadrature encoders.
 */
extern ROMCONST QEIConfig moduleHalQeiConfig;

/**
 * @brief   QEI increments per wheel revolution.
 * @details 2 signal edges per pulse * 2 signals * 16 pulses per motor
 *          revolution * 22:1 gearbox
 */
#define MODULE_HAL_QEI_INCREMENTS_PER_REVOLUTION  (apalQEICount_t)(2 * 2 * 16 * 22)

/**
 * @brief   Serial driver of the programmer interface.
 */
#define MODULE_HAL_SERIAL                       SD1

/**
 * @brief   Configuration for the programmer serial interface driver.
 */
extern ROMCONST SerialConfig moduleHalSerialConfig;

/**
 * @brief   Real-Time Clock driver.
 */
#define MODULE_HAL_RTC                          RTCD1

/**
 * @brief   SerialCANDriver to be used by the system shell.
 */
extern SerialCANDriver moduleHalSerialCAN;

/**
 * @brief   Configuration for the shell SerialCANDriver.
 */
extern ROMCONST SerialCANConfig moduleHalSerialCanConfig;

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
extern ROMCONST apalControlGpio_t moduleGpioLed;

/**
 * @brief   POWER_EN output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioPowerEn;

/**
 * @brief   IR_INT input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioIrInt;

/**
 * @brief   SYS_UART_UP bidirectional signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysUartUp;

/**
 * @brief   IMU_INT input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioImuInt;

/**
 * @brief   SYS_SNYC bidirectional signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysSync;

/**
 * @brief   IMU_RESET output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioImuReset;

/**
 * @brief   PATH_DCSTAT input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioPathDcStat;

/**
 * @brief   PATH_DCEN output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioPathDcEn;

/**
 * @brief   SYS_PD bidirectional signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysPd;

/**
 * @brief   SYS_REG_EN input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysRegEn;

/**
 * @brief   IMU_BOOT_LOAD output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioImuBootLoad;

/**
 * @brief   IMU_BL_IND input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioImuBlInd;

/**
 * @brief   SYS_WARMRST bidirectional signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysWarmrst;

/** @} */

/*===========================================================================*/
/**
 * @name AMiRo-OS core configurations
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Event flag to be set on a SYS_SYNC interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_SYSSYNC         AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_SYS_INT_N))

/**
 * @brief   Event flag to be set on a SYS_WARMRST interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_SYSWARMRST      AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_SYS_WARMRST_N))

/**
 * @brief   Event flag to be set on a PATH_DCSTAT interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_PATHDCSTAT      AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_PATH_DCEN))

/**
 * @brief   Event flag to be set on a SYS_PD interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_SYSPD           AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_SYS_PD_N))

/**
 * @brief   Event flag to be set on a SYS_REG_EN interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_SYSREGEN        AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_SYS_REG_EN))

/**
 * @brief   Event flag to be set on a IR_INT interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_IRINT           AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_IR_INT))

/**
 * @brief   Event flag to be set on a SYS_UART_UP interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_SYSUARTUP       AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_SYS_UART_UP))

/**
 * @brief   Event flag to be set on a IMU_INT interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_IMUINT          AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_IMU_INT))

/**
 * @brief   Interrupt initialization hook.
 */
#define MODULE_INIT_INTERRUPTS() {                                              \
  moduleInitInterrupts();                                                       \
}

/**
 * @brief   Periphery communication interfaces initialization hook.
 */
#define MODULE_INIT_PERIPHERY_INTERFACES() {                                    \
  moduleInitPeripheryInterfaces();                                              \
}

#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)
/**
 * @brief   Test initialization hook.
 */
#define MODULE_INIT_TESTS() {                                                   \
  moduleInitTests();                                                            \
}
#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */

/**
 * @brief   Periphery communication interface deinitialization hook.
 */
#define MODULE_SHUTDOWN_PERIPHERY_INTERFACES() {                                \
  moduleShutdownPeripheryInterfaces();                                          \
}

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
void moduleInitInterrupts(void);
void moduleInitPeripheryInterfaces(void);
#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)
void moduleInitTests(void);
#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */
void moduleShutdownPeripheryInterfaces(void);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/** @} */

/*===========================================================================*/
/**
 * @name Startup Shutdown Synchronization Protocol (SSSP)
 * @{
 */
/*===========================================================================*/
#if (AMIROOS_CFG_SSSP_ENABLE == true) || defined(__DOXYGEN__)

#define moduleSsspSignalPD()                    (&moduleGpioSysPd)
#define moduleSsspEventflagPD()                 MODULE_OS_GPIOEVENTFLAG_SYSPD

#define moduleSsspSignalS()                     (&moduleGpioSysSync)
#define moduleSsspEventflagS()                  MODULE_OS_GPIOEVENTFLAG_SYSSYNC

#define moduleSsspSignalUP()                    (&moduleGpioSysUartUp)
#define moduleSsspEventflagUP()                 MODULE_OS_GPIOEVENTFLAG_SYSUARTUP

#if (AMIROOS_CFG_SSSP_MSI == true) || defined(__DOXYGEN__)

struct module_sssp_bcbdata {
  aos_fbcan_filter_t filter;
  CANRxFrame frame;
};

#define MODULE_SSSP_BCBDATA                     struct module_sssp_bcbdata

#endif /* (AMIROOS_CFG_SSSP_MSI == true) */

#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */
/** @} */

/*===========================================================================*/
/**
 * @name AMiRo-OS shell configurations
 * @{
 */
/*===========================================================================*/
#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)

/**
 * @brief   Shell prompt text.
 */
extern ROMCONST char* moduleShellPrompt;

/**
 * @brief   Shell stream channel for CAN I/O.
 */
extern AosShellChannel moduleShellSerialCanChannel;

/**
 * @brief   Shell setup hook.
 */
#define MODULE_SHELL_SETUP_HOOK() {                                             \
  aosShellChannelInit(&moduleShellSerialCanChannel, (BaseAsynchronousChannel*)&moduleHalSerialCAN, AOS_SHELLCHANNEL_REMOTE); \
  aosShellStreamAddChannel(&aos.shell.stream, &moduleShellSerialCanChannel);    \
}

#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */
/** @} */

/*===========================================================================*/
/**
 * @name Low-level drivers
 * @{
 */
/*===========================================================================*/
#include <alld_A3906.h>
#include <alld_AT24C01B.h>
#include <alld_BNO055.h>
#include <alld_INA219.h>
#include <alld_LED.h>
#include <alld_LTC4412.h>
#include <alld_PCA9544A.h>
#include <alld_TPS6211x.h>
#include <alld_VCNL4020.h>

/**
 * @brief   Motor driver.
 */
extern A3906Driver moduleLldMotors;

/**
 * @brief   EEPROM driver.
 */
extern AT24C01BDriver moduleLldEeprom;

/**
 * @brief   BNO055 driver.
 */
extern BNO055DRIVER moduleLldBno;

/**
 * @brief   Power monitor (VDD) driver.
 */
extern INA219Driver moduleLldPowerMonitorVdd;

/**
 * @brief   Status LED driver.
 */
extern LEDDriver moduleLldStatusLed;

/**
 * @brief   Power path controler (charging pins) driver.
 */
extern LTC4412Driver moduleLldPowerPathController;

/**
 * @brief   I2C multiplexer driver.
 */
extern PCA9544ADriver moduleLldI2cMultiplexer;

/**
 * @brief   Step down converter (VDRIVE) driver.
 */
extern TPS6211xDriver moduleLldStepDownConverterVdrive;

/**
 * @brief   Proximity sensor driver.
 */
extern VCNL4020Driver moduleLldProximity;

/** @} */

/*===========================================================================*/
/**
 * @name Tests
 * @{
 */
/*===========================================================================*/
#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)
#if (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) || defined(__DOXYGEN__)

/**
 * @brief   A3906 (motor driver) test command.
 */
extern aos_shellcommand_t moduleTestA3906ShellCmd;

/**
 * @brief   AT24C01BN-SH-B (EEPROM) test command.
 */
extern aos_shellcommand_t moduleTestAt24c01bShellCmd;

/**
 * @brief  BNO055 (IMU) test command.
 */
extern aos_shellcommand_t moduleTestBno055ShellCmd;
/**
 * @brief   INA219 (power monitor) test command.
 */
extern aos_shellcommand_t moduleTestIna219ShellCmd;

/**
 * @brief   Status LED test command.
 */
extern aos_shellcommand_t moduleTestLedShellCmd;

/**
 * @brief   LTC4412 (power path controller) test command.
 */
extern aos_shellcommand_t moduleTestLtc4412ShellCmd;

/**
 * @brief   PCA9544A (I2C multiplexer) test command.
 */
extern aos_shellcommand_t moduleTestPca9544aShellCmd;

/**
 * @brief   TPS62113 (step-down converter) test command.
 */
extern aos_shellcommand_t moduleTestTps62113ShellCmd;

/**
 * @brief   VCNL4020 (proximity sensor) test command.
 */
extern aos_shellcommand_t moduleTestVcnl4020ShellCmd;

#endif /* (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/**
 * @brief   Entire module test command.
 */
extern aos_shellcommand_t moduleTestAllShellCmd;

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */

/** @} */

#endif /* AMIROOS_MODULE_H */

/** @} */
