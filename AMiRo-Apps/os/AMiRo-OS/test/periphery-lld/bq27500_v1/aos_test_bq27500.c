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
 * @file    aos_test_bq27500.c
 * @brief   Fuel gauge driver test types implementation.
 *
 * @addtogroup test_bq27500_v1
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "aos_test_bq27500.h"
#include <string.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

// change saved unseal keys to test bruteforcing
#if defined(BQ27500_TEST_BRUTEFORCE)

#if defined(BQ27500_LLD_DEFAULT_UNSEAL_KEY0)
#undef BQ27500_LLD_DEFAULT_UNSEAL_KEY0
#define BQ27500_LLD_DEFAULT_UNSEAL_KEY0 0x1234
#endif /* defined(BQ27500_LLD_DEFAULT_UNSEAL_KEY0) */

#if defined(BQ27500_LLD_DEFAULT_UNSEAL_KEY1)
#undef BQ27500_LLD_DEFAULT_UNSEAL_KEY1
#define BQ27500_LLD_DEFAULT_UNSEAL_KEY1 0x5678
#endif /* defined(BQ27500_LLD_DEFAULT_UNSEAL_KEY1) */

#endif /* defined(BQ27500_TEST_BRUTEFORCE) */

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

bq27500_lld_control_status_t _aosTestBq27500TryUnseal(BQ27500Driver* driver, uint16_t key0, uint16_t key1, apalTime_t timeout) {
  uint16_t dst;
  bq27500_lld_control_status_t ctrl;
  bq27500_lld_send_ctnl_data(driver, key1, timeout);
  aosThdUSleep(1);
  bq27500_lld_send_ctnl_data(driver, key0, timeout);
  aosThdUSleep(1);
  bq27500_lld_sub_command_call(driver, BQ27500_LLD_SUB_CMD_CONTROL_STATUS, timeout);
  aosThdUSleep(1);
  bq27500_lld_std_command(driver, BQ27500_LLD_STD_CMD_Control, &dst, timeout);
  bq27500_lld_sub_command_read(driver, &ctrl.value, timeout);
  return ctrl;
}

uint8_t _aosTestBq27500BruteforceSealedKeyBitflips(BaseSequentialStream* stream, BQ27500Driver* driver, uint16_t key0, uint16_t key1, apalTime_t timeout) {
  bq27500_lld_control_status_t ctrl;
  uint16_t k0;
  uint16_t k1 = key1;
  for (uint8_t i = 0; i < 16; i++) {
    k0 = key0 ^ (1 << i); // flip bit i
    ctrl = _aosTestBq27500TryUnseal(driver, k0, k1, timeout);
    if (ctrl.content.ss == 0x0) {
      chprintf(stream, "\t\tSUCCESS!\n");
      chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", k0, k1);
      return 1;
    }
  }
  k0 = key0;
  for (uint8_t i = 0; i < 16; i++) {
    k1 = key1 ^ (1 << i); // flip bit i
    ctrl = _aosTestBq27500TryUnseal(driver, k0, k1, timeout);
    if (ctrl.content.ss == 0x0) {
      chprintf(stream, "\t\tSUCCESS!\n");
      chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", k0, k1);
      return 1;
    }
  }
  return 0;
}

void _aosTestBq27500BruteforceSealedKey(BaseSequentialStream* stream, BQ27500Driver* driver, apalTime_t timeout) {
  chprintf(stream, "start bruteforcing sealed keys...\n");
  bq27500_lld_control_status_t ctrl;
  uint16_t key0_reversed = 0x7236;
  uint16_t key1_reversed = 0x1404;
  uint16_t key0 = BQ27500_LLD_DEFAULT_UNSEAL_KEY0;
  uint16_t key1 = BQ27500_LLD_DEFAULT_UNSEAL_KEY1;
  uint16_t k0 = key0;
  uint16_t k1 = key1;

  // testing default keys in different orders
  chprintf(stream, "\ttry reversed byte order and different key order...\n");
  // default unseal keys
  ctrl = _aosTestBq27500TryUnseal(driver, k0, k1, timeout);
  if (ctrl.content.ss == 0x0) {
    chprintf(stream, "\t\tSUCCESS!\n");
    chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", k0, k1);
    return;
  }
  // default unseal keys in reversed order
  ctrl = _aosTestBq27500TryUnseal(driver, k1, k0, timeout);
  if (ctrl.content.ss == 0x0) {
    chprintf(stream, "\t\tSUCCESS!\n");
    chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", k0, k1);
    return;
  }
  // byte reversed keys
  k0 = key0_reversed;
  k1 = key1_reversed;
  ctrl = _aosTestBq27500TryUnseal(driver, k0, k1, timeout);
  if (ctrl.content.ss == 0x0) {
    chprintf(stream, "\t\tSUCCESS!\n");
    chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", k0, k1);
    return;
  }
  // byte reversed keys in reversed order
  ctrl = _aosTestBq27500TryUnseal(driver, k1, k0, timeout);
  if (ctrl.content.ss == 0x0) {
    chprintf(stream, "\t\tSUCCESS!\n");
    chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", k0, k1);
    return;
  }
  chprintf(stream, "\t\tfailed\n");


  // testing single bit flips of the default keys in different orders
  chprintf(stream, "\ttry single bit flips of default keys...\n");
  // default unseal keys
  uint8_t result = 0;
  result = _aosTestBq27500BruteforceSealedKeyBitflips(stream, driver, key0, key1, timeout);
  if (result == 1) {
    return;
  }
  // default unseal keys in reversed order
  result = _aosTestBq27500BruteforceSealedKeyBitflips(stream, driver, key1, key0, timeout);
  if (result == 1) {
    return;
  }
  // byte reversed keys
  result = _aosTestBq27500BruteforceSealedKeyBitflips(stream, driver, key0_reversed, key1_reversed, timeout);
  if (result == 1) {
    return;
  }
  // byte reversed keys in reversed order
  result = _aosTestBq27500BruteforceSealedKeyBitflips(stream, driver, key1_reversed, key0_reversed, timeout);
  if (result == 1) {
    return;
  }
  chprintf(stream, "\t\tfailed\n");


  // bruteforcing one of the keys, assuming only one of them was changed
  chprintf(stream, "\ttry bruteforcing a single key...\n");
  // default unseal key0
  for (uint32_t i = 0; i <= 0xFFFF; i++) {
    ctrl = _aosTestBq27500TryUnseal(driver, key0, i, timeout);
    if (ctrl.content.ss == 0x0) {
      chprintf(stream, "\t\tSUCCESS!\n");
      chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", key0, i);
      return;
    }
  }
  chprintf(stream, "\t\tkey failed. 1/8\n");
  // reversed unseal key0
  for (uint32_t i = 0; i <= 0xFFFF; i++) {
    ctrl = _aosTestBq27500TryUnseal(driver, key0_reversed, i, timeout);
    if (ctrl.content.ss == 0x0) {
      chprintf(stream, "\t\tSUCCESS!\n");
      chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", key0_reversed, i);
      return;
    }
  }
  chprintf(stream, "\t\tkey failed. 2/8\n");
  // default unseal key0 in reversed order
  for (uint32_t i = 0; i <= 0xFFFF; i++) {
    ctrl = _aosTestBq27500TryUnseal(driver, i, key0, timeout);
    if (ctrl.content.ss == 0x0) {
      chprintf(stream, "\t\tSUCCESS!\n");
      chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", i, key0);
      return;
    }
  }
  chprintf(stream, "\t\tkey failed. 3/8\n");
  // reversed unseal key0 in reversed order
  for (uint32_t i = 0; i <= 0xFFFF; i++) {
    ctrl = _aosTestBq27500TryUnseal(driver, i, key0_reversed, timeout);
    if (ctrl.content.ss == 0x0) {
      chprintf(stream, "\t\tSUCCESS!\n");
      chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", i, key0_reversed);
      return;
    }
  }
  chprintf(stream, "\t\tkey failed. 4/8\n");
  // default unseal key1
  for (uint32_t i = 0; i <= 0xFFFF; i++) {
    ctrl = _aosTestBq27500TryUnseal(driver, i, key1, timeout);
    if (ctrl.content.ss == 0x0) {
      chprintf(stream, "\t\tSUCCESS!\n");
      chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", i, key1);
      return;
    }
  }
  chprintf(stream, "\t\tkey failed. 5/8\n");
  // reversed unseal key1
  for (uint32_t i = 0; i <= 0xFFFF; i++) {
    ctrl = _aosTestBq27500TryUnseal(driver, i, key1_reversed, timeout);
    if (ctrl.content.ss == 0x0) {
      chprintf(stream, "\t\tSUCCESS!\n");
      chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", i, key1_reversed);
      return;
    }
  }
  chprintf(stream, "\t\tkey failed. 6/8\n");
  // default unseal key1 in reversed order
  for (uint32_t i = 0; i <= 0xFFFF; i++) {
    ctrl = _aosTestBq27500TryUnseal(driver, key1, i, timeout);
    if (ctrl.content.ss == 0x0) {
      chprintf(stream, "\t\tSUCCESS!\n");
      chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", key1, i);
      return;
    }
  }
  chprintf(stream, "\t\tkey failed. 7/8\n");
  // reversed unseal key1 in reversed order
  for (uint32_t i = 0; i <= 0xFFFF; i++) {
    ctrl = _aosTestBq27500TryUnseal(driver, key1_reversed, i, timeout);
    if (ctrl.content.ss == 0x0) {
      chprintf(stream, "\t\tSUCCESS!\n");
      chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", key1_reversed, i);
      return;
    }
  }
  chprintf(stream, "\t\tkey failed. 8/8\n");
  chprintf(stream, "\t\tfailed\n");


  // full bruteforce
  chprintf(stream, "\tbruteforcing both keys...\t");
  for (uint32_t i = 0; i <= 0xFFFF; i++) {
    chprintf(stream, "\t\ti: %u\n", i);
    for (uint32_t j = 0; j <= 0xFFFF; j++) {
      ctrl = _aosTestBq27500TryUnseal(driver, i, j, timeout);
      if (ctrl.content.ss == 0x0) {
        chprintf(stream, "\t\tSUCCESS!\n");
        chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", i, j);
        return;
      }
    }
  }

  chprintf(stream, "\t\tfailed, no keys could be found");
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

aos_testresult_t aosTestBq27500Func(BaseSequentialStream* stream, const aos_test_t* test)
{
  aosDbgCheck(test->data != NULL && ((aos_test_bq27500data_t*)(test->data))->driver != NULL);

  // local variables
  aos_testresult_t result;
  int32_t status;
  bq27500_lld_batlow_t bl;
  bq27500_lld_batgood_t bg;
  uint16_t dst;
  bq27500_lld_flags_t flags;
  uint16_t subdata = 0;
  uint8_t original_length;
  char original_name[8+1];
  uint8_t val = 0x00;
  uint8_t block[32];
  char new_name[] = "test";
  uint8_t new_lenght;
  char name[8+1] = {'\0'};
  uint8_t sum = 0;
  bool success;
  bool success2;

  aosTestResultInit(&result);

  chprintf(stream, "read battery low gpio...\n");
  status = bq27500_lld_read_batlow(((aos_test_bq27500data_t*)test->data)->driver, &bl);
  chprintf(stream, "\t\tbattery low: 0x%X\n", bl);
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "read battery good gpio...\n");
  status = bq27500_lld_read_batgood(((aos_test_bq27500data_t*)test->data)->driver, &bg);
  chprintf(stream, "\t\tbattery good: 0x%X\n", bg);
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "std command FLAGS...\n");
  status = bq27500_lld_std_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_STD_CMD_Flags, &flags.value, ((aos_test_bq27500data_t*)test->data)->timeout);
  chprintf(stream, "\t\tflags: 0x%04X\n", flags.value);
  chprintf(stream, "\t\tbattery detected: 0x%X\n", flags.content.bat_det);
  chprintf(stream, "\t\tbattery fully charged: 0x%X\n", flags.content.fc);
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "std command CTNL...\n");
  status = bq27500_lld_std_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_STD_CMD_Control, &dst, ((aos_test_bq27500data_t*)test->data)->timeout);
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "sub command: CTRL Status...\n");
  aosThdUSleep(1);
  status = bq27500_lld_sub_command_call(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_SUB_CMD_CONTROL_STATUS, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdUSleep(1);
  status |= bq27500_lld_std_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_STD_CMD_Control, &dst, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdUSleep(1);
  bq27500_lld_control_status_t ctrl;
  status |= bq27500_lld_sub_command_read(((aos_test_bq27500data_t*)test->data)->driver, &ctrl.value, ((aos_test_bq27500data_t*)test->data)->timeout);
  chprintf(stream, "\t\tdst: 0x%X\n", ctrl.value);
  chprintf(stream, "\t\tsleep: 0x%X\n", ctrl.content.sleep);
  chprintf(stream, "\t\thibernate: 0x%X\n", ctrl.content.hibernate);
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "sub command: firmware version...\n");
  status = bq27500_lld_sub_command_call(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_SUB_CMD_FW_VERSION, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(1);
  bq27500_lld_version_t version;
  status |= bq27500_lld_sub_command_read(((aos_test_bq27500data_t*)test->data)->driver, &version.value, ((aos_test_bq27500data_t*)test->data)->timeout);
  chprintf(stream, "\t\tfirmware version: %X%X-%X%X\n", version.content.major_high, version.content.major_low, version.content.minor_high, version.content.minor_low);
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "sub command: hardware version...\n");
  status = bq27500_lld_sub_command_call(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_SUB_CMD_HW_VERSION, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(1);
  status |= bq27500_lld_sub_command_read(((aos_test_bq27500data_t*)test->data)->driver, &version.value, ((aos_test_bq27500data_t*)test->data)->timeout);
  chprintf(stream, "\t\thardware version: %X%X-%X%X\n", version.content.major_high, version.content.major_low, version.content.minor_high, version.content.minor_low);
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "ext command: device name length...\n");
  status = bq27500_lld_ext_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_EXT_CMD_DeviceNameLength, BQ27500_LLD_EXT_CMD_READ, &original_length, 1, 0, ((aos_test_bq27500data_t*)test->data)->timeout);
  chprintf(stream, "\t\tdevice name length: %d\n", original_length);
  if (status == APAL_STATUS_SUCCESS && original_length <= 8) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
    original_length = 8;
  }

  chprintf(stream, "ext command: device name (read)...\n");
  status = bq27500_lld_ext_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_EXT_CMD_DeviceName, BQ27500_LLD_EXT_CMD_READ, (uint8_t*)original_name, original_length, 0, ((aos_test_bq27500data_t*)test->data)->timeout);
  original_name[original_length] = '\0';
  chprintf(stream, "\t\tdevice name: %s\n", original_name);
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "battery info std commands...\n");
  status = bq27500_lld_std_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_STD_CMD_Temperature, &dst, ((aos_test_bq27500data_t*)test->data)->timeout);
  chprintf(stream, "\t\ttemperature: %fK (%fC)\n", (float)dst/10.0f, (float)dst/10.0f-273.5f);
  status |= bq27500_lld_std_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_STD_CMD_FullAvailableCapacity, &dst, ((aos_test_bq27500data_t*)test->data)->timeout);
  chprintf(stream, "\t\tfull available capacity: %umAh\n", dst);
  status |= bq27500_lld_std_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_STD_CMD_FullChargeCapacity, &dst, ((aos_test_bq27500data_t*)test->data)->timeout);
  chprintf(stream, "\t\tfull charge capacity: %umAh\n", dst);
  status |= bq27500_lld_std_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_STD_CMD_RemainingCapacity, &dst, ((aos_test_bq27500data_t*)test->data)->timeout);
  chprintf(stream, "\t\tremaining capacity: %umAh\n", dst);
  status |= bq27500_lld_std_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_STD_CMD_Voltage, &dst, ((aos_test_bq27500data_t*)test->data)->timeout);
  chprintf(stream, "\t\tvoltage: %umV\n", dst);
  status |= bq27500_lld_std_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_STD_CMD_AverageCurrent, &dst, ((aos_test_bq27500data_t*)test->data)->timeout);
  chprintf(stream, "\t\taverage current: %dmA\n", (int8_t)dst);
  status |= bq27500_lld_std_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_STD_CMD_AveragePower, &dst, ((aos_test_bq27500data_t*)test->data)->timeout);
  chprintf(stream, "\t\taverage power: %dmW\n", (int8_t)dst);
  status |= bq27500_lld_std_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_STD_CMD_StateOfCharge, &dst, ((aos_test_bq27500data_t*)test->data)->timeout);
  chprintf(stream, "\t\tstate of charge: %u%%\n", dst);
  status |= bq27500_lld_std_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_STD_CMD_TimeToFull, &dst, ((aos_test_bq27500data_t*)test->data)->timeout);
  if (dst != (uint16_t)~0) {
    chprintf(stream, "\t\ttime to full: %umin\n", dst);
  } else {
    chprintf(stream, "\t\ttime to full: (not charging)\n", dst);
  }
  status |= bq27500_lld_std_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_STD_CMD_TimeToEmpty, &dst, ((aos_test_bq27500data_t*)test->data)->timeout);
  if (dst != (uint16_t)~0) {
    chprintf(stream, "\t\ttime to empty: %umin\n", dst);
  } else {
    chprintf(stream, "\t\ttime to empty: (not discharging)\n", dst);
  }
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "check sealed state...\n");
  status = bq27500_lld_std_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_STD_CMD_Control, &dst, ((aos_test_bq27500data_t*)test->data)->timeout);
  status |= bq27500_lld_sub_command_call(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_SUB_CMD_CONTROL_STATUS, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(1);
  status |= bq27500_lld_sub_command_read(((aos_test_bq27500data_t*)test->data)->driver, &ctrl.value, ((aos_test_bq27500data_t*)test->data)->timeout);
  chprintf(stream, "\t\tsealed: 0x%X\n", ctrl.content.ss);
  chprintf(stream, "\t\tfull access sealed: 0x%X\n", ctrl.content.fas);
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "unseale...\n");
  status = bq27500_lld_send_ctnl_data(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_DEFAULT_UNSEAL_KEY1, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(1);
  status |= bq27500_lld_send_ctnl_data(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_DEFAULT_UNSEAL_KEY0, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(1);
  status |= bq27500_lld_sub_command_call(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_SUB_CMD_CONTROL_STATUS, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(1);
  status |= bq27500_lld_std_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_STD_CMD_Control, &dst, ((aos_test_bq27500data_t*)test->data)->timeout);
  status |= bq27500_lld_sub_command_read(((aos_test_bq27500data_t*)test->data)->driver, &ctrl.value, ((aos_test_bq27500data_t*)test->data)->timeout);
  if (status == APAL_STATUS_SUCCESS && ctrl.content.ss == 0x0) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
    _aosTestBq27500BruteforceSealedKey(stream, ((aos_test_bq27500data_t*)test->data)->driver, ((aos_test_bq27500data_t*)test->data)->timeout);
  }

  chprintf(stream, "read device name from data flash...\n");
  status = bq27500_lld_ext_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_EXT_CMD_BlockDataControl, BQ27500_LLD_EXT_CMD_WRITE, &val, 1, 0, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(50);
  val = 0x30;
  status |= bq27500_lld_ext_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_EXT_CMD_DataFlashClass, BQ27500_LLD_EXT_CMD_WRITE, &val, 1, 0, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(50);
  val = 0;
  status |= bq27500_lld_ext_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_EXT_CMD_DataFlashBlock, BQ27500_LLD_EXT_CMD_WRITE, &val, 1, 0, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(50);
  status |= bq27500_lld_ext_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_EXT_CMD_BlockData, BQ27500_LLD_EXT_CMD_READ, (uint8_t*)block, original_length, 13, ((aos_test_bq27500data_t*)test->data)->timeout);
  block[original_length] = '\0';
  chprintf(stream, "\t\tdevice name: %s\n", block);
  if (status == APAL_STATUS_SUCCESS && (strcmp((char*)block, (char*)original_name) == 0)) {
    aosTestPassed(stream, &result);
  } else {
    chprintf(stream, "\t\tread data: ");
    for (uint8_t blockIdx = 0; blockIdx < original_length; blockIdx++) {
      chprintf(stream, "0x%02X\n", block[blockIdx]);
    }
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "change device name in data flash to \"test\"...\n");
  status = bq27500_lld_ext_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_EXT_CMD_BlockData, BQ27500_LLD_EXT_CMD_WRITE, (uint8_t*)new_name, 5, 13, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(50);
  status |= bq27500_lld_ext_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_EXT_CMD_BlockData, BQ27500_LLD_EXT_CMD_READ, (uint8_t*)block, 32, 0, ((aos_test_bq27500data_t*)test->data)->timeout);
  // compute blockdata checksum
  status |= bq27500_lld_compute_blockdata_checksum(block, &sum);
  // write checksum to BlockDataChecksum, triggering the write of BlackData
  status |= bq27500_lld_ext_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_EXT_CMD_BlockDataCheckSum, BQ27500_LLD_EXT_CMD_WRITE, &sum, 1, 0, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(50);
  status |= bq27500_lld_ext_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_EXT_CMD_DeviceNameLength, BQ27500_LLD_EXT_CMD_READ, &new_lenght, 1, 0, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(50);
  // read out device name, to see if changing it was successfull
  status |= bq27500_lld_ext_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_EXT_CMD_DeviceName, BQ27500_LLD_EXT_CMD_READ, (uint8_t*)name, new_lenght, 0, ((aos_test_bq27500data_t*)test->data)->timeout);
  name[new_lenght] = '\0';
  chprintf(stream, "\t\tdevice name: %s\n", name);
  success = (strcmp(name, new_name) == 0);

  // change device name back to original name
  val = 0x00;
  status |= bq27500_lld_ext_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_EXT_CMD_BlockDataControl, BQ27500_LLD_EXT_CMD_WRITE, &val, 1, 0, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(50);
  val = 0x30;
  status |= bq27500_lld_ext_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_EXT_CMD_DataFlashClass, BQ27500_LLD_EXT_CMD_WRITE, &val, 1, 0, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(50);
  val = 0;
  status |= bq27500_lld_ext_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_EXT_CMD_DataFlashBlock, BQ27500_LLD_EXT_CMD_WRITE, &val, 1, 0, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(50);
  status |= bq27500_lld_ext_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_EXT_CMD_BlockData, BQ27500_LLD_EXT_CMD_WRITE, (uint8_t*)original_name, 7, 13, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(50);
  status |= bq27500_lld_ext_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_EXT_CMD_BlockData, BQ27500_LLD_EXT_CMD_READ, (uint8_t*)block, 32, 0, ((aos_test_bq27500data_t*)test->data)->timeout);
  // compute blockdata checksum
  sum = 0;
  status |= bq27500_lld_compute_blockdata_checksum(block, &sum);
  // write checksum to BlockDataChecksum, triggering the write of BlackData
  status |= bq27500_lld_ext_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_EXT_CMD_BlockDataCheckSum, BQ27500_LLD_EXT_CMD_WRITE, &sum, 1, 0, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(1000);
  status |= bq27500_lld_ext_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_EXT_CMD_DeviceNameLength, BQ27500_LLD_EXT_CMD_READ, &original_length, 1, 0, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(50);
  // read device name, to see if changing it back to the original name was successfull
  status |= bq27500_lld_ext_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_EXT_CMD_DeviceName, BQ27500_LLD_EXT_CMD_READ, (uint8_t*)name, original_length, 0, ((aos_test_bq27500data_t*)test->data)->timeout);
  success2 = (strcmp(name, original_name) == 0);
  name[original_length] = '\0';
  chprintf(stream, "\t\tchanged back to name: %s, original_name: %s\n", name, original_name);
  if (status == APAL_STATUS_OK && ((success && success2) || (ctrl.content.sleep == 0))) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X, changing: 0x%X - changing back: 0x%X\n", status, success, success2);
  }

  chprintf(stream, "seal...\n");
  status = bq27500_lld_std_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_STD_CMD_Control, &dst, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(50);
  status |= bq27500_lld_sub_command_call(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_SUB_CMD_SEALED, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(50);
  status |= bq27500_lld_std_command(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_STD_CMD_Control, &dst, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(50);
  status |= bq27500_lld_sub_command_call(((aos_test_bq27500data_t*)test->data)->driver, BQ27500_LLD_SUB_CMD_CONTROL_STATUS, ((aos_test_bq27500data_t*)test->data)->timeout);
  aosThdMSleep(50);
  status |= bq27500_lld_sub_command_read(((aos_test_bq27500data_t*)test->data)->driver, &ctrl.value, ((aos_test_bq27500data_t*)test->data)->timeout);
  if (status == APAL_STATUS_SUCCESS && ctrl.content.ss == 0x1) {
    aosTestPassed(stream, &result);
  } else {
    chprintf(stream, "\tfailed (0x%X)\n", status);
    aosTestFailedMsg(stream, &result, "0x%08X, ctrl 0x%X\n", status, subdata);
    ++result.failed;
  }

  aosTestInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(BQ27500Driver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
