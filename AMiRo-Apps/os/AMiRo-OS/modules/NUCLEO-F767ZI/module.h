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
 * @brief   Structures and constant for the NUCLEO-F767ZI module.
 *
 * @addtogroup nucleof767zi
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
 * @brief   Serial driver of the programmer interface.
 */
#define MODULE_HAL_SERIAL                       SD3

/**
 * @brief   Configuration for the programmer serial interface driver.
 */
extern ROMCONST SerialConfig moduleHalSerialConfig;

/**
 * @brief   Real-Time Clock driver.
 */
#define MODULE_HAL_RTC                          RTCD1

/** @} */

/*===========================================================================*/
/**
 * @name GPIO definitions
 * @{
 */
/*===========================================================================*/

/**
 * @brief   LED1 output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioLed1;

/**
 * @brief   LED2 output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioLed2;

/**
 * @brief   LED3 output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioLed3;

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

/**
 * @brief   Event flag to be set on a USER_BUTTON interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_USERBUTTON      AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_BUTTON))

/**
 * @brief   Interrupt initialization hook.
 */
#define MODULE_INIT_INTERRUPTS() {                                              \
  /* user button */                                                             \
  palSetLineCallback(moduleGpioUserButton.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioUserButton.gpio->line);  \
  palEnableLineEvent(moduleGpioUserButton.gpio->line, APAL2CH_EDGE(moduleGpioUserButton.meta.edge));                  \
}

/**
 * @brief   Periphery communication interfaces initialization hook.
 */
#define MODULE_INIT_PERIPHERY_INTERFACES() {                                    \
  /* serial driver */                                                           \
  sdStart(&MODULE_HAL_SERIAL, &moduleHalSerialConfig);                          \
}

/**
 * @brief   Test initialization hook.
 */
#define MODULE_INIT_TESTS() {                                                   \
  /* add test commands to shell */                                              \
}

/**
 * @brief   Periphery communication interface deinitialization hook.
 */
#define MODULE_SHUTDOWN_PERIPHERY_INTERFACES() {                                \
  /* serial driver */                                                           \
  /* don't stop the serial driver so messages can still be printed */           \
}

/**
 * @brief   HOOK to toggle the LEDs when the user button is pressed.
 */
#define MODULE_MAIN_LOOP_GPIOEVENT(eventflags) {                                \
  if (eventflags & MODULE_OS_GPIOEVENTFLAG_USERBUTTON) {                        \
    apalControlGpioState_t buttonstate;                                         \
    apalControlGpioGet(&moduleGpioUserButton, &buttonstate);                    \
    apalControlGpioSet(&moduleGpioLed1, buttonstate);                           \
    apalControlGpioSet(&moduleGpioLed2, buttonstate);                           \
    apalControlGpioSet(&moduleGpioLed3, buttonstate);                           \
  }                                                                             \
}

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

/** @} */

/*===========================================================================*/
/**
 * @name Tests
 * @{
 */
/*===========================================================================*/
#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */

/** @} */

#endif /* AMIROOS_MODULE_H */

/** @} */
