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
 * @brief   LightRing v1.0 Board specific macros.
 *
 * @defgroup lightring10_cfg_board Board
 * @ingroup lightring10_cfg
 *
 * @brief   Todo
 * @details Todo
 *
 * @addtogroup lightring10_cfg_board
 * @{
 */

#ifndef BOARD_H
#define BOARD_H

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*
 * Setup for AMiRo LightRing v1.0 board.
 */

/*
 * Board identifier.
 */
#define BOARD_LIGHTRING_1_0
#define BOARD_NAME              "AMiRo LightRing v1.0"

/*
 * Board oscillators-related settings.
 * NOTE: LSE not fitted.
 */
#if !defined(STM32_LSECLK)
#define STM32_LSECLK                0U
#endif

#if !defined(STM32_HSECLK)
#define STM32_HSECLK                8000000U
#endif

/*
 * Board voltages.
 * Required for performance limits calculation.
 */
#define STM32_VDD                   330U

/*
 * MCU type as defined in the ST header.
 */
#define STM32F103xE

/*
 * IO pins assignments.
 */
#define GPIOA_PIN0                  0U
#define GPIOA_PIN1                  1U
#define GPIOA_LASER_RX              2U
#define GPIOA_LASER_TX              3U
#define GPIOA_LIGHT_BLANK           4U
#define GPIOA_LIGHT_SCLK            5U
#define GPIOA_PIN6                  6U
#define GPIOA_LIGHT_MOSI            7U
#define GPIOA_PIN8                  8U
#define GPIOA_PROG_RX               9U
#define GPIOA_PROG_TX               10U
#define GPIOA_CAN_RX                11U
#define GPIOA_CAN_TX                12U
#define GPIOA_SWDIO                 13U
#define GPIOA_SWCLK                 14U
#define GPIOA_PIN15                 15U

#define GPIOB_PIN0                  0U
#define GPIOB_PIN1                  1U
#define GPIOB_LASER_EN              2U
#define GPIOB_PIN3                  3U
#define GPIOB_PIN4                  4U
#define GPIOB_LASER_OC_N            5U
#define GPIOB_SYS_UART_DN           6U
#define GPIOB_PIN7                  7U
#define GPIOB_WL_GDO2               8U
#define GPIOB_WL_GDO0               9U
#define GPIOB_MEM_SCL               10U
#define GPIOB_MEM_SDA               11U
#define GPIOB_WL_SS_N               12U
#define GPIOB_WL_SCLK               13U
#define GPIOB_WL_MISO               14U
#define GPIOB_WL_MOSI               15U

#define GPIOC_PIN0                  0U
#define GPIOC_PIN1                  1U
#define GPIOC_PIN2                  2U
#define GPIOC_PIN3                  3U
#define GPIOC_LIGHT_XLAT            4U
#define GPIOC_PIN5                  5U
#define GPIOC_PIN6                  6U
#define GPIOC_PIN7                  7U
#define GPIOC_PIN8                  8U
#define GPIOC_PIN9                  9U
#define GPIOC_SYS_UART_RX           10U
#define GPIOC_SYS_UART_TX           11U
#define GPIOC_PIN12                 12U
#define GPIOC_PIN13                 13U
#define GPIOC_SYS_PD_N              14U
#define GPIOC_PIN15                 15U

#define GPIOD_OSC_IN                0U
#define GPIOD_OSC_OUT               1U
#define GPIOD_SYS_INT_N             2U
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

#define GPIOG_PIN0                  0U
#define GPIOG_PIN1                  1U
#define GPIOG_PIN2                  2U
#define GPIOG_PIN3                  3U
#define GPIOG_PIN4                  4U
#define GPIOG_PIN5                  5U
#define GPIOG_PIN6                  6U
#define GPIOG_PIN7                  7U
#define GPIOG_PIN8                  8U
#define GPIOG_PIN9                  9U
#define GPIOG_PIN10                 10U
#define GPIOG_PIN11                 11U
#define GPIOG_PIN12                 12U
#define GPIOG_PIN13                 13U
#define GPIOG_PIN14                 14U
#define GPIOG_PIN15                 15U

/*
 * IO lines assignments.
 */
#define LINE_LASER_RX               PAL_LINE(GPIOA, GPIOA_LASER_RX)
#define LINE_LASER_TX               PAL_LINE(GPIOA, GPIOA_LASER_TX)
#define LINE_LIGHT_BLANK            PAL_LINE(GPIOA, GPIOA_LIGHT_BLANK)
#define LINE_LIGHT_SCLK             PAL_LINE(GPIOA, GPIOA_LIGHT_SCLK)
#define LINE_LIGHT_MOSI             PAL_LINE(GPIOA, GPIOA_LIGHT_MOSI)
#define LINE_PROG_RX                PAL_LINE(GPIOA, GPIOA_PROG_RX)
#define LINE_PROG_TX                PAL_LINE(GPIOA, GPIOA_PROG_TX)
#define LINE_CAN_RX                 PAL_LINE(GPIOA, GPIOA_CAN_RX)
#define LINE_CAN_TX                 PAL_LINE(GPIOA, GPIOA_CAN_TX)
#define LINE_SWDIO                  PAL_LINE(GPIOA, GPIOA_SWDIO)
#define LINE_SWCLK                  PAL_LINE(GPIOA, GPIOA_SWCLK)

#define LINE_LASER_EN               PAL_LINE(GPIOB, GPIOB_LASER_EN)
#define LINE_LASER_OC_N             PAL_LINE(GPIOB, GPIOB_LASER_OC_N)
#define LINE_SYS_UART_DN            PAL_LINE(GPIOB, GPIOB_SYS_UART_DN)
#define LINE_WL_GDO2                PAL_LINE(GPIOB, GPIOB_WL_GDO2)
#define LINE_WL_GDO0                PAL_LINE(GPIOB, GPIOB_WL_GDO0)
#define LINE_MEM_SCL                PAL_LINE(GPIOB, GPIOB_MEM_SCL)
#define LINE_MEM_SDA                PAL_LINE(GPIOB, GPIOB_MEM_SDA)
#define LINE_WL_SS_N                PAL_LINE(GPIOB, GPIOB_WL_SS_N)
#define LINE_WL_SCLK                PAL_LINE(GPIOB, GPIOB_WL_SCLK)
#define LINE_WL_MISO                PAL_LINE(GPIOB, GPIOB_WL_MISO)
#define LINE_WL_MOSI                PAL_LINE(GPIOB, GPIOB_WL_MOSI)

#define LINE_LIGHT_XLAT             PAL_LINE(GPIOC, GPIOC_LIGHT_XLAT)
#define LINE_SYS_UART_RX            PAL_LINE(GPIOC, GPIOC_SYS_UART_RX)
#define LINE_SYS_UART_TX            PAL_LINE(GPIOC, GPIOC_SYS_UART_TX)
#define LINE_SYS_PD_N               PAL_LINE(GPIOC, GPIOC_SYS_PD_N)

#define LINE_SYS_INT_N              PAL_LINE(GPIOD, GPIOD_SYS_INT_N)

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
 * Please refer to the STM32 Reference Manual for details.
 */
#define PIN_MODE_INPUT              0U
#define PIN_MODE_OUTPUT_2M          2U
#define PIN_MODE_OUTPUT_10M         1U
#define PIN_MODE_OUTPUT_50M         3U
#define PIN_CNF_INPUT_ANALOG        0U
#define PIN_CNF_INPUT_FLOATING      1U
#define PIN_CNF_INPUT_PULLX         2U
#define PIN_CNF_OUTPUT_PUSHPULL     0U
#define PIN_CNF_OUTPUT_OPENDRAIN    1U
#define PIN_CNF_ALTERNATE_PUSHPULL  2U
#define PIN_CNF_ALTERNATE_OPENDRAIN 3U
#define PIN_CR(pin, mode, cnf)      (((mode) | ((cnf) << 2U)) << (((pin) % 8U) * 4U))
#define PIN_ODR_LOW(n)              (0U << (n))
#define PIN_ODR_HIGH(n)             (1U << (n))
#define PIN_IGNORE(n)               (1U << (n))

/*
 * GPIOA setup:
 *
 * PA0  - PIN0                      (input floating)
 * PA1  - PIN1                      (input floating)
 * PA2  - LASER_RX                  (alternate pushpull high 50MHz)
 * PA3  - LASER_TX                  (input pullup)
 * PA4  - LIGHT_BLANK               (output pushpull high 50MHz)
 * PA5  - LIGHT_SCLK                (alternate pushpull 50MHz)
 * PA6  - PIN6                      (input foating)
 * PA7  - LIGHT_MOSI                (alternate pushpull 50MHz)
 * PA8  - PIN8                      (input floating)
 * PA9  - PROG_RX                   (alternate pushpull 50MHz)
 * PA10 - PROG_TX                   (input pullup)
 * PA11 - CAN_RX                    (input floating)
 * PA12 - CAN_TX                    (alternate pushpull 50MHz)
 * PA13 - SWDIO                     (input pullup)
 * PA14 - SWCLK                     (input pullup)
 * PA15 - PIN15                     (input floating)
 */
#define VAL_GPIOAIGN                0
#define VAL_GPIOACRL                (PIN_CR(GPIOA_PIN0, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOA_PIN1, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOA_LASER_RX, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL) |   \
                                     PIN_CR(GPIOA_LASER_TX, PIN_MODE_INPUT, PIN_CNF_INPUT_PULLX) |               \
                                     PIN_CR(GPIOA_LIGHT_BLANK, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_PUSHPULL) |   \
                                     PIN_CR(GPIOA_LIGHT_SCLK, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL) | \
                                     PIN_CR(GPIOA_PIN6, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOA_LIGHT_MOSI, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL))
#define VAL_GPIOACRH                (PIN_CR(GPIOA_PIN8, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOA_PROG_RX, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL) |    \
                                     PIN_CR(GPIOA_PROG_TX, PIN_MODE_INPUT, PIN_CNF_INPUT_PULLX) |                \
                                     PIN_CR(GPIOA_CAN_RX, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |              \
                                     PIN_CR(GPIOA_CAN_TX, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL) |     \
                                     PIN_CR(GPIOA_SWDIO, PIN_MODE_INPUT, PIN_CNF_INPUT_PULLX) |                  \
                                     PIN_CR(GPIOA_SWCLK, PIN_MODE_INPUT, PIN_CNF_INPUT_PULLX) |                  \
                                     PIN_CR(GPIOA_PIN15, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOAODR                (PIN_ODR_LOW(GPIOA_PIN0) |                                                   \
                                     PIN_ODR_LOW(GPIOA_PIN1) |                                                   \
                                     PIN_ODR_HIGH(GPIOA_LASER_RX) |                                              \
                                     PIN_ODR_HIGH(GPIOA_LASER_TX) |                                              \
                                     PIN_ODR_HIGH(GPIOA_LIGHT_BLANK) |                                           \
                                     PIN_ODR_HIGH(GPIOA_LIGHT_SCLK) |                                            \
                                     PIN_ODR_LOW(GPIOA_PIN6) |                                                   \
                                     PIN_ODR_HIGH(GPIOA_LIGHT_MOSI) |                                            \
                                     PIN_ODR_LOW(GPIOA_PIN8) |                                                   \
                                     PIN_ODR_HIGH(GPIOA_PROG_RX) |                                               \
                                     PIN_ODR_HIGH(GPIOA_PROG_TX) |                                               \
                                     PIN_ODR_HIGH(GPIOA_CAN_RX) |                                                \
                                     PIN_ODR_HIGH(GPIOA_CAN_TX) |                                                \
                                     PIN_ODR_HIGH(GPIOA_SWDIO) |                                                 \
                                     PIN_ODR_HIGH(GPIOA_SWCLK) |                                                 \
                                     PIN_ODR_LOW(GPIOA_PIN15))

/*
 * GPIOB setup:
 *
 * PB0  - PIN0                      (input floating)
 * PB1  - PIN1                      (input floating)
 * PB2  - LASER_EN                  (output pushpull low 50MHz)
 * PB3  - PIN3                      (input floating)
 * PB4  - PIN4                      (input floating)
 * PB5  - LASER_OC_N                (input floating)
 * PB6  - SYS_UART_DN               (output opendrain high 50MHz)
 * PB7  - PIN7                      (input floating)
 * PB8  - WL_GDO2                   (input pullup)
 * PB9  - WL_GDO0                   (input pullup)
 * PB10 - MEM_SCL                   (alternate opendrain 50MHz)
 * PB11 - MEM_SDA                   (alternate opendrain 50MHz)
 * PB12 - WL_SS_N                   (output pushpull high 50MHz)
 * PB13 - WL_SCLK                   (alternate pushpull 50MHz)
 * PB14 - WL_MISO                   (input pullup)
 * PB15 - WL_MOSI                   (alternate pushpull 50MHz)
 */
#define VAL_GPIOBIGN                (PIN_IGNORE(GPIOB_SYS_UART_DN)) & 0
#define VAL_GPIOBCRL                (PIN_CR(GPIOB_PIN0, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOB_PIN1, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOB_LASER_EN, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_PUSHPULL) |      \
                                     PIN_CR(GPIOB_PIN3, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOB_PIN4, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOB_LASER_OC_N, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |          \
                                     PIN_CR(GPIOB_SYS_UART_DN, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_OPENDRAIN) |  \
                                     PIN_CR(GPIOB_PIN7, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOBCRH                (PIN_CR(GPIOB_WL_GDO2, PIN_MODE_INPUT, PIN_CNF_INPUT_PULLX) |                \
                                     PIN_CR(GPIOB_WL_GDO0, PIN_MODE_INPUT, PIN_CNF_INPUT_PULLX) |                \
                                     PIN_CR(GPIOB_MEM_SCL, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_OPENDRAIN) |   \
                                     PIN_CR(GPIOB_MEM_SDA, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_OPENDRAIN) |   \
                                     PIN_CR(GPIOB_WL_SS_N, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_PUSHPULL) |       \
                                     PIN_CR(GPIOB_WL_SCLK, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL) |    \
                                     PIN_CR(GPIOB_WL_MISO, PIN_MODE_INPUT, PIN_CNF_INPUT_PULLX) |                \
                                     PIN_CR(GPIOB_WL_MOSI, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL))
#define VAL_GPIOBODR                (PIN_ODR_LOW(GPIOB_PIN0) |                                                   \
                                     PIN_ODR_LOW(GPIOB_PIN1) |                                                   \
                                     PIN_ODR_LOW(GPIOB_LASER_EN) |                                               \
                                     PIN_ODR_LOW(GPIOB_PIN3) |                                                   \
                                     PIN_ODR_LOW(GPIOB_PIN4) |                                                   \
                                     PIN_ODR_HIGH(GPIOB_LASER_OC_N) |                                            \
                                     PIN_ODR_HIGH(GPIOB_SYS_UART_DN) |                                           \
                                     PIN_ODR_LOW(GPIOB_PIN7) |                                                   \
                                     PIN_ODR_HIGH(GPIOB_WL_GDO2) |                                               \
                                     PIN_ODR_HIGH(GPIOB_WL_GDO0) |                                               \
                                     PIN_ODR_HIGH(GPIOB_MEM_SCL) |                                               \
                                     PIN_ODR_HIGH(GPIOB_MEM_SDA) |                                               \
                                     PIN_ODR_HIGH(GPIOB_WL_SS_N) |                                               \
                                     PIN_ODR_HIGH(GPIOB_WL_SCLK) |                                               \
                                     PIN_ODR_HIGH(GPIOB_WL_MISO) |                                               \
                                     PIN_ODR_HIGH(GPIOB_WL_MOSI))

/*
 * GPIOC setup:
 *
 * PC0  - PIN0                      (input floating)
 * PC1  - PIN1                      (input floating)
 * PC2  - PIN2                      (input floating)
 * PC3  - PIN3                      (input floating)
 * PC4  - LIGHT_XLAT                (output pushpull low 50MHz)
 * PC5  - PIN5                      (input floating)
 * PC6  - PIN6                      (input floating)
 * PC7  - PIN7                      (input floating)
 * PC8  - PIN8                      (input floating)
 * PC9  - PIN9                      (input floating)
 * PC10 - SYS_UART_RX               (input pullup)
 * PC11 - SYS_UART_TX               (input pullup)
 * PC12 - PIN12                     (input floating)
 * PC13 - PIN13                     (input floating)
 * PC14 - SYS_PD_N                  (output opendrain high 50MHz)
 * PC15 - PIN15                     (input floating)
 */
#define VAL_GPIOCIGN                (PIN_IGNORE(GPIOC_SYS_PD_N)) & 0
#define VAL_GPIOCCRL                (PIN_CR(GPIOC_PIN0, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOC_PIN1, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOC_PIN2, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOC_PIN3, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOC_LIGHT_XLAT, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_PUSHPULL) |    \
                                     PIN_CR(GPIOC_PIN5, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOC_PIN6, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOC_PIN7, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOCCRH                (PIN_CR(GPIOC_PIN8, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOC_PIN9, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOC_SYS_UART_RX, PIN_MODE_INPUT, PIN_CNF_INPUT_PULLX) |            \
                                     PIN_CR(GPIOC_SYS_UART_TX, PIN_MODE_INPUT, PIN_CNF_INPUT_PULLX) |            \
                                     PIN_CR(GPIOC_PIN12, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |               \
                                     PIN_CR(GPIOC_PIN13, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |               \
                                     PIN_CR(GPIOC_SYS_PD_N, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_OPENDRAIN) |     \
                                     PIN_CR(GPIOC_PIN15, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOCODR                (PIN_ODR_LOW(GPIOC_PIN0) |                                                   \
                                     PIN_ODR_LOW(GPIOC_PIN1) |                                                   \
                                     PIN_ODR_LOW(GPIOC_PIN2) |                                                   \
                                     PIN_ODR_LOW(GPIOC_PIN3) |                                                   \
                                     PIN_ODR_LOW(GPIOC_LIGHT_XLAT) |                                             \
                                     PIN_ODR_LOW(GPIOC_PIN5) |                                                   \
                                     PIN_ODR_LOW(GPIOC_PIN6) |                                                   \
                                     PIN_ODR_LOW(GPIOC_PIN7) |                                                   \
                                     PIN_ODR_LOW(GPIOC_PIN8) |                                                   \
                                     PIN_ODR_LOW(GPIOC_PIN9) |                                                   \
                                     PIN_ODR_HIGH(GPIOC_SYS_UART_RX) |                                           \
                                     PIN_ODR_HIGH(GPIOC_SYS_UART_TX) |                                           \
                                     PIN_ODR_LOW(GPIOC_PIN12) |                                                  \
                                     PIN_ODR_LOW(GPIOC_PIN13) |                                                  \
                                     PIN_ODR_HIGH(GPIOC_SYS_PD_N) |                                              \
                                     PIN_ODR_LOW(GPIOC_PIN15))

/*
 * GPIOD setup:
 *
 * PD0  - OSC_IN                    (input floating)
 * PD1  - OSC_OUT                   (input floating)
 * PD2  - SYS_INT_N                 (output opendrain low 50MHz)
 * PD3  - PIN3                      (input floating)
 * PD4  - PIN4                      (input floating)
 * PD5  - PIN5                      (input floating)
 * PD6  - PIN6                      (input floating)
 * PD7  - PIN7                      (input floating)
 * PD8  - PIN8                      (input floating)
 * PD9  - PIN9                      (input floating)
 * PD10 - PIN10                     (input floating)
 * PD11 - PIN11                     (input floating)
 * PD12 - PIN12                     (input floating)
 * PD13 - PIN13                     (input floating)
 * PD14 - PIN14                     (input floating)
 * PD15 - PIN15                     (input floating)
 */
#define VAL_GPIODIGN                (PIN_IGNORE(GPIOD_SYS_INT_N)) & 0
#define VAL_GPIODCRL                (PIN_CR(GPIOD_OSC_IN, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |              \
                                     PIN_CR(GPIOD_OSC_OUT, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |             \
                                     PIN_CR(GPIOD_SYS_INT_N, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_OPENDRAIN) |    \
                                     PIN_CR(GPIOD_PIN3, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOD_PIN4, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOD_PIN5, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOD_PIN6, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOD_PIN7, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIODCRH                (PIN_CR(GPIOD_PIN8, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOD_PIN9, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOD_PIN10, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |               \
                                     PIN_CR(GPIOD_PIN11, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |               \
                                     PIN_CR(GPIOD_PIN12, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |               \
                                     PIN_CR(GPIOD_PIN13, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |               \
                                     PIN_CR(GPIOD_PIN14, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |               \
                                     PIN_CR(GPIOD_PIN15, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIODODR                (PIN_ODR_HIGH(GPIOD_OSC_IN) |                                                \
                                     PIN_ODR_HIGH(GPIOD_OSC_OUT) |                                               \
                                     PIN_ODR_LOW(GPIOD_SYS_INT_N) |                                             \
                                     PIN_ODR_LOW(GPIOD_PIN3) |                                                   \
                                     PIN_ODR_LOW(GPIOD_PIN4) |                                                   \
                                     PIN_ODR_LOW(GPIOD_PIN5) |                                                   \
                                     PIN_ODR_LOW(GPIOD_PIN6) |                                                   \
                                     PIN_ODR_LOW(GPIOD_PIN7) |                                                   \
                                     PIN_ODR_LOW(GPIOD_PIN8) |                                                   \
                                     PIN_ODR_LOW(GPIOD_PIN9) |                                                   \
                                     PIN_ODR_LOW(GPIOD_PIN10) |                                                  \
                                     PIN_ODR_LOW(GPIOD_PIN11) |                                                  \
                                     PIN_ODR_LOW(GPIOD_PIN12) |                                                  \
                                     PIN_ODR_LOW(GPIOD_PIN13) |                                                  \
                                     PIN_ODR_LOW(GPIOD_PIN14) |                                                  \
                                     PIN_ODR_LOW(GPIOD_PIN15))

/*
 * GPIOE setup:
 *
 * PE0  - PIN0                      (input floating)
 * PE1  - PIN1                      (input floating)
 * PE2  - PIN2                      (input floating)
 * PE3  - PIN3                      (input floating)
 * PE4  - PIN4                      (input floating)
 * PE5  - PIN5                      (input floating)
 * PE6  - PIN6                      (input floating)
 * PE7  - PIN7                      (input floating)
 * PE8  - PIN8                      (input floating)
 * PE9  - PIN9                      (input floating)
 * PE10 - PIN10                     (input floating)
 * PE11 - PIN11                     (input floating)
 * PE12 - PIN12                     (input floating)
 * PE13 - PIN13                     (input floating)
 * PE14 - PIN14                     (input floating)
 * PE15 - PIN15                     (input floating)
 */
#define VAL_GPIOEIGN                0
#define VAL_GPIOECRL                (PIN_CR(GPIOE_PIN0, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOE_PIN1, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOE_PIN2, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOE_PIN3, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOE_PIN4, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOE_PIN5, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOE_PIN6, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOE_PIN7, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOECRH                (PIN_CR(GPIOE_PIN8, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOE_PIN9, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOE_PIN10, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                 \
                                     PIN_CR(GPIOE_PIN11, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                 \
                                     PIN_CR(GPIOE_PIN12, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                 \
                                     PIN_CR(GPIOE_PIN13, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                 \
                                     PIN_CR(GPIOE_PIN14, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                 \
                                     PIN_CR(GPIOE_PIN15, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOEODR                (PIN_ODR_LOW(GPIOE_PIN0) |                                                     \
                                     PIN_ODR_LOW(GPIOE_PIN1) |                                                     \
                                     PIN_ODR_LOW(GPIOE_PIN2) |                                                     \
                                     PIN_ODR_LOW(GPIOE_PIN3) |                                                     \
                                     PIN_ODR_LOW(GPIOE_PIN4) |                                                     \
                                     PIN_ODR_LOW(GPIOE_PIN5) |                                                     \
                                     PIN_ODR_LOW(GPIOE_PIN6) |                                                     \
                                     PIN_ODR_LOW(GPIOE_PIN7) |                                                     \
                                     PIN_ODR_LOW(GPIOE_PIN8) |                                                     \
                                     PIN_ODR_LOW(GPIOE_PIN9) |                                                     \
                                     PIN_ODR_LOW(GPIOE_PIN10) |                                                    \
                                     PIN_ODR_LOW(GPIOE_PIN11) |                                                    \
                                     PIN_ODR_LOW(GPIOE_PIN12) |                                                    \
                                     PIN_ODR_LOW(GPIOE_PIN13) |                                                    \
                                     PIN_ODR_LOW(GPIOE_PIN14) |                                                    \
                                     PIN_ODR_LOW(GPIOE_PIN15))

/*
 * GPIOF setup:
 *
 * PF0  - PIN0                      (input floating)
 * PF1  - PIN1                      (input floating)
 * PF2  - PIN2                      (input floating)
 * PF3  - PIN3                      (input floating)
 * PF4  - PIN4                      (input floating)
 * PF5  - PIN5                      (input floating)
 * PF6  - PIN6                      (input floating)
 * PF7  - PIN7                      (input floating)
 * PF8  - PIN8                      (input floating)
 * PF9  - PIN9                      (input floating)
 * PF10 - PIN10                     (input floating)
 * PF11 - PIN11                     (input floating)
 * PF12 - PIN12                     (input floating)
 * PF13 - PIN13                     (input floating)
 * PF14 - PIN14                     (input floating)
 * PF15 - PIN15                     (input floating)
 */
#define VAL_GPIOFIGN                0
#define VAL_GPIOFCRL                (PIN_CR(GPIOF_PIN0, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOF_PIN1, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOF_PIN2, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOF_PIN3, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOF_PIN4, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOF_PIN5, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOF_PIN6, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOF_PIN7, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOFCRH                (PIN_CR(GPIOF_PIN8, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOF_PIN9, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOF_PIN10, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                 \
                                     PIN_CR(GPIOF_PIN11, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                 \
                                     PIN_CR(GPIOF_PIN12, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                 \
                                     PIN_CR(GPIOF_PIN13, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                 \
                                     PIN_CR(GPIOF_PIN14, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                 \
                                     PIN_CR(GPIOF_PIN15, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOFODR                (PIN_ODR_LOW(GPIOF_PIN0) |                                                     \
                                     PIN_ODR_LOW(GPIOF_PIN1) |                                                     \
                                     PIN_ODR_LOW(GPIOF_PIN2) |                                                     \
                                     PIN_ODR_LOW(GPIOF_PIN3) |                                                     \
                                     PIN_ODR_LOW(GPIOF_PIN4) |                                                     \
                                     PIN_ODR_LOW(GPIOF_PIN5) |                                                     \
                                     PIN_ODR_LOW(GPIOF_PIN6) |                                                     \
                                     PIN_ODR_LOW(GPIOF_PIN7) |                                                     \
                                     PIN_ODR_LOW(GPIOF_PIN8) |                                                     \
                                     PIN_ODR_LOW(GPIOF_PIN9) |                                                     \
                                     PIN_ODR_LOW(GPIOF_PIN10) |                                                    \
                                     PIN_ODR_LOW(GPIOF_PIN11) |                                                    \
                                     PIN_ODR_LOW(GPIOF_PIN12) |                                                    \
                                     PIN_ODR_LOW(GPIOF_PIN13) |                                                    \
                                     PIN_ODR_LOW(GPIOF_PIN14) |                                                    \
                                     PIN_ODR_LOW(GPIOF_PIN15))

/*
 * GPIOG setup:
 *
 * PG0  - PIN0                      (input floating)
 * PG1  - PIN1                      (input floating)
 * PG2  - PIN2                      (input floating)
 * PG3  - PIN3                      (input floating)
 * PG4  - PIN4                      (input floating)
 * PG5  - PIN5                      (input floating)
 * PG6  - PIN6                      (input floating)
 * PG7  - PIN7                      (input floating)
 * PG8  - PIN8                      (input floating)
 * PG9  - PIN9                      (input floating)
 * PG10 - PIN10                     (input floating)
 * PG11 - PIN11                     (input floating)
 * PG12 - PIN12                     (input floating)
 * PG13 - PIN13                     (input floating)
 * PG14 - PIN14                     (input floating)
 * PG15 - PIN15                     (input floating)
 */
#define VAL_GPIOGIGN                0
#define VAL_GPIOGCRL                (PIN_CR(GPIOG_PIN0, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOG_PIN1, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOG_PIN2, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOG_PIN3, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOG_PIN4, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOG_PIN5, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOG_PIN6, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOG_PIN7, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOGCRH                (PIN_CR(GPIOG_PIN8, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOG_PIN9, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOG_PIN10, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                 \
                                     PIN_CR(GPIOG_PIN11, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                 \
                                     PIN_CR(GPIOG_PIN12, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                 \
                                     PIN_CR(GPIOG_PIN13, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                 \
                                     PIN_CR(GPIOG_PIN14, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                 \
                                     PIN_CR(GPIOG_PIN15, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOGODR                (PIN_ODR_LOW(GPIOG_PIN0) |                                                     \
                                     PIN_ODR_LOW(GPIOG_PIN1) |                                                     \
                                     PIN_ODR_LOW(GPIOG_PIN2) |                                                     \
                                     PIN_ODR_LOW(GPIOG_PIN3) |                                                     \
                                     PIN_ODR_LOW(GPIOG_PIN4) |                                                     \
                                     PIN_ODR_LOW(GPIOG_PIN5) |                                                     \
                                     PIN_ODR_LOW(GPIOG_PIN6) |                                                     \
                                     PIN_ODR_LOW(GPIOG_PIN7) |                                                     \
                                     PIN_ODR_LOW(GPIOG_PIN8) |                                                     \
                                     PIN_ODR_LOW(GPIOG_PIN9) |                                                     \
                                     PIN_ODR_LOW(GPIOG_PIN10) |                                                    \
                                     PIN_ODR_LOW(GPIOG_PIN11) |                                                    \
                                     PIN_ODR_LOW(GPIOG_PIN12) |                                                    \
                                     PIN_ODR_LOW(GPIOG_PIN13) |                                                    \
                                     PIN_ODR_LOW(GPIOG_PIN14) |                                                    \
                                     PIN_ODR_LOW(GPIOG_PIN15))

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
