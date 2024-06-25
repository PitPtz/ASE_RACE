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
 * @brief   Structures and constant for the NUCLEO-L476RG module.
 */

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

ROMCONST SerialConfig moduleHalSerialConfig = {
  /* bit rate */ 115200,
  /* CR1      */ 0,
  /* CR1      */ 0,
  /* CR1      */ 0,
};

I2CConfig moduleHalI2cConfig = {
  /* timing reg */ 0, // configured later in MODULE_INIT_PERIPHERY_IF hook
  /* CR1        */ 0,
  /* CR2        */ 0,
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
  /* line */ LINE_LED_GREEN,
};
ROMCONST apalControlGpio_t moduleGpioLed = {
  /* GPIO */ &_gpioLed,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   User button input signal GPIO.
 */
static apalGpio_t _gpioUserButton = {
  /* line */ LINE_BUTTON,
};
ROMCONST apalControlGpio_t moduleGpioUserButton = {
  /* GPIO */ &_gpioUserButton,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

#if defined(BOARD_VL6180X_CONNECTED) || defined(__DOXYGEN__)
/**
 * @brief   VL6190X enable signal GPIO.
 */
static apalGpio_t _gpioVl6180xEnable = {
  .line = LINE_ARD_A1,
};
ROMCONST apalControlGpio_t moduleGpioVl6180xEnable = {
  .gpio = &_gpioVl6180xEnable,
  .meta = {
    .direction = APAL_GPIO_DIRECTION_OUTPUT,
    .active = APAL_GPIO_ACTIVE_HIGH,
    .edge = APAL_GPIO_EDGE_NONE,
  },
};
#endif /* defined(BOARD_VL6180X_CONNECTED) */

#if defined(BOARD_VL53L1X_CONNECTED) || defined(__DOXYGEN__)
/**
 * @brief   VL53L1X interrupt signal GPIO.
 */
static apalGpio_t _gpioVl53l1xINT = {
  /* line */ LINE_ARD_A2,
};
ROMCONST apalControlGpio_t moduleGpioVl53l1xINT = {
  /* GPIO */ &_gpioVl53l1xINT,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_FALLING,
  },
};

/**
 * @brief   VL53L1X XSHUT signal GPIO.
 */
static apalGpio_t _gpioVl53l1xXSHUT = {
  /* line */ LINE_ARD_A3,
};
ROMCONST apalControlGpio_t moduleGpioVl53l1xXSHUT = {
  /* GPIO */ &_gpioVl53l1xXSHUT,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};
#endif /* defined(BOARD_VL53L1X_CONNECTED) */

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
  // user button
  palSetLineCallback(moduleGpioUserButton.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioUserButton.gpio->line);
  palEnableLineEvent(moduleGpioUserButton.gpio->line, APAL2CH_EDGE(moduleGpioUserButton.meta.edge));

  // VL53L1X
#if defined(BOARD_VL53L1X_CONNECTED)
  palSetLineCallback(moduleGpioVl53l1xINT.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioVl53l1xINT.gpio->line);
    palEnableLineEvent(moduleGpioVl53l1xINT.gpio->line, APAL2CH_EDGE(moduleGpioVl53l1xINT.meta.edge));
#endif	/* defined(BOARD_VL53L1X_CONNECTED) */

  return;
}

#if defined (BOARD_VL53L1X_CONNECTED) || defined(__DOXYGEN__)
/**
 * @brief   Limits I2C frequency to maximum value supported by VL53L1X and configures required GPIOs.
 */
uint32_t _moduleInitPeripheryInterfacesVL53L1X(uint32_t freq)
{
  freq = (VL53L1X_LLD_I2C_MAXFREQUENCY < freq) ? VL53L1X_LLD_I2C_MAXFREQUENCY : freq;
  chSysLock();
  /* set XSHUT GPIO as output in open-drain configuration with internal pull-up, but initially pulled-down signal */
  PAL_PORT(moduleLldVl53l1x.Interface.xshut->gpio->line)->OTYPER &=
    ~(1U << PAL_PAD(moduleLldVl53l1x.Interface.xshut->gpio->line));
  PAL_PORT(moduleLldVl53l1x.Interface.xshut->gpio->line)->OTYPER |=
    PIN_OTYPE_OPENDRAIN(PAL_PAD(moduleLldVl53l1x.Interface.xshut->gpio->line));
  PAL_PORT(moduleLldVl53l1x.Interface.xshut->gpio->line)->PUPDR &=
    ~(3U << (PAL_PAD(moduleLldVl53l1x.Interface.xshut->gpio->line) * 2U));
  PAL_PORT(moduleLldVl53l1x.Interface.xshut->gpio->line)->PUPDR |=
    PIN_PUPDR_PULLUP(PAL_PAD(moduleLldVl53l1x.Interface.xshut->gpio->line));
  PAL_PORT(moduleLldVl53l1x.Interface.xshut->gpio->line)->ODR &=
    ~(1U << PAL_PAD(moduleLldVl53l1x.Interface.xshut->gpio->line));
  PAL_PORT(moduleLldVl53l1x.Interface.xshut->gpio->line)->ODR |=
    PIN_ODR_LOW(PAL_PAD(moduleLldVl53l1x.Interface.xshut->gpio->line));
  PAL_PORT(moduleLldVl53l1x.Interface.xshut->gpio->line)->MODER &=
    ~(3U << (PAL_PAD(moduleLldVl53l1x.Interface.xshut->gpio->line) * 2U));
  PAL_PORT(moduleLldVl53l1x.Interface.xshut->gpio->line)->MODER |=
    PIN_MODE_OUTPUT(PAL_PAD(moduleLldVl53l1x.Interface.xshut->gpio->line));
  /* set GPIO1 (interrupt) GPIO as input with internal pull-up */
  PAL_PORT(moduleLldVl53l1x.Interface.gpio1->gpio->line)->PUPDR &=
    ~(3U << (PAL_PAD(moduleLldVl53l1x.Interface.gpio1->gpio->line) * 2U));
  PAL_PORT(moduleLldVl53l1x.Interface.gpio1->gpio->line)->PUPDR |=
    PIN_PUPDR_PULLUP(PAL_PAD(moduleLldVl53l1x.Interface.gpio1->gpio->line));
  PAL_PORT(moduleLldVl53l1x.Interface.gpio1->gpio->line)->MODER &=
    ~(3U << (PAL_PAD(moduleLldVl53l1x.Interface.gpio1->gpio->line) * 2U));
  PAL_PORT(moduleLldVl53l1x.Interface.gpio1->gpio->line)->MODER |=
    PIN_MODE_INPUT(PAL_PAD(moduleLldVl53l1x.Interface.gpio1->gpio->line));
  chSysUnlock();

  return freq;
}
#endif /* defined (BOARD_VL53L1X_CONNECTED) */

/**
 * @brief   Initializes periphery communication interfaces.
 */
void moduleInitPeripheryInterfaces()
{
  // serial driver
  sdStart(&MODULE_HAL_SERIAL, &moduleHalSerialConfig);

  // I2C
  uint32_t i2c_freq = 1000000; /* maximum I2C frequency supported for this MCU */
#if defined(BOARD_MPU6050_CONNECTED)
  i2c_freq = (MPU6050_LLD_I2C_MAXFREQUENCY < i2c_freq) ? MPU6050_LLD_I2C_MAXFREQUENCY : i2c_freq;
#endif /* defined(BOARD_MPU6050_CONNECTED) */
#if defined(BOARD_VL6180X_CONNECTED)
  i2c_freq = (VL6180X_LLD_I2C_MAXFREQUENCY < i2c_freq) ? VL6180X_LLD_I2C_MAXFREQUENCY : i2c_freq;
#endif /* defined(BOARD_VL6180X_CONNECTED) */
#if defined (BOARD_VL53L1X_CONNECTED)
  i2c_freq = _moduleInitPeripheryInterfacesVL53L1X(i2c_freq);
#endif /* defined (BOARD_VL53L1X_CONNECTED) */
  if (i2c_freq == 100000) /* standard-mode @ 100 kHz */ {
    moduleHalI2cConfig.timingr = 0x10909CEC; /* obtained via STM32CubeMX with STM32_I2CxCLK = 80 MHz */
  } else if (i2c_freq == 400000) /* fast-mode @ 400 kHz */ {
    moduleHalI2cConfig.timingr = 0x00702991; /* obtained via STM32CubeMX with STM32_I2CxCLK = 80 MHz */
  } else if (i2c_freq == 1000000) /* fast-mode-plus @ 10000 kHz */ {
    moduleHalI2cConfig.timingr = 0x00300F33; /* obtained via STM32CubeMX with STM32_I2CxCLK = 80 MHz */
  } else {
    aosDbgAssertMsg(false, "I2C frequency not supported");
  }
  i2cStart(&MODULE_HAL_I2C, &moduleHalI2cConfig);

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
  aosShellAddCommand(&moduleTestLedShellCmd);
  aosShellAddCommand(&moduleTestButtonShellCmd);
#if defined(BOARD_MPU6050_CONNECTED)
  aosShellAddCommand(&moduleTestMpu6050ShellCmd);
#endif /* defined(BOARD_MPU6050_CONNECTED) */
#if defined(BOARD_VL6180X_CONNECTED)
  aosShellAddCommand(&moduleTestVl6180xShellCmd);
#endif /* defined(BOARD_VL6180X_CONNECTED) */
#if defined(BOARD_VL53L1X_CONNECTED)
  aosShellAddCommand(&moduleTestVL53L1XShellCmd);
#endif /* defined(BOARD_VL53L1X_CONNECTED) */
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
  // I2C
  i2cStop(&MODULE_HAL_I2C);

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

/** @} */

/*===========================================================================*/
/**
 * @name AMiRo-OS shell configurations
 * @{
 */
/*===========================================================================*/
#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)

ROMCONST char* moduleShellPrompt = "NUCLEO-L476RG";

#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */
/** @} */

/*===========================================================================*/
/**
 * @name Low-level drivers
 * @{
 */
/*===========================================================================*/

LEDDriver moduleLldLed = {
  /* LED enable Gpio */ &moduleGpioLed,
};

ButtonDriver moduleLldUserButton = {
  /* Button Gpio  */ &moduleGpioUserButton,
};

#if defined(BOARD_MPU6050_CONNECTED)
MPU6050Driver moduleLldMpu6050 = {
  /* I2C Driver       */ &MODULE_HAL_I2C,
  /* I²C address      */ MPU6050_LLD_I2C_ADDR_FIXED,
};
#endif /* defined(BOARD_MPU6050_CONNECTED) */

#if defined(BOARD_VL6180X_CONNECTED) || defined(__DOXYGEN__)
VL6180XDriver moduleLldVl6180x = {
  .i2cd = &MODULE_HAL_I2C,
  .addr = VL6180X_LLD_I2C_DEFAULTADDRESS,
  .enable = &moduleGpioVl6180xEnable,
};
#endif /* defined(BOARD_VL6180X_CONNECTED) */

#if defined(BOARD_VL53L1X_CONNECTED)
VL53L1XDriver moduleLldVl53l1x = {
  /* device data    */ {},
  /* interface data */ {
    /* I2C Driver     */ &MODULE_HAL_I2C,
    /* I²C address    */ VL53L1X_LLD_I2C_ADDR_DEFAULT,
    /* GPIO 1:INT pin */ &moduleGpioVl53l1xINT,
    /* Xshut          */ &moduleGpioVl53l1xXSHUT,
  },
};
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

/*
 * LED
 */
#include "test/LED/module_test_LED.h"
static int _moduleTestLedShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestLedShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestLedShellCmd, "test:LED", _moduleTestLedShellCmdCb);

/*
 * User button
 */
#include "test/button/module_test_button.h"
static int _moduleTestButtonShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestButtonShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestButtonShellCmd, "test:button", _moduleTestButtonShellCmdCb);

#if defined(BOARD_MPU6050_CONNECTED) || defined(__DOXYGEN__)
/*
 * MPU6050 (accelerometer & gyroscope)
 */
#include "test/MPU6050/module_test_MPU6050.h"
static int _moduleTestMpu6050ShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestMpu6050ShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestMpu6050ShellCmd, "test:IMU", _moduleTestMpu6050ShellCmdCb);
#endif /* defined(BOARD_MPU6050_CONNECTED) */

#if defined(BOARD_VL6180X_CONNECTED) || defined(__DOXYGEN__)
/*
 * VL6180X (proximity & ambient light sensor)
 */
#include "test/VL6180X/module_test_VL6180X.h"
static int _moduleTestVl6180xShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestVl6180xShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestVl6180xShellCmd, "test:Proximity", _moduleTestVl6180xShellCmdCb);
#endif /* defined(BOARD_VL6180X_CONNECTED) */

#if defined(BOARD_VL53L1X_CONNECTED) || defined(__DOXYGEN__)
/*
 * VL53L1X (ToF distance sensor with configurable RoI)
 */
#include "test/VL53L1X/module_test_VL53L1X.h"
static int _moduleTestVL53L1XShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestVL53L1XShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestVL53L1XShellCmd, "test:VL53L1X", _moduleTestVL53L1XShellCmdCb);
#endif /* defined(BOARD_VL53L1X_CONNECTED) */

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

  /* LED */
  status |= moduleTestLedShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

  /* User button */
  status |= moduleTestButtonShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

#if defined(BOARD_MPU6050_CONNECTED)
  /* MPU6050 (accelerometer & gyroscope) */
  status |= moduleTestMpu6050ShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
#endif /* defined(BOARD_MPU6050_CONNECTED) */

#if defined(BOARD_VL6180X_CONNECTED)
  /* VL6180X (proximity & ambient light) */
  status |= moduleTestVl6180xShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
#endif /* defined(BOARD_VL6180X_CONNECTED) */

#if defined(BOARD_VL53L1X_CONNECTED)
  /* VL53L1X (ToF sensor) */
  status |= moduleTestVL53L1XShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
#endif /* defined(BOARD_VL53L1X_CONNECTED) */

#endif /* (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

  // print total result
  chprintf(stream, "\n");
  aosTestResultPrintSummary(stream, &result_total, "entire module");

  return status;
}
AOS_SHELL_COMMAND(moduleTestAllShellCmd, "test:all", _moduleTestAllShellCmdCb);

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */

/** @} */
