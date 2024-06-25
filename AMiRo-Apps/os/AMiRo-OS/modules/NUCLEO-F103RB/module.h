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
 * @brief   Structures and constant for the NUCLEO-F103RB module.
 *
 * @addtogroup nucleof103rb
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

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

#if defined(BOARD_DW1000_CONNECTED)
void dw1000_spi_init(void);
void process_deca_irq(void);
apalGpioState_t port_CheckEXT_IRQ(void) ;
#endif /* defined(BOARD_DW1000_CONNECTED) */

void set_SPI_chip_select(void);
void clear_SPI_chip_select(void);
void reset_DW1000(void);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/** @} */

/*===========================================================================*/
/**
 * @name ChibiOS/HAL configuration
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Serial driver of the programmer interface.
 */
#define MODULE_HAL_SERIAL                       SD2

/**
 * @brief   Configuration for the programmer serial interface driver.
 */
extern ROMCONST SerialConfig moduleHalSerialConfig;

/**
 * @brief   Real-Time Clock driver.
 */
#define MODULE_HAL_RTC                          RTCD1

#if defined(BOARD_DW1000_CONNECTED)
/**
 * @brief   SPI interface driver for the motion sensors (gyroscope and
 *          accelerometer).
 */
#define MODULE_HAL_SPI_UWB                      SPID2

/**
 * @brief   Configuration for the SPI interface driver to communicate with the
 *          LED driver.
 */
extern ROMCONST SPIConfig moduleHalSpiUwbHsConfig;

/**
 * @brief   Configuration for the SPI interface driver to communicate with the
 *          wireless transceiver.
 */
extern ROMCONST SPIConfig moduleHalSpiUwbLsConfig;
#endif /* defined(BOARD_DW1000_CONNECTED) */

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

//#if (BOARD_DW1000_CONNECTED == true)
/**
 * @brief   DW1000 reset output signal
 * @note    the reset pin should be drived as low by MCU to activate.
 *          Then, put back the reset pin as input to MCU (tri-state float on the
 *          air is not supported in AMiRo)
 */
extern ROMCONST apalControlGpio_t moduleGpioDw1000Reset;

/**
 * @brief   DW1000 interrupt IRQn input signal.
 */
extern ROMCONST apalControlGpio_t moduleGpioDw1000Irqn;

/**
 * @brief   DW1000 SPI chip select  output signal.
 */
extern ROMCONST apalControlGpio_t moduleGpioSpiChipSelect ;
//#endif /* (BOARD_DW1000_CONNECTED == true) */

/**
 * @brief   User button input signal.
 */
extern ROMCONST apalControlGpio_t moduleGpioUserButton;

/** @} */

/*===========================================================================*/
/**
 * @name AMiRo-OS core configurations
 * @{
 */
/*===========================================================================*/

#if defined(BOARD_DW1000_CONNECTED) || defined(__DOXYGEN__)
/**
 * @brief   Event flag to be call dwt_isr() interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_DW1000_IRQn     AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_ARD_D14))
#endif /* defined(BOARD_DW1000_CONNECTED) */

/**
 * @brief   Event flag to be set on a USER_BUTTON interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_USERBUTTON      AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_BUTTON))

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

/**
 * @brief   HOOK to toggle the LEDs when the user button is pressed.
 */
#define MODULE_MAIN_LOOP_GPIOEVENT(eventflags) {                                \
  void moduleMainLoopGpioEvent(eventflags_t flags);                             \
}

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
void moduleInitInterrupts(void);
void moduleInitPeripheryInterfaces(void);
#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)
void moduleInitTests(void);
#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */
void moduleMainLoopGpioEvent(eventflags_t flags);
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
  #error "SSSP is not supported on this module."
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

#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */
/** @} */

/*===========================================================================*/
/**
 * @name Low-level drivers
 * @{
 */
/*===========================================================================*/
#include <alld_LED.h>
#include <alld_button.h>

/**
 * @brief   LED driver.
 */
extern LEDDriver moduleLldLed;

/**
 * @brief   Button driver.
 */
extern ButtonDriver moduleLldUserButton;


#if defined(BOARD_DW1000_CONNECTED)
/**
 * @brief   DW1000 driver.
 */
#include <alld_DW1000.h>
void setHighSpeed_SPI(bool speedValue, DW1000Driver* drv);

extern DW1000Driver moduleLldDw1000;
#endif /* defined(BOARD_DW1000_CONNECTED) */

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
 * @brief   LED test command.
 */
extern aos_shellcommand_t moduleTestLedShellCmd;

/**
 * @brief   User button test command.
 */
extern aos_shellcommand_t moduleTestButtonShellCmd;

#if defined(BOARD_DW1000_CONNECTED)
/**
 * @brief   DW1000 (UWB transmitter) test command.
 */
extern aos_shellcommand_t moduleTestDw1000ShellCmd;
#endif /* defined(BOARD_DW1000_CONNECTED) */

#endif /* (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/**
 * @brief   Entire module test command.
 */
extern aos_shellcommand_t moduleTestAllShellCmd;

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */

/** @} */

#endif /* AMIROOS_MODULE_H */

/** @} */
