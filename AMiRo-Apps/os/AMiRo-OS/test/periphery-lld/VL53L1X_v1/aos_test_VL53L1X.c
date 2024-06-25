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
 * @file    aos_test_VL43L1X.c
 * @brief   Time-of-flight sensor driver test types implementation.
 *
 * @addtogroup test_VL43L1X_v1
 * @{
 */

#include <amiroos.h>
#include <aos_test_VL53L1X.h>


#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)
#define DATAPOINTS 1000
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
static aos_testresult_t result;
static VL53L1_Error error;
static VL53L1XDriver *dev;
static apalExitStatus_t status;
static VL53L1_RangingMeasurementData_t record[DATAPOINTS];
static aos_test_vl53l1xdata_t *data;
/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/
/**
 * @brief      Setup test parameter.
 *
 * @details    Extract driver from test struct,
 *             reset vl53l1 error and initialize the test result.
 *
 * @param      test struct containing the Driver
 */
void _setup(const aos_test_t* test){
  data = (aos_test_vl53l1xdata_t*)test->data;
  aosDbgCheck(data != NULL && data->vl53l1x != NULL);
  dev = data->vl53l1x;
  error = VL53L1_ERROR_NONE;
  status = APAL_STATUS_OK;
  vl53l1x_lld_reset(dev);
  aosTestResultInit(&result);
}

void _printTestEnd(bool success){
  if(success)
    apalDbgPrintf("===============DONE===============\n");
  else
    apalDbgPrintf("===============FAIL===============\n");
}


void _finalTestCheck(BaseSequentialStream* stream){

  if(error != VL53L1_ERROR_NONE || status != APAL_STATUS_OK)
    aosTestFailedMsg(stream, &result, "VL53L1: %i, apalState: %i", error, status);
  else
    aosTestPassed(stream, &result);
  _printTestEnd(true);
}

void _check(BaseSequentialStream* stream, int line){
  if(error != VL53L1_ERROR_NONE || status != APAL_STATUS_OK)
    aosTestFailedMsg(stream, &result, "Line: %i, VL53L1: %i, apalState: %i", line, error, status);
}


void _printSensorDataHeader(void){
  apalDbgPrintf("===============BEGIN===============\n");
  apalDbgPrintf("State,Signal,Ambient,Spad,Sigma,Range\n");
}

void _printSensorDataFooter(void){
  apalDbgPrintf("===============END===============\n");
}


void _printSensorDataRef(VL53L1_RangingMeasurementData_t *rData){
  apalDbgPrintf("%i,%f,%f,%f,%f,%i\n",
		rData->RangeStatus,
		rData->SignalRateRtnMegaCps / 65536.0,
		rData->AmbientRateRtnMegaCps / 65536.0,
		rData->EffectiveSpadRtnCount / 256.0,
		rData->SigmaMilliMeter / 65536.0,
		rData->RangeMilliMeter
		);
}

void _printImmediateMeasurementResults(void){
  apalControlGpioState_t gState;
  VL53L1_RangingMeasurementData_t rData;
  apalControlGpioGet(dev->Interface.gpio1, &gState);
  uint8_t pReady = 0;
  /* Start Measurement */
  VL53L1_StartMeasurement(dev);
  for(int i = 0; i < data->measurements; i++){
    /* Poll until interrupt occures */
    VL53L1_GetMeasurementDataReady(dev, &pReady);
    while (pReady == 0) {
      VL53L1_GetMeasurementDataReady(dev, &pReady);
    }
    /* apalDbgPrintf("Register: %d\n", i); */
    error = VL53L1_GetRangingMeasurementData(dev, &rData);
    VL53L1_ClearInterruptAndStartMeasurement(dev);
    _printSensorDataRef(&rData);
    /* Break if error occures */
    if (error != VL53L1_ERROR_NONE)
      break;
  }
  VL53L1_StopMeasurement(dev);
}

void _clearRecord(void){
  /* Set range to -1 to illustrate that the record is empty.
     This is useful to detect if the recording was interrupted by an error.
   */
  for(int i=0; i<data->measurements; i++){
    record[i].RangeMilliMeter = -1;
  }
}
/**
   Main Data Recording function.
  */
void _fillRecord(void){
  _clearRecord();
  apalControlGpioState_t gState;
  apalControlGpioGet(dev->Interface.gpio1, &gState);
//  uint8_t pReady = 0;
  /* Start Measurement */
  VL53L1_StartMeasurement(dev);
  VL53L1_ClearInterruptAndStartMeasurement(dev);
  for(int i = 0; i < data->measurements; i++){
    /* Poll until interrupt occures */
    /* VL53L1_GetMeasurementDataReady(dev, &pReady); */
    /* while (pReady == 0) { */
    /*   VL53L1_GetMeasurementDataReady(dev, &pReady); */
    /*   /\* apalDbgPrintf("Wait"); *\/ */

    /* } */
    /* apalDbgPrintf("Register: %d\n", i); */
    error = VL53L1_WaitMeasurementDataReady(dev);

    error |= VL53L1_GetRangingMeasurementData(dev, &record[i]);
    VL53L1_ClearInterruptAndStartMeasurement(dev);
    /* Break if error occures */
    if (error != VL53L1_ERROR_NONE)
      break;
  }
  VL53L1_StopMeasurement(dev);
}

void _printRecord(void){
  _printSensorDataHeader();
  for(int i = 0; i < data->measurements; i++){
    _printSensorDataRef(&record[i]);
  }
  _printSensorDataFooter();
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/*****************************************************************************
*/

/**
 * @brief   VL53L1X test function.
 *
 * @param[in] stream  Stream for input/output.
 * @param[in] test    Test object.
 *
 * @return            Test result value.
 */
aos_testresult_t aosTestVL53L1XFunc(BaseSequentialStream* stream, const aos_test_t* test)
{
  _setup(test);

  /* Setup Device */
  status = vl53l1x_lld_init(dev);
  _check(stream, __LINE__);
  /* Set Range mode */
  error = VL53L1_SetDistanceMode(dev, data->distanceMode);
  _check(stream, __LINE__);
  error = VL53L1_SetMeasurementTimingBudgetMicroSeconds(dev, data->tb*1000);
  _check(stream, __LINE__);
  error = VL53L1_SetInterMeasurementPeriodMilliSeconds(dev, data->tb +6);
  _check(stream, __LINE__);
  apalDbgPrintf("ROI: xt: %d, xb : %d, yt: %d, yb: %d\n",
		data->roiConfig.TopLeftX,
		data->roiConfig.BotRightX,
		data->roiConfig.TopLeftY,
		data->roiConfig.BotRightY);
  error = VL53L1_SetUserROI(dev, &data->roiConfig);
  _check(stream, __LINE__);
  _printSensorDataHeader();
  _printImmediateMeasurementResults();
  /* _fillRecord(); */
  /* _printRecord(); */
  _finalTestCheck(stream);
  return result;
}


aos_testresult_t aosTestVL53L1X_shortRangeModeInterrupt(BaseSequentialStream* stream, const aos_test_t* test)
{
  _setup(test);

  /* Setup Device */
  status = vl53l1x_lld_init(dev);
  _check(stream, __LINE__);
  /* Set Range mode */
  error = VL53L1_SetPresetMode (dev, VL53L1_PRESETMODE_LITE_RANGING);
  _check(stream, __LINE__);
  error = VL53L1_SetDistanceMode(dev, data->distanceMode);
  _check(stream, __LINE__);
  error = VL53L1_SetMeasurementTimingBudgetMicroSeconds(dev, data->tb*1000);
  _check(stream, __LINE__);
  /* error = VL53L1_SetInterMeasurementPeriodMilliSeconds(dev, data->tb +4); */
  _check(stream, __LINE__);
  error = VL53L1_SetUserROI(dev, &data->roiConfig);
  _check(stream, __LINE__);

  _fillRecord();
  _printRecord();

  _finalTestCheck(stream);
  return result;
}



aos_testresult_t aosTestVL53L1X_setConfig(BaseSequentialStream *stream,
                                    const aos_test_t *test) {
  _setup(test);
  apalDbgPrintf("Not Implemented yet!\n");


  _finalTestCheck(stream);
  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
