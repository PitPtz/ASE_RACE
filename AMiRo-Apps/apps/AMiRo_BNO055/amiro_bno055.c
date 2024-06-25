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
 * @file    amiro_bno055.c
 */

#include <amiro_bno055.h>
#include <amiroos.h>

#if defined(BOARD_DIWHEELDRIVE_1_2) || defined(__DOXYGEN__)

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/**
 * @brief   Event mask to set on an accelerometer trigger event.
 */
#define ACCO_TRIGGEREVENT                (urt_osEventMask_t)(1<<1)

/**
 * @brief   Event mask to set on a magnetometer trigger event.
 */
#define MAGNO_TRIGGEREVENT               (urt_osEventMask_t)(1<<2)

/**
 * @brief   Event mask to set on a gyroscope trigger event.
 */
#define GYRO_TRIGGEREVENT                (urt_osEventMask_t)(1<<3)

/**
 * @brief   Event mask to set on a gravity trigger event.
 */
#define GRAVITY_TRIGGEREVENT             (urt_osEventMask_t)(1<<4)

/******************************************************************************/
/* EXPORTED VARIABLES                                                         */
/******************************************************************************/

/******************************************************************************/
/* LOCAL TYPES                                                                */
/******************************************************************************/

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/

/**
 * @brief   Name of bno055 nodes.
 */
static const char _amiro_bno_name[] = "Amiro_BNO055";

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   Callback function for the trigger timer of the Gravity data.
 * @details Own callback for the gravity data because its frequency equals the accelerometer/gyrometer or magnetometer frequency.
 *
 * @param[in] params   Pointer to the timer that should be triggered.
 * @param[in] params   Parameter for the callback.
 */
static void _amiro_bnogravTriggerCb(virtual_timer_t* timer, void* params)
{
  (void)timer;
  (void)params;

  // local constants 
  amiro_bno_node_t* const bno = (amiro_bno_node_t*)params;

  // signal node thread to read gravity data
  urtEventSignal(bno->node.thread, GRAVITY_TRIGGEREVENT);

  return;
}

/**
 * @brief   Callback function for the trigger timer of the accelerometer, gyrometer and magnetometer.
 *
 * @param[in] params   Pointer to the timer that should be triggered.
 * @param[in] params   Parameter for the callback.
 */
static void _amiro_bnoTriggerCb(virtual_timer_t* timer, void* params)
{
  (void)timer;
  (void)params;

  // local constants 
  amiro_bno_node_t* const bno = (amiro_bno_node_t*)params;

  // signal node thread to read accelerometer data
  if (timer == &bno->acco.timer.vt) {
    urtEventSignal(bno->node.thread, ACCO_TRIGGEREVENT);
  }

  // signal node thread to read gyrscope data
  if (timer == &bno->gyro.timer.vt) {
    urtEventSignal(bno->node.thread, GYRO_TRIGGEREVENT);
  }

  // signal node thread to read magnetometer data
  if (timer == &bno->magno.timer.vt) {
    urtEventSignal(bno->node.thread, MAGNO_TRIGGEREVENT);
  }

  return;
}

/**
 * @brief   Setup callback function for bno055 nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] bno055  Pointer to the bno055 structure.
 *                    Must nor be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _amiro_bnoSetup(urt_node_t* node, void* bno055)
{
  urtDebugAssert(bno055 != NULL);
  (void)node;

  // local constants
  amiro_bno_node_t* const bno = (amiro_bno_node_t*)bno055;

  // set thread name
  chRegSetThreadName(_amiro_bno_name);

  // activate the Accelerometer timer
  uint8_t acco_frequency;
  if (bno->acco.frequency <= 7.81f) {
    bno->acco.frequency = 7.81f;
    acco_frequency = BNO055_ACCEL_BW_7_81HZ;
  } else if (bno->acco.frequency <= 15.63f) {
    bno->acco.frequency = 15.63f;
    acco_frequency = BNO055_ACCEL_BW_15_63HZ;
  } else if (bno->acco.frequency <= 31.25f) {
    bno->acco.frequency = 31.25f;
    acco_frequency = BNO055_ACCEL_BW_31_25HZ;
  } else if (bno->acco.frequency <= 62.5f) {
    bno->acco.frequency = 62.5f;
    acco_frequency = BNO055_ACCEL_BW_62_5HZ;
  } else if (bno->acco.frequency <= 125) {
    bno->acco.frequency = 125;
    acco_frequency = BNO055_ACCEL_BW_125HZ;
  } else if (bno->acco.frequency <= 250) {
    bno->acco.frequency = 250;
    acco_frequency = BNO055_ACCEL_BW_250HZ;
  } else if (bno->acco.frequency <= 500) {
    bno->acco.frequency = 500;
    acco_frequency = BNO055_ACCEL_BW_500HZ;
  } else {
    bno->acco.frequency = 1000;
    acco_frequency = BNO055_ACCEL_BW_1000HZ;
  }
  aosTimerPeriodicInterval(&bno->acco.timer, 1.0f / bno->acco.frequency * MICROSECONDS_PER_SECOND + 0.5f, _amiro_bnoTriggerCb, bno);

  // activate the Magnetometer timer
  uint8_t magno_frequency;
  if (bno->magno.frequency <= 2) {
    bno->magno.frequency = 2;   
    magno_frequency = BNO055_MAG_DATA_OUTRATE_2HZ; 
  } else if (bno->magno.frequency <= 6) {
    bno->magno.frequency = 6;
    magno_frequency = BNO055_MAG_DATA_OUTRATE_6HZ;
  } else if (bno->magno.frequency <= 8) {
    bno->magno.frequency = 8;
    magno_frequency = BNO055_MAG_DATA_OUTRATE_8HZ;
  } else if (bno->magno.frequency <= 10) {
    bno->magno.frequency = 10;
    magno_frequency = BNO055_MAG_DATA_OUTRATE_10HZ;
  } else if (bno->magno.frequency <= 15) {
    bno->magno.frequency = 15;
    magno_frequency = BNO055_MAG_DATA_OUTRATE_15HZ;
  } else if (bno->magno.frequency <= 20) {
    bno->magno.frequency = 20;
    magno_frequency = BNO055_MAG_DATA_OUTRATE_20HZ;
  } else if (bno->magno.frequency <= 25) {
    bno->magno.frequency = 25;
    magno_frequency = BNO055_MAG_DATA_OUTRATE_25HZ;
  } else {
    bno->magno.frequency = 30;
    magno_frequency = BNO055_MAG_DATA_OUTRATE_30HZ;
  }
  aosTimerPeriodicInterval(&bno->magno.timer, 1.0f / bno->magno.frequency * MICROSECONDS_PER_SECOND + 0.5f, _amiro_bnoTriggerCb, bno);

  // activate the Gyroscope timer
  uint8_t gyro_frequency;
  if (bno->gyro.frequency <= 12) {
    bno->gyro.frequency = 12;
    gyro_frequency = BNO055_GYRO_BW_12HZ;
  } else if (bno->gyro.frequency <= 23) {
    bno->gyro.frequency = 23;
    gyro_frequency = BNO055_GYRO_BW_23HZ;
  } else if (bno->gyro.frequency <= 32) {
    bno->gyro.frequency = 32;
    gyro_frequency = BNO055_GYRO_BW_32HZ;
  } else if (bno->gyro.frequency <= 47) {
    bno->gyro.frequency = 47;
    gyro_frequency = BNO055_GYRO_BW_47HZ;
  } else if (bno->gyro.frequency <= 64) {
    bno->gyro.frequency = 64;
    gyro_frequency = BNO055_GYRO_BW_64HZ;
  } else if (bno->gyro.frequency <= 116) {
    bno->gyro.frequency = 116;
    gyro_frequency = BNO055_GYRO_BW_116HZ;
  } else if (bno->gyro.frequency <= 230) {
    bno->gyro.frequency = 230;
    gyro_frequency = BNO055_GYRO_BW_230HZ;
  } else {
    bno->gyro.frequency = 523;
    gyro_frequency = BNO055_GYRO_BW_523HZ;   
  }
  aosTimerPeriodicInterval(&bno->gyro.timer, 1.0f / bno->gyro.frequency * MICROSECONDS_PER_SECOND + 0.5f, _amiro_bnoTriggerCb, bno);

  // Check if gravity data are calculated from the BNO
  if (bno->operation_mode == BNO055_OPERATION_MODE_NDOF) {
    // gravity is calculated out of the accelerometer, gyroscope and magnetometer data
    // there gravity timer frequency equals slowest timer frequency of accelerometer, gyroscope and magnetometer
    if (bno->gyro.frequency < bno->magno.frequency && bno->gyro.frequency < bno->acco.frequency) {
      aosTimerPeriodicInterval(&bno->gravity.timer, 1.0f / bno->gyro.frequency * MICROSECONDS_PER_SECOND + 0.5f, _amiro_bnogravTriggerCb, bno);
    } else if (bno->acco.frequency < bno->gyro.frequency && bno->acco.frequency < bno->magno.frequency) {
      aosTimerPeriodicInterval(&bno->gravity.timer, 1.0f / bno->acco.frequency * MICROSECONDS_PER_SECOND + 0.5f, _amiro_bnogravTriggerCb, bno);
    } else {
      aosTimerPeriodicInterval(&bno->gravity.timer, 1.0f / bno->magno.frequency * MICROSECONDS_PER_SECOND + 0.5f, _amiro_bnogravTriggerCb, bno);
    }
  } else if (bno->operation_mode == BNO055_OPERATION_MODE_IMUPLUS ) {
    // gravity is calculated out of the accelerometer and gyroscope data
    // there gravity timer frequency equals slowest timer frequency of accelerometer and gyroscope
    if (bno->gyro.frequency < bno->acco.frequency) {
      aosTimerPeriodicInterval(&bno->gravity.timer, 1.0f / bno->gyro.frequency * MICROSECONDS_PER_SECOND + 0.5f, _amiro_bnogravTriggerCb, bno);
    } else {
      aosTimerPeriodicInterval(&bno->gravity.timer, 1.0f / bno->acco.frequency * MICROSECONDS_PER_SECOND + 0.5f, _amiro_bnogravTriggerCb, bno);
    }
  }

  // initialize and start the BNO055
  bno055_lld_init(bno->driver, acco_frequency, gyro_frequency, magno_frequency);
  // set the operating mode of the BNO055
  bno055_lld_set_mode(bno->operation_mode);

  if (bno->operation_mode == BNO055_OPERATION_MODE_NDOF || bno->operation_mode == BNO055_OPERATION_MODE_IMUPLUS) {
    return ACCO_TRIGGEREVENT | MAGNO_TRIGGEREVENT | GYRO_TRIGGEREVENT | GRAVITY_TRIGGEREVENT;
  }
  return ACCO_TRIGGEREVENT | MAGNO_TRIGGEREVENT | GYRO_TRIGGEREVENT;
}

/**
 * @brief   Loop callback function for bno055 nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] bno055  Pointer to the bno055 structure.
 *                    Must not be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _amiro_bnoLoop(urt_node_t* node, urt_osEventMask_t event, void* bno055)
{
  urtDebugAssert(bno055 != NULL);

  (void)node;

  // local constants
  amiro_bno_node_t* const bno = (amiro_bno_node_t*)bno055;

  // if accelerometer and/or gyroscope and/or magnetometer data shall be read
  if (event & (ACCO_TRIGGEREVENT | MAGNO_TRIGGEREVENT | GYRO_TRIGGEREVENT)) {
    urt_osTime_t t;
    /* get the current time
    * Since the exact measurement time at the sensor is unknown, this is an
    * optimistic (breifly before this function call) but conservative (before
    * reading the sensors) estimation.
    */
    urtTimeNow(&t);

    if (event & ACCO_TRIGGEREVENT) {
        // Check if the linear accelerometer data are calculated from the BNO and should be read
        if (bno->operation_mode == BNO055_OPERATION_MODE_IMUPLUS || bno->operation_mode == BNO055_OPERATION_MODE_NDOF) {
          // Read the linear accelerometer data
          if (bno055_lld_read_linear_acco(bno->acco.data.data) == APAL_STATUS_OK) {
          #if (URT_CFG_PUBSUB_ENABLED == true) 
            // publish the data on the accelerometer topic
            urtPublisherPublish(&bno->acco.publisher, &bno->acco.data, sizeof(bno->acco.data), &t, URT_PUBLISHER_PUBLISH_ENFORCING);
          #endif /* URT_CFG_PUBSUB_ENABLED == true) */
          }
        } else {
          if (bno055_lld_read_converted_acco(bno->acco.data.data) == APAL_STATUS_OK) {
          #if (URT_CFG_PUBSUB_ENABLED == true) 
            // publish the data on the accelerometer topic
            urtPublisherPublish(&bno->acco.publisher, &bno->acco.data, sizeof(bno->acco.data), &t, URT_PUBLISHER_PUBLISH_ENFORCING);
          #endif /* URT_CFG_PUBSUB_ENABLED == true) */
          }
        }

        event &= ~ACCO_TRIGGEREVENT;
    }

    if (event & MAGNO_TRIGGEREVENT) {
        // read the to float converted data from the BNO055
        if (bno055_lld_read_converted_magno(bno->magno.data.data) == APAL_STATUS_OK) {
        #if (URT_CFG_PUBSUB_ENABLED == true) 
          // publish the data on the magnetometer topic
          urtPublisherPublish(&bno->magno.publisher, &bno->magno.data, sizeof(bno->magno.data), &t, URT_PUBLISHER_PUBLISH_ENFORCING);
        #endif /* URT_CFG_PUBSUB_ENABLED == true) */
        }
        event &= ~MAGNO_TRIGGEREVENT;
    }

    if (event & GYRO_TRIGGEREVENT) {
        // read the to float converted data from the BNO055
        if (bno055_lld_read_converted_gyro(bno->gyro.data.data) == APAL_STATUS_OK) {
        #if (URT_CFG_PUBSUB_ENABLED == true) 
          // Publish the data on the Gyroscope topic
          urtPublisherPublish(&bno->gyro.publisher, &bno->gyro.data, sizeof(bno->gyro.data), &t, URT_PUBLISHER_PUBLISH_ENFORCING);
        #endif /* URT_CFG_PUBSUB_ENABLED == true) */
        }
        event &= ~GYRO_TRIGGEREVENT;   
    }

    if (event & GRAVITY_TRIGGEREVENT) {
      // read the gravity data from the BNO055
      if (bno055_lld_read_gravity(bno->gravity.data.data) == APAL_STATUS_OK) {
      #if (URT_CFG_PUBSUB_ENABLED == true) 
        // Publish the data on the gravity topic
        urtPublisherPublish(&bno->gravity.publisher, &bno->gravity.data, sizeof(bno->gravity.data), &t, URT_PUBLISHER_PUBLISH_ENFORCING);
      #endif /* URT_CFG_PUBSUB_ENABLED == true) */
      }
      event &= ~GRAVITY_TRIGGEREVENT;
    }
  }

  if (bno->operation_mode == BNO055_OPERATION_MODE_NDOF || bno->operation_mode == BNO055_OPERATION_MODE_IMUPLUS) {
    return ACCO_TRIGGEREVENT | MAGNO_TRIGGEREVENT | GYRO_TRIGGEREVENT | GRAVITY_TRIGGEREVENT;
  }
  return ACCO_TRIGGEREVENT | MAGNO_TRIGGEREVENT | GYRO_TRIGGEREVENT;
}

/**
 * @brief   Shutdown callback function for bno055 nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] reason  Reason why the node stoped.
 * @param[in] bno     Pointer to the bno055 structure.
 *                    Must not be NULL.
 */
void _amiro_bnoShutdown(urt_node_t* node, urt_status_t reason, void* bno)
{
  urtDebugAssert(bno != NULL);

  (void)node;
  (void)reason;

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_amiro_bno055
 * @{
 */

/**
 * @brief   Initialization function of the AMIRO_BNO055 app.
 *
 * @param[in] bno               Pointer to the bno055 structure.
 *                              Must not be NULL.
 * @param[in] gyro_topicid      Topicid to publish gyroscope data.
 * @param[in] magno_topicid     Topicid to publish magnetometer data.
 * @param[in] acco_topicid      Topicid to publish accelerometer data.
 * @param[in] grav_topicid      Topicid to publish gravity data.
 *                              Can be 0 if operation_mode == BNO055_OPERATION_MODE_AMG.
 * @param[in] gyro_frequency    Frequency of the gyroscope sensor.
 * @param[in] magno_frequency   Frequency of the magnetometer sensor.
 * @param[in] acco_frequency    Frequency of the accelerometer sensor.
 * @param[in] operation_mode    Operation mode of the BNO055. Common and supported modes are:
 *                              BNO055_OPERATION_MODE_AMG (no fusioned data)
 *                              BNO055_OPERATION_MODE_IMUPLUS (relative orientation is calculated out of the accelerometer and gyroscope)
 *                              BNO055_OPERATION_MODE_NDOF (absolute orientation is calculated)
 * @param[in] prio              Priority of the bno055 sensing node thread.
 */
void amiroBnoInit(amiro_bno_node_t* bno,
             urt_topicid_t gyro_topicid, 
             urt_topicid_t magno_topicid, 
             urt_topicid_t acco_topicid, 
             urt_topicid_t grav_topicid, 
             float gyro_frequency,
             float magno_frequency,
             float acco_frequency,
             uint8_t operation_mode,
             urt_osThreadPrio_t prio) {
  urtDebugAssert(bno != NULL);
  urtDebugAssert(operation_mode == BNO055_OPERATION_MODE_AMG || operation_mode == BNO055_OPERATION_MODE_IMUPLUS || operation_mode == BNO055_OPERATION_MODE_NDOF);

  // initialize the timer
  aosTimerInit(&bno->acco.timer);
  aosTimerInit(&bno->gyro.timer);
  aosTimerInit(&bno->magno.timer);
  if (operation_mode == BNO055_OPERATION_MODE_NDOF || operation_mode == BNO055_OPERATION_MODE_IMUPLUS) {
    aosTimerInit(&bno->gravity.timer);
  }
#if (URT_CFG_PUBSUB_ENABLED == true)
  // initialize the publisher for the bno data
  urtPublisherInit(&bno->gyro.publisher, urtCoreGetTopic(gyro_topicid));
  urtPublisherInit(&bno->magno.publisher, urtCoreGetTopic(magno_topicid));
  urtPublisherInit(&bno->acco.publisher, urtCoreGetTopic(acco_topicid));
  if (operation_mode == BNO055_OPERATION_MODE_NDOF || operation_mode == BNO055_OPERATION_MODE_IMUPLUS) {
    urtPublisherInit(&bno->gravity.publisher, urtCoreGetTopic(grav_topicid));
  }
#endif /* URT_CFG_PUBSUB_ENABLED == true */

  // set the driver
  bno->driver = &moduleLldBno;
  // set the operation_mode
  bno->operation_mode = operation_mode;

  // set the sensor frequencies
  bno->magno.frequency = magno_frequency;
  bno->gyro.frequency = gyro_frequency;
  bno->acco.frequency = acco_frequency;

  // initialize the node
  urtNodeInit(&bno->node, (urt_osThread_t*)bno->thread, sizeof(bno->thread), prio,
              _amiro_bnoSetup, bno,
              _amiro_bnoLoop, bno,
              _amiro_bnoShutdown, bno);


  return;
}

#endif /* BOARD_DIWHEELDRIVE_1_2 */

/** @} */
