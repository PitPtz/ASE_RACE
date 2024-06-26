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
 * @file    board.h
 * @brief   Board support header.
 *
 * @defgroup nucleof103rb_cfg_board Board
 * @ingroup nucleof103rb_cfg
 *
 * @brief   Todo
 * @details Todo
 *
 * @addtogroup nucleof103rb_cfg_board
 * @{
 */

#ifndef BOARD_H
#define BOARD_H

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*
 * Setup for the ST NUCLEO64-F103RB board.
 */

/*
 * Board identifier.
 */
#define BOARD_ST_NUCLEO64_F103RB
#define BOARD_NAME              "STMicroelectronics NUCLEO-F103RB"

/*
 * Board frequencies.
 */
#define STM32_LSECLK            0

#if defined(NUCLEO_EXTERNAL_OSCILLATOR)
#define STM32_HSECLK            8000000
#define STM32_HSE_BYPASS

#elif defined(NUCLEO_HSE_CRYSTAL)
#define STM32_HSECLK            8000000

#else
#define STM32_HSECLK            0
#endif

/*
 * MCU type, supported types are defined in ./os/hal/platforms/hal_lld.h.
 */
#define STM32F103xB

/*
 * IO pins assignments.
 */
#define GPIOA_ARD_A0                0U
#define GPIOA_ADC1_IN0              0U
#define GPIOA_ARD_A1                1U
#define GPIOA_ADC1_IN1              1U
#define GPIOA_ARD_D1                2U
#define GPIOA_USART2_TX             2U
#define GPIOA_ARD_D0                3U
#define GPIOA_USART2_RX             3U
#define GPIOA_ARD_A2                4U
#define GPIOA_ADC1_IN4              4U
#define GPIOA_ARD_D13               5U
#define GPIOA_LED_GREEN             5U
#define GPIOA_ARD_D12               6U
#define GPIOA_ARD_D11               7U
#define GPIOA_ARD_D7                8U
#define GPIOA_ARD_D8                9U
#define GPIOA_ARD_D2                10U
#define GPIOA_PIN11                 11U
#define GPIOA_PIN12                 12U
#define GPIOA_SWDIO                 13U
#define GPIOA_SWCLK                 14U
#define GPIOA_PIN15                 15U

#define GPIOB_ARD_A3                0U
#define GPIOB_ADC1_IN8              0U
#define GPIOB_PIN1                  1U
#define GPIOB_PIN2                  2U
#define GPIOB_ARD_D3                3U
#define GPIOB_SWO                   3U
#define GPIOB_ARD_D5                4U
#define GPIOB_ARD_D4                5U
#define GPIOB_ARD_D10               6U
#define GPIOB_PIN7                  7U
#define GPIOB_ARD_D15               8U
#define GPIOB_ARD_D14               9U
#define GPIOB_ARD_D6                10U
#define GPIOB_PIN11                 11U
#define GPIOB_PIN12                 12U
#define GPIOB_PIN13                 13U
#define GPIOB_PIN14                 14U
#define GPIOB_PIN15                 15U

#define GPIOC_ARD_A5                0U
#define GPIOC_ADC1_IN11             0U
#define GPIOC_ARD_A4                1U
#define GPIOC_ADC1_IN10             1U
#define GPIOC_PIN2                  2U
#define GPIOC_PIN3                  3U
#define GPIOC_PIN4                  4U
#define GPIOC_PIN5                  5U
#define GPIOC_PIN6                  6U
#define GPIOC_ARD_D9                7U
#define GPIOC_PIN8                  8U
#define GPIOC_PIN9                  9U
#define GPIOC_PIN10                 10U
#define GPIOC_PIN11                 11U
#define GPIOC_PIN12                 12U
#define GPIOC_BUTTON                13U
#define GPIOC_PIN14                 14U
#define GPIOC_PIN15                 15U

#define GPIOD_OSC_IN                0U
#define GPIOD_PIN0                  0U
#define GPIOD_OSC_OUT               1U
#define GPIOD_PIN1                  1U
#define GPIOD_PIN2                  2U
#define GPIOD_PIN3                  3U
#define GPIOD_PIN4                  4U
#define GPIOD_PIN5                  5U
#define GPIOD_PIN6                  6U
#define GPIOD_PIN7                  7U
#define GPIOD_PIN8                  8U
#define GPIOD_PIN9                  9U
#define GPIOD_PIN10                 10U
#define GPIOD_PIN11                 11U
#define GPIOD_PIN12                 12U
#define GPIOD_PIN13                 13U
#define GPIOD_PIN14                 14U
#define GPIOD_PIN15                 15U

#define GPIOE_PIN0                  0U
#define GPIOE_PIN1                  1U
#define GPIOE_PIN2                  2U
#define GPIOE_PIN3                  3U
#define GPIOE_PIN4                  4U
#define GPIOE_PIN5                  5U
#define GPIOE_PIN6                  6U
#define GPIOE_PIN7                  7U
#define GPIOE_PIN8                  8U
#define GPIOE_PIN9                  9U
#define GPIOE_PIN10                 10U
#define GPIOE_PIN11                 11U
#define GPIOE_PIN12                 12U
#define GPIOE_PIN13                 13U
#define GPIOE_PIN14                 14U
#define GPIOE_PIN15                 15U

#define GPIOF_PIN0                  0U
#define GPIOF_PIN1                  1U
#define GPIOF_PIN2                  2U
#define GPIOF_PIN3                  3U
#define GPIOF_PIN4                  4U
#define GPIOF_PIN5                  5U
#define GPIOF_PIN6                  6U
#define GPIOF_PIN7                  7U
#define GPIOF_PIN8                  8U
#define GPIOF_PIN9                  9U
#define GPIOF_PIN10                 10U
#define GPIOF_PIN11                 11U
#define GPIOF_PIN12                 12U
#define GPIOF_PIN13                 13U
#define GPIOF_PIN14                 14U
#define GPIOF_PIN15                 15U

/*
 * IO lines assignments.
 */
#define LINE_ARD_A0                 PAL_LINE(GPIOA, GPIOA_ARD_A0)
#define LINE_ADC1_IN0               PAL_LINE(GPIOA, GPIOA_ADC1_IN0)
#define LINE_ARD_A1                 PAL_LINE(GPIOA, GPIOA_ARD_A1)
#define LINE_ADC1_IN1               PAL_LINE(GPIOA, GPIOA_ADC1_IN1)
#define LINE_ARD_D1                 PAL_LINE(GPIOA, GPIOA_ARD_D1)
#define LINE_USART2_TX              PAL_LINE(GPIOA, GPIOA_USART2_TX)
#define LINE_ARD_D0                 PAL_LINE(GPIOA, GPIOA_ARD_D0)
#define LINE_USART2_RX              PAL_LINE(GPIOA, GPIOA_USART2_RX)
#define LINE_ARD_A2                 PAL_LINE(GPIOA, GPIOA_ARD_A2)
#define LINE_ADC1_IN4               PAL_LINE(GPIOA, GPIOA_ADC1_IN4)
#define LINE_ARD_D13                PAL_LINE(GPIOA, GPIOA_ARD_D13)
#define LINE_LED_GREEN              PAL_LINE(GPIOA, GPIOA_LED_GREEN)
#define LINE_ARD_D12                PAL_LINE(GPIOA, GPIOA_ARD_D12)
#define LINE_ARD_D11                PAL_LINE(GPIOA, GPIOA_ARD_D11)
#define LINE_ARD_D7                 PAL_LINE(GPIOA, GPIOA_ARD_D7)
#define LINE_ARD_D8                 PAL_LINE(GPIOA, GPIOA_ARD_D8)
#define LINE_ARD_D2                 PAL_LINE(GPIOA, GPIOA_ARD_D2)
#define LINE_SWDIO                  PAL_LINE(GPIOA, GPIOA_SWDIO)
#define LINE_SWCLK                  PAL_LINE(GPIOA, GPIOA_SWCLK)

#define LINE_ARD_A3                 PAL_LINE(GPIOB, GPIOB_ARD_A3)
#define LINE_ADC1_IN8               PAL_LINE(GPIOB, GPIOB_ADC1_IN8)
#define LINE_ARD_D3                 PAL_LINE(GPIOB, GPIOB_ARD_D3)
#define LINE_SWO                    PAL_LINE(GPIOB, GPIOB_SWO)
#define LINE_ARD_D5                 PAL_LINE(GPIOB, GPIOB_ARD_D5)
#define LINE_ARD_D4                 PAL_LINE(GPIOB, GPIOB_ARD_D4)
#define LINE_ARD_D10                PAL_LINE(GPIOB, GPIOB_ARD_D10)
#define LINE_ARD_D15                PAL_LINE(GPIOB, GPIOB_ARD_D15)
#define LINE_ARD_D14                PAL_LINE(GPIOB, GPIOB_ARD_D14)
#define LINE_ARD_D6                 PAL_LINE(GPIOB, GPIOB_ARD_D6)

#define LINE_ARD_A5                 PAL_LINE(GPIOC, GPIOC_ARD_A5)
#define LINE_ADC1_IN11              PAL_LINE(GPIOC, GPIOC_ADC1_IN11)
#define LINE_ARD_A4                 PAL_LINE(GPIOC, GPIOC_ARD_A4)
#define LINE_ADC1_IN10              PAL_LINE(GPIOC, GPIOC_ADC1_IN10)
#define LINE_ARD_D9                 PAL_LINE(GPIOC, GPIOC_ARD_D9)
#define LINE_BUTTON                 PAL_LINE(GPIOC, GPIOC_BUTTON)

#define LINE_OSC_IN                 PAL_LINE(GPIOD, GPIOH_OSC_IN)
#define LINE_OSC_OUT                PAL_LINE(GPIOD, GPIOH_OSC_OUT)

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*
 * I/O ports initial setup, this configuration is established soon after reset
 * in the initialization code.
 *
 * The digits have the following meaning:
 *   0 - Analog input.
 *   1 - Push Pull output 10MHz.
 *   2 - Push Pull output 2MHz.
 *   3 - Push Pull output 50MHz.
 *   4 - Digital input.
 *   5 - Open Drain output 10MHz.
 *   6 - Open Drain output 2MHz.
 *   7 - Open Drain output 50MHz.
 *   8 - Digital input with PullUp or PullDown resistor depending on ODR.
 *   9 - Alternate Push Pull output 10MHz.
 *   A - Alternate Push Pull output 2MHz.
 *   B - Alternate Push Pull output 50MHz.
 *   C - Reserved.
 *   D - Alternate Open Drain output 10MHz.
 *   E - Alternate Open Drain output 2MHz.
 *   F - Alternate Open Drain output 50MHz.
 * Please refer to the STM32 Reference Manual for details.
 */

/*
 * Port A setup.
 * Everything input with pull-up except:
 * PA2  - Alternate output          (GPIOA_ARD_D1, GPIOA_USART2_TX).
 * PA3  - Normal input              (GPIOA_ARD_D0, GPIOA_USART2_RX).
 * PA5  - Push Pull output          (GPIOA_LED_GREEN).
 * PA13 - Pull-up input             (GPIOA_SWDIO).
 * PA14 - Pull-down input           (GPIOA_SWCLK).
 */
#define VAL_GPIOACRL            0x88384B88      /*  PA7...PA0 */
#define VAL_GPIOACRH            0x88888888      /* PA15...PA8 */
#define VAL_GPIOAODR            0xFFFFBFDF

/*
 * Port B setup.
 * Everything input with pull-up except:
 * PB3  - Pull-up input             (GPIOA_SWO).
 */
#define VAL_GPIOBCRL            0x88888888      /*  PB7...PB0 */
#define VAL_GPIOBCRH            0x88888888      /* PB15...PB8 */
#define VAL_GPIOBODR            0xFFFFFFFF

/*
 * Port C setup.
 * Everything input with pull-up except:
 * PC13 - Normal input              (GPIOC_BUTTON).
 */
#define VAL_GPIOCCRL            0x88888888      /*  PC7...PC0 */
#define VAL_GPIOCCRH            0x88488888      /* PC15...PC8 */
#define VAL_GPIOCODR            0xFFFFFFFF

/*
 * Port D setup.
 * Everything input with pull-up except:
 * PD0  - Normal input              (GPIOD_OSC_IN).
 * PD1  - Normal input              (GPIOD_OSC_OUT).
 */
#define VAL_GPIODCRL            0x88888844      /*  PD7...PD0 */
#define VAL_GPIODCRH            0x88888888      /* PD15...PD8 */
#define VAL_GPIODODR            0xFFFFFFFF

/*
 * Port E setup.
 * Everything input with pull-up except:
 */
#define VAL_GPIOECRL            0x88888888      /*  PE7...PE0 */
#define VAL_GPIOECRH            0x88888888      /* PE15...PE8 */
#define VAL_GPIOEODR            0xFFFFFFFF

/*
 * USB bus activation macro, required by the USB driver.
 */
#define usb_lld_connect_bus(usbp)

/*
 * USB bus de-activation macro, required by the USB driver.
 */
#define usb_lld_disconnect_bus(usbp)

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* BOARD_H */

/** @} */
