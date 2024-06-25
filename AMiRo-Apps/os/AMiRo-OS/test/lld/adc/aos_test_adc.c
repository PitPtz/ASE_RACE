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
 * @file    aos_test_adc.c
 * @brief   ChibiOS ADC test implementation.
 *
 * @addtogroup test_adc
 * @{
 */

#include <amiroos.h>

#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)

#include "aos_test_adc.h"

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/**
 * @brief   Event mask of the ADC analog watchdog event.
 */
#define WATCHDOG_EVENT                          EVENT_MASK(31)

/**
 * @brief   ADC analog watchdog threshold.
 */
#define WATCHDOG_THRESHOLD                      9.0f

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

/**
 * @brief   ADC analog watchdog callback function.
 *
 * @param[in] adcp  ADC driver.
 * @param[in] err   ADC error value.
 */
static void _aosTestAdcAwdCallback(ADCDriver* adcp, adcerror_t err)
{
  (void)adcp;

  if (err == ADC_ERR_AWD) {
    chSysLockFromISR();
    if (adcp->listener != NULL) {
      chEvtSignalI(adcp->listener, WATCHDOG_EVENT);
      adcp->listener = NULL;
    }
    chSysUnlockFromISR();
  }

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

/**
 * @brief   ADC test function.
 *
 * @param[in] stream  Stream for input/output.
 * @param[in] test    Test object.
 *
 * @return            Test result value.
 */
aos_testresult_t aosTestAdcFunc(BaseSequentialStream* stream, const aos_test_t* test)
{
  aosDbgCheck(test->data != NULL && ((aos_test_adcdata_t*)(test->data))->driver != NULL && ((aos_test_adcdata_t*)(test->data))->convgroup != NULL);

  // local variables
  aos_testresult_t result = {0, 0};
  adcsample_t buffer[1] = {0};
  bool wdgpassed = false;
  eventmask_t eventmask = 0;
  ADCConversionGroup conversionGroup = *(((aos_test_adcdata_t*)(test->data))->convgroup);
  conversionGroup.circular = true;
  conversionGroup.end_cb = NULL;
  conversionGroup.error_cb = NULL;
  conversionGroup.htr = ADC_HTR_HT;
  conversionGroup.ltr = 0;

  chprintf(stream, "reading voltage for five seconds...\n");
  adcStartConversion(((aos_test_adcdata_t*)(test->data))->driver, &conversionGroup, buffer, 1);
  for (uint8_t s = 0; s < 5; ++s) {
    aosThdSSleep(1);
    chprintf(stream, "\tVSYS = %fV\n", moduleADC2V(buffer[0]));
  }
  adcStopConversion(((aos_test_adcdata_t*)(test->data))->driver);
  if (buffer[0] != 0) {
    aosTestPassed(stream, &result);
  } else {
    aosTestFailed(stream, &result);
  }

  chprintf(stream, "detecting external power...\n");
  conversionGroup.error_cb = _aosTestAdcAwdCallback;
  conversionGroup.htr = moduleV2ADC(WATCHDOG_THRESHOLD);
  conversionGroup.ltr = moduleV2ADC(WATCHDOG_THRESHOLD);
  ((aos_test_adcdata_t*)(test->data))->driver->listener = chThdGetSelfX();
  adcStartConversion(((aos_test_adcdata_t*)(test->data))->driver, &conversionGroup, buffer, 1);
  eventmask = chEvtWaitOneTimeout(WATCHDOG_EVENT, chTimeS2I(5));
  if (eventmask == WATCHDOG_EVENT) {
    aosTestPassedMsg(stream, &result, "%fV %c %fV\n", moduleADC2V(buffer[0]), (buffer[0] > moduleV2ADC(WATCHDOG_THRESHOLD)) ? '>' : '<', WATCHDOG_THRESHOLD);
    wdgpassed = true;
  } else {
    adcStopConversion(((aos_test_adcdata_t*)(test->data))->driver);
    aosTestFailed(stream, &result);
    wdgpassed = false;
  }

  if (wdgpassed) {
    for (uint8_t i = 0; i < 2; ++i) {
      if (buffer[0] > moduleV2ADC(WATCHDOG_THRESHOLD)) {
        chprintf(stream, "Remove external power within ten seconds.\n");
        conversionGroup.htr = ADC_HTR_HT;
        conversionGroup.ltr = moduleV2ADC(WATCHDOG_THRESHOLD);
      } else {
        chprintf(stream, "Connect external power within ten seconds.\n");
        conversionGroup.htr = moduleV2ADC(WATCHDOG_THRESHOLD);
        conversionGroup.ltr = 0;
      }
      aosThdMSleep(100); // wait some time so the ADC wil trigger again immediately due to noise
      ((aos_test_adcdata_t*)(test->data))->driver->listener = chThdGetSelfX();
      adcStartConversion(((aos_test_adcdata_t*)(test->data))->driver, &conversionGroup, buffer, 1);
      eventmask = chEvtWaitOneTimeout(WATCHDOG_EVENT, chTimeS2I(10));
      if (eventmask == WATCHDOG_EVENT) {
        aosTestPassedMsg(stream, &result, "%fV %c %fV\n", moduleADC2V(buffer[0]), (buffer[0] > moduleV2ADC(WATCHDOG_THRESHOLD)) ? '>' : '<', WATCHDOG_THRESHOLD);
      } else {
        adcStopConversion(((aos_test_adcdata_t*)(test->data))->driver);
        aosTestFailed(stream, &result);
        break;
      }
    }
  }

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == TRUE) */

/** @} */
