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
 * @file    alld_AT42QT1050.h
 * @brief   Touch sensor macros and structures.
 *
 * @defgroup lld_AT42QT1050_v1 Version 1
 * @brief   Version 1
 * @ingroup lld_AT42QT1050
 *
 * @addtogroup lld_AT42QT1050_v1
 * @{
 */

#ifndef AMIROLLD_AT42QT1050_H
#define AMIROLLD_AT42QT1050_H

#include <amiro-lld.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief   Maximum I2C frequency.
 */
#define AT42QT1050_LLD_I2C_MAXFREQUENCY         400000

/**
 * @brief   A falling edge indicats an interrupt.
 */
#define AT42QT1050_LLD_INT_EDGE                 APAL_GPIO_EDGE_FALLING

/**
 * @brief   Number of touch keys supported by AT42QT1050.
 */
#define AT42QT1050_LLD_NUM_KEYS                 5

/**
 * @brief   Maximum time (in microseconds) to acquire all key signals before the overflow bit of the detection status register is set.
 */
#define AT42QT1050_LLD_MAX_KEY_ACQUIRATION_TIME 8000

/**
 * @brief   The chip ID as can be read from the according register (constant).
 */
#define AT42QT1050_LLD_CHIPID                   0x46

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

/**
 * @brief   The AT42QT1050Driver sruct.
 */
typedef struct {
  apalI2CDriver_t* i2cd;
  apalI2Caddr_t addr;
} AT42QT1050Driver;

/**
 * @brief   Possible I2C address configurations.
 */
enum {
  AT42QT1050_LLD_I2C_ADDRSEL_LOW  = 0x0041u,  /**< ADDR_SEL pin is pulled low. */
  AT42QT1050_LLD_I2C_ADDRSEL_HIGH = 0x0046u,  /**< ADDR_SEL pin is pulled high. */
};

/**
 * @brief   Available register addresses of the AT42QT1050.
 */
typedef enum {
  AT42QT1050_LLD_REG_CHIPID               = 0x00u,  /**<  read only */
  AT42QT1050_LLD_REG_FIRMWAREVERSION      = 0x01u,  /**<  read only */
  AT42QT1050_LLD_REG_DETECTIONSTATUS      = 0x02u,  /**<  read only */
  AT42QT1050_LLD_REG_KEYSTATUS            = 0x03u,  /**<  read only */
  AT42QT1050_LLD_REG_KEYSIGNAL_0          = 0x06u,  /**<  read only */
  AT42QT1050_LLD_REG_KEYSIGNAL_1          = 0x08u,  /**<  read only */
  AT42QT1050_LLD_REG_KEYSIGNAL_2          = 0x0Du,  /**<  read only */
  AT42QT1050_LLD_REG_KEYSIGNAL_3          = 0x0Fu,  /**<  read only */
  AT42QT1050_LLD_REG_KEYSIGNAL_4          = 0x11u,  /**<  read only */
  AT42QT1050_LLD_REG_REFERENCEDATA_0      = 0x14u,  /**<  read only */
  AT42QT1050_LLD_REG_REFERENCEDATA_1      = 0x16u,  /**<  read only */
  AT42QT1050_LLD_REG_REFERENCEDATA_2      = 0x1Au,  /**<  read only */
  AT42QT1050_LLD_REG_REFERENCEDATA_3      = 0x1Cu,  /**<  read only */
  AT42QT1050_LLD_REG_REFERENCEDATA_4      = 0x1Eu,  /**<  read only */
  AT42QT1050_LLD_REG_NEGATIVETHRESHOLD_0  = 0x21u,  /**< read/write */
  AT42QT1050_LLD_REG_NEGATIVETHRESHOLD_1  = 0x22u,  /**< read/write */
  AT42QT1050_LLD_REG_NEGATIVETHRESHOLD_2  = 0x24u,  /**< read/write */
  AT42QT1050_LLD_REG_NEGATIVETHRESHOLD_3  = 0x25u,  /**< read/write */
  AT42QT1050_LLD_REG_NEGATIVETHRESHOLD_4  = 0x26u,  /**< read/write */
  AT42QT1050_LLD_REG_PULSE_SCALE_0        = 0x28u,  /**< read/write */
  AT42QT1050_LLD_REG_PULSE_SCALE_1        = 0x29u,  /**< read/write */
  AT42QT1050_LLD_REG_PULSE_SCALE_2        = 0x2Bu,  /**< read/write */
  AT42QT1050_LLD_REG_PULSE_SCALE_3        = 0x2Cu,  /**< read/write */
  AT42QT1050_LLD_REG_PULSE_SCALE_4        = 0x2Du,  /**< read/write */
  AT42QT1050_LLD_REG_INTEGRATOR_AKS_0     = 0x2Fu,  /**< read/write */
  AT42QT1050_LLD_REG_INTEGRATOR_AKS_1     = 0x30u,  /**< read/write */
  AT42QT1050_LLD_REG_INTEGRATOR_AKS_2     = 0x32u,  /**< read/write */
  AT42QT1050_LLD_REG_INTEGRATOR_AKS_3     = 0x33u,  /**< read/write */
  AT42QT1050_LLD_REG_INTEGRATOR_AKS_4     = 0x34u,  /**< read/write */
  AT42QT1050_LLD_REG_CHARGESHAREDELAY_0   = 0x36u,  /**< read/write */
  AT42QT1050_LLD_REG_CHARGESHAREDELAY_1   = 0x37u,  /**< read/write */
  AT42QT1050_LLD_REG_CHARGESHAREDELAY_2   = 0x39u,  /**< read/write */
  AT42QT1050_LLD_REG_CHARGESHAREDELAY_3   = 0x3Au,  /**< read/write */
  AT42QT1050_LLD_REG_CHARGESHAREDELAY_4   = 0x3Bu,  /**< read/write */
  AT42QT1050_LLD_REG_FINFOUTMAXCALGUARD   = 0x3Cu,  /**< read/write */
  AT42QT1050_LLD_REG_LOWPOWERMODE         = 0x3Du,  /**< read/write */
  AT42QT1050_LLD_REG_MAXONDURATION        = 0x3Eu,  /**< read/write */
  AT42QT1050_LLD_REG_RESET_CALIBRATE      = 0x3Fu,  /**< read/write */
} at42qt1050_lld_register_t;

/**
 * @brief   Firmware version register structure.
 */
typedef union {
  uint8_t raw;
  struct {
    uint8_t minor : 4;
    uint8_t major : 4;
  };
} at42qt1050_lld_firmwarereg_t;


/**
 * @brief   Relevant bits of the detection status register.
 */
typedef enum {
  AT42QT1050_LLD_DETECTIONSTATUS_TOUCH      = 0x01u,  /**< Set if any keys are in detect.  */
  AT42QT1050_LLD_DETECTIONSTATUS_OVERFLOW   = 0x40u,  /**< Set if the time to acquire all key signals exceeds 8ms. */
  AT42QT1050_LLD_DETECTIONSTATUS_CALIBRATE  = 0x80u,  /**< Set during calibration sequence. */
} at42qt1050_lld_detectionstatusreg_t;

/**
 * @brief   Key status register masks.
 */
typedef enum {
  AT42QT1050_LLD_KEYSTATUS_KEY0 = 0x02u,
  AT42QT1050_LLD_KEYSTATUS_KEY1 = 0x04u,
  AT42QT1050_LLD_KEYSTATUS_KEY2 = 0x10u,
  AT42QT1050_LLD_KEYSTATUS_KEY3 = 0x20u,
  AT42QT1050_LLD_KEYSTATUS_KEY4 = 0x40u,
} at42qt1050_lld_keystatusreg_t;

/**
 * @brief   Pulse/Scale register structure.
 */
typedef union {
  uint8_t raw;
  struct {
    uint8_t scale : 4;
    uint8_t pulse : 4;
  };
} at42qt1050_lld_pulsescalereg_t;

/**
 * @brief   Detection Integrator (DI) / AKS register structure.
 */
typedef union {
  uint8_t raw;
  struct {
    uint8_t aks : 2;
    uint8_t detection_integrator : 6;
  };
} at42qt1050_lld_detectionintegratoraksreg_t;

/**
 * @brief   Charge share delay constant sclaing factor.
 * @details Values in the charge share delay registers are multiplied by this factor.
 *          Unit is microseconds (µs).
 */
#define AT42QT1050_LLD_CHARGESHAREDELAY_FACTOR  2.5f

/**
 * @brief   FastIn / FastOutDI / Max Cal / Guard Channel register masks.
 */
typedef enum {
  AT42QT1050_LLD_FINFOUTMAXCALGUARD_GUARD   = 0x0Fu,
  AT42QT1050_LLD_FINFOUTMAXCALGUARD_MAXCAL  = 0x10u,
  AT42QT1050_LLD_FINFOUTMAXCALGUARD_FO      = 0x20u,
  AT42QT1050_LLD_FINFOUTMAXCALGUARD_FI      = 0x40u,
} at42qt1050_lld_finfoutmaxcalguardreg_t;

/**
 * @brief   Low power mode constant scaling factor.
 * @details The values in the low poer mode register is multiplied by this factor.
 *          Unit is microseconds (µs).
 * @note    Setting the power mode scaling register value to zero makes the AT42QT1050 enter deep-sleep mode.
 */
#define AT42QT1050_LLD_LOWPOWER_FACTOR          8000

/**
 * @brief   Man on duration constant scaling factor.
 * @details The value in the max on duration register is multiplied by this factor.
 *          Unit is microseconds (µs).
 */
#define AT42QT1050_LLD_MAXONDURATION_FACTOR     160000

/**
 * @brief   RESET / Calibrate register masks.
 */
typedef enum {
  AT42QT1050_LLD_RESETCALIBRATE_CALIBRATE = 0x7Fu,
  AT42QT1050_LLD_RESETCALIBRATE_RESET     = 0x80u,
} at42qt1050_lld_resetcalibratereg_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
  apalExitStatus_t at42qt1050_lld_read_reg(const AT42QT1050Driver* at42qt1050d, const at42qt1050_lld_register_t reg, uint8_t* const data, const apalTime_t timeout);
  apalExitStatus_t at42qt1050_lld_write_reg(const AT42QT1050Driver* at42qt1050d, const at42qt1050_lld_register_t reg, const uint8_t data, const apalTime_t timeout);

  apalExitStatus_t at42qt1050_lld_read_keyssignal(const AT42QT1050Driver* at42qt1050d, const uint8_t key, uint16_t* signal, const apalTime_t timeout);
  apalExitStatus_t at42qt1050_lld_read_referencedata(const AT42QT1050Driver* at42qt1050d, const uint8_t key, uint16_t* refdata, const apalTime_t timeout);

  apalExitStatus_t at42qt1050_lld_reset_safe(const AT42QT1050Driver* at42qt1050d, const bool wait4wakeup, const apalTime_t timeout);
  apalExitStatus_t at42qt1050_lld_reset(const AT42QT1050Driver* at42qt1050d, const apalTime_t timeout, const bool wait4wakeup);

  uint16_t at42qt1050_lld_pulse2samples(const uint8_t pulse);
  float at42qt1050_lld_samples2pulse(const uint16_t samples);
  uint16_t at42qt1050_lld_scale2scaling(const uint8_t scale);
  float at42qt1050_lld_scaling2scale(const uint16_t factor);


  /**
   * @brief   Calculates n-th address based on address of register 0.
   * @details Calculation: <scale value> = log2(<scaling factor>
   * )
   * @param[in]   base    Base address = frist register
   * @param[in]   inc     Jump to the next register inc times
   *
   * @return    Calculated register address
   */
  inline at42qt1050_lld_register_t at42qt1050_lld_addr_calc(const at42qt1050_lld_register_t base, const uint8_t inc) {
    apalDbgAssert(inc < 5);

    uint8_t double_result = 0; //16bit access

    switch (base) {
      case AT42QT1050_LLD_REG_KEYSIGNAL_0:         //2 4 2 2
      case AT42QT1050_LLD_REG_REFERENCEDATA_0:     //2 4 2 2
        double_result = 1;
        __attribute__((fallthrough));
      case AT42QT1050_LLD_REG_NEGATIVETHRESHOLD_0: //1 2 1 1
      case AT42QT1050_LLD_REG_PULSE_SCALE_0:       //1 2 1 1
      case AT42QT1050_LLD_REG_INTEGRATOR_AKS_0:    //1 2 1 1
      case AT42QT1050_LLD_REG_CHARGESHAREDELAY_0:  //1 2 1 1
      {
        uint8_t increase = ((inc>1)?inc+1:inc);
        return (at42qt1050_lld_register_t) (((uint8_t) base)+(increase << double_result));
      }
      default:
      {
        apalDbgPrintf("invalid base register 0x%04X\n", base);
        return (at42qt1050_lld_register_t) 0xFF; //does not exist
      }
    }
  }

#ifdef __cplusplus
}
#endif

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROLLD_AT42QT1050_H */

/** @} */

