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
 * @brief   PowerManagement v1.2 Board specific macros.
 *
 * @defgroup powermanagement12_cfg_board Board
 * @ingroup powermanagement12_cfg
 *
 * @brief   Todo
 * @details Todo
 *
 * @addtogroup powermanagement12_cfg_board
 * @{
 */

#ifndef BOARD_H
#define BOARD_H

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*
 * Setup for AMiRo PowerManagement v1.2 board.
 */

/*
 * Board identifier.
 */
#define BOARD_POWERMANAGEMENT_1_2
#define BOARD_NAME                      "AMiRo PowerManagement v1.2"

/*
 * Board oscillators-related settings.
 * NOTE: LSE not fitted.
 */
#if !defined(STM32_LSECLK)
#define STM32_LSECLK                    0U
#endif

#if !defined(STM32_HSECLK)
#define STM32_HSECLK                    8000000U
#endif

/*
 * Board voltages.
 * Required for performance limits calculation.
 */
#define STM32_VDD                       330U

/*
 * MCU type as defined in the ST header.
 */
#define STM32F405xx

/*
 * STM32F4 alternate function definitions
 */
#define STM32F4xx_AF_system             0U
#define STM32F4xx_AF_TIM1to2            1U
#define STM32F4xx_AF_TIM3to5            2U
#define STM32F4xx_AF_TIM8to11           3U
#define STM32F4xx_AF_I2C1to3            4U
#define STM32F4xx_AF_SPI1to2            5U
#define STM32F4xx_AF_SPI3               6U
#define STM32F4xx_AF_USART1to3          7U
#define STM32F4xx_AF_USART4to6          8U
#define STM32F4xx_AF_CAN1to2_TIM12to14  9U
#define STM32F4xx_AF_OTG_HSFS           10U
#define STM32F4xx_AF_ETH                11U
#define STM32F4xx_AF_FSMC_SDIO_OTGHS    12U
#define STM32F4xx_AF_DCMI               13U
#define STM32F4xx_AF_EVENTOUT           15U

/*
 * Ignore this part if called from assembly context.
 */
#if !defined(_FROM_ASM_)

/*
 * Identifiers for the several breakout boards, which can be attached to the PowerManagement v1.2 module.
 */
#define BOARD_BREAKOUT_NONE             -1
#define BOARD_BREAKOUT_ESP               1

/* sanity check */
#if !defined(BOARD_BREAKOUT_MODULE)
# error "BOARD_BREAKOUT_MODULE not defined."
#elif ((BOARD_BREAKOUT_MODULE != BOARD_BREAKOUT_NONE) && \
       (BOARD_BREAKOUT_MODULE != BOARD_BREAKOUT_ESP))
# error "BOARD_BREAKOUT_MODULE set to invalid value"
#endif

/*
 * Identifiers for the several sensor rings, which can be attached to the PowerManagement v1.2 module.
 */
#define BOARD_NOSENSORRING              -1
#define BOARD_PROXIMITYSENSOR           1
#define BOARD_DISTANCESENSOR_VL53L0X    2
#define BOARD_DISTANCESENSOR_VL53L1X    3

/* sanity check */
#if !defined(BOARD_SENSORRING)
# error "BOARD_SENSORRING not defined."
#elif ((BOARD_SENSORRING != BOARD_NOSENSORRING) && \
       (BOARD_SENSORRING != BOARD_PROXIMITYSENSOR) && \
       (BOARD_SENSORRING != BOARD_DISTANCESENSOR_VL53L0X) && \
       (BOARD_SENSORRING != BOARD_DISTANCESENSOR_VL53L1X))
# error "BOARD_SENSORRING set to invalid value"
#endif

#endif /* !defined(_FROM_ASM_) */

/*
 * IO pins assignments.
 */
#define GPIOA_WKUP                      0U
#define GPIOA_SWITCH_STATUS_N           1U
#define GPIOA_SYS_UART_TX               2U
#define GPIOA_SYS_UART_RX               3U
#define GPIOA_SYS_SPI_SS0_N             4U
#define GPIOA_SYS_SPI_SCLK              5U
#define GPIOA_SYS_SPI_MISO              6U
#define GPIOA_SYS_SPI_MOSI              7U
#define GPIOA_SYS_REG_EN                8U
#define GPIOA_PROG_RX                   9U
#define GPIOA_PROG_TX                   10U
#define GPIOA_CAN_RX                    11U
#define GPIOA_CAN_TX                    12U
#define GPIOA_SWDIO                     13U
#define GPIOA_SWCLK                     14U
#define GPIOA_SYS_SPI_SS1_N             15U

#define GPIOB_IR_INT1_N                 0U
#define GPIOB_VSYS_SENSE                1U
#define GPIOB_POWER_EN                  2U
#define GPIOB_SYS_UART_DN               3U
#define GPIOB_CHARGE_STAT2A             4U
#define GPIOB_BUZZER                    5U
#define GPIOB_GAUGE_BATLOW2             6U
#define GPIOB_GAUGE_BATGD2_N            7U
#define GPIOB_GAUGE_SCL2                8U
#define GPIOB_GAUGE_SDA2                9U
#define GPIOB_GAUGE_SCL1                10U
#define GPIOB_GAUGE_SDA1                11U
#define GPIOB_LED                       12U
#define GPIOB_BT_RTS                    13U
#define GPIOB_BT_CTS                    14U
#define GPIOB_SYS_UART_UP               15U

#define GPIOC_CHARGE_STAT1A             0U
#define GPIOC_GAUGE_BATLOW1             1U
#define GPIOC_GAUGE_BATGD1_N            2U
#define GPIOC_CHARGE_EN1_N              3U
#define GPIOC_IR_INT2_N                 4U
#define GPIOC_TOUCH_INT_N               5U
#define GPIOC_SYS_DONE                  6U
#define GPIOC_SYS_PROG_N                7U
#define GPIOC_PATH_DC                   8U
#define GPIOC_SYS_SPI_DIR               9U
#define GPIOC_BT_RX                     10U
#define GPIOC_BT_TX                     11U
#define GPIOC_SYS_INT_N                 12U
#define GPIOC_SYS_PD_N                  13U
#define GPIOC_SYS_WARMRST_N             14U
#define GPIOC_BT_RST                    15U

#define GPIOD_PIN0                      0U
#define GPIOD_PIN1                      1U
#define GPIOD_CHARGE_EN2_N              2U
#define GPIOD_PIN3                      3U
#define GPIOD_PIN4                      4U
#define GPIOD_PIN5                      5U
#define GPIOD_PIN6                      6U
#define GPIOD_PIN7                      7U
#define GPIOD_PIN8                      8U
#define GPIOD_PIN9                      9U
#define GPIOD_PIN10                     10U
#define GPIOD_PIN11                     11U
#define GPIOD_PIN12                     12U
#define GPIOD_PIN13                     13U
#define GPIOD_PIN14                     14U
#define GPIOD_PIN15                     15U

#define GPIOE_PIN0                      0U
#define GPIOE_PIN1                      1U
#define GPIOE_PIN2                      2U
#define GPIOE_PIN3                      3U
#define GPIOE_PIN4                      4U
#define GPIOE_PIN5                      5U
#define GPIOE_PIN6                      6U
#define GPIOE_PIN7                      7U
#define GPIOE_PIN8                      8U
#define GPIOE_PIN9                      9U
#define GPIOE_PIN10                     10U
#define GPIOE_PIN11                     11U
#define GPIOE_PIN12                     12U
#define GPIOE_PIN13                     13U
#define GPIOE_PIN14                     14U
#define GPIOE_PIN15                     15U

#define GPIOF_PIN0                      0U
#define GPIOF_PIN1                      1U
#define GPIOF_PIN2                      2U
#define GPIOF_PIN3                      3U
#define GPIOF_PIN4                      4U
#define GPIOF_PIN5                      5U
#define GPIOF_PIN6                      6U
#define GPIOF_PIN7                      7U
#define GPIOF_PIN8                      8U
#define GPIOF_PIN9                      9U
#define GPIOF_PIN10                     10U
#define GPIOF_PIN11                     11U
#define GPIOF_PIN12                     12U
#define GPIOF_PIN13                     13U
#define GPIOF_PIN14                     14U
#define GPIOF_PIN15                     15U

#define GPIOG_PIN0                      0U
#define GPIOG_PIN1                      1U
#define GPIOG_PIN2                      2U
#define GPIOG_PIN3                      3U
#define GPIOG_PIN4                      4U
#define GPIOG_PIN5                      5U
#define GPIOG_PIN6                      6U
#define GPIOG_PIN7                      7U
#define GPIOG_PIN8                      8U
#define GPIOG_PIN9                      9U
#define GPIOG_PIN10                     10U
#define GPIOG_PIN11                     11U
#define GPIOG_PIN12                     12U
#define GPIOG_PIN13                     13U
#define GPIOG_PIN14                     14U
#define GPIOG_PIN15                     15U

#define GPIOH_OSC_IN                    0U
#define GPIOH_OSC_OUT                   1U
#define GPIOH_PIN2                      2U
#define GPIOH_PIN3                      3U
#define GPIOH_PIN4                      4U
#define GPIOH_PIN5                      5U
#define GPIOH_PIN6                      6U
#define GPIOH_PIN7                      7U
#define GPIOH_PIN8                      8U
#define GPIOH_PIN9                      9U
#define GPIOH_PIN10                     10U
#define GPIOH_PIN11                     11U
#define GPIOH_PIN12                     12U
#define GPIOH_PIN13                     13U
#define GPIOH_PIN14                     14U
#define GPIOH_PIN15                     15U

#define GPIOI_PIN0                      0U
#define GPIOI_PIN1                      1U
#define GPIOI_PIN2                      2U
#define GPIOI_PIN3                      3U
#define GPIOI_PIN4                      4U
#define GPIOI_PIN5                      5U
#define GPIOI_PIN6                      6U
#define GPIOI_PIN7                      7U
#define GPIOI_PIN8                      8U
#define GPIOI_PIN9                      9U
#define GPIOI_PIN10                     10U
#define GPIOI_PIN11                     11U
#define GPIOI_PIN12                     12U
#define GPIOI_PIN13                     13U
#define GPIOI_PIN14                     14U
#define GPIOI_PIN15                     15U

/*
 * IO lines assignments.
 */
#define LINE_WKUP                       PAL_LINE(GPIOA, GPIOA_WKUP)
#define LINE_SWITCH_STATUS_N            PAL_LINE(GPIOA, GPIOA_SWITCH_STATUS_N)
#define LINE_SYS_UART_TX                PAL_LINE(GPIOA, GPIOA_SYS_UART_TX)
#define LINE_SYS_UART_RX                PAL_LINE(GPIOA, GPIOA_SYS_UART_RX)
#define LINE_SYS_SPI_SS0_N              PAL_LINE(GPIOA, GPIOA_SYS_SPI_SS0_N)
#define LINE_SYS_SPI_SCLK               PAL_LINE(GPIOA, GPIOA_SYS_SPI_SCLK)
#define LINE_SYS_SPI_MISO               PAL_LINE(GPIOA, GPIOA_SYS_SPI_MISO)
#define LINE_SYS_SPI_MOSI               PAL_LINE(GPIOA, GPIOA_SYS_SPI_MOSI)
#define LINE_SYS_REG_EN                 PAL_LINE(GPIOA, GPIOA_SYS_REG_EN)
#define LINE_PROG_RX                    PAL_LINE(GPIOA, GPIOA_PROG_RX)
#define LINE_PROG_TX                    PAL_LINE(GPIOA, GPIOA_PROG_TX)
#define LINE_CAN_RX                     PAL_LINE(GPIOA, GPIOA_CAN_RX)
#define LINE_CAN_TX                     PAL_LINE(GPIOA, GPIOA_CAN_TX)
#define LINE_SWDIO                      PAL_LINE(GPIOA, GPIOA_SWDIO)
#define LINE_SWCLK                      PAL_LINE(GPIOA, GPIOA_SWCLK)
#define LINE_SYS_SPI_SS1_N              PAL_LINE(GPIOA, GPIOA_SYS_SPI_SS1_N)

#define LINE_IR_INT1_N                  PAL_LINE(GPIOB, GPIOB_IR_INT1_N)
#define LINE_VSYS_SENSE                 PAL_LINE(GPIOB, GPIOB_VSYS_SENSE)
#define LINE_POWER_EN                   PAL_LINE(GPIOB, GPIOB_POWER_EN)
#define LINE_SYS_UART_DN                PAL_LINE(GPIOB, GPIOB_SYS_UART_DN)
#define LINE_CHARGE_STAT2A              PAL_LINE(GPIOB, GPIOB_CHARGE_STAT2A)
#define LINE_BUZZER                     PAL_LINE(GPIOB, GPIOB_BUZZER)
#define LINE_GAUGE_BATLOW2              PAL_LINE(GPIOB, GPIOB_GAUGE_BATLOW2)
#define LINE_GAUGE_BATGD2_N             PAL_LINE(GPIOB, GPIOB_GAUGE_BATGD2_N)
#define LINE_GAUGE_SCL2                 PAL_LINE(GPIOB, GPIOB_GAUGE_SCL2)
#define LINE_GAUGE_SDA2                 PAL_LINE(GPIOB, GPIOB_GAUGE_SDA2)
#define LINE_GAUGE_SCL1                 PAL_LINE(GPIOB, GPIOB_GAUGE_SCL1)
#define LINE_GAUGE_SDA1                 PAL_LINE(GPIOB, GPIOB_GAUGE_SDA1)
#define LINE_LED                        PAL_LINE(GPIOB, GPIOB_LED)
#define LINE_BT_RTS                     PAL_LINE(GPIOB, GPIOB_BT_RTS)
#define LINE_BT_CTS                     PAL_LINE(GPIOB, GPIOB_BT_CTS)
#define LINE_SYS_UART_UP                PAL_LINE(GPIOB, GPIOB_SYS_UART_UP)

#define LINE_CHARGE_STAT1A              PAL_LINE(GPIOC, GPIOC_CHARGE_STAT1A)
#define LINE_GAUGE_BATLOW1              PAL_LINE(GPIOC, GPIOC_GAUGE_BATLOW1)
#define LINE_GAUGE_BATGD1_N             PAL_LINE(GPIOC, GPIOC_GAUGE_BATGD1_N)
#define LINE_CHARGE_EN1_N               PAL_LINE(GPIOC, GPIOC_CHARGE_EN1_N)
#define LINE_IR_INT2_N                  PAL_LINE(GPIOC, GPIOC_IR_INT2_N)
#define LINE_TOUCH_INT_N                PAL_LINE(GPIOC, GPIOC_TOUCH_INT_N)
#define LINE_SYS_DONE                   PAL_LINE(GPIOC, GPIOC_SYS_DONE)
#define LINE_SYS_PROG_N                 PAL_LINE(GPIOC, GPIOC_SYS_PROG_N)
#define LINE_PATH_DC                    PAL_LINE(GPIOC, GPIOC_PATH_DC)
#define LINE_SYS_SPI_DIR                PAL_LINE(GPIOC, GPIOC_SYS_SPI_DIR)
#define LINE_BT_RX                      PAL_LINE(GPIOC, GPIOC_BT_RX)
#define LINE_BT_TX                      PAL_LINE(GPIOC, GPIOC_BT_TX)
#define LINE_SYS_INT_N                  PAL_LINE(GPIOC, GPIOC_SYS_INT_N)
#define LINE_SYS_PD_N                   PAL_LINE(GPIOC, GPIOC_SYS_PD_N)
#define LINE_SYS_WARMRST_N              PAL_LINE(GPIOC, GPIOC_SYS_WARMRST_N)
#define LINE_BT_RST                     PAL_LINE(GPIOC, GPIOC_BT_RST)

#define LINE_CHARGE_EN2_N               PAL_LINE(GPIOD, GPIOD_CHARGE_EN2_N)

#define LINE_OSC_IN                     PAL_LINE(GPIOH, 0U)
#define LINE_OSC_OUT                    PAL_LINE(GPIOH, 1U)

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
#define PIN_MODE_INPUT(n)               (0U << ((n) * 2U))
#define PIN_MODE_OUTPUT(n)              (1U << ((n) * 2U))
#define PIN_MODE_ALTERNATE(n)           (2U << ((n) * 2U))
#define PIN_MODE_ANALOG(n)              (3U << ((n) * 2U))
#define PIN_ODR_LOW(n)                  (0U << (n))
#define PIN_ODR_HIGH(n)                 (1U << (n))
#define PIN_OTYPE_PUSHPULL(n)           (0U << (n))
#define PIN_OTYPE_OPENDRAIN(n)          (1U << (n))
#define PIN_OSPEED_LOW(n)               (0U << ((n) * 2U))
#define PIN_OSPEED_MEDIUM(n)            (1U << ((n) * 2U))
#define PIN_OSPEED_HIGH(n)              (2U << ((n) * 2U))
#define PIN_OSPEED_VERYHIGH(n)          (3U << ((n) * 2U))
#define PIN_PUPDR_FLOATING(n)           (0U << ((n) * 2U))
#define PIN_PUPDR_PULLUP(n)             (1U << ((n) * 2U))
#define PIN_PUPDR_PULLDOWN(n)           (2U << ((n) * 2U))
#define PIN_AFIO_AF(n, v)               ((v) << (((n) % 8U) * 4U))

/*
 * GPIOA setup:
 *
 * PA0  - WKUP                          (input floating)
 * PA1  - SWITCH_STATUS_N               (input floating)
 * PA2  - SYS_UART_TX                   (input floating)
 * PA3  - SYS_UART_RX                   (alternate 7 pushpull floating)
 * PA4  - SYS_SPI_SS0_N                 (input floating)
 * PA5  - SYS_SPI_SCLK                  (alternate 5 pushpull floating)
 * PA6  - SYS_SPI_MISO                  (alternate 5 pushpull floating)
 * PA7  - SYS_SPI_MOSI                  (alternate 5 pushpull floating)
 * PA8  - SYS_REG_EN                    (output pushpull high)
 * PA9  - PROG_RX                       (alternate 7 pushpull floating)
 * PA10 - PROG_TX                       (alternate 7 pushpull pullup)
 * PA11 - CAN_RX                        (alternate 9 pushpull floating)
 * PA12 - CAN_TX                        (alternate 9 pushpull floating)
 * PA13 - SWDIO                         (alternate 0 pushpull floating)
 * PA14 - SWCLK                         (alternate 0 pushpull floating)
 * PA15 - SYS_SPI_SS1_N                 (input floating)
 */
#define VAL_GPIOA_MODER                 (PIN_MODE_INPUT(GPIOA_WKUP) |                                 \
                                         PIN_MODE_INPUT(GPIOA_SWITCH_STATUS_N) |                      \
                                         PIN_MODE_INPUT(GPIOA_SYS_UART_TX) |                          \
                                         PIN_MODE_ALTERNATE(GPIOA_SYS_UART_RX) |                      \
                                         PIN_MODE_INPUT(GPIOA_SYS_SPI_SS0_N) |                        \
                                         PIN_MODE_ALTERNATE(GPIOA_SYS_SPI_SCLK) |                     \
                                         PIN_MODE_ALTERNATE(GPIOA_SYS_SPI_MISO) |                     \
                                         PIN_MODE_ALTERNATE(GPIOA_SYS_SPI_MOSI) |                     \
                                         PIN_MODE_OUTPUT(GPIOA_SYS_REG_EN) |                          \
                                         PIN_MODE_ALTERNATE(GPIOA_PROG_RX) |                          \
                                         PIN_MODE_ALTERNATE(GPIOA_PROG_TX) |                          \
                                         PIN_MODE_ALTERNATE(GPIOA_CAN_RX) |                           \
                                         PIN_MODE_ALTERNATE(GPIOA_CAN_TX) |                           \
                                         PIN_MODE_ALTERNATE(GPIOA_SWDIO) |                            \
                                         PIN_MODE_ALTERNATE(GPIOA_SWCLK) |                            \
                                         PIN_MODE_INPUT(GPIOA_SYS_SPI_SS1_N))
#define VAL_GPIOA_OTYPER                (PIN_OTYPE_PUSHPULL(GPIOA_WKUP) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOA_SWITCH_STATUS_N) |                  \
                                         PIN_OTYPE_PUSHPULL(GPIOA_SYS_UART_TX) |                      \
                                         PIN_OTYPE_PUSHPULL(GPIOA_SYS_UART_RX) |                      \
                                         PIN_OTYPE_PUSHPULL(GPIOA_SYS_SPI_SS0_N) |                    \
                                         PIN_OTYPE_PUSHPULL(GPIOA_SYS_SPI_SCLK) |                     \
                                         PIN_OTYPE_PUSHPULL(GPIOA_SYS_SPI_MISO) |                     \
                                         PIN_OTYPE_PUSHPULL(GPIOA_SYS_SPI_MOSI) |                     \
                                         PIN_OTYPE_PUSHPULL(GPIOA_SYS_REG_EN) |                       \
                                         PIN_OTYPE_PUSHPULL(GPIOA_PROG_RX) |                          \
                                         PIN_OTYPE_PUSHPULL(GPIOA_PROG_TX) |                          \
                                         PIN_OTYPE_PUSHPULL(GPIOA_CAN_RX) |                           \
                                         PIN_OTYPE_PUSHPULL(GPIOA_CAN_TX) |                           \
                                         PIN_OTYPE_PUSHPULL(GPIOA_SWDIO) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOA_SWCLK) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOA_SYS_SPI_SS1_N))
#define VAL_GPIOA_OSPEEDR               (PIN_OSPEED_VERYHIGH(GPIOA_WKUP) |                            \
                                         PIN_OSPEED_VERYHIGH(GPIOA_SWITCH_STATUS_N) |                 \
                                         PIN_OSPEED_VERYHIGH(GPIOA_SYS_UART_TX) |                     \
                                         PIN_OSPEED_VERYHIGH(GPIOA_SYS_UART_RX) |                     \
                                         PIN_OSPEED_VERYHIGH(GPIOA_SYS_SPI_SS0_N) |                   \
                                         PIN_OSPEED_VERYHIGH(GPIOA_SYS_SPI_SCLK) |                    \
                                         PIN_OSPEED_VERYHIGH(GPIOA_SYS_SPI_MISO) |                    \
                                         PIN_OSPEED_VERYHIGH(GPIOA_SYS_SPI_MOSI) |                    \
                                         PIN_OSPEED_VERYHIGH(GPIOA_SYS_REG_EN) |                      \
                                         PIN_OSPEED_VERYHIGH(GPIOA_PROG_RX) |                         \
                                         PIN_OSPEED_VERYHIGH(GPIOA_PROG_TX) |                         \
                                         PIN_OSPEED_VERYHIGH(GPIOA_CAN_RX) |                          \
                                         PIN_OSPEED_VERYHIGH(GPIOA_CAN_TX) |                          \
                                         PIN_OSPEED_VERYHIGH(GPIOA_SWDIO) |                           \
                                         PIN_OSPEED_VERYHIGH(GPIOA_SWCLK) |                           \
                                         PIN_OSPEED_VERYHIGH(GPIOA_SYS_SPI_SS1_N))
#define VAL_GPIOA_PUPDR                 (PIN_PUPDR_FLOATING(GPIOA_WKUP) |                             \
                                         PIN_PUPDR_FLOATING(GPIOA_SWITCH_STATUS_N) |                  \
                                         PIN_PUPDR_FLOATING(GPIOA_SYS_UART_TX) |                      \
                                         PIN_PUPDR_FLOATING(GPIOA_SYS_UART_RX) |                      \
                                         PIN_PUPDR_FLOATING(GPIOA_SYS_SPI_SS0_N) |                    \
                                         PIN_PUPDR_FLOATING(GPIOA_SYS_SPI_SCLK) |                     \
                                         PIN_PUPDR_FLOATING(GPIOA_SYS_SPI_MISO) |                     \
                                         PIN_PUPDR_FLOATING(GPIOA_SYS_SPI_MOSI) |                     \
                                         PIN_PUPDR_FLOATING(GPIOA_SYS_REG_EN) |                       \
                                         PIN_PUPDR_FLOATING(GPIOA_PROG_RX) |                          \
                                         PIN_PUPDR_PULLUP(GPIOA_PROG_TX) |                            \
                                         PIN_PUPDR_FLOATING(GPIOA_CAN_RX) |                           \
                                         PIN_PUPDR_FLOATING(GPIOA_CAN_TX) |                           \
                                         PIN_PUPDR_FLOATING(GPIOA_SWDIO) |                            \
                                         PIN_PUPDR_FLOATING(GPIOA_SWCLK) |                            \
                                         PIN_PUPDR_FLOATING(GPIOA_SYS_SPI_SS1_N))
#define VAL_GPIOA_ODR                   (PIN_ODR_HIGH(GPIOA_WKUP) |                                   \
                                         PIN_ODR_HIGH(GPIOA_SWITCH_STATUS_N) |                        \
                                         PIN_ODR_HIGH(GPIOA_SYS_UART_TX) |                            \
                                         PIN_ODR_HIGH(GPIOA_SYS_UART_RX) |                            \
                                         PIN_ODR_HIGH(GPIOA_SYS_SPI_SS0_N) |                          \
                                         PIN_ODR_HIGH(GPIOA_SYS_SPI_SCLK) |                           \
                                         PIN_ODR_HIGH(GPIOA_SYS_SPI_MISO) |                           \
                                         PIN_ODR_HIGH(GPIOA_SYS_SPI_MOSI) |                           \
                                         PIN_ODR_HIGH(GPIOA_SYS_REG_EN) |                             \
                                         PIN_ODR_HIGH(GPIOA_PROG_RX) |                                \
                                         PIN_ODR_HIGH(GPIOA_PROG_TX) |                                \
                                         PIN_ODR_HIGH(GPIOA_CAN_RX) |                                 \
                                         PIN_ODR_HIGH(GPIOA_CAN_TX) |                                 \
                                         PIN_ODR_HIGH(GPIOA_SWDIO) |                                  \
                                         PIN_ODR_HIGH(GPIOA_SWCLK) |                                  \
                                         PIN_ODR_HIGH(GPIOA_SYS_SPI_SS1_N))
#define VAL_GPIOA_AFRL                  (PIN_AFIO_AF(GPIOA_WKUP, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOA_SWITCH_STATUS_N, STM32F4xx_AF_system) |    \
                                         PIN_AFIO_AF(GPIOA_SYS_UART_TX, STM32F4xx_AF_USART1to3) |     \
                                         PIN_AFIO_AF(GPIOA_SYS_UART_RX, STM32F4xx_AF_USART1to3) |     \
                                         PIN_AFIO_AF(GPIOA_SYS_SPI_SS0_N, STM32F4xx_AF_system) |      \
                                         PIN_AFIO_AF(GPIOA_SYS_SPI_SCLK, STM32F4xx_AF_SPI1to2) |      \
                                         PIN_AFIO_AF(GPIOA_SYS_SPI_MISO, STM32F4xx_AF_SPI1to2) |      \
                                         PIN_AFIO_AF(GPIOA_SYS_SPI_MOSI, STM32F4xx_AF_SPI1to2))
#define VAL_GPIOA_AFRH                  (PIN_AFIO_AF(GPIOA_SYS_REG_EN, STM32F4xx_AF_system) |         \
                                         PIN_AFIO_AF(GPIOA_PROG_RX, STM32F4xx_AF_USART1to3) |         \
                                         PIN_AFIO_AF(GPIOA_PROG_TX, STM32F4xx_AF_USART1to3) |         \
                                         PIN_AFIO_AF(GPIOA_CAN_RX, STM32F4xx_AF_CAN1to2_TIM12to14) |  \
                                         PIN_AFIO_AF(GPIOA_CAN_TX, STM32F4xx_AF_CAN1to2_TIM12to14) |  \
                                         PIN_AFIO_AF(GPIOA_SWDIO, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOA_SWCLK, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOA_SYS_SPI_SS1_N, STM32F4xx_AF_system))

/*
 * GPIOB setup:
 *
 * PB0  - IR_INT1_N                     (input floating)
 * PB1  - VSYS_SENSE                    (analog)
 * PB2  - POWER_EN                      (output pushpull high)
 * PB3  - SYS_UART_DN                   (output opendrain high)
 * PB4  - CHARGE_STAT2A                 (inout floating)
 * PB5  - BUZZER                        (alternate 2 pushpull floating)
 * PB6  - GAUGE_BATLOW2                 (input floating)
 * PB7  - GAUGE_BATGD2_N                (input floating)
 * PB8  - GAUGE_SCL2                    (alternate 4 opendrain floating)
 * PB9  - GAUGE_SDA2                    (alternate 4 opendrain floating)
 * PB10 - GAUGE_SCL1                    (alternate 4 opendrain floating)
 * PB11 - GAUGE_SDA1                    (alternate 4 opendrain floating)
 * PB12 - LED                           (output opendrain high)
 * PB13 - BT_RTS                        (alternate 7 pushpull floating)
 * PB14 - BT_CTS                        (inout floating)
 * PB15 - SYS_UART_UP                   (output opendrain high)
 */
#define VAL_GPIOB_MODER                 (PIN_MODE_INPUT(GPIOB_IR_INT1_N) |                            \
                                         PIN_MODE_ANALOG(GPIOB_VSYS_SENSE) |                          \
                                         PIN_MODE_OUTPUT(GPIOB_POWER_EN) |                            \
                                         PIN_MODE_OUTPUT(GPIOB_SYS_UART_DN) |                         \
                                         PIN_MODE_INPUT(GPIOB_CHARGE_STAT2A) |                        \
                                         PIN_MODE_ALTERNATE(GPIOB_BUZZER) |                           \
                                         PIN_MODE_INPUT(GPIOB_GAUGE_BATLOW2) |                        \
                                         PIN_MODE_INPUT(GPIOB_GAUGE_BATGD2_N) |                       \
                                         PIN_MODE_ALTERNATE(GPIOB_GAUGE_SCL2) |                       \
                                         PIN_MODE_ALTERNATE(GPIOB_GAUGE_SDA2) |                       \
                                         PIN_MODE_ALTERNATE(GPIOB_GAUGE_SCL1) |                       \
                                         PIN_MODE_ALTERNATE(GPIOB_GAUGE_SDA1) |                       \
                                         PIN_MODE_OUTPUT(GPIOB_LED) |                                 \
                                         PIN_MODE_ALTERNATE(GPIOB_BT_RTS) |                           \
                                         PIN_MODE_INPUT(GPIOB_BT_CTS) |                               \
                                         PIN_MODE_OUTPUT(GPIOB_SYS_UART_UP))
#define VAL_GPIOB_OTYPER                (PIN_OTYPE_PUSHPULL(GPIOB_IR_INT1_N) |                        \
                                         PIN_OTYPE_PUSHPULL(GPIOB_VSYS_SENSE) |                       \
                                         PIN_OTYPE_PUSHPULL(GPIOB_POWER_EN) |                         \
                                         PIN_OTYPE_OPENDRAIN(GPIOB_SYS_UART_DN) |                     \
                                         PIN_OTYPE_PUSHPULL(GPIOB_CHARGE_STAT2A) |                    \
                                         PIN_OTYPE_PUSHPULL(GPIOB_BUZZER) |                           \
                                         PIN_OTYPE_PUSHPULL(GPIOB_GAUGE_BATLOW2) |                    \
                                         PIN_OTYPE_PUSHPULL(GPIOB_GAUGE_BATGD2_N) |                   \
                                         PIN_OTYPE_OPENDRAIN(GPIOB_GAUGE_SCL2) |                      \
                                         PIN_OTYPE_OPENDRAIN(GPIOB_GAUGE_SDA2) |                      \
                                         PIN_OTYPE_OPENDRAIN(GPIOB_GAUGE_SCL1) |                      \
                                         PIN_OTYPE_OPENDRAIN(GPIOB_GAUGE_SDA1) |                      \
                                         PIN_OTYPE_OPENDRAIN(GPIOB_LED) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOB_BT_RTS) |                           \
                                         PIN_OTYPE_PUSHPULL(GPIOB_BT_CTS) |                           \
                                         PIN_OTYPE_OPENDRAIN(GPIOB_SYS_UART_UP))
#define VAL_GPIOB_OSPEEDR               (PIN_OSPEED_VERYHIGH(GPIOB_IR_INT1_N) |                       \
                                         PIN_OSPEED_VERYHIGH(GPIOB_VSYS_SENSE) |                      \
                                         PIN_OSPEED_VERYHIGH(GPIOB_POWER_EN) |                        \
                                         PIN_OSPEED_VERYHIGH(GPIOB_SYS_UART_DN) |                     \
                                         PIN_OSPEED_VERYHIGH(GPIOB_CHARGE_STAT2A) |                   \
                                         PIN_OSPEED_VERYHIGH(GPIOB_BUZZER) |                          \
                                         PIN_OSPEED_VERYHIGH(GPIOB_GAUGE_BATLOW2) |                   \
                                         PIN_OSPEED_VERYHIGH(GPIOB_GAUGE_BATGD2_N) |                  \
                                         PIN_OSPEED_VERYHIGH(GPIOB_GAUGE_SCL2) |                      \
                                         PIN_OSPEED_VERYHIGH(GPIOB_GAUGE_SDA2) |                      \
                                         PIN_OSPEED_VERYHIGH(GPIOB_GAUGE_SCL1) |                      \
                                         PIN_OSPEED_VERYHIGH(GPIOB_GAUGE_SDA1) |                      \
                                         PIN_OSPEED_VERYHIGH(GPIOB_LED) |                             \
                                         PIN_OSPEED_VERYHIGH(GPIOB_BT_RTS) |                          \
                                         PIN_OSPEED_VERYHIGH(GPIOB_BT_CTS) |                          \
                                         PIN_OSPEED_VERYHIGH(GPIOB_SYS_UART_UP))
#define VAL_GPIOB_PUPDR                 (PIN_PUPDR_FLOATING(GPIOB_IR_INT1_N) |                        \
                                         PIN_PUPDR_FLOATING(GPIOB_VSYS_SENSE) |                       \
                                         PIN_PUPDR_FLOATING(GPIOB_POWER_EN) |                         \
                                         PIN_PUPDR_FLOATING(GPIOB_SYS_UART_DN) |                      \
                                         PIN_PUPDR_FLOATING(GPIOB_CHARGE_STAT2A) |                    \
                                         PIN_PUPDR_FLOATING(GPIOB_BUZZER) |                           \
                                         PIN_PUPDR_FLOATING(GPIOB_GAUGE_BATLOW2) |                    \
                                         PIN_PUPDR_FLOATING(GPIOB_GAUGE_BATGD2_N) |                   \
                                         PIN_PUPDR_FLOATING(GPIOB_GAUGE_SCL2) |                       \
                                         PIN_PUPDR_FLOATING(GPIOB_GAUGE_SDA2) |                       \
                                         PIN_PUPDR_FLOATING(GPIOB_GAUGE_SCL1) |                       \
                                         PIN_PUPDR_FLOATING(GPIOB_GAUGE_SDA1) |                       \
                                         PIN_PUPDR_FLOATING(GPIOB_LED) |                              \
                                         PIN_PUPDR_FLOATING(GPIOB_BT_RTS) |                           \
                                         PIN_PUPDR_FLOATING(GPIOB_BT_CTS) |                           \
                                         PIN_PUPDR_FLOATING(GPIOB_SYS_UART_UP))
#define VAL_GPIOB_ODR                   (PIN_ODR_HIGH(GPIOB_IR_INT1_N) |                              \
                                         PIN_ODR_HIGH(GPIOB_VSYS_SENSE) |                             \
                                         PIN_ODR_HIGH(GPIOB_POWER_EN) |                               \
                                         PIN_ODR_HIGH(GPIOB_SYS_UART_DN) |                            \
                                         PIN_ODR_HIGH(GPIOB_CHARGE_STAT2A) |                          \
                                         PIN_ODR_HIGH(GPIOB_BUZZER) |                                 \
                                         PIN_ODR_HIGH(GPIOB_GAUGE_BATLOW2) |                          \
                                         PIN_ODR_HIGH(GPIOB_GAUGE_BATGD2_N) |                         \
                                         PIN_ODR_HIGH(GPIOB_GAUGE_SCL2) |                             \
                                         PIN_ODR_HIGH(GPIOB_GAUGE_SDA2) |                             \
                                         PIN_ODR_HIGH(GPIOB_GAUGE_SCL1) |                             \
                                         PIN_ODR_HIGH(GPIOB_GAUGE_SDA1) |                             \
                                         PIN_ODR_HIGH(GPIOB_LED) |                                    \
                                         PIN_ODR_HIGH(GPIOB_BT_RTS) |                                 \
                                         PIN_ODR_HIGH(GPIOB_BT_CTS) |                                 \
                                         PIN_ODR_HIGH(GPIOB_SYS_UART_UP))
#define VAL_GPIOB_AFRL                  (PIN_AFIO_AF(GPIOB_IR_INT1_N, STM32F4xx_AF_system) |          \
                                         PIN_AFIO_AF(GPIOB_VSYS_SENSE, STM32F4xx_AF_system) |         \
                                         PIN_AFIO_AF(GPIOB_POWER_EN, STM32F4xx_AF_system) |           \
                                         PIN_AFIO_AF(GPIOB_SYS_UART_DN, STM32F4xx_AF_system) |        \
                                         PIN_AFIO_AF(GPIOB_CHARGE_STAT2A, STM32F4xx_AF_system) |      \
                                         PIN_AFIO_AF(GPIOB_BUZZER, STM32F4xx_AF_TIM3to5) |            \
                                         PIN_AFIO_AF(GPIOB_GAUGE_BATLOW2, STM32F4xx_AF_system) |      \
                                         PIN_AFIO_AF(GPIOB_GAUGE_BATGD2_N, STM32F4xx_AF_system))
#define VAL_GPIOB_AFRH                  (PIN_AFIO_AF(GPIOB_GAUGE_SCL2, STM32F4xx_AF_I2C1to3) |        \
                                         PIN_AFIO_AF(GPIOB_GAUGE_SDA2, STM32F4xx_AF_I2C1to3) |        \
                                         PIN_AFIO_AF(GPIOB_GAUGE_SCL1, STM32F4xx_AF_I2C1to3) |        \
                                         PIN_AFIO_AF(GPIOB_GAUGE_SDA1, STM32F4xx_AF_I2C1to3) |        \
                                         PIN_AFIO_AF(GPIOB_LED, STM32F4xx_AF_system) |                \
                                         PIN_AFIO_AF(GPIOB_BT_RTS, STM32F4xx_AF_USART1to3) |          \
                                         PIN_AFIO_AF(GPIOB_BT_CTS, STM32F4xx_AF_USART1to3) |          \
                                         PIN_AFIO_AF(GPIOB_SYS_UART_UP, STM32F4xx_AF_system))

/*
 * GPIOC setup:
 *
 * PC0  - CHARGE_STAT1A                 (input floating)
 * PC1  - GAUGE_BATLOW1                 (input floating)
 * PC2  - GAUGE_BATGD1_N                (input floating)
 * PC3  - CHARGE_EN1_N                  (output opendrain high)
 * PC4  - IR_INT2_N                     (input floating)
 * PC5  - TOUCH_INT_N                   (input floating)
 * PC6  - SYS_DONE                      (input floating)
 * PC7  - SYS_PROG_N                    (output opendrain high)
 * PC8  - PATH_DC                       (input floating)
 * PC9  - SYS_SPI_DIR                   (output opendrain high)
 * PC10 - BT_RX                         (alternate 7 pushpull floating)
 * PC11 - BT_TX                         (alternate 7 pushpull floating)
 * PC12 - SYS_INT_N                     (output opendrain low)
 * PC13 - SYS_PD_N                      (output opendrain high)
 * PC14 - SYS_WARMRST_N                 (output opendrain high)
 * PC15 - BT_RST                        (output opendrain high)
 */
#define VAL_GPIOC_MODER                 (PIN_MODE_INPUT(GPIOC_CHARGE_STAT1A) |                        \
                                         PIN_MODE_INPUT(GPIOC_GAUGE_BATLOW1) |                        \
                                         PIN_MODE_INPUT(GPIOC_GAUGE_BATGD1_N) |                       \
                                         PIN_MODE_OUTPUT(GPIOC_CHARGE_EN1_N) |                        \
                                         PIN_MODE_INPUT(GPIOC_IR_INT2_N) |                            \
                                         PIN_MODE_INPUT(GPIOC_TOUCH_INT_N) |                          \
                                         PIN_MODE_INPUT(GPIOC_SYS_DONE) |                             \
                                         PIN_MODE_OUTPUT(GPIOC_SYS_PROG_N) |                          \
                                         PIN_MODE_INPUT(GPIOC_PATH_DC) |                              \
                                         PIN_MODE_OUTPUT(GPIOC_SYS_SPI_DIR) |                         \
                                         PIN_MODE_ALTERNATE(GPIOC_BT_RX) |                            \
                                         PIN_MODE_ALTERNATE(GPIOC_BT_TX) |                            \
                                         PIN_MODE_OUTPUT(GPIOC_SYS_INT_N) |                           \
                                         PIN_MODE_OUTPUT(GPIOC_SYS_PD_N) |                            \
                                         PIN_MODE_OUTPUT(GPIOC_SYS_WARMRST_N) |                       \
                                         PIN_MODE_OUTPUT(GPIOC_BT_RST))
#define VAL_GPIOC_OTYPER                (PIN_OTYPE_PUSHPULL(GPIOC_CHARGE_STAT1A) |                    \
                                         PIN_OTYPE_PUSHPULL(GPIOC_GAUGE_BATLOW1) |                    \
                                         PIN_OTYPE_PUSHPULL(GPIOC_GAUGE_BATGD1_N) |                   \
                                         PIN_OTYPE_OPENDRAIN(GPIOC_CHARGE_EN1_N) |                    \
                                         PIN_OTYPE_PUSHPULL(GPIOC_IR_INT2_N) |                        \
                                         PIN_OTYPE_PUSHPULL(GPIOC_TOUCH_INT_N) |                      \
                                         PIN_OTYPE_PUSHPULL(GPIOC_SYS_DONE) |                         \
                                         PIN_OTYPE_OPENDRAIN(GPIOC_SYS_PROG_N) |                      \
                                         PIN_OTYPE_PUSHPULL(GPIOC_PATH_DC) |                          \
                                         PIN_OTYPE_OPENDRAIN(GPIOC_SYS_SPI_DIR) |                     \
                                         PIN_OTYPE_PUSHPULL(GPIOC_BT_RX) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOC_BT_TX) |                            \
                                         PIN_OTYPE_OPENDRAIN(GPIOC_SYS_INT_N) |                       \
                                         PIN_OTYPE_OPENDRAIN(GPIOC_SYS_PD_N) |                        \
                                         PIN_OTYPE_OPENDRAIN(GPIOC_SYS_WARMRST_N) |                   \
                                         PIN_OTYPE_OPENDRAIN(GPIOC_BT_RST))
#define VAL_GPIOC_OSPEEDR               (PIN_OSPEED_VERYHIGH(GPIOC_CHARGE_STAT1A) |                   \
                                         PIN_OSPEED_VERYHIGH(GPIOC_GAUGE_BATLOW1) |                   \
                                         PIN_OSPEED_VERYHIGH(GPIOC_GAUGE_BATGD1_N) |                  \
                                         PIN_OSPEED_VERYHIGH(GPIOC_CHARGE_EN1_N) |                    \
                                         PIN_OSPEED_VERYHIGH(GPIOC_IR_INT2_N) |                       \
                                         PIN_OSPEED_VERYHIGH(GPIOC_TOUCH_INT_N) |                     \
                                         PIN_OSPEED_VERYHIGH(GPIOC_SYS_DONE) |                        \
                                         PIN_OSPEED_VERYHIGH(GPIOC_SYS_PROG_N) |                      \
                                         PIN_OSPEED_VERYHIGH(GPIOC_PATH_DC) |                         \
                                         PIN_OSPEED_VERYHIGH(GPIOC_SYS_SPI_DIR) |                     \
                                         PIN_OSPEED_VERYHIGH(GPIOC_BT_RX) |                           \
                                         PIN_OSPEED_VERYHIGH(GPIOC_BT_TX) |                           \
                                         PIN_OSPEED_VERYHIGH(GPIOC_SYS_INT_N) |                       \
                                         PIN_OSPEED_VERYHIGH(GPIOC_SYS_PD_N) |                        \
                                         PIN_OSPEED_VERYHIGH(GPIOC_SYS_WARMRST_N) |                   \
                                         PIN_OSPEED_VERYHIGH(GPIOC_BT_RST))
#define VAL_GPIOC_PUPDR                 (PIN_PUPDR_FLOATING(GPIOC_CHARGE_STAT1A) |                    \
                                         PIN_PUPDR_FLOATING(GPIOC_GAUGE_BATLOW1) |                    \
                                         PIN_PUPDR_FLOATING(GPIOC_GAUGE_BATGD1_N) |                   \
                                         PIN_PUPDR_FLOATING(GPIOC_CHARGE_EN1_N) |                     \
                                         PIN_PUPDR_FLOATING(GPIOC_IR_INT2_N) |                        \
                                         PIN_PUPDR_FLOATING(GPIOC_TOUCH_INT_N) |                      \
                                         PIN_PUPDR_FLOATING(GPIOC_SYS_DONE) |                         \
                                         PIN_PUPDR_FLOATING(GPIOC_SYS_PROG_N) |                       \
                                         PIN_PUPDR_FLOATING(GPIOC_PATH_DC) |                          \
                                         PIN_PUPDR_FLOATING(GPIOC_SYS_SPI_DIR) |                      \
                                         PIN_PUPDR_FLOATING(GPIOC_BT_RX) |                            \
                                         PIN_PUPDR_FLOATING(GPIOC_BT_TX) |                            \
                                         PIN_PUPDR_FLOATING(GPIOC_SYS_INT_N) |                        \
                                         PIN_PUPDR_FLOATING(GPIOC_SYS_PD_N) |                         \
                                         PIN_PUPDR_FLOATING(GPIOC_SYS_WARMRST_N) |                    \
                                         PIN_PUPDR_FLOATING(GPIOC_BT_RST))
#define VAL_GPIOC_ODR                   (PIN_ODR_HIGH(GPIOC_CHARGE_STAT1A) |                          \
                                         PIN_ODR_HIGH(GPIOC_GAUGE_BATLOW1) |                          \
                                         PIN_ODR_HIGH(GPIOC_GAUGE_BATGD1_N) |                         \
                                         PIN_ODR_HIGH(GPIOC_CHARGE_EN1_N) |                           \
                                         PIN_ODR_HIGH(GPIOC_IR_INT2_N) |                              \
                                         PIN_ODR_HIGH(GPIOC_TOUCH_INT_N) |                            \
                                         PIN_ODR_HIGH(GPIOC_SYS_DONE) |                               \
                                         PIN_ODR_HIGH(GPIOC_SYS_PROG_N) |                             \
                                         PIN_ODR_LOW(GPIOC_PATH_DC) |                                 \
                                         PIN_ODR_HIGH(GPIOC_SYS_SPI_DIR) |                            \
                                         PIN_ODR_HIGH(GPIOC_BT_RX) |                                  \
                                         PIN_ODR_HIGH(GPIOC_BT_TX) |                                  \
                                         PIN_ODR_LOW(GPIOC_SYS_INT_N) |                               \
                                         PIN_ODR_HIGH(GPIOC_SYS_PD_N) |                               \
                                         PIN_ODR_HIGH(GPIOC_SYS_WARMRST_N) |                          \
                                         PIN_ODR_HIGH(GPIOC_BT_RST))
#define VAL_GPIOC_AFRL                  (PIN_AFIO_AF(GPIOC_CHARGE_STAT1A, STM32F4xx_AF_system) |      \
                                         PIN_AFIO_AF(GPIOC_GAUGE_BATLOW1, STM32F4xx_AF_system) |      \
                                         PIN_AFIO_AF(GPIOC_GAUGE_BATGD1_N, STM32F4xx_AF_system) |     \
                                         PIN_AFIO_AF(GPIOC_CHARGE_EN1_N, STM32F4xx_AF_system) |       \
                                         PIN_AFIO_AF(GPIOC_IR_INT2_N, STM32F4xx_AF_system) |          \
                                         PIN_AFIO_AF(GPIOC_TOUCH_INT_N, STM32F4xx_AF_system) |        \
                                         PIN_AFIO_AF(GPIOC_SYS_DONE, STM32F4xx_AF_system) |           \
                                         PIN_AFIO_AF(GPIOC_SYS_PROG_N, STM32F4xx_AF_system))
#define VAL_GPIOC_AFRH                  (PIN_AFIO_AF(GPIOC_PATH_DC, STM32F4xx_AF_system) |            \
                                         PIN_AFIO_AF(GPIOC_SYS_SPI_DIR, STM32F4xx_AF_system) |        \
                                         PIN_AFIO_AF(GPIOC_BT_RX, STM32F4xx_AF_USART1to3) |           \
                                         PIN_AFIO_AF(GPIOC_BT_TX, STM32F4xx_AF_USART1to3) |           \
                                         PIN_AFIO_AF(GPIOC_SYS_INT_N, STM32F4xx_AF_system) |          \
                                         PIN_AFIO_AF(GPIOC_SYS_PD_N, STM32F4xx_AF_system) |           \
                                         PIN_AFIO_AF(GPIOC_SYS_WARMRST_N, STM32F4xx_AF_system) |      \
                                         PIN_AFIO_AF(GPIOC_BT_RST, STM32F4xx_AF_system))

/*
 * GPIOD setup:
 *
 * PD0  - PIN0                          (input floating)
 * PD1  - PIN1                          (input floating)
 * PD2  - CHARGE_EN2_N                  (output opendrain high)
 * PD3  - PIN3                          (input floating)
 * PD4  - PIN4                          (input floating)
 * PD5  - PIN5                          (input floating)
 * PD6  - PIN6                          (input floating)
 * PD7  - PIN7                          (input floating)
 * PD8  - PIN8                          (input floating)
 * PD9  - PIN9                          (input floating)
 * PD10 - PIN10                         (input floating)
 * PD11 - PIN11                         (input floating)
 * PD12 - PIN12                         (input floating)
 * PD13 - PIN13                         (input floating)
 * PD14 - PIN14                         (input floating)
 * PD15 - PIN15                         (input floating)
 */
#define VAL_GPIOD_MODER                 (PIN_MODE_INPUT(GPIOD_PIN0) |                                 \
                                         PIN_MODE_INPUT(GPIOD_PIN1) |                                 \
                                         PIN_MODE_OUTPUT(GPIOD_CHARGE_EN2_N) |                        \
                                         PIN_MODE_INPUT(GPIOD_PIN3) |                                 \
                                         PIN_MODE_INPUT(GPIOD_PIN4) |                                 \
                                         PIN_MODE_INPUT(GPIOD_PIN5) |                                 \
                                         PIN_MODE_INPUT(GPIOD_PIN6) |                                 \
                                         PIN_MODE_INPUT(GPIOD_PIN7) |                                 \
                                         PIN_MODE_INPUT(GPIOD_PIN8) |                                 \
                                         PIN_MODE_INPUT(GPIOD_PIN9) |                                 \
                                         PIN_MODE_INPUT(GPIOD_PIN10) |                                \
                                         PIN_MODE_INPUT(GPIOD_PIN11) |                                \
                                         PIN_MODE_INPUT(GPIOD_PIN12) |                                \
                                         PIN_MODE_INPUT(GPIOD_PIN13) |                                \
                                         PIN_MODE_INPUT(GPIOD_PIN14) |                                \
                                         PIN_MODE_INPUT(GPIOD_PIN15))
#define VAL_GPIOD_OTYPER                (PIN_OTYPE_PUSHPULL(GPIOD_PIN0) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOD_PIN1) |                             \
                                         PIN_OTYPE_OPENDRAIN(GPIOD_CHARGE_EN2_N) |                    \
                                         PIN_OTYPE_PUSHPULL(GPIOD_PIN3) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOD_PIN4) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOD_PIN5) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOD_PIN6) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOD_PIN7) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOD_PIN8) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOD_PIN9) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOD_PIN10) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOD_PIN11) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOD_PIN12) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOD_PIN13) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOD_PIN14) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOD_PIN15))
#define VAL_GPIOD_OSPEEDR               (PIN_OSPEED_VERYHIGH(GPIOD_PIN0) |                            \
                                         PIN_OSPEED_VERYHIGH(GPIOD_PIN1) |                            \
                                         PIN_OSPEED_VERYHIGH(GPIOD_CHARGE_EN2_N) |                    \
                                         PIN_OSPEED_VERYHIGH(GPIOD_PIN3) |                            \
                                         PIN_OSPEED_VERYHIGH(GPIOD_PIN4) |                            \
                                         PIN_OSPEED_VERYHIGH(GPIOD_PIN5) |                            \
                                         PIN_OSPEED_VERYHIGH(GPIOD_PIN6) |                            \
                                         PIN_OSPEED_VERYHIGH(GPIOD_PIN7) |                            \
                                         PIN_OSPEED_VERYHIGH(GPIOD_PIN8) |                            \
                                         PIN_OSPEED_VERYHIGH(GPIOD_PIN9) |                            \
                                         PIN_OSPEED_VERYHIGH(GPIOD_PIN10) |                           \
                                         PIN_OSPEED_VERYHIGH(GPIOD_PIN11) |                           \
                                         PIN_OSPEED_VERYHIGH(GPIOD_PIN12) |                           \
                                         PIN_OSPEED_VERYHIGH(GPIOD_PIN13) |                           \
                                         PIN_OSPEED_VERYHIGH(GPIOD_PIN14) |                           \
                                         PIN_OSPEED_VERYHIGH(GPIOD_PIN15))
#define VAL_GPIOD_PUPDR                 (PIN_PUPDR_PULLUP(GPIOD_PIN0) |                               \
                                         PIN_PUPDR_PULLUP(GPIOD_PIN1) |                               \
                                         PIN_PUPDR_FLOATING(GPIOD_CHARGE_EN2_N) |                     \
                                         PIN_PUPDR_PULLUP(GPIOD_PIN3) |                               \
                                         PIN_PUPDR_PULLUP(GPIOD_PIN4) |                               \
                                         PIN_PUPDR_PULLUP(GPIOD_PIN5) |                               \
                                         PIN_PUPDR_PULLUP(GPIOD_PIN6) |                               \
                                         PIN_PUPDR_PULLUP(GPIOD_PIN7) |                               \
                                         PIN_PUPDR_PULLUP(GPIOD_PIN8) |                               \
                                         PIN_PUPDR_PULLUP(GPIOD_PIN9) |                               \
                                         PIN_PUPDR_PULLUP(GPIOD_PIN10) |                              \
                                         PIN_PUPDR_PULLUP(GPIOD_PIN11) |                              \
                                         PIN_PUPDR_PULLUP(GPIOD_PIN12) |                              \
                                         PIN_PUPDR_PULLUP(GPIOD_PIN13) |                              \
                                         PIN_PUPDR_PULLUP(GPIOD_PIN14) |                              \
                                         PIN_PUPDR_PULLUP(GPIOD_PIN15))
#define VAL_GPIOD_ODR                   (PIN_ODR_HIGH(GPIOD_PIN0) |                                   \
                                         PIN_ODR_HIGH(GPIOD_PIN1) |                                   \
                                         PIN_ODR_HIGH(GPIOD_CHARGE_EN2_N) |                           \
                                         PIN_ODR_HIGH(GPIOD_PIN3) |                                   \
                                         PIN_ODR_HIGH(GPIOD_PIN4) |                                   \
                                         PIN_ODR_HIGH(GPIOD_PIN5) |                                   \
                                         PIN_ODR_HIGH(GPIOD_PIN6) |                                   \
                                         PIN_ODR_HIGH(GPIOD_PIN7) |                                   \
                                         PIN_ODR_HIGH(GPIOD_PIN8) |                                   \
                                         PIN_ODR_HIGH(GPIOD_PIN9) |                                   \
                                         PIN_ODR_HIGH(GPIOD_PIN10) |                                  \
                                         PIN_ODR_HIGH(GPIOD_PIN11) |                                  \
                                         PIN_ODR_HIGH(GPIOD_PIN12) |                                  \
                                         PIN_ODR_HIGH(GPIOD_PIN13) |                                  \
                                         PIN_ODR_HIGH(GPIOD_PIN14) |                                  \
                                         PIN_ODR_HIGH(GPIOD_PIN15))
#define VAL_GPIOD_AFRL                  (PIN_AFIO_AF(GPIOD_PIN0, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOD_PIN1, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOD_CHARGE_EN2_N, STM32F4xx_AF_system) |       \
                                         PIN_AFIO_AF(GPIOD_PIN3, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOD_PIN4, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOD_PIN5, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOD_PIN6, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOD_PIN7, STM32F4xx_AF_system))
#define VAL_GPIOD_AFRH                  (PIN_AFIO_AF(GPIOD_PIN8, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOD_PIN9, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOD_PIN10, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOD_PIN11, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOD_PIN12, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOD_PIN13, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOD_PIN14, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOD_PIN15, STM32F4xx_AF_system))

/*
 * GPIOE setup:
 *
 * PE0  - PIN0                          (input floating)
 * PE1  - PIN1                          (input floating)
 * PE2  - PIN2                          (input floating)
 * PE3  - PIN3                          (input floating)
 * PE4  - PIN4                          (input floating)
 * PE5  - PIN5                          (input floating)
 * PE6  - PIN6                          (input floating)
 * PE7  - PIN7                          (input floating)
 * PE8  - PIN8                          (input floating)
 * PE9  - PIN9                          (input floating)
 * PE10 - PIN10                         (input floating)
 * PE11 - PIN11                         (input floating)
 * PE12 - PIN12                         (input floating)
 * PE13 - PIN13                         (input floating)
 * PE14 - PIN14                         (input floating)
 * PE15 - PIN15                         (input floating)
 */
#define VAL_GPIOE_MODER                 (PIN_MODE_INPUT(GPIOE_PIN0) |                                 \
                                         PIN_MODE_INPUT(GPIOE_PIN1) |                                 \
                                         PIN_MODE_INPUT(GPIOE_PIN2) |                                 \
                                         PIN_MODE_INPUT(GPIOE_PIN3) |                                 \
                                         PIN_MODE_INPUT(GPIOE_PIN4) |                                 \
                                         PIN_MODE_INPUT(GPIOE_PIN5) |                                 \
                                         PIN_MODE_INPUT(GPIOE_PIN6) |                                 \
                                         PIN_MODE_INPUT(GPIOE_PIN7) |                                 \
                                         PIN_MODE_INPUT(GPIOE_PIN8) |                                 \
                                         PIN_MODE_INPUT(GPIOE_PIN9) |                                 \
                                         PIN_MODE_INPUT(GPIOE_PIN10) |                                \
                                         PIN_MODE_INPUT(GPIOE_PIN11) |                                \
                                         PIN_MODE_INPUT(GPIOE_PIN12) |                                \
                                         PIN_MODE_INPUT(GPIOE_PIN13) |                                \
                                         PIN_MODE_INPUT(GPIOE_PIN14) |                                \
                                         PIN_MODE_INPUT(GPIOE_PIN15))
#define VAL_GPIOE_OTYPER                (PIN_OTYPE_PUSHPULL(GPIOE_PIN0) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOE_PIN1) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOE_PIN2) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOE_PIN3) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOE_PIN4) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOE_PIN5) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOE_PIN6) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOE_PIN7) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOE_PIN8) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOE_PIN9) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOE_PIN10) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOE_PIN11) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOE_PIN12) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOE_PIN13) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOE_PIN14) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOE_PIN15))
#define VAL_GPIOE_OSPEEDR               (PIN_OSPEED_LOW(GPIOE_PIN0) |                                 \
                                         PIN_OSPEED_LOW(GPIOE_PIN1) |                                 \
                                         PIN_OSPEED_LOW(GPIOE_PIN2) |                                 \
                                         PIN_OSPEED_LOW(GPIOE_PIN3) |                                 \
                                         PIN_OSPEED_LOW(GPIOE_PIN4) |                                 \
                                         PIN_OSPEED_LOW(GPIOE_PIN5) |                                 \
                                         PIN_OSPEED_LOW(GPIOE_PIN6) |                                 \
                                         PIN_OSPEED_LOW(GPIOE_PIN7) |                                 \
                                         PIN_OSPEED_LOW(GPIOE_PIN8) |                                 \
                                         PIN_OSPEED_LOW(GPIOE_PIN9) |                                 \
                                         PIN_OSPEED_LOW(GPIOE_PIN10) |                                \
                                         PIN_OSPEED_LOW(GPIOE_PIN11) |                                \
                                         PIN_OSPEED_LOW(GPIOE_PIN12) |                                \
                                         PIN_OSPEED_LOW(GPIOE_PIN13) |                                \
                                         PIN_OSPEED_LOW(GPIOE_PIN14) |                                \
                                         PIN_OSPEED_LOW(GPIOE_PIN15))
#define VAL_GPIOE_PUPDR                 (PIN_PUPDR_FLOATING(GPIOE_PIN0) |                             \
                                         PIN_PUPDR_FLOATING(GPIOE_PIN1) |                             \
                                         PIN_PUPDR_FLOATING(GPIOE_PIN2) |                             \
                                         PIN_PUPDR_FLOATING(GPIOE_PIN3) |                             \
                                         PIN_PUPDR_FLOATING(GPIOE_PIN4) |                             \
                                         PIN_PUPDR_FLOATING(GPIOE_PIN5) |                             \
                                         PIN_PUPDR_FLOATING(GPIOE_PIN6) |                             \
                                         PIN_PUPDR_FLOATING(GPIOE_PIN7) |                             \
                                         PIN_PUPDR_FLOATING(GPIOE_PIN8) |                             \
                                         PIN_PUPDR_FLOATING(GPIOE_PIN9) |                             \
                                         PIN_PUPDR_FLOATING(GPIOE_PIN10) |                            \
                                         PIN_PUPDR_FLOATING(GPIOE_PIN11) |                            \
                                         PIN_PUPDR_FLOATING(GPIOE_PIN12) |                            \
                                         PIN_PUPDR_FLOATING(GPIOE_PIN13) |                            \
                                         PIN_PUPDR_FLOATING(GPIOE_PIN14) |                            \
                                         PIN_PUPDR_FLOATING(GPIOE_PIN15))
#define VAL_GPIOE_ODR                   (PIN_ODR_LOW(GPIOE_PIN0) |                                    \
                                         PIN_ODR_LOW(GPIOE_PIN1) |                                    \
                                         PIN_ODR_LOW(GPIOE_PIN2) |                                    \
                                         PIN_ODR_LOW(GPIOE_PIN3) |                                    \
                                         PIN_ODR_LOW(GPIOE_PIN4) |                                    \
                                         PIN_ODR_LOW(GPIOE_PIN5) |                                    \
                                         PIN_ODR_LOW(GPIOE_PIN6) |                                    \
                                         PIN_ODR_LOW(GPIOE_PIN7) |                                    \
                                         PIN_ODR_LOW(GPIOE_PIN8) |                                    \
                                         PIN_ODR_LOW(GPIOE_PIN9) |                                    \
                                         PIN_ODR_LOW(GPIOE_PIN10) |                                   \
                                         PIN_ODR_LOW(GPIOE_PIN11) |                                   \
                                         PIN_ODR_LOW(GPIOE_PIN12) |                                   \
                                         PIN_ODR_LOW(GPIOE_PIN13) |                                   \
                                         PIN_ODR_LOW(GPIOE_PIN14) |                                   \
                                         PIN_ODR_LOW(GPIOE_PIN15))
#define VAL_GPIOE_AFRL                  (PIN_AFIO_AF(GPIOE_PIN0, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOE_PIN1, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOE_PIN2, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOE_PIN3, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOE_PIN4, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOE_PIN5, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOE_PIN6, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOE_PIN7, STM32F4xx_AF_system))
#define VAL_GPIOE_AFRH                  (PIN_AFIO_AF(GPIOE_PIN8, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOE_PIN9, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOE_PIN10, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOE_PIN11, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOE_PIN12, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOE_PIN13, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOE_PIN14, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOE_PIN15, STM32F4xx_AF_system))

/*
 * GPIOF setup:
 *
 * PF0  - PIN0                          (input floating)
 * PF1  - PIN1                          (input floating)
 * PF2  - PIN2                          (input floating)
 * PF3  - PIN3                          (input floating)
 * PF4  - PIN4                          (input floating)
 * PF5  - PIN5                          (input floating)
 * PF6  - PIN6                          (input floating)
 * PF7  - PIN7                          (input floating)
 * PF8  - PIN8                          (input floating)
 * PF9  - PIN9                          (input floating)
 * PF10 - PIN10                         (input floating)
 * PF11 - PIN11                         (input floating)
 * PF12 - PIN12                         (input floating)
 * PF13 - PIN13                         (input floating)
 * PF14 - PIN14                         (input floating)
 * PF15 - PIN15                         (input floating)
 */
#define VAL_GPIOF_MODER                 (PIN_MODE_INPUT(GPIOF_PIN0) |                                 \
                                         PIN_MODE_INPUT(GPIOF_PIN1) |                                 \
                                         PIN_MODE_INPUT(GPIOF_PIN2) |                                 \
                                         PIN_MODE_INPUT(GPIOF_PIN3) |                                 \
                                         PIN_MODE_INPUT(GPIOF_PIN4) |                                 \
                                         PIN_MODE_INPUT(GPIOF_PIN5) |                                 \
                                         PIN_MODE_INPUT(GPIOF_PIN6) |                                 \
                                         PIN_MODE_INPUT(GPIOF_PIN7) |                                 \
                                         PIN_MODE_INPUT(GPIOF_PIN8) |                                 \
                                         PIN_MODE_INPUT(GPIOF_PIN9) |                                 \
                                         PIN_MODE_INPUT(GPIOF_PIN10) |                                \
                                         PIN_MODE_INPUT(GPIOF_PIN11) |                                \
                                         PIN_MODE_INPUT(GPIOF_PIN12) |                                \
                                         PIN_MODE_INPUT(GPIOF_PIN13) |                                \
                                         PIN_MODE_INPUT(GPIOF_PIN14) |                                \
                                         PIN_MODE_INPUT(GPIOF_PIN15))
#define VAL_GPIOF_OTYPER                (PIN_OTYPE_PUSHPULL(GPIOF_PIN0) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOF_PIN1) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOF_PIN2) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOF_PIN3) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOF_PIN4) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOF_PIN5) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOF_PIN6) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOF_PIN7) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOF_PIN8) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOF_PIN9) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOF_PIN10) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOF_PIN11) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOF_PIN12) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOF_PIN13) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOF_PIN14) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOF_PIN15))
#define VAL_GPIOF_OSPEEDR               (PIN_OSPEED_LOW(GPIOF_PIN0) |                                 \
                                         PIN_OSPEED_LOW(GPIOF_PIN1) |                                 \
                                         PIN_OSPEED_LOW(GPIOF_PIN2) |                                 \
                                         PIN_OSPEED_LOW(GPIOF_PIN3) |                                 \
                                         PIN_OSPEED_LOW(GPIOF_PIN4) |                                 \
                                         PIN_OSPEED_LOW(GPIOF_PIN5) |                                 \
                                         PIN_OSPEED_LOW(GPIOF_PIN6) |                                 \
                                         PIN_OSPEED_LOW(GPIOF_PIN7) |                                 \
                                         PIN_OSPEED_LOW(GPIOF_PIN8) |                                 \
                                         PIN_OSPEED_LOW(GPIOF_PIN9) |                                 \
                                         PIN_OSPEED_LOW(GPIOF_PIN10) |                                \
                                         PIN_OSPEED_LOW(GPIOF_PIN11) |                                \
                                         PIN_OSPEED_LOW(GPIOF_PIN12) |                                \
                                         PIN_OSPEED_LOW(GPIOF_PIN13) |                                \
                                         PIN_OSPEED_LOW(GPIOF_PIN14) |                                \
                                         PIN_OSPEED_LOW(GPIOF_PIN15))
#define VAL_GPIOF_PUPDR                 (PIN_PUPDR_FLOATING(GPIOF_PIN0) |                             \
                                         PIN_PUPDR_FLOATING(GPIOF_PIN1) |                             \
                                         PIN_PUPDR_FLOATING(GPIOF_PIN2) |                             \
                                         PIN_PUPDR_FLOATING(GPIOF_PIN3) |                             \
                                         PIN_PUPDR_FLOATING(GPIOF_PIN4) |                             \
                                         PIN_PUPDR_FLOATING(GPIOF_PIN5) |                             \
                                         PIN_PUPDR_FLOATING(GPIOF_PIN6) |                             \
                                         PIN_PUPDR_FLOATING(GPIOF_PIN7) |                             \
                                         PIN_PUPDR_FLOATING(GPIOF_PIN8) |                             \
                                         PIN_PUPDR_FLOATING(GPIOF_PIN9) |                             \
                                         PIN_PUPDR_FLOATING(GPIOF_PIN10) |                            \
                                         PIN_PUPDR_FLOATING(GPIOF_PIN11) |                            \
                                         PIN_PUPDR_FLOATING(GPIOF_PIN12) |                            \
                                         PIN_PUPDR_FLOATING(GPIOF_PIN13) |                            \
                                         PIN_PUPDR_FLOATING(GPIOF_PIN14) |                            \
                                         PIN_PUPDR_FLOATING(GPIOF_PIN15))
#define VAL_GPIOF_ODR                   (PIN_ODR_LOW(GPIOF_PIN0) |                                    \
                                         PIN_ODR_LOW(GPIOF_PIN1) |                                    \
                                         PIN_ODR_LOW(GPIOF_PIN2) |                                    \
                                         PIN_ODR_LOW(GPIOF_PIN3) |                                    \
                                         PIN_ODR_LOW(GPIOF_PIN4) |                                    \
                                         PIN_ODR_LOW(GPIOF_PIN5) |                                    \
                                         PIN_ODR_LOW(GPIOF_PIN6) |                                    \
                                         PIN_ODR_LOW(GPIOF_PIN7) |                                    \
                                         PIN_ODR_LOW(GPIOF_PIN8) |                                    \
                                         PIN_ODR_LOW(GPIOF_PIN9) |                                    \
                                         PIN_ODR_LOW(GPIOF_PIN10) |                                   \
                                         PIN_ODR_LOW(GPIOF_PIN11) |                                   \
                                         PIN_ODR_LOW(GPIOF_PIN12) |                                   \
                                         PIN_ODR_LOW(GPIOF_PIN13) |                                   \
                                         PIN_ODR_LOW(GPIOF_PIN14) |                                   \
                                         PIN_ODR_LOW(GPIOF_PIN15))
#define VAL_GPIOF_AFRL                  (PIN_AFIO_AF(GPIOF_PIN0, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOF_PIN1, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOF_PIN2, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOF_PIN3, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOF_PIN4, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOF_PIN5, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOF_PIN6, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOF_PIN7, STM32F4xx_AF_system))
#define VAL_GPIOF_AFRH                  (PIN_AFIO_AF(GPIOF_PIN8, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOF_PIN9, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOF_PIN10, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOF_PIN11, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOF_PIN12, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOF_PIN13, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOF_PIN14, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOF_PIN15, STM32F4xx_AF_system))

/*
 * GPIOG setup:
 *
 * PG0  - PIN0                          (input floating)
 * PG1  - PIN1                          (input floating)
 * PG2  - PIN2                          (input floating)
 * PG3  - PIN3                          (input floating)
 * PG4  - PIN4                          (input floating)
 * PG5  - PIN5                          (input floating)
 * PG6  - PIN6                          (input floating)
 * PG7  - PIN7                          (input floating)
 * PG8  - PIN8                          (input floating)
 * PG9  - PIN9                          (input floating)
 * PG10 - PIN10                         (input floating)
 * PG11 - PIN11                         (input floating)
 * PG12 - PIN12                         (input floating)
 * PG13 - PIN13                         (input floating)
 * PG14 - PIN14                         (input floating)
 * PG15 - PIN15                         (input floating)
 */
#define VAL_GPIOG_MODER                 (PIN_MODE_INPUT(GPIOG_PIN0) |                                 \
                                         PIN_MODE_INPUT(GPIOG_PIN1) |                                 \
                                         PIN_MODE_INPUT(GPIOG_PIN2) |                                 \
                                         PIN_MODE_INPUT(GPIOG_PIN3) |                                 \
                                         PIN_MODE_INPUT(GPIOG_PIN4) |                                 \
                                         PIN_MODE_INPUT(GPIOG_PIN5) |                                 \
                                         PIN_MODE_INPUT(GPIOG_PIN6) |                                 \
                                         PIN_MODE_INPUT(GPIOG_PIN7) |                                 \
                                         PIN_MODE_INPUT(GPIOG_PIN8) |                                 \
                                         PIN_MODE_INPUT(GPIOG_PIN9) |                                 \
                                         PIN_MODE_INPUT(GPIOG_PIN10) |                                \
                                         PIN_MODE_INPUT(GPIOG_PIN11) |                                \
                                         PIN_MODE_INPUT(GPIOG_PIN12) |                                \
                                         PIN_MODE_INPUT(GPIOG_PIN13) |                                \
                                         PIN_MODE_INPUT(GPIOG_PIN14) |                                \
                                         PIN_MODE_INPUT(GPIOG_PIN15))
#define VAL_GPIOG_OTYPER                (PIN_OTYPE_PUSHPULL(GPIOG_PIN0) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOG_PIN1) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOG_PIN2) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOG_PIN3) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOG_PIN4) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOG_PIN5) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOG_PIN6) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOG_PIN7) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOG_PIN8) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOG_PIN9) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOG_PIN10) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOG_PIN11) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOG_PIN12) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOG_PIN13) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOG_PIN14) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOG_PIN15))
#define VAL_GPIOG_OSPEEDR               (PIN_OSPEED_LOW(GPIOG_PIN0) |                                 \
                                         PIN_OSPEED_LOW(GPIOG_PIN1) |                                 \
                                         PIN_OSPEED_LOW(GPIOG_PIN2) |                                 \
                                         PIN_OSPEED_LOW(GPIOG_PIN3) |                                 \
                                         PIN_OSPEED_LOW(GPIOG_PIN4) |                                 \
                                         PIN_OSPEED_LOW(GPIOG_PIN5) |                                 \
                                         PIN_OSPEED_LOW(GPIOG_PIN6) |                                 \
                                         PIN_OSPEED_LOW(GPIOG_PIN7) |                                 \
                                         PIN_OSPEED_LOW(GPIOG_PIN8) |                                 \
                                         PIN_OSPEED_LOW(GPIOG_PIN9) |                                 \
                                         PIN_OSPEED_LOW(GPIOG_PIN10) |                                \
                                         PIN_OSPEED_LOW(GPIOG_PIN11) |                                \
                                         PIN_OSPEED_LOW(GPIOG_PIN12) |                                \
                                         PIN_OSPEED_LOW(GPIOG_PIN13) |                                \
                                         PIN_OSPEED_LOW(GPIOG_PIN14) |                                \
                                         PIN_OSPEED_LOW(GPIOG_PIN15))
#define VAL_GPIOG_PUPDR                 (PIN_PUPDR_FLOATING(GPIOG_PIN0) |                             \
                                         PIN_PUPDR_FLOATING(GPIOG_PIN1) |                             \
                                         PIN_PUPDR_FLOATING(GPIOG_PIN2) |                             \
                                         PIN_PUPDR_FLOATING(GPIOG_PIN3) |                             \
                                         PIN_PUPDR_FLOATING(GPIOG_PIN4) |                             \
                                         PIN_PUPDR_FLOATING(GPIOG_PIN5) |                             \
                                         PIN_PUPDR_FLOATING(GPIOG_PIN6) |                             \
                                         PIN_PUPDR_FLOATING(GPIOG_PIN7) |                             \
                                         PIN_PUPDR_FLOATING(GPIOG_PIN8) |                             \
                                         PIN_PUPDR_FLOATING(GPIOG_PIN9) |                             \
                                         PIN_PUPDR_FLOATING(GPIOG_PIN10) |                            \
                                         PIN_PUPDR_FLOATING(GPIOG_PIN11) |                            \
                                         PIN_PUPDR_FLOATING(GPIOG_PIN12) |                            \
                                         PIN_PUPDR_FLOATING(GPIOG_PIN13) |                            \
                                         PIN_PUPDR_FLOATING(GPIOG_PIN14) |                            \
                                         PIN_PUPDR_FLOATING(GPIOG_PIN15))
#define VAL_GPIOG_ODR                   (PIN_ODR_LOW(GPIOG_PIN0) |                                    \
                                         PIN_ODR_LOW(GPIOG_PIN1) |                                    \
                                         PIN_ODR_LOW(GPIOG_PIN2) |                                    \
                                         PIN_ODR_LOW(GPIOG_PIN3) |                                    \
                                         PIN_ODR_LOW(GPIOG_PIN4) |                                    \
                                         PIN_ODR_LOW(GPIOG_PIN5) |                                    \
                                         PIN_ODR_LOW(GPIOG_PIN6) |                                    \
                                         PIN_ODR_LOW(GPIOG_PIN7) |                                    \
                                         PIN_ODR_LOW(GPIOG_PIN8) |                                    \
                                         PIN_ODR_LOW(GPIOG_PIN9) |                                    \
                                         PIN_ODR_LOW(GPIOG_PIN10) |                                   \
                                         PIN_ODR_LOW(GPIOG_PIN11) |                                   \
                                         PIN_ODR_LOW(GPIOG_PIN12) |                                   \
                                         PIN_ODR_LOW(GPIOG_PIN13) |                                   \
                                         PIN_ODR_LOW(GPIOG_PIN14) |                                   \
                                         PIN_ODR_LOW(GPIOG_PIN15))
#define VAL_GPIOG_AFRL                  (PIN_AFIO_AF(GPIOG_PIN0, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOG_PIN1, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOG_PIN2, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOG_PIN3, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOG_PIN4, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOG_PIN5, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOG_PIN6, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOG_PIN7, STM32F4xx_AF_system))
#define VAL_GPIOG_AFRH                  (PIN_AFIO_AF(GPIOG_PIN8, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOG_PIN9, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOG_PIN10, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOG_PIN11, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOG_PIN12, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOG_PIN13, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOG_PIN14, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOG_PIN15, STM32F4xx_AF_system))

/*
 * GPIOH setup:
 *
 * PH0  - OSC_IN                        (input floating)
 * PH1  - OSC_OUT                       (input floating)
 * PH2  - PIN2                          (input floating)
 * PH3  - PIN3                          (input floating)
 * PH4  - PIN4                          (input floating)
 * PH5  - PIN5                          (input floating)
 * PH6  - PIN6                          (input floating)
 * PH7  - PIN7                          (input floating)
 * PH8  - PIN8                          (input floating)
 * PH9  - PIN9                          (input floating)
 * PH10 - PIN10                         (input floating)
 * PH11 - PIN11                         (input floating)
 * PH12 - PIN12                         (input floating)
 * PH13 - PIN13                         (input floating)
 * PH14 - PIN14                         (input floating)
 * PH15 - PIN15                         (input floating)
 */
#define VAL_GPIOH_MODER                 (PIN_MODE_INPUT(GPIOH_OSC_IN) |                               \
                                         PIN_MODE_INPUT(GPIOH_OSC_OUT) |                              \
                                         PIN_MODE_INPUT(GPIOH_PIN2) |                                 \
                                         PIN_MODE_INPUT(GPIOH_PIN3) |                                 \
                                         PIN_MODE_INPUT(GPIOH_PIN4) |                                 \
                                         PIN_MODE_INPUT(GPIOH_PIN5) |                                 \
                                         PIN_MODE_INPUT(GPIOH_PIN6) |                                 \
                                         PIN_MODE_INPUT(GPIOH_PIN7) |                                 \
                                         PIN_MODE_INPUT(GPIOH_PIN8) |                                 \
                                         PIN_MODE_INPUT(GPIOH_PIN9) |                                 \
                                         PIN_MODE_INPUT(GPIOH_PIN10) |                                \
                                         PIN_MODE_INPUT(GPIOH_PIN11) |                                \
                                         PIN_MODE_INPUT(GPIOH_PIN12) |                                \
                                         PIN_MODE_INPUT(GPIOH_PIN13) |                                \
                                         PIN_MODE_INPUT(GPIOH_PIN14) |                                \
                                         PIN_MODE_INPUT(GPIOH_PIN15))
#define VAL_GPIOH_OTYPER                (PIN_OTYPE_PUSHPULL(GPIOH_OSC_IN) |                           \
                                         PIN_OTYPE_PUSHPULL(GPIOH_OSC_OUT) |                          \
                                         PIN_OTYPE_PUSHPULL(GPIOH_PIN2) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOH_PIN3) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOH_PIN4) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOH_PIN5) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOH_PIN6) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOH_PIN7) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOH_PIN8) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOH_PIN9) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOH_PIN10) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOH_PIN11) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOH_PIN12) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOH_PIN13) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOH_PIN14) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOH_PIN15))
#define VAL_GPIOH_OSPEEDR               (PIN_OSPEED_VERYHIGH(GPIOH_OSC_IN) |                          \
                                         PIN_OSPEED_VERYHIGH(GPIOH_OSC_OUT) |                         \
                                         PIN_OSPEED_LOW(GPIOH_PIN2) |                                 \
                                         PIN_OSPEED_LOW(GPIOH_PIN3) |                                 \
                                         PIN_OSPEED_LOW(GPIOH_PIN4) |                                 \
                                         PIN_OSPEED_LOW(GPIOH_PIN5) |                                 \
                                         PIN_OSPEED_LOW(GPIOH_PIN6) |                                 \
                                         PIN_OSPEED_LOW(GPIOH_PIN7) |                                 \
                                         PIN_OSPEED_LOW(GPIOH_PIN8) |                                 \
                                         PIN_OSPEED_LOW(GPIOH_PIN9) |                                 \
                                         PIN_OSPEED_LOW(GPIOH_PIN10) |                                \
                                         PIN_OSPEED_LOW(GPIOH_PIN11) |                                \
                                         PIN_OSPEED_LOW(GPIOH_PIN12) |                                \
                                         PIN_OSPEED_LOW(GPIOH_PIN13) |                                \
                                         PIN_OSPEED_LOW(GPIOH_PIN14) |                                \
                                         PIN_OSPEED_LOW(GPIOH_PIN15))
#define VAL_GPIOH_PUPDR                 (PIN_PUPDR_FLOATING(GPIOH_OSC_IN) |                           \
                                         PIN_PUPDR_FLOATING(GPIOH_OSC_OUT) |                          \
                                         PIN_PUPDR_FLOATING(GPIOH_PIN2) |                             \
                                         PIN_PUPDR_FLOATING(GPIOH_PIN3) |                             \
                                         PIN_PUPDR_FLOATING(GPIOH_PIN4) |                             \
                                         PIN_PUPDR_FLOATING(GPIOH_PIN5) |                             \
                                         PIN_PUPDR_FLOATING(GPIOH_PIN6) |                             \
                                         PIN_PUPDR_FLOATING(GPIOH_PIN7) |                             \
                                         PIN_PUPDR_FLOATING(GPIOH_PIN8) |                             \
                                         PIN_PUPDR_FLOATING(GPIOH_PIN9) |                             \
                                         PIN_PUPDR_FLOATING(GPIOH_PIN10) |                            \
                                         PIN_PUPDR_FLOATING(GPIOH_PIN11) |                            \
                                         PIN_PUPDR_FLOATING(GPIOH_PIN12) |                            \
                                         PIN_PUPDR_FLOATING(GPIOH_PIN13) |                            \
                                         PIN_PUPDR_FLOATING(GPIOH_PIN14) |                            \
                                         PIN_PUPDR_FLOATING(GPIOH_PIN15))
#define VAL_GPIOH_ODR                   (PIN_ODR_HIGH(GPIOH_OSC_IN) |                                 \
                                         PIN_ODR_HIGH(GPIOH_OSC_OUT) |                                \
                                         PIN_ODR_HIGH(GPIOH_PIN2) |                                   \
                                         PIN_ODR_HIGH(GPIOH_PIN3) |                                   \
                                         PIN_ODR_HIGH(GPIOH_PIN4) |                                   \
                                         PIN_ODR_HIGH(GPIOH_PIN5) |                                   \
                                         PIN_ODR_HIGH(GPIOH_PIN6) |                                   \
                                         PIN_ODR_HIGH(GPIOH_PIN7) |                                   \
                                         PIN_ODR_HIGH(GPIOH_PIN8) |                                   \
                                         PIN_ODR_HIGH(GPIOH_PIN9) |                                   \
                                         PIN_ODR_HIGH(GPIOH_PIN10) |                                  \
                                         PIN_ODR_HIGH(GPIOH_PIN11) |                                  \
                                         PIN_ODR_HIGH(GPIOH_PIN12) |                                  \
                                         PIN_ODR_HIGH(GPIOH_PIN13) |                                  \
                                         PIN_ODR_HIGH(GPIOH_PIN14) |                                  \
                                         PIN_ODR_HIGH(GPIOH_PIN15))
#define VAL_GPIOH_AFRL                  (PIN_AFIO_AF(GPIOH_OSC_IN, STM32F4xx_AF_system) |             \
                                         PIN_AFIO_AF(GPIOH_OSC_OUT, STM32F4xx_AF_system) |            \
                                         PIN_AFIO_AF(GPIOH_PIN2, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOH_PIN3, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOH_PIN4, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOH_PIN5, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOH_PIN6, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOH_PIN7, STM32F4xx_AF_system))
#define VAL_GPIOH_AFRH                  (PIN_AFIO_AF(GPIOH_PIN8, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOH_PIN9, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOH_PIN10, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOH_PIN11, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOH_PIN12, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOH_PIN13, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOH_PIN14, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOH_PIN15, STM32F4xx_AF_system))

/*
 * GPIOI setup:
 *
 * PI0  - PIN0                          (input floating)
 * PI1  - PIN1                          (input floating)
 * PI2  - PIN2                          (input floating)
 * PI3  - PIN3                          (input floating)
 * PI4  - PIN4                          (input floating)
 * PI5  - PIN5                          (input floating)
 * PI6  - PIN6                          (input floating)
 * PI7  - PIN7                          (input floating)
 * PI8  - PIN8                          (input floating)
 * PI9  - PIN9                          (input floating)
 * PI10 - PIN10                         (input floating)
 * PI11 - PIN11                         (input floating)
 * PI12 - PIN12                         (input floating)
 * PI13 - PIN13                         (input floating)
 * PI14 - PIN14                         (input floating)
 * PI15 - PIN15                         (input floating)
 */
#define VAL_GPIOI_MODER                 (PIN_MODE_INPUT(GPIOI_PIN0) |                                 \
                                         PIN_MODE_INPUT(GPIOI_PIN1) |                                 \
                                         PIN_MODE_INPUT(GPIOI_PIN2) |                                 \
                                         PIN_MODE_INPUT(GPIOI_PIN3) |                                 \
                                         PIN_MODE_INPUT(GPIOI_PIN4) |                                 \
                                         PIN_MODE_INPUT(GPIOI_PIN5) |                                 \
                                         PIN_MODE_INPUT(GPIOI_PIN6) |                                 \
                                         PIN_MODE_INPUT(GPIOI_PIN7) |                                 \
                                         PIN_MODE_INPUT(GPIOI_PIN8) |                                 \
                                         PIN_MODE_INPUT(GPIOI_PIN9) |                                 \
                                         PIN_MODE_INPUT(GPIOI_PIN10) |                                \
                                         PIN_MODE_INPUT(GPIOI_PIN11) |                                \
                                         PIN_MODE_INPUT(GPIOI_PIN12) |                                \
                                         PIN_MODE_INPUT(GPIOI_PIN13) |                                \
                                         PIN_MODE_INPUT(GPIOI_PIN14) |                                \
                                         PIN_MODE_INPUT(GPIOI_PIN15))
#define VAL_GPIOI_OTYPER                (PIN_OTYPE_PUSHPULL(GPIOI_PIN0) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOI_PIN1) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOI_PIN2) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOI_PIN3) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOI_PIN4) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOI_PIN5) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOI_PIN6) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOI_PIN7) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOI_PIN8) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOI_PIN9) |                             \
                                         PIN_OTYPE_PUSHPULL(GPIOI_PIN10) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOI_PIN11) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOI_PIN12) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOI_PIN13) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOI_PIN14) |                            \
                                         PIN_OTYPE_PUSHPULL(GPIOI_PIN15))
#define VAL_GPIOI_OSPEEDR               (PIN_OSPEED_LOW(GPIOI_PIN0) |                                 \
                                         PIN_OSPEED_LOW(GPIOI_PIN1) |                                 \
                                         PIN_OSPEED_LOW(GPIOI_PIN2) |                                 \
                                         PIN_OSPEED_LOW(GPIOI_PIN3) |                                 \
                                         PIN_OSPEED_LOW(GPIOI_PIN4) |                                 \
                                         PIN_OSPEED_LOW(GPIOI_PIN5) |                                 \
                                         PIN_OSPEED_LOW(GPIOI_PIN6) |                                 \
                                         PIN_OSPEED_LOW(GPIOI_PIN7) |                                 \
                                         PIN_OSPEED_LOW(GPIOI_PIN8) |                                 \
                                         PIN_OSPEED_LOW(GPIOI_PIN9) |                                 \
                                         PIN_OSPEED_LOW(GPIOI_PIN10) |                                \
                                         PIN_OSPEED_LOW(GPIOI_PIN11) |                                \
                                         PIN_OSPEED_LOW(GPIOI_PIN12) |                                \
                                         PIN_OSPEED_LOW(GPIOI_PIN13) |                                \
                                         PIN_OSPEED_LOW(GPIOI_PIN14) |                                \
                                         PIN_OSPEED_LOW(GPIOI_PIN15))
#define VAL_GPIOI_PUPDR                 (PIN_PUPDR_FLOATING(GPIOI_PIN0) |                             \
                                         PIN_PUPDR_FLOATING(GPIOI_PIN1) |                             \
                                         PIN_PUPDR_FLOATING(GPIOI_PIN2) |                             \
                                         PIN_PUPDR_FLOATING(GPIOI_PIN3) |                             \
                                         PIN_PUPDR_FLOATING(GPIOI_PIN4) |                             \
                                         PIN_PUPDR_FLOATING(GPIOI_PIN5) |                             \
                                         PIN_PUPDR_FLOATING(GPIOI_PIN6) |                             \
                                         PIN_PUPDR_FLOATING(GPIOI_PIN7) |                             \
                                         PIN_PUPDR_FLOATING(GPIOI_PIN8) |                             \
                                         PIN_PUPDR_FLOATING(GPIOI_PIN9) |                             \
                                         PIN_PUPDR_FLOATING(GPIOI_PIN10) |                            \
                                         PIN_PUPDR_FLOATING(GPIOI_PIN11) |                            \
                                         PIN_PUPDR_FLOATING(GPIOI_PIN12) |                            \
                                         PIN_PUPDR_FLOATING(GPIOI_PIN13) |                            \
                                         PIN_PUPDR_FLOATING(GPIOI_PIN14) |                            \
                                         PIN_PUPDR_FLOATING(GPIOI_PIN15))
#define VAL_GPIOI_ODR                   (PIN_ODR_LOW(GPIOI_PIN0) |                                    \
                                         PIN_ODR_LOW(GPIOI_PIN1) |                                    \
                                         PIN_ODR_LOW(GPIOI_PIN2) |                                    \
                                         PIN_ODR_LOW(GPIOI_PIN3) |                                    \
                                         PIN_ODR_LOW(GPIOI_PIN4) |                                    \
                                         PIN_ODR_LOW(GPIOI_PIN5) |                                    \
                                         PIN_ODR_LOW(GPIOI_PIN6) |                                    \
                                         PIN_ODR_LOW(GPIOI_PIN7) |                                    \
                                         PIN_ODR_LOW(GPIOI_PIN8) |                                    \
                                         PIN_ODR_LOW(GPIOI_PIN9) |                                    \
                                         PIN_ODR_LOW(GPIOI_PIN10) |                                   \
                                         PIN_ODR_LOW(GPIOI_PIN11) |                                   \
                                         PIN_ODR_LOW(GPIOI_PIN12) |                                   \
                                         PIN_ODR_LOW(GPIOI_PIN13) |                                   \
                                         PIN_ODR_LOW(GPIOI_PIN14) |                                   \
                                         PIN_ODR_LOW(GPIOI_PIN15))
#define VAL_GPIOI_AFRL                  (PIN_AFIO_AF(GPIOI_PIN0, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOI_PIN1, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOI_PIN2, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOI_PIN3, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOI_PIN4, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOI_PIN5, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOI_PIN6, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOI_PIN7, STM32F4xx_AF_system))
#define VAL_GPIOI_AFRH                  (PIN_AFIO_AF(GPIOI_PIN8, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOI_PIN9, STM32F4xx_AF_system) |               \
                                         PIN_AFIO_AF(GPIOI_PIN10, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOI_PIN11, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOI_PIN12, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOI_PIN13, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOI_PIN14, STM32F4xx_AF_system) |              \
                                         PIN_AFIO_AF(GPIOI_PIN15, STM32F4xx_AF_system))

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
