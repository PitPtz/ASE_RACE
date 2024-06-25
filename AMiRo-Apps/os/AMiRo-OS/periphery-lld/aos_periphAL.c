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
 * @file    aos_periphAL.c
 * @brief   Periphery abstraction layer implementation.
 *
 * @addtogroup lldlib
 * @{
 */

#include <amiroos.h>

/*============================================================================*/
/* DEBUG                                                                      */
/*============================================================================*/
/** @cond */

#if (AMIROLLD_CFG_DBG == true)
#if (AMIROOS_CFG_DBG == true)

void apalDbgAssertMsg(const bool c, const char* fmt, ...)
{
  if (!c) {
    va_list ap;

    va_start(ap, fmt);
    chvprintf((BaseSequentialStream*)&aos.iostream, fmt, ap);
    va_end(ap);
    chThdExit(MSG_RESET);
  }

  return;
}

int apalDbgPrintf(const char* fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  const int chars = chvprintf((BaseSequentialStream*)&aos.iostream, fmt, ap);
  va_end(ap);

  return chars;
}

#endif /* (AMIROOS_CFG_DBG == true) */
#endif /* (AMIROLLD_CFG_DBG == true) */

/** @endcond */

/*============================================================================*/
/* TIMING                                                                     */
/*============================================================================*/
/** @cond */

#if (AMIROOS_CFG_DBG == true)

void apalSleep(apalTime_t us)
{
  // check if the specified time can be represented by the system
  aosDbgCheck(us <= chTimeI2US(TIME_INFINITE));

  const sysinterval_t interval = chTimeUS2I(us);
  // TIME_IMMEDIATE makes no sense and would even cause system halt
  if (interval != TIME_IMMEDIATE) {
    chThdSleep(interval);
  }

  return;
}

#endif /* (AMIROOS_CFG_DBG == true) */

/** @endcond */

/*============================================================================*/
/* GPIO                                                                       */
/*============================================================================*/
/** @cond */

#if (HAL_USE_PAL == TRUE)

apalExitStatus_t apalGpioRead(apalGpio_t* gpio, apalGpioState_t* const val)
{
  aosDbgCheck(gpio != NULL);
  aosDbgCheck(val != NULL);

  *val = (palReadLine(gpio->line) == PAL_HIGH) ? APAL_GPIO_HIGH : APAL_GPIO_LOW;

  return APAL_STATUS_OK;
}

apalExitStatus_t apalGpioWrite(apalGpio_t* gpio, const apalGpioState_t val)
{
  aosDbgCheck(gpio != NULL);

  // palWriteLine() is not guaranteed to be atomic, thus the scheduler is locked.
  syssts_t sysstatus = chSysGetStatusAndLockX();
  palWriteLine(gpio->line, (val == APAL_GPIO_HIGH) ? PAL_HIGH : PAL_LOW);
  chSysRestoreStatusX(sysstatus);

  return APAL_STATUS_OK;
}

apalExitStatus_t apalGpioToggle(apalGpio_t* gpio)
{
  aosDbgCheck(gpio != NULL);

  // palWriteLine() is not guaranteed to be atomic, thus the scheduler is locked.
  syssts_t sysstatus = chSysGetStatusAndLockX();
  palWriteLine(gpio->line, (palReadLine(gpio->line) == PAL_HIGH) ? PAL_LOW : PAL_HIGH);
  chSysRestoreStatusX(sysstatus);

  return APAL_STATUS_OK;
}

apalExitStatus_t apalGpioIsInterruptEnabled(apalGpio_t* gpio, bool* const enabled)
{
  aosDbgCheck(gpio != NULL);
  aosDbgCheck(enabled != NULL);

  *enabled = palIsLineEventEnabledX(gpio->line);

  return APAL_STATUS_OK;
}

apalExitStatus_t apalControlGpioGet(const apalControlGpio_t* const cgpio, apalControlGpioState_t* const val)
{
  aosDbgCheck(cgpio != NULL);
  aosDbgCheck(cgpio->gpio != NULL);
  aosDbgCheck(val != NULL);

  *val = ((palReadLine(cgpio->gpio->line) == PAL_HIGH) ^ (cgpio->meta.active == APAL_GPIO_ACTIVE_HIGH)) ? APAL_GPIO_OFF : APAL_GPIO_ON;

  return APAL_STATUS_OK;
}

apalExitStatus_t apalControlGpioSet(const apalControlGpio_t* const cgpio, const apalControlGpioState_t val)
{
  aosDbgCheck(cgpio != NULL);
  aosDbgCheck(cgpio->gpio != NULL);
  aosDbgCheck(cgpio->meta.direction == APAL_GPIO_DIRECTION_OUTPUT || cgpio->meta.direction == APAL_GPIO_DIRECTION_BIDIRECTIONAL);

  // palWriteLine() is not guaranteed to be atomic, thus the scheduler is locked.
  syssts_t sysstatus = chSysGetStatusAndLockX();
  palWriteLine(cgpio->gpio->line, ((cgpio->meta.active == APAL_GPIO_ACTIVE_HIGH) ^ (val == APAL_GPIO_ON)) ? PAL_LOW : PAL_HIGH);
  chSysRestoreStatusX(sysstatus);

  return APAL_STATUS_OK;
}

apalExitStatus_t apalControlGpioSetInterrupt(const apalControlGpio_t* const cgpio, const bool enable)
{
  aosDbgCheck(cgpio != NULL);
  aosDbgCheck(cgpio->gpio != NULL);

  if (enable) {
    aosDbgAssert(pal_lld_get_line_event(cgpio->gpio->line) != NULL);
    palEnableLineEvent(cgpio->gpio->line, APAL2CH_EDGE(cgpio->meta.edge));
  } else {
    palDisableLineEvent(cgpio->gpio->line);
  }

  return APAL_STATUS_OK;
}

#endif /* (HAL_USE_PAL == TRUE) */

/** @endcond */

/*============================================================================*/
/* PWM                                                                        */
/*============================================================================*/
/** @cond */

#if (HAL_USE_PWM == TRUE)

apalExitStatus_t apalPWMSet(apalPWMDriver_t* pwm, const apalPWMchannel_t channel, const apalPWMwidth_t width)
{
  aosDbgCheck(pwm != NULL);

  pwmEnableChannel(pwm, (pwmchannel_t)channel, pwm->period * ((float)width / (float)APAL_PWM_WIDTH_MAX) + 0.5f);

  return APAL_STATUS_OK;
}

apalExitStatus_t apalPWMGetFrequency(apalPWMDriver_t* pwm, apalPWMfrequency_t* const frequency)
{
  aosDbgCheck(pwm != NULL);
  aosDbgCheck(frequency != NULL);

  *frequency = pwm->config->frequency;

  return APAL_STATUS_OK;
}

apalExitStatus_t apalPWMGetPeriod(apalPWMDriver_t* pwm, apalPWMperiod_t* const period)
{
  aosDbgCheck(pwm != NULL);
  aosDbgCheck(period != NULL);

  *period = pwm->period;

  return APAL_STATUS_OK;
}

#endif /* (HAL_USE_PWM == TRUE) */

/** @endcond */

/*============================================================================*/
/* QEI                                                                        */
/*============================================================================*/
/** @cond */

#if (HAL_USE_QEI == TRUE)

apalExitStatus_t apalQEIGetPosition(apalQEIDriver_t* qei, apalQEICount_t* const position)
{
  aosDbgCheck(qei != NULL);
  aosDbgCheck(position != NULL);

  *position = qeiGetCount(qei);

  return APAL_STATUS_OK;
}

apalExitStatus_t apalQEIGetDelta(apalQEIDriver_t* qei, apalQEIDiff_t* const delta)
{
  aosDbgCheck(qei != NULL);
  aosDbgCheck(delta != NULL);

  *delta = qeiUpdate(qei);

  return APAL_STATUS_OK;
}

apalExitStatus_t apalQEIGetRange(apalQEIDriver_t* qei, apalQEICount_t* const range)
{
  aosDbgCheck(qei != NULL);
  aosDbgCheck(range != NULL);

  *range = qei->config->max - qei->config->min;

  return APAL_STATUS_OK;
}

#endif /* (HAL_USE_QEI == TRUE) */

/** @endcond */

/*============================================================================*/
/* I2C                                                                        */
/*============================================================================*/
/** @cond */

#if (HAL_USE_I2C == TRUE) || defined(__DOXYGEN__)

apalExitStatus_t apalI2CMasterTransmit(apalI2CDriver_t* i2cd, const apalI2Caddr_t addr, const uint8_t* const txbuf, const size_t txbytes, uint8_t* const rxbuf, const size_t rxbytes, const apalTime_t timeout)
{
  aosDbgCheck(i2cd != NULL);

#if (I2C_USE_MUTUAL_EXCLUSION == TRUE)
  // check whether the I2C driver was locked externally
  const bool i2cd_locked_external = i2cd->mutex.owner == chThdGetSelfX();
  if (!i2cd_locked_external) {
    i2cAcquireBus(i2cd);
  }
#endif /* (I2C_USE_MUTUAL_EXCLUSION == TRUE) */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#if defined(STM32F1XX_I2C)
  // Due to a hardware limitation, for STM32F1 platform the minimum number of bytes that can be received is two.
  msg_t status = MSG_OK;
  if (rxbytes == 1) {
    uint8_t buffer[2];
    status = i2cMasterTransmitTimeout(i2cd, addr, txbuf, txbytes, buffer, 2, ((timeout >= TIME_INFINITE) ? TIME_INFINITE : TIME_US2I(timeout)) );
    rxbuf[0] = buffer[0];
  } else {
    status = i2cMasterTransmitTimeout(i2cd, addr, txbuf, txbytes, rxbuf, rxbytes, ((timeout >= TIME_INFINITE) ? TIME_INFINITE : TIME_US2I(timeout)) );
  }
#else /* defined(STM32F1XX_I2C) */
  const msg_t status = i2cMasterTransmitTimeout(i2cd, addr, txbuf, txbytes, rxbuf, rxbytes, ((timeout >= TIME_INFINITE) ? TIME_INFINITE : TIME_US2I(timeout)) );
#endif /* defined(STM32F1XX_I2C) */
#pragma GCC diagnostic pop

#if (I2C_USE_MUTUAL_EXCLUSION == TRUE)
  if (!i2cd_locked_external) {
    i2cReleaseBus(i2cd);
  }
#endif /* (I2C_USE_MUTUAL_EXCLUSION == TRUE) */

  switch (status)
  {
    case MSG_OK:
#if defined(STM32F1XX_I2C)
      return (rxbytes != 1) ? APAL_STATUS_OK : APAL_STATUS_IO;
#else /* defined(STM32F1XX_I2C) */
      return APAL_STATUS_OK;
#endif /* defined(STM32F1XX_I2C) */
    case MSG_TIMEOUT:
      return APAL_STATUS_ERROR | APAL_STATUS_TIMEOUT;
    case MSG_RESET:
    default:
      return APAL_STATUS_ERROR;
  }
}

apalExitStatus_t apalI2CMasterReceive(apalI2CDriver_t* i2cd, const apalI2Caddr_t addr, uint8_t* const rxbuf, const size_t rxbytes, const apalTime_t timeout)
{
  aosDbgCheck(i2cd != NULL);

#if (I2C_USE_MUTUAL_EXCLUSION == TRUE)
  // check whether the I2C driver was locked externally
  const bool i2cd_locked_external = i2cd->mutex.owner == chThdGetSelfX();
  if (!i2cd_locked_external) {
    i2cAcquireBus(i2cd);
  }
#endif /* (I2C_USE_MUTUAL_EXCLUSION == TRUE) */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#if defined(STM32F1XX_I2C)
  // Due to a hardware limitation, for STM32F1 platform the minimum number of bytes that can be received is two.
  msg_t status = MSG_OK;
  if (rxbytes == 1) {
    uint8_t buffer[2];
    status = i2cMasterReceiveTimeout(i2cd, addr, buffer, 2, ((timeout >= TIME_INFINITE) ? TIME_INFINITE : TIME_US2I(timeout)) );
    rxbuf[0] = buffer[0];
  } else {
    status = i2cMasterReceiveTimeout(i2cd, addr, rxbuf, rxbytes, ((timeout >= TIME_INFINITE) ? TIME_INFINITE : TIME_US2I(timeout)) );
  }
#else /* defined(STM32F1XX_I2C) */
  const msg_t status = i2cMasterReceiveTimeout(i2cd, addr, rxbuf, rxbytes, ((timeout >= TIME_INFINITE) ? TIME_INFINITE : TIME_US2I(timeout)) );
#endif /* defined(STM32F1XX_I2C) */
#pragma GCC diagnostic pop

#if (I2C_USE_MUTUAL_EXCLUSION == TRUE)
  if (!i2cd_locked_external) {
    i2cReleaseBus(i2cd);
  }
#endif /* (I2C_USE_MUTUAL_EXCLUSION == TRUE) */

  switch (status)
  {
    case MSG_OK:
#if defined(STM32F1XX_I2C)
      return (rxbytes != 1) ? APAL_STATUS_OK : APAL_STATUS_IO;
#else /* defined(STM32F1XX_I2C) */
      return APAL_STATUS_OK;
#endif /* defined(STM32F1XX_I2C) */
    case MSG_TIMEOUT:
      return APAL_STATUS_ERROR | APAL_STATUS_TIMEOUT;
    case MSG_RESET:
    default:
      return APAL_STATUS_ERROR;
  }
}

#endif /* (HAL_USE_I2C == TRUE) */

/** @endcond */

/*============================================================================*/
/* SPI                                                                        */
/*============================================================================*/
/** @cond */

#if (HAL_USE_SPI == TRUE) || defined(__DOXYGEN__)

apalExitStatus_t apalSPITransmit(apalSPIDriver_t* spid, const uint8_t* const data, const size_t length)
{
  aosDbgCheck(spid != NULL);

#if (SPI_USE_MUTUAL_EXCLUSION == TRUE)
  // check whether the SPI driver was locked externally
  const bool spid_locked_external = spid->mutex.owner == chThdGetSelfX();
  if (!spid_locked_external) {
    spiAcquireBus(spid);
  }
#endif /* (SPI_USE_MUTUAL_EXCLUSION == TRUE) */

  spiSelect(spid);
  spiSend(spid, length, data);
  spiUnselect(spid);

#if (SPI_USE_MUTUAL_EXCLUSION == TRUE)
  if (!spid_locked_external) {
    spiReleaseBus(spid);
  }
#endif /* (SPI_USE_MUTUAL_EXCLUSION == TRUE) */

  return APAL_STATUS_OK;
}

apalExitStatus_t apalSPIReceive(apalSPIDriver_t* spid, uint8_t* const data, const size_t length)
{
  aosDbgCheck(spid != NULL);

#if (SPI_USE_MUTUAL_EXCLUSION == TRUE)
  // check whether the SPI driver was locked externally
  const bool spid_locked_external = spid->mutex.owner == chThdGetSelfX();
  if (!spid_locked_external) {
    spiAcquireBus(spid);
  }
#endif /* (SPI_USE_MUTUAL_EXCLUSION == TRUE) */

  spiSelect(spid);
  spiReceive(spid, length, data);
  spiUnselect(spid);

#if (SPI_USE_MUTUAL_EXCLUSION == TRUE)
  if (!spid_locked_external) {
    spiReleaseBus(spid);
  }
#endif /* (SPI_USE_MUTUAL_EXCLUSION == TRUE) */

  return APAL_STATUS_OK;
}

apalExitStatus_t apalSPITransmitAndReceive(apalSPIDriver_t* spid, const uint8_t* const txData , uint8_t* const rxData, const size_t txLength, const size_t rxLength)
{
  aosDbgCheck(spid != NULL);

#if (SPI_USE_MUTUAL_EXCLUSION == TRUE)
  // check whether the SPI driver was locked externally
  const bool spid_locked_external = spid->mutex.owner == chThdGetSelfX();
  if (!spid_locked_external) {
    spiAcquireBus(spid);
  }
#endif /* (SPI_USE_MUTUAL_EXCLUSION == TRUE) */

  spiSelect(spid);
  spiSend(spid, txLength, txData);
  spiReceive(spid, rxLength, rxData);
  spiUnselect(spid);

#if (SPI_USE_MUTUAL_EXCLUSION == TRUE)
  if (!spid_locked_external) {
    spiReleaseBus(spid);
  }
#endif /* (SPI_USE_MUTUAL_EXCLUSION == TRUE) */

  return APAL_STATUS_OK;
}

apalExitStatus_t apalSPIExchange(apalSPIDriver_t* spid, const uint8_t* const txData , uint8_t* const rxData, const size_t length)
{
  aosDbgCheck(spid != NULL);

#if (SPI_USE_MUTUAL_EXCLUSION == TRUE)
  // check whether the SPI driver was locked externally
  const bool spid_locked_external = spid->mutex.owner == chThdGetSelfX();
  if (!spid_locked_external) {
    spiAcquireBus(spid);
  }
#endif /* (SPI_USE_MUTUAL_EXCLUSION == TRUE) */

  spiSelect(spid);
  spiExchange(spid, length, txData, rxData);
  spiUnselect(spid);

#if (SPI_USE_MUTUAL_EXCLUSION == TRUE)
  if (!spid_locked_external) {
    spiReleaseBus(spid);
  }
#endif /* (SPI_USE_MUTUAL_EXCLUSION == TRUE) */

  return APAL_STATUS_OK;
}

apalExitStatus_t apalSPIReconfigure(apalSPIDriver_t* spid, const apalSPIConfig_t* config)
{
  aosDbgCheck(spid != NULL);
  aosDbgCheck(config != NULL);

#if (SPI_USE_MUTUAL_EXCLUSION == TRUE)
  // check whether the SPI driver was locked externally
  const bool spid_locked_external = spid->mutex.owner == chThdGetSelfX();
  if (!spid_locked_external) {
    spiAcquireBus(spid);
  }
#endif /* (SPI_USE_MUTUAL_EXCLUSION == TRUE) */

  spiStop(spid);
  spiStart(spid, config);

#if (SPI_USE_MUTUAL_EXCLUSION == TRUE)
  if (!spid_locked_external) {
    spiReleaseBus(spid);
  }
#endif /* (SPI_USE_MUTUAL_EXCLUSION == TRUE) */

  return APAL_STATUS_OK;
}

#endif /* (HAL_USE_SPI == TRUE) */

/** @endcond */

/** @} */
