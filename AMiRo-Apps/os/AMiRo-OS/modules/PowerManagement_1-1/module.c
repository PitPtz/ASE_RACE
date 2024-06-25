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
 * @brief   Structures and constant for the PowerManagement v1.1 module.
 */

#include <amiroos.h>

#include <string.h>

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

ADCConversionGroup moduleHalAdcVsysConversionGroup = {
  /* buffer type        */ true,
  /* number of channels */ 1,
  /* callback function  */ NULL,
  /* error callback     */ NULL,
  /* CR1                */ ADC_CR1_AWDEN | ADC_CR1_AWDIE,
  /* CR2                */ ADC_CR2_SWSTART | ADC_CR2_CONT,
  /* SMPR1              */ 0,
  /* SMPR2              */ ADC_SMPR2_SMP_AN9(ADC_SAMPLE_480),
  /* HTR                */ ADC_HTR_HT,
  /* LTR                */ 0,
  /* SQR1               */ ADC_SQR1_NUM_CH(1),
  /* SQR2               */ 0,
  /* SQR3               */ ADC_SQR3_SQ1_N(ADC_CHANNEL_IN9),
};

ROMCONST CANConfig moduleHalCanConfig = {
  /* mcr  */ CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
  /* btr  */ CAN_BTR_SJW(1) | CAN_BTR_TS2(3) | CAN_BTR_TS1(15) | CAN_BTR_BRP(1),
};

I2CConfig moduleHalI2cSrPm18Pm33GaugeRearConfig = {
  /* I²C mode   */ OPMODE_I2C,
  /* frequency  */ 400000, // TODO: replace with some macro (-> ChibiOS/HAL)
  /* duty cycle */ FAST_DUTY_CYCLE_2,
};

I2CConfig moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig = {
  /* I²C mode   */ OPMODE_I2C,
  /* frequency  */ 400000, // TODO: replace with some macro (-> ChibiOS/HAL)
  /* duty cycle */ FAST_DUTY_CYCLE_2,
};

PWMConfig moduleHalPwmBuzzerConfig = {
  /* frequency              */ 1000000,
  /* period                 */ 0,
  /* callback               */ NULL,
  /* channel configurations */ {
    /* channel 0              */ {
      /* mode                   */ PWM_OUTPUT_DISABLED,
      /* callback               */ NULL
    },
    /* channel 1              */ {
      /* mode                   */ PWM_OUTPUT_ACTIVE_HIGH,
      /* callback               */ NULL
    },
    /* channel 2              */ {
      /* mode                   */ PWM_OUTPUT_DISABLED,
      /* callback               */ NULL
    },
    /* channel 3              */ {
      /* mode                   */ PWM_OUTPUT_DISABLED,
      /* callback               */ NULL
    },
  },
  /* TIM CR2 register       */ 0,
  /* TIM BDTR register      */ 0,
  /* TIM DIER register      */ 0,
};

SerialCANDriver moduleHalSerialCAN;

ROMCONST SerialCANConfig moduleHalSerialCanConfig = {
  /* FBCAN driver   */ &MODULE_HAL_CAN,
  /* CAN identifer  */ {.std={
                          /* rsvd */ 0,
                          /* id   */ 0x7FF,
                          /* ide  */ CAN_IDE_STD,
                          /* rtr  */ CAN_RTR_DATA,
                        }},
};

#if (AMIROOS_CFG_DBG == true) || defined(__DOXYGEN__)

ROMCONST SerialConfig moduleHalSerialConfig = {
  /* bit rate */ 115200,
  /* CR1      */ 0,
  /* CR1      */ 0,
  /* CR1      */ 0,
};

#endif /* (AMIROOS_CFG_DBG == true) */

/** @} */

/*===========================================================================*/
/**
 * @name GPIO definitions
 * @{
 */
/*===========================================================================*/

/**
 * @brief   SYS_REG_EN output signal GPIO.
 */
static apalGpio_t _gpioSysRegEn = {
  /* line */ LINE_SYS_REG_EN,
};
ROMCONST apalControlGpio_t moduleGpioSysRegEn = {
  /* GPIO */ &_gpioSysRegEn,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   IR_INT1 input signal GPIO.
 */
static apalGpio_t _gpioIrInt1 = {
  /* line */ LINE_IR_INT1_N,
};
ROMCONST apalControlGpio_t moduleGpioIrInt1 = {
  /* GPIO */ &_gpioIrInt1,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
#if (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR)
    /* active state   */ (VCNL4020_LLD_INT_EDGE == APAL_GPIO_EDGE_RISING) ? APAL_GPIO_ACTIVE_HIGH : APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ VCNL4020_LLD_INT_EDGE,
#elif (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L0X)
    /* active state   */ (PCAL6524_LLD_INT_EDGE == APAL_GPIO_EDGE_RISING) ? APAL_GPIO_ACTIVE_HIGH : APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ PCAL6524_LLD_INT_EDGE,
#elif (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L1X)
    /* active state   */ (PCAL6524_LLD_INT_EDGE == APAL_GPIO_EDGE_RISING) ? APAL_GPIO_ACTIVE_HIGH : APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ PCAL6524_LLD_INT_EDGE,
#else /* (BOARD_SENSORRING == ?) */
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
#endif /* (BOARD_SENSORRING == ?) */
  },
};

/**
 * @brief   POWER_EN output signal GPIO.
 */
static apalGpio_t _gpioPowerEn = {
  /* line */ LINE_POWER_EN,
};
ROMCONST apalControlGpio_t moduleGpioPowerEn = {
  /* GPIO */ &_gpioPowerEn,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   SYS_UART_DN bidirectional signal GPIO.
 */
static apalGpio_t _gpioSysUartDn = {
  /* line */ LINE_SYS_UART_DN,
};
ROMCONST apalControlGpio_t moduleGpioSysUartDn = {
  /* GPIO */ &_gpioSysUartDn,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   CHARGE_STAT2A input signal GPIO.
 */
static apalGpio_t _gpioChargeStat2A = {
  /* line */ LINE_CHARGE_STAT2A,
};
ROMCONST apalControlGpio_t moduleGpioChargeStat2A = {
  /* GPIO */ &_gpioChargeStat2A,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ BQ241xx_LLD_CHARGE_STATUS_GPIO_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   GAUGE_BATLOW2 input signal GPIO.
 */
static apalGpio_t _gpioGaugeBatLow2 = {
  /* line */ LINE_GAUGE_BATLOW2,
};
ROMCONST apalControlGpio_t moduleGpioGaugeBatLow2 = {
  /* GPIO */ &_gpioGaugeBatLow2,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ BQ27500_LLD_BATLOW_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   GAUGE_BATGD2 input signal GPIO.
 */
static apalGpio_t _gpioGaugeBatGd2 = {
  /* line */ LINE_GAUGE_BATGD2_N,
};
ROMCONST apalControlGpio_t moduleGpioGaugeBatGd2 = {
  /* GPIO */ &_gpioGaugeBatGd2,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ BQ27500_LLD_BATGOOD_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   LED output signal GPIO.
 */
static apalGpio_t _gpioLed = {
  /* line */ LINE_LED,
};
ROMCONST apalControlGpio_t moduleGpioLed = {
  /* GPIO */ &_gpioLed,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   SYS_UART_UP bidirectional signal GPIO.
 */
static apalGpio_t _gpioSysUartUp = {
  /* line */ LINE_SYS_UART_UP,
};
ROMCONST apalControlGpio_t moduleGpioSysUartUp = {
  /* GPIO */ &_gpioSysUartUp,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   CHARGE_STAT1A input signal GPIO.
 */
static apalGpio_t _gpioChargeStat1A = {
  /* line */ LINE_CHARGE_STAT1A,
};
ROMCONST apalControlGpio_t moduleGpioChargeStat1A = {
  /* GPIO */ &_gpioChargeStat1A,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ BQ241xx_LLD_CHARGE_STATUS_GPIO_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   GAUGE_BATLOW1 input signal GPIO.
 */
static apalGpio_t _gpioGaugeBatLow1 = {
  /* line */ LINE_GAUGE_BATLOW1,
};
ROMCONST apalControlGpio_t moduleGpioGaugeBatLow1 = {
  /* GPIO */ &_gpioGaugeBatLow1,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ BQ27500_LLD_BATLOW_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   GAUGE_BATGD1 input signal GPIO.
 */
static apalGpio_t _gpioGaugeBatGd1 = {
  /* line */ LINE_GAUGE_BATGD1_N,
};
ROMCONST apalControlGpio_t moduleGpioGaugeBatGd1 = {
  /* GPIO */ &_gpioGaugeBatGd1,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ BQ27500_LLD_BATGOOD_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   CHARG_EN1 output signal GPIO.
 */
static apalGpio_t _gpioChargeEn1 = {
  /* line */ LINE_CHARGE_EN1_N,
};
ROMCONST apalControlGpio_t moduleGpioChargeEn1 = {
  /* GPIO */ &_gpioChargeEn1,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ BQ241xx_LLD_ENABLED_GPIO_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   IR_INT2 input signal GPIO.
 */
static apalGpio_t _gpioIrInt2 = {
  /* line */ LINE_IR_INT2_N,
};
ROMCONST apalControlGpio_t moduleGpioIrInt2 = {
  /* GPIO */ &_gpioIrInt2,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
#if (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR)
    /* active state   */ (VCNL4020_LLD_INT_EDGE == APAL_GPIO_EDGE_RISING) ? APAL_GPIO_ACTIVE_HIGH : APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ VCNL4020_LLD_INT_EDGE,
#elif (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L0X)
    /* active state   */ (PCAL6524_LLD_INT_EDGE == APAL_GPIO_EDGE_RISING) ? APAL_GPIO_ACTIVE_HIGH : APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ PCAL6524_LLD_INT_EDGE,
#elif (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L1X)
    /* active state   */ (PCAL6524_LLD_INT_EDGE == APAL_GPIO_EDGE_RISING) ? APAL_GPIO_ACTIVE_HIGH : APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ PCAL6524_LLD_INT_EDGE,
#else /* (BOARD_SENSORRING == ?) */
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
#endif /* (BOARD_SENSORRING == ?) */
  },
};

/**
 * @brief   TOUCH_INT input signal GPIO.
 */
static apalGpio_t _gpioTouchInt = {
  /* line */ LINE_TOUCH_INT_N,
};
ROMCONST apalControlGpio_t moduleGpioTouchInt = {
  /* GPIO */ &_gpioTouchInt,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
#if (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR)
    /* active state   */ (MPR121_LLD_INT_EDGE == APAL_GPIO_EDGE_RISING) ? APAL_GPIO_ACTIVE_HIGH : APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ MPR121_LLD_INT_EDGE,
#elif (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L0X)
    /* active state   */ (AT42QT1050_LLD_INT_EDGE == APAL_GPIO_EDGE_RISING) ? APAL_GPIO_ACTIVE_HIGH : APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ AT42QT1050_LLD_INT_EDGE,
#elif (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L1X)
    /* active state   */ (AT42QT1050_LLD_INT_EDGE == APAL_GPIO_EDGE_RISING) ? APAL_GPIO_ACTIVE_HIGH : APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ AT42QT1050_LLD_INT_EDGE,
#else /* (BOARD_SENSORRING == ?) */
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
#endif /* (BOARD_SENSORRING == ?) */
  },
};

/**
 * @brief   SYS_DONE input signal GPIO.
 */
static apalGpio_t _gpioSysDone = {
  /* line */ LINE_SYS_DONE,
};
ROMCONST apalControlGpio_t moduleGpioSysDone = {
  /* GPIO */ &_gpioSysDone,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ APAL_GPIO_ACTIVE_HIGH,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   SYS_PROG output signal GPIO.
 */
static apalGpio_t _gpioSysProg = {
  /* line */ LINE_SYS_PROG_N,
};
ROMCONST apalControlGpio_t moduleGpioSysProg = {
  /* GPIO */ &_gpioSysProg,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   PATH_DC input signal GPIO.
 */
static apalGpio_t _gpioPathDc = {
  /* line */ LINE_PATH_DC,
};
ROMCONST apalControlGpio_t moduleGpioPathDc = {
  /* GPIO */ &_gpioPathDc,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_INPUT,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   SYS_SPI_DIR bidirectional signal GPIO.
 */
static apalGpio_t _gpioSysSpiDir = {
  /* line */ LINE_SYS_SPI_DIR,
};
ROMCONST apalControlGpio_t moduleGpioSysSpiDir = {
  /* GPIO */ &_gpioSysSpiDir,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_FALLING,
  },
};

/**
 * @brief   SYS_SYNC bidirectional signal GPIO.
 */
static apalGpio_t _gpioSysSync = {
  /* line */ LINE_SYS_INT_N,
};
ROMCONST apalControlGpio_t moduleGpioSysSync = {
  /* GPIO */ &_gpioSysSync,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   SYS_PD bidirectional signal GPIO.
 */
static apalGpio_t _gpioSysPd = {
  /* line */ LINE_SYS_PD_N,
};
ROMCONST apalControlGpio_t moduleGpioSysPd = {
  /* GPIO */ &_gpioSysPd,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   SYS_WARMRST bidirectional signal GPIO.
 */
static apalGpio_t _gpioSysWarmrst = {
  /* line */ LINE_SYS_WARMRST_N,
};
ROMCONST apalControlGpio_t moduleGpioSysWarmrst = {
  /* GPIO */ &_gpioSysWarmrst,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_BOTH,
  },
};

/**
 * @brief   BT_RST output signal GPIO.
 */
static apalGpio_t _gpioBtRst = {
  /* line */ LINE_BT_RST,
};
ROMCONST apalControlGpio_t moduleGpioBtRst = {
  /* GPIO */ &_gpioBtRst,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ APAL_GPIO_ACTIVE_LOW,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
  },
};

/**
 * @brief   CHARGE_EN2 output signal GPIO.
 */
static apalGpio_t _gpioChargeEn2 = {
  /* line */ LINE_CHARGE_EN2_N,
};
ROMCONST apalControlGpio_t moduleGpioChargeEn2 = {
  /* GPIO */ &_gpioChargeEn2,
  /* meta */ {
    /* direction      */ APAL_GPIO_DIRECTION_OUTPUT,
    /* active state   */ BQ241xx_LLD_ENABLED_GPIO_ACTIVE_STATE,
    /* interrupt edge */ APAL_GPIO_EDGE_NONE,
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
  // IR_INT1
  palSetLineCallback(moduleGpioIrInt1.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioIrInt1.gpio->line);
  palEnableLineEvent(moduleGpioIrInt1.gpio->line, APAL2CH_EDGE(moduleGpioIrInt1.meta.edge));

  // GAUGE_BATLOW2
  palSetLineCallback(moduleGpioGaugeBatLow2.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioGaugeBatLow2.gpio->line);
  palEnableLineEvent(moduleGpioGaugeBatLow2.gpio->line, APAL2CH_EDGE(moduleGpioGaugeBatLow2.meta.edge));

  // GAUGE_BATGD2
  palSetLineCallback(moduleGpioGaugeBatGd2.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioGaugeBatGd2.gpio->line);
  palEnableLineEvent(moduleGpioGaugeBatGd2.gpio->line, APAL2CH_EDGE(moduleGpioGaugeBatGd2.meta.edge));

  // GAUGE_BATLOW1
  palSetLineCallback(moduleGpioGaugeBatLow1.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioGaugeBatLow1.gpio->line);
  palEnableLineEvent(moduleGpioGaugeBatLow1.gpio->line, APAL2CH_EDGE(moduleGpioGaugeBatLow1.meta.edge));

  // GAUGE_BATGD1
  palSetLineCallback(moduleGpioGaugeBatGd1.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioGaugeBatGd1.gpio->line);
  palEnableLineEvent(moduleGpioGaugeBatGd1.gpio->line, APAL2CH_EDGE(moduleGpioGaugeBatGd1.meta.edge));

  // IR_INT2
  palSetLineCallback(moduleGpioIrInt2.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioIrInt2.gpio->line);
  palEnableLineEvent(moduleGpioIrInt2.gpio->line, APAL2CH_EDGE(moduleGpioIrInt2.meta.edge));

  // TOUCH_INT
  palSetLineCallback(moduleGpioTouchInt.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioTouchInt.gpio->line);
  palEnableLineEvent(moduleGpioTouchInt.gpio->line, APAL2CH_EDGE(moduleGpioTouchInt.meta.edge));

  // PATH_DC
  palSetLineCallback(moduleGpioPathDc.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioPathDc.gpio->line);
  palEnableLineEvent(moduleGpioPathDc.gpio->line, APAL2CH_EDGE(moduleGpioPathDc.meta.edge));

  // SYS_SPI_DIR
  palSetLineCallback(moduleGpioSysSpiDir.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioSysSpiDir.gpio->line);
  palEnableLineEvent(moduleGpioSysSpiDir.gpio->line, APAL2CH_EDGE(moduleGpioSysSpiDir.meta.edge));

  // SYS_WARMRST
  palSetLineCallback(moduleGpioSysWarmrst.gpio->line, aosSysGetStdGpioCallback(), &moduleGpioSysWarmrst.gpio->line);
  palEnableLineEvent(moduleGpioSysWarmrst.gpio->line, APAL2CH_EDGE(moduleGpioSysWarmrst.meta.edge));

  return;
}

/**
 * @brief   Initializes periphery communication interfaces.
 */
void moduleInitPeripheryInterfaces()
{
  // serial driver
#if (AMIROOS_CFG_DBG == true)
  sdStart(&MODULE_HAL_SERIAL, &moduleHalSerialConfig);
#endif

  // I2C
#if (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR)
  moduleHalI2cSrPm18Pm33GaugeRearConfig.clock_speed = (PCA9544A_LLD_I2C_MAXFREQUENCY < moduleHalI2cSrPm18Pm33GaugeRearConfig.clock_speed) ? PCA9544A_LLD_I2C_MAXFREQUENCY : moduleHalI2cSrPm18Pm33GaugeRearConfig.clock_speed;
  moduleHalI2cSrPm18Pm33GaugeRearConfig.clock_speed = (VCNL4020_LLD_I2C_MAXFREQUENCY < moduleHalI2cSrPm18Pm33GaugeRearConfig.clock_speed) ? VCNL4020_LLD_I2C_MAXFREQUENCY : moduleHalI2cSrPm18Pm33GaugeRearConfig.clock_speed;
#elif (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L0X)
  moduleHalI2cSrPm18Pm33GaugeRearConfig.clock_speed = (PCAL6524_LLD_I2C_MAXFREQUENCY < moduleHalI2cSrPm18Pm33GaugeRearConfig.clock_speed) ? PCAL6524_LLD_I2C_MAXFREQUENCY : moduleHalI2cSrPm18Pm33GaugeRearConfig.clock_speed;
#elif (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L1X)
  moduleHalI2cSrPm18Pm33GaugeRearConfig.clock_speed = (PCAL6524_LLD_I2C_MAXFREQUENCY < moduleHalI2cSrPm18Pm33GaugeRearConfig.clock_speed) ? PCAL6524_LLD_I2C_MAXFREQUENCY : moduleHalI2cSrPm18Pm33GaugeRearConfig.clock_speed;
#endif
  moduleHalI2cSrPm18Pm33GaugeRearConfig.clock_speed = (INA219_LLD_I2C_MAXFREQUENCY < moduleHalI2cSrPm18Pm33GaugeRearConfig.clock_speed) ? INA219_LLD_I2C_MAXFREQUENCY : moduleHalI2cSrPm18Pm33GaugeRearConfig.clock_speed;
  moduleHalI2cSrPm18Pm33GaugeRearConfig.clock_speed = (BQ27500_LLD_I2C_MAXFREQUENCY < moduleHalI2cSrPm18Pm33GaugeRearConfig.clock_speed) ? BQ27500_LLD_I2C_MAXFREQUENCY : moduleHalI2cSrPm18Pm33GaugeRearConfig.clock_speed;
  moduleHalI2cSrPm18Pm33GaugeRearConfig.duty_cycle = (moduleHalI2cSrPm18Pm33GaugeRearConfig.clock_speed <= 100000) ? STD_DUTY_CYCLE : FAST_DUTY_CYCLE_2;
  i2cStart(&MODULE_HAL_I2C_SR_PM18_PM33_GAUGEREAR, &moduleHalI2cSrPm18Pm33GaugeRearConfig);
#if (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR)
  moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed = (PCA9544A_LLD_I2C_MAXFREQUENCY < moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed) ? PCA9544A_LLD_I2C_MAXFREQUENCY : moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed;
  moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed = (VCNL4020_LLD_I2C_MAXFREQUENCY < moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed) ? VCNL4020_LLD_I2C_MAXFREQUENCY : moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed;
  moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed = (MPR121_LLD_I2C_MAXFREQUENCY < moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed) ? MPR121_LLD_I2C_MAXFREQUENCY : moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed;
#elif (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L0X)
  moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed = (PCAL6524_LLD_I2C_MAXFREQUENCY < moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed) ? PCAL6524_LLD_I2C_MAXFREQUENCY : moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed;
  moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed = (AT42QT1050_LLD_I2C_MAXFREQUENCY < moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed) ? AT42QT1050_LLD_I2C_MAXFREQUENCY : moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed;
#elif (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L1X)
  moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed = (PCAL6524_LLD_I2C_MAXFREQUENCY < moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed) ? PCAL6524_LLD_I2C_MAXFREQUENCY : moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed;
  moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed = (AT42QT1050_LLD_I2C_MAXFREQUENCY < moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed) ? AT42QT1050_LLD_I2C_MAXFREQUENCY : moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed;
#endif
  moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed = (INA219_LLD_I2C_MAXFREQUENCY < moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed) ? INA219_LLD_I2C_MAXFREQUENCY : moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed;
  moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed = (AT24C01B_LLD_I2C_MAXFREQUENCY < moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed) ? AT24C01B_LLD_I2C_MAXFREQUENCY : moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed;
  moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed = (BQ27500_LLD_I2C_MAXFREQUENCY < moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed) ? BQ27500_LLD_I2C_MAXFREQUENCY : moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed;
  moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.duty_cycle = (moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig.clock_speed <= 100000) ? STD_DUTY_CYCLE : FAST_DUTY_CYCLE_2;
  i2cStart(&MODULE_HAL_I2C_SR_PM42_PM50_PMVDD_EEPROM_GAUGEFRONT, &moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig);

  // ADC
  adcStart(&MODULE_HAL_ADC_VSYS, NULL);

  // PWM
  pwmStart(&MODULE_HAL_PWM_BUZZER, &moduleHalPwmBuzzerConfig);
  moduleHalPwmBuzzerConfig.frequency = MODULE_HAL_PWM_BUZZER.clock;
  pwmStop(&MODULE_HAL_PWM_BUZZER);
  moduleHalPwmBuzzerConfig.period = moduleHalPwmBuzzerConfig.frequency / PKxxxExxx_LLD_FREQUENCY_SPEC;
  pwmStart(&MODULE_HAL_PWM_BUZZER, &moduleHalPwmBuzzerConfig);

  // CAN
  aosFBCanInit(&MODULE_HAL_CAN, NULL, NULL);
  aosFBCanStart(&MODULE_HAL_CAN, &moduleHalCanConfig);
  sdcanInit();
  sdcanObjectInit(&moduleHalSerialCAN);
  sdcanStart(&moduleHalSerialCAN, &moduleHalSerialCanConfig);

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
  aosShellAddCommand(&moduleTestAdcShellCmd);
  aosShellAddCommand(&moduleTestAt24c01bShellCmd);
  aosShellAddCommand(&moduleTestBq241xxShellCmd);
  aosShellAddCommand(&moduleTestBq27500ShellCmd);
  aosShellAddCommand(&moduleTestBq27500Bq241xxShellCmd);
  aosShellAddCommand(&moduleTestIna219ShellCmd);
  aosShellAddCommand(&moduleTestLedShellCmd);
  aosShellAddCommand(&moduleTestPkxxxexxxShellCmd);
  aosShellAddCommand(&moduleTestTps6211xShellCmd);
  aosShellAddCommand(&moduleTestTps6211xIna219ShellCmd);
#if (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR)
  aosShellAddCommand(&moduleTestPca9544aShellCmd);
  aosShellAddCommand(&moduleTestMpr121ShellCmd);
  aosShellAddCommand(&moduleTestVcnl4020ShellCmd);
#elif (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L0X)
  aosShellAddCommand(&moduleTestPcal6524ShellCmd);
  aosShellAddCommand(&moduleTestAt42qt1050ShellCmd);
#elif (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L1X)
  aosShellAddCommand(&moduleTestPcal6524ShellCmd);
  aosShellAddCommand(&moduleTestAt42qt1050ShellCmd);
#endif
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
  // CAN
  sdcanStop(&moduleHalSerialCAN);
  aosFBCanStop(&MODULE_HAL_CAN);

  // PWM
  pwmStop(&MODULE_HAL_PWM_BUZZER);

  // ADC */
  adcStop(&MODULE_HAL_ADC_VSYS);

  // I2C
  i2cStop(&MODULE_HAL_I2C_SR_PM18_PM33_GAUGEREAR);
  i2cStop(&MODULE_HAL_I2C_SR_PM42_PM50_PMVDD_EEPROM_GAUGEFRONT);

  // serial driver
#if (AMIROOS_CFG_DBG == true)
  /* don't stop the serial driver so messages can still be printed */
#endif

  return;
}

/** @} */

/*===========================================================================*/
/**
 * @name Startup Shutdown Synchronization Protocol (SSSP)
 * @{
 */
/*===========================================================================*/
#if ((AMIROOS_CFG_SSSP_ENABLE == true) && (AMIROOS_CFG_SSSP_MSI == true)) ||    \
    defined(__DOXYGEN__)

/* some local definitions */
// maximum number of bytes per CAN frame
#define CAN_BYTES_PER_FRAME                     8
// identifier (as dominant as possible)
#define MSI_BCBMSG_CANID                        0

static void _moduleSsspBCBfilterCallback(const CANRxFrame* frame, void* params)
{
  // copy the frame to the buffer
  *((CANRxFrame*)params) = *frame;
  return;
}

void moduleSsspBcbSetup(void* data)
{
  aosDbgCheck(data != NULL);

  // set mask
  ((MODULE_SSSP_BCBDATA*)data)->filter.mask.std.id = ~0;
  ((MODULE_SSSP_BCBDATA*)data)->filter.mask.std.ide = 1;
  ((MODULE_SSSP_BCBDATA*)data)->filter.mask.std.rtr = 1;

  // set filter
  ((MODULE_SSSP_BCBDATA*)data)->filter.filter.std.id = MSI_BCBMSG_CANID;
  ((MODULE_SSSP_BCBDATA*)data)->filter.filter.std.ide = CAN_IDE_STD;
  ((MODULE_SSSP_BCBDATA*)data)->filter.filter.std.rtr = CAN_RTR_DATA;

  // set callback and parameter
  ((MODULE_SSSP_BCBDATA*)data)->filter.callback = _moduleSsspBCBfilterCallback;
  ((MODULE_SSSP_BCBDATA*)data)->filter.cbparams = &((MODULE_SSSP_BCBDATA*)data)->frame;

  // invalidate frame
  ((MODULE_SSSP_BCBDATA*)data)->frame.SID = ~MSI_BCBMSG_CANID;

  // add filter
  aosFBCanAddFilter(&MODULE_HAL_CAN, &((MODULE_SSSP_BCBDATA*)data)->filter);

  return;
}

void moduleSsspBcbRelease(void* data)
{
  // remove filter
  const aos_status_t status = aosFBCanRemoveFilter(&MODULE_HAL_CAN, &((MODULE_SSSP_BCBDATA*)data)->filter);
  aosDbgAssertMsg(status == AOS_SUCCESS, "SSSP BCB failed to remove filter");

  return;
}

aos_ssspbcbstatus_t moduleSsspBcbTransmit(const uint8_t* buffer, size_t length, void* data)
{
  aosDbgCheck(buffer != NULL);
  aosDbgCheck(length > 0 && length <= CAN_BYTES_PER_FRAME);

  (void)data;

  // local variables
  CANTxFrame frame;

  // setup the common parts of the message frame
  frame.DLC = (uint8_t)length;
  frame.RTR = CAN_RTR_DATA;
  frame.IDE = CAN_IDE_STD;
  frame.SID = MSI_BCBMSG_CANID;
  memcpy(frame.data8, buffer, length);

  // sent the frame and return
  return (aosFBCanTransmitTimeout(&MODULE_HAL_CAN, &frame, TIME_IMMEDIATE) != AOS_TIMEOUT) ? AOS_SSSP_BCB_SUCCESS : AOS_SSSP_BCB_ERROR;
}

aos_ssspbcbstatus_t moduleSsspBcbReceive(uint8_t* buffer, size_t length, void* data)
{
  aosDbgCheck(buffer != NULL);
  aosDbgCheck(length > 0 && length <= CAN_BYTES_PER_FRAME);

  // check buffer for new frame
  if (((MODULE_SSSP_BCBDATA*)data)->frame.SID == MSI_BCBMSG_CANID) {
    // verify frame
    if (((MODULE_SSSP_BCBDATA*)data)->frame.DLC == length) {
      // success: fetch, invalidate buffer and return
      memcpy(buffer, ((MODULE_SSSP_BCBDATA*)data)->frame.data8, length);
      ((MODULE_SSSP_BCBDATA*)data)->frame.SID = ~MSI_BCBMSG_CANID;
      return AOS_SSSP_BCB_SUCCESS;
    }
    // an unexpected frame was received
    else {
      return AOS_SSSP_BCB_INVALIDMSG;
    }
  }
  else {
    // failure: return with error
    return AOS_SSSP_BCB_ERROR;
  }
}

#undef MSI_BCBMSG_CANID
#undef CAN_BYTES_PER_FRAME

#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) && (AMIROOS_CFG_SSSP_MSI == true) */
/** @} */

/*===========================================================================*/
/**
 * @name AMiRo-OS shell configurations
 * @{
 */
/*===========================================================================*/
#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)

ROMCONST char* moduleShellPrompt = "PowerManagement";

AosShellChannel moduleShellSerialCanChannel;

#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */
/** @} */

/*===========================================================================*/
/**
 * @name Low-level drivers
 * @{
 */
/*===========================================================================*/

AT24C01BDriver moduleLldEeprom = {
  /* I2C driver   */ &MODULE_HAL_I2C_SR_PM42_PM50_PMVDD_EEPROM_GAUGEFRONT,
  /* I2C address  */ AT24C01B_LLD_I2C_ADDR_FIXED,
};

BQ241xxDriver moduleLldBatteryChargerFront = {
  /* charge enable GPIO */ &moduleGpioChargeEn1,
  /* charge status GPIO */ &moduleGpioChargeStat1A,
};

BQ241xxDriver moduleLldBatteryChargerRear = {
  /* charge enable GPIO */ &moduleGpioChargeEn2,
  /* charge status GPIO */ &moduleGpioChargeStat2A,
};

BQ27500Driver moduleLldFuelGaugeFront = {
  /* I2C driver         */ &MODULE_HAL_I2C_SR_PM42_PM50_PMVDD_EEPROM_GAUGEFRONT,
  /* battery low GPIO   */ &moduleGpioGaugeBatLow1,
  /* battery good GPIO  */ &moduleGpioGaugeBatGd1,
};

BQ27500Driver moduleLldFuelGaugeRear = {
  /* I2C driver         */ &MODULE_HAL_I2C_SR_PM18_PM33_GAUGEREAR,
  /* battery low GPIO   */ &moduleGpioGaugeBatLow2,
  /* battery good GPIO  */ &moduleGpioGaugeBatGd2,
};

INA219Driver moduleLldPowerMonitorVdd = {
  /* I2C Driver       */ &MODULE_HAL_I2C_SR_PM42_PM50_PMVDD_EEPROM_GAUGEFRONT,
  /* I²C address      */ INA219_LLD_I2C_ADDR_A0 | INA219_LLD_I2C_ADDR_A1,
  /* current LSB (uA) */ 0x00u,
  /* configuration    */ NULL,
};

INA219Driver moduleLldPowerMonitorVio18 = {
  /* I2C Driver       */ &MODULE_HAL_I2C_SR_PM18_PM33_GAUGEREAR,
  /* I²C address      */ INA219_LLD_I2C_ADDR_A1,
  /* current LSB (uA) */ 0x00u,
  /* configuration    */ NULL,
};

INA219Driver moduleLldPowerMonitorVio33 = {
  /* I2C Driver       */ &MODULE_HAL_I2C_SR_PM18_PM33_GAUGEREAR,
  /* I²C address      */ INA219_LLD_I2C_ADDR_FIXED,
  /* current LSB (uA) */ 0x00u,
  /* configuration    */ NULL,
};

INA219Driver moduleLldPowerMonitorVsys42 = {
  /* I2C Driver       */ &MODULE_HAL_I2C_SR_PM42_PM50_PMVDD_EEPROM_GAUGEFRONT,
  /* I²C address      */ INA219_LLD_I2C_ADDR_FIXED,
  /* current LSB (uA) */ 0x00u,
  /* configuration    */ NULL,
};

INA219Driver moduleLldPowerMonitorVio50 = {
  /* I2C Driver       */ &MODULE_HAL_I2C_SR_PM42_PM50_PMVDD_EEPROM_GAUGEFRONT,
  /* I²C address      */ INA219_LLD_I2C_ADDR_A1,
  /* current LSB (uA) */ 0x00u,
  /* configuration    */ NULL,
};

LEDDriver moduleLldStatusLed = {
  /* LED GPIO */ &moduleGpioLed,
};

TPS6211xDriver moduleLldStepDownConverter = {
  /* Power enable GPIO */ &moduleGpioPowerEn,
};

#if (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR) || defined(__DOXYGEN__)

MPR121Driver moduleLldTouch = {
  /* I²C Driver */ &MODULE_HAL_I2C_SR_PM42_PM50_PMVDD_EEPROM_GAUGEFRONT,
};

PCA9544ADriver moduleLldI2cMultiplexer1 = {
  /* I²C driver   */ &MODULE_HAL_I2C_SR_PM18_PM33_GAUGEREAR,
  /* I²C address  */ PCA9544A_LLD_I2C_ADDR_A0 | PCA9544A_LLD_I2C_ADDR_A1 | PCA9544A_LLD_I2C_ADDR_A2,
};

PCA9544ADriver moduleLldI2cMultiplexer2 = {
  /* I²C driver   */ &MODULE_HAL_I2C_SR_PM42_PM50_PMVDD_EEPROM_GAUGEFRONT,
  /* I²C address  */ PCA9544A_LLD_I2C_ADDR_A0 | PCA9544A_LLD_I2C_ADDR_A1 | PCA9544A_LLD_I2C_ADDR_A2,
};

VCNL4020Driver moduleLldProximity1 = {
  /* I²C Driver */ &MODULE_HAL_I2C_SR_PM18_PM33_GAUGEREAR,
};

VCNL4020Driver moduleLldProximity2 = {
  /* I²C Driver */ &MODULE_HAL_I2C_SR_PM42_PM50_PMVDD_EEPROM_GAUGEFRONT,
};

#endif /* (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR) */

#if (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L0X) || defined(__DOXYGEN__)

PCAL6524Driver moduleLldGpioExtender1 = {
  /* I²C driver   */ &MODULE_HAL_I2C_SR_PM18_PM33_GAUGEREAR,
  /* I²C address  */ PCAL6524_LLD_I2C_ADDR_VDD,
};

PCAL6524Driver moduleLldGpioExtender2 = {
  /* I²C driver   */ &MODULE_HAL_I2C_SR_PM42_PM50_PMVDD_EEPROM_GAUGEFRONT,
  /* I²C address  */ PCAL6524_LLD_I2C_ADDR_VDD,
};

AT42QT1050Driver moduleLldTouch = {
  /* I²C driver   */ &MODULE_HAL_I2C_SR_PM42_PM50_PMVDD_EEPROM_GAUGEFRONT,
  /* I²C address  */ AT42QT1050_LLD_I2C_ADDRSEL_LOW,
};

#endif /* (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L0X) */

#if (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L1X) || defined(__DOXYGEN__)

PCAL6524Driver moduleLldGpioExtender1 = {
  /* I²C driver   */ &MODULE_HAL_I2C_SR_PM18_PM33_GAUGEREAR,
  /* I²C address  */ PCAL6524_LLD_I2C_ADDR_VDD,
};

PCAL6524Driver moduleLldGpioExtender2 = {
  /* I²C driver   */ &MODULE_HAL_I2C_SR_PM42_PM50_PMVDD_EEPROM_GAUGEFRONT,
  /* I²C address  */ PCAL6524_LLD_I2C_ADDR_VDD,
};

AT42QT1050Driver moduleLldTouch = {
  /* I²C driver   */ &MODULE_HAL_I2C_SR_PM42_PM50_PMVDD_EEPROM_GAUGEFRONT,
  /* I²C address  */ AT42QT1050_LLD_I2C_ADDRSEL_LOW,
};

#endif /* (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L1X) */

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
 * ADC (VSYS)
 */
#include "test/adc/module_test_adc.h"
static int _moduleTestAdcShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestAdcShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestAdcShellCmd, "test:ADC", _moduleTestAdcShellCmdCb);

/*
 * AT24C01BN-SH-B (EEPROM)
 */
#include "test/AT24C01B/module_test_AT24C01B.h"
static int _moduleTestAt24c01bShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestAt24c01bShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestAt24c01bShellCmd, "test:EEPROM", _moduleTestAt24c01bShellCmdCb);

/*
 * bq24103a (battery charger)
 */
#include "test/bq241xx/module_test_bq241xx.h"
static int _moduleTestBq241xxShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestBq241xxShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestBq241xxShellCmd, "test:BatteryCharger", _moduleTestBq241xxShellCmdCb);

/*
 * bq27500 (fuel gauge)
 */
#include "test/bq27500/module_test_bq27500.h"
static int _moduleTestBq27500ShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestBq27500ShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestBq27500ShellCmd, "test:FuelGauge", _moduleTestBq27500ShellCmdCb);

/*
 * bq27500 (fuel gauge) in combination with bq24103a (battery charger)
 */
#include "test/bq27500_bq241xx/module_test_bq27500_bq241xx.h"
static int _moduleTestBq27500Bq241xxShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestBq27500Bq241xxShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestBq27500Bq241xxShellCmd, "test:FuelGauge&BatteryCharger", _moduleTestBq27500Bq241xxShellCmdCb);

/*
 * INA219 (power monitor)
 */
#include "test/INA219/module_test_INA219.h"
static int _moduleTestIna219ShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestIna219ShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestIna219ShellCmd, "test:PowerMonitor", _moduleTestIna219ShellCmdCb);

/*
 * Status LED
 */
#include "test/LED/module_test_LED.h"
static int _moduleTestLedShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestLedShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestLedShellCmd, "test:StatusLED", _moduleTestLedShellCmdCb);

/*
 * PKLCS1212E4001 (buzzer)
 */
#include "test/PKxxxExxx/module_test_PKxxxExxx.h"
static int _moduleTestPkxxxexxxShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestPkxxxexxxShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestPkxxxexxxShellCmd, "test:Buzzer", _moduleTestPkxxxexxxShellCmdCb);

/*
 * TPS62113 (step-down converter)
 */
#include "test/TPS6211x/module_test_TPS6211x.h"
static int _moduleTestTps6211xShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestTps6211xShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestTps6211xShellCmd, "test:StepDownConverter", _moduleTestTps6211xShellCmdCb);

/*
 * TPS62113 (step-sown converter) in combination with INA219 (power monitor)
 */
#include "test/TPS6211x_INA219/module_test_TPS6211x_INA219.h"
static int _moduleTestTps6211xIna219ShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestTps6211xIna219ShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestTps6211xIna219ShellCmd, "test:StepDownConverter&PowerMonitor", _moduleTestTps6211xIna219ShellCmdCb);

#if (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR) || defined(__DOXYGEN__)

/*
 * MPR121 (touch sensor)
 */
#include "test/MPR121/module_test_MPR121.h"
static int _moduleTestMpr121ShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestMpr121ShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestMpr121ShellCmd, "test:Touch", _moduleTestMpr121ShellCmdCb);

/*
 * PCA9544A (I2C multiplexer)
 */
#include "test/PCA9544A/module_test_PCA9544A.h"
static int _moduleTestPca9544aShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestPca9544aShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestPca9544aShellCmd, "test:I2CMultiplexer", _moduleTestPca9544aShellCmdCb);

/*
 * VCNL4020 (proximity sensor)
 */
#include "test/VCNL4020/module_test_VCNL4020.h"
static int _moduleTestVcnl4020ShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestVcnl4020ShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestVcnl4020ShellCmd, "test:Proximity", _moduleTestVcnl4020ShellCmdCb);

#endif /* (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR) */

#if (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L0X) || defined(__DOXYGEN__)

/*
 * PCAL6524 (GPIO extender)
 */
#include "test/PCAL6524/module_test_PCAL6524.h"
static int _moduleTestPcal6524ShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestPcal6524ShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestPcal6524ShellCmd, "test:GPIOExtenter", _moduleTestPcal6524ShellCmdCb);

/*
 * AT42QT1050 (touch sensor)
 */
#include "test/AT42QT1050/module_test_AT42QT1050.h"
static int _moduleTestAt42qt1050ShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestAt42qt1050ShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestAt42qt1050ShellCmd, "test:Touch", _moduleTestAt42qt1050ShellCmdCb);

#endif /* (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L0X) */

#if (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L1X) || defined(__DOXYGEN__)

/*
 * PCAL6524 (GPIO extender)
 */
#include "test/PCAL6524/module_test_PCAL6524.h"
static int _moduleTestPcal6524ShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestPcal6524ShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestPcal6524ShellCmd, "test:GPIOExtenter", _moduleTestPcal6524ShellCmdCb);

/*
 * AT42QT1050 (touch sensor)
 */
#include "test/AT42QT1050/module_test_AT42QT1050.h"
static int _moduleTestAt42qt1050ShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return moduleTestAt42qt1050ShellCb(stream, argc, argv, NULL);
}
AOS_SHELL_COMMAND(moduleTestAt42qt1050ShellCmd, "test:Touch", _moduleTestAt42qt1050ShellCmdCb);

#endif /* (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L1X) */

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

  /* ADC */
  status |= moduleTestAdcShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

  /* AT24C01BN-SH-B (EEPROM) */
  status |= moduleTestAt24c01bShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

  /* bq24103a (battery charger) */
  // front
  targv[1] = "-f";
  status |= moduleTestBq241xxShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  // rear
  targv[1] = "-r";
  status |= moduleTestBq241xxShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  targv[1] = "";

  /* bq27500 (fuel gauge) */
  // front
  targv[1] = "-f";
  status |= moduleTestBq27500ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  // rear
  targv[1] = "-r";
  status |= moduleTestBq27500ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  targv[1] = "";

  /* bq 27500 (fuel gauge) in combination with bq24103a (battery charger) */
  // front
  targv[1] = "-f";
  status |= moduleTestBq27500Bq241xxShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  // rear
  targv[1] = "-r";
  status |= moduleTestBq27500Bq241xxShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  targv[1] = "";

  /* INA219 (power monitor) */
  // VDD
  targv[1] = "VDD";
  status |= moduleTestIna219ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  // VIO 1.8V
  targv[1] = "VIO1.8";
  status |= moduleTestIna219ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  // VIO 3.3V
  targv[1] = "VIO3.3";
  status |= moduleTestIna219ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  // VSYS 4.2V
  targv[1] = "VSYS4.2";
  status |= moduleTestIna219ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  // VIO 5.0V
  targv[1] = "VIO5.0";
  status |= moduleTestIna219ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  targv[1] = "";

  /* status LED */
  status |= moduleTestLedShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

  /* PKLCS1212E4001 (buzzer) */
  status |= moduleTestPkxxxexxxShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

  /* TPS62113 (step-down converter) */
  status |= moduleTestTps6211xShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

  /* TPS62113 (step-down converter) in combination with INA219 (power monitor) */
  status |= moduleTestTps6211xIna219ShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

#if (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR)

  /* MPR121 (touch sensor) */
  status |= moduleTestMpr121ShellCb(stream, 0, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

  /* PCA9544A (I2C multiplexer) */
  // #1
  targv[1] = "#1";
  status |= moduleTestPca9544aShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  // #2
  targv[1] = "#2";
  status |= moduleTestPca9544aShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  targv[1] = "";

  /* VCNL4020 (proximity sensor) */
  // north-northeast
  targv[1] = "-nne";
  status |= moduleTestVcnl4020ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  // east-northeast
  targv[1] = "-ene";
  status |= moduleTestVcnl4020ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  // east-southeast
  targv[1] = "-ese";
  status |= moduleTestVcnl4020ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  // south-southeast
  targv[1] = "-sse";
  status |= moduleTestVcnl4020ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  // south-southwest
  targv[1] = "-ssw";
  status |= moduleTestVcnl4020ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  // west-southwest
  targv[1] = "-wsw";
  status |= moduleTestVcnl4020ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  // west-northwest
  targv[1] = "-wnw";
  status |= moduleTestVcnl4020ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  // west-northwest
  targv[1] = "-nnw";
  status |= moduleTestVcnl4020ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  targv[1] = "";

#endif /* (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR) */

#if (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L0X)

  /* PCAL6524 (GPIO extender) */
  // #1
  targv[1] = "#1";
  status |= moduleTestPcal6524ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  // #2
  targv[1] = "#2";
  status |= moduleTestPcal6524ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  targv[1] = "";

  /* AT42QT1050 (touch sensor) */
  status |= moduleTestAt42qt1050ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

#endif /* (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L0X) */

#if (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L1X)

  /* PCAL6524 (GPIO extender) */
  // #1
  targv[1] = "#1";
  status |= moduleTestPcal6524ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  // #2
  targv[1] = "#2";
  status |= moduleTestPcal6524ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);
  targv[1] = "";

  /* AT42QT1050 (touch sensor) */
  status |= moduleTestAt42qt1050ShellCb(stream, 2, (const char**)targv, &result_test);
  result_total = aosTestResultAdd(result_total, result_test);

#endif /* (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L1X) */

#endif /* (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

  // print total result
  chprintf(stream, "\n");
  aosTestResultPrintSummary(stream, &result_total, "entire module");

  return status;
}
AOS_SHELL_COMMAND(moduleTestAllShellCmd, "test:all", _moduleTestAllShellCmdCb);

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */

/** @} */
