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
 * @brief   Structures and constant for the NUCLEO-L476RG module.
 *
 * @addtogroup nucleol476rg
 * @{
 */

#ifndef AMIROOS_MODULE_H
#define AMIROOS_MODULE_H

#include <amiroos.h>

#if defined(BOARD_MPU6050_CONNECTED)
#include <math.h>
#endif /* defined(BOARD_MPU6050_CONNECTED) */

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

/**
 * @brief   Default I2C interface for the NUCLEO I/O.
 */
#define MODULE_HAL_I2C                          I2CD1

/**
 * @brief   Configuration for the I2C driver #1.
 */
extern I2CConfig moduleHalI2cConfig;

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
 * @brief   User button input signal.
 */
extern ROMCONST apalControlGpio_t moduleGpioUserButton;

#if defined(BOARD_VL6180X_CONNECTED) || defined(__DOXYGEN__)
/**
 * @brief   VL6180X chip enable signal.
 */
extern ROMCONST apalControlGpio_t moduleGpioVl6180xEnable;
#endif /* defined(BOARD_VL6180X_CONNECTED) */

#if defined(BOARD_VL53L1X_CONNECTED)
/**
 * @brief   VL53L1X interrupt signal (GPIO1).
 */
extern ROMCONST apalControlGpio_t moduleGpioVl53l1xINT;

/**
 * @brief   VL53L1X XSHUT signal.
 */
extern ROMCONST apalControlGpio_t moduleGpioVl53l1xXSHUT;
#endif /* defined(BOARD_VL53L1X_CONNECTED) */

/** @} */

/*===========================================================================*/
/**
 * @name AMiRo-OS core configurations
 * @{
 */
/*===========================================================================*/

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
  if (eventflags & MODULE_OS_GPIOEVENTFLAG_USERBUTTON) {                        \
    button_lld_state_t buttonstate;                                             \
    button_lld_get(&moduleLldUserButton, &buttonstate);                         \
    led_lld_set(&moduleLldLed, (buttonstate == BUTTON_LLD_STATE_PRESSED) ? LED_LLD_STATE_ON : LED_LLD_STATE_OFF); \
  }                                                                             \
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

#if defined(BOARD_MPU6050_CONNECTED) || defined(__DOXYGEN__)
#include <alld_MPU6050.h>

/**
 * @brief   Accelerometer (MPU6050) driver.
 */
extern MPU6050Driver moduleLldMpu6050;
#endif /* defined(BOARD_MPU6050_CONNECTED) */

#if defined(BOARD_VL6180X_CONNECTED) || defined(__DOXYGEN__)
#include <alld_VL6180X.h>

/**
 * @brief   Proximity sensor (VL6180X) driver.
 */
extern VL6180XDriver moduleLldVl6180x;
#endif /* defined(BOARD_VL6180X_CONNECTED) */

#if defined(BOARD_VL53L1X_CONNECTED) || defined(__DOXYGEN__)
#include <alld_VL53L1X.h>

/**
 * @brief   ToF sensor (VL53L1X) driver.
 */
extern VL53L1XDriver moduleLldVl53l1x;
#endif /* defined(BOARD_VL53L1X_CONNECTED) */

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

#if defined(BOARD_MPU6050_CONNECTED) || defined(__DOXYGEN__)
/**
 * @brief   MPU6050 (Accelerometer & Gyroscope) test command.
 */
extern aos_shellcommand_t moduleTestMpu6050ShellCmd;
#endif /* defined(BOARD_MPU6050_CONNECTED) */

#if defined(BOARD_VL6180X_CONNECTED) || defined(__DOXYGEN__)
/**
 * @brief   VL6180X (Proximity & ALS) test command.
 */
extern aos_shellcommand_t moduleTestVl6180xShellCmd;
#endif /* defined(BOARD_VL6180X_CONNECTED) */

#if defined(BOARD_VL53L1X_CONNECTED) || defined(__DOXYGEN__)
/**
 * @brief   VL53L1X (ToF sonsor) test command.
 */
extern aos_shellcommand_t moduleTestVL53L1XShellCmd;
#endif /* defined(BOARD_VL53L1X_CONNECTED) */

#endif /* (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/**
 * @brief   Entire module test command.
 */
extern aos_shellcommand_t moduleTestAllShellCmd;

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */

/** @} */

#endif /* AMIROOS_MODULE_H */

/** @} */
