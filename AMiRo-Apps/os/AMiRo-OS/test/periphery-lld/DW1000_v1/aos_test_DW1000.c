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
 * @file    aos_test_DW1000.c
 * @brief   UWB transceiver driver test types implementation.
 *
 * @addtogroup test_DW1000_v1
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "aos_test_DW1000.h"
#include <deca_instance.h>
#include <math.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

#define TEST_SNIPPETS_DW1000   // switch between test and demo apps

#define SWS1_SHF_MODE 0x02  //short frame mode (6.81M)
#define SWS1_CH5_MODE 0x04  //channel 5 mode
#define SWS1_ANC_MODE 0x08  //anchor mode
#define SWS1_A1A_MODE 0x10  //anchor/tag address A1
#define SWS1_A2A_MODE 0x20  //anchor/tag address A2
#define SWS1_A3A_MODE 0x40  //anchor/tag address A3

#define S1_SWITCH_ON  (1)
#define S1_SWITCH_OFF (0)

/******************************************************************************/
/* EXPORTED VARIABLES                                                         */
/******************************************************************************/

/******************************************************************************/
/* LOCAL TYPES                                                                */
/******************************************************************************/

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/
uint8_t s1switch = 0;
int instance_anchaddr = 0;
int dr_mode = 0;
int chan, tagaddr, ancaddr;
int instance_mode = ANCHOR;


/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/*! @brief Configure instance tag/anchor/etc... addresses */
void addressconfigure(uint8_t s1switch, uint8_t mode){
  uint16_t instAddress ;

  instance_anchaddr = (((s1switch & SWS1_A1A_MODE) << 2) + (s1switch & SWS1_A2A_MODE) + ((s1switch & SWS1_A3A_MODE) >> 2)) >> 4;

  if(mode == ANCHOR) {
    if(instance_anchaddr > 3) {
      instAddress = GATEWAY_ANCHOR_ADDR | 0x4 ; //listener
    }
    else {
      instAddress = GATEWAY_ANCHOR_ADDR | (uint16_t)instance_anchaddr;
    }
  }
  else{
    instAddress = (uint16_t)instance_anchaddr;
  }

  instancesetaddresses(instAddress);
}

/*! @brief returns the use case / operational mode */
int decarangingmode(uint8_t s1switch){
  int mode = 0;

  if(s1switch & SWS1_SHF_MODE)  {
    mode = 1;
  }

  if(s1switch & SWS1_CH5_MODE) {
    mode = mode + 2;
  }

  return mode;
}

/*! @brief Check connection setting and initialize DW1000 module */
int32_t inittestapplication(uint8_t s1switch, DW1000Driver* drv){
  uint32_t devID ;
  int result;

  setHighSpeed_SPI(FALSE, drv);          //low speed spi max. ~4M
  devID = instancereaddeviceid() ;

  if(DWT_DEVICE_ID != devID) {
    clear_SPI_chip_select();
    Sleep(1);
    set_SPI_chip_select();
    Sleep(7);
    devID = instancereaddeviceid() ;
    if(DWT_DEVICE_ID != devID){
      return(-1) ;
    }    
    dwt_softreset();
  }

    reset_DW1000();  //reset the DW1000 by driving the RSTn line low

  if((s1switch & SWS1_ANC_MODE) == 0){
    instance_mode = TAG;
  }
  else{
    instance_mode = ANCHOR;
  }

  result = instance_init(drv) ;

  if (0 > result){
    return(-1) ;
  }

  setHighSpeed_SPI(TRUE, drv);       // high speed spi max. ~ 20M
  devID = instancereaddeviceid() ;

  if (DWT_DEVICE_ID != devID){
    return(-1) ;
  }

  addressconfigure(s1switch, (uint8_t)instance_mode) ;

  if((instance_mode == ANCHOR) && (instance_anchaddr > 0x3)){
    instance_mode = LISTENER;
  }

  instancesetrole(instance_mode) ;       // Set this instance role
  dr_mode = decarangingmode(s1switch);
  chan = chConfig[dr_mode].channelNumber ;
  instance_config(&chConfig[dr_mode], &sfConfig[dr_mode], drv) ;

  return (int32_t)devID;
}

/*! @brief Main Entry point to Initialization of UWB DW1000 configuration  */
#pragma GCC optimize ("O3")
int UWB_Init(DW1000Driver* drv){

  /*! Software defined Configurartion for TAG, ANC, and other settings as needed */
  s1switch = S1_SWITCH_OFF << 1  // (on = 6.8 Mbps, off = 110 kbps)
           | S1_SWITCH_OFF << 2  // (on = CH5, off = CH2)
           | S1_SWITCH_OFF << 3  // (on = Anchor, off = TAG)
           | S1_SWITCH_OFF << 4  // (configure Tag or anchor ID no.)
           | S1_SWITCH_OFF << 5  // (configure Tag or anchor ID no.)
           | S1_SWITCH_OFF << 6  // (configure Tag or anchor ID no.)
           | S1_SWITCH_OFF << 7; // Not use in this demo


  port_DisableEXT_IRQ();           //disable ScenSor IRQ until we configure the device

  if(inittestapplication(s1switch, drv) == -1) {
    return (-1); //error
  }

  aosThdMSleep(5);

  port_EnableEXT_IRQ();  //enable DW1000 IRQ before starting

  return 0;
}


/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

aos_testresult_t aosTestDw1000Func(BaseSequentialStream* stream, const aos_test_t* test) {

  aosDbgCheck(test->data != NULL &&
      ((aos_test_dw1000data_t*)test->data)->driver != NULL &&
      ((aos_test_dw1000data_t*)test->data)->evtsource != NULL);

  // local variables
  aos_testresult_t result;

  aosTestResultInit(&result);

#if (BOARD_MIC9404x_CONNECTED == true)
  // Enable 3.3V and 1.8V supply voltages for powering up the DW1000 module in AMiRo Light Ring
  if ((test->data != NULL) && (((aos_test_dw1000data_t*)(test->data))->mic9404xd != NULL)){
//      && (((aos_test_dw1000data_t*)(test->data))->driver == NULL)){
    mic9404x_lld_state_t state;
    uint32_t status = APAL_STATUS_OK;

    chprintf(stream, "reading current status of the Power..\n");
    status = mic9404x_lld_get(((aos_test_dw1000data_t*)(test->data))->mic9404xd, &state);
    if (status == APAL_STATUS_OK) {
      aosTestPassedMsg(stream, &result, "power %s\n", (state == MIC9404x_LLD_STATE_ON) ? "enabled" : "disabled");
    } else {
      aosTestFailed(stream, &result);
    }
    if (state == MIC9404x_LLD_STATE_OFF) {
      chprintf(stream, "enabling the power ...\n");
      status = mic9404x_lld_set(((aos_test_dw1000data_t*)(test->data))->mic9404xd, MIC9404x_LLD_STATE_ON);
      status |= mic9404x_lld_get(((aos_test_dw1000data_t*)(test->data))->mic9404xd, &state);
      if (state == MIC9404x_LLD_STATE_ON) {
        aosThdSSleep(2);
        status |= mic9404x_lld_get(((aos_test_dw1000data_t*)(test->data))->mic9404xd, &state);
      }
      if ((status == APAL_STATUS_OK) && (state == MIC9404x_LLD_STATE_ON)) {
        aosTestPassed(stream, &result);
      } else {
        aosTestFailed(stream, &result);
      }
    }
    aosThdSleep(1);
    return result;
  }
#endif  /* BOARD_MIC9404x_CONNECTED == true */


  chprintf(stream, "init DW1000...\n");
  dwt_initialise(DWT_LOADUCODE, ((aos_test_dw1000data_t*)test->data)->driver);
  aosThdMSleep(5);

/*! Test snippets for DW1000.
 * @Note: Event IRQ for DW1000 should be tested separately
 */
#if defined(TEST_SNIPPETS_DW1000)

  uint32_t actual_deviceId;

  port_DisableEXT_IRQ();

  setHighSpeed_SPI(false, ((aos_test_dw1000data_t*)test->data)->driver);
  chprintf(stream, "expected device ID (LS SPI): 0xDECA0130\n");
  aosThdMSleep(5);
  actual_deviceId = instancereaddeviceid();
  chprintf(stream, "actual read ID: 0x%x\n", actual_deviceId);
  aosThdMSleep(5);

  //if the read of device ID fails, the DW1000 could be asleep
  if(DWT_DEVICE_ID != actual_deviceId){

    clear_SPI_chip_select();
    aosThdMSleep(1);
    set_SPI_chip_select();
    aosThdMSleep(7);
    actual_deviceId = instancereaddeviceid() ;

    if(DWT_DEVICE_ID != actual_deviceId){
      chprintf(stream, "SPI is not working or Unsupported Device ID\n");
      chprintf(stream, "actual device ID is: 0x%x\n", actual_deviceId);
      chprintf(stream, "expected device ID: 0xDECA0130\n");
      aosThdMSleep(5);
    }

    //clear the sleep bit - so that after the hard reset below the DW does not go into sleep
    dwt_softreset();
  }

  /*! Test1: Low speed SPI result */
  if (actual_deviceId == DWT_DEVICE_ID){
    aosTestPassed(stream, &result);
  } else {
    aosTestFailed(stream, &result);
  }

  reset_DW1000();

  chprintf(stream, "initialise instance for DW1000\n");
  aosThdSleep(1);

  int x_init = instance_init(((aos_test_dw1000data_t*)test->data)->driver) ;

  if (0 != x_init){
    chprintf(stream, "init error with return value: %d\n", x_init);
    aosThdSleep(1);
  }
  else {
    chprintf(stream, "init success with return value: %d\n", x_init);
    aosThdSleep(1);
  }

  /*! Test2: Initialization  result*/
  if (x_init == 0){
    aosTestPassed(stream, &result);
  } else {
    aosTestFailed(stream, &result);
  }

  setHighSpeed_SPI(true, ((aos_test_dw1000data_t*)test->data)->driver);

  chprintf(stream, "expected device ID (HS SPI): 0xDECA0130\n");
  actual_deviceId = instancereaddeviceid();
  chprintf(stream, "actual read ID: 0x%x\n", actual_deviceId);
  aosThdMSleep(1);

  /*! Test3: High speed SPI result*/
  if (actual_deviceId == DWT_DEVICE_ID){
    aosTestPassed(stream, &result);
  } else {
    aosTestFailed(stream, &result);
  }

  port_EnableEXT_IRQ();
  reset_DW1000();

  chprintf(stream, "initialise the configuration for UWB application\n");
  aosThdSleep(1);

  int uwb_init = UWB_Init(((aos_test_dw1000data_t*)test->data)->driver);

  if (0 != uwb_init){
    chprintf(stream, "UWB config error with return value: %d\n", uwb_init);
    aosThdSleep(1);
  }
  else {
    chprintf(stream, "UWB config success with return value: %d\n", uwb_init);
    aosThdSleep(1);
  }

  /*! Test4: UWB configuration result
   * If all the four tests are passed, the module is ready to run.
   * Note that the interrupt IRQn should be tested separately.
   */
  if (uwb_init == 0){
    aosTestPassed(stream, &result);
  } else {
    aosTestFailed(stream, &result);
  }

  /************** End of TEST_SNIPPETS_DW1000*****************/

#else /* defined(TEST_SNIPPETS_DW1000) */


  /*! RUN THE STATE MACHINE DEMO APP (RTLS) */

  chprintf(stream, "initialise the State Machine\n");
  aosThdSleep(2);

  /* Initialize UWB system with user defined configuration  */
  int uwb_init = UWB_Init(((aos_test_dw1000data_t*)test->data)->driver);

  if (0 != uwb_init){
    chprintf(stream, "error in UWB config with return value: %d\n", uwb_init);
  }
  else {
    chprintf(stream, "succeed the init of UWB config\n");
  }
  aosThdSleep(1);

  chprintf(stream, "running the RTLS demo application ...\n");
  aosThdSleep(1);

  /*! Run the localization system demo app as a thread */
  while(1){
    instance_run();
//    aosThdUSleep(100);
  }

#endif  /* defined(TEST_SNIPPETS_DW1000) */

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
