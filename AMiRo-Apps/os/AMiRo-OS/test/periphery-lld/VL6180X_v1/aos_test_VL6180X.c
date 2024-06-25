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
 * @file    aos_test_VL6180X.c
 * @brief   Proximity and ambient light sensor driver test types implementation.
 *
 * @addtogroup test_VL6180X_v1
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "aos_test_VL6180X.h"
#include <string.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/**
 * @brief   Custom I2C slave address to use for tests.
 */
#define VL6180X_LLD_I2C_CUSTOMADDRESS (VL6180X_LLD_I2C_DEFAULTADDRESS - 1)

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

aos_testresult_t aosTestVl6180xFunc(BaseSequentialStream* stream, const aos_test_t* test)
{
  aosDbgCheck((test->data != NULL) &&
              (((aos_test_vl6180xdata_t*)(test->data))->vl6180xd != NULL));

  // local constants
  aos_test_vl6180xdata_t* const testdata = (aos_test_vl6180xdata_t*)test->data;

  // local variables
  aos_testresult_t result;
  int32_t status;
  uint8_t reg_buf[4] = {0};
  VL6180x_AlsData_t alsdata;
  VL6180x_RangeData_t rangedata;

  aosTestResultInit(&result);

  chprintf(stream, "resetting device...\n");
  status = apalControlGpioSet(testdata->vl6180xd->enable, APAL_GPIO_OFF);
  aosThdUSleep(1); // signal must be low for at least 100 ns
  status |= apalControlGpioSet(testdata->vl6180xd->enable, APAL_GPIO_ON);
  // wait until device is ready and retrieve currently set I2C identifier
  while (true) {
    /* Note:
     * This loop should actually call VL6180x_WaitDeviceBooted() function. Due
     * to a bug (probably; reading SYSTEM_FRESH_OUT_OF_RESET register returns
     * the expected value 1 only after power-up, but not after reset via the CE
     * signal pin) however, it may get stuck in an infinite loop. Thus, the
     * register is read manually, but its value is ignored.
     */
    testdata->vl6180xd->addr = VL6180X_LLD_I2C_DEFAULTADDRESS;
    if (VL6180x_RdByte(testdata->vl6180xd, SYSTEM_FRESH_OUT_OF_RESET, reg_buf) == APAL_STATUS_OK) {
      break;
    }
    testdata->vl6180xd->addr = VL6180X_LLD_I2C_CUSTOMADDRESS;
    if (VL6180x_RdByte(testdata->vl6180xd, SYSTEM_FRESH_OUT_OF_RESET, reg_buf) == APAL_STATUS_OK) {
      break;
    }
  }
  chprintf(stream, "current device address is 0x%02X\n", testdata->vl6180xd->addr);
  /* Note:
   * Since VL6180x_InitData() checks the value of the SYSTEM_FRESH_OUT_OF_RESET
   * register, which may be considered invalid (see note above), this function
   * may return CALIBRATION_WARNING.
   */
  status |= VL6180x_InitData(testdata->vl6180xd);
  status |= VL6180x_Prepare(testdata->vl6180xd);
  if (status == 0 || status == CALIBRATION_WARNING) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "%d\n", status);
  }

  chprintf(stream, "reading module ID...\n");
  status = VL6180x_RdByte(testdata->vl6180xd, IDENTIFICATION_MODEL_ID, reg_buf);
  if (status == 0 && reg_buf[0] == VL6180X_LLD_MODELID) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "%d 0x%02X\n", status, reg_buf[0]);
  }

  chprintf(stream, "setting I2C address...\n");
  reg_buf[0] = (testdata->vl6180xd->addr == VL6180X_LLD_I2C_DEFAULTADDRESS) ? VL6180X_LLD_I2C_CUSTOMADDRESS : VL6180X_LLD_I2C_DEFAULTADDRESS;
  status = VL6180x_SetI2CAddress(testdata->vl6180xd, reg_buf[0] << 1);
  testdata->vl6180xd->addr = reg_buf[0];
  status |= VL6180x_RdByte(testdata->vl6180xd, IDENTIFICATION_MODEL_ID, reg_buf);
  if (status == 0 && reg_buf[0] == VL6180X_LLD_MODELID) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "%d 0x%02X\n", status, reg_buf[0]);
  }

  chprintf(stream, "reading ambient light for ten seconds...\n");
  alsdata.lux = 0;
  alsdata.errorStatus = 0;
  status = VL6180x_AlsSetInterMeasurementPeriod(testdata->vl6180xd, 0);
  status |= VL6180x_SetupGPIO1(testdata->vl6180xd, GPIOx_SELECT_OFF, 1);
  status |= VL6180x_AlsConfigInterrupt(testdata->vl6180xd, CONFIG_GPIO_INTERRUPT_NEW_SAMPLE_READY);
  status |= VL6180x_AlsSetSystemMode(testdata->vl6180xd, MODE_CONTINUOUS | MODE_START_STOP);
  for (uint8_t i = 0; i < 10; ++i) {
    aosThdSSleep(1);
    status |= VL6180x_AlsGetMeasurement(testdata->vl6180xd, &alsdata);
    chprintf(stream, "\t\tambient light: %u lux (0x%X)\n", alsdata.lux, alsdata.errorStatus);
  }
  status |= VL6180x_AlsSetSystemMode(testdata->vl6180xd, MODE_START_STOP);
  if (status == 0 && alsdata.lux != 0) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "%d 0x%02X\n", status);
  }

  chprintf(stream, "reading proximity for ten seconds...\n");
  rangedata.range_mm = 0;
  rangedata.errorStatus = 0;
  status = VL6180x_RangeSetInterMeasPeriod(testdata->vl6180xd, 0);
  status |= VL6180x_SetupGPIO1(testdata->vl6180xd, GPIOx_SELECT_OFF, 1);
  status |= VL6180x_RangeConfigInterrupt(testdata->vl6180xd, CONFIG_GPIO_INTERRUPT_NEW_SAMPLE_READY);
  status |= VL6180x_RangeSetSystemMode(testdata->vl6180xd, MODE_CONTINUOUS | MODE_START_STOP);
  for (uint8_t i = 0; i < 10; ++i) {
    aosThdSSleep(1);
    status |= VL6180x_RangeGetMeasurementIfReady(testdata->vl6180xd, &rangedata);
    chprintf(stream, "\t\tdistance: %d mm (0x%X)\n", rangedata.range_mm, rangedata.errorStatus);
  }
  status |= VL6180x_RangeSetSystemMode(testdata->vl6180xd, MODE_START_STOP);
  if (status == 0 && rangedata.range_mm != 0) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "%d 0x%02X\n", status);
  }

  chprintf(stream, "reading ambient light and proximity for ten seconds...\n");
  alsdata.lux = 0;
  alsdata.errorStatus = 0;
  rangedata.range_mm = 0;
  rangedata.errorStatus = 0;
  status = VL6180x_AlsConfigInterrupt(testdata->vl6180xd, CONFIG_GPIO_INTERRUPT_NEW_SAMPLE_READY);
  status |= VL6180x_WrByte(testdata->vl6180xd, INTERLEAVED_MODE__ENABLE, 1);
  status |= VL6180x_AlsSetInterMeasurementPeriod(testdata->vl6180xd, 100);
  status |= VL6180x_AlsSetSystemMode(testdata->vl6180xd, MODE_CONTINUOUS | MODE_START_STOP);
  for (uint8_t i = 0; i < 10; ++i) {
    aosThdSSleep(1);
    status |= VL6180x_AlsGetMeasurement(testdata->vl6180xd, &alsdata);
    status |= VL6180x_RangeGetMeasurementIfReady(testdata->vl6180xd, &rangedata);
    chprintf(stream, "\t\tambient light: %u lux (0x%X)\tdistance: %d mm (0x%X)\n", alsdata.lux, alsdata.errorStatus, rangedata.range_mm, rangedata.errorStatus);
  }
  status |= VL6180x_AlsSetSystemMode(testdata->vl6180xd, MODE_START_STOP);
  status |= VL6180x_WrByte(testdata->vl6180xd, INTERLEAVED_MODE__ENABLE, 0);
  if (status == 0 && alsdata.lux != 0 && rangedata.range_mm != 0) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "%d 0x%02X\n", status);
  }

  chprintf(stream, "reading proximity for ten seconds with sporadic ambient light measurements...\n");
  alsdata.lux = 0;
  alsdata.errorStatus = 0;
  rangedata.range_mm = 0;
  rangedata.errorStatus = 0;
  status = VL6180x_RangeSetInterMeasPeriod(testdata->vl6180xd, 0);
  status |= VL6180x_SetupGPIO1(testdata->vl6180xd, GPIOx_SELECT_OFF, 1);
  status |= VL6180x_RangeConfigInterrupt(testdata->vl6180xd, CONFIG_GPIO_INTERRUPT_NEW_SAMPLE_READY);
  status |= VL6180x_AlsConfigInterrupt(testdata->vl6180xd, CONFIG_GPIO_INTERRUPT_NEW_SAMPLE_READY);
  status |= VL6180x_RangeSetSystemMode(testdata->vl6180xd, MODE_CONTINUOUS | MODE_START_STOP);
  for (uint8_t i = 0; i < 10; ++i) {
    aosThdSSleep(1);
    status |= VL6180x_RangeGetMeasurementIfReady(testdata->vl6180xd, &rangedata);
    if (i % 2) {
      chprintf(stream, "\t\tdistance: %d mm (0x%X)\n", rangedata.range_mm, rangedata.errorStatus);
    } else {
      status |= VL6180x_AlsSetSystemMode(testdata->vl6180xd, MODE_SINGLESHOT);
      status |= VL6180x_AlsPollMeasurement(testdata->vl6180xd, &alsdata);
      chprintf(stream, "\t\tdistance: %d mm (0x%X)\tambient light: %u lux (0x%X)\n", rangedata.range_mm, rangedata.errorStatus, alsdata.lux, alsdata.errorStatus);
    }
  }
  status |= VL6180x_RangeSetSystemMode(testdata->vl6180xd, MODE_START_STOP);
  if (status == 0 && rangedata.range_mm != 0 && alsdata.lux != 0) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "%d 0x%02X\n", status);
  }

  aosTestInfoMsg(stream, "driver object memory footprint: %u bytes\n", sizeof(VL6180XDriver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
