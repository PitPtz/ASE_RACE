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
 * @file    periphAL.h
 * @brief   Periphery Abstraction Layer specification header.
 *
 * @defgroup periphAL PeriphAL
 * @brief   Periphery Abstraction Layer interface API for interaction with the
 *          operating system from within driver context.
 * @details Since AMiRo-LLD is platform independent, any interaction with the
 *          operating system (including hardware access for I/O) is defined by
 *          the API of this abstraction layer.
 *
 * @addtogroup periphAL
 * @{
 */

#ifndef AMIROLLD_PERIPHAL_H
#define AMIROLLD_PERIPHAL_H

/*============================================================================*/
/* VERSION                                                                    */
/*============================================================================*/

/**
 * @brief   The periphery abstraction layer interface major version.
 * @note    Changes of the major version imply incompatibilities.
 */
#define PERIPHAL_VERSION_MAJOR    1

/**
 * @brief   The periphery abstraction layer interface minor version.
 * @note    A higher minor version implies new functionalty, but all old interfaces are still available.
 */
#define PERIPHAL_VERSION_MINOR    3

/*============================================================================*/
/* DEPENDENCIES                                                               */
/*============================================================================*/

#include <amiro-lld.h>

# if defined(AMIROLLD_CFG_PERIPHAL_HEADER)
#   define __include(file) <file>
#   define _include(file) __include(file)
#   include _include(AMIROLLD_CFG_PERIPHAL_HEADER)
#   undef _include
#   undef __include
# endif

#include <stdint.h>

/*============================================================================*/
/* GENERAL                                                                    */
/*============================================================================*/
/**
 * @defgroup periphAL_general General
 * @brief   todo
 * @details todo
 * @ingroup periphAL
 *
 * @addtogroup periphAL_general
 * @{
 */

/**
 * @brief Status values used as return value for all (or most) function calls.
 * @note  The status can be used as mask of flags.
 */
typedef int8_t apalExitStatus_t;

/**
 * @brief   Status value for success (no error or warning occurred).
 */
#define APAL_STATUS_OK                          (apalExitStatus_t)0
/** @brief  Alias for APAL_STATUS_OK. */
#define APAL_STATUS_SUCCESS                     (apalExitStatus_t)0

/**
 * @brief   Status flag for (unspecified) failure.
 */
#define APAL_STATUS_ERROR                       (apalExitStatus_t)(1 << 7)
/** @brief  Alias for APAL_STATUS_ERROR. */
#define APAL_STATUS_FAILURE                     (apalExitStatus_t)(1 << 7)

/**
 * @brief   Status flag for timeout issues.
 */
#define APAL_STATUS_TIMEOUT                     (apalExitStatus_t)(1 << 0)

/**
 * @brief   Status flag for argument issues.
 */
#define APAL_STATUS_INVALIDARGUMENTS            (apalExitStatus_t)(1 << 1)

/**
 * @brief   Status flag for not available/implemented function or feature.
 */
#define APAL_STATUS_UNAVAILABLE                 (apalExitStatus_t)(1 << 2)

/**
 * @brief   Status flag for I/O issues.
 */
#define APAL_STATUS_IO                          (apalExitStatus_t)(1 << 3)

/** @} */

/*============================================================================*/
/* DEBUG                                                                      */
/*============================================================================*/
/**
 * @defgroup periphAL_debug Debug
 * @brief   todo
 * @details todo
 * @ingroup periphAL
 *
 * @addtogroup periphAL_debug
 * @{
 */

#if (AMIROLLD_CFG_DBG == true) || defined(__DOXYGEN__)

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

#if !defined(apalDbgAssertMsg) || defined(__DOXYGEN__)
  /**
   * @brief Assert function to check a given condition and print a message
   *        string.
   *
   * @param[in] c     The condition to check.
   * @param[in] fmt   Formatted message string to print.
   */
  void apalDbgAssertMsg(const bool c, const char* fmt, ...);
#endif

#if !defined(apalDbgPrintf) || defined(__DOXYGEN__)
  /**
   * @brief Printf function for messages printed only in debug builds.
   *
   * @param[in] fmt   Formatted string to print.
   *
   * @return    Number of printed characters.
   */
  int apalDbgPrintf(const char* fmt, ...);
#endif

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#else /* (AMIROLLD_CFG_DBG == true) */

#define apalDbgAssertMsg(c, fmt, ...)
#define apalDbgPrintf(fmt, ...)

#endif /* (AMIROLLD_CFG_DBG == true) */

/**
 * @brief Assert function to check a given condition.
 *
 * @param[in] c     The condition to check.
 */
#define apalDbgAssert(c) apalDbgAssertMsg(c, "%s(%u): apalDbgAssert failed", __FILE__, __LINE__)

/** @} */

/*============================================================================*/
/* TIMING                                                                     */
/*============================================================================*/
/**
 * @defgroup periphAL_timing Timing
 * @brief   todo
 * @details todo
 * @ingroup periphAL
 *
 * @addtogroup periphAL_timing
 * @{
 */

/**
 * @brief Time measurement type (in microseconds).
 */
#if (AMIROLLD_CFG_TIME_SIZE == 8)
  typedef uint8_t   apalTime_t;
#elif (AMIROLLD_CFG_TIME_SIZE == 16)
  typedef uint16_t  apalTime_t;
#elif (AMIROLLD_CFG_TIME_SIZE == 32) || defined(__DOXYGEN__)
  typedef uint32_t  apalTime_t;
#elif (AMIROLLD_CFG_TIME_SIZE == 64)
  typedef uint64_t  apalTime_t;
#else
  #error "AMIROLLD_CFG_TIME_SIZE must be 8, 16, 32 or 64"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(apalSleep) || defined(__DOXYGEN__)
  /**
   * @brief Delay execution by a specific number of microseconds.
   *
   * @param[in]   us    Time to sleep until execution continues in microseconds.
   */
  void apalSleep(apalTime_t us);
#endif

#ifdef __cplusplus
}
#endif

/** @} */

/*============================================================================*/
/* GPIO                                                                       */
/*============================================================================*/
/**
 * @defgroup periphAL_gpio GPIO
 * @brief   todo
 * @details todo
 * @ingroup periphAL
 *
 * @addtogroup periphAL_gpio
 * @{
 */

#if (AMIROLLD_CFG_GPIO == true) || defined(__DOXYGEN__)

/*
 * The following type must be defined by the implementation:
 */
#if defined(__DOXYGEN__)
/**
 * @brief   Type to represent a GPIO object.
 * @note    Is only used via pointer by the API.
 */
struct apalGpio_t {};
#endif

/**
 * @brief Status values to read/write a GPIO port.
 */
typedef uint8_t apalGpioState_t;

/**
 * @brief   GPIO physical low state.
 */
#define APAL_GPIO_LOW                           ((apalGpioState_t)0)

/**
 * @brief   GPIO physical high state.
 */
#define APAL_GPIO_HIGH                          ((apalGpioState_t)1)

/**
 * @brief   Invert a physical GPIO state.
 *
 * @param[in] state   GPIO state to invert.
 *
 * @return  Inverted physical GPIO state.
 */
#define APAL_GPIO_STATE_INVERT(state) (                                         \
  (apalGpioState_t)state ^ APAL_GPIO_HIGH                                       \
)

/**
 * @brief Logical status values to turn a control GPIO 'on' and 'off'.
 */
typedef uint8_t apalControlGpioState_t;

/**
 * @brief   GPIO logical off state.
 */
#define APAL_GPIO_OFF                           ((apalControlGpioState_t)0)

/**
 * @brief   GPIO logical on state.
 */
#define APAL_GPIO_ON                            ((apalControlGpioState_t)1)

/**
 * @brief   Polarity state of the control GPIO.
 */
typedef uint8_t apalGpioActive_t;

/**
 * @brief   The control GPIO is never defined to be 'on' (does not apply).
 */
#define APAL_GPIO_ACTIVE_NONE                   ((apalGpioActive_t)0)

/**
 * @brief   The control GPIO is defined to be 'on' when it is phsically low.
 */
#define APAL_GPIO_ACTIVE_LOW                    ((apalGpioActive_t)1)

/**
 * @brief   The control GPIO is defined to be 'on' when it is physically high.
 */
#define APAL_GPIO_ACTIVE_HIGH                   ((apalGpioActive_t)2)

/**
 * @brief   The control GPIO is defined to be always 'on'.
 */
#define APAL_GPIO_ACTIVE_ANY                    ((apalGpioActive_t)3)

/**
 * @brief   Invert a GPIO active state.
 * @details The active state is inverted only if it was either low or high.
 *          In case it was set to none or any, the value is not modified.
 *
 * @param[in] active  Active state to be inverted.
 *
 * @return  Inverted active state.
 */
#define APAL_GPIO_ACTIVE_INVERT(active) (                                       \
  (((apalGpioActive_t)active & APAL_GPIO_ACTIVE_LOW) ^                          \
   ((apalGpioActive_t)active & APAL_GPIO_ACTIVE_HIGH)) ?                        \
  ((apalGpioActive_t)active ^ APAL_GPIO_ACTIVE_ANY) :                           \
  ((apalGpioActive_t)active)                                                    \
)

/**
 * @brief   Signal direction for the control GPIO.
 */
typedef uint8_t apalGpioDirection_t;

/**
 * @brief   Signal direction for the control GPIO is undefined.
 */
#define APAL_GPIO_DIRECTION_UNDEFINED           ((apalGpioDirection_t)0)

/**
 * @brief   Signal direction for the control GPIO is input only.
 */
#define APAL_GPIO_DIRECTION_INPUT               ((apalGpioDirection_t)1)

/**
 * @brief   Signal direction for the control GPIO is output only.
 */
#define APAL_GPIO_DIRECTION_OUTPUT              ((apalGpioDirection_t)2)

/**
 * @brief   Signal direction for the control GPIO is didirectional.
 */
#define APAL_GPIO_DIRECTION_BIDIRECTIONAL       ((apalGpioDirection_t)3)

/**
 * @brief   Informative or effective signal edge for control GPIOs.
 */
typedef uint8_t apalGpioEdge_t;

/**
 * @brief   No edges indicate an interrupt or trigger an action.
 */
#define APAL_GPIO_EDGE_NONE                     ((apalGpioEdge_t)0)

/**
 * @brief   Rising edges indicate an interrupt or trigger an action.
 */
#define APAL_GPIO_EDGE_RISING                   ((apalGpioEdge_t)1)

/**
 * @brief   Falling edges indicate an interrupt or trigger an action.
 */
#define APAL_GPIO_EDGE_FALLING                  ((apalGpioEdge_t)2)

/**
 * @brief   Both rising and falling edges indicate an interrupt or trigger an
 *          action.
 */
#define APAL_GPIO_EDGE_BOTH                     ((apalGpioEdge_t)3)

/**
 * @brief   Inverts the value of the informative or effective signal edge for
 *          interrupts.
 * @details Rising edge is inverted to falling and vice versa.
 *          If none or both edges are enabled, the identical value is returned.
 */
#define APAL_GPIO_EDGE_INVERT(edge) (                                           \
  (((apalGpioEdge_t)edge & APAL_GPIO_EDGE_RISING) ^                             \
   ((apalGpioEdge_t)edge & APAL_GPIO_EDGE_FALLING)) ?                           \
  ((apalGpioEdge_t)edge ^ APAL_GPIO_EDGE_BOTH) :                                \
  ((apalGpioEdge_t)edge)                                                        \
)

/**
 * @brief Control GPIO meta information
 */
typedef struct {
  apalGpioDirection_t direction : 2;  /**< Direction configuration for according signals */
  apalGpioActive_t active       : 2;  /**< Active state of the GPIO */
  apalGpioEdge_t edge           : 2;  /**< Edge configuration for according signals */
} apalGpioMeta_t;

/**
 * @brief Control GPIO type.
 */
typedef struct {
  apalGpio_t* gpio;     /**< The GPIO to use.                 */
  apalGpioMeta_t meta;  /**< Meta information about the GPIO. */
} apalControlGpio_t;

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(apalGpioRead) || defined(__DOXYGEN__)
  /**
   * @brief Read the current value of a GPIO pin.
   *
   * @param[in]   gpio  GPIO to read.
   * @param[out]  val   Current value of the GPIO.
   *
   * @return The status indicates whether the function call was successful.
   */
  apalExitStatus_t apalGpioRead(apalGpio_t* gpio, apalGpioState_t* const val);
#endif

#if !defined(apalGpioWrite) || defined(__DOXYGEN__)
  /**
   * @brief Set the value of a GPIO pin.
   *
   * @param[in] gpio  GPIO to write.
   * @param[in] val   Value to set for the GPIO.
   *
   * @return The status indicates whether the function call was successful.
   */
  apalExitStatus_t apalGpioWrite(apalGpio_t* gpio, const apalGpioState_t val);
#endif

#if !defined(apalGpioToggle) || defined(__DOXYGEN__)
  /**
   * @brief Toggle the output of a GPIO.
   *
   * @param[in] gpio  GPIO to toggle.
   *
   * @return The status indicates whether the function call was successful.
   */
  apalExitStatus_t apalGpioToggle(apalGpio_t* gpio);
#endif

#if !defined(apalGpioIsInterruptEnabled) || defined(__DOXYGEN__)
  /**
   * @brief Return the interrupt enable status of the GPIO.
   *
   * @param[in]   gpio      GPIO to check.
   * @param[out]  enabled   Flag, indicating whether interrupt is enabled for
   *                        the GPIO.
   *
   * @return The status indicates whether the function call was successful.
   */
  apalExitStatus_t apalGpioIsInterruptEnabled(apalGpio_t* gpio, bool* const enabled);
#endif

#if !defined(apalControlGpioGet) || defined(__DOXYGEN__)
  /**
   * @brief Get the current on/off state of a control GPIO.
   *
   * @param[in]   cgpio Control GPIO to read.
   * @param[out]  val   Current activation status of the control GPIO.
   *
   * @return The status indicates whether the function call was successful.
   */
  apalExitStatus_t apalControlGpioGet(const apalControlGpio_t* const cgpio, apalControlGpioState_t* const val);
#endif

#if !defined(apalControlGpioSet) || defined(__DOXYGEN__)
  /**
   * @brief Turn a control GPIO 'on' or 'off' respectively.
   *
   * @param[in] cgpio Control GPIO to set.
   * @param[in] val   Activation value to set for the control GPIO.
   *
   * @return The status indicates whether the function call was successful.
   */
  apalExitStatus_t apalControlGpioSet(const apalControlGpio_t* const cgpio, const apalControlGpioState_t val);
#endif

#if !defined(apalControlGpioSetInterrupt) || defined(__DOXYGEN__)
  /**
   * @brief   Enable or disable the interrupt event functionality.
   *
   * @param[in] cgpio   Control GPIO to set.
   * @param[in] enable  Flag, indicating whether the interrupt shall be
   *                    activated (true) or deactivated (false).
   *
   * @return The status indicates whether the function call was successful.
   */
  apalExitStatus_t apalControlGpioSetInterrupt(const apalControlGpio_t* const cgpio, const bool enable);
#endif

#ifdef __cplusplus
}
#endif

#endif /* (AMIROLLD_CFG_GPIO == true) */

/** @} */

/*============================================================================*/
/* PWM                                                                        */
/*============================================================================*/
/**
 * @defgroup periphAL_pwm PWM
 * @brief   todo
 * @details todo
 * @ingroup periphAL
 *
 * @addtogroup periphAL_pwm
 * @{
 */

#if (AMIROLLD_CFG_PWM == true) || defined(__DOXYGEN__)

/*
 * The following type must be defined by the implementation:
 */
#if defined(__DOXYGEN__)
/**
 * @brief   Type to represent a PWM driver object.
 * @note    Is only used via pointer by the API.
 */
struct apalPWMDriver_t {};
#endif

/**
 * @brief PWM channel type.
 */
typedef uint8_t   apalPWMchannel_t;

/**
 * @brief PWM width type.
 */
typedef uint16_t  apalPWMwidth_t;

/**
 * @brief PWM frequency type.
 */
typedef uint32_t  apalPWMfrequency_t;

/**
 * @brief PWM period time.
 */
typedef uint32_t  apalPWMperiod_t;

/**
 * @brief   PWM width to turn off the PWM.
 */
#define APAL_PWM_WIDTH_OFF                      ((apalPWMwidth_t)0x0000u)

/**
 * @brief   Minimum allowed PWM width.
 */
#define APAL_PWM_WIDTH_MIN                      ((apalPWMwidth_t)0x0000u)

/**
 * @brief   Maximum allowed PWM width.
 */
#define APAL_PWM_WIDTH_MAX                      ((apalPWMwidth_t)0xFFFFu)

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(apalPWMSet) || defined(__DOXYGEN__)
  /**
   * @brief   Set the PWM with given parameters.
   *
   * @param[in] pwm       PWM driver to set.
   * @param[in] channel   Channel of the PWM driver to set.
   * @param[in] width     Width to set the channel to.
   *
   * @return  The status indicates whether the function call was successful.
   */
  apalExitStatus_t apalPWMSet(apalPWMDriver_t* pwm, const apalPWMchannel_t channel, const apalPWMwidth_t width);
#endif

#if !defined(apalPWMGetFrequency) || defined(__DOXYGEN__)
  /**
   * @brief   Retrieve the current frequency of the PWM.
   *
   * @param[in]  pwm        PWM driver to read.
   * @param[out] frequency  The currently set frequency.
   *
   * @return  The status indicates whether the function call was successful.
   */
  apalExitStatus_t apalPWMGetFrequency(apalPWMDriver_t* pwm, apalPWMfrequency_t* const frequency);
#endif

#if !defined(apalPWMGetPeriod) || defined(__DOXYGEN__)
  /**
   * @brief   Retrieve the current period of the PWM.
   *
   * @param[in]   pwm     PWM driver to read.
   * @param[out]  period  The currently set period.
   *
   * @return  The status indicates whether the function call was successful.
   */
  apalExitStatus_t apalPWMGetPeriod(apalPWMDriver_t* pwm, apalPWMperiod_t* const period);
#endif

#ifdef __cplusplus
}
#endif

#endif /* (AMIROLLD_CFG_PWM == true) */

/** @} */

/*============================================================================*/
/* QEI                                                                        */
/*============================================================================*/
/**
 * @defgroup periphAL_qei QEI
 * @brief   todo
 * @details todo
 * @ingroup periphAL
 *
 * @addtogroup periphAL_qei
 * @{
 */

#if (AMIROLLD_CFG_QEI == true) || defined(__DOXYGEN__)

/*
 * The following type must be defined by the implementation:
 */
#if defined(__DOXYGEN__)
/**
 * @brief   Type to represent a QEI driver object.
 * @note    Is only used via pointer by the API.
 */
struct apalQEIDriver_t {};
#endif

/**
 * @brief QEI counter type.
 */
typedef uint32_t  apalQEICount_t;

/**
 * @brief QEI delta type.
 */
typedef int32_t   apalQEIDiff_t;

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(apalQEIGetPosition) || defined(__DOXYGEN__)
  /**
   * @brief Gets the current position of the ecnoder.
   *
   * @param[in]   qei       The QEI driver to use.
   * @param[out]  position  The current position of the encoder.
   *
   * @return The status indicates whether the function call was successful.
   */
  apalExitStatus_t apalQEIGetPosition(apalQEIDriver_t* qei, apalQEICount_t* const position);
#endif

#if !defined(apalQEIGetUpdate) || defined(__DOXYGEN__)
  /**
   * @brief Retrieves the delta since the last read access.
   *
   * @param[in]   qei     The QEI driver to use.
   * @param[out]  delta   Position difference since the last read access.
   *
   * @return The status indicates whether the function call was successful.
   */
  apalExitStatus_t apalQEIGetDelta(apalQEIDriver_t* qei, apalQEIDiff_t* const delta);
#endif

#if !defined(apalQEIGetRange) || defined(__DOXYGEN__)
  /**
   * @brief Gets the value range of the encoder.
   *
   * @param[in]   qei     The QEI driver to use.
   * @param[out]  range   The value range of the encoder.
   *
   * @return The status indicates whether the function call was successful.
   */
  apalExitStatus_t apalQEIGetRange(apalQEIDriver_t* qei, apalQEICount_t* const range);
#endif

#ifdef __cplusplus
}
#endif

#endif /* (AMIROLLD_CFG_QEI == true) */

/** @} */

/*============================================================================*/
/* I2C                                                                        */
/*============================================================================*/
/**
 * @defgroup periphAL_i2c I2C
 * @brief   todo
 * @details todo
 * @ingroup periphAL
 *
 * @addtogroup periphAL_i2c
 * @{
 */

#if (AMIROLLD_CFG_I2C == true) || defined(__DOXYGEN__)

/*
 * The following type must be defined by the implementation:
 */
#if defined(__DOXYGEN__)
/**
 * @brief   Type to represent a I2C driver object.
 * @note    Is only used via pointer by the API.
 */
struct apalI2CDriver_t {};
#endif

/**
 * @brief I2C address type.
 */
typedef uint16_t apalI2Caddr_t;

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(apalI2CMasterTransmit) || defined(__DOXYGEN__)
  /**
   * @brief Transmit data and receive a response.
   *
   * @param[in]   i2cd      The I2C driver to use.
   * @param[in]   addr      Address to write to.
   * @param[in]   txbuf     Buffer containing data to send.
   * @param[in]   txbytes   Number of bytes to send.
   * @param[out]  rxbuf     Buffer to store a response to.
   * @param[in]   rxbytes   Number of bytes to receive.
   * @param[in]   timeout   Timeout for the function to return (in
   *                        microseconds).
   *
   * @return  The status indicates whether the function call was succesful or a
   *          timeout occurred.
   */
  apalExitStatus_t apalI2CMasterTransmit(apalI2CDriver_t* i2cd, const apalI2Caddr_t addr, const uint8_t* const txbuf, const size_t txbytes, uint8_t* const rxbuf, const size_t rxbytes, const apalTime_t timeout);
#endif

#if !defined(apalI2CMasterReceive) || defined(__DOXYGEN__)
  /**
   * @brief Read data from a specific address.
   *
   * @param[in]   i2cd      The I2C driver to use.
   * @param[in]   addr      Address to read.
   * @param[out]  rxbuf     Buffer to store the response to.
   * @param[in]   rxbytes   Number of bytes to receive.
   * @param[in]   timeout   Timeout for the function to return (in
   *                        microseconds).
   *
   * @return  The status indicates whether the function call was succesful or a
   *          timeout occurred.
   */
  apalExitStatus_t apalI2CMasterReceive(apalI2CDriver_t* i2cd, const apalI2Caddr_t addr, uint8_t* const rxbuf, const size_t rxbytes, const apalTime_t timeout);
#endif

#ifdef __cplusplus
}
#endif

#endif /* (AMIROLLD_CFG_I2C == true) */

/** @} */

/*============================================================================*/
/* SPI                                                                        */
/*============================================================================*/
/**
 * @defgroup periphAL_spi SPI
 * @brief   todo
 * @details todo
 * @ingroup periphAL
 *
 * @addtogroup periphAL_spi
 * @{
 */

#if (AMIROLLD_CFG_SPI == true) || defined(__DOXYGEN__)

/*
 * The following types must be defined by the implementation:
 */
#if defined(__DOXYGEN__)
/**
 * @brief   Type to represent a SPI driver object.
 * @note    Is only used via pointer by the API.
 */
struct apalSPIDriver_t {};

/**
 * @brief   Type to represent SPI a configuration object.
 * @note    Is only used via pointer by the API.
 */
struct apalSPIConfig_t {};
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(apalSPITransmit) || defined(__DOXYGEN__)
  /**
   * @brief Transmit data to SPI.
   *
   * @param[in]   spid      The SPI driver to use.
   * @param[in]   data      Buffer containing data to send.
   * @param[in]   length    Number of bytes to send.
   *
   * @return The status indicates whether the function call was succesful.
   */
  apalExitStatus_t apalSPITransmit(apalSPIDriver_t* spid, const uint8_t* const data, const size_t length);
#endif

#if !defined(apalSPIReceive) || defined(__DOXYGEN__)
  /**
   * @brief Receive data from SPI.
   *
   * @param[in]   spid      The SPI driver to use.
   * @param[out]  data      Buffer to store receied data.
   * @param[in]   length    Number of bytes to receive.
   *
   * @return The status indicates whether the function call was succesful.
   */
  apalExitStatus_t apalSPIReceive(apalSPIDriver_t* spid, uint8_t* const data, const size_t length);
#endif

#if !defined(apalSPITransmitAndReceive) || defined(__DOXYGEN__)
  /**
   * @brief Transmit data to SPI and receive data afterwards without releasing the bus in between.
   *
   * @param[in]   spid        The SPI driver to use.
   * @param[in]   txData      Transmit data buffer.
   * @param[in]   rxData      Receive data buffer.
   * @param[in]   txLength    Number of bytes to send.
   * @param[in]   rxLength    Number of bytes to receive.
   *
   * @return The status indicates whether the function call was succesful.
   */
  apalExitStatus_t apalSPITransmitAndReceive(apalSPIDriver_t* spid, const uint8_t* const txData , uint8_t* const rxData, const size_t txLength, const size_t rxLength);
#endif

#if !defined(apalSPIExchange) || defined(__DOXYGEN__)
  /**
   * @brief Transmit and receive data from SPI simultaneously.
   *
   * @param[in]   spid      The SPI driver to use.
   * @param[in]   txData    Buffer containing data to send.
   * @param[out]  rxData    Buffer to store received data.
   * @param[in]   length    Number of bytes to send and receive.
   *
   * @return The status indicates whether the function call was succesful.
   */
  apalExitStatus_t apalSPIExchange(apalSPIDriver_t* spid, const uint8_t* const txData , uint8_t* const rxData, const size_t length);
#endif

#if !defined(apalSPIReconfigure) || defined(__DOXYGEN__)
  /**
   * @brief Reconfigure an SPI driver.
   *
   * @param[in] spid    The SPI driver to be reconfigured.
   * @param[in] config  Configuration to apply.
   *
   * @return The status indicates whether the function call was succesful.
   */
  apalExitStatus_t apalSPIReconfigure(apalSPIDriver_t* spid, const apalSPIConfig_t* config);
#endif

#ifdef __cplusplus
}
#endif

#endif /* (AMIROLLD_CFG_SPI == true) */

/** @} */

#endif /* AMIROOS_PERIPHAL_H */

/** @} */
