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
 * @file    aos_test_AT24C01B.c
 * @brief   EEPROM driver test types implementation.
 *
 * @addtogroup test_AT24C01B_v1
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "aos_test_AT24C01B.h"

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/**
 * @brief   EEPROM address for page write/read access.
 */
#define PAGE_ADDRESS                            (AT24C01B_LLD_SIZE_BYTES - 2*AT24C01B_LLD_PAGE_SIZE_BYTES)

/**
 * @brief   EEPROM address for byte write/read access.
 */
#define BYTE_ADDRESS                            (PAGE_ADDRESS + AT24C01B_LLD_PAGE_SIZE_BYTES - 1)

/**
 * @brief   Offset for overflow tests.
 */
#define OVERFLOW_OFFSET                         3

/**
 * @brief   EEPROM address for page write/read access with overflow.
 */
#define OVERFLOW_ADDRESS                        (PAGE_ADDRESS + OVERFLOW_OFFSET)

/**
 * @brief   Test data byte.
 */
#define BYTE_DATA                               0xA5

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

/**
 * @brief   AT24C01B test function.
 *
 * @param[in] stream  Stream for input/output.
 * @param[in] test    Test object.
 *
 * @return            Test result value.
 */
aos_testresult_t aosTestAt24c01bFunc(BaseSequentialStream* stream, const aos_test_t* test)
{
  aosDbgCheck(test->data != NULL && ((aos_test_at24c01bdata_t*)(test->data))->driver != NULL);

  // local variables
  aos_testresult_t result;
  int32_t status;
  uint8_t page_data[AT24C01B_LLD_PAGE_SIZE_BYTES];
  uint8_t original_data[AT24C01B_LLD_PAGE_SIZE_BYTES];
  uint8_t read_data[AT24C01B_LLD_PAGE_SIZE_BYTES];
  size_t errors = 0;

  aosTestResultInit(&result);
  for (size_t dataIdx = 0; dataIdx < AT24C01B_LLD_PAGE_SIZE_BYTES; ++dataIdx) {
    page_data[dataIdx] = dataIdx;
  }

  chprintf(stream, "reading byte...\n");
  status = at24c01b_lld_read(((aos_test_at24c01bdata_t*)(test->data))->driver, BYTE_ADDRESS, original_data, 1, ((aos_test_at24c01bdata_t*)(test->data))->timeout);
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "writing byte...\n");
  status = at24c01b_lld_write_byte(((aos_test_at24c01bdata_t*)(test->data))->driver, BYTE_ADDRESS, BYTE_DATA, ((aos_test_at24c01bdata_t*)(test->data))->timeout);
  while (at24c01b_lld_poll_ack(((aos_test_at24c01bdata_t*)(test->data))->driver, ((aos_test_at24c01bdata_t*)(test->data))->timeout) == APAL_STATUS_FAILURE) {
    continue;
  }
  status |= at24c01b_lld_read(((aos_test_at24c01bdata_t*)(test->data))->driver, BYTE_ADDRESS, &read_data[0], 1, ((aos_test_at24c01bdata_t*)(test->data))->timeout);
  status |= (read_data[0] == BYTE_DATA) ? 0x00 : 0x20;
  status |= at24c01b_lld_write_byte(((aos_test_at24c01bdata_t*)(test->data))->driver, BYTE_ADDRESS, (uint8_t)~BYTE_DATA, ((aos_test_at24c01bdata_t*)(test->data))->timeout);
  while (at24c01b_lld_poll_ack(((aos_test_at24c01bdata_t*)(test->data))->driver, ((aos_test_at24c01bdata_t*)(test->data))->timeout) == APAL_STATUS_FAILURE) {
    continue;
  }
  status |= at24c01b_lld_read(((aos_test_at24c01bdata_t*)(test->data))->driver, BYTE_ADDRESS, &read_data[1], 1, ((aos_test_at24c01bdata_t*)(test->data))->timeout);
  status |= (read_data[1] == (uint8_t)~BYTE_DATA) ? 0x00 : 0x40;
  status |= at24c01b_lld_write_byte(((aos_test_at24c01bdata_t*)(test->data))->driver, BYTE_ADDRESS, original_data[0], ((aos_test_at24c01bdata_t*)(test->data))->timeout);
  while (at24c01b_lld_poll_ack(((aos_test_at24c01bdata_t*)(test->data))->driver, ((aos_test_at24c01bdata_t*)(test->data))->timeout) == APAL_STATUS_FAILURE) {
    continue;
  }
  status |= at24c01b_lld_read(((aos_test_at24c01bdata_t*)(test->data))->driver, BYTE_ADDRESS, &read_data[2], 1, ((aos_test_at24c01bdata_t*)(test->data))->timeout);
  status |= (read_data[2] == original_data[0]) ? 0x00 : 0x80;
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "reading page...\n");
  status = at24c01b_lld_read(((aos_test_at24c01bdata_t*)(test->data))->driver, BYTE_ADDRESS, original_data, AT24C01B_LLD_PAGE_SIZE_BYTES, ((aos_test_at24c01bdata_t*)(test->data))->timeout);
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "writing page...\n");
  status = at24c01b_lld_write_page(((aos_test_at24c01bdata_t*)(test->data))->driver, BYTE_ADDRESS, page_data, AT24C01B_LLD_PAGE_SIZE_BYTES, ((aos_test_at24c01bdata_t*)(test->data))->timeout);
  while (at24c01b_lld_poll_ack(((aos_test_at24c01bdata_t*)(test->data))->driver, ((aos_test_at24c01bdata_t*)(test->data))->timeout) == APAL_STATUS_FAILURE) {
    continue;
  }
  status |= at24c01b_lld_read(((aos_test_at24c01bdata_t*)(test->data))->driver, BYTE_ADDRESS, read_data, AT24C01B_LLD_PAGE_SIZE_BYTES, ((aos_test_at24c01bdata_t*)(test->data))->timeout);
  errors = 0;
  for (size_t dataIdx = 0; dataIdx < AT24C01B_LLD_PAGE_SIZE_BYTES; dataIdx++) {
    if (read_data[dataIdx] != page_data[dataIdx]) {
      ++errors;
    }
  }
  if (status == APAL_STATUS_OK && errors == 0) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X; %u\n", status, errors);
  }

  chprintf(stream, "reading page at current address...\n");
  // set address counter to UT_ALLD_AT24C01B_PAGE_ADDRESS
  status = at24c01b_lld_read(((aos_test_at24c01bdata_t*)(test->data))->driver, BYTE_ADDRESS-1, read_data, 1, ((aos_test_at24c01bdata_t*)(test->data))->timeout);
  if (status == APAL_STATUS_IO) {
    // on STM32F1 platform the address gets incremented by 2 after reading
    status = at24c01b_lld_read(((aos_test_at24c01bdata_t*)(test->data))->driver, BYTE_ADDRESS-2, read_data, 1, ((aos_test_at24c01bdata_t*)(test->data))->timeout);
  }
  // read page from the current address
  status |= at24c01b_lld_read_current_address(((aos_test_at24c01bdata_t*)(test->data))->driver, read_data, AT24C01B_LLD_PAGE_SIZE_BYTES, ((aos_test_at24c01bdata_t*)(test->data))->timeout);
  errors = 0;
  for (uint8_t dataIdx = 0; dataIdx < 8; dataIdx++) {
    if (read_data[dataIdx] != page_data[dataIdx]) {
      ++errors;
    }
  }
  if (status == APAL_STATUS_OK && errors == 0) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X; %u\n", status, errors);
  }

  chprintf(stream, "writing page with overflow (7 bytes)...\n");
  status = at24c01b_lld_write_page(((aos_test_at24c01bdata_t*)(test->data))->driver, OVERFLOW_ADDRESS, page_data, AT24C01B_LLD_PAGE_SIZE_BYTES-1, ((aos_test_at24c01bdata_t*)(test->data))->timeout);
  while (at24c01b_lld_poll_ack(((aos_test_at24c01bdata_t*)(test->data))->driver, ((aos_test_at24c01bdata_t*)(test->data))->timeout) == APAL_STATUS_FAILURE) {
    continue;
  }
  status |= at24c01b_lld_read(((aos_test_at24c01bdata_t*)(test->data))->driver, OVERFLOW_ADDRESS, read_data, AT24C01B_LLD_PAGE_SIZE_BYTES-OVERFLOW_OFFSET, ((aos_test_at24c01bdata_t*)(test->data))->timeout);
  status |= at24c01b_lld_read(((aos_test_at24c01bdata_t*)(test->data))->driver, OVERFLOW_ADDRESS-OVERFLOW_OFFSET, read_data+(AT24C01B_LLD_PAGE_SIZE_BYTES-OVERFLOW_OFFSET), OVERFLOW_OFFSET-1, ((aos_test_at24c01bdata_t*)(test->data))->timeout);
  errors = 0;
  for (uint8_t dataIdx = 0; dataIdx < AT24C01B_LLD_PAGE_SIZE_BYTES-1; dataIdx++) {
    if (read_data[dataIdx] != page_data[dataIdx]) {
      ++errors;
    }
  }
  if (status == APAL_STATUS_OK && errors == 0) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X; %u\n", status, errors);
  }

  chprintf(stream, "writing page with overflow (8 bytes)...\n");
  status = at24c01b_lld_write_page(((aos_test_at24c01bdata_t*)(test->data))->driver, OVERFLOW_ADDRESS, page_data, AT24C01B_LLD_PAGE_SIZE_BYTES, ((aos_test_at24c01bdata_t*)(test->data))->timeout);
  while (at24c01b_lld_poll_ack(((aos_test_at24c01bdata_t*)(test->data))->driver, ((aos_test_at24c01bdata_t*)(test->data))->timeout) == APAL_STATUS_FAILURE) {
    continue;
  }
  status |= at24c01b_lld_read(((aos_test_at24c01bdata_t*)(test->data))->driver, OVERFLOW_ADDRESS, read_data, AT24C01B_LLD_PAGE_SIZE_BYTES-OVERFLOW_OFFSET, ((aos_test_at24c01bdata_t*)(test->data))->timeout);
  status |= at24c01b_lld_read(((aos_test_at24c01bdata_t*)(test->data))->driver, OVERFLOW_ADDRESS-OVERFLOW_OFFSET, read_data+(AT24C01B_LLD_PAGE_SIZE_BYTES-OVERFLOW_OFFSET), OVERFLOW_OFFSET, ((aos_test_at24c01bdata_t*)(test->data))->timeout);
  errors = 0;
  for (uint8_t dataIdx = 0; dataIdx < AT24C01B_LLD_PAGE_SIZE_BYTES; dataIdx++) {
    if (read_data[dataIdx] != page_data[dataIdx]) {
      ++errors;
    }
  }
  if (status == APAL_STATUS_OK && errors == 0) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X; %u\n", status, errors);
  }

  chprintf(stream, "write back original data...\n");
  status = at24c01b_lld_write_page(((aos_test_at24c01bdata_t*)(test->data))->driver, PAGE_ADDRESS, original_data, AT24C01B_LLD_PAGE_SIZE_BYTES, ((aos_test_at24c01bdata_t*)(test->data))->timeout);
  if (status == APAL_STATUS_SUCCESS) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  aosTestInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(AT24C01BDriver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
