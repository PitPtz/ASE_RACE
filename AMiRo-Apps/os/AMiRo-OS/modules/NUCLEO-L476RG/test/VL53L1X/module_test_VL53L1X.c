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
 * @file    module_test_VL63L1X.c
 *
 * @addtogroup nucleol476rg_test
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include <module_test_VL53L1X.h>
#include <aos_test_VL53L1X.h>

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

#define FAST_RANGE_SPEED 10
#define HIGH_ACCURACCY_SPEED 200
#define LONG_RANGE_FASTES 33
#define ROI_MIN_PIXEL 0
#define ROI_MAX_PIXEL 15
static aos_test_vl53l1xdata_t _data = {
  /* pointer to VL53L1X driver  */ &moduleLldVl53l1x,
  FAST_RANGE_SPEED,
  VL53L1_DISTANCEMODE_SHORT,
  {				/* Init ROI, Choose full FOV by default */
    0,  /*!< Top Left x coordinate:  0-15 range */
    15  /*!< Top Left y coordinate:  0-15 range */,
    15  /*!< Bot Right x coordinate: 0-15 range */,
    0,  /*!< Bot Right y coordinate: 0-15 range */
  },
  100,
  0,
};

static AOS_TEST(_test, "VL53L1X Default Test", "Ranging with continuous output", moduleTestVL53L1XShellCb, aosTestVL53L1XFunc, &_data);
static AOS_TEST(_test1, "VL53L1X Short Range Mode Interrupt", "Record data and print them", moduleTestVL53L1XShellCb, aosTestVL53L1X_shortRangeModeInterrupt, &_data);
static AOS_TEST(_test2, "VL53L1X Not Implemented", "Not Implemented yet", moduleTestVL53L1XShellCb, aosTestVL53L1X_setConfig, &_data);

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

void _argparse(const char* flag, const char* val){
  if((strcmp("--range-mode", flag) == 0) || (strcmp("-d", flag) == 0)){
    /*
       Set Range mode only if valid values are given, otherwise short mode is selected
     */
    uint8_t mode = atoi(val);
    if(mode >= VL53L1_DISTANCEMODE_SHORT
       && mode <= VL53L1_DISTANCEMODE_LONG)
      _data.distanceMode = (VL53L1_DistanceModes) mode;
    else
      apalDbgPrintf("[WARNING]: Dinstancemode %u cannot be set!\n", mode);

  }else if ((strcmp("--speed", flag) == 0) || (strcmp("-s", flag) == 0)){
    int speed = atoi(val);
    if (speed >= FAST_RANGE_SPEED
    && speed <= HIGH_ACCURACCY_SPEED)
      _data.tb = speed;
    else
      apalDbgPrintf("[WARNING]: Invalid speed value %d\n", speed);
  } else if ((strcmp("--xt", flag) == 0)) {
    uint8_t pix = atoi(val);
    if(pix <= ROI_MAX_PIXEL) _data.roiConfig.TopLeftX = pix;
  } else if ((strcmp("--xb", flag) == 0)) {
    uint8_t pix = atoi(val);
    if(pix <= ROI_MAX_PIXEL) _data.roiConfig.BotRightX = pix;
  } else if ((strcmp("--yt", flag) == 0)) {
    uint8_t pix = atoi(val);
    if(pix <= ROI_MAX_PIXEL) _data.roiConfig.TopLeftY = pix;
  } else if ((strcmp("--yb", flag) == 0)) {
    uint8_t pix = atoi(val);
    if(pix <= ROI_MAX_PIXEL) _data.roiConfig.BotRightY = pix;
  } else if ((strcmp("--measurements", flag) == 0) || (strcmp("-m", flag) == 0)) {
    int datapoints = atoi(val);
    if(datapoints > 0) _data.measurements = datapoints;
  /* } else if ((strcmp("--reference-distance", flag) == 0) || (strcmp("-r", flag) == 0)) { */
    /* int dist =  */
  }else{
    apalDbgPrintf("[WARNING]: Unknown flag %s\n", flag);
  }
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup nucleol476rg_test
 * @{
 */

/**
 * @brief   VL53L1X test.
 *
 * @param[in]  stream   Stream to print output to.
 * @param[in]  argc     Number of arguments.
 * @param[in]  argv     Argument list
 * @param[out] result   Pointer to store the test result to.
 *                      May be NULL.
 *
 * @return  Status indicating the success of the function call.
 */
int moduleTestVL53L1XShellCb(BaseSequentialStream* stream, int argc, const char* argv[], aos_testresult_t* result)
{
  (void)argc;
  (void)argv;

  aos_test_t *aosTest = NULL;
  if (argc == 1){
    apalDbgPrintf("Available tests:\n");
    apalDbgPrintf("\tp (print record)\n");
    apalDbgPrintf("\td (default)\n");
    apalDbgPrintf("\trange | calib | d\n");
    apalDbgPrintf("\t\tSet distance mode:\n");
    apalDbgPrintf("\t\t--range-mode, -d <1|2|3>\n");
    apalDbgPrintf("\t\t\t1 - Short\n");
    apalDbgPrintf("\t\t\t2 - Medium\n");
    apalDbgPrintf("\t\t\t3 - Long\n");
    apalDbgPrintf("\t\tConfigures the timing budget:\n");
    apalDbgPrintf("\t\t--speed, -s <15ms-200ms>\n");
    apalDbgPrintf("\t\tSpecify region of interest, TopLeft to Bottom Right:\n");
    apalDbgPrintf("\t\t--xt <0-15>\n");
    apalDbgPrintf("\t\t\tdefault: %d\n", _data.roiConfig.TopLeftX);
    apalDbgPrintf("\t\t--xb <0-15>\n");
    apalDbgPrintf("\t\t\tdefault: %d\n", _data.roiConfig.BotRightX);
    apalDbgPrintf("\t\t--yt <0-15>\n");
    apalDbgPrintf("\t\t\tdefault: %d\n", _data.roiConfig.TopLeftY);
    apalDbgPrintf("\t\t--yb <0-15>\n");
    apalDbgPrintf("\t\t\tdefault: %d\n", _data.roiConfig.BotRightY);
    apalDbgPrintf("\t\tHow many measurements should be conducted\n");
    apalDbgPrintf("\t\t--measurements, -m <1-1000>\n");

    return AOS_OK;

  }else if(argc == 2) {
    if((strcmp("default", argv[1]) == 0) || (strcmp("d", argv[1]) == 0)){
      aosTest = &_test;
    }
  }
  else if(argc >= 4 && (argc % 2 == 0)){
    if(strcmp("range", argv[1]) == 0){
      aosTest = &_test1;
    }else if(strcmp("d", argv[1]) == 0){
      aosTest = &_test;
    }else if (strcmp("roi", argv[1]) == 0){
      aosTest = &_test2;
    }
    for (int i=2; i<argc; i+=2){
      _argparse(argv[i], argv[i+1]);
    }
  }else{
    apalDbgPrintf("[WARNING]: Cannot determine correct parameter configuration!\n");
    aosTest = NULL;
  }

  if ((result != NULL) && (aosTest != NULL)) {
    *result = aosTestRun(stream, aosTest, NULL);
  } else if (aosTest != NULL) {
    aosTestRun(stream, aosTest, NULL);
  }else{
    apalDbgPrintf("Invalid Arguments:\n");
    for(int i = 1; i < argc; i++){
      apalDbgPrintf("%s ", argv[i]);
    }
    apalDbgPrintf("\n");
  }

  return AOS_OK;
}

/** @} */

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
