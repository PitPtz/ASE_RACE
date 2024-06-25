
/*
* Copyright (c) 2017, STMicroelectronics - All Rights Reserved
*
* This file is part of VL53L1 Core and is dual licensed,
* either 'STMicroelectronics
* Proprietary license'
* or 'BSD 3-clause "New" or "Revised" License' , at your option.
*
********************************************************************************
*
* 'STMicroelectronics Proprietary license'
*
********************************************************************************
*
* License terms: STMicroelectronics Proprietary in accordance with licensing
* terms at www.st.com/sla0081
*
* STMicroelectronics confidential
* Reproduction and Communication of this document is strictly prohibited unless
* specifically authorized in writing by STMicroelectronics.
*
*
********************************************************************************
*
* Alternatively, VL53L1 Core may be distributed under the terms of
* 'BSD 3-clause "New" or "Revised" License', in which case the following
* provisions apply instead of the ones mentioned above :
*
********************************************************************************
*
* License terms: BSD 3-clause "New" or "Revised" License.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors
* may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*
********************************************************************************
*
* Modified for use within AMiRo-LLD.
*
* AMiRo-LLD is a compilation of low-level hardware drivers for the Autonomous
* Mini Robot (AMiRo) platform.
* Copyright (C) 2016..2022  Thomas Schöpping et al.
*
********************************************************************************
*
*/

#ifndef _VL53L1_PLATFORM_USER_DATA_H_
#define _VL53L1_PLATFORM_USER_DATA_H_

#include "vl53l1_def.h"
#include <periphAL.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief VL53L1X interface related data structure.
 */
typedef struct {
  /**
   * @brief   Pointer to the associated I2C driver.
   */
  apalI2CDriver_t* i2cd;

  /**
   * @brief   Currently set I2C device address.
   */
  apalI2Caddr_t addr;

  /**
   * @brief   Pointer to the GPIO driver for the GPIO1 signal.
   */
  const apalControlGpio_t* gpio1;

  /**
   * @brief   Pointer to the GPIO driver for the XSHUT signal.
   */
  const apalControlGpio_t* xshut;
} VL53L1_IfData_t;

/**
 * @brief   VL53L1X device driver structure as to be used by the ST API.
 */
typedef struct {
  /**
   * @brief   Device data as specified by the ST API.
   */
  VL53L1_DevData_t   Data;

  /**
   * @brief   Interface data.
   */
  VL53L1_IfData_t     Interface;

} VL53L1_Dev_t;

/**
 * @brief   Typedef as it is used by the ST API.
 */
typedef VL53L1_Dev_t *VL53L1_DEV;

/**
 * @brief   Typedef matching the PeriphAL conventions.
 */
typedef VL53L1_Dev_t VL53L1XDriver;

#define VL53L1DevDataGet(Dev, field) (Dev->Data.field)
#define VL53L1DevDataSet(Dev, field, VL53L1_PRM_00005) ((Dev->Data.field) = (VL53L1_PRM_00005))
#define VL53L1DevStructGetLLDriverHandle(Dev) (&Dev->Data.LLData)
#define VL53L1DevStructGetLLResultsHandle(Dev) (&Dev->Data.llresults)

#ifdef __cplusplus
}
#endif

#endif /* _VL53L1_PLATFORM_USER_DATA_H_ */
