#ifndef IODEF_H
#define IODEF_H

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#define USART_CTS_GPIO      GPIOA
#define USART_CTS_PIN       GPIO_Pin_0
#define USART_RTS_GPIO      GPIOA
#define USART_RTS_PIN       GPIO_Pin_1
#define USART_TX_GPIO       GPIOA
#define USART_TX_PIN        GPIO_Pin_2
#define USART_RX_GPIO       GPIOA
#define USART_RX_PIN        GPIO_Pin_3
#define LIGHT_BLANK_GPIO    GPIOA
#define LIGHT_BLANK_PIN     GPIO_Pin_4
#define LIGHT_SCLK_GPIO     GPIOA
#define LIGHT_SCLK_PIN      GPIO_Pin_5
#define LIGHT_MISO_GPIO     GPIOA
#define LIGHT_MISO_PIN      GPIO_Pin_6
#define LIGHT_MOSI_GPIO     GPIOA
#define LIGHT_MOSI_PIN      GPIO_Pin_7
#define PROG_RX_GPIO        GPIOA
#define PROG_RX_PIN         GPIO_Pin_9
#define PROG_TX_GPIO        GPIOA
#define PROG_TX_PIN         GPIO_Pin_10
#define CAN_RX_GPIO         GPIOA
#define CAN_RX_PIN          GPIO_Pin_11
#define CAN_TX_GPIO         GPIOA
#define CAN_TX_PIN          GPIO_Pin_12
#define SWDIO_GPIO          GPIOA
#define SWDIO_PIN           GPIO_Pin_13
#define SWCLK_GPIO          GPIOA
#define SWCLK_PIN           GPIO_Pin_14
#define RS232_R_EN_N_GPIO   GPIOA
#define RS232_R_EN_N_PIN    GPIO_Pin_15

#define SW_V33_EN_GPIO      GPIOB
#define SW_V33_EN_PIN       GPIO_Pin_0
#define SW_V42_EN_GPIO      GPIOB
#define SW_V42_EN_PIN       GPIO_Pin_1
#define SW_V50_EN_GPIO      GPIOB
#define SW_V50_EN_PIN       GPIO_Pin_2
#define IO_3_GPIO           GPIOB
#define IO_3_PIN            GPIO_Pin_3
#define IO_5_GPIO           GPIOB
#define IO_5_PIN            GPIO_Pin_4
#define IO_6_GPIO           GPIOB
#define IO_6_PIN            GPIO_Pin_5
#define SYS_UART_DN_GPIO    GPIOB
#define SYS_UART_DN_PIN     GPIO_Pin_6
#define IO_7_GPIO           GPIOB
#define IO_7_PIN            GPIO_Pin_8
#define IO_8_GPIO           GPIOB
#define IO_8_PIN            GPIO_Pin_9
#define I2C_SCL_GPIO        GPIOB
#define I2C_SCL_PIN         GPIO_Pin_10
#define I2C_SDA_GPIO        GPIOB
#define I2C_SDA_PIN         GPIO_Pin_11
#define SPI_SS_N_GPIO       GPIOB
#define SPI_SS_N_PIN        GPIO_Pin_12
#define SPI_SCLK_GPIO       GPIOB
#define SPI_SCLK_PIN        GPIO_Pin_13
#define SPI_MISO_GPIO       GPIOB
#define SPI_MISO_PIN        GPIO_Pin_14
#define SPI_MOSI_GPIO       GPIOB
#define SPI_MOSI_PIN        GPIO_Pin_15

#define IO_4_GPIO           GPIOC
#define IO_4_PIN            GPIO_Pin_0
#define IO_1_GPIO           GPIOC
#define IO_1_PIN            GPIO_Pin_1
#define IO_2_GPIO           GPIOC
#define IO_2_PIN            GPIO_Pin_2
#define LED_GPIO            GPIOC
#define LED_PIN             GPIO_Pin_3
#define LIGHT_XLAT_GPIO     GPIOC
#define LIGHT_XLAT_PIN      GPIO_Pin_4
#define SW_V18_EN_GPIO      GPIOC
#define SW_V18_EN_PIN       GPIO_Pin_5
#define SW_VSYS_EN_GPIO     GPIOC
#define SW_VSYS_EN_PIN      GPIO_Pin_6
#define SYS_UART_UP_GPIO    GPIOC
#define SYS_UART_UP_PIN     GPIO_Pin_7
#define SYS_UART_RX_GPIO    GPIOC
#define SYS_UART_RX_PIN     GPIO_Pin_10
#define SYS_UART_TX_GPIO    GPIOC
#define SYS_UART_TX_PIN     GPIO_Pin_11
#define RS232_D_OFF_N_GPIO  GPIOC
#define RS232_D_OFF_N_PIN   GPIO_Pin_12
#define SYS_PD_N_GPIO       GPIOC
#define SYS_PD_N_PIN        GPIO_Pin_14

#define OSC_IN_GPIO         GPIOD
#define OSC_IN_PIN          GPIO_Pin_0
#define OSC_OUT_GPIO        GPIOD
#define OSC_OUT_PIN         GPIO_Pin_1
#define SYS_SYNC_N_GPIO     GPIOD
#define SYS_SYNC_N_PIN      GPIO_Pin_2

#endif /* IODEF_H */
