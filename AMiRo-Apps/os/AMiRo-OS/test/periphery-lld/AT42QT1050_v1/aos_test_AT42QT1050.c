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
 * @file    aos_test_AT42QT1050.c
 * @brief   Touch sensor driver test types implementation.
 *
 * @addtogroup test_AT42QT1050_v1
 * @{
 */

#include <amiroos.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true)) || defined(__DOXYGEN__)

#include "aos_test_AT42QT1050.h"

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

#define INTERRUPT_EVENT_ID            1

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

void print_settings(int32_t* status, BaseSequentialStream* stream, const aos_test_t* test) {
    chprintf(stream, "settings...\n");

    uint8_t test8;
    at42qt1050_lld_register_t txbuf;
    *status |= at42qt1050_lld_read_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, AT42QT1050_LLD_REG_LOWPOWERMODE, &test8, ((aos_test_at42qt1050data_t*)test->data)->timeout);
    chprintf(stream, "\tmeasurement inverval %d ms\n", test8*8);

    *status |= at42qt1050_lld_read_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, AT42QT1050_LLD_REG_MAXONDURATION, &test8, ((aos_test_at42qt1050data_t*)test->data)->timeout);
    chprintf(stream, "\tMax on duration %d ms\n", test8*160);

    *status |= at42qt1050_lld_read_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, AT42QT1050_LLD_REG_FINFOUTMAXCALGUARD, &test8, ((aos_test_at42qt1050data_t*)test->data)->timeout);
    chprintf(stream, "\tFast In %d, Fast Out, %d MaxCal %d\n\tGuard channel ",
             test8 & AT42QT1050_LLD_FINFOUTMAXCALGUARD_FI,     //enter fast mode whenever an unfiltered signal value is detected
             test8 & AT42QT1050_LLD_FINFOUTMAXCALGUARD_FO,     //DI of 4 (global setting for all keys)
             test8 & AT42QT1050_LLD_FINFOUTMAXCALGUARD_MAXCAL);//recalibrate ALL KEYS after a Max On Duration timeout vs. individually

    //guard channel (which gets priority filtering)
    if((test8 & AT42QT1050_LLD_FINFOUTMAXCALGUARD_GUARD) > AT42QT1050_LLD_NUM_KEYS-1)
        chprintf(stream, "off");
    else
        chprintf(stream, " %d", test8 & AT42QT1050_LLD_FINFOUTMAXCALGUARD_GUARD);

    chprintf(stream, "\n\n\tkey\tgroup\tintegr.\tdelay\tthresh.\tpulse\tscale\n");
    for (uint8_t key = 0; key < AT42QT1050_LLD_NUM_KEYS; ++key) {
        chprintf(stream, "\t%d\t", key);
        txbuf = at42qt1050_lld_addr_calc(AT42QT1050_LLD_REG_INTEGRATOR_AKS_0, key);
        *status |= at42qt1050_lld_read_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, txbuf, &test8, ((aos_test_at42qt1050data_t*)test->data)->timeout);
        if(!((at42qt1050_lld_detectionintegratoraksreg_t)test8).aks)
            chprintf(stream, "none\t");
        else
            chprintf(stream, "%d\t", ((at42qt1050_lld_detectionintegratoraksreg_t)test8).aks);

        if(!((at42qt1050_lld_detectionintegratoraksreg_t)test8).detection_integrator)
            chprintf(stream, "off");
        else
            chprintf(stream, "%d", ((at42qt1050_lld_detectionintegratoraksreg_t)test8).detection_integrator);

        txbuf = at42qt1050_lld_addr_calc(AT42QT1050_LLD_REG_CHARGESHAREDELAY_0, key);
        *status |= at42qt1050_lld_read_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, txbuf, &test8, ((aos_test_at42qt1050data_t*)test->data)->timeout);
        chprintf(stream, "\t+%d", test8);

        txbuf = at42qt1050_lld_addr_calc(AT42QT1050_LLD_REG_NEGATIVETHRESHOLD_0, key);
        *status |= at42qt1050_lld_read_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, txbuf, &test8, ((aos_test_at42qt1050data_t*)test->data)->timeout);
        chprintf(stream, "\t%d", test8);

        txbuf = at42qt1050_lld_addr_calc(AT42QT1050_LLD_REG_PULSE_SCALE_0, key);
        *status |= at42qt1050_lld_read_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, txbuf, &test8, ((aos_test_at42qt1050data_t*)test->data)->timeout);
        chprintf(stream, "\t%d\t%d\n", ((at42qt1050_lld_pulsescalereg_t)test8).pulse, ((at42qt1050_lld_pulsescalereg_t)test8).scale);
    }

}

void show_live(const uint8_t first_key, int32_t* status,
               BaseSequentialStream* stream, const aos_test_t* test) {

    apalDbgAssert(first_key<5);

    event_listener_t event_listener;
    aos_timestamp_t tcurrent, tend ,tdemo_end;

    //stop demo after 15 seconds
    aosSysGetUptime(&tcurrent);
    aosTimestampSet(tdemo_end, aosTimestampGet(tcurrent) + 15*MICROSECONDS_PER_SECOND);

    uint8_t keyStatus, detectionStatus;
    uint16_t signal, reference;
    uint8_t threshold[AT42QT1050_LLD_NUM_KEYS];

    chprintf(stream, "key, count, ref, signal, [threshold], \033[31mtouch\n\033[0m");

    //get thresholds
    for (uint8_t key = 0; key < AT42QT1050_LLD_NUM_KEYS; ++key) {
        const at42qt1050_lld_register_t txbuf = at42qt1050_lld_addr_calc(AT42QT1050_LLD_REG_NEGATIVETHRESHOLD_0, key);
        *status |= at42qt1050_lld_read_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, txbuf, &threshold[key], ((aos_test_at42qt1050data_t*)test->data)->timeout);
    }
    chEvtRegister(((aos_test_at42qt1050data_t*)test->data)->evtsource, &event_listener, INTERRUPT_EVENT_ID);
    while(true) {
        aosSysGetUptime(&tcurrent);

        for (uint8_t key = first_key; key < AT42QT1050_LLD_NUM_KEYS; ++key) {

            //highlight touched key
            *status |= at42qt1050_lld_read_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, AT42QT1050_LLD_REG_KEYSTATUS, &keyStatus, ((aos_test_at42qt1050data_t*)test->data)->timeout);
            if((key == 0 && keyStatus&AT42QT1050_LLD_KEYSTATUS_KEY0)
                    || (key == 1 && keyStatus&AT42QT1050_LLD_KEYSTATUS_KEY1)
                    || (key == 2 && keyStatus&AT42QT1050_LLD_KEYSTATUS_KEY2)
                    || (key == 3 && keyStatus&AT42QT1050_LLD_KEYSTATUS_KEY3)
                    || (key == 4 && keyStatus&AT42QT1050_LLD_KEYSTATUS_KEY4))
                chprintf(stream, "\033[31m"); //red
            else
                chprintf(stream, "\033[0m"); //black

            *status |= at42qt1050_lld_read_keyssignal(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, key, &signal, ((aos_test_at42qt1050data_t*)test->data)->timeout);
            *status |= at42qt1050_lld_read_referencedata(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, key, &reference, ((aos_test_at42qt1050data_t*)test->data)->timeout);

            const uint16_t dist = (signal<reference?0:signal-reference);
            chprintf(stream, "\033[Kkey %d, %d\t0x%04X 0x%04X [", key, dist, reference, signal);
            uint16_t stars=0;
            for(; stars < dist; stars++) {
                chprintf(stream, "0");
                if(stars > 40) {           //max_stars = 40
                    chprintf(stream, "+"); //more than max_stars
                    break;
                }
            }
            for(; stars < threshold[key]; stars++)
                chprintf(stream, " ");
            if(stars == threshold[key])
                chprintf(stream, "]");
            chprintf(stream, "\n\033[0m");
        }

        //wait 1/3 second for a touch event
        aosTimestampSet(tend, aosTimestampGet(tcurrent) + (MICROSECONDS_PER_SECOND/3));
        const eventmask_t emask = chEvtWaitOneTimeout(EVENT_MASK(INTERRUPT_EVENT_ID), chTimeUS2I(aosTimestampGet(tend)));
        const eventflags_t eflags = chEvtGetAndClearFlags(&event_listener);
        if (emask == EVENT_MASK(INTERRUPT_EVENT_ID) && eflags == ((aos_test_at42qt1050data_t*)test->data)->evtflags) {
            // interrupt detected
            chprintf(stream, "interrupt ");
        } // else: timeout

        *status |= at42qt1050_lld_read_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, AT42QT1050_LLD_REG_DETECTIONSTATUS, &detectionStatus, ((aos_test_at42qt1050data_t*)test->data)->timeout);
        if(detectionStatus & AT42QT1050_LLD_DETECTIONSTATUS_TOUCH)
            chprintf(stream, "touch ");
        if(detectionStatus & AT42QT1050_LLD_DETECTIONSTATUS_OVERFLOW)
            chprintf(stream, "overflow ");
        if(detectionStatus & AT42QT1050_LLD_DETECTIONSTATUS_CALIBRATE)
            chprintf(stream, "calibrate");
        chprintf(stream, "\033[K");

        if(aosTimestampDiff(tcurrent, tdemo_end) > 0)
            break;

        chprintf(stream, "\033[%dF", 5-first_key); //cursor up
    }
    chEvtUnregister(((aos_test_at42qt1050data_t*)test->data)->evtsource, &event_listener);
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

aos_testresult_t aosTestAt42qt1050Func(BaseSequentialStream* stream, const aos_test_t* test)
{
    aosDbgCheck(test->data != NULL && ((aos_test_at42qt1050data_t*)test->data)->at42qt1050d != NULL);

    // local variables
    aos_testresult_t result;
    int32_t status;
    uint8_t test_8;
    bool error;

    aosTestResultInit(&result);

    chprintf(stream, "read register...\n");
    error = false;
    status = at42qt1050_lld_read_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, AT42QT1050_LLD_REG_CHIPID, &test_8, ((aos_test_at42qt1050data_t*)test->data)->timeout);
    chprintf(stream, "\t\tchip ID: 0x%02X\n", test_8);
    if (test_8 != AT42QT1050_LLD_CHIPID)
        error = true;
    status |= at42qt1050_lld_read_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, AT42QT1050_LLD_REG_FIRMWAREVERSION, &test_8, ((aos_test_at42qt1050data_t*)test->data)->timeout);
    chprintf(stream, "\t\tfirmware version: %u.%u (0x%02X)\n", ((at42qt1050_lld_firmwarereg_t)test_8).major, ((at42qt1050_lld_firmwarereg_t)test_8).minor, test_8);
    if (status == APAL_STATUS_SUCCESS && !error) {
        aosTestPassed(stream, &result);
    } else {
        aosTestFailedMsg(stream, &result, "0x%08X\n", status);
    }

    chprintf(stream, "write and readback threshold data...\n");
    status = APAL_STATUS_OK;
    const uint8_t threshold_test = 0x40;
    error = false;
    for (uint8_t key = 0; key < AT42QT1050_LLD_NUM_KEYS; ++key) {
        const at42qt1050_lld_register_t txbuf = at42qt1050_lld_addr_calc(AT42QT1050_LLD_REG_NEGATIVETHRESHOLD_0, key);
        status |= at42qt1050_lld_write_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, txbuf, threshold_test , ((aos_test_at42qt1050data_t*)test->data)->timeout);
        status |= at42qt1050_lld_read_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, txbuf, &test_8, ((aos_test_at42qt1050data_t*)test->data)->timeout);
        if(test_8 != threshold_test)
            error = true;
    }
    if (status == APAL_STATUS_SUCCESS  &&  !error) {
        aosTestPassedMsg(stream, &result, "Set thresholds successfull to 0x%04X\n", threshold_test);
    } else {
        aosTestFailedMsg(stream, &result, "0x%08X\n", status);
    }

    chprintf(stream, "guarding...\nincrease charge_delay0\ndisable multitouch\n");

    //channel 0 is to big to be charged in the default cycle
    status = at42qt1050_lld_write_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, AT42QT1050_LLD_REG_CHARGESHAREDELAY_0, 128 , ((aos_test_at42qt1050data_t*)test->data)->timeout);

    //set all channel to group 1 -> only 1 simultaneous touch in group = single touch
    at42qt1050_lld_detectionintegratoraksreg_t detectionintegrator;
    for (uint8_t key = 0; key < AT42QT1050_LLD_NUM_KEYS; ++key) {
        const at42qt1050_lld_register_t txbuf = at42qt1050_lld_addr_calc(AT42QT1050_LLD_REG_INTEGRATOR_AKS_0, key);
        detectionintegrator.aks = 1;                  //on touch per group-id simultaneous
        detectionintegrator.detection_integrator = 4; //4 times > threshold => touchevent
        status |= at42qt1050_lld_write_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, txbuf, detectionintegrator.raw, ((aos_test_at42qt1050data_t*)test->data)->timeout);
    }
    if (status == APAL_STATUS_SUCCESS) {
        aosTestPassed(stream, &result);
    } else {
        aosTestFailedMsg(stream, &result, "0x%08X\n", status);
    }

    chprintf(stream, "reset device...\n");
    status = at42qt1050_lld_reset(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, ((aos_test_at42qt1050data_t*)test->data)->timeout, true);
    if (status == APAL_STATUS_SUCCESS) {
        aosTestPassed(stream, &result);
    } else {
        aosTestFailedMsg(stream, &result, "0x%08X\n", status);
    }

    chprintf(stream, "read threshold data again...\n");
    status = APAL_STATUS_OK;
    const uint8_t threshold_default = 20;
    error = false;
    for (uint8_t key = 0; key < AT42QT1050_LLD_NUM_KEYS; ++key) {
        const at42qt1050_lld_register_t txbuf = at42qt1050_lld_addr_calc(AT42QT1050_LLD_REG_NEGATIVETHRESHOLD_0, key);
        status |= at42qt1050_lld_read_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, txbuf, &test_8, ((aos_test_at42qt1050data_t*)test->data)->timeout);
        if(test_8 != threshold_default)
            error = true;
    }

    if (status == APAL_STATUS_SUCCESS && !error) {
        aosTestPassedMsg(stream, &result, "threshold 0x%04X = default\n", threshold_default);
    } else {
        aosTestFailedMsg(stream, &result, "0x%08X\n", status);
    }

    chprintf(stream, "demo of default configuration:\n");
    status = APAL_STATUS_OK;
    show_live(0, &status, stream, test);
    if (status == APAL_STATUS_SUCCESS) {
        aosTestPassed(stream, &result);
    } else {
        aosTestFailedMsg(stream, &result, "0x%08X\n", status);
    }

    status = APAL_STATUS_OK;
    chprintf(stream, "write configuration...\npuls + scale + threshold 8\n");
    at42qt1050_lld_pulsescalereg_t pulse_scale;
    //values stored as exponent of 2
    pulse_scale.pulse = 0; // accumulate #pulses -> increase resolution & time to acquire
    pulse_scale.scale = 0; // scale = average factor n: NewAvg = (NewData/n) + [OldAvg*(n-1/n)] -> decrease noise
    status  = at42qt1050_lld_write_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, AT42QT1050_LLD_REG_PULSE_SCALE_0, pulse_scale.raw , ((aos_test_at42qt1050data_t*)test->data)->timeout);
    pulse_scale.pulse = 1;
    status |= at42qt1050_lld_write_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, AT42QT1050_LLD_REG_PULSE_SCALE_3, pulse_scale.raw , ((aos_test_at42qt1050data_t*)test->data)->timeout);

    pulse_scale.pulse = 4;
    status |= at42qt1050_lld_write_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, AT42QT1050_LLD_REG_PULSE_SCALE_1, pulse_scale.raw , ((aos_test_at42qt1050data_t*)test->data)->timeout);
    status |= at42qt1050_lld_write_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, AT42QT1050_LLD_REG_PULSE_SCALE_2, pulse_scale.raw , ((aos_test_at42qt1050data_t*)test->data)->timeout);

    status |= at42qt1050_lld_write_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, AT42QT1050_LLD_REG_PULSE_SCALE_4, pulse_scale.raw , ((aos_test_at42qt1050data_t*)test->data)->timeout);

    for (uint8_t key = 0; key < AT42QT1050_LLD_NUM_KEYS; ++key) {
        const at42qt1050_lld_register_t txbuf = at42qt1050_lld_addr_calc(AT42QT1050_LLD_REG_NEGATIVETHRESHOLD_0, key);
        status |= at42qt1050_lld_write_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, txbuf, 8, ((aos_test_at42qt1050data_t*)test->data)->timeout);
    }
    chprintf(stream, "disable guard key\n");
    status |= at42qt1050_lld_write_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, AT42QT1050_LLD_REG_INTEGRATOR_AKS_0, 0 , ((aos_test_at42qt1050data_t*)test->data)->timeout); //disable key
    status |= at42qt1050_lld_write_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, AT42QT1050_LLD_REG_FINFOUTMAXCALGUARD, 8 , ((aos_test_at42qt1050data_t*)test->data)->timeout); //disable feature

    chprintf(stream, "calibrate...\n");
    status = at42qt1050_lld_write_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, AT42QT1050_LLD_REG_RESET_CALIBRATE, AT42QT1050_LLD_RESETCALIBRATE_CALIBRATE, ((aos_test_at42qt1050data_t*)test->data)->timeout);



    //wait for calibration to complete
    error = true;
    for(uint8_t i=0; i<0xFF; i++) {
        aosThdUSleep(10);
        status |= at42qt1050_lld_read_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, AT42QT1050_LLD_REG_DETECTIONSTATUS, &test_8, ((aos_test_at42qt1050data_t*)test->data)->timeout);
        if(!(test_8 & AT42QT1050_LLD_DETECTIONSTATUS_CALIBRATE)) {
            error = false;
            break;
        }
    }
    if (status == APAL_STATUS_SUCCESS) {
        aosTestPassed(stream, &result);
    } else {
        aosTestFailedMsg(stream, &result, "0x%08X\n", status);
    }
    chprintf(stream, "demo of custom configuration:\n");
    status = APAL_STATUS_OK;
    show_live(1, &status, stream, test);
    if (status == APAL_STATUS_SUCCESS) {
        aosTestPassed(stream, &result);
    } else {
        aosTestFailedMsg(stream, &result, "0x%08X\n", status);
    }

    status = APAL_STATUS_OK;
    print_settings(&status, stream, test);
    if (status == APAL_STATUS_SUCCESS) {
        aosTestPassed(stream, &result);
    } else {
        aosTestFailedMsg(stream, &result, "0x%08X\n", status);
    }

    chprintf(stream, "shutdown touch\n");
    test_8 = 0; //Power down
    status = at42qt1050_lld_write_reg(((aos_test_at42qt1050data_t*)test->data)->at42qt1050d, AT42QT1050_LLD_REG_LOWPOWERMODE, test_8, ((aos_test_at42qt1050data_t*)test->data)->timeout);
    if (status == APAL_STATUS_SUCCESS) {
        aosTestPassed(stream, &result);
    } else {
        aosTestFailedMsg(stream, &result, "0x%08X\n", status);
    }

    aosTestInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(AT42QT1050Driver));

    return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_PERIPHERY == true) */

/** @} */
