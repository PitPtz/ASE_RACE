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
 * @file    amiro_floor.c
 */

#include "amiro_floor.h"

#include <amiroos.h>
#include <stdlib.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/**
 * @brief   Event mask to set on a trigger event to read proximity data.
 */
#define PROXIMITY_TRIGGEREVENT        (urtCoreGetEventMask() << 1)

/**
 * @brief   Event mask to set on a trigger event to read ambient data.
 */
#define AMBIENT_TRIGGEREVENT          (urtCoreGetEventMask() << 2)

/**
 * @brief   Event mast to set an on aclibratuin request.
 */
#define PROXIMITY_SERVICEEVENT        (urtCoreGetEventMask() << 3)

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
 * @brief   Name of floor nodes.
 */
static const char _amiro_floor_name[] = "Amiro_Floor";

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   Callback function for the trigger timer.
 *
 * @param[in] flags   Flags to emit for the trigger event.
 *                    Pointer is reinterpreted as integer value.
 */
static void _amiro_floor_triggercb(virtual_timer_t* timer, void* params)
{
  // local constants
  amiro_floor_node_t* const fn = (amiro_floor_node_t*)params;

  // signal node thread to read proximity data
  if (timer == &fn->proximity.timer.vt) {
    urtEventSignal(fn->node.thread, PROXIMITY_TRIGGEREVENT);
  }

  // signal node thread to read ambient data
  if (timer == &fn->ambient.timer.vt) {
    urtEventSignal(fn->node.thread, AMBIENT_TRIGGEREVENT);
  }

  return;
}

/**
 * @brief   Mapping from sensor (according to @p floor_proximitydata_t) to I2C
 *          multiplexer channel.
 *
 * @param[in] sensor  Sensor id (according to @p floor_proximitydata_t).
 *
 * @return  Corresponding I2C Multiplexer channel.
 */
inline pca9544a_lld_chid_t _amiro_floor_Sensor2Channel(const size_t sensor)
{
  pca9544a_lld_chid_t channel;

  switch (sensor) {
    case 0:
      // left wheel
      channel = PCA9544A_LLD_CH2;
      break;
    case 1:
      // left front
      channel = PCA9544A_LLD_CH3;
      break;
    case 2:
      // right front
      channel = PCA9544A_LLD_CH0;
      break;
    case 3:
      // right wheel
      channel = PCA9544A_LLD_CH1;
      break;
    default:
      channel = PCA9544A_LLD_CH_NONE;
      break;
  }

  return channel;
}

/**
 * @brief   Subtract offset from a proximity reading but expand the value to
 *          full 16 bit range.
 *
 * @param[in] value     Value to apply the offset to.
 * @param[in] offset    Offset to apply.
 *
 * @return  Proximity value without offset but scaled.
 */
inline floor_proximity_t _amiro_floor_ProxApplyCalibration(const floor_proximity_t value, const floor_proximity_t offset)
{
  const float scalefactor = (float)((floor_proximity_t)~0) / (float)(((floor_proximity_t)~0) - offset);
  return (value - offset) * scalefactor;
}

/**
 * @brief   Setup callback function for floor nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] floor    Pointer to the floor structure.
 *                    Must nor be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _amiro_floor_Setup(urt_node_t* node, void* floor)
{
  urtDebugAssert(floor != NULL);
  (void)node;

  // local constants
  amiro_floor_node_t* const fn = (amiro_floor_node_t*)floor;

  // local variables
  float frequency;

  // set thread name
  chRegSetThreadName(_amiro_floor_name);

#if (URT_CFG_RPC_ENABLED == true)
  // initialize the calibration service
  urtServiceInit(&fn->proximity.calibration.service, fn->proximity.calibration.service.id, urtThreadGetSelf(), PROXIMITY_SERVICEEVENT);
#endif /* URT_CFG_RPC_ENABLED == true */

  // configure and start every sensor
  for (size_t sensor = 0; sensor < FLOOR_NUM_SENSORS; ++sensor) {
    // select VCNL
    pca9544a_lld_setchannel(fn->drivers.mux, _amiro_floor_Sensor2Channel(sensor), TIME_INFINITE);
    // configure sensor
    vcnl4020_lld_writereg(fn->drivers.vcnl, VCNL4020_LLD_REGADDR_PROXRATE, fn->drivers.proximity_frequency, TIME_INFINITE);
    vcnl4020_lld_writereg(fn->drivers.vcnl, VCNL4020_LLD_REGADDR_LEDCURRENT, VCNL4020_LLD_LEDCURRENTREG_200_mA, TIME_INFINITE);
    vcnl4020_lld_writereg(fn->drivers.vcnl, VCNL4020_LLD_REGADDR_ALPARAM, fn->drivers.ambient_params, TIME_INFINITE);
    // start sensor
    vcnl4020_lld_writereg(fn->drivers.vcnl, VCNL4020_LLD_REGADDR_CMD, (VCNL4020_LLD_CMDREG_ALSEN | VCNL4020_LLD_CMDREG_PROXEN | VCNL4020_LLD_CMDREG_SELFTIMED), TIME_INFINITE);
  }

  // activate the proximity timer
  switch (fn->drivers.proximity_frequency) {
    case VCNL4020_LLD_PROXRATEREG_1_95_HZ:
      frequency = 1.95f;
      break;
    case VCNL4020_LLD_PROXRATEREG_3_90625_HZ:
      frequency = 3.90625f;
      break;
    case VCNL4020_LLD_PROXRATEREG_7_8125_HZ:
      frequency = 7.8125f;
      break;
    case VCNL4020_LLD_PROXRATEREG_15_625_HZ:
      frequency = 15.625f;
      break;
    case VCNL4020_LLD_PROXRATEREG_31_25_HZ:
      frequency = 31.25f;
      break;
    case VCNL4020_LLD_PROXRATEREG_62_5_HZ:
      frequency = 62.5f;
      break;
    case VCNL4020_LLD_PROXRATEREG_125_HZ:
      frequency = 125.0f;
      break;
    case VCNL4020_LLD_PROXRATEREG_250_HZ:
      frequency = 250.0f;
      break;
    default:
      frequency = 1.95f;
      break;
  }
  aosTimerPeriodicInterval(&fn->proximity.timer, 1.0f / frequency * MICROSECONDS_PER_SECOND + 0.5f, _amiro_floor_triggercb, fn);

  // activate the ambient timer
  switch (fn->drivers.ambient_params & VCNL4020_LLD_ALPARAMREG_RATE_MASK) {
    case VCNL4020_LLD_ALPARAMREG_RATE_1_HZ:
      frequency = 1.0f;
      break;
    case VCNL4020_LLD_ALPARAMREG_RATE_2_HZ:
      frequency = 2.0f;
      break;
    case VCNL4020_LLD_ALPARAMREG_RATE_3_HZ:
      frequency = 3.0f;
      break;
    case VCNL4020_LLD_ALPARAMREG_RATE_4_HZ:
      frequency = 4.0f;
      break;
    case VCNL4020_LLD_ALPARAMREG_RATE_5_HZ:
      frequency = 5.0f;
      break;
    case VCNL4020_LLD_ALPARAMREG_RATE_6_HZ:
      frequency = 6.0f;
      break;
    case VCNL4020_LLD_ALPARAMREG_RATE_8_HZ:
      frequency = 8.0f;
      break;
    case VCNL4020_LLD_ALPARAMREG_RATE_10_HZ:
      frequency = 10.0f;
      break;
    default:
      frequency = 2.0f;
      break;
  }
  aosTimerPeriodicInterval(&fn->ambient.timer, 1.0f / frequency * MICROSECONDS_PER_SECOND + 0.5f, _amiro_floor_triggercb, fn);

  return PROXIMITY_TRIGGEREVENT | AMBIENT_TRIGGEREVENT | PROXIMITY_SERVICEEVENT;
}

/**
 * @brief   Loop callback function for floor nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] floor   Pointer to the floor structure.
 *                    Must not be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _amiro_floor_Loop(urt_node_t* node, urt_osEventMask_t event, void* floor)
{
  urtDebugAssert(floor != NULL);
  (void)node;

  // local constants
  amiro_floor_node_t* const fn = (amiro_floor_node_t*)floor;

  // if proximity and/or ambient data shall be read
  if (event & (PROXIMITY_TRIGGEREVENT | AMBIENT_TRIGGEREVENT)) {
    urt_osTime_t t;
    floor_proximitydata_t proxdata;
    floor_ambientdata_t aldata;

    /* get the current time
     * Since the exact measurement time at the sensor is unknown, this is an
     * optimistic (breifly before this function call) but conservative (before
     * reading the sensors) estimation.
     */
    urtTimeNow(&t);

    // read every sensor
    for (size_t sensor = 0; sensor < FLOOR_NUM_SENSORS; ++sensor) {
      // select VCNL
      pca9544a_lld_setchannel(fn->drivers.mux, _amiro_floor_Sensor2Channel(sensor), TIME_INFINITE);
      // read data
      if ((event & PROXIMITY_TRIGGEREVENT) && (event & AMBIENT_TRIGGEREVENT)) {
        vcnl4020_lld_readalsandprox(fn->drivers.vcnl, &aldata.data[sensor], &proxdata.data[sensor], TIME_INFINITE);
      } else if (event & PROXIMITY_TRIGGEREVENT) {
        vcnl4020_lld_readprox(fn->drivers.vcnl, &proxdata.data[sensor], TIME_INFINITE);
      } else if (event & AMBIENT_TRIGGEREVENT) {
        vcnl4020_lld_readals(fn->drivers.vcnl, &aldata.data[sensor], TIME_INFINITE);
      }
      // execute some proximity related logic
      if (event & PROXIMITY_TRIGGEREVENT) {
        // if calibration is in process
        if (fn->proximity.calibration.automatic.measurements > 0) {
          // accumulate sensor data
          fn->proximity.calibration.automatic.accumulated[sensor] += proxdata.data[sensor];
        }
        // apply calibration offset
        proxdata.data[sensor] = _amiro_floor_ProxApplyCalibration(proxdata.data[sensor], fn->proximity.calibration.offsets.data[sensor]);

      }
    }

    // publish data
    if (event & PROXIMITY_TRIGGEREVENT) {
      urtPublisherPublish(&fn->proximity.publisher, &proxdata, sizeof(proxdata), &t, URT_PUBLISHER_PUBLISH_ENFORCING);

      // if calibration is in process
      if (fn->proximity.calibration.automatic.measurements > 0) {
        // increase measurement counter
        ++fn->proximity.calibration.automatic.counter;
        // if calibration just finished
        if (fn->proximity.calibration.automatic.counter == fn->proximity.calibration.automatic.measurements) {
          // set new offsets
          for (size_t sensor = 0; sensor < FLOOR_NUM_SENSORS; ++sensor) {
            fn->proximity.calibration.offsets.data[sensor] = fn->proximity.calibration.automatic.accumulated[sensor] / fn->proximity.calibration.automatic.measurements;
          }
          // acquire request
          if (urtServiceAcquireRequest(&fn->proximity.calibration.service, &fn->proximity.calibration.dispatched) == URT_STATUS_OK) {
            // set return data
            floor_calibration_t* const dispatchedpayload = (floor_calibration_t*)urtRequestGetPayload(urtServiceDispatchedGetRequest(&fn->proximity.calibration.dispatched));
            dispatchedpayload->response.status = FLOOR_CALIBSTATUS_OK;
            dispatchedpayload->response.offsets = fn->proximity.calibration.offsets;
            // respond
            urtServiceRespond(&fn->proximity.calibration.dispatched, sizeof(floor_calibration_t));
          }
          // reset counters
          fn->proximity.calibration.automatic.measurements = 0;
          fn->proximity.calibration.automatic.counter = 0;
          // make the loop check for further requests
          event |= PROXIMITY_SERVICEEVENT;
        }
      }
    }
    if (event & AMBIENT_TRIGGEREVENT) {
      urtPublisherPublish(&fn->ambient.publisher, &aldata, sizeof(aldata), &t, URT_PUBLISHER_PUBLISH_ENFORCING);
    }

    event &= ~(PROXIMITY_TRIGGEREVENT | AMBIENT_TRIGGEREVENT);
  }

  // if a proximity service event occurred and no calibration is ongoing
  if (event & PROXIMITY_SERVICEEVENT && fn->proximity.calibration.automatic.measurements == 0) {
    // dispatch request
    floor_calibration_t requestdata;
    if (urtServiceDispatch(&fn->proximity.calibration.service, &fn->proximity.calibration.dispatched, &requestdata, NULL, NULL)) {
      urtDebugAssert(urtRequestGetPayloadSize(urtServiceDispatchedGetRequest(&fn->proximity.calibration.dispatched)) == sizeof(floor_calibration_t));
      switch (requestdata.request.command) {
        case FLOOR_CALIBRATION_GET:
        {
          // acquire request
          if (urtServiceAcquireRequest(&fn->proximity.calibration.service, &fn->proximity.calibration.dispatched) == URT_STATUS_OK) {
            // set return data
            floor_calibration_t* const dispatchedpayload = (floor_calibration_t*)urtRequestGetPayload(urtServiceDispatchedGetRequest(&fn->proximity.calibration.dispatched));
            dispatchedpayload->response.status = FLOOR_CALIBSTATUS_OK;
            dispatchedpayload->response.offsets = fn->proximity.calibration.offsets;
            // respond
            urtServiceRespond(&fn->proximity.calibration.dispatched, sizeof(floor_calibration_t));
          }
          break;
        }
        case FLOOR_CALIBRATION_SETONE:
        {
          // check sensor
          if (requestdata.request.data.offset.sensor < FLOOR_NUM_SENSORS) {
            // set offset
            fn->proximity.calibration.offsets.data[requestdata.request.data.offset.sensor] = requestdata.request.data.offset.value;
          }
          // acquire request
          if (urtServiceAcquireRequest(&fn->proximity.calibration.service, &fn->proximity.calibration.dispatched) == URT_STATUS_OK) {
            // set return data
            floor_calibration_t* const dispatchedpayload = (floor_calibration_t*)urtRequestGetPayload(urtServiceDispatchedGetRequest(&fn->proximity.calibration.dispatched));
            dispatchedpayload->response.status = (requestdata.request.data.offset.sensor < FLOOR_NUM_SENSORS) ? FLOOR_CALIBSTATUS_OK : FLOOR_CALIBSTATUS_FAIL;
            dispatchedpayload->response.offsets = fn->proximity.calibration.offsets;
            // respond
            urtServiceRespond(&fn->proximity.calibration.dispatched, sizeof(floor_calibration_t));
          }
          break;
        }
        case FLOOR_CALIBRATION_SETALL:
        {
          // set offsets
          fn->proximity.calibration.offsets = requestdata.request.data.offsets;
          // acquire request
          if (urtServiceAcquireRequest(&fn->proximity.calibration.service, &fn->proximity.calibration.dispatched) == URT_STATUS_OK) {
            // set return data
            floor_calibration_t* const dispatchedpayload = (floor_calibration_t*)urtRequestGetPayload(urtServiceDispatchedGetRequest(&fn->proximity.calibration.dispatched));
            dispatchedpayload->response.status = FLOOR_CALIBSTATUS_OK;
            dispatchedpayload->response.offsets = fn->proximity.calibration.offsets;
            // respond
            urtServiceRespond(&fn->proximity.calibration.dispatched, sizeof(floor_calibration_t));
          }
          break;
        }
        case FLOOR_CALIBRATION_AUTO:
        {
          // activate measurement
          fn->proximity.calibration.automatic.measurements = requestdata.request.data.measurements;
          fn->proximity.calibration.automatic.counter = 0;
          for (size_t sensor = 0; sensor < FLOOR_NUM_SENSORS; ++sensor) {
            fn->proximity.calibration.automatic.accumulated[sensor] = 0;
          }
          // calibration is executed with subsequent sensor readings
          break;
        }
      }
    }

    event &= ~PROXIMITY_SERVICEEVENT;
  }

  urtDebugAssert(event == 0);

  return PROXIMITY_TRIGGEREVENT | AMBIENT_TRIGGEREVENT | PROXIMITY_SERVICEEVENT;
}

/**
 * @brief   Shutdown callback function for floor nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] floor  Pointer to the floor structure.
 *                    Must nor be NULL.
 */
void _amiro_floor_Shutdown(urt_node_t* node, urt_status_t reason, void* floor)
{
  urtDebugAssert(floor != NULL);
  (void)node;
  (void)reason;

  // local constants
  amiro_floor_node_t* const fn = (amiro_floor_node_t*)floor;

  // stop timers
  aosTimerReset(&fn->proximity.timer);
  aosTimerReset(&fn->ambient.timer);

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_amiro_floor
 * @{
 */

/**
 * @brief   Initialize a floor sensing node.
 *
 * @param[in] floor                 Pointer to the floor sensing node to initialize.
 * @param[in] vcnl_driver           Pointer to a VCNL driver to use for sensor access.
 * @param[in] mux_driver            Pointer to a I2C multiplexer driver to select sensors.
 * @param[in] proximity_frequency   Frequency to configure for the proximity sensors.
 * @param[in] proximity_topic       Pointer to the topic to publish proximity data.
 * @param[in] proximity_serviceid   Service identifier for the proximity calibration service.
 * @param[in] ambient_parameters    Parameters for ambient light measurement.
 * @param[in] ambient_topic         Pointer to the topic to publish ambient data.
 * @param[in] prio                  Priority of the floor sensing node thread.
 */
void amiroFloorInit(amiro_floor_node_t* floor,
               VCNL4020Driver* vcnl_driver,
               PCA9544ADriver* mux_driver,
               vcnl4020_lld_proxratereg_t proximity_frequency,
               urt_topic_t* proximity_topic,
               urt_serviceid_t proximity_serviceid,
               uint8_t ambient_parameters,
               urt_topic_t* ambient_topic,
               urt_osThreadPrio_t prio)
{
  urtDebugAssert(floor != NULL);
  urtDebugAssert(vcnl_driver != NULL);
  urtDebugAssert(mux_driver != NULL);
  urtDebugAssert(proximity_topic != NULL);
  urtDebugAssert(ambient_topic != NULL);

  // set drivers
  floor->drivers.vcnl = vcnl_driver;
  floor->drivers.mux = mux_driver;
  floor->drivers.proximity_frequency = proximity_frequency;
  floor->drivers.ambient_params = ambient_parameters;

  // initialize proximity related data
  aosTimerInit(&floor->proximity.timer);
  urtPublisherInit(&floor->proximity.publisher, proximity_topic);
  for (size_t sensor = 0; sensor < FLOOR_NUM_SENSORS; ++sensor) {
    floor->proximity.calibration.offsets.data[sensor] = 0;
  }
  floor->proximity.calibration.service.id = proximity_serviceid;
  floor->proximity.calibration.automatic.measurements = 0;
  floor->proximity.calibration.automatic.counter = 0;

  // initialize ambient related data
  aosTimerInit(&floor->ambient.timer);
  urtPublisherInit(&floor->ambient.publisher, ambient_topic);

  // initialize the node
  urtNodeInit(&floor->node, (urt_osThread_t*)floor->thread, sizeof(floor->thread), prio,
              _amiro_floor_Setup, floor,
              _amiro_floor_Loop, floor,
              _amiro_floor_Shutdown, floor);

  return;
}

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
/**
 * @brief   Print floor sensor offsets to a stream.
 *
 * @param[in] stream    Stream to proint to.
 * @param[in] offsets   Offsets to print.
 */
void _amiro_floor_printOffsets(BaseSequentialStream* const stream, const floor_proximitydata_t* const offsets)
{
  urtDebugAssert(offsets != NULL);

  chprintf(stream, " left wheel: 0x%04X (%u)\n", offsets->sensors.left_wheel, offsets->sensors.left_wheel);
  chprintf(stream, " left front: 0x%04X (%u)\n", offsets->sensors.left_front, offsets->sensors.left_front);
  chprintf(stream, "right front: 0x%04X (%u)\n", offsets->sensors.right_front, offsets->sensors.right_front);
  chprintf(stream, "right wheel: 0x%04X (%u)\n", offsets->sensors.right_wheel, offsets->sensors.right_wheel);

  return;
}

/**
 * @brief   Shell callback to interact with the floor calibration service.
 *
 * @param[in] stream    Shell I/O stream.
 * @param[in] argc      Number of arguments.
 * @param[in] argv      Argument list.
 * @param[in] service   Pointer to the calibration service.
 *
 * @return  Operation result.
 */
int floorShellCallback_calibrate(BaseSequentialStream* stream, int argc, const char* argv[], urt_service_t* const service)
{
  urtDebugAssert(service != NULL);

  // local constants
  const eventmask_t SERVICEEVENT = EVENT_MASK(2);

  // local variables
  urt_nrtrequest_t request;
  floor_calibration_t calibdata;
  urtNrtRequestInit(&request, &calibdata);

  // read current offsets
  if ((argc == 2) &&
      (strcmp(argv[1], "--get") == 0 || strcmp(argv[1], "-g") == 0)) {
    // acquire request
    urtRequestAcquire((urt_baserequest_t*)&request);
    floor_calibration_t* const payload = (floor_calibration_t*)urtRequestGetPayload((urt_baserequest_t*)&request);
    // set command payload
    payload->request.command = FLOOR_CALIBRATION_GET;
    // submit request and wait for response
    urtRequestSubmit((urt_baserequest_t*)&request, service, sizeof(floor_calibration_t), SERVICEEVENT, URT_DELAY_INFINITE);
    urtEventWait(SERVICEEVENT, URT_EVENT_WAIT_ALL, URT_DELAY_INFINITE);
    urtRequestRetrieve((urt_baserequest_t*)&request, URT_REQUEST_RETRIEVE_ENFORCING, NULL, NULL);
    // print response
    if (payload->response.status == FLOOR_CALIBSTATUS_OK) {
      _amiro_floor_printOffsets(stream, &payload->response.offsets);
      urtRequestRelease((urt_baserequest_t*)&request);
      return AOS_OK;
    } else {
      chprintf(stream, "failed\n");
      urtRequestRelease((urt_baserequest_t*)&request);
      return AOS_WARNING;
    }
  }

  // set a single offset to a specified value
  else if ((argc == 4) &&
           (strcmp(argv[1], "--set") == 0 || strcmp(argv[1], "-s") == 0)) {
    // acquire request
    urtRequestAcquire((urt_baserequest_t*)&request);
    floor_calibration_t* const payload = (floor_calibration_t*)urtRequestGetPayload((urt_baserequest_t*)&request);
    // set command payload
    payload->request.command = FLOOR_CALIBRATION_SETONE;
    if (strcmp(argv[2], "lw") == 0) {
      payload->request.data.offset.sensor = 0;
    } else if (strcmp(argv[2], "lf") == 0) {
      payload->request.data.offset.sensor = 1;
    } else if (strcmp(argv[2], "rf") == 0) {
      payload->request.data.offset.sensor = 2;
    } else if (strcmp(argv[2], "rw") == 0) {
      payload->request.data.offset.sensor = 3;
    } else {
      payload->request.data.offset.sensor = strtol(argv[2], NULL, 0);
    }
    payload->request.data.offset.value = strtol(argv[3], NULL, 0);
    // submit request and wait for response
    urtRequestSubmit((urt_baserequest_t*)&request, service, sizeof(floor_calibration_t), SERVICEEVENT, URT_DELAY_INFINITE);
    urtEventWait(SERVICEEVENT, URT_EVENT_WAIT_ALL, URT_DELAY_INFINITE);
    urtRequestRetrieve((urt_baserequest_t*)&request, URT_REQUEST_RETRIEVE_ENFORCING, NULL, NULL);
    // print response
    if (payload->response.status == FLOOR_CALIBSTATUS_OK) {
      _amiro_floor_printOffsets(stream, &payload->response.offsets);
      urtRequestRelease((urt_baserequest_t*)&request);
      return AOS_OK;
    } else {
      chprintf(stream, "failed\n");
      urtRequestRelease((urt_baserequest_t*)&request);
      return AOS_WARNING;
    }
  }

  // set all offsets to specified values
  else if ((argc == 2 + FLOOR_NUM_SENSORS) &&
           (strcmp(argv[1], "--set") == 0 || strcmp(argv[1], "-s") == 0)) {
    // acquire request
    urtRequestAcquire((urt_baserequest_t*)&request);
    floor_calibration_t* const payload = (floor_calibration_t*)urtRequestGetPayload((urt_baserequest_t*)&request);
    // set command payload
    payload->request.command = FLOOR_CALIBRATION_SETALL;
    for (size_t sensor = 0; sensor < FLOOR_NUM_SENSORS; ++sensor) {
      payload->request.data.offsets.data[sensor] = strtol(argv[sensor + 2], NULL, 0);
    }
    // submit request and wait for response
    urtRequestSubmit((urt_baserequest_t*)&request, service, sizeof(floor_calibration_t), SERVICEEVENT, URT_DELAY_INFINITE);
    urtEventWait(SERVICEEVENT, URT_EVENT_WAIT_ALL, URT_DELAY_INFINITE);
    urtRequestRetrieve((urt_baserequest_t*)&request, URT_REQUEST_RETRIEVE_ENFORCING, NULL, NULL);
    // print response
    if (payload->response.status == FLOOR_CALIBSTATUS_OK) {
      _amiro_floor_printOffsets(stream, &payload->response.offsets);
      urtRequestRelease((urt_baserequest_t*)&request);
      return AOS_OK;
    } else {
      chprintf(stream, "failed\n");
      urtRequestRelease((urt_baserequest_t*)&request);
      return AOS_WARNING;
    }
  }

  // perform automatic calibration
  else if ((argc == 3) &&
           (strcmp(argv[1], "--auto") == 0 || strcmp(argv[1], "-a") == 0)) {
    // acquire request
    urtRequestAcquire((urt_baserequest_t*)&request);
    floor_calibration_t* const payload = (floor_calibration_t*)urtRequestGetPayload((urt_baserequest_t*)&request);
    // set command payload
    payload->request.command = FLOOR_CALIBRATION_AUTO;
    payload->request.data.measurements = strtol(argv[2], NULL, 0);
    // submit request and wait for response
    urtRequestSubmit((urt_baserequest_t*)&request, service, sizeof(floor_calibration_t), SERVICEEVENT, URT_DELAY_INFINITE);
    urtEventWait(SERVICEEVENT, URT_EVENT_WAIT_ALL, URT_DELAY_INFINITE);
    urtRequestRetrieve((urt_baserequest_t*)&request, URT_REQUEST_RETRIEVE_ENFORCING, NULL, NULL);
    // print response
    if (payload->response.status == FLOOR_CALIBSTATUS_OK) {
      _amiro_floor_printOffsets(stream, &payload->response.offsets);
      urtRequestRelease((urt_baserequest_t*)&request);
      return AOS_OK;
    } else {
      chprintf(stream, "failed\n");
      urtRequestRelease((urt_baserequest_t*)&request);
      return AOS_WARNING;
    }
  }

  // print help
  else {
    chprintf(stream, "usage: %s OPTION [PARAMETERS...]\n", argv[0]);
    chprintf(stream, "The following options are available:\n");
    chprintf(stream, "--help, -h\n");
    chprintf(stream, "  Print this help message.\n");
    chprintf(stream, "--get, -g\n");
    chprintf(stream, "  Retrieve the currently configured calibration offsets.\n");
    chprintf(stream, "--set, -s\n");
    chprintf(stream, "  Manually set calibration offsets.\n");
    chprintf(stream, "  Requires either %u offset parameters or two parameters: SENSOR OFFSET.\n", FLOOR_NUM_SENSORS);
    chprintf(stream, "  In the latter case, SENSOR may be an integer in range [0, %u] or a string of the set\n", FLOOR_NUM_SENSORS);
    chprintf(stream, "  {lw, lf, rf, rw}\n");
    chprintf(stream, "--auto, -a\n");
    chprintf(stream, "  Perform automatic calibration.\n");
    chprintf(stream, "  Requires an additional parameter, specifying the number of measurements.\n");

    if (argc == 2 &&
        (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
      return AOS_OK;
    } else {
      return AOS_INVALIDARGUMENTS;
    }
  }
}
#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */

/** @} */
