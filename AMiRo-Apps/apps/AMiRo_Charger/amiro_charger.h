/*
AMiRo-Apps is a collection of applications and configurations for the
Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2018..2022  Thomas Schöpping et al.

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
 * @file    amiro_charger.h
 *
 * @defgroup apps_amiro_charger AMiRo Charger
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_amirocharger
 * @{
 */

#ifndef AMIRO_CHARGER_H
#define AMIRO_CHARGER_H

#include <urt.h>
#include <amiroos.h>
#include "../../messagetypes/chargedata.h"

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#if !defined(AMIRO_CHARGER_STACKSIZE) || defined(__DOXYGEN__)
/**
 * @brief   Stack size of AMiRo charger thread.
 */
#define AMIRO_CHARGER_STACKSIZE                 256
#endif /* !defined(AMIRO_CHARGER_STACKSIZE) */

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

#if defined(BOARD_POWERMANAGEMENT_1_1) || defined(BOARD_POWERMANAGEMENT_1_2) || defined(__DOXYGEN__)

/**
 * @brief   Function type to convert ADC sample data to a voltage.
 */
typedef float (*amirochargerADC2V_f)(adcsample_t adc);

/**
 * @brief   List element type to add an arbitrary number of chargers to the
 *          AMiRo charger application.
 * @struct  amirocharging_charger
 */
typedef struct amiro_charger_bq241xx {
  BQ241xxDriver* bq241xx;
  struct amiro_charger_bq241xx* next;
} amiro_charger_bq241xx_t;

/**
 * @brief   Data structure only relevant for the AMiRo PowerManagement modules.
 */
typedef struct amiro_charger_powermanagementdata {
  /**
   * @brief   ADC related data.
   */
  struct {
    /**
     * @brief   Pointer to the ADC to monitor.
     */
    ADCDriver* driver;

    /**
     * @brief   Conversion group for the ADC driver.
     */
    ADCConversionGroup* cgroup;

    /**
     * @brief   ADC sample buffer.
     * @details The buffer consists of only a single element because sample
     *          history is not relevant but the latest measurement is.
     */
    adcsample_t buffer[1];

    /**
     * @brief   Threshold value for the analog watchdog.
     */
    adcsample_t threshold;

    /**
     * @brief   Conversion function to map ADC sample data to voltage.
     */
    amirochargerADC2V_f converter;
  } adc;

  /**
   * @brief   List of chargers to enable/disable on ADC events.
   */
  amiro_charger_bq241xx_t* chargers;
} amiro_charger_powermanagementdata_t;

#endif /* defined(BOARD_POWERMANAGEMENT_1_1) || defined(BOARD_POWERMANAGEMENT_1_2) */

/**
 * @brief   AMiRo charger node.
 * @struct  amiro_charger_node
 */
typedef struct amiro_charger_node {
  /**
   * @brief   Thread memory.
   */
  URT_THREAD_MEMORY(thread, AMIRO_CHARGER_STACKSIZE);

  /**
   * @brief   Node object.
   */
  urt_node_t node;

  /**
   * @brief   Timer to periodically publish battery data.
   */
  aos_timer_t timer;

  /**
   * @brief Time of the published data
   */
  urt_osTime_t publish_time;

  /**
   * @brief Battery level data type to be published
   */
  battery_data_t battery_data;
  
  /**
   * @brief   Pointer to a LED to visualize the charging state.
   * @details If set to NULL, the current charging state will not be visualized.
   */
  LEDDriver* led;

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
  /**
   * @brief   Publisher to inform about the charging state.
   */
  urt_publisher_t publisher;
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (defined(BOARD_POWERMANAGEMENT_1_1) || defined(BOARD_POWERMANAGEMENT_1_2))  \
    || defined(__DOXYGEN__)
  /**
   * @brief   Data related to charging logic of the PowerManagement modules.
   */
  amiro_charger_powermanagementdata_t* pmdata;
#endif /* defined(BOARD_POWERMANAGEMENT_1_1) || defined(BOARD_POWERMANAGEMENT_1_2) */

} amiro_charger_node_t;


/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

#if defined(BOARD_POWERMANAGEMENT_1_1) || defined(BOARD_POWERMANAGEMENT_1_2)
  void amiroChargerPmDataInit(amiro_charger_powermanagementdata_t* data, ADCDriver* adc, ADCConversionGroup* cgroup, adcsample_t threshold, amirochargerADC2V_f converter, amiro_charger_bq241xx_t* chargers);
#endif /* defined(BOARD_POWERMANAGEMENT_1_1) || defined(BOARD_POWERMANAGEMENT_1_2) */

  void amiroChargerInit(amiro_charger_node_t* acn, LEDDriver* led, urt_topic_t* topic, urt_osThreadPrio_t prio, void* data);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIRO_CHARGER_H */

/** @} */
