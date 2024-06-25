/*
AMiRo-LLD is a compilation of low-level hardware drivers for the Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2016..2022  Thomas Schöpping et al.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    alld_VL53L1X.c
 * @brief   ToF sensor function implementations.
 *
 * @addtogroup lld_VL53L1X_v1
 * @{
 */

#include <alld_VL53L1X.h>
#include <vl53l1_api.h>
#include <string.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/**
 * @brief   I2C communication timeout (in microseconds).
 * @details Cutsom values can be set via alldconf.h file.
 */
#if !defined(VL53L1X_LLD_I2C_TIMEOUT) || defined(__DOXYGEN__)
# define VL53L1X_LLD_I2C_TIMEOUT               ((apalTime_t)5000)
#endif

/******************************************************************************/
/* EXPORTED VARIABLES                                                         */
/******************************************************************************/

/******************************************************************************/
/* LOCAL TYPES                                                                */
/******************************************************************************/

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

/* interface functions for ST API *********************************************/

/**
 * @brief   Initialization of communication interface (I2C).
 * @details Function not supported.
 *          Initialization must be done by OS or application instead.
 *
 * @param[in] pdev              VL53L1X device handle.
 * @param[in] comms_type        I2C speed class (?).
 * @param[in] comms_speed_khz   I2C transmission frequency.
 *
 * @return  Always returns VL53L1_ERROR_NOT_SUPPORTED.
 */
VL53L1_Error VL53L1_CommsInitialise(VL53L1_Dev_t *pdev, uint8_t comms_type, uint16_t comms_speed_khz)
{
  apalDbgAssert(pdev != NULL);

  (void)pdev;
  (void)comms_type;
  (void)comms_speed_khz;

  return VL53L1_ERROR_NOT_SUPPORTED;
}

/**
 * @brief   Deinitialization of communication interface (I2C).
 * @details Function not supported.
 *          Deinitialization must be done by OS or application instead.
 *
 * @param[in] pdev  VL53L1X device handle.
 *
 * @return  Always returns VL53L1_ERROR_NOT_SUPPORTED.
 */
VL53L1_Error VL53L1_CommsClose(VL53L1_Dev_t *pdev)
{
  apalDbgAssert(pdev != NULL);

  (void)pdev;

  return VL53L1_ERROR_NOT_SUPPORTED;
}

/**
 * @brief   Transmit multiple bytes of data via I2C.
 *
 * @param[in] pdev    Device handle.
 * @param[in] index   Register address to write to.
 * @param[in] pdata   Data buffer to be transmitted.
 * @param[in] count   Number of bytes to transmit.
 *
 * @return  Status indicating whether the call was successful.
 * @retval  VL53L1_ERROR_NONE       Transmission was successful.
 * @retval  VL53L1_ERROR_TIME_OUT   Transmission timed out.
 * @retval  VL53L1_ERROR_UNDEFINED  Transmission failed for some other reason.
 */
VL53L1_Error VL53L1_WriteMulti(VL53L1_Dev_t *pdev, uint16_t index, uint8_t *pdata, uint32_t count)
{
  apalDbgAssert(pdev != NULL);

  // prepare transmit buffer with prepended address bytes.
  uint8_t txbuf[sizeof(uint16_t) + count];
  txbuf[0] = index >> 8;
  txbuf[1] = index & 0xFF;
  memcpy(&txbuf[2], pdata, count);

  // transmit address and data
  apalExitStatus_t status = apalI2CMasterTransmit(pdev->Interface.i2cd, pdev->Interface.addr, txbuf, sizeof(txbuf), NULL, 0, VL53L1X_LLD_I2C_TIMEOUT);

  return (status == APAL_STATUS_OK || status > 0) ? VL53L1_ERROR_NONE :
         (status == APAL_STATUS_TIMEOUT) ? VL53L1_ERROR_TIME_OUT : VL53L1_ERROR_UNDEFINED;
}

/**
 * @brief   Read multiple bytes of data via I2C.
 *
 * @param[in]   pdev    Device handle.
 * @param[in]   index   Register address to read from.
 * @param[out]  pdata   Data buffer to store read data to.
 * @param[in]   count   Number of bytes to read.
 *
 * @return  Status indicating whether the call was successful.
 * @retval  VL53L1_ERROR_NONE       Transmission was successful.
 * @retval  VL53L1_ERROR_TIME_OUT   Transmission timed out.
 * @retval  VL53L1_ERROR_UNDEFINED  Transmission failed for some other reason.
 */
VL53L1_Error VL53L1_ReadMulti(VL53L1_Dev_t *pdev, uint16_t index, uint8_t *pdata, uint32_t count)
{
  apalDbgAssert(pdev != NULL);

  // prepare address buffer
  uint8_t txbuf[sizeof(uint16_t)] = {index >> 8, index & 0xFF};

  // transmit address and recieve data
  apalExitStatus_t status = apalI2CMasterTransmit(pdev->Interface.i2cd, pdev->Interface.addr, txbuf, sizeof(txbuf), pdata, count, VL53L1X_LLD_I2C_TIMEOUT);

  return (status == APAL_STATUS_OK || status > 0) ? VL53L1_ERROR_NONE :
         (status == APAL_STATUS_TIMEOUT) ? VL53L1_ERROR_TIME_OUT : VL53L1_ERROR_UNDEFINED;
}

/**
 * @brief   Transmit a single byte via I2C.
 *
 * @param[in] pdev              Device handle.
 * @param[in] index             Register address to write to.
 * @param[in] VL53L1_PRM_00005  Byte to transmit.
 *
 * @return  Status indicating whether the call was successful.
 * @retval  VL53L1_ERROR_NONE       Transmission was successful.
 * @retval  VL53L1_ERROR_TIME_OUT   Transmission timed out.
 * @retval  VL53L1_ERROR_UNDEFINED  Transmission failed for some other reason.
 */
VL53L1_Error VL53L1_WrByte(VL53L1_Dev_t *pdev, uint16_t index, uint8_t VL53L1_PRM_00005)
{
  apalDbgAssert(pdev != NULL);

  // prepare transmit buffer with prepended address bytes.
  uint8_t txbuf[sizeof(uint16_t) + sizeof(uint8_t)] = {index >> 8, index & 0xFF,
                                                       VL53L1_PRM_00005};

  // transmit address and data
  apalExitStatus_t status = apalI2CMasterTransmit(pdev->Interface.i2cd, pdev->Interface.addr, txbuf, sizeof(txbuf), NULL, 0, VL53L1X_LLD_I2C_TIMEOUT);

  return (status == APAL_STATUS_OK || status > 0) ? VL53L1_ERROR_NONE :
         (status == APAL_STATUS_TIMEOUT) ? VL53L1_ERROR_TIME_OUT : VL53L1_ERROR_UNDEFINED;
}

/**
 * @brief   Transmit a 16-bit data word via I2C.
 *
 * @param[in] pdev              Device handle.
 * @param[in] index             Register address to write to.
 * @param[in] VL53L1_PRM_00005  16-bit data word to transmit.
 *
 * @return  Status indicating whether the call was successful.
 * @retval  VL53L1_ERROR_NONE       Transmission was successful.
 * @retval  VL53L1_ERROR_TIME_OUT   Transmission timed out.
 * @retval  VL53L1_ERROR_UNDEFINED  Transmission failed for some other reason.
 */
VL53L1_Error VL53L1_WrWord(VL53L1_Dev_t *pdev, uint16_t index, uint16_t VL53L1_PRM_00005)
{
  apalDbgAssert(pdev != NULL);

  // prepare transmit buffer with prepended address bytes.
  uint8_t txbuf[sizeof(uint16_t) + sizeof(uint16_t)] = {index >> 8, index & 0xFF,
                                                        (VL53L1_PRM_00005 >> 8) & 0xFF,
                                                        (VL53L1_PRM_00005 >> 0) & 0xFF};

  // transmit address and data
  apalExitStatus_t status = apalI2CMasterTransmit(pdev->Interface.i2cd, pdev->Interface.addr, txbuf, sizeof(txbuf), NULL, 0, VL53L1X_LLD_I2C_TIMEOUT);

  return (status == APAL_STATUS_OK || status > 0) ? VL53L1_ERROR_NONE :
         (status == APAL_STATUS_TIMEOUT) ? VL53L1_ERROR_TIME_OUT : VL53L1_ERROR_UNDEFINED;
}

/**
 * @brief   Transmit a 32-bit data word via I2C.
 *
 * @param[in] pdev              Device handle.
 * @param[in] index             Register address to write to.
 * @param[in] VL53L1_PRM_00005  32-bit data word to transmit.
 *
 * @return  Status indicating whether the call was successful.
 * @retval  VL53L1_ERROR_NONE       Transmission was successful.
 * @retval  VL53L1_ERROR_TIME_OUT   Transmission timed out.
 * @retval  VL53L1_ERROR_UNDEFINED  Transmission failed for some other reason.
 */
VL53L1_Error VL53L1_WrDWord(VL53L1_Dev_t *pdev,uint16_t index, uint32_t VL53L1_PRM_00005)
{
  apalDbgAssert(pdev != NULL);

  // prepare transmit buffer with prepended address bytes.
  uint8_t txbuf[sizeof(uint16_t) + sizeof(uint32_t)] = {index >> 8, index & 0xFF,
                                                        (VL53L1_PRM_00005 >> 24) & 0xFF,
                                                        (VL53L1_PRM_00005 >> 16) & 0xFF,
                                                        (VL53L1_PRM_00005 >> 8) & 0xFF,
                                                        (VL53L1_PRM_00005 >> 0) & 0xFF};

  // transmit address and data
  apalExitStatus_t status = apalI2CMasterTransmit(pdev->Interface.i2cd, pdev->Interface.addr, txbuf, sizeof(txbuf), NULL, 0, VL53L1X_LLD_I2C_TIMEOUT);

  return (status == APAL_STATUS_OK || status > 0) ? VL53L1_ERROR_NONE :
         (status == APAL_STATUS_TIMEOUT) ? VL53L1_ERROR_TIME_OUT : VL53L1_ERROR_UNDEFINED;
}

/**
 * @brief   Read a single byte via I2C.
 *
 * @param[in]   pdev    Device handle.
 * @param[in]   index   Register address to read from.
 * @param[out]  pdata   Single byte buffer to store the read data to.
 *
 * @return  Status indicating whether the call was successful.
 * @retval  VL53L1_ERROR_NONE       Transmission was successful.
 * @retval  VL53L1_ERROR_TIME_OUT   Transmission timed out.
 * @retval  VL53L1_ERROR_UNDEFINED  Transmission failed for some other reason.
 */
VL53L1_Error VL53L1_RdByte(VL53L1_Dev_t *pdev, uint16_t index, uint8_t *pdata)
{
  apalDbgAssert(pdev != NULL);

  // prepare address buffer
  uint8_t txbuf[sizeof(uint16_t)] = {index >> 8, index & 0xFF};

  // transmit address and recieve data
  apalExitStatus_t status = apalI2CMasterTransmit(pdev->Interface.i2cd, pdev->Interface.addr, txbuf, sizeof(txbuf), pdata, 1, VL53L1X_LLD_I2C_TIMEOUT);

  return (status == APAL_STATUS_OK || status > 0) ? VL53L1_ERROR_NONE :
         (status == APAL_STATUS_TIMEOUT) ? VL53L1_ERROR_TIME_OUT : VL53L1_ERROR_UNDEFINED;
}

/**
 * @brief   Read a 16-bit data word via I2C.
 *
 * @param[in]   pdev    Device handle.
 * @param[in]   index   Register address to read from.
 * @param[out]  pdata   16-bit buffer to store the read data to.
 *
 * @return  Status indicating whether the call was successful.
 * @retval  VL53L1_ERROR_NONE       Transmission was successful.
 * @retval  VL53L1_ERROR_TIME_OUT   Transmission timed out.
 * @retval  VL53L1_ERROR_UNDEFINED  Transmission failed for some other reason.
 */
VL53L1_Error VL53L1_RdWord(VL53L1_Dev_t *pdev,uint16_t index, uint16_t *pdata)
{
  apalDbgAssert(pdev != NULL);

  // prepare address buffer
  uint8_t txbuf[sizeof(uint16_t)] = {index >> 8, index & 0xFF};
  // prepare receive buffer
  uint8_t rxbuf[sizeof(uint16_t)];

  // transmit address and recieve data
  apalExitStatus_t status = apalI2CMasterTransmit(pdev->Interface.i2cd, pdev->Interface.addr, txbuf, sizeof(txbuf), rxbuf, sizeof(rxbuf), VL53L1X_LLD_I2C_TIMEOUT);

  // fill return value
  *pdata = (((uint16_t)rxbuf[0]) << 8) |
           (((uint16_t)rxbuf[1]) << 0);
  /* apalDbgPrintf("[└─>] Word: %i\n", *pdata); */
  /* apalDbgPrintf("[└─>] sizeof(uint16_t): %i\n", sizeof(uint16_t)); */
  /* apalDbgPrintf("[└─>] Index: %i\n", index); */
  /* apalDbgPrintf("[└─>] txbuf[0]: %i\n", txbuf[0]); */
  /* apalDbgPrintf("[└─>] txbuf[1]: %i\n", txbuf[1]); */
  /* apalDbgPrintf("[└─>] rxbuf[0]: %i\n", rxbuf[0]); */
  /* apalDbgPrintf("[└─>] rxbuf[1]: %i\n", rxbuf[1]); */
  /* /\* apalDbgPrintf("[└─>] rxbuf1: %i\n", index); *\/ */
  /* apalDbgPrintf("[└─>] Status: %i\n", status); */

  return (status == APAL_STATUS_OK || status > 0) ? VL53L1_ERROR_NONE :
         (status == APAL_STATUS_TIMEOUT) ? VL53L1_ERROR_TIME_OUT : VL53L1_ERROR_UNDEFINED;
}

/**
 * @brief   Read a 32-bit data word via I2C.
 *
 * @param[in]   pdev    Device handle.
 * @param[in]   index   Register address to read from.
 * @param[out]  pdata   32-bit buffer to store the read data to.
 *
 * @return  Status indicating whether the call was successful.
 * @retval  VL53L1_ERROR_NONE       Transmission was successful.
 * @retval  VL53L1_ERROR_TIME_OUT   Transmission timed out.
 * @retval  VL53L1_ERROR_UNDEFINED  Transmission failed for some other reason.
 */
VL53L1_Error VL53L1_RdDWord(VL53L1_Dev_t *pdev,uint16_t index, uint32_t *pdata)
{
  apalDbgAssert(pdev != NULL);

  // prepare address buffer
  uint8_t txbuf[sizeof(uint16_t)] = {index >> 8, index & 0xFF};
  // prepare receive buffer
  uint8_t rxbuf[sizeof(uint32_t)];

  apalExitStatus_t status = apalI2CMasterTransmit(pdev->Interface.i2cd, pdev->Interface.addr, txbuf, sizeof(txbuf), rxbuf, sizeof(rxbuf), VL53L1X_LLD_I2C_TIMEOUT);

  // fill return value
  *pdata = (((uint32_t)rxbuf[0]) << 24) |
           (((uint32_t)rxbuf[1]) << 16) |
           (((uint32_t)rxbuf[2]) << 8) |
           (((uint32_t)rxbuf[3]) << 0);

  return (status == APAL_STATUS_OK || status > 0) ? VL53L1_ERROR_NONE :
         (status == APAL_STATUS_TIMEOUT) ? VL53L1_ERROR_TIME_OUT : VL53L1_ERROR_UNDEFINED;
}

/**
 * @brief   Wait for a specified amount of microseconds.
 *
 * @param[in] pdev      Device handle.
 * @param[in] wait_us   Time to wait in microsecnonds.
 *
 * @return  Always returns VL53L1_ERROR_NONE.
 */
VL53L1_Error VL53L1_WaitUs(VL53L1_Dev_t *pdev, int32_t wait_us)
{
  (void)pdev;

  apalSleep(wait_us);

  return VL53L1_ERROR_NONE;
}

/**
 * @brief   Wait for a specified amount of milliseconds.
 *
 * @param[in] pdev      Device handle.
 * @param[in] wait_ms   Time to wait in milliseconds.
 *
 * @return  Always returns VL53L1_ERROR_NONE.
 */
VL53L1_Error VL53L1_WaitMs(VL53L1_Dev_t *pdev, int32_t wait_ms)
{
  (void)pdev;

  apalSleep(wait_ms * 1000);

  return VL53L1_ERROR_NONE;
}

/**
 * @brief   Retrieve the frequency of a timer?
 * @details Function not supported since it is not used by the ST API.
 * @note    Intention of this function is unknown as it is neither documented nor used by the ST API.
 *
 * @param[out]  ptimer_freq_hz  Buffer to store the timer frequency to.
 *
 * @return  Always returns VL53L1_ERROR_NOT_SUPPORTED.
 */
VL53L1_Error VL53L1_GetTimerFrequency(int32_t *ptimer_freq_hz)
{
  apalDbgAssert(ptimer_freq_hz != NULL);

  (void)ptimer_freq_hz;

  return VL53L1_ERROR_NOT_SUPPORTED;
}

/**
 * @brief   Retrieve the current value of a timer?
 * @details Function not supported since it is not used by the ST API.
 * @note    Intention of this function is unknown as it is neither documented nor used by the ST API.
 *
 * @param[out]  ptimer_count  Buffer to store the current timer value to.
 *
 * @return  Always returns VL53L1_ERROR_NOT_SUPPORTED.
 */
VL53L1_Error VL53L1_GetTimerValue(int32_t *ptimer_count)
{
  apalDbgAssert(ptimer_count != NULL);

  (void)ptimer_count;

  return VL53L1_ERROR_NOT_SUPPORTED;
}

/**
 * @brief   Set the mode for a GPIO.
 * @details Function not supported since it is not used by the ST API.
 * @note    Intention of this function is unknown as it is neither documented nor used by the ST API.
 *
 * @param[in] pin   GPIO pin to modify.
 * @param[in] mode  Mode to set for the GPIO.
 *
 * @return  Always returns VL53L1_ERROR_NOT_SUPPORTED.
 */
VL53L1_Error VL53L1_GpioSetMode(uint8_t pin, uint8_t mode)
{
  (void)pin;
  (void)mode;

  return VL53L1_ERROR_NOT_SUPPORTED;
}

/**
 * @brief   Set the output value for a GPIO.
 * @details Function not supported since it is not used by the ST API.
 * @note    Intention of this function is unknown as it is neither documented nor used by the ST API.
 *
 * @param[in] pin     GPIO pin to set.
 * @param[in] value   Value to set for the GPIO.
 *
 * @return  Always returns VL53L1_ERROR_NOT_SUPPORTED.
 */
VL53L1_Error VL53L1_GpioSetValue(uint8_t pin, uint8_t value)
{
  (void)pin;
  (void)value;

  return VL53L1_ERROR_NOT_SUPPORTED;
}

/**
 * @brief   Read the input value of a GPIO.
 * @details Function not supported since it is not used by the ST API.
 * @note    Intention of this function is unknown as it is neither documented nor used by the ST API.
 *
 * @param[in]   pin     GPIO pin to read.
 * @param[out]  pvalue  Buffer to write the read value to.
 *
 * @return  Always returns VL53L1_ERROR_NOT_SUPPORTED.
 */
VL53L1_Error VL53L1_GpioGetValue(uint8_t pin, uint8_t *pvalue)
{
  (void)pin;
  (void)pvalue;

  return VL53L1_ERROR_NOT_SUPPORTED;
}

/**
 * @brief   Set to XSHUT GPIO.
 * @details Function not supported since it is not used by the ST API.
 * @note    Intention of this function is unknown as it is neither documented nor used by the ST API.
 *
 * @param[in] value   Value to set the XSHUT GPIO to.
 *
 * @return  Always returns VL53L1_ERROR_NOT_SUPPORTED.
 */
VL53L1_Error VL53L1_GpioXshutdown(uint8_t value)
{
  (void)value;

  return VL53L1_ERROR_NOT_SUPPORTED;
}

/**
 * @brief   Select a GPIO communication interface?
 * @details Function not supported since it is not used by the ST API.
 * @note    Intention of this function is unknown as it is neither documented nor used by the ST API.
 *
 * @param[in] value   Identifier of the interface to select?
 *
 * @return  Always returns VL53L1_ERROR_NOT_SUPPORTED.
 */
VL53L1_Error VL53L1_GpioCommsSelect(uint8_t value)
{
  (void)value;

  return VL53L1_ERROR_NOT_SUPPORTED;
}

/**
 * @brief   Enable power?
 * @details Function not supported since it is not used by the ST API.
 * @note    Intention of this function is unknown as it is neither documented nor used by the ST API.
 *
 * @param[in] value   ?
 *
 * @return  Always returns VL53L1_ERROR_NOT_SUPPORTED.
 */
VL53L1_Error VL53L1_GpioPowerEnable(uint8_t value)
{
  (void)value;

  return VL53L1_ERROR_NOT_SUPPORTED;
}

/**
 * @brief   Enable GPIO interrupt.
 * @details Function not supported since it is not used by the ST API.
 * @note    Intention of this function is unknown as it is neither documented nor used by the ST API.
 *
 * @param[in] function    Callback function to be executed on interrupt.
 * @param[in] edge_type   Signal edge to trigger an interrupts.
 *
 * @return  Always returns VL53L1_ERROR_NOT_SUPPORTED.
 */
VL53L1_Error VL53L1_GpioInterruptEnable(void (*function)(void), uint8_t edge_type)
{
  (void)function;
  (void)edge_type;

  return VL53L1_ERROR_NOT_SUPPORTED;
}

/**
 * @brief   Disable GPIO interrupt.
 * @details Function not supported since it is not used by the ST API.
 * @note    Intention of this function is unknown as it is neither documented nor used by the ST API.
 *
 * @return  Always returns VL53L1_ERROR_NOT_SUPPORTED.
 */
VL53L1_Error VL53L1_GpioInterruptDisable(void)
{
  return VL53L1_ERROR_NOT_SUPPORTED;
}



/**
 * @brief   Retrieve the current system tick count.
 *
 * @param[out]  ptime_ms  Buffer to store the current system tick count to.
 *
 * @return  Always returns VL53L1_ERROR_NONE.
 */
VL53L1_Error VL53L1_GetTickCount(uint32_t *ptime_ms)
{
  static RTCDateTime time;
  rtcGetTime(&RTCD1, &time);
  *ptime_ms = time.millisecond;

  return VL53L1_ERROR_NONE;
}

/**
 * @brief   Wait for a specific register to match a (masked) value.
 *
 * @param[in] pdev            Device handle.
 * @param[in] timeout_ms      Timeout value in milliseconds.
 * @param[in] index           Register address to check.
 * @param[in] value           Value to be matched.
 * @param[in] mask            Mask to apply to the register value.
 *                            Must not be 0x00.
 * @param[in] poll_delay_ms   Poll delay in milliseconds.
 *
 * @return  Status indicating whether the call was successful.
 * @retval  VL53L1_ERROR_NONE       Value matched before timeout.
 * @retval  VL53L1_ERROR_TIME_OUT   Value did not match before timeout occurred.
 * @retval  VL53L1_ERROR_UNDEFINED  I2C communication failed.
 */
VL53L1_Error VL53L1_WaitValueMaskEx(VL53L1_Dev_t *pdev, uint32_t timeout_ms, uint16_t index, uint8_t value, uint8_t mask, uint32_t poll_delay_ms)
{
  apalDbgAssert(pdev != NULL);
  apalDbgAssert(mask != 0x00);

  // local variables
  static RTCDateTime time;
  rtcGetTime(&RTCD1, &time);

  const apalTime_t starttime = time.millisecond;
  apalTime_t transmissionstartoffset, transmissionendoffset;
  apalTime_t remaintime = timeout_ms * 1000;
  uint8_t txbuf[sizeof(uint16_t)] = {index >> 8, index & 0xFF};
  uint8_t rxbuf[sizeof(uint8_t)];

  // infinite loop will return from within
  for(;;) {
    rtcGetTime(&RTCD1, &time);
    // measure time before transmission
    transmissionstartoffset = time.millisecond - starttime;

    // try transmission
    if (apalI2CMasterTransmit(pdev->Interface.i2cd, pdev->Interface.addr, txbuf, sizeof(txbuf), rxbuf, sizeof(rxbuf), (remaintime < VL53L1X_LLD_I2C_TIMEOUT) ? remaintime : VL53L1X_LLD_I2C_TIMEOUT) < 0) {
      return VL53L1_ERROR_UNDEFINED;
    }

    // check returned data
    if ((rxbuf[0] & mask) == value) {
      return VL53L1_ERROR_NONE;
    }

    // measure time after transmission
    rtcGetTime(&RTCD1, &time);
    transmissionendoffset = time.millisecond - starttime;

    // check for timeout and recalculate remaining time
    if (transmissionendoffset < timeout_ms * 1000) {
      remaintime = (timeout_ms * 1000) - transmissionendoffset;
    } else {
      return VL53L1_ERROR_TIME_OUT;
    }

    // sleep one poll delay, or the remaining time
    if (remaintime > (poll_delay_ms * 1000) - (transmissionendoffset - transmissionstartoffset)) {
      apalSleep((poll_delay_ms * 1000) - (transmissionendoffset - transmissionstartoffset));
    } else {
      apalSleep(remaintime);
      return VL53L1_ERROR_TIME_OUT;
    }

    // recalculate remaining time
    rtcGetTime(&RTCD1, &time);
    remaintime = time.millisecond - starttime;
    if (remaintime < timeout_ms * 1000) {
      remaintime = (timeout_ms * 1000) - remaintime;
    } else {
      return VL53L1_ERROR_TIME_OUT;
    }
  }
}

/* driver functions ***********************************************************/

/**
 * @brief   Read the current activation state of the device.
 *
 * @param[in]   vl53l1x   Driver object to check.
 * @param[out]  state     State of the device.
 *
 * @return  Indicator whether the call was successful.
 */
apalExitStatus_t vl53l1x_lld_getState(VL53L1XDriver* vl53l1x, vl53l1x_lld_state_t* state)
{
  apalDbgAssert(vl53l1x != NULL);
  apalDbgAssert(state != NULL);

  apalControlGpioState_t gpio_state;
  apalExitStatus_t status = apalControlGpioGet(vl53l1x->Interface.xshut, &gpio_state);
  *state = gpio_state == APAL_GPIO_ON ? VL53L1X_LLD_STATE_ON : VL53L1X_LLD_STATE_OFF;

  return status;
}

/**
 * @brief   Performs initialization procedure according to manual after device
 *          brought out of reset.
 *
 * @param[in,out] vl53l1x   Device to be initialized.
 *
 * @return  Indicator whether the call was successful.
 */
apalExitStatus_t vl53l1x_lld_init(VL53L1XDriver* vl53l1x)
{
  apalDbgAssert(vl53l1x != NULL);
  /* apalDbgPrintf("Hello Init-----\n"); */

  // local variables
  apalExitStatus_t status;
  VL53L1_Error errval;

  // check whether the device is already active
  {
    vl53l1x_lld_state_t xshut_state;
    status = vl53l1x_lld_getState(vl53l1x, &xshut_state);
    if (status != APAL_STATUS_OK) {
      return APAL_STATUS_ERROR;
    }
    // return warning if the device is already active
    if (xshut_state == VL53L1X_LLD_STATE_ON) {
      return APAL_STATUS_IO;
    }
  }

  // activate the device
  /* apalDbgPrintf("apalControlGpioSet()\n"); */
  status |= apalControlGpioSet(vl53l1x->Interface.xshut, APAL_GPIO_ON);
  /* apalDbgPrintf("\tstatus: %d\n", status); */

  // initialize device as described in manual
  /* apalDbgPrintf("VL53L1_WaitDeviceBooted()\n"); */
  errval = VL53L1_WaitDeviceBooted(vl53l1x);
  status |= (errval == VL53L1_ERROR_NONE) ? APAL_STATUS_SUCCESS : APAL_STATUS_ERROR;
  /* apalDbgPrintf("\tstatus: %d\terrval: %d\n", status, errval); */
  /* apalDbgPrintf("VL53L1_DataInit()\n"); */
  errval = VL53L1_DataInit(vl53l1x);
  status |= (errval == VL53L1_ERROR_NONE) ? APAL_STATUS_SUCCESS : APAL_STATUS_ERROR;
  /* apalDbgPrintf("\tstatus: %d\terrval: %d\n", status, errval); */
  /* apalDbgPrintf("VL53L1_StaticInit()\n"); */
  errval = VL53L1_StaticInit(vl53l1x);
  status |= (errval == VL53L1_ERROR_NONE) ? APAL_STATUS_SUCCESS : APAL_STATUS_ERROR;
  /* apalDbgPrintf("\tstatus: %d\terrval: %d\n", status, errval); */

  return status;
}

/**
 * @brief   Reset a device by deactivating the XSHUT signal.
 *
 * @param[in] vl53l1x   Device to reset.
 *
 * @return  Indicator whether the call was successful.
 */
apalExitStatus_t vl53l1x_lld_reset(VL53L1XDriver* vl53l1x)
{
  return apalControlGpioSet(vl53l1x->Interface.xshut, APAL_GPIO_OFF);
}

apalExitStatus_t vl53l1x_lld_setRangemode(VL53L1XDriver* dev, VL53L1_DistanceModes mode){
  const VL53L1_Error error = VL53L1_SetDistanceMode(dev, mode);

  return (error == VL53L1_ERROR_NONE) ? APAL_STATUS_OK : APAL_STATUS_ERROR;
}

/** @} */
