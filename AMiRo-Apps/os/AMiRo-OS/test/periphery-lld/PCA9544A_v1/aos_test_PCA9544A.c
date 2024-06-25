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
 * @file    aos_test_PCA9544A.c
 * @brief   I2C multiplexer driver test types implementation.
 *
 * @addtogroup test_PCA9544A_v1
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "aos_test_PCA9544A.h"

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

aos_testresult_t aosTestPca9544aFunc(BaseSequentialStream* stream, const aos_test_t* test)
{
  aosDbgCheck(test->data != NULL && ((aos_test_pca9544adata_t*)test->data)->driver != NULL);

  // local variables
  aos_testresult_t result;
  int32_t status;
  uint8_t ctrlreg;
  pca9544a_lld_intstatus_t interrupt;
  pca9544a_lld_chid_t channel;
  uint8_t test_mask = 0x00u;

  aosTestResultInit(&result);

  chprintf(stream, "reading control register...\n");
  status = pca9544a_lld_read(((aos_test_pca9544adata_t*)test->data)->driver, &ctrlreg, ((aos_test_pca9544adata_t*)test->data)->timeout);
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "writing control register...\n");
  status = APAL_STATUS_OK;
  status |= pca9544a_lld_write(((aos_test_pca9544adata_t*)test->data)->driver, (uint8_t)(PCA9544A_LLD_CTRLREG_EN), ((aos_test_pca9544adata_t*)test->data)->timeout);
  status |= pca9544a_lld_read(((aos_test_pca9544adata_t*)test->data)->driver, &ctrlreg, ((aos_test_pca9544adata_t*)test->data)->timeout);
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK && ctrlreg == PCA9544A_LLD_CTRLREG_EN) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X, 0x%X\n", status, ctrlreg);
  }

  chprintf(stream, "reading interrupt status...\n");
  status = pca9544a_lld_getintstatus(((aos_test_pca9544adata_t*)test->data)->driver, &interrupt, ((aos_test_pca9544adata_t*)test->data)->timeout);
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassedMsg(stream, &result, "0x%08X\n", interrupt);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "reading current channel...\n");
  status = pca9544a_lld_getcurrentchannel(((aos_test_pca9544adata_t*)test->data)->driver, &channel, ((aos_test_pca9544adata_t*)test->data)->timeout);
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK) {
    aosTestPassedMsg(stream, &result, "0x%08X\n", channel);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "setting current channel...\n");
  status = APAL_STATUS_OK;
  status |= pca9544a_lld_setchannel(((aos_test_pca9544adata_t*)test->data)->driver, PCA9544A_LLD_CH0, ((aos_test_pca9544adata_t*)test->data)->timeout);
  status |= pca9544a_lld_getcurrentchannel(((aos_test_pca9544adata_t*)test->data)->driver, &channel, ((aos_test_pca9544adata_t*)test->data)->timeout);
  test_mask |= (channel != PCA9544A_LLD_CH0) ? 0x01u : 0x00u;
  status |= pca9544a_lld_setchannel(((aos_test_pca9544adata_t*)test->data)->driver, PCA9544A_LLD_CH1, ((aos_test_pca9544adata_t*)test->data)->timeout);
  status |= pca9544a_lld_getcurrentchannel(((aos_test_pca9544adata_t*)test->data)->driver, &channel, ((aos_test_pca9544adata_t*)test->data)->timeout);
  test_mask |= (channel != PCA9544A_LLD_CH1) ? 0x02u : 0x00u;
  status |= pca9544a_lld_setchannel(((aos_test_pca9544adata_t*)test->data)->driver, PCA9544A_LLD_CH2, ((aos_test_pca9544adata_t*)test->data)->timeout);
  status |= pca9544a_lld_getcurrentchannel(((aos_test_pca9544adata_t*)test->data)->driver, &channel, ((aos_test_pca9544adata_t*)test->data)->timeout);
  test_mask |= (channel != PCA9544A_LLD_CH2) ? 0x04u : 0x00u;
  status |= pca9544a_lld_setchannel(((aos_test_pca9544adata_t*)test->data)->driver, PCA9544A_LLD_CH3, ((aos_test_pca9544adata_t*)test->data)->timeout);
  status |= pca9544a_lld_getcurrentchannel(((aos_test_pca9544adata_t*)test->data)->driver, &channel, ((aos_test_pca9544adata_t*)test->data)->timeout);
  test_mask |= (channel != PCA9544A_LLD_CH3) ? 0x08u : 0x00u;
  status |= pca9544a_lld_setchannel(((aos_test_pca9544adata_t*)test->data)->driver, PCA9544A_LLD_CH_NONE, ((aos_test_pca9544adata_t*)test->data)->timeout);
  status |= pca9544a_lld_getcurrentchannel(((aos_test_pca9544adata_t*)test->data)->driver, &channel, ((aos_test_pca9544adata_t*)test->data)->timeout);
  test_mask |= (channel != PCA9544A_LLD_CH_NONE) ? 0x10u : 0x00u;
  if ((status & ~APAL_STATUS_IO) == APAL_STATUS_OK && test_mask == 0x00u) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailedMsg(stream, &result, "0x%08X, 0x%X\n", status, test_mask);
  }

  aosTestInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(PCA9544ADriver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
