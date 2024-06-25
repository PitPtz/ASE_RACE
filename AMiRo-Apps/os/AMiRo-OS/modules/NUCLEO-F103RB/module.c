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
 * @brief   Structures and constant for the NUCLEO-F103RB module.
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

#if defined(BOARD_DW1000_CONNECTED)
/*! SPI (high and low speed) configuration for DW1000 */
ROMCONST SPIConfig moduleHalSpiUwbHsConfig = {
  /* circular buffer mode        */ false,
  /* callback function pointer   */ NULL,
  /* chip select line port       */ GPIOB,
  /* chip select line pad number */ GPIOB_PIN12,
  /* CR1                         */ 0,
  /* CR2                         */ 0,
};

ROMCONST SPIConfig moduleHalSpiUwbLsConfig = {
  /* circular buffer mode        */ false,
  /* callback function pointer   */ NULL,
  /* chip select line port       */ GPIOB,
  /* chip select line pad number */ GPIOB_PIN12,
  /* CR1                         */ SPI_CR1_BR_1 | SPI_CR1_BR_0,
  /* CR2                         */ 0,
};
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

#if defined(BOARD_DW1000_CONNECTED)
/**
 * @brief   DW1000 reset output signal GPIO.
 */
static apalGpio_t _gpioDw1000Reset = {
  /* line */ LINE_ARD_D15, //PAL_LINE(GPIOA, GPIOA_ARD_A0)
};
ROMCONST apalControlGpio_t moduleGpioDw1000Reset = {
  /* GPIO */ &_gpioDw1000Reset,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};


/**
 * @brief   DW1000 interrrupt input signal GPIO.
 */
static apalGpio_t _gpioDw1000Irqn = {
  /* line */ LINE_ARD_D14,  // PAL_LINE(GPIOB, GPIOB_ARD_D6)
};
ROMCONST apalControlGpio_t moduleGpioDw1000Irqn = {
  /* GPIO */ &_gpioDw1000Irqn,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_RISING,
  },
};


/**
 * @brief   DW1000 SPI chip select output signal GPIO.
 */
static apalGpio_t _gpioSpiChipSelect = {
  /* line */ PAL_LINE(GPIOB, GPIOB_PIN12),
};
ROMCONST apalControlGpio_t moduleGpioSpiChipSelect = {
  /* GPIO */ &_gpioSpiChipSelect,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};
#endif /* defined(BOARD_DW1000_CONNECTED) */

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

#if defined(BOARD_DW1000_CONNECTED)
  palSetLineCallback(moduleGpioDw1000Irqn.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioDw1000Irqn.gpio->line);
  palEnableLineEvent(moduleGpioDw1000Irqn.gpio->line, APAL2CH_EDGE(moduleGpioDw1000Irqn.meta.edge));
#endif /* defined(BOARD_DW1000_CONNECTED) */

  return;
}

/**
 * @brief   Initializes periphery communication interfaces.
 */
void moduleInitPeripheryInterfaces()
{
  // serial driver
  sdStart(&MODULE_HAL_SERIAL, &moduleHalSerialConfig);

  // SPI
#if defined(BOARD_DW1000_CONNECTED)
  dw1000_spi_init();
  spiStart(&MODULE_HAL_SPI_UWB, &moduleHalSpiUwbLsConfig);
#endif /* defined(BOARD_DW1000_CONNECTED) */

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
#if defined(BOARD_DW1000_CONNECTED)
  aosShellAddCommand(&moduleTestDw1000ShellCmd);
#endif /* defined(BOARD_DW1000_CONNECTED) */
#endif /* (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */
  aosShellAddCommand(&moduleTestAllShellCmd);

  return;
}
#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */

/**
 * @brief   Handles GPIO events in the main loop.
 *
 * @param[in] flags   Flags indicating which GPIOs fired events.
 */
void moduleMainLoopGpioEvent(eventflags_t flags)
{
  if (flags & MODULE_OS_GPIOEVENTFLAG_USERBUTTON) {
    button_lld_state_t buttonstate;
    button_lld_get(&moduleLldUserButton, &buttonstate);
    led_lld_set(&moduleLldLed, (buttonstate == BUTTON_LLD_STATE_PRESSED) ? LED_LLD_STATE_ON : LED_LLD_STATE_OFF);
  }
#if defined(BOARD_DW1000_CONNECTED)
  if(flags & MODULE_GPOS_IOEVENTFLAG_DW1000_IRQn) {
    /*apalGpioToggle(moduleGpioLed.gpio); //just for debug*/
    process_deca_irq();
  }
#endif /* defined(BOARD_DW1000_CONNECTED) */
}

/**
 * @brief   Deinitializes periphery communication interfaces.
 */
void moduleShutdownPeripheryInterfaces()
{
  // SPI
#if defined(BOARD_DW1000_CONNECTED)
  spiStop(&MODULE_HAL_SPI_UWB);
#endif /* defined(BOARD_DW1000_CONNECTED) */

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

ROMCONST char* moduleShellPrompt = "NUCLEO-F103RB";

#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */
/** @} */

/*===========================================================================*/
/**
 * @name Hardware specific wrappers Functions
 * @{
 */
/*===========================================================================*/

#if defined(BOARD_DW1000_CONNECTED)

/**
 * @brief TODO: Manual implementation of SPI configuration. Somehow, it is
 *        necessary in NUCLEO-F103RB.
 */
void dw1000_spi_init(void){
  palSetPadMode(GPIOB, GPIOB_PIN13, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
  palSetPadMode(GPIOB, GPIOB_PIN14, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
  palSetPadMode(GPIOB, GPIOB_PIN15, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
  palSetLineMode(moduleGpioSpiChipSelect.gpio->line, PAL_MODE_OUTPUT_PUSHPULL);
  apalGpioWrite(moduleGpioSpiChipSelect.gpio, APAL_GPIO_LOW);
}

/*! @brief Manually reset the DW1000 module  */
void reset_DW1000(void){

  // Set the pin as output
  palSetLineMode(moduleGpioDw1000Reset.gpio->line, APAL_GPIO_DIRECTION_OUTPUT);

  //drive the RSTn pin low
  apalGpioWrite(moduleGpioDw1000Reset.gpio, APAL_GPIO_LOW);

  //put the pin back to tri-state ... as input
//  palSetLineMode(moduleGpioDw1000Reset.gpio->line, APAL_GPIO_DIRECTION_INPUT); // TODO:

  aosThdMSleep(2);
}

/*! @brief entry point to the IRQn event in DW1000 module
 *
 * */
void process_deca_irq(void){
  do{
    dwt_isr();
   //while IRS line active (ARM can only do edge sensitive interrupts)
  }while(port_CheckEXT_IRQ() == 1);
}

/*! @brief Check the current value of GPIO pin and return the value */
apalGpioState_t port_CheckEXT_IRQ(void) {
  apalGpioState_t  val;
  apalGpioRead(moduleGpioDw1000Irqn.gpio, &val);
  return val;
}

/*! @brief Manually set the chip select pin of the SPI */
void set_SPI_chip_select(void){
  apalGpioWrite(moduleGpioSpiChipSelect.gpio, APAL_GPIO_HIGH);
}

/*! @brief Manually reset the chip select pin of the SPI */
void clear_SPI_chip_select(void){
  apalGpioWrite(moduleGpioSpiChipSelect.gpio, APAL_GPIO_LOW);
}

/*! @brief Change the SPI speed configuration on the fly */
void setHighSpeed_SPI(bool speedValue, DW1000Driver* drv){

  spiStop(drv->spid);

  if (speedValue == FALSE){
    // low speed spi configuration
    spiStart(drv->spid, &moduleHalSpiUwbLsConfig);
  } else{
    // high speed spi configuration
    spiStart(drv->spid, &moduleHalSpiUwbHsConfig);
  }
}

#endif /* defined(BOARD_DW1000_CONNECTED) */

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

#if defined(BOARD_DW1000_CONNECTED)
DW1000Driver moduleLldDw1000 = {
  /* SPI driver         */ &MODULE_HAL_SPI_UWB,
  /* ext interrupt      */ &moduleGpioDw1000Irqn,
  /* RESET DW1000       */ &moduleGpioDw1000Reset,
};
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

#if defined(BOARD_DW1000_CONNECTED) || defined(__DOXYGEN__)
/*
 * UwB Driver (DW1000)
 */
#include "test/DW1000/module_test_DW1000.h"
static int _moduleTestDw1000ShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestDw1000ShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestDw1000ShellCmd, "test:DW1000", _moduleTestDw1000ShellCmdCb);
#endif /* defined(BOARD_DW1000_CONNECTED) */

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

#if defined(BOARD_DW1000_CONNECTED)
  /* DW1000 */
  status |= moduleTestDw1000ShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
#endif /* defined(BOARD_DW1000_CONNECTED) */

#endif /* (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

  // print total result
  chprintf(stream, "\n");
  aosTestResultPrintSummary(stream, &result_total, "entire module");

  return status;
}
AOS_SHELL_COMMAND(moduleTestAllShellCmd, "test:all", _moduleTestAllShellCmdCb);

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */

/** @} */
