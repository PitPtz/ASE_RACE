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
 * @file    aos_test_INA219.c
 * @brief   Power monitor driver test types implementation.
 *
 * @addtogroup test_INA219_v1
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "aos_test_INA219.h"
#include <math.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

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

aos_testresult_t aosTestIna219Func(BaseSequentialStream* stream, const aos_test_t* test)
{
  aosDbgCheck(test->data != NULL && ((aos_test_ina219data_t*)(test->data))->inad != NULL);

  // local variables
  aos_testresult_t result;
  int32_t status;
  uint16_t data[6];
  uint16_t write_data = 0x1011;
  uint16_t new_data[6];
  uint16_t reset_data;
  uint16_t test_calib = 0;
  int16_t usensor_data = 0;
  uint16_t busready = 0;
  uint32_t power = 0;
  int32_t shunt;
  uint32_t bus;

  aosTestResultInit(&result);

  chprintf(stream, "read registers...\n");
  status = ina219_lld_read_register(((aos_test_ina219data_t*)test->data)->inad, INA219_LLD_REGISTER_CONFIGURATION, data, 6, ((aos_test_ina219data_t*)test->data)->timeout);
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailed(stream, &result);
  }

  chprintf(stream, "write registers...\n");
  status = ina219_lld_write_register(((aos_test_ina219data_t*)test->data)->inad, INA219_LLD_REGISTER_CONFIGURATION, &write_data, 1, ((aos_test_ina219data_t*)test->data)->timeout);
  status |= ina219_lld_read_register(((aos_test_ina219data_t*)test->data)->inad, INA219_LLD_REGISTER_CONFIGURATION, new_data, 6, ((aos_test_ina219data_t*)test->data)->timeout);
  if (status == APAL_STATUS_SUCCESS && new_data[0] == write_data) {
    uint8_t errors = 0;
    for (uint8_t dataIdx = 1; dataIdx < 6; dataIdx++) {
      if (new_data[dataIdx] != data[dataIdx]) {
        ++errors;
      }
    }
    if (errors == 0) {
      aosTestPassed(stream, &result);
    } else {
      aosTestFailed(stream, &result);
    }
  } else {
    aosTestFailed(stream, &result);
  }

  chprintf(stream, "reset...\n");
  status = ina219_lld_reset(((aos_test_ina219data_t*)test->data)->inad, ((aos_test_ina219data_t*)test->data)->timeout);
  status |= ina219_lld_read_register(((aos_test_ina219data_t*)test->data)->inad, INA219_LLD_REGISTER_CONFIGURATION, &reset_data, 1, ((aos_test_ina219data_t*)test->data)->timeout);
  if (status == APAL_STATUS_SUCCESS && reset_data == 0x399F) {
    aosTestPassed(stream, &result);
  } else {
    chprintf(stream, "\tfailed\n");
    ++result.failed;
  }

  chprintf(stream, "read config...\n");
  ina219_lld_cfg_t ina_config;
  status = ina219_lld_read_config(((aos_test_ina219data_t*)test->data)->inad, &ina_config, ((aos_test_ina219data_t*)test->data)->timeout);
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailed(stream, &result);
  }

  chprintf(stream, "write config...\n");
  ina_config.data = 0x7FFu;
  status = ina219_lld_write_config(((aos_test_ina219data_t*)test->data)->inad, ina_config, ((aos_test_ina219data_t*)test->data)->timeout);
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailed(stream, &result);
  }


  chprintf(stream, "calibrate...\n");
  ina219_lld_calib_input_t calib_in;
  calib_in.shunt_resistance_0 = 0.1f;
  calib_in.max_expected_current_A = 0.075f;
  calib_in.current_lsb_uA = 10;
  calib_in.cfg_reg = ina_config;
  ina219_lld_calib_output_t calib_out;
  status = ina219_lld_calibration(((aos_test_ina219data_t*)test->data)->inad, &calib_in, &calib_out);
  status |= ina219_lld_write_calibration(((aos_test_ina219data_t*)test->data)->inad, calib_out.calibration & 0xFFFEu, ((aos_test_ina219data_t*)test->data)->timeout);
  status |= ina219_lld_write_calibration(((aos_test_ina219data_t*)test->data)->inad, 0xA000, ((aos_test_ina219data_t*)test->data)->timeout);
  ina219_lld_cfg_t test_config;
  ((aos_test_ina219data_t*)test->data)->inad->current_lsb_uA = 0xA;
  status |= ina219_lld_read_config(((aos_test_ina219data_t*)test->data)->inad, &test_config, ((aos_test_ina219data_t*)test->data)->timeout);
  status |= ina219_lld_read_calibration(((aos_test_ina219data_t*)test->data)->inad, &test_calib, ((aos_test_ina219data_t*)test->data)->timeout);
  while (!busready || power == 0) {
    aosThdMSleep(20);
    status |= ina219_lld_bus_conversion_ready(((aos_test_ina219data_t*)test->data)->inad, &busready, ((aos_test_ina219data_t*)test->data)->timeout);
    status |= ina219_lld_read_power(((aos_test_ina219data_t*)test->data)->inad, &power, ((aos_test_ina219data_t*)test->data)->timeout);
  }
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailed(stream, &result);
  }

  chprintf(stream, "read shunt voltage...\n");
  status = ina219_lld_read_shunt_voltage(((aos_test_ina219data_t*)test->data)->inad, &shunt, ((aos_test_ina219data_t*)test->data)->timeout);
  chprintf(stream, "\t\tshunt voltage: %fV\n", (float)shunt/1000000.f);
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailed(stream, &result);
  }

  chprintf(stream, "read bus voltage (%u%% tolerance)...\n", (uint8_t)(((aos_test_ina219data_t*)test->data)->tolerance * 100.f + 0.5f));
  status = ina219_lld_read_bus_voltage(((aos_test_ina219data_t*)test->data)->inad, &bus, ((aos_test_ina219data_t*)test->data)->timeout);
  chprintf(stream, "\t\tbus voltage: %fV\n", (float)bus/1000000.f);
  if ((status == APAL_STATUS_SUCCESS) && (fabs(((float)bus/1000000.f) - ((aos_test_ina219data_t*)test->data)->v_expected) < ((aos_test_ina219data_t*)test->data)->v_expected * ((aos_test_ina219data_t*)test->data)->tolerance)) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailed(stream, &result);
  }

  chprintf(stream, "read power...\n");
  status = ina219_lld_read_power(((aos_test_ina219data_t*)test->data)->inad, &power, ((aos_test_ina219data_t*)test->data)->timeout);
  chprintf(stream, "\t\tpower: %fW\n", (float)(power)/1000000.f);
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailed(stream, &result);
  }

  chprintf(stream, "read current...\n");
  status = ina219_lld_read_current(((aos_test_ina219data_t*)test->data)->inad, &usensor_data, ((aos_test_ina219data_t*)test->data)->timeout);
  chprintf(stream, "\t\tcurrent: %fA\n", (float)(usensor_data)/1000000.f);
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailed(stream, &result);
  }

  aosTestInfoMsg(stream, "driver object memory footprint: %u bytes\n", sizeof(INA219Driver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
