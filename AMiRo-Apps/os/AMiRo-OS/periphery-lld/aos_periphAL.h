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
 * @file    aos_periphAL.h
 * @brief   Periphery abstraction layer implementation header.
 *
 * @addtogroup lldlib
 * @{
 */

#ifndef AMIROOS_PERIPHAL_H
#define AMIROOS_PERIPHAL_H

/*============================================================================*/
/* DEPENDENCIES                                                               */
/*============================================================================*/

#include <alldconf.h>
#include <aosconf.h>
#include <hal.h>
#include <ch.h>

/*============================================================================*/
/* DEBUG                                                                      */
/*============================================================================*/

#if (AMIROLLD_CFG_DBG == true)
#if (AMIROOS_CFG_DBG != true)

#define apalDbgAssertMsg(c, fmt, ...)
#define apalDbgPrintf(fmt, ...)                 0

#endif /* (AMIROOS_CFG_DBG != true) */
#endif /* (AMIROLLD_CFG_DBG == true) */

/*============================================================================*/
/* TIMING                                                                     */
/*============================================================================*/

#if (AMIROOS_CFG_DBG != true)

#define apalSleep(us)                           chThdSleep(chTimeUS2I((apalTime_t)us))

#endif /* (AMIROOS_CFG_DBG != true) */

/*============================================================================*/
/* GPIO                                                                       */
/*============================================================================*/

#if (HAL_USE_PAL == TRUE) || defined(__DOXYGEN__)

/**
 * @brief GPIO driver type.
 */
typedef struct __PACKED {
  ioline_t line;
} apalGpio_t;

/**
 * @brief   Converts an apalGpioEdge_t to an ChibiOS PAL edge.
 */
#define APAL2CH_EDGE(edge) (                                                    \
  (edge == APAL_GPIO_EDGE_RISING) ? PAL_EVENT_MODE_RISING_EDGE :                \
    (edge == APAL_GPIO_EDGE_FALLING) ? PAL_EVENT_MODE_FALLING_EDGE :            \
     (edge == APAL_GPIO_EDGE_BOTH) ? PAL_EVENT_MODE_BOTH_EDGES :                \
      PAL_EVENT_MODE_DISABLED                                                   \
)

#endif /* (HAL_USE_PAL == TRUE) */

/*============================================================================*/
/* PWM                                                                        */
/*============================================================================*/

#if (HAL_USE_PWM == TRUE) || defined (__DOXYGEN__)

/**
 * @brief PWM driver type.
 */
typedef PWMDriver apalPWMDriver_t;

#endif /* (HAL_USE_PWM == TRUE) */

/*============================================================================*/
/* QEI                                                                        */
/*============================================================================*/

#if (HAL_USE_QEI == TRUE) || defined (__DOXYGEN__)

/**
 * @brief QEI driver type.
 */
typedef QEIDriver apalQEIDriver_t;

#endif /* (HAL_USE_QEI == TRUE) */

/*============================================================================*/
/* I2C                                                                        */
/*============================================================================*/

#if (HAL_USE_I2C == TRUE) || defined(__DOXYGEN__)

/**
 * @brief I2C driver type.
 */
typedef I2CDriver apalI2CDriver_t;

#endif /* (HAL_USE_I2C == TRUE) */

/*============================================================================*/
/* SPI                                                                        */
/*============================================================================*/

#if (HAL_USE_SPI == TRUE) || defined(__DOXYGEN__)

/**
 * @brief SPI driver type.
 */
typedef SPIDriver apalSPIDriver_t;

/**
 * @brief SPI confguration type.
 */
typedef SPIConfig apalSPIConfig_t;

#endif /* (HAL_USE_SPI == TRUE) */

#endif /* AMIROOS_PERIPHAL_H */

/** @} */
