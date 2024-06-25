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
#include <math.h>
#include <amiroos.h>
#include <AMiRoDefault_topics.h>
#include <AMiRoDefault_services.h>
#include <amiro_differentialmotionestimator.h>
#include <amiro_differentialmotorcontrol.h>
#include <amiro_floor.h>
#include <canbridge.h>
#include <amiro_bno055.h>
#include <lightfloordata.h>
#include <obstacleavoidance.h>
#include <odometry.h>
#include <intro_linefollowing.h>
#include <intro_wallfollowing.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

#if !defined(M_PI)
#define M_PI                                      3.14159265358979323846
#endif

/**
 * @brief   Wheel diameter in um.
 * @todo    Should be two values (left & right) stored at EEPROM, so that individual robots can have individual values.
 */
#define WHEEL_DIAMETER                            55710

/**
 * @brief   Wheel offset from the center in um.
 * @todo    Should be stored at EEPROM, so that individual robots can have individual values.
 */
#define WHEEL_OFFSET                              (34000 + (9000/2))

/**
 * @brief   Frequency of the DME in Hertz.
 */
#define DME_FREQUENCY                             100

/**
 * @brief   Driver frequency (in Hz) of the accelerometer, gyroscope and compass.
 */
#define IMU_FREQUENCY                             50.0f

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
static urt_topic_t _amiro_floor_proximity_topic;
static urt_topic_t _ring_proximity_topic;
static urt_topic_t _ring_ambient_topic;
static urt_topic_t _amiro_floor_ambient_topic;
static urt_topic_t _gyro_topic;
static urt_topic_t _acco_topic;
static urt_topic_t _magno_topic;
static urt_topic_t _gravity_topic;
static urt_topic_t _odometry_topic;
static urt_topic_t _status_topic;
//static urt_topic_t _battery_topic;

/*
 * Payloads of the different data.
 */
static floor_proximitydata_t _amiro_floor_proximity_payload;
static proximitysensor_proximitydata_t _ring_proximity_payload;
static proximitysensor_ambientdata_t _ring_ambient_payload;
static floor_ambientdata_t _amiro_floor_ambient_payload;
static gyro_converted_data_t _gyro_payload;
static acco_converted_data_t _acco_payload;
static magno_converted_data_t _magno_payload;
static gravity_data_t _gravity_payload;
static position_cv_ccsi _odometry_payload;
static status_data_t _status_payload;
//static battery_data_t _battery_payload;

/*
 * BNO055 node instance.
 */
static amiro_bno_node_t _bno;

/**
 * @brief   DME instance.make 
 */
static amiro_dme_t _amiro_dme;

/**
 * @brief   Odometry instance.
 */
static odometry_node_t _odom;

/*
 * Floor node instance.
 */
static amiro_floor_node_t _amiro_floor;


static obstacleavoidance_node_t _obstacleavoidance;
/*
 * LineFollowing node instance.
 */
// static linefollowing_node_t _linefollowing;

/*
 * LightFloorData node instance.
 */
static lightfloordata_node_t _lfd;

/*
 * Linefolowing intro node instance.
 */
static intro_linefollowing_node_t _intro_linefollowing;

/*
 * wallfolowing intro node instance.
 */
static intro_wallfollowing_node_t _intro_wallfollowing;



/**
 * @brief   DME configuration.
 */
static const amiro_dme_config_t _amiro_dme_config = {
  .left = {
    .circumference = (M_PI * WHEEL_DIAMETER) + 0.5f,
    .offset = WHEEL_OFFSET,
    .qei = {
      .driver = &MODULE_HAL_QEI_LEFT_WHEEL,
      .increments_per_revolution = MODULE_HAL_QEI_INCREMENTS_PER_REVOLUTION,
    },
  },
  .right = {
    .circumference = (M_PI * WHEEL_DIAMETER) + 0.5f,
    .offset = WHEEL_OFFSET,
    .qei = {
      .driver = &MODULE_HAL_QEI_RIGHT_WHEEL,
      .increments_per_revolution = MODULE_HAL_QEI_INCREMENTS_PER_REVOLUTION,
    },
  },
  .interval = MICROSECONDS_PER_SECOND / DME_FREQUENCY,
};

/**
 * @brief   DME motion topic.
 */
static urt_topic_t _amiro_dme_motion_topic;

/**
 * @brief   Payload for the DME motion topic mandatory message.
 */
static dme_motionpayload_t _amiro_dme_motion_topic_payload;

/**
 * @brief   DMC instance.
 */
static amiro_dmc_t _amiro_dmc;

/**
 * @brief   DMC configuration.
 */
static const amiro_dmc_config_t _amiro_dmc_config = {
  .motors = {
    .left = {
      .forward = {
        .driver = &MODULE_HAL_PWM_DRIVE,
        .channel = MODULE_HAL_PWM_DRIVE_CHANNEL_LEFT_FORWARD,
      },
      .reverse= {
        .driver = &MODULE_HAL_PWM_DRIVE,
        .channel = MODULE_HAL_PWM_DRIVE_CHANNEL_LEFT_BACKWARD,
      },
    },
    .right = {
      .forward = {
        .driver = &MODULE_HAL_PWM_DRIVE,
        .channel = MODULE_HAL_PWM_DRIVE_CHANNEL_RIGHT_FORWARD,
      },
      .reverse= {
        .driver = &MODULE_HAL_PWM_DRIVE,
        .channel = MODULE_HAL_PWM_DRIVE_CHANNEL_RIGHT_BACKWARD,
      },
    },
  },
  .lpf = {
    .factor = 10.0f,
    .max = {
      .steering = 3.14f,
      .left = 1.0f,
      .right = 1.0f,
    },
  },
};

/******************************************************************************/
/* CAN DATA                                                                   */
/******************************************************************************/

/**
 * Event masks to set on can events.
 */
#define CAN_LIGHT_REQUESTEVENT               (urtCoreGetEventMask() << 1)
#define CAN_FLOOR_PROXEVENT                  (urtCoreGetEventMask() << 2)
#define CAN_FLOOR_AMBEVENT                   (urtCoreGetEventMask() << 3)
#define CAN_GYROEVENT                        (urtCoreGetEventMask() << 4)
#define CAN_ACCOEVENT                        (urtCoreGetEventMask() << 5)
#define CAN_MAGNOEVENT                       (urtCoreGetEventMask() << 6)
#define CAN_GRAVITYEVENT                     (urtCoreGetEventMask() << 7)
#define CAN_ODOMETRYEVENT                    (urtCoreGetEventMask() << 8)
#define CAN_AMIROSTATUS_EVENT                (urtCoreGetEventMask() << 9)


#define NUM_SERVICES  1
#define NUM_SUBSCRIBER  8
/**
 * @brief   CANBridge related data.
 */
struct {
  /**
   * @brief   CANBridge application node.
   */
  canBridge_node_t app;

  /**
   * @brief   Set of Service list elements for the DiWheelDrive.
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

urt_service_t can_service[NUM_SERVICES];
size_t payload_sizes[NUM_SERVICES];

#define NUM_REQUESTS 2
urt_serviceid_t service_IDS[NUM_REQUESTS];

#define NUM_PUBLISHER   2
urt_topicid_t pub_topic_IDs[NUM_PUBLISHER];

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)

/**
 * @brief   DMC target velocity shell command callback.
 *
 * @param[in] stream  Shell I/O stream.
 * @param[in] argc    Number of command arguments.
 * @param[in] argv    Argument list.
 *
 * @return  The operation result.
 */
static int _appsDmcShellCmdCb_setVelocity(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return dmcShellCallback_setVelocity(stream, argc, argv, urtCoreGetService(SERVICEID_DMC_TARGET));
}

/**
 * @brief   DMC target velocity shell command.
 */
static AOS_SHELL_COMMAND(_appsDmcShellCmd_setVelocity, "DMC:setVelocity", _appsDmcShellCmdCb_setVelocity);

/**
 * @brief   DMC get gains shell command callback.
 *
 * @param[in] stream  Shell I/O stream.
 * @param[in] argc    Number of arguments.
 * @param[in] argv    Argument list.
 *
 * @return  The operation result.
 */
static int _appsDmcShellCmdCb_getGains(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return dmcShellCallback_getGains(stream, argc, argv, &_amiro_dmc);
}

/**
 * @brief   DMC get gains shell command.
 */
static AOS_SHELL_COMMAND(_appsDmcShellCmd_getGains, "DMC:getGains", _appsDmcShellCmdCb_getGains);

/**
 * @brief   DMC set gains shell command callback.
 *
 * @param[in] stream  Shell I/O stream.
 * @param[in] argc    Number of arguments.
 * @param[in] argv    Argument list.
 *
 * @return  The operation result.
 */
static int _appsDmcShellCmdCb_setGains(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return dmcShellCallback_setGains(stream, argc, argv, &_amiro_dmc);
}

/**
 * @brief   DMC set gains shell command.
 */
static AOS_SHELL_COMMAND(_appsDmcShellCmd_setGains, "DMC:setGains", _appsDmcShellCmdCb_setGains);

#if (DMC_CALIBRATION_ENABLE == true) || defined(__DOXYGEN__)

/**
 * @brief   DMC auto calibration shell coammand callback.
 *
 * @param[in] stream  Shell I/O stream.
 * @param[in] argc    Number of arguments.
 * @param[in] argv    Argument list.
 *
 * @return  The operation result.
 */
static int _appsDmcShellCmdCb_autoCalibration(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return dmcShellCallback_autoCalibration(stream, argc, argv, urtCoreGetService(SERVICEID_DMC_CALIBRATION));
}

/**
 * @brief   DMC auto calibration shell command.
 */
static AOS_SHELL_COMMAND(_appsDmcShellCmd_autoCalibration, "DMC:calibration", _appsDmcShellCmdCb_autoCalibration);

#endif /* (DMC_CALIBRATION_ENABLE == true) */

/**
 * @brief   Floor offset calibration shell command callback.
 *
 * @param[in] stream  Shell I/O stream.
 * @param[in] argc    Number of arguments.
 * @param[in] argv    Argument list.
 *
 * @return  The operation result.
 */
static int _amiro_floorCalibShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return floorShellCallback_calibrate(stream, argc, argv, urtCoreGetService(SERVICEID_FLOOR_CALIBRATION));
}

/**
 * @brief   Floor offset calibration shell command.
 */
static AOS_SHELL_COMMAND(_amiro_floorCalibShellCmd, "floor:calibration", _amiro_floorCalibShellCmdCb);

/**
 * @brief   LineFollowing set strategy shell command callback.
 *
 * @param[in] stream  Shell I/O stream.
 * @param[in] argc    Number of arguments.
 * @param[in] argv    Argument list.
 *
 * @return  The operation result.
 */
static int _linefollowingStrategyShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  //return linefollowing_ShellCallback_strategy(stream, argc, argv, &_linefollowing);
}

/**
 * @brief   LineFollowing set strategy shell command.
 */
static AOS_SHELL_COMMAND(_linefollowingStrategyShellCmd, "lineFollowing:setStrategy", _linefollowingStrategyShellCmdCb);

/**
 * @brief   ObstacleAvoidance set strategy shell command callback.
 *
 * @param[in] stream  Shell I/O stream.
 * @param[in] argc    Number of arguments.
 * @param[in] argv    Argument list.
 *
 * @return  The operation result.
 */
static int _obstacleavoidanceStrategyShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return obstacleavoidance_ShellCallback_strategy(stream, argc, argv, &_obstacleavoidance);
}

/**
 * @brief   ObstacleAvoidance set strategy shell command.
 */
static AOS_SHELL_COMMAND(_obstacleavoidanceStrategyShellCmd, "obstacleAvoidance:setStrategy", _obstacleavoidanceStrategyShellCmdCb);

/**
 * @brief   LightFloorData set state shell command callback.
 *
 * @param[in] stream  Shell I/O stream.
 * @param[in] argc    Number of arguments.
 * @param[in] argv    Argument list.
 *
 * @return  The operation result.
 */
static int _lightfloordataStateShellCmdCb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  return lightfloordata_ShellCallback_state(stream, argc, argv, &_lfd);
}

/**
 * @brief   LightFloorData set state shell command.
 */
static AOS_SHELL_COMMAND(_lightfloordataStateShellCmd, "lightFloorData:setState", _lightfloordataStateShellCmdCb);
#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__) */

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup configs_amirodefault_modules_diwheeldrive12
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
  urtTopicInit(&_amiro_dme_motion_topic, TOPICID_DME, &_amiro_dme_motion_topic_payload);
  urtTopicInit(&_amiro_floor_proximity_topic, TOPICID_FLOOR_PROXIMITY, &_amiro_floor_proximity_payload);
  urtTopicInit(&_amiro_floor_ambient_topic, TOPICID_FLOOR_AMBIENT, &_amiro_floor_ambient_payload);
  urtTopicInit(&_gravity_topic, TOPICID_COMPASS, &_gravity_payload);
  urtTopicInit(&_magno_topic, TOPICID_MAGNETOMETER, &_magno_payload);
  urtTopicInit(&_acco_topic, TOPICID_ACCELEROMETER, &_acco_payload);
  urtTopicInit(&_gyro_topic, TOPICID_GYROSCOPE, &_gyro_payload);
  urtTopicInit(&_ring_proximity_topic, TOPICID_PROXIMITYSENSOR_PROXIMITY, &_ring_proximity_payload);
  urtTopicInit(&_ring_ambient_topic, TOPICID_PROXIMITYSENSOR_AMBIENT, &_ring_ambient_payload);
  urtTopicInit(&_odometry_topic, TOPICID_ODOMETRY, &_odometry_payload);
  urtTopicInit(&_status_topic, TOPICID_STATUS, &_status_payload);
  //urtTopicInit(&_battery_topic, TOPICID_BATTERY, &_battery_payload);

  // initialize DME app
  amiroDmeInit(&_amiro_dme, &_amiro_dme_config, TOPICID_DME, URT_THREAD_PRIO_RT_MAX);

  // initialize Odometry app
  //odometryInit(odometry_node_t* odom, urt_osThreadPrio_t prio, urt_topicid_t motion_topicid, urt_topicid_t odom_topicid, urt_serviceid_t reset_serviceid)
  odometryInit(&_odom, URT_THREAD_PRIO_NORMAL_MAX, TOPICID_DME, TOPICID_ODOMETRY, SERVICEID_ODOMETRY);

  // initialize DMC app
#if (DMC_CALIBRATION_ENABLE == true)
  amiroDmcInit(&_amiro_dmc, &_amiro_dmc_config, TOPICID_DME, SERVICEID_DMC_TARGET, SERVICEID_DMC_CALIBRATION, URT_THREAD_PRIO_RT_MAX);
#else
  amiroDmcInit(&_amiro_dmc, &_amiro_dmc_config, TOPICID_DME, SERVICEID_DMC_TARGET, URT_THREAD_PRIO_RT_MAX);
#endif

  // initialize Floor app
  amiroFloorInit(&_amiro_floor,
                 &moduleLldProximity,
                 &moduleLldI2cMultiplexer,
                 VCNL4020_LLD_PROXRATEREG_125_HZ,
                 &_amiro_floor_proximity_topic,
                 SERVICEID_FLOOR_CALIBRATION,
                 (VCNL4020_LLD_ALPARAMREG_RATE_1_HZ | VCNL4020_LLD_ALPARAMREG_AUTOOFFSET_DEFAULT | VCNL4020_LLD_ALPARAMREG_AVG_64_CONV),
                 &_amiro_floor_ambient_topic,
                 URT_THREAD_PRIO_HIGH_MIN);

  // initialize BNO055 app
  amiroBnoInit(&_bno,
               TOPICID_GYROSCOPE,
               TOPICID_MAGNETOMETER,
               TOPICID_ACCELEROMETER,
               TOPICID_COMPASS,
               (float)12.f,
               (float)2.f,
               (float)7.81f,
               (uint8_t)BNO055_OPERATION_MODE_IMUPLUS,
               URT_THREAD_PRIO_HIGH_MIN);


  // initialize AMiRo CAN app
  service_IDS[0] = SERVICEID_DMC_TARGET;
  service_IDS[1] = SERVICEID_ODOMETRY;

  can_service[0].id = SERVICEID_LIGHT;
  can_service[0].notification.mask = CAN_LIGHT_REQUESTEVENT;
  _can.services[0].service = &can_service[0];
  _can.services[0].payload_size = sizeof(light_led_data_t);
  _can.services[0].next = NULL;

  subscriber_masks[0] = CAN_FLOOR_PROXEVENT;
  subscriber_payload_sizes[0] = sizeof(floor_proximitydata_t);
  subscriber_topic_ids[0] = TOPICID_FLOOR_PROXIMITY;
  _can.subscriber[0].subscriber = &can_subscriber[0];
  _can.subscriber[0].topic_id = subscriber_topic_ids[0];
  _can.subscriber[0].payload_size = subscriber_payload_sizes[0];
  _can.subscriber[0].transmit_factor = 4;
  _can.subscriber[0].mask = &subscriber_masks[0];
  _can.subscriber[0].next = &_can.subscriber[1];

  subscriber_masks[1] = CAN_FLOOR_AMBEVENT;
  subscriber_payload_sizes[1] = sizeof(floor_ambientdata_t);
  subscriber_topic_ids[1] = TOPICID_FLOOR_AMBIENT;
  _can.subscriber[1].subscriber = &can_subscriber[1];
  _can.subscriber[1].topic_id = subscriber_topic_ids[1];
  _can.subscriber[1].payload_size = subscriber_payload_sizes[1];
  _can.subscriber[1].transmit_factor = 1;
  _can.subscriber[1].mask = &subscriber_masks[1];
  _can.subscriber[1].next = &_can.subscriber[2];;

  subscriber_masks[2] = CAN_GYROEVENT;
  subscriber_payload_sizes[2] = sizeof(_gyro_payload);
  subscriber_topic_ids[2] = TOPICID_GYROSCOPE;
  _can.subscriber[2].subscriber = &can_subscriber[2];
  _can.subscriber[2].topic_id = subscriber_topic_ids[2];
  _can.subscriber[2].payload_size = subscriber_payload_sizes[2];
  _can.subscriber[2].transmit_factor = 2;
  _can.subscriber[2].mask = &subscriber_masks[2];
  _can.subscriber[2].next = &_can.subscriber[3];

  subscriber_masks[3] = CAN_ACCOEVENT;
  subscriber_payload_sizes[3] = sizeof(_acco_payload);
  subscriber_topic_ids[3] = TOPICID_ACCELEROMETER;
  _can.subscriber[3].subscriber = &can_subscriber[3];
  _can.subscriber[3].topic_id = subscriber_topic_ids[3];
  _can.subscriber[3].payload_size = subscriber_payload_sizes[3];
  _can.subscriber[3].transmit_factor = 1;
  _can.subscriber[3].mask = &subscriber_masks[3];
  _can.subscriber[3].next = &_can.subscriber[4];

  subscriber_masks[4] = CAN_MAGNOEVENT;
  subscriber_payload_sizes[4] = sizeof(_magno_payload);
  subscriber_topic_ids[4] = TOPICID_MAGNETOMETER;
  _can.subscriber[4].subscriber = &can_subscriber[4];
  _can.subscriber[4].topic_id = subscriber_topic_ids[4];
  _can.subscriber[4].payload_size = subscriber_payload_sizes[4];
  _can.subscriber[4].transmit_factor = 1;
  _can.subscriber[4].mask = &subscriber_masks[4];
  _can.subscriber[4].next = &_can.subscriber[5];

  subscriber_masks[5] = CAN_GRAVITYEVENT;
  subscriber_payload_sizes[5] = sizeof(_gravity_payload);
  subscriber_topic_ids[5] = TOPICID_COMPASS;
  _can.subscriber[5].subscriber = &can_subscriber[5];
  _can.subscriber[5].topic_id = subscriber_topic_ids[5];
  _can.subscriber[5].payload_size = subscriber_payload_sizes[5];
  _can.subscriber[5].transmit_factor = 1;
  _can.subscriber[5].mask = &subscriber_masks[5];
  _can.subscriber[5].next = &_can.subscriber[6];;

  subscriber_masks[6] = CAN_ODOMETRYEVENT;
  subscriber_payload_sizes[6] = sizeof(_odometry_payload);
  subscriber_topic_ids[6] = TOPICID_ODOMETRY;
  _can.subscriber[6].subscriber = &can_subscriber[6];
  _can.subscriber[6].topic_id = subscriber_topic_ids[6];
  _can.subscriber[6].payload_size = subscriber_payload_sizes[6];
  _can.subscriber[6].transmit_factor = 4;
  _can.subscriber[6].mask = &subscriber_masks[6];
  _can.subscriber[6].next = &_can.subscriber[7];

    subscriber_masks[7] = CAN_AMIROSTATUS_EVENT;
  subscriber_payload_sizes[7] = sizeof(_status_payload);
  subscriber_topic_ids[7] = TOPICID_STATUS;
  _can.subscriber[7].subscriber = &can_subscriber[7];
  _can.subscriber[7].topic_id = subscriber_topic_ids[7];
  _can.subscriber[7].payload_size = subscriber_payload_sizes[7];
  _can.subscriber[7].transmit_factor = 1;
  _can.subscriber[7].mask = &subscriber_masks[7];
  _can.subscriber[7].next = NULL;

  pub_topic_IDs[0] = TOPICID_PROXIMITYSENSOR_PROXIMITY;
  pub_topic_IDs[1] = TOPICID_PROXIMITYSENSOR_AMBIENT;
  //pub_topic_IDs[2] = TOPICID_BATTERY;

  // initialize AMiRo CAN app
  canBridgeInit(&_can.app,
               _can.subscriber,
               pub_topic_IDs,
               (uint8_t)NUM_PUBLISHER,
               _can.services,
               service_IDS,
               (uint8_t)NUM_REQUESTS,
               URT_THREAD_PRIO_HIGH_MAX);

  // initialize LineFollowing app
  /*linefollowingInit(&_linefollowing, 
                SERVICEID_DMC_TARGET,
                SERVICEID_LIGHT,
                TOPICID_FLOOR_PROXIMITY,
                TOPICID_PROXIMITYSENSOR_PROXIMITY,
                URT_THREAD_PRIO_HIGH_MIN);
*/

  intro_lf_Init(&_intro_linefollowing,
                URT_THREAD_PRIO_NORMAL_MIN,
                SERVICEID_LIGHT,
                SERVICEID_DMC_TARGET,
                TOPICID_FLOOR_PROXIMITY,
                TOPICID_PROXIMITYSENSOR_PROXIMITY);

  intro_wl_Init(&_intro_wallfollowing,
                URT_THREAD_PRIO_NORMAL_MIN,
                SERVICEID_LIGHT,
                SERVICEID_DMC_TARGET,
                TOPICID_FLOOR_PROXIMITY,
                TOPICID_PROXIMITYSENSOR_PROXIMITY,
                TOPICID_STATUS);

  obstacleavoidanceInit(&_obstacleavoidance,
                 SERVICEID_DMC_TARGET,
                 SERVICEID_LIGHT,
                 TOPICID_PROXIMITYSENSOR_PROXIMITY,
                 URT_THREAD_PRIO_HIGH_MIN);

  lightfloordataInit(&_lfd, 
                     SERVICEID_LIGHT,
                     TOPICID_FLOOR_PROXIMITY,
                     URT_THREAD_PRIO_NORMAL_MIN);

#if (AMIROOS_CFG_SHELL_ENABLE == true)
  // add DMC shell commands
  aosShellAddCommand(&_appsDmcShellCmd_setVelocity);
  aosShellAddCommand(&_appsDmcShellCmd_getGains);
  aosShellAddCommand(&_appsDmcShellCmd_setGains);
#if (DMC_CALIBRATION_ENABLE == true)
  aosShellAddCommand(&_appsDmcShellCmd_autoCalibration);
#endif /* (DMC_CALIBRATION_ENABLE == true) */
  aosShellAddCommand(&_amiro_floorCalibShellCmd);
  aosShellAddCommand(&_linefollowingStrategyShellCmd);
  aosShellAddCommand(&_obstacleavoidanceStrategyShellCmd);
  aosShellAddCommand(&_lightfloordataStateShellCmd);
#endif /* AMIROOS_CFG_SHELL_ENABLE == true */

  return;
}
/** @} */
