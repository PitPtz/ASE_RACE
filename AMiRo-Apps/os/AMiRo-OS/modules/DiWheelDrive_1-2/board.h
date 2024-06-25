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
 * @brief   DiWheeDrive v1.2 Board specific macros.
 *
 * @defgroup diwheeldrive12_cfg_board Board
 * @ingroup diwheeldrive12_cfg
 *
 * @brief   Todo
 * @details Todo
 *
 * @addtogroup diwheeldrive12_cfg_board
 * @{
 */

#ifndef BOARD_H
#define BOARD_H

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*
 * Setup for AMiRo DiWheelDrive v1.2 board.
 */

/*
 * Board identifier.
 */
#define BOARD_DIWHEELDRIVE_1_2
#define BOARD_NAME              "AMiRo DiWheelDrive v1.2"

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
 * Identifiers for the several breakout boards, which can be attached to the PowerManagement v1.2 module.
 */
#define BOARD_BREAKOUT_NONE                -1
#define BOARD_BREAKOUT_ESP               1

/* sanity check */
#if !defined(BOARD_BREAKOUT_MODULE)
# error "BOARD_BREAKOUT_MODULE not defined."
#elif ((BOARD_BREAKOUT_MODULE != BOARD_BREAKOUT_NONE) && \
       (BOARD_BREAKOUT_MODULE != BOARD_BREAKOUT_ESP))
# error "BOARD_BREAKOUT_MODULE set to invalid value"
#endif

#endif /* !defined(_FROM_ASM_) */

/*
 * IO pins assignments.
 */
#define GPIOA_WKUP                  0U
#define GPIOA_LED                   1U
#define GPIOA_DRIVE_PWM1A           2U
#define GPIOA_DRIVE_PWM1B           3U
#define GPIOA_PIN4                  4U
#define GPIOA_PIN5                  5U
#define GPIOA_PIN6                  6U
#define GPIOA_PIN7                  7U
#define GPIOA_PIN8                  8U
#define GPIOA_PROG_RX               9U
#define GPIOA_PROG_TX               10U
#define GPIOA_CAN_RX                11U
#define GPIOA_CAN_TX                12U
#define GPIOA_SWDIO                 13U
#define GPIOA_SWCLK                 14U
#define GPIOA_DRIVE_PWM2B           15U

#define GPIOB_PIN0                  0U
#define GPIOB_DRIVE_SENSE2          1U
#define GPIOB_POWER_EN              2U
#define GPIOB_DRIVE_PWM2A           3U
#define GPIOB_PIN4                  4U
#define GPIOB_PIN5                  5U
#define GPIOB_DRIVE_ENC1A           6U
#define GPIOB_DRIVE_ENC1B           7U
#define GPIOB_IMU_SCL               8U
#define GPIOB_IMU_SDA               9U
#define GPIOB_IR_SCL                10U
#define GPIOB_IR_SDA                11U
#define GPIOB_IR_INT                12U
#define GPIOB_PIN13                 13U
#define GPIOB_SYS_UART_UP           14U
#define GPIOB_IMU_INT               15U

#define GPIOC_DRIVE_SENSE1          0U
#define GPIOC_SYS_INT_N             1U
#define GPIOC_IMU_RESET_N           2U
#define GPIOC_PATH_DCSTAT           3U
#define GPIOC_PIN4                  4U
#define GPIOC_PATH_DCEN             5U
#define GPIOC_DRIVE_ENC2B           6U
#define GPIOC_DRIVE_ENC2A           7U
#define GPIOC_SYS_PD_N              8U
#define GPIOC_SYS_REG_EN            9U
#define GPIOC_SYS_UART_RX           10U
#define GPIOC_SYS_UART_TX           11U
#define GPIOC_IMU_BOOT_LOAD_N       12U
#define GPIOC_PIN13                 13U
#define GPIOC_PIN14                 14U
#define GPIOC_IMU_BL_IND            15U

#define GPIOD_OSC_IN                0U
#define GPIOD_OSC_OUT               1U
#define GPIOD_SYS_WARMRST_N         2U
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
#define LINE_WKUP                   PAL_LINE(GPIOA, GPIOA_WKUP)
#define LINE_LED                    PAL_LINE(GPIOA, GPIOA_LED)
#define LINE_DRIVE_PWM1A            PAL_LINE(GPIOA, GPIOA_DRIVE_PWM1A)
#define LINE_DRIVE_PWM1B            PAL_LINE(GPIOA, GPIOA_DRIVE_PWM1B)
#define LINE_PROG_RX                PAL_LINE(GPIOA, GPIOA_PROG_RX)
#define LINE_PROG_TX                PAL_LINE(GPIOA, GPIOA_PROG_TX)
#define LINE_CAN_RX                 PAL_LINE(GPIOA, GPIOA_CAN_RX)
#define LINE_CAN_TX                 PAL_LINE(GPIOA, GPIOA_CAN_TX)
#define LINE_SWDIO                  PAL_LINE(GPIOA, GPIOA_SWDIO)
#define LINE_SWCLK                  PAL_LINE(GPIOA, GPIOA_SWCLK)
#define LINE_DRIVE_PWM2B            PAL_LINE(GPIOA, GPIOA_DRIVE_PWM2B)

#define LINE_DRIVE_SENSE2           PAL_LINE(GPIOB, GPIOB_DRIVE_SENSE2)
#define LINE_POWER_EN               PAL_LINE(GPIOB, GPIOB_POWER_EN)
#define LINE_DRIVE_PWM2A            PAL_LINE(GPIOB, GPIOB_DRIVE_PWM2A)
#define LINE_DRIVE_ENC1A            PAL_LINE(GPIOB, GPIOB_DRIVE_ENC1A)
#define LINE_DRIVE_ENC1B            PAL_LINE(GPIOB, GPIOB_DRIVE_ENC1B)
#define LINE_IMU_SCL                PAL_LINE(GPIOB, GPIOB_IMU_SCL)
#define LINE_IMU_SDA                PAL_LINE(GPIOB, GPIOB_IMU_SDA)
#define LINE_IR_SCL                 PAL_LINE(GPIOB, GPIOB_IR_SCL)
#define LINE_IR_SDA                 PAL_LINE(GPIOB, GPIOB_IR_SDA)
#define LINE_IR_INT                 PAL_LINE(GPIOB, GPIOB_IR_INT)
#define LINE_SYS_UART_UP            PAL_LINE(GPIOB, GPIOB_SYS_UART_UP)
#define LINE_IMU_INT                PAL_LINE(GPIOB, GPIOB_IMU_INT)

#define LINE_DRIVE_SENSE1           PAL_LINE(GPIOC, GPIOC_DRIVE_SENSE1)
#define LINE_SYS_INT_N              PAL_LINE(GPIOC, GPIOC_SYS_INT_N)
#define LINE_IMU_RESET_N            PAL_LINE(GPIOC, GPIOC_IMU_RESET_N)
#define LINE_PATH_DCSTAT            PAL_LINE(GPIOC, GPIOC_PATH_DCSTAT)
#define LINE_PATH_DCEN              PAL_LINE(GPIOC, GPIOC_PATH_DCEN)
#define LINE_DRIVE_ENC2B            PAL_LINE(GPIOC, GPIOC_DRIVE_ENC2B)
#define LINE_DRIVE_ENC2A            PAL_LINE(GPIOC, GPIOC_DRIVE_ENC2A)
#define LINE_SYS_PD_N               PAL_LINE(GPIOC, GPIOC_SYS_PD_N)
#define LINE_SYS_REG_EN             PAL_LINE(GPIOC, GPIOC_SYS_REG_EN)
#define LINE_SYS_UART_RX            PAL_LINE(GPIOC, GPIOC_SYS_UART_RX)
#define LINE_SYS_UART_TX            PAL_LINE(GPIOC, GPIOC_SYS_UART_TX)
#define LINE_IMU_BOOT_LOAD_N        PAL_LINE(GPIOC, GPIOC_IMU_BOOT_LOAD_N)
#define LINE_IMU_BL_IND             PAL_LINE(GPIOC, GPIOC_IMU_BL_IND)

#define LINE_OSC_IN                 PAL_LINE(GPIOD, GPIOD_OSC_IN)
#define LINE_OSC_OUT                PAL_LINE(GPIOD, GPIOD_OSC_OUT)
#define LINE_SYS_WARMRST_N          PAL_LINE(GPIOD, GPIOD_SYS_WARMRST_N)

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
 * PA0  - WKUP                      (input floating)
 * PA1  - LED                       (output opendrain high 50MHz)
 * PA2  - DRIVE_PWM1A               (alternate pushpull 50MHz)
 * PA3  - DRIVE_PWM1B               (alternate pushpull 50MHz)
 * PA4  - PIN4                      (input floating)
 * PA5  - PIN5                      (input floating)
 * PA6  - PIN6                      (input floating)
 * PA7  - PIN7                      (input floating)
 * PA8  - PIN8                      (input floating)
 * PA9  - PROG_RX                   (alternate pushpull 50MHz)
 * PA10 - PROG_TX                   (input pullup)
 * PA11 - CAN_RX                    (input pullup)
 * PA12 - CAN_TX                    (alternate pushpull 50MHz)
 * PA13 - SWDIO                     (input pullup)
 * PA14 - SWCLK                     (input pullup)
 * PA15 - DRIVE_PWM2B               (alternate pushpull 50MHz)
 */
#define VAL_GPIOAIGN                (PIN_IGNORE(GPIOA_LED)) & 0
#define VAL_GPIOACRL                (PIN_CR(GPIOA_WKUP, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOA_LED, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_OPENDRAIN) |              \
                                     PIN_CR(GPIOA_DRIVE_PWM1A, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL) |    \
                                     PIN_CR(GPIOA_DRIVE_PWM1B, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL) |    \
                                     PIN_CR(GPIOA_PIN4, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOA_PIN5, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOA_PIN6, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOA_PIN7, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOACRH                (PIN_CR(GPIOA_PIN8, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOA_PROG_RX, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL) |        \
                                     PIN_CR(GPIOA_PROG_TX, PIN_MODE_INPUT, PIN_CNF_INPUT_PULLX) |                    \
                                     PIN_CR(GPIOA_CAN_RX, PIN_MODE_INPUT, PIN_CNF_INPUT_PULLX) |                     \
                                     PIN_CR(GPIOA_CAN_TX, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL) |         \
                                     PIN_CR(GPIOA_SWDIO, PIN_MODE_INPUT, PIN_CNF_INPUT_PULLX) |                      \
                                     PIN_CR(GPIOA_SWCLK, PIN_MODE_INPUT, PIN_CNF_INPUT_PULLX) |                      \
                                     PIN_CR(GPIOA_DRIVE_PWM2B, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL))
#define VAL_GPIOAODR                (PIN_ODR_HIGH(GPIOA_WKUP) |                                                      \
                                     PIN_ODR_HIGH(GPIOA_LED) |                                                       \
                                     PIN_ODR_HIGH(GPIOA_DRIVE_PWM1A) |                                               \
                                     PIN_ODR_HIGH(GPIOA_DRIVE_PWM1B) |                                               \
                                     PIN_ODR_LOW(GPIOA_PIN4) |                                                       \
                                     PIN_ODR_LOW(GPIOA_PIN5) |                                                       \
                                     PIN_ODR_LOW(GPIOA_PIN6) |                                                       \
                                     PIN_ODR_LOW(GPIOA_PIN7) |                                                       \
                                     PIN_ODR_LOW(GPIOA_PIN8) |                                                       \
                                     PIN_ODR_HIGH(GPIOA_PROG_RX) |                                                   \
                                     PIN_ODR_HIGH(GPIOA_PROG_TX) |                                                   \
                                     PIN_ODR_HIGH(GPIOA_CAN_RX) |                                                    \
                                     PIN_ODR_HIGH(GPIOA_CAN_TX) |                                                    \
                                     PIN_ODR_HIGH(GPIOA_SWDIO) |                                                     \
                                     PIN_ODR_HIGH(GPIOA_SWCLK) |                                                     \
                                     PIN_ODR_HIGH(GPIOA_DRIVE_PWM2B))

/*
 * GPIOB setup:
 *
 * PB0  - PIN0                      (input floating)
 * PB1  - DRIVE_SENSE2              (input analog)
 * PB2  - POWER_EN                  (output pushpull low 50MHz)
 * PB3  - DRIVE_PWM2A               (alternate pushpull 50MHz)
 * PB4  - PIN4                      (input floating)
 * PB5  - PIN5                      (input floating)
 * PB6  - DRIVE_ENC1A               (input floating)
 * PB7  - DRIVE_ENC1B               (input floating)
 * PB8  - IMU_SCL                   (alternate opendrain 50MHz)
 * PB9  - IMU_SDA                   (alternate opendrain 50MHz)
 * PB10 - IR_SCL                    (alternate opendrain 50MHz)
 * PB11 - IR_SDA                    (alternate opendrain 50MHz)
 * PB12 - IR_INT                    (input floating)
 * PB13 - PIN13                     (input floating)
 * PB14 - SYS_UART_UP               (output opendrain high 50MHz)
 * PB15 - IMU_INT                   (input floating)
 */
#define VAL_GPIOBIGN                (PIN_IGNORE(GPIOB_SYS_UART_UP)) & 0
#define VAL_GPIOBCRL                (PIN_CR(GPIOB_PIN0, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOB_DRIVE_SENSE2, PIN_MODE_INPUT, PIN_CNF_INPUT_ANALOG) |              \
                                     PIN_CR(GPIOB_POWER_EN, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_PUSHPULL) |          \
                                     PIN_CR(GPIOB_DRIVE_PWM2A, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_PUSHPULL) |    \
                                     PIN_CR(GPIOB_PIN4, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOB_PIN5, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOB_DRIVE_ENC1A, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |             \
                                     PIN_CR(GPIOB_DRIVE_ENC1B, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOBCRH                (PIN_CR(GPIOB_IMU_SCL, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_OPENDRAIN) |       \
                                     PIN_CR(GPIOB_IMU_SDA, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_OPENDRAIN) |       \
                                     PIN_CR(GPIOB_IR_SCL, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_OPENDRAIN) |        \
                                     PIN_CR(GPIOB_IR_SDA, PIN_MODE_OUTPUT_50M, PIN_CNF_ALTERNATE_OPENDRAIN) |        \
                                     PIN_CR(GPIOB_IR_INT, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOB_PIN13, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOB_SYS_UART_UP, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_OPENDRAIN) |      \
                                     PIN_CR(GPIOB_IMU_INT, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOBODR                (PIN_ODR_LOW(GPIOB_PIN0) |                                                       \
                                     PIN_ODR_HIGH(GPIOB_DRIVE_SENSE2) |                                              \
                                     PIN_ODR_LOW(GPIOB_POWER_EN) |                                                   \
                                     PIN_ODR_HIGH(GPIOB_DRIVE_PWM2A) |                                               \
                                     PIN_ODR_LOW(GPIOB_PIN4) |                                                       \
                                     PIN_ODR_LOW(GPIOB_PIN5) |                                                       \
                                     PIN_ODR_HIGH(GPIOB_DRIVE_ENC1A) |                                               \
                                     PIN_ODR_HIGH(GPIOB_DRIVE_ENC1B) |                                               \
                                     PIN_ODR_HIGH(GPIOB_IMU_SCL) |                                                   \
                                     PIN_ODR_HIGH(GPIOB_IMU_SDA) |                                                   \
                                     PIN_ODR_HIGH(GPIOB_IR_SCL) |                                                    \
                                     PIN_ODR_HIGH(GPIOB_IR_SDA) |                                                    \
                                     PIN_ODR_HIGH(GPIOB_IR_INT) |                                                    \
                                     PIN_ODR_LOW(GPIOB_PIN13) |                                                      \
                                     PIN_ODR_HIGH(GPIOB_SYS_UART_UP) |                                               \
                                     PIN_ODR_LOW(GPIOB_IMU_INT))

/*
 * GPIOC setup:
 *
 * PC0  - DRIVE_SENSE1              (input analog)
 * PC1  - SYS_INT_N                 (output opendrain low 50MHz)
 * PC2  - IMU_RESET_N               (output opendrain high 50MHz)
 * PC3  - PATH_DCSTAT               (input floating)
 * PC4  - PIN4                      (input floating)
 * PC5  - PATH_DCEN                 (output pushpull low 50MHz)
 * PC6  - DRIVE_ENC2B               (input floating)
 * PC7  - DRIVE_ENC2A               (input floating)
 * PC8  - SYS_PD_N                  (output opendrain high 50MHz)
 * PC9  - SYS_REG_EN                (input floating)
 * PC10 - SYS_UART_RX               (input floating)
 * PC11 - SYS_UART_TX               (input floating)
 * PC12 - IMU_BOOT_LOAD_N           (output opendrain high 50MHz)
 * PC13 - PIN13                     (input floating)
 * PC14 - PIN14                     (input floating)
 * PC15 - IMU_BL_IND                (input floating)
 */
#define VAL_GPIOCIGN                (PIN_IGNORE(GPIOC_SYS_INT_N) |                                                   \
                                     PIN_IGNORE(GPIOC_SYS_PD_N)) & 0
#define VAL_GPIOCCRL                (PIN_CR(GPIOC_DRIVE_SENSE1, PIN_MODE_INPUT, PIN_CNF_INPUT_ANALOG) |              \
                                     PIN_CR(GPIOC_SYS_INT_N, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_OPENDRAIN) |        \
                                     PIN_CR(GPIOC_IMU_RESET_N, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_OPENDRAIN) |      \
                                     PIN_CR(GPIOC_PATH_DCSTAT, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |             \
                                     PIN_CR(GPIOC_PIN4, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOC_PATH_DCEN, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_PUSHPULL) |         \
                                     PIN_CR(GPIOC_DRIVE_ENC2B, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |             \
                                     PIN_CR(GPIOC_DRIVE_ENC2A, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOCCRH                (PIN_CR(GPIOC_SYS_PD_N, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_OPENDRAIN) |         \
                                     PIN_CR(GPIOC_SYS_REG_EN, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |              \
                                     PIN_CR(GPIOC_SYS_UART_RX, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |             \
                                     PIN_CR(GPIOC_SYS_UART_TX, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |             \
                                     PIN_CR(GPIOC_IMU_BOOT_LOAD_N, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_OPENDRAIN) |  \
                                     PIN_CR(GPIOC_PIN13, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOC_PIN14, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOC_IMU_BL_IND, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOCODR                (PIN_ODR_HIGH(GPIOC_DRIVE_SENSE1) |                                              \
                                     PIN_ODR_LOW(GPIOC_SYS_INT_N) |                                                  \
                                     PIN_ODR_HIGH(GPIOC_IMU_RESET_N) |                                               \
                                     PIN_ODR_HIGH(GPIOC_PATH_DCSTAT) |                                               \
                                     PIN_ODR_LOW(GPIOC_PIN4) |                                                       \
                                     PIN_ODR_HIGH(GPIOC_PATH_DCEN) |                                                 \
                                     PIN_ODR_HIGH(GPIOC_DRIVE_ENC2B) |                                               \
                                     PIN_ODR_HIGH(GPIOC_DRIVE_ENC2A) |                                               \
                                     PIN_ODR_HIGH(GPIOC_SYS_PD_N) |                                                  \
                                     PIN_ODR_HIGH(GPIOC_SYS_REG_EN) |                                                \
                                     PIN_ODR_HIGH(GPIOC_SYS_UART_RX) |                                               \
                                     PIN_ODR_HIGH(GPIOC_SYS_UART_TX) |                                               \
                                     PIN_ODR_HIGH(GPIOC_IMU_BOOT_LOAD_N) |                                           \
                                     PIN_ODR_LOW(GPIOC_PIN13) |                                                      \
                                     PIN_ODR_LOW(GPIOC_PIN14) |                                                      \
                                     PIN_ODR_LOW(GPIOC_IMU_BL_IND))

/*
 * GPIOD setup:
 *
 * PD0  - OSC_IN                    (input floating)
 * PD1  - OSC_OUT                   (input floating)
 * PD2  - SYS_WARMRST_N             (output opendrain high 50MHz)
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
#define VAL_GPIODIGN                0
#define VAL_GPIODCRL                (PIN_CR(GPIOD_OSC_IN, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                  \
                                     PIN_CR(GPIOD_OSC_OUT, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                 \
                                     PIN_CR(GPIOD_SYS_WARMRST_N, PIN_MODE_OUTPUT_50M, PIN_CNF_OUTPUT_OPENDRAIN) |    \
                                     PIN_CR(GPIOD_PIN3, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOD_PIN4, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOD_PIN5, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOD_PIN6, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOD_PIN7, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIODCRH                (PIN_CR(GPIOD_PIN8, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOD_PIN9, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOD_PIN10, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOD_PIN11, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOD_PIN12, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOD_PIN13, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOD_PIN14, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOD_PIN15, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIODODR                (PIN_ODR_HIGH(GPIOD_OSC_IN) |                                                    \
                                     PIN_ODR_HIGH(GPIOD_OSC_OUT) |                                                   \
                                     PIN_ODR_HIGH(GPIOD_SYS_WARMRST_N) |                                             \
                                     PIN_ODR_LOW(GPIOD_PIN3) |                                                       \
                                     PIN_ODR_LOW(GPIOD_PIN4) |                                                       \
                                     PIN_ODR_LOW(GPIOD_PIN5) |                                                       \
                                     PIN_ODR_LOW(GPIOD_PIN6) |                                                       \
                                     PIN_ODR_LOW(GPIOD_PIN7) |                                                       \
                                     PIN_ODR_LOW(GPIOD_PIN8) |                                                       \
                                     PIN_ODR_LOW(GPIOD_PIN9) |                                                       \
                                     PIN_ODR_LOW(GPIOD_PIN10) |                                                      \
                                     PIN_ODR_LOW(GPIOD_PIN11) |                                                      \
                                     PIN_ODR_LOW(GPIOD_PIN12) |                                                      \
                                     PIN_ODR_LOW(GPIOD_PIN13) |                                                      \
                                     PIN_ODR_LOW(GPIOD_PIN14) |                                                      \
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
#define VAL_GPIOECRL                (PIN_CR(GPIOE_PIN0, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOE_PIN1, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOE_PIN2, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOE_PIN3, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOE_PIN4, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOE_PIN5, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOE_PIN6, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOE_PIN7, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOECRH                (PIN_CR(GPIOE_PIN8, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOE_PIN9, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOE_PIN10, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOE_PIN11, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOE_PIN12, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOE_PIN13, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOE_PIN14, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOE_PIN15, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOEODR                (PIN_ODR_LOW(GPIOE_PIN0) |                                                       \
                                     PIN_ODR_LOW(GPIOE_PIN1) |                                                       \
                                     PIN_ODR_LOW(GPIOE_PIN2) |                                                       \
                                     PIN_ODR_LOW(GPIOE_PIN3) |                                                       \
                                     PIN_ODR_LOW(GPIOE_PIN4) |                                                       \
                                     PIN_ODR_LOW(GPIOE_PIN5) |                                                       \
                                     PIN_ODR_LOW(GPIOE_PIN6) |                                                       \
                                     PIN_ODR_LOW(GPIOE_PIN7) |                                                       \
                                     PIN_ODR_LOW(GPIOE_PIN8) |                                                       \
                                     PIN_ODR_LOW(GPIOE_PIN9) |                                                       \
                                     PIN_ODR_LOW(GPIOE_PIN10) |                                                      \
                                     PIN_ODR_LOW(GPIOE_PIN11) |                                                      \
                                     PIN_ODR_LOW(GPIOE_PIN12) |                                                      \
                                     PIN_ODR_LOW(GPIOE_PIN13) |                                                      \
                                     PIN_ODR_LOW(GPIOE_PIN14) |                                                      \
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
#define VAL_GPIOFCRL                (PIN_CR(GPIOF_PIN0, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOF_PIN1, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOF_PIN2, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOF_PIN3, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOF_PIN4, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOF_PIN5, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOF_PIN6, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOF_PIN7, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOFCRH                (PIN_CR(GPIOF_PIN8, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOF_PIN9, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOF_PIN10, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOF_PIN11, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOF_PIN12, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOF_PIN13, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOF_PIN14, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOF_PIN15, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOFODR                (PIN_ODR_LOW(GPIOF_PIN0) |                                                       \
                                     PIN_ODR_LOW(GPIOF_PIN1) |                                                       \
                                     PIN_ODR_LOW(GPIOF_PIN2) |                                                       \
                                     PIN_ODR_LOW(GPIOF_PIN3) |                                                       \
                                     PIN_ODR_LOW(GPIOF_PIN4) |                                                       \
                                     PIN_ODR_LOW(GPIOF_PIN5) |                                                       \
                                     PIN_ODR_LOW(GPIOF_PIN6) |                                                       \
                                     PIN_ODR_LOW(GPIOF_PIN7) |                                                       \
                                     PIN_ODR_LOW(GPIOF_PIN8) |                                                       \
                                     PIN_ODR_LOW(GPIOF_PIN9) |                                                       \
                                     PIN_ODR_LOW(GPIOF_PIN10) |                                                      \
                                     PIN_ODR_LOW(GPIOF_PIN11) |                                                      \
                                     PIN_ODR_LOW(GPIOF_PIN12) |                                                      \
                                     PIN_ODR_LOW(GPIOF_PIN13) |                                                      \
                                     PIN_ODR_LOW(GPIOF_PIN14) |                                                      \
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
#define VAL_GPIOGCRL                (PIN_CR(GPIOG_PIN0, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOG_PIN1, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOG_PIN2, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOG_PIN3, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOG_PIN4, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOG_PIN5, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOG_PIN6, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOG_PIN7, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOGCRH                (PIN_CR(GPIOG_PIN8, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOG_PIN9, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                    \
                                     PIN_CR(GPIOG_PIN10, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOG_PIN11, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOG_PIN12, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOG_PIN13, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOG_PIN14, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING) |                   \
                                     PIN_CR(GPIOG_PIN15, PIN_MODE_INPUT, PIN_CNF_INPUT_FLOATING))
#define VAL_GPIOGODR                (PIN_ODR_LOW(GPIOG_PIN0) |                                                       \
                                     PIN_ODR_LOW(GPIOG_PIN1) |                                                       \
                                     PIN_ODR_LOW(GPIOG_PIN2) |                                                       \
                                     PIN_ODR_LOW(GPIOG_PIN3) |                                                       \
                                     PIN_ODR_LOW(GPIOG_PIN4) |                                                       \
                                     PIN_ODR_LOW(GPIOG_PIN5) |                                                       \
                                     PIN_ODR_LOW(GPIOG_PIN6) |                                                       \
                                     PIN_ODR_LOW(GPIOG_PIN7) |                                                       \
                                     PIN_ODR_LOW(GPIOG_PIN8) |                                                       \
                                     PIN_ODR_LOW(GPIOG_PIN9) |                                                       \
                                     PIN_ODR_LOW(GPIOG_PIN10) |                                                      \
                                     PIN_ODR_LOW(GPIOG_PIN11) |                                                      \
                                     PIN_ODR_LOW(GPIOG_PIN12) |                                                      \
                                     PIN_ODR_LOW(GPIOG_PIN13) |                                                      \
                                     PIN_ODR_LOW(GPIOG_PIN14) |                                                      \
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
