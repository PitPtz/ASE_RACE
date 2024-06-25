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
 * @file    board.c
 * @brief   LightRing v1.2 Board specific initializations.
 *
 * @addtogroup lightring12_cfg_board
 * @{
 */

#include <hal.h>

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

#if HAL_USE_PAL || defined(__DOXYGEN__)
/**
 * @brief   PAL setup.
 * @details Digital I/O ports static configuration as defined in @p board.h.
 *          This variable is used by the HAL when initializing the PAL driver.
 */
const PALConfig pal_default_config = {
#if STM32_HAS_GPIOA
  {VAL_GPIOAODR, VAL_GPIOACRL, VAL_GPIOACRH},
#endif
#if STM32_HAS_GPIOB
  {VAL_GPIOBODR, VAL_GPIOBCRL, VAL_GPIOBCRH},
#endif
#if STM32_HAS_GPIOC
  {VAL_GPIOCODR, VAL_GPIOCCRL, VAL_GPIOCCRH},
#endif
#if STM32_HAS_GPIOD
  {VAL_GPIODODR, VAL_GPIODCRL, VAL_GPIODCRH},
#endif
#if STM32_HAS_GPIOE
  {VAL_GPIOEODR, VAL_GPIOECRL, VAL_GPIOECRH},
#endif
#if STM32_HAS_GPIOF
  {VAL_GPIOFODR, VAL_GPIOFCRL, VAL_GPIOFCRH},
#endif
#if STM32_HAS_GPIOG
  {VAL_GPIOGODR, VAL_GPIOGCRL, VAL_GPIOGCRH},
#endif
};
#endif

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Early initialization code.
 * @details This initialization must be performed just after stack setup
 *          and before any other initialization.
 */
void __early_init(void) {

  stm32_clock_init();
}

/**
 * @brief   Board-specific initialization code.
 * @todo    Add your board-specific code, if any.
 */
void boardInit(void) {
  /*
   * Several I/O pins are re-mapped:
   *   JTAG disabled and SWD enabled
   */
  AFIO->MAPR = AFIO_MAPR_SWJ_CFG_JTAGDISABLE |
               AFIO_MAPR_USART3_REMAP_PARTIALREMAP;
}

/** @} */
