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
 * @file    aos_test_bq27500_bq241xx.c
 * @brief   Fuel gauge & battery charger drivers test types implementation.
 *
 * @addtogroup test_bq27500_v1_bq241xx_v1
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "aos_test_bq27500_bq241xx.h"

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


aos_testresult_t aosTestBq27500Bq241xxFunc(BaseSequentialStream *stream, const aos_test_t *test)
{
  aosDbgCheck(test->data != NULL && ((aos_test_bq27500bq241xxdata_t*)test->data)->bq27500 != NULL && ((aos_test_bq27500bq241xxdata_t*)test->data)->bq241xx != NULL);

  // local variables
  aos_testresult_t result;
  int32_t status;
  uint16_t dst;
  bq27500_lld_flags_t flags;
  bq27500_lld_batgood_t bg;
  bq241xx_lld_enable_t charger_enabled;
  uint32_t sleeptime_s;
  bq241xx_lld_charge_state_t charge;

  aosTestResultInit(&result);

  chprintf(stream, "check for battery...\n");
  status = bq27500_lld_std_command(((aos_test_bq27500bq241xxdata_t*)test->data)->bq27500, BQ27500_LLD_STD_CMD_Flags, &dst, ((aos_test_bq27500bq241xxdata_t*)test->data)->timeout);
  flags.value = dst;
  chprintf(stream, "\t\tbattery detected: %s\n", flags.content.bat_det ? "yes" : "no");
  chprintf(stream, "\t\tbattery fully charged: %s\n", flags.content.fc ? "yes" : "no");
  status |= bq27500_lld_read_batgood(((aos_test_bq27500bq241xxdata_t*)test->data)->bq27500, &bg);
  chprintf(stream, "\t\tbattery good: %s\n", (bg == BQ27500_LLD_BATTERY_GOOD) ? "yes" : "no");
  if (status == APAL_STATUS_SUCCESS) {
    if (!flags.content.bat_det) {
      aosTestPassedMsg(stream, &result, "no battery detected, aborting\n");
      return result;
    } else if (!bg) {
      aosTestPassedMsg(stream, &result, "battery damaged, aborting\n");
      return result;
    } else {
      aosTestPassed(stream, &result);
    }
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X; aborting\n", status);
    return result;
  }

  chprintf(stream, "get current charger setting...\n");
  status = bq241xx_lld_get_enabled(((aos_test_bq27500bq241xxdata_t*)test->data)->bq241xx, &charger_enabled);
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassedMsg(stream, &result, "currently %s\n", (charger_enabled == BQ241xx_LLD_ENABLED) ? "enabled" : "disabled");
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  // disable/enable charger
  for (uint8_t iteration = 0; iteration < 2; ++iteration) {
    if (charger_enabled == BQ241xx_LLD_ENABLED) {
      sleeptime_s = 120;
      chprintf(stream, "disable charger...\n");
      status = bq241xx_lld_set_enabled(((aos_test_bq27500bq241xxdata_t*)test->data)->bq241xx, BQ241xx_LLD_DISABLED);
      aosThdSSleep(1);
      status |= bq241xx_lld_get_enabled(((aos_test_bq27500bq241xxdata_t*)test->data)->bq241xx, &charger_enabled);
      status |= (charger_enabled == BQ241xx_LLD_DISABLED) ? 0x0000 : 0x0100;
      for (uint32_t s = 0; s < sleeptime_s; ++s) {
        int nchars = chprintf(stream, "%us / %us", s, sleeptime_s);
        aosThdSSleep(1);
        for (int c = 0; c < nchars; ++c) {
          chprintf(stream, "\b \b");
        }
      }
      status |= bq241xx_lld_get_charge_status(((aos_test_bq27500bq241xxdata_t*)test->data)->bq241xx, &charge);
      chprintf(stream, "\t\tcharge status: %scharging\n", (charge == BQ241xx_LLD_CHARGING) ? "" : "not ");
      status |= bq27500_lld_std_command(((aos_test_bq27500bq241xxdata_t*)test->data)->bq27500, BQ27500_LLD_STD_CMD_TimeToFull, &dst, ((aos_test_bq27500bq241xxdata_t*)test->data)->timeout);
      if (status == APAL_STATUS_SUCCESS) {
        if (dst == 0xFFFF || dst == 0) {
          aosTestPassedMsg(stream, &result, "battery %sfull\n", flags.content.fc ? "" : "not ");
        } else {
          aosTestFailedMsg(stream, &result, "battery %sfull but charging (TTF = %umin)\n", flags.content.fc ? "" : "not ", dst);
        }
      } else {
        aosTestFailedMsg(stream, &result, "0x%08X\n", status);
      }
    }
    else { /* charger_enabled != BQ241xx_LLD_ENABLED */
      sleeptime_s = 90;
      chprintf(stream, "enable charger...\n");
      status = bq241xx_lld_set_enabled(((aos_test_bq27500bq241xxdata_t*)test->data)->bq241xx, BQ241xx_LLD_ENABLED);
      aosThdSSleep(1);
      status |= bq241xx_lld_get_enabled(((aos_test_bq27500bq241xxdata_t*)test->data)->bq241xx, &charger_enabled);
      status |= (charger_enabled == BQ241xx_LLD_ENABLED) ? 0x0000 : 0x0100;
      for (uint32_t s = 0; s < sleeptime_s; ++s) {
        int nchars = chprintf(stream, "%us / %us", s, sleeptime_s);
        aosThdSSleep(1);
        for (int c = 0; c < nchars; ++c) {
          chprintf(stream, "\b \b");
        }
      }
      status |= bq241xx_lld_get_charge_status(((aos_test_bq27500bq241xxdata_t*)test->data)->bq241xx, &charge);
      chprintf(stream, "\t\tcharge status: %scharging\n", (charge == BQ241xx_LLD_CHARGING) ? "" : "not ");
      status |= bq27500_lld_std_command(((aos_test_bq27500bq241xxdata_t*)test->data)->bq27500, BQ27500_LLD_STD_CMD_TimeToFull, &dst, ((aos_test_bq27500bq241xxdata_t*)test->data)->timeout);
      if (status == APAL_STATUS_SUCCESS) {
        if (((dst == 0xFFFF || dst == 0) && flags.content.fc) || (!(dst == 0xFFFF || dst == 0) && !flags.content.fc)) {
          aosTestPassedMsg(stream, &result, "battery %sfull and %scharging (TTF = %umin)\n", flags.content.fc ? "" : "not ", (dst != 0xFFFF || dst != 0) ? "" : "not ", dst);
        } else {
          aosTestFailedMsg(stream, &result, "battery %sfull and %scharging (TTF = %umin)\n", flags.content.fc ? "" : "not ", (dst != 0xFFFF || dst != 0) ? "" : "not ", dst);
        }
      } else {
        aosTestFailedMsg(stream, &result, "0x%08X\n", status);
      }
    }

    bq241xx_lld_get_enabled(((aos_test_bq27500bq241xxdata_t*)test->data)->bq241xx, &charger_enabled);
  }

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
