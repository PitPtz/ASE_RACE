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
 * @file    apps.c
 * @brief   AMiRoDefault configuration application container.
 */

#include "apps.h"
#include <AMiRoDefault_topics.h>
#include <AMiRoDefault_services.h>
#include <amiro_charger.h>
#if defined(BOARD_SENSORRING) && (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR)
#include <amiro_proximitysensor.h>
#endif /* defined(BOARD_SENSORRING) && (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR) */
#include <canbridge.h>
#include <math.h>
#include <stdlib.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/**
 * @brief    Threshold voltage to enable/disable battery charging.
 */
#define CHARGER_ADC_TRESHOLDVOLTAGE             9.0f

/******************************************************************************/
/* EXPORTED VARIABLES                                                         */
/******************************************************************************/

/******************************************************************************/
/* LOCAL TYPES                                                                */
/******************************************************************************/

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/

/*
 * Topics for the different data.
 */
#if (defined(BOARD_SENSORRING) && (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR)) || \
    defined(__DOXYGEN__)
static urt_topic_t _amiro_proximitysensor_proximity_topic;
static urt_topic_t _amiro_proximitysensor_ambient_topic;
#endif /* defined(BOARD_SENSORRING) && (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR) */
static urt_topic_t _ambient_amiro_floor_topic;
static urt_topic_t _proximity_amiro_floor_topic;
static urt_topic_t _battery_topic;


/*
 * Payloads of the different data.
 */
#if (defined(BOARD_SENSORRING) && (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR)) || \
    defined(__DOXYGEN__)
static proximitysensor_proximitydata_t _amiro_proximitysensor_proximity_payload;
static proximitysensor_ambientdata_t _amiro_proximitysensor_ambient_payload;
#endif /* defined(BOARD_SENSORRING) && (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR) */
static floor_proximitydata_t _proximity_amiro_floor_payload;
static floor_ambientdata_t _ambient_amiro_floor_payload;
static battery_data_t _battery_payload;

/**
 * @brief   Charger related data.
 */
struct {
  /**
   * @brief   Charger application node.
   */
  amiro_charger_node_t app;

  /**
   * @brief   PowerManagement related data for the charger node.
   */
  amiro_charger_powermanagementdata_t pmdata;

  /**
   * @brief   Set of BQ241xx list elements for the PowerManagement data.
   */
  amiro_charger_bq241xx_t bq241xx[2];
} _charger;

#if (defined(BOARD_SENSORRING) && (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR)) || \
    defined(__DOXYGEN__)
/**
 * @brief ProximitySensor node instance.
 */
static amiro_proximitysensor_node_t _amiro_proximitysensor;
#endif /* defined(BOARD_SENSORRING) && (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR) */

/******************************************************************************/
/* CAN DATA                                                                   */
/******************************************************************************/

/**
 * Event masks to set on can events.
 */
#define CAN_RING_PROXEVENT                  (urtCoreGetEventMask() << 1)
#define CAN_RING_AMBEVENT                   (urtCoreGetEventMask() << 2)
#define CAN_LIGHT_REQUESTEVENT              (urtCoreGetEventMask() << 3)
#define CAN_MOTOR_REQUESTEVENT              (urtCoreGetEventMask() << 4)
#define CAN_CHARGER_BATTERYEVENT            (urtCoreGetEventMask() << 5)

#define NUM_SUBSCRIBER  3
#define NUM_SERVICES  2

/**
 * @brief   CANBridge related data.
 */
struct {
  /**
   * @brief   CANBridge application node.
   */
  canBridge_node_t app;

  /**
   * @brief   Set of Service list elements for the PowerManagement.
   */
  canBridge_service_list_t services[NUM_SERVICES];

  /**
   * @brief   Set of Subscriber list elements for the DiWheelDrive.
   */
  canBridge_subscriber_list_t subscriber[NUM_SUBSCRIBER];
} _can;


urt_service_t can_service[NUM_SERVICES];
size_t payload_sizes[NUM_SERVICES];

urt_nrtsubscriber_t can_subscriber[NUM_SUBSCRIBER];
urt_osEventMask_t subscriber_masks[NUM_SUBSCRIBER];
size_t subscriber_payload_sizes[NUM_SUBSCRIBER];
urt_topicid_t subscriber_topic_ids[NUM_SUBSCRIBER];

#define NUM_PUBLISHER   2
urt_topicid_t pub_topic_IDs[NUM_PUBLISHER];

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)

#if (defined(BOARD_SENSORRING) && (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR)) || \
    defined(__DOXYGEN__)

/**
 * @brief   ProximitySensor offset calibration shell command callback.
 *
 * @param[in] stream  Shell I/O stream.
 * @param[in] argc    Number of arguments.
 * @param[in] argv    Argument list.
 *
 * @return  The operation result.
 */
static int _amiro_proximitysensorCalibShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return proximitysensorShellCallback_calibrate(stream, argc, argv, urtCoreGetService(SERVICEID_PROXIMITYSENSOR_CALIBRATION));
}

/**
 * @brief   Floor offset calibration shell coammand.
 */
static AOS_SHELL_COMMAND(_amiro_proximitysensorCalibShellCmd, "ProximitySensor:calibration", _amiro_proximitysensorCalibShellCmdCb);

/**
 * @brief   Print battery percentage shell command callback.
 *
 * @param[in] stream  Shell I/O stream.
 *
 * @return  The operation result.
 */

static int _amiro_batterystatusShellCmdCb(BaseSequentialStream* stream)
{
  return batteryShellCallback_getPercentage(stream, &_charger.app);
}

static AOS_SHELL_COMMAND(_amiro_batterystatusShellCmd, "Battery:status", _amiro_batterystatusShellCmdCb);


#endif /* defined(BOARD_SENSORRING) && (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR) */

#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__) */

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup configs_amirodefault_modules_powermanagement12
 * @{
 */

/**
 * @brief   Initializes all data applications for the AMiRo default configuration.
 */
void appsInit(void)
{
  // initialize common data
  appsCommonInit();

  // initialize the different topics
#if defined(BOARD_SENSORRING) && (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR)
  urtTopicInit(&_amiro_proximitysensor_proximity_topic, TOPICID_PROXIMITYSENSOR_PROXIMITY, &_amiro_proximitysensor_proximity_payload);
  urtTopicInit(&_amiro_proximitysensor_ambient_topic, TOPICID_PROXIMITYSENSOR_AMBIENT, &_amiro_proximitysensor_ambient_payload);
#endif /* defined(BOARD_SENSORRING) && (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR) */
 urtTopicInit(&_proximity_amiro_floor_topic, TOPICID_FLOOR_PROXIMITY, &_proximity_amiro_floor_payload);
 urtTopicInit(&_ambient_amiro_floor_topic, TOPICID_FLOOR_AMBIENT, &_ambient_amiro_floor_payload);
 urtTopicInit(&_battery_topic, TOPICID_BATTERY, &_battery_payload);

  // initialize AMiRo charger app
  _charger.bq241xx[0].bq241xx = &moduleLldBatteryChargerFront;
  _charger.bq241xx[0].next = &_charger.bq241xx[1];
  _charger.bq241xx[1].bq241xx = &moduleLldBatteryChargerRear;
  _charger.bq241xx[1].next = NULL;
  amiroChargerPmDataInit(&_charger.pmdata,
                         &MODULE_HAL_ADC_VSYS, &moduleHalAdcVsysConversionGroup,
                         moduleV2ADC(CHARGER_ADC_TRESHOLDVOLTAGE),
                         moduleADC2V,
                         _charger.bq241xx);
  amiroChargerInit(&_charger.app, &moduleLldStatusLed, NULL, URT_THREAD_PRIO_RT_MIN, &_charger.pmdata);

#if defined(BOARD_SENSORRING) && (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR)
  // initialize ProximitySensor app
  amiroProximitySensorInit(&_amiro_proximitysensor,
                      (VCNL4020Driver*[]){&moduleLldProximity1, &moduleLldProximity2},
                      (PCA9544ADriver*[]){&moduleLldI2cMultiplexer1, &moduleLldI2cMultiplexer2},
                      VCNL4020_LLD_PROXRATEREG_125_HZ,
                      &_amiro_proximitysensor_proximity_topic,
                      SERVICEID_PROXIMITYSENSOR_CALIBRATION,
                      (VCNL4020_LLD_ALPARAMREG_RATE_1_HZ | VCNL4020_LLD_ALPARAMREG_AUTOOFFSET_DEFAULT | VCNL4020_LLD_ALPARAMREG_AVG_64_CONV),
                      &_amiro_proximitysensor_ambient_topic,
                      URT_THREAD_PRIO_HIGH_MIN);
#endif /* defined(BOARD_SENSORRING) && (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR) */


  // initialize AMiRo CAN app
  can_service[0].id = SERVICEID_LIGHT;
  can_service[0].notification.mask = CAN_LIGHT_REQUESTEVENT;
  _can.services[0].service = &can_service[0];
  _can.services[0].payload_size = sizeof(light_led_data_t);
  _can.services[0].next = &_can.services[1];

  can_service[1].id = SERVICEID_DMC_TARGET;
  can_service[1].notification.mask = CAN_MOTOR_REQUESTEVENT;
  _can.services[1].service = &can_service[1];
  _can.services[1].payload_size = sizeof(motion_ev_csi);
  _can.services[1].next = NULL;

  subscriber_masks[0] = CAN_RING_PROXEVENT;
  subscriber_payload_sizes[0] = sizeof(proximitysensor_proximitydata_t);
  subscriber_topic_ids[0] = TOPICID_PROXIMITYSENSOR_PROXIMITY;
  _can.subscriber[0].subscriber = &can_subscriber[0];
  _can.subscriber[0].topic_id = subscriber_topic_ids[0];
  _can.subscriber[0].payload_size = subscriber_payload_sizes[0];
  _can.subscriber[0].transmit_factor = 4;
  _can.subscriber[0].mask = &subscriber_masks[0];
  _can.subscriber[0].next = &_can.subscriber[1];

  subscriber_masks[1] = CAN_RING_AMBEVENT;
  subscriber_payload_sizes[1] = sizeof(proximitysensor_ambientdata_t);
  subscriber_topic_ids[1] = TOPICID_PROXIMITYSENSOR_AMBIENT;
  _can.subscriber[1].subscriber = &can_subscriber[1];
  _can.subscriber[1].topic_id = subscriber_topic_ids[1];
  _can.subscriber[1].payload_size = subscriber_payload_sizes[1];
  _can.subscriber[1].transmit_factor = 1;
  _can.subscriber[1].mask = &subscriber_masks[1];
  _can.subscriber[1].next = &_can.subscriber[2];

  subscriber_masks[2] = CAN_CHARGER_BATTERYEVENT;
  subscriber_payload_sizes[2] = sizeof(battery_data_t);
  subscriber_topic_ids[2] = TOPICID_BATTERY;
  _can.subscriber[2].subscriber = &can_subscriber[2];
  _can.subscriber[2].topic_id = subscriber_topic_ids[2];
  _can.subscriber[2].payload_size = subscriber_payload_sizes[2];
  _can.subscriber[2].transmit_factor = 1;
  _can.subscriber[2].mask = &subscriber_masks[2];
  _can.subscriber[2].next = NULL;


  pub_topic_IDs[0] = TOPICID_FLOOR_PROXIMITY;
  pub_topic_IDs[1] = TOPICID_FLOOR_AMBIENT;

  canBridgeInit(&_can.app,
                _can.subscriber,
                pub_topic_IDs,
                (uint8_t)NUM_PUBLISHER,
                _can.services,
                NULL,
                (uint8_t)0,
                URT_THREAD_PRIO_NORMAL_MAX);

#if (AMIROOS_CFG_SHELL_ENABLE == true)
#if defined(BOARD_SENSORRING) && (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR)
  aosShellAddCommand(&_amiro_proximitysensorCalibShellCmd);
  aosShellAddCommand(&_amiro_batterystatusShellCmd);
#endif /* defined(BOARD_SENSORRING) && (BOARD_SENSORRING == BOARD_PROXIMITYSENSOR) */
#endif /* AMIROOS_CFG_SHELL_ENABLE == true */

  return;
}

/** @} */
