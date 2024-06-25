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
 * @file    aos_test_PCAL6524.c
 * @brief   GPIO extender driver test types implementation.
 *
 * @addtogroup test_PCAL6524_v1
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "aos_test_PCAL6524.h"
#include <string.h>

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

aos_testresult_t aosTestPcal6524Func(BaseSequentialStream* stream, const aos_test_t* test)
{
  aosDbgCheck((test->data != NULL) && (((aos_test_pcal6524data_t*)test->data)->pcal6524d != NULL));

  // local variables
  aos_testresult_t result;
  int32_t status;
  uint8_t buffer[24];
  memset(buffer, 0xAA, sizeof(buffer));

  aosTestResultInit(&result);

// This test is currently not supported. See PCAL6524 driver implementation for further information.
//  chprintf(stream, "reading device ID...\n");
//  status = pcal6524_lld_read_id(((aos_test_pcal6524data_t*)test->data)->pcal6524d, buffer, ((aos_test_pcal6524data_t*)test->data)->timeout);
//  chprintf(stream, "\t\traw: 0x%02X 0x%02X 0x%02X\n", buffer[0], buffer[1], buffer[2]);
//  chprintf(stream, "\t\tname:     0x%03X\n", ((pcal6524_lld_deviceid_t*)buffer)->name);
//  chprintf(stream, "\t\tpart:     0x%03X\n", ((pcal6524_lld_deviceid_t*)buffer)->part);
//  chprintf(stream, "\t\trevision: 0x%01X\n", ((pcal6524_lld_deviceid_t*)buffer)->revision);
//  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
//    aosTestPassed(stream, &result);
//  } else {
//    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
//  }

  chprintf(stream, "reading register...\n");
  status = pcal6524_lld_read_reg(((aos_test_pcal6524data_t*)test->data)->pcal6524d, PCAL6524_LLD_CMD_SWITCHDEBOUNCECOUNT, buffer, ((aos_test_pcal6524data_t*)test->data)->timeout);
  chprintf(stream, "\t\tdebounce count: %u\n", buffer[0]);
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "writing register...\n");
  buffer[3] = 0xFF;
  status = pcal6524_lld_write_reg(((aos_test_pcal6524data_t*)test->data)->pcal6524d, PCAL6524_LLD_CMD_SWITCHDEBOUNCECOUNT, buffer[3], ((aos_test_pcal6524data_t*)test->data)->timeout);
  status |= pcal6524_lld_read_reg(((aos_test_pcal6524data_t*)test->data)->pcal6524d, PCAL6524_LLD_CMD_SWITCHDEBOUNCECOUNT, &buffer[4], ((aos_test_pcal6524data_t*)test->data)->timeout);
  status |= pcal6524_lld_write_reg(((aos_test_pcal6524data_t*)test->data)->pcal6524d, PCAL6524_LLD_CMD_SWITCHDEBOUNCECOUNT, buffer[0], ((aos_test_pcal6524data_t*)test->data)->timeout);
  status |= pcal6524_lld_read_reg(((aos_test_pcal6524data_t*)test->data)->pcal6524d, PCAL6524_LLD_CMD_SWITCHDEBOUNCECOUNT, &buffer[1], ((aos_test_pcal6524data_t*)test->data)->timeout);
  if (((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) &&
      ((buffer[1] == buffer[0]) && (buffer[4] == buffer[3]))) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "reading group...\n");
  status = pcal6524_lld_read_group(((aos_test_pcal6524data_t*)test->data)->pcal6524d, PCAL6524_LLD_CMD_OUTPUTDRIVESTRENGTH_P0A, buffer, ((aos_test_pcal6524data_t*)test->data)->timeout);
  chprintf(stream, "\t\toutput drive strength: 0x%04X 0x%04X 0x%04X\n",
           (((uint16_t)buffer[0]) << 8) | buffer[1],
           (((uint16_t)buffer[2]) << 8) | buffer[3],
           (((uint16_t)buffer[4]) << 8) | buffer[5]);
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "writing group...\n");
  memset(&buffer[6], PCAL6524_LL_OUTPUTDRIVESTRENGTH_0_25, 6);
  status = pcal6524_lld_write_group(((aos_test_pcal6524data_t*)test->data)->pcal6524d, PCAL6524_LLD_CMD_OUTPUTDRIVESTRENGTH_P0A, &buffer[6], ((aos_test_pcal6524data_t*)test->data)->timeout);
  status |= pcal6524_lld_read_group(((aos_test_pcal6524data_t*)test->data)->pcal6524d, PCAL6524_LLD_CMD_OUTPUTDRIVESTRENGTH_P0A, &buffer[12], ((aos_test_pcal6524data_t*)test->data)->timeout);
  status |= pcal6524_lld_write_group(((aos_test_pcal6524data_t*)test->data)->pcal6524d, PCAL6524_LLD_CMD_OUTPUTDRIVESTRENGTH_P0A, &buffer[0], ((aos_test_pcal6524data_t*)test->data)->timeout);
  status |= pcal6524_lld_read_group(((aos_test_pcal6524data_t*)test->data)->pcal6524d, PCAL6524_LLD_CMD_OUTPUTDRIVESTRENGTH_P0A, &buffer[18], ((aos_test_pcal6524data_t*)test->data)->timeout);
  if (((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) &&
      ((memcmp(&buffer[12], &buffer[6], 6) == 0) && (memcmp(&buffer[18], &buffer[0], 6) == 0))) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "reading continuously...\n");
  // read the following registers continously (24 bytes):
  // output 0-3 + polarity inversions 0-3 + configuration 0-3 + output drive strength 0A-2B + input latch 0-3 + pupd enable 0-3 + pupd selection 0-3
  status = pcal6524_lld_read_continuous(((aos_test_pcal6524data_t*)test->data)->pcal6524d, PCAL6524_LLD_CMD_OUTPUT_P0, buffer, 24, ((aos_test_pcal6524data_t*)test->data)->timeout);
  chprintf(stream, "\t\toutput: 0x%02X 0x%02X 0x%02X\n", buffer[0], buffer[1], buffer[2]);
  chprintf(stream, "\t\tpolarity inversion: 0x%02X 0x%02X 0x%02X\n", buffer[3], buffer[4], buffer[5]);
  chprintf(stream, "\t\tconfiguration: 0x%02X 0x%02X 0x%02X\n", buffer[6], buffer[7], buffer[8]);
  chprintf(stream, "\t\toutput drive strength: 0x%04X 0x%04X 0x%04X\n",
           (((uint16_t)buffer[9]) << 8) | buffer[10],
           (((uint16_t)buffer[11]) << 8) | buffer[12],
           (((uint16_t)buffer[13]) << 8) | buffer[14]);
  chprintf(stream, "\t\tinput latch: 0x%02X 0x%02X 0x%02X\n", buffer[15], buffer[16], buffer[17]);
  chprintf(stream, "\t\tpupd enable: 0x%02X 0x%02X 0x%02X\n", buffer[18], buffer[19], buffer[20]);
  chprintf(stream, "\t\tpupd selection: 0x%02X 0x%02X 0x%02X\n", buffer[21], buffer[22], buffer[23]);
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "writing continuously...\n");
  {
    uint8_t writebuffer[24];
    uint8_t readbuffer[2][24];
    // copy the read configuration but set the output drive strength to factor 0.25
    memcpy(writebuffer, buffer, sizeof(buffer));
    memset(&writebuffer[9], PCAL6524_LL_OUTPUTDRIVESTRENGTH_0_25, 6);
    status = pcal6524_lld_write_continuous(((aos_test_pcal6524data_t*)test->data)->pcal6524d, PCAL6524_LLD_CMD_OUTPUT_P0, writebuffer, 24, ((aos_test_pcal6524data_t*)test->data)->timeout);
    status |= pcal6524_lld_read_continuous(((aos_test_pcal6524data_t*)test->data)->pcal6524d, PCAL6524_LLD_CMD_OUTPUT_P0, readbuffer[0], 24, ((aos_test_pcal6524data_t*)test->data)->timeout);
    status |= pcal6524_lld_write_continuous(((aos_test_pcal6524data_t*)test->data)->pcal6524d, PCAL6524_LLD_CMD_OUTPUT_P0, buffer, 24, ((aos_test_pcal6524data_t*)test->data)->timeout);
    status |= pcal6524_lld_read_continuous(((aos_test_pcal6524data_t*)test->data)->pcal6524d, PCAL6524_LLD_CMD_OUTPUT_P0, readbuffer[1], 24, ((aos_test_pcal6524data_t*)test->data)->timeout);
    if (((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) &&
        ((memcmp(writebuffer, readbuffer[0], 24) == 0) && (memcmp(buffer, readbuffer[1], 24) == 0))) {
      aosTestPassed(stream, &result);
    } else {
      aosTestFailedMsg(stream, &result, "0x%08X\n", status);
    }
  }

  aosTestInfoMsg(stream, "driver object memory footprint: %u bytes\n", sizeof(PCAL6524Driver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
