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
 * @file    module.h
 * @brief   Structures and constant for the PowerManagement v1.2 module.
 *
 * @addtogroup powermanagement12
 * @{
 */

#ifndef AMIROOS_MODULE_H
#define AMIROOS_MODULE_H

#include <amiroos.h>

/*===========================================================================*/
/**
 * @name Module specific functions
 * @{
 */
/*===========================================================================*/

/**
 * @brief Makro to store data in the core coupled memory (ccm).
 *        Example:
 *        int compute_buffer[128] CCM_RAM;
 *
 * @note The ccm is not connected to any bus system.
 */
#define CCM_RAM(...)                            __attribute__((section(".ram4"), ##__VA_ARGS__))

/**
 * @brief Makro to store data in the ethernet memory (eth).
 *        Example:
 *        int dma_buffer[128] ETH_RAM;
 *
 * @note The eth is a dedicated memory block with its own DMA controller.
 */
#define ETH_RAM(...)                            __attribute__((section(".ram2"), ##__VA_ARGS__))

/**
 * @brief Makro to store data in the backup memory (bckp).
 *        Example:
 *        int backup_buffer[128] BCKP_RAM;
 *
 * @note The eth is a dedicated memory block with its own DMA controller.
 */
#define BCKP_RAM(...)                           __attribute__((section(".ram5"), ##__VA_ARGS__))

/**
 * @brief   Convert ADC sample data to voltage.
 *
 * @param[in] sample  ADC sample.
 *
 * @return  Voltage in V.
 */
static inline float moduleADC2V(adcsample_t sample)
{
  // apply multiplications according to hard-wired voltage divider
  return (float)sample / (float)ADC_HTR_HT * 3.3f * 5.0f;
}

/**
 * @brief   Convert voltage to ADC sample value.
 *
 * @param[in] volts   Voltage in V.
 *
 * @return  ADC sample value.
 */
static inline adcsample_t moduleV2ADC(float volts)
{
  // apply divisions according to hard-wired voltage divider
  return (adcsample_t)(volts / 5.0f / 3.3f * (float)ADC_HTR_HT + 0.5f);
}

/** @} */

/*===========================================================================*/
/**
 * @name ChibiOS/HAL configuration
 * @{
 */
/*===========================================================================*/

/**
 * @brief   ADC driver for reading the system voltage.
 */
#define MODULE_HAL_ADC_VSYS                     ADCD1

/**
 * @brief   Configuration for the ADC.
 */
extern ADCConversionGroup moduleHalAdcVsysConversionGroup;

/**
 * @brief   CAN driver to use.
 */
#define MODULE_HAL_CAN                          FBCAND1

/**
 * @brief   Configuration for the CAN driver.
 */
extern ROMCONST CANConfig moduleHalCanConfig;

/**
 * @brief   I2C driver to access the sensor ring, power monitors for VIO1.8 and
 *          VIO3.3, and fuel gauge (rear battery).
 * @details Depending on the attached sensor ring, the devices connected to this
 *          bus vary:
 *          - ProximitySensor:
 *            - I2C multiplexer (PCA9544A)
 *            - proximity sensors (VCNL4020) #1 - #4
 *          - DistanceSensor (VL53L0X):
 *            TODO
 *          - DistanceSensor (VL53L1X):
 *            TODO
 */
#define MODULE_HAL_I2C_SR_PM18_PM33_GAUGEREAR   I2CD1

/**
 * @brief   Configuration for the sensor ring, power monitors for VIO1.8 and
 *          VIO3.3, and fuel gauge (rear battery) I2C driver.
 * @details Depending on the attached sensor ring, the devices connected to this
 *          bus vary:
 *          - ProximitySensor:
 *            - I2C multiplexer (PCA9544A)
 *            - proximity sensors (VCNL4020) #1 - #4
 *          - DistanceSensor (VL53L0X):
 *            TODO
 *          - DistanceSensor (VL53L1X):
 *            TODO
 */
extern I2CConfig moduleHalI2cSrPm18Pm33GaugeRearConfig;

/**
 * @brief   I2C driver to access the sensor ring, power monitors for VSYS4.2,
 *          VIO5.0 and VDD, EEPROM, and fuel gauge (front battery).
 * @details Depending on the attached sensor ring, the devices connected to this
 *          bus vary:
 *          - ProximitySensor:
 *            - I2C multiplexer (PCA9544A)
 *            - proximity sensors (VCNL4020) #1 - #4
 *            - touch sensor (MPR121)
 *          - DistanceSensor (VL53L0X):
 *            TODO
 *          - DistanceSensor (VL53L1X):
 *            TODO
 */
#define MODULE_HAL_I2C_SR_PM42_PM50_PMVDD_EEPROM_GAUGEFRONT I2CD2

/**
 * @brief   Configuration for the sensor ring, power monitors for VSYS4.2,
 *          VIO5.0 and VDD, EEPROM, and fuel gauge (front battery) I2C driver.
 * @details Depending on the attached sensor ring, the devices connected to this
 *          bus vary:
 *          - ProximitySensor:
 *            - I2C multiplexer (PCA9544A)
 *            - proximity sensors (VCNL4020) #1 - #4
 *            - touch sensor (MPR121)
 *          - DistanceSensor (VL53L0X):
 *            TODO
 *          - DistanceSensor (VL53L1X):
 *            TODO
 */
extern I2CConfig moduleHalI2cSrPm42Pm50PmVddEepromGaugeFrontConfig;

/**
 * @brief   PWM driver for the buzzer.
 */
#define MODULE_HAL_PWM_BUZZER                   PWMD3

/**
 * @brief   Configuration of the PWM driver.
 */
extern PWMConfig moduleHalPwmBuzzerConfig;

/**
 * @brief   PWM channel for the buzzer.
 */
#define MODULE_HAL_PWM_BUZZER_CHANNEL           1

/**
 * @brief   Real-Time Clock driver.
 */
#define MODULE_HAL_RTC                          RTCD1

/**
 * @brief   SerialCANDriver to be used by the system shell.
 */
extern SerialCANDriver moduleHalSerialCAN;

/**
 * @brief   Configuration for the shell SerialCANDriver.
 */
extern ROMCONST SerialCANConfig moduleHalSerialCanConfig;

#if (AMIROOS_CFG_DBG == true) || defined(__DOXYGEN__)

/**
 * @brief   Serial driver of the programmer interface.
 */
#define MODULE_HAL_SERIAL                       SD1

/**
 * @brief   Configuration for the programmer serial interface driver.
 */
extern ROMCONST SerialConfig moduleHalSerialConfig;

#endif /* (AMIROOS_CFG_DBG == true) */

/** @} */

/*===========================================================================*/
/**
 * @name GPIO definitions
 * @{
 */
/*===========================================================================*/

/**
 * @brief   SWITCH_STATUS input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSwitchStatus;

/**
 * @brief   SYS_REG_EN output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysRegEn;

/**
 * @brief   IR_INT1 input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioIrInt1;

/**
 * @brief   POWER_EN output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioPowerEn;

/**
 * @brief   SYS_UART_DN bidirectional signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysUartDn;

/**
 * @brief   CHARGE_STAT2A input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioChargeStat2A;

/**
 * @brief   GAUGE_BATLOW2 input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioGaugeBatLow2;

/**
 * @brief   GAUGE_BATGD2 input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioGaugeBatGd2;

/**
 * @brief   LED output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioLed;

/**
 * @brief   SYS_UART_UP bidirectional signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysUartUp;

/**
 * @brief   CHARGE_STAT1A input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioChargeStat1A;

/**
 * @brief   GAUGE_BATLOW1 input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioGaugeBatLow1;

/**
 * @brief   GAUGE_BATGD1 input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioGaugeBatGd1;

/**
 * @brief   CHARGE_EN1 output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioChargeEn1;

/**
 * @brief   IR_INT2 input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioIrInt2;

/**
 * @brief   TOUCH_INT input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioTouchInt;

/**
 * @brief   SYS_DONE input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysDone;

/**
 * @brief   SYS_PROG output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysProg;

/**
 * @brief   PATH_DC input signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioPathDc;

/**
 * @brief   SYS_SPI_DIR bidirectional signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysSpiDir;

/**
 * @brief   SYS_SYNC bidirectional signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysSync;

/**
 * @brief   SYS_PD bidirectional signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysPd;

/**
 * @brief   SYS_WARMRST bidirectional signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioSysWarmrst;

/**
 * @brief   BT_RST output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioBtRst;

/**
 * @brief   CHARGE_EN2 output signal GPIO.
 */
extern ROMCONST apalControlGpio_t moduleGpioChargeEn2;

/** @} */

/*===========================================================================*/
/**
 * @name AMiRo-OS core configurations
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Event flag to be set on a IR_INT1 / CHARGE_STAT1A interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_IRINT1          AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_IR_INT1_N))

/**
 * @brief   Event flag to be set on a GAUGE_BATLOW1 interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_GAUGEBATLOW1    AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_GAUGE_BATLOW1))

/**
 * @brief   Event flag to be set on a GAUGE_BATGD1 interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_GAUGEBATGD1     AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_GAUGE_BATGD1_N))

/**
 * @brief   Event flag to be set on a SYS_UART_DN interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_SYSUARTDN       AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_SYS_UART_DN))

/**
 * @brief   Event flag to be set on a IR_INT2 / CHARGE_STAT2A interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_IRINT2          AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_IR_INT2_N))

/**
 * @brief   Event flag to be set on a TOUCH_INT interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_TOUCHINT        AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_TOUCH_INT_N))

/**
 * @brief   Event flag to be set on a GAUGE_BATLOW2 interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_GAUGEBATLOW2    AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_GAUGE_BATLOW2))

/**
 * @brief   Event flag to be set on a GAUGE_BATGD2 interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_GAUGEBATGD2     AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_GAUGE_BATGD2_N))

/**
 * @brief   Event flag to be set on a PATH_DC interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_PATHDC          AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_PATH_DC))

/**
 * @brief   Event flag to be set on a SYS_SPI_DIR interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_SYSSPIDIR       AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_SYS_SPI_DIR))

/**
 * @brief   Event flag to be set on a SYS_SYNC interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_SYSSYNC         AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_SYS_INT_N))

/**
 * @brief   Event flag to be set on a SYS_PD interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_SYSPD           AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_SYS_PD_N))

/**
 * @brief   Event flag to be set on a SYS_WARMRST interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_SYSWARMRST      AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_SYS_WARMRST_N))

/**
 * @brief   Event flag to be set on a SYS_UART_UP interrupt.
 */
#define MODULE_OS_GPIOEVENTFLAG_SYSUARTUP       AOS_GPIOEVENT_FLAG(PAL_PAD(LINE_SYS_UART_UP))

/**
 * @brief   Interrupt initialization hook.
 */
#define MODULE_INIT_INTERRUPTS() {                                              \
  moduleInitInterrupts();                                                       \
}

/**
 * @brief   Periphery communication interfaces initialization hook.
 */
#define MODULE_INIT_PERIPHERY_INTERFACES() {                                    \
  moduleInitPeripheryInterfaces();                                              \
}

#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)
/**
 * @brief   Test initialization hook.
 */
#define MODULE_INIT_TESTS() {                                                   \
  moduleInitTests();                                                            \
}
#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */

/**
 * @brief   Periphery communication interface deinitialization hook.
 */
#define MODULE_SHUTDOWN_PERIPHERY_INTERFACES() {                                \
  moduleShutdownPeripheryInterfaces();                                          \
}

#define AMIROOS_CFG_SYSINFO_HOOK() {                                            \
  _aosSysPrintSystemInfoLine(stream, "built for breakout board", "%s",             \
                            (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_NONE) ? "none" :\
                            (BOARD_BREAKOUT_MODULE == BOARD_BREAKOUT_ESP) ? "ESP32" : \
                            "unknown");                                        \
  _aosSysPrintSystemInfoLine(stream, "built for sensor ring", "%s",             \
                            (BOARD_SENSORRING == BOARD_NOSENSORRING) ? "none" :\
                            (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR) ? "ProximitySensor (VCNL4020)" : \
                            (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L0X) ? "DistanceSensor (VL53L0X)" :  \
                            (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L1X) ? "DistanceSensor (VL53L1X)" :  \
                            "unknown");                                        \
  switch_lld_state_t s;                                                         \
  switch_lld_get(&moduleLldBatterySwitch, &s);                                  \
  _aosSysPrintSystemInfoLine(stream, "Battery Switch", "%s", (s == SWITCH_LLD_STATE_ON) ? "on" : "off");  \
}

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
void moduleInitInterrupts(void);
void moduleInitPeripheryInterfaces(void);
#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)
void moduleInitTests(void);
#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */
void moduleShutdownPeripheryInterfaces(void);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/** @} */

/*===========================================================================*/
/**
 * @name Startup Shutdown Synchronization Protocol (SSSP)
 * @{
 */
/*===========================================================================*/
#if (AMIROOS_CFG_SSSP_ENABLE == true) || defined(__DOXYGEN__)

#define moduleSsspSignalPD()                    (&moduleGpioSysPd)
#define moduleSsspEventflagPD()                 MODULE_OS_GPIOEVENTFLAG_SYSPD

#define moduleSsspSignalS()                     (&moduleGpioSysSync)
#define moduleSsspEventflagS()                  MODULE_OS_GPIOEVENTFLAG_SYSSYNC

#define moduleSsspSignalUP()                    (&moduleGpioSysUartUp)
#define moduleSsspEventflagUP()                 MODULE_OS_GPIOEVENTFLAG_SYSUARTUP

#define moduleSsspSignalDN()                    (&moduleGpioSysUartDn)
#define moduleSsspEventflagDN()                 MODULE_OS_GPIOEVENTFLAG_SYSUARTDN

#if (AMIROOS_CFG_SSSP_MSI == true) || defined(__DOXYGEN__)

struct module_sssp_bcbdata {
  aos_fbcan_filter_t filter;
  CANRxFrame frame;
};

#define MODULE_SSSP_BCBDATA                     struct module_sssp_bcbdata

#endif /* (AMIROOS_CFG_SSSP_MSI == true) */

#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */
/** @} */

/*===========================================================================*/
/**
 * @name AMiRo-OS shell configurations
 * @{
 */
/*===========================================================================*/
#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)

/**
 * @brief   Shell prompt text.
 */
extern ROMCONST char* moduleShellPrompt;

/**
 * @brief   Shell stream channel for CAN I/O.
 */
extern AosShellChannel moduleShellSerialCanChannel;

/**
 * @brief   Shell setup hook.
 */
#define MODULE_SHELL_SETUP_HOOK() {                                             \
  aosShellChannelInit(&moduleShellSerialCanChannel, (BaseAsynchronousChannel*)&moduleHalSerialCAN, AOS_SHELLCHANNEL_REMOTE); \
  aosShellStreamAddChannel(&aos.shell.stream, &moduleShellSerialCanChannel);    \
}

#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */
/** @} */

/*===========================================================================*/
/**
 * @name Low-level drivers
 * @{
 */
/*===========================================================================*/
#include <alld_AT24C01B.h>
#include <alld_bq241xx.h>
#include <alld_bq27500.h>
#include <alld_INA219.h>
#include <alld_LED.h>
#include <alld_PKxxxExxx.h>
#include <alld_switch.h>
#include <alld_TPS6211x.h>

/**
 * @brief   EEPROM driver.
 */
extern AT24C01BDriver moduleLldEeprom;

/**
 * @brief   Battery charger (front battery) driver.
 */
extern BQ241xxDriver moduleLldBatteryChargerFront;

/**
 * @brief   Battery charger (rear battery) driver.
 */
extern BQ241xxDriver moduleLldBatteryChargerRear;

/**
 * @brief   Fuel gauge (front battery) driver.
 */
extern BQ27500Driver moduleLldFuelGaugeFront;

/**
 * @brief   Fuel gauge (rear battery) driver.
 */
extern BQ27500Driver moduleLldFuelGaugeRear;

/**
 * @brief   Power monitor (VDD) driver.
 */
extern INA219Driver moduleLldPowerMonitorVdd;

/**
 * @brief   Power monitor (VIO 1.8) driver.
 */
extern INA219Driver moduleLldPowerMonitorVio18;

/**
 * @brief   Power monitor (VIO 3.3) driver.
 */
extern INA219Driver moduleLldPowerMonitorVio33;

/**
 * @brief   Power monitor (VSYS 4.2) driver.
 */
extern INA219Driver moduleLldPowerMonitorVsys42;

/**
 * @brief   Power monitor (VIO 5.0) driver.
 */
extern INA219Driver moduleLldPowerMonitorVio50;

/**
 * @brief   Status LED driver.
 */
extern LEDDriver moduleLldStatusLed;

/**
 * @brief   Battery switch driver.
 */
extern SwitchDriver moduleLldBatterySwitch;

/**
 * @brief   Step down converter driver.
 * @note    Although there multiple TPS6211x, those are completely identical
 *          from driver few (share the same signals).
 */
extern TPS6211xDriver moduleLldStepDownConverter;

#if (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR) || defined(__DOXYGEN__)

#include <alld_MPR121.h>
#include <alld_PCA9544A.h>
#include <alld_VCNL4020.h>

/**
 * @brief   Touch sensor driver.
 */
extern MPR121Driver moduleLldTouch;

/**
 * @brief   I2C multiplexer (I2C #1) driver.
 */
extern PCA9544ADriver moduleLldI2cMultiplexer1;

/**
 * @brief   I2C multiplexer (I2C #2) driver.
 */
extern PCA9544ADriver moduleLldI2cMultiplexer2;

/**
 * @brief   Proximity sensor (I2C #1) driver.
 */
extern VCNL4020Driver moduleLldProximity1;

/**
 * @brief   Proximity sensor (I2C #2) driver.
 */
extern VCNL4020Driver moduleLldProximity2;

#endif /* (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR) */

#if (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L0X) || defined(__DOXYGEN__)

#include <alld_PCAL6524.h>
#include <alld_AT42QT1050.h>

/**
 * @brief   GPIO extender (I2C #1) driver.
 */
extern PCAL6524Driver moduleLldGpioExtender1;

/**
 * @brief   GPIO extender (I2C #2) driver.
 */
extern PCAL6524Driver moduleLldGpioExtender2;

/**
 * @brief   Touch sensor driver.
 */
extern AT42QT1050Driver moduleLldTouch;

#endif /* (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L0X) */

#if (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L1X) || defined(__DOXYGEN__)

#include <alld_PCAL6524.h>
#include <alld_AT42QT1050.h>

/**
 * @brief   GPIO extender (I2C #1) driver.
 */
extern PCAL6524Driver moduleLldGpioExtender1;

/**
 * @brief   GPIO extender (I2C #2) driver.
 */
extern PCAL6524Driver moduleLldGpioExtender2;

/**
 * @brief   Touch sensor driver.
 */
extern AT42QT1050Driver moduleLldTouch;

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

/**
 * @brief   ADC test command.
 */
extern aos_shellcommand_t moduleTestAdcShellCmd;

/**
 * @brief   AT24C01BN-SH-B (EEPROM) test command.
 */
extern aos_shellcommand_t moduleTestAt24c01bShellCmd;

/**
 * @brief   bq24103a (battery charger) test command.
 */
extern aos_shellcommand_t moduleTestBq241xxShellCmd;

/**
 * @brief   bq27500 (fuel gauge) test command.
 */
extern aos_shellcommand_t moduleTestBq27500ShellCmd;

/**
 * @brief   bq27500 (fuel gauge) in combination with bq24103a (battery charger)
 *          test command.
 */
extern aos_shellcommand_t moduleTestBq27500Bq241xxShellCmd;

/**
 * @brief   INA219 (power monitor) test command.
 */
extern aos_shellcommand_t moduleTestIna219ShellCmd;

/**
 * @brief   Status LED test command.
 */
extern aos_shellcommand_t moduleTestLedShellCmd;

/**
 * @brief   PKLCS1212E4001 (buzzer) test command.
 */
extern aos_shellcommand_t moduleTestPkxxxexxxShellCmd;

/**
 * @brief   Switch test command.
 */
extern aos_shellcommand_t moduleTestSwitchShellCmd;

/**
 * @brief   TPS62113 (step-down converter) test command.
 */
extern aos_shellcommand_t moduleTestTps6211xShellCmd;

/**
 * @brief   TPS62113 (step-sown converter) in combination with INA219 (power
 *          monitor) test command.
 */
extern aos_shellcommand_t moduleTestTps6211xIna219ShellCmd;

#if (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR) || defined(__DOXYGEN__)

/**
 * @brief   MPR121 (touch sensor) test command.
 */
extern aos_shellcommand_t moduleTestMpr121ShellCmd;

/**
 * @brief   PCA9544A (I2C multiplexer) test command.
 */
extern aos_shellcommand_t moduleTestPca9544aShellCmd;

/**
 * @brief   VCNL4020 (proximity sensor) test command.
 */
extern aos_shellcommand_t moduleTestVcnl4020ShellCmd;

#endif /* (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR) */

#if (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L0X) || defined(__DOXYGEN__)

/**
 * @brief   PCAL6524 (GPIO extender) test command.
 */
extern aos_shellcommand_t moduleTestPcal6524ShellCmd;

/**
 * @brief   AT42QT1050 (touch sensor) test command.
 */
extern aos_shellcommand_t moduleTestAt42qt1050ShellCmd;

#endif /* (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L0X) */

#if (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L1X) || defined(__DOXYGEN__)

/**
 * @brief   PCAL6524 (GPIO extender) test command.
 */
extern aos_shellcommand_t moduleTestPcal6524ShellCmd;

/**
 * @brief   AT42QT1050 (touch sensor) test command.
 */
extern aos_shellcommand_t moduleTestAt42qt1050ShellCmd;

#endif /* (BOARD_SENSORRING == BOARD_DISTANCESENSOR_VL53L1X) */

#endif /* (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/**
 * @brief   Entire module test command.
 */
extern aos_shellcommand_t moduleTestAllShellCmd;

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */

/** @} */

#endif /* AMIROOS_MODULE_H */

/** @} */
