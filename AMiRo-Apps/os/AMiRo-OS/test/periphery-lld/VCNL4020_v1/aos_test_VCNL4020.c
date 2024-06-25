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
 * @file    aos_test_VCNL4020.c
 * @brief   Proximity sensor with ambient light measurement driver test types implementation.
 *
 * @addtogroup test_VCNL4020_v1
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "aos_test_VCNL4020.h"

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

#define INTERRUPT_EVENT_ID                      1

#define INTERRUPT_THRESHOLD                     0x2000

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

aos_testresult_t aosTestVcnl4020Func(BaseSequentialStream* stream, const aos_test_t* test)
{
  aosDbgCheck((test->data != NULL) &&
              (((aos_test_vcnl4020data_t*)(test->data))->vcnld != NULL) &&
              (((aos_test_vcnl4020data_t*)(test->data))->evtsource != NULL));

  // local variables
  aos_testresult_t result;
  int32_t status;
  uint8_t reg_buf[4] = {0};
  uint8_t* reg_8 = (uint8_t*)(reg_buf);
  uint16_t* reg_16 = (uint16_t*)(reg_buf);
  event_listener_t event_listener;
  aos_timestamp_t tcurrent, tend;

  aosTestResultInit(&result);

  chprintf(stream, "reading register...\n");
  status = vcnl4020_lld_readreg(((aos_test_vcnl4020data_t*)(test->data))->vcnld, VCNL4020_LLD_REGADDR_IDREV, &reg_8[0], ((aos_test_vcnl4020data_t*)(test->data))->timeout);
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK && reg_8[0] == 0x21u) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X; 0x%08X\n", status, reg_8[0]);
  }

  chprintf(stream, "writing register...\n");
  status = vcnl4020_lld_writereg(((aos_test_vcnl4020data_t*)(test->data))->vcnld, VCNL4020_LLD_REGADDR_PROXRATE, VCNL4020_LLD_PROXRATEREG_250_HZ, ((aos_test_vcnl4020data_t*)(test->data))->timeout);
  status |= vcnl4020_lld_readreg(((aos_test_vcnl4020data_t*)(test->data))->vcnld, VCNL4020_LLD_REGADDR_PROXRATE, &reg_8[0], ((aos_test_vcnl4020data_t*)(test->data))->timeout);
  status |= (reg_8[0] == VCNL4020_LLD_PROXRATEREG_250_HZ) ? 0x0000 : 0x0100;
  status |= vcnl4020_lld_writereg(((aos_test_vcnl4020data_t*)(test->data))->vcnld, VCNL4020_LLD_REGADDR_CMD, (VCNL4020_LLD_CMDREG_ALSEN | VCNL4020_LLD_CMDREG_PROXEN | VCNL4020_LLD_CMDREG_SELFTIMED), ((aos_test_vcnl4020data_t*)(test->data))->timeout);
  status |= vcnl4020_lld_readreg(((aos_test_vcnl4020data_t*)(test->data))->vcnld, VCNL4020_LLD_REGADDR_CMD, &reg_8[0], ((aos_test_vcnl4020data_t*)(test->data))->timeout);
  status |= (reg_8[0] & (VCNL4020_LLD_CMDREG_ALSEN | VCNL4020_LLD_CMDREG_PROXEN | VCNL4020_LLD_CMDREG_SELFTIMED)) ? 0x0000 : 0x0200;
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X, 0x%02X\n", status, reg_8[0]);
  }

  chprintf(stream, "reading ambient light for ten seconds...\n");
  status = APAL_STATUS_OK;
  for (uint8_t i = 0; i < 10; ++i) {
    aosThdSSleep(1);
    status |= vcnl4020_lld_readals(((aos_test_vcnl4020data_t*)(test->data))->vcnld, &reg_16[0], ((aos_test_vcnl4020data_t*)(test->data))->timeout);
    chprintf(stream, "\t\tambient light: 0x%04X\n", reg_16[0]);
  }
  status |= (reg_16[0] == 0x0000) ? 0x10 : 0x00;
  if (status == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "reading proximity for ten seconds...\n");
  status = APAL_STATUS_OK;
  for (uint8_t i = 0; i < 10; ++i) {
    aosThdSSleep(1);
    status |= vcnl4020_lld_readprox(((aos_test_vcnl4020data_t*)(test->data))->vcnld, &reg_16[1], ((aos_test_vcnl4020data_t*)(test->data))->timeout);
    chprintf(stream, "\t\tproximity: 0x%04X\n", reg_16[1]);
  }
  status |= (reg_16[1] == 0x0000) ? 0x10 : 0x00;
  if (status == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "reading ambient light and proximity for ten seconds...\n");
  status = APAL_STATUS_OK;
  for (uint8_t i = 0; i < 10; ++i) {
    aosThdSSleep(1);
    status |= vcnl4020_lld_readalsandprox(((aos_test_vcnl4020data_t*)(test->data))->vcnld, &reg_16[0], &reg_16[1], ((aos_test_vcnl4020data_t*)(test->data))->timeout);
    chprintf(stream, "\t\tambient light: 0x%04X\tproximity: 0x%04X\n", reg_16[0], reg_16[1]);
  }
  status |= (reg_16[0] == 0x0000 || reg_16[1] == 0x0000) ? 0x10 : 0x00;
  if (status == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "reading low threshold register...\n");
  status = vcnl4020_lld_readlth(((aos_test_vcnl4020data_t*)(test->data))->vcnld, &reg_16[0], ((aos_test_vcnl4020data_t*)(test->data))->timeout);
  if (status == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "reading high threshold register...\n");
  status = vcnl4020_lld_readhth(((aos_test_vcnl4020data_t*)(test->data))->vcnld, &reg_16[1], ((aos_test_vcnl4020data_t*)(test->data))->timeout);
  if (status == APAL_STATUS_OK) {
    aosTestPassedMsg(stream, &result, "0x%04X\n", reg_16[1]);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "reading both threshold registers...\n");
  status = vcnl4020_lld_readth(((aos_test_vcnl4020data_t*)(test->data))->vcnld, &reg_16[0], &reg_16[1], ((aos_test_vcnl4020data_t*)(test->data))->timeout);
  if (status == APAL_STATUS_OK) {
    aosTestPassedMsg(stream, &result, "low: 0x%04X; high: 0x%04X\n", reg_16[0], reg_16[1]);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "writing low threshold register...\n");
  status = vcnl4020_lld_writelth(((aos_test_vcnl4020data_t*)(test->data))->vcnld, 0xFFFFu, ((aos_test_vcnl4020data_t*)(test->data))->timeout);
  status |= vcnl4020_lld_readlth(((aos_test_vcnl4020data_t*)(test->data))->vcnld, &reg_16[0], ((aos_test_vcnl4020data_t*)(test->data))->timeout);
  if (status == APAL_STATUS_OK && reg_16[0] == 0xFFFFu) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X, 0x%04X\n", status, reg_16[0]);
  }

  chprintf(stream, "writing high threshold register...\n");
  status = vcnl4020_lld_writehth(((aos_test_vcnl4020data_t*)(test->data))->vcnld, 0xFFFFu, ((aos_test_vcnl4020data_t*)(test->data))->timeout);
  status |= vcnl4020_lld_readhth(((aos_test_vcnl4020data_t*)(test->data))->vcnld, &reg_16[1], ((aos_test_vcnl4020data_t*)(test->data))->timeout);
  if (status == APAL_STATUS_OK && reg_16[1] == 0xFFFFu) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X, 0x%04X\n", status, reg_16[1]);
  }

  chprintf(stream, "writing both threshold registers...\n");
  status = vcnl4020_lld_writeth(((aos_test_vcnl4020data_t*)(test->data))->vcnld, 0x5555u, 0xAAAAu, ((aos_test_vcnl4020data_t*)(test->data))->timeout);
  status |= vcnl4020_lld_readth(((aos_test_vcnl4020data_t*)(test->data))->vcnld, &reg_16[0], &reg_16[1], ((aos_test_vcnl4020data_t*)(test->data))->timeout);
  if (status == APAL_STATUS_OK && reg_16[0] == 0x5555u && reg_16[1] == 0xAAAAu) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X, 0x%04X, 0x%04X\n", status, reg_16[0], reg_16[1]);
  }

  chprintf(stream, "test interrupts...\n");
  chprintf(stream, "(thresholds are 0x%04X and 0x%04X)\n", 0, INTERRUPT_THRESHOLD);
  chEvtRegister(((aos_test_vcnl4020data_t*)(test->data))->evtsource, &event_listener, INTERRUPT_EVENT_ID);
  aosSysGetUptime(&tend);
  aosTimestampAddInterval(tend, 30 * MICROSECONDS_PER_SECOND);
  status = vcnl4020_lld_writeth(((aos_test_vcnl4020data_t*)(test->data))->vcnld, 0x0000, INTERRUPT_THRESHOLD, ((aos_test_vcnl4020data_t*)(test->data))->timeout);
  status |= vcnl4020_lld_writereg(((aos_test_vcnl4020data_t*)(test->data))->vcnld, VCNL4020_LLD_REGADDR_INTCTRL, VCNL4020_LLD_INTCTRLREG_THRES_EN, ((aos_test_vcnl4020data_t*)(test->data))->timeout);
  do {
    // read proximity data, interrupt event information and interrupt status
    const eventmask_t emask = chEvtWaitOneTimeout(EVENT_MASK(INTERRUPT_EVENT_ID), chTimeUS2I(10*MICROSECONDS_PER_MILLISECOND));
    const eventflags_t eflags = chEvtGetAndClearFlags(&event_listener);
    status |= vcnl4020_lld_readprox(((aos_test_vcnl4020data_t*)(test->data))->vcnld, &reg_16[0], ((aos_test_vcnl4020data_t*)(test->data))->timeout);
    status |= vcnl4020_lld_readreg(((aos_test_vcnl4020data_t*)(test->data))->vcnld, VCNL4020_LLD_REGADDR_INTSTATUS, &reg_8[2], ((aos_test_vcnl4020data_t*)(test->data))->timeout);
    if (emask == EVENT_MASK(INTERRUPT_EVENT_ID) &&
        eflags == ((aos_test_vcnl4020data_t*)(test->data))->evtflags &&
        reg_8[2] == VCNL4020_LLD_INTSTATUSREG_THHIGH) {
      // true positive (correct case)
      chprintf(stream, "\t\tinterrupt detected: prox = 0x%04X\n", reg_16[0]);
      aosThdMSleep(100);
    }
    else if (emask != EVENT_MASK(INTERRUPT_EVENT_ID) &&
             eflags != ((aos_test_vcnl4020data_t*)(test->data))->evtflags &&
             reg_8[2] != VCNL4020_LLD_INTSTATUSREG_THHIGH) {
      // true negative (correct case)
      // do nothing, just keep on looping
    }
    else if (emask == EVENT_MASK(INTERRUPT_EVENT_ID) &&
             eflags == ((aos_test_vcnl4020data_t*)(test->data))->evtflags &&
             reg_8[2] != 0 ) {
      // false positive
      status |= 0x1000;
      chprintf(stream, "\t\tfalse positive: intstatus = 0x%02X; prox = 0x%04X\n", reg_8[2], reg_16[0]);
    }
    else if (emask != EVENT_MASK(INTERRUPT_EVENT_ID) &&
             eflags != ((aos_test_vcnl4020data_t*)(test->data))->evtflags &&
             reg_8[2] == 0) {
      // false negative
      status |= 0x2000;
      chprintf(stream, "\t\tfalse negative: intstatus = 0x%02X; prox = 0x%04X\n", reg_8[2], reg_16[0]);
    } // any other events are just ignored
    // reset interrupt status
    if (reg_8[2] != 0) {
      status |= vcnl4020_lld_writereg(((aos_test_vcnl4020data_t*)(test->data))->vcnld, VCNL4020_LLD_REGADDR_INTSTATUS, reg_8[2], ((aos_test_vcnl4020data_t*)(test->data))->timeout);
    }
    aosSysGetUptime(&tcurrent);
  } while (aosTimestampDiff(tcurrent, tend) > 0);
  chEvtUnregister(((aos_test_vcnl4020data_t*)(test->data))->evtsource, &event_listener);
  status |= vcnl4020_lld_writereg(((aos_test_vcnl4020data_t*)(test->data))->vcnld, VCNL4020_LLD_REGADDR_INTCTRL, 0, ((aos_test_vcnl4020data_t*)(test->data))->timeout);
  status |= vcnl4020_lld_readreg(((aos_test_vcnl4020data_t*)(test->data))->vcnld, VCNL4020_LLD_REGADDR_INTSTATUS, &reg_8[2], ((aos_test_vcnl4020data_t*)(test->data))->timeout);
  status |= vcnl4020_lld_writereg(((aos_test_vcnl4020data_t*)(test->data))->vcnld, VCNL4020_LLD_REGADDR_INTSTATUS, reg_8[2], ((aos_test_vcnl4020data_t*)(test->data))->timeout);
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  aosTestInfoMsg(stream, "driver object memory footprint: %u bytes\n", sizeof(VCNL4020Driver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
