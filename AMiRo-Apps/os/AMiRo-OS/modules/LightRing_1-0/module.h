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
 * @brief   Structures and constant for the LightRing v1.0 module.
 *
 * @addtogroup lightring10
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
 * @brief   I2C driver to access the EEPROM.
 */
#define MODULE_HAL_I2C_EEPROM                   I2CD2

/**
 * @brief   Configuration for the EEPROM I2C driver.
 */
extern I2CConfig moduleHalI2cEepromConfig;

/**
 * @brief   SPI interface driver for the motion sensors (gyroscope and
 *          accelerometer).
 */
#define MODULE_HAL_SPI_LIGHT                    SPID1

/**
 * @brief   SPI interface driver for the wireless transceiver.
 */
#define MODULE_HAL_SPI_WL                       SPID2

/**
 * @brief   Configuration for the SPI interface driver to communicate with the
 *          LED driver.
 */
extern ROMCONST SPIConfig moduleHalSpiLightConfig;

/**
 * @brief   Configuration for the SPI interface driver to communicate with the
 *          wireless transceiver.
 */
extern ROMCONST SPIConfig moduleHalSpiWlConfig;

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

#if (AMIROOS_CFG_DBG == true) || defined(__DOXYGEN__)

/**
 * @brief   Serial driver of the programmer interface.
 */
#define MODULE_HAL_SERIAL                       SD1

/**
 * @brief   Configuration for the programmer serial interface driver.
 */
extern ROMCONST SerialConfig moduleHalSerialConfig;

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
extern ROMCONST apalControlGpio_t moduleGpioLightBlank;

/**
 * @brief   LASER_EN output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioLaserEn;

/**
 * @brief   LASER_OC input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioLaserOc;

/**
 * @brief   SYS_UART_DN bidirectional signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysUartDn;

/**
 * @brief   WL_GDO2 input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioWlGdo2;

/**
 * @brief   WL_GDO0 input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioWlGdo0;

/**
 * @brief   LIGHT_XLAT output signal.
 */
extern ROMCONST apalControlGpio_t moduleGpioLightXlat;

/**
 * @brief   SYS_PD bidirectional signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysPd;

/**
 * @brief   SYS_SYNC bidirectional signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysSync;

/** @} */

/*===========================================================================*/
/**
 * @name AMiRo-OS core configurations
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Event flag to be set on a LASER_OC interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_LASEROC         AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_LASER_OC_N))

/**
 * @brief   Event flag to be set on a SYS_UART_DN interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_SYSUARTDN       AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_SYS_UART_DN))

/**
 * @brief   Event flag to be set on a WL_GDO2 interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_WLGDO2          AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_WL_GDO2))

/**
 * @brief   Event flag to be set on a WL_GDO0 interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_WLGDO0          AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_WL_GDO0))

/**
 * @brief   Event flag to be set on a SYS_PD interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_SYSPD           AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_SYS_PD_N))

/**
 * @brief   Event flag to be set on a SYS_SYNC interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_SYSSYNC         AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_SYS_INT_N))

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

#define moduleSsspSignalDN()                    (&moduleGpioSysUartDn)
#define moduleSsspEventflagDN()                 MODULE_OS_GPIOEVENTFLAG_SYSUARTDN

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
#include <alld_AT24C01B.h>
#include <alld_TLC5947.h>
#include <alld_TPS20xxB.h>

/**
 * @brief   EEPROM driver.
 */
extern AT24C01BDriver moduleLldEeprom;

/**
 * @brief   24 channel PWM LED driver.
 */
extern TLC5947Driver moduleLldLedPwm;

/**
 * @brief   Power switch driver for the laser supply power.
 */
extern TPS20xxBDriver moduleLldPowerSwitchLaser;

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
 * @brief   AT24C01BN-SH-B (EEPROM) test command.
 */
extern aos_shellcommand_t moduleTestAt24c01bShellCmd;

/**
 * @brief   TLC5947 (24 channel PWM LED driver) test command
 */
extern aos_shellcommand_t moduleTestTlc5947ShellCmd;

/**
 * @brief   TPS2051BDBV (Current-limited power switch) test command
 */
extern aos_shellcommand_t moduleTestTps2051bdbvShellCmd;

#endif /* (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/**
 * @brief   Entire module test command.
 */
extern aos_shellcommand_t moduleTestAllShellCmd;

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */

/** @} */

#endif /* AMIROOS_MODULE_H */

/** @} */
