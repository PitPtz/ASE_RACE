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
 * @brief   Structures and constant for the LightRing v1.2 module.
 *
 * @addtogroup lightring12
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
 * @brief   I2C driver to access the EEPROM, power monitor and the breakout
 *          header.
 */
#define MODULE_HAL_I2C_EEPROM_PWRMTR_BREAKOUT   I2CD2

/**
 * @brief   Configuration for the EEPROM, power monitor and breakout I2C driver.
 */
extern I2CConfig moduleHalI2cEepromPwrmtrBreakoutConfig;

/**
 * @brief   SPI interface driver for the motion sensors (gyroscope and
 *          accelerometer).
 */
#define MODULE_HAL_SPI_LIGHT                    SPID1

/**
 * @brief   Configuration for the SPI interface driver to communicate with the
 *          LED driver.
 */
extern ROMCONST SPIConfig moduleHalSpiLightConfig;

/**
 * @brief   SPI interface driver for the breakout header.
 */
#define MODULE_HAL_SPI_BREAKOUT                 SPID2

/**
 * @brief   UART interface driver for the breakout header (alternative to
 *          serial).
 */
#define MODULE_HAL_UART_BREAKOUT                UARTD2

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

#if (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_UWBv10) || defined(__DOXYGEN__)

/**
 * @brief   SPI interface driver for UWB DW1000 module.
 */
#define MODULE_HAL_SPI_UWB                      MODULE_HAL_SPI_BREAKOUT

/**
 * @brief   Configuration for the high-speed SPI interface driver of DW1000
 *          module.
 */
extern ROMCONST SPIConfig moduleHalSpiUwbHsConfig;

/**
 * @brief   Configuration for the low-speed SPI interface driver of DW1000
 *          module.
 */
extern ROMCONST SPIConfig moduleHalSpiUwbLsConfig;

#endif /* (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_UWBv10) */

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
 * @brief   RS232_R_EN_N output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioRs232En;

/**
 * @brief   SW_V33_EN output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSwV33En;

// The 4.2V switch is disabled due to a hardware bug.
///**
// * @brief   SW_V42_EN output signal GPIO.
// */
//extern ROMCONST apalControlGpio_t moduleGpioSwV42En;

/**
 * @brief   SW_V50_EN output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSwV50En;

/**
 * @brief   IO_3 breakout signal GPIO.
 */
extern apalControlGpio_t moduleGpioBreakoutIo3;

/**
 * @brief   IO_5 breakout signal GPIO.
 */
extern apalControlGpio_t moduleGpioBreakoutIo5;

/**
 * @brief   IO_6 breakout signal GPIO.
 */
extern apalControlGpio_t moduleGpioBreakoutIo6;

/**
 * @brief   SYS_UART_DN bidirectional signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysUartDn;

/**
 * @brief   IO_7 breakout signal GPIO.
 */
extern apalControlGpio_t moduleGpioBreakoutIo7;

/**
 * @brief   IO_8 breakout signal GPIO.
 */
extern apalControlGpio_t moduleGpioBreakoutIo8;

/**
 * @brief   IO_4 breakout signal GPIO.
 */
extern apalControlGpio_t moduleGpioBreakoutIo4;

/**
 * @brief   IO_1 breakout signal GPIO.
 */
extern apalControlGpio_t moduleGpioBreakoutIo1;

/**
 * @brief   IO_2 breakout signal GPIO.
 */
extern apalControlGpio_t moduleGpioBreakoutIo2;

/**
 * @brief   LED output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioLed;

/**
 * @brief   LIGHT_XLAT output signal.
 */
extern ROMCONST apalControlGpio_t moduleGpioLightXlat;

/**
 * @brief   SW_V18_EN output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSwV18En;

/**
 * @brief   SW_VSYS_EN output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSwVsysEn;

/**
 * @brief   SYS_UART_UP bidirectional signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysUartUp;

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
 * @brief   Event flag to be set on a IO_4 (breakout) interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_BREAKOUTIO4     AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_IO_4))

/**
 * @brief   Event flag to be set on a IO_1 (breakout) interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_BREAKOUTIO1     AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_IO_1))

/**
 * @brief   Event flag to be set on a SYS_SYNC interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_SYSSYNC         AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_SYS_INT_N))

/**
 * @brief   Event flag to be set on a IO_3 (breakout) interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_BREAKOUTIO3     AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_IO_3))

/**
 * @brief   Event flag to be set on a IO_5 (breakout) interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_BREAKOUTIO5     AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_IO_5))

/**
 * @brief   Event flag to be set on a IO_6 (breakout) interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_BREAKOUTIO6     AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_IO_6))

/**
 * @brief   Event flag to be set on a SYS_UART_DN interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_SYSUARTDN       AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_SYS_UART_DN))

/**
 * @brief   Event flag to be set on a SYS_UART_UP interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_SYSUARTUP       AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_SYS_UART_UP))

/**
 * @brief   Event flag to be set on a IO_7 (breakout) interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_BREAKOUTIO7     AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_IO_7))

/**
 * @brief   Event flag to be set on a IO_8 (breakout) interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_BREAKOUTIO8     AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_IO_8))

/**
 * @brief   Event flag to be set on a SYS_PD interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_SYSPD           AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_SYS_PD_N))

#if (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_UWBv10) || defined(__DOXYGEN__)

/**
 * @brief   Event flag to be set on a DW1000 interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_DW1000          MODULE_OS_GPIOEVENTFLAGS_BREAKOUTIO8

#endif /* (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_UWBv10) */

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

#define AMIROOS_CFG_SYSINFO_HOOK() {                                            \
  _aosSysPrintSystemInfoLine(stream, "built for breakout module", "%s",         \
                             (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_NONE) ? "none" :  \
                             (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_ESP) ? "ESP32" : \
                             (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_NICLASENSE) ? "NiclaSense" : \
                             (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_UWBv10) ? "UWB v1.0 (DW1000)" : \
                             "unknown");                                        \
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
#include <alld_INA219.h>
#include <alld_LED.h>
#include <alld_MIC9404x.h>
// TODO: add SNx5C3221E
#include <alld_TLC5947.h>

/**
 * @brief   EEPROM driver.
 */
extern AT24C01BDriver moduleLldEeprom;

/**
 * @brief   Power monitor (VLED 4.2) driver.
 */
extern INA219Driver moduleLldPowerMonitorVled;

/**
 * @brief   Status LED driver.
 */
extern LEDDriver moduleLldStatusLed;

/**
 * @brief   Power switch driver (1.8V).
 */
extern MIC9404xDriver moduleLldPowerSwitchV18;

/**
 * @brief   Power switch driver (3.3V).
 */
extern MIC9404xDriver moduleLldPowerSwitchV33;

/**
 * @brief   Power switch driver (4.2V).
 */
extern MIC9404xDriver moduleLldPowerSwitchV42;

/**
 * @brief   Power switch driver (5.0V).
 */
extern MIC9404xDriver moduleLldPowerSwitchV50;

/**
 * @brief   Pseudo power switch driver (VSYS).
 * @details There is no actual MIC9040x device, but the swicthable circuit
 *          behaves analogous.
 */
extern MIC9404xDriver moduleLldPowerSwitchVsys;

// TODO: add SNx5C3221E

/**
 * @brief   24 channel PWM LED driver.
 */
extern TLC5947Driver moduleLldLedPwm;

#if (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_UWBv10) || defined(__DOXYGEN__)

#include <alld_DW1000.h>

/**
 * @brief   Alias for the DW1000 driver object.
 * @note    The dw1000 struct is defined as external variable (singleton) by the
 *          driver, since the Decawave software stacks assumes no more than a
 *          single device in a system.
 */
#define moduleLldDw1000                         dw1000

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

/**
 * @brief   AT24C01BN-SH-B (EEPROM) test command.
 */
extern aos_shellcommand_t moduleTestAt24c01bShellCmd;

/**
 * @brief   INA219 (power monitor) test command.
 */
extern aos_shellcommand_t moduleTestIna219ShellCmd;

/**
 * @brief   Status LED test command.
 */
extern aos_shellcommand_t moduleTestLedShellCmd;

/**
 * @brief   MIC9404x (power switch) test command.
 */
extern aos_shellcommand_t moduleTestMic9404xShellCmd;

// TODO: add SNx5C3221E

/**
 * @brief   TLC5947 (24 channel PWM LED driver) test command.
 */
extern aos_shellcommand_t moduleTestTlc5947ShellCmd;

#if (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_UWBv10) || defined(__DOXYGEN__)

/**
 * @brief   DW1000 (UWB transmitter) test command.
 */
extern aos_shellcommand_t moduleTestDw1000ShellCmd;

#endif /* (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_UWBv10) */

#endif /* (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/**
 * @brief   Entire module test command.
 */
extern aos_shellcommand_t moduleTestAllShellCmd;

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */

/** @} */

#endif /* AMIROOS_MODULE_H */

/** @} */
