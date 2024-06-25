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
 * @brief   LightRing v1.2 Board specific macros.
 *
 * @defgroup lightring12_cfg_board Board
 * @ingroup lightring12_cfg
 *
 * @brief   Todo
 * @details Todo
 *
 * @addtogroup lightring12_cfg_board
 * @{
 */

#ifndef BOARD_H
#define BOARD_H

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*
 * Setup for AMiRo LightRing v1.2 board.
 */

/*
 * Board identifier.
 */
#define BOARD_LIGHTRING_1_2
#define BOARD_NAME              "AMiRo LightRing v1.2"

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
 * Ignore this part if called from assembly context.
 */
#if !defined(_FROM_ASM_)

/*
 * Identifiers for the several breakout modules, which can be attached to the LightRing v1.2 module.
 */
#define BOARD_BREAKOUT_NONE         -1
#define BOARD_BREAKOUT_ESP          1
#define BOARD_BREAKOUT_UWBv10       2
#define BOARD_BREAKOUT_NICLASENSE   3


/* sanity check */
#if !defined(BOARD_BREAKOUT_MODULE)
# error "BOARD_BREAKOUT_MODULE not defined."
#elif ((BOARD_BREAKOUT_MODULE != BOARD_BREAKOUT_NONE) && \
      (BOARD_BREAKOUT_MODULE != BOARD_BREAKOUT_ESP) && \
      (BOARD_BREAKOUT_MODULE != BOARD_BREAKOUT_NICLASENSE) && \
      (BOARD_BREAKOUT_MODULE != BOARD_BREAKOUT_UWBv10))
# error "BOARD_BREAKOUT_MODULE set to invalid value"
#endif

#endif /* !defined(_FROM_ASM_) */

/*
 * IO pins assignments.
 */
#define GPIOA_USART_CTS             0U
#define GPIOA_USART_RTS             1U
#define GPIOA_USART_RX              2U
#define GPIOA_USART_TX              3U
#define GPIOA_LIGHT_BLANK           4U
#define GPIOA_LIGHT_SCLK            5U
#define GPIOA_LIGHT_MISO            6U
#define GPIOA_LIGHT_MOSI            7U
#define GPIOA_PIN8                  8U
#define GPIOA_PROG_RX               9U
#define GPIOA_PROG_TX               10U
#define GPIOA_CAN_RX                11U
#define GPIOA_CAN_TX                12U
#define GPIOA_SWDIO                 13U
#define GPIOA_SWCLK                 14U
#define GPIOA_RS232_R_EN_N          15U

#define GPIOB_SW_V33_EN             0U
#define GPIOB_SW_V42_EN             1U
#define GPIOB_SW_V50_EN             2U
#define GPIOB_IO_3                  3U
#define GPIOB_IO_5                  4U
#define GPIOB_IO_6                  5U
#define GPIOB_SYS_UART_DN           6U
#define GPIOB_PIN7                  7U
#define GPIOB_IO_7                  8U
#define GPIOB_IO_8                  9U
#define GPIOB_I2C_SCL               10U
#define GPIOB_I2C_SDA               11U
#define GPIOB_SPI_SS_N              12U
#define GPIOB_SPI_SCLK              13U
#define GPIOB_SPI_MISO              14U
#define GPIOB_SPI_MOSI              15U

#define GPIOC_IO_4                  0U
#define GPIOC_IO_1                  1U
#define GPIOC_IO_2                  2U
#define GPIOC_LED                   3U
#define GPIOC_LIGHT_XLAT            4U
#define GPIOC_SW_V18_EN             5U
#define GPIOC_SW_VSYS_EN            6U
#define GPIOC_SYS_UART_UP           7U
#define GPIOC_PIN8                  8U
#define GPIOC_PIN9                  9U
#define GPIOC_SYS_UART_RX           10U
#define GPIOC_SYS_UART_TX           11U
#define GPIOC_RS232_D_OFF_N         12U
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
#define LINE_USART_CTS              PAL_LINE(GPIOA, GPIOA_USART_CTS)
#define LINE_USART_RTS              PAL_LINE(GPIOA, GPIOA_USART_RTS)
#define LINE_USART_RX               PAL_LINE(GPIOA, GPIOA_USART_RX)
#define LINE_USART_TX               PAL_LINE(GPIOA, GPIOA_USART_TX)
#define LINE_LIGHT_BLANK            PAL_LINE(GPIOA, GPIOA_LIGHT_BLANK)
#define LINE_LIGHT_SCLK             PAL_LINE(GPIOA, GPIOA_LIGHT_SCLK)
#define LINE_LIGHT_MISO             PAL_LINE(GPIOA, GPIOA_LIGHT_MISO)
#define LINE_LIGHT_MOSI             PAL_LINE(GPIOA, GPIOA_LIGHT_MOSI)
#define LINE_PROG_RX                PAL_LINE(GPIOA, GPIOA_PROG_RX)
#define LINE_PROG_TX                PAL_LINE(GPIOA, GPIOA_PROG_TX)
#define LINE_CAN_RX                 PAL_LINE(GPIOA, GPIOA_CAN_RX)
#define LINE_CAN_TX                 PAL_LINE(GPIOA, GPIOA_CAN_TX)
#define LINE_SWDIO                  PAL_LINE(GPIOA, GPIOA_SWDIO)
#define LINE_SWCLK                  PAL_LINE(GPIOA, GPIOA_SWCLK)
#define LINE_RS232_R_EN_N           PAL_LINE(GPIOA, GPIOA_RS232_R_EN_N)

#define LINE_SW_V33_EN              PAL_LINE(GPIOB, GPIOB_SW_V33_EN)
#define LINE_SW_V42_EN              PAL_LINE(GPIOB, GPIOB_SW_V42_EN)
#define LINE_SW_V50_EN              PAL_LINE(GPIOB, GPIOB_SW_V50_EN)
#define LINE_IO_3                   PAL_LINE(GPIOB, GPIOB_IO_3)
#define LINE_IO_5                   PAL_LINE(GPIOB, GPIOB_IO_5)
#define LINE_IO_6                   PAL_LINE(GPIOB, GPIOB_IO_6)
#define LINE_SYS_UART_DN            PAL_LINE(GPIOB, GPIOB_SYS_UART_DN)
#define LINE_IO_7                   PAL_LINE(GPIOB, GPIOB_IO_7)
#define LINE_IO_8                   PAL_LINE(GPIOB, GPIOB_IO_8)
#define LINE_I2C_SCL                PAL_LINE(GPIOB, GPIOB_I2C_SCL)
#define LINE_I2C_SDA                PAL_LINE(GPIOB, GPIOB_I2C_SDA)
#define LINE_SPI_SS_N               PAL_LINE(GPIOB, GPIOB_SPI_SS_N)
#define LINE_SPI_SCLK               PAL_LINE(GPIOB, GPIOB_SPI_SCLK)
#define LINE_SPI_MISO               PAL_LINE(GPIOB, GPIOB_SPI_MISO)
#define LINE_SPI_MOSI               PAL_LINE(GPIOB, GPIOB_SPI_MOSI)

#define LINE_IO_4                   PAL_LINE(GPIOC, GPIOC_IO_4)
#define LINE_IO_1                   PAL_LINE(GPIOC, GPIOC_IO_1)
#define LINE_IO_2                   PAL_LINE(GPIOC, GPIOC_IO_2)
#define LINE_LED                    PAL_LINE(GPIOC, GPIOC_LED)
#define LINE_LIGHT_XLAT             PAL_LINE(GPIOC, GPIOC_LIGHT_XLAT)
#define LINE_SW_V18_EN              PAL_LINE(GPIOC, GPIOC_SW_V18_EN)
#define LINE_SW_VSYS_EN             PAL_LINE(GPIOC, GPIOC_SW_VSYS_EN)
#define LINE_SYS_UART_UP            PAL_LINE(GPIOC, GPIOC_SYS_UART_UP)
#define LINE_SYS_UART_RX            PAL_LINE(GPIOC, GPIOC_SYS_UART_RX)
#define LINE_SYS_UART_TX            PAL_LINE(GPIOC, GPIOC_SYS_UART_TX)
#define LINE_RS232_D_OFF_N          PAL_LINE(GPIOC, GPIOC_RS232_D_OFF_N)
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
 * PA0  - USART_CTS                 (alternate pushpull 50MHz)
 * PA1  - USART_RTS                 (alternate pushpull 50MHz)
 * PA2  - USART_RX                  (alternate pushpull 50MHz)
 * PA3  - USART_TX                  (input pullup)
 * PA4  - LIGHT_BLANK               (output pushpull high 50MHz)
 * PA5  - LIGHT_SCLK                (alternate pushpull 50MHz)
 * PA6  - LIGHT_MISO                (input pullup)
 * PA7  - LIGHT_MOSI                (alternate pushpull 50MHz)
 * PA8  - PIN8                      (input floating)
 * PA9  - PROG_RX                   (alternate pushpull 50MHz)
 * PA10 - PROG_TX                   (input pullup)
 * PA11 - CAN_RX                    (input floating)
 * PA12 - CAN_TX                    (alternate pushpull 50MHz)
 * PA13 - SWDIO                     (input pullup)
 * PA14 - SWCLK                     (input pullup)
 * PA15 - RS232_R_EN_N              (output opendrain low 50MHz)
 */
#define VAL_GPIOAIGN                0
#define VAL_GPIOACRL                (PIN_CR(GPIOA_USART_CTS, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL) |  \
                                     PIN_CR(GPIOA_USART_RTS, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL) |  \
                                     PIN_CR(GPIOA_USART_RX, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL) |   \
                                     PIN_CR(GPIOA_USART_TX, PIN_MODE_INPUT, PIN_CNF_INPUT_PULLX) |               \
                                     PIN_CR(GPIOA_LIGHT_BLANK, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_PUSHPULL) |   \
                                     PIN_CR(GPIOA_LIGHT_SCLK, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL) | \
                                     PIN_CR(GPIOA_LIGHT_MISO, PIN_MODE_INPUT, PIN_CNF_INPUT_PULLX) |             \
                                     PIN_CR(GPIOA_LIGHT_MOSI, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL))
#define VAL_GPIOACRH                (PIN_CR(GPIOA_PIN8, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOA_PROG_RX, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL) |    \
                                     PIN_CR(GPIOA_PROG_TX, PIN_MODE_INPUT, PIN_CNF_INPUT_PULLX) |                \
                                     PIN_CR(GPIOA_CAN_RX, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |              \
                                     PIN_CR(GPIOA_CAN_TX, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL) |     \
                                     PIN_CR(GPIOA_SWDIO, PIN_MODE_INPUT, PIN_CNF_INPUT_PULLX) |                  \
                                     PIN_CR(GPIOA_SWCLK, PIN_MODE_INPUT, PIN_CNF_INPUT_PULLX) |                  \
                                     PIN_CR(GPIOA_RS232_R_EN_N, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_OPENDRAIN))
#define VAL_GPIOAODR                (PIN_ODR_LOW(GPIOA_USART_CTS) |                                              \
                                     PIN_ODR_LOW(GPIOA_USART_RTS) |                                              \
                                     PIN_ODR_HIGH(GPIOA_USART_RX) |                                              \
                                     PIN_ODR_HIGH(GPIOA_USART_TX) |                                              \
                                     PIN_ODR_HIGH(GPIOA_LIGHT_BLANK) |                                           \
                                     PIN_ODR_HIGH(GPIOA_LIGHT_SCLK) |                                            \
                                     PIN_ODR_HIGH(GPIOA_LIGHT_MISO) |                                            \
                                     PIN_ODR_HIGH(GPIOA_LIGHT_MOSI) |                                            \
                                     PIN_ODR_LOW(GPIOA_PIN8) |                                                   \
                                     PIN_ODR_HIGH(GPIOA_PROG_RX) |                                               \
                                     PIN_ODR_HIGH(GPIOA_PROG_TX) |                                               \
                                     PIN_ODR_HIGH(GPIOA_CAN_RX) |                                                \
                                     PIN_ODR_HIGH(GPIOA_CAN_TX) |                                                \
                                     PIN_ODR_HIGH(GPIOA_SWDIO) |                                                 \
                                     PIN_ODR_HIGH(GPIOA_SWCLK) |                                                 \
                                     PIN_ODR_LOW(GPIOA_RS232_R_EN_N))

/*
 * GPIOB setup:
 *
 * PB0  - SW_V33_EN                 (output pushpull low 50MHz)
 * PB1  - SW_V42_EN                 (output pushpull low 50MHz)
 * PB2  - SW_V50_EN                 (output pushpull low 50MHz)
 * PB3  - IO_3                      (input floating)
 * PB4  - IO_5                      (input floating)
 * PB5  - IO_6                      (input floating)
 * PB6  - SYS_UART_DN               (output opendrain high 50MHz)
 * PB7  - PIN7                      (input foating)
 * PB8  - IO_7                      (input floating)
 * PB9  - IO_8                      (input floating)
 * PB10 - I2C_SCL                   (alternate opendrain 50MHz)
 * PB11 - I2C_SDA                   (alternate opendrain 50MHz)
 * PB12 - SPI_SS_N                  (output pushpull high 50MHz)
 * PB13 - SPI_SCLK                  (alternate pushpull 50MHz)
 * PB14 - SPI_MISO                  (input pullup)
 * PB15 - SPI_MOSI                  (alternate pushpull 50MHz)
 */
#define VAL_GPIOBIGN                (PIN_IGNORE(GPIOB_SYS_UART_DN)) & 0
#define VAL_GPIOBCRL                (PIN_CR(GPIOB_SW_V33_EN, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_PUSHPULL) |     \
                                     PIN_CR(GPIOB_SW_V42_EN, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_PUSHPULL) |     \
                                     PIN_CR(GPIOB_SW_V50_EN, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_PUSHPULL) |     \
                                     PIN_CR(GPIOB_IO_3, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOB_IO_5, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOB_IO_6, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOB_SYS_UART_DN, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_OPENDRAIN) |  \
                                     PIN_CR(GPIOB_PIN7, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOBCRH                (PIN_CR(GPIOB_IO_7, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOB_IO_8, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOB_I2C_SCL, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_OPENDRAIN) |   \
                                     PIN_CR(GPIOB_I2C_SDA, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_OPENDRAIN) |   \
                                     PIN_CR(GPIOB_SPI_SS_N, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_PUSHPULL) |      \
                                     PIN_CR(GPIOB_SPI_SCLK, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL) |   \
                                     PIN_CR(GPIOB_SPI_MISO, PIN_MODE_INPUT, PIN_CNF_INPUT_PULLX) |               \
                                     PIN_CR(GPIOB_SPI_MOSI, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL))
#define VAL_GPIOBODR                (PIN_ODR_LOW(GPIOB_SW_V33_EN) |                                              \
                                     PIN_ODR_LOW(GPIOB_SW_V42_EN) |                                              \
                                     PIN_ODR_LOW(GPIOB_SW_V50_EN) |                                              \
                                     PIN_ODR_LOW(GPIOB_IO_3) |                                                   \
                                     PIN_ODR_LOW(GPIOB_IO_5) |                                                   \
                                     PIN_ODR_LOW(GPIOB_IO_6) |                                                   \
                                     PIN_ODR_HIGH(GPIOB_SYS_UART_DN) |                                           \
                                     PIN_ODR_LOW(GPIOB_PIN7) |                                                   \
                                     PIN_ODR_LOW(GPIOB_IO_7) |                                                   \
                                     PIN_ODR_LOW(GPIOB_IO_8) |                                                   \
                                     PIN_ODR_HIGH(GPIOB_I2C_SCL) |                                               \
                                     PIN_ODR_HIGH(GPIOB_I2C_SDA) |                                               \
                                     PIN_ODR_HIGH(GPIOB_SPI_SS_N) |                                              \
                                     PIN_ODR_HIGH(GPIOB_SPI_SCLK) |                                              \
                                     PIN_ODR_HIGH(GPIOB_SPI_MISO) |                                              \
                                     PIN_ODR_HIGH(GPIOB_SPI_MOSI))

/*
 * GPIOC setup:
 *
 * PC0  - IO_4                      (input floating)
 * PC1  - IO_1                      (input floating)
 * PC2  - IO_2                      (input floating)
 * PC3  - LED                       (output opendrain high 50MHz)
 * PC4  - LIGHT_XLAT                (output pushpull low 50MHz)
 * PC5  - SW_V18_EN                 (output pushpull low 50MHz)
 * PC6  - SW_VSYS_EN                (output pushpull low 50MHz)
 * PC7  - SYS_UART_UP               (output opendrain high 50MHz)
 * PC8  - PIN8                      (input floating)
 * PC9  - PIN9                      (input floating)
 * PC10 - SYS_UART_RX               (input pullup)
 * PC11 - SYS_UART_TX               (input pullup)
 * PC12 - RS232_D_OFF_N             (output puspull low 50MHz)
 * PC13 - PIN13                     (input floating)
 * PC14 - SYS_PD_N                  (output opendrain high 50MHz)
 * PC15 - PIN15                     (input floating)
 */
#define VAL_GPIOCIGN                (PIN_IGNORE(GPIOC_SYS_PD_N)) & 0
#define VAL_GPIOCCRL                (PIN_CR(GPIOC_IO_4, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOC_IO_1, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOC_IO_2, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOC_LED, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_OPENDRAIN) |          \
                                     PIN_CR(GPIOC_LIGHT_XLAT, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_PUSHPULL) |    \
                                     PIN_CR(GPIOC_SW_V18_EN, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_PUSHPULL) |     \
                                     PIN_CR(GPIOC_SW_VSYS_EN, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_PUSHPULL) |    \
                                     PIN_CR(GPIOC_SYS_UART_UP, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_OPENDRAIN))
#define VAL_GPIOCCRH                (PIN_CR(GPIOC_PIN8, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOC_PIN9, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                \
                                     PIN_CR(GPIOC_SYS_UART_RX, PIN_MODE_INPUT, PIN_CNF_INPUT_PULLX) |            \
                                     PIN_CR(GPIOC_SYS_UART_TX, PIN_MODE_INPUT, PIN_CNF_INPUT_PULLX) |            \
                                     PIN_CR(GPIOC_RS232_D_OFF_N, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_PUSHPULL) | \
                                     PIN_CR(GPIOC_PIN13, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |               \
                                     PIN_CR(GPIOC_SYS_PD_N, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_OPENDRAIN) |     \
                                     PIN_CR(GPIOC_PIN15, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOCODR                (PIN_ODR_LOW(GPIOC_IO_4) |                                                   \
                                     PIN_ODR_LOW(GPIOC_IO_1) |                                                   \
                                     PIN_ODR_LOW(GPIOC_IO_2) |                                                   \
                                     PIN_ODR_HIGH(GPIOC_LED) |                                                   \
                                     PIN_ODR_LOW(GPIOC_LIGHT_XLAT) |                                             \
                                     PIN_ODR_LOW(GPIOC_SW_V18_EN) |                                              \
                                     PIN_ODR_LOW(GPIOC_SW_VSYS_EN) |                                             \
                                     PIN_ODR_HIGH(GPIOC_SYS_UART_UP) |                                           \
                                     PIN_ODR_LOW(GPIOC_PIN8) |                                                   \
                                     PIN_ODR_LOW(GPIOC_PIN9) |                                                   \
                                     PIN_ODR_HIGH(GPIOC_SYS_UART_RX) |                                           \
                                     PIN_ODR_HIGH(GPIOC_SYS_UART_TX) |                                           \
                                     PIN_ODR_LOW(GPIOC_RS232_D_OFF_N) |                                          \
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
