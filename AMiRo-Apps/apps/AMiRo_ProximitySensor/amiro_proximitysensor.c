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
 * @file    amiro_proximitysensor.c
 */

#include "amiro_proximitysensor.h"

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

/**
 * @brief   I2C multiplexer channel selection type.
 */
struct proximitysensor_i2cchannel {
  /**
   * @brief   I2C bus to use to communicate with the sensor.
   */
  size_t bus : (sizeof(size_t) - sizeof(pca9544a_lld_chid_t)) * 8;

  /**
   * @brief   Multiplexer channel to set i order to communicate with the sensor.
   */
  pca9544a_lld_chid_t channel : sizeof(pca9544a_lld_chid_t) * 8;
};

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/

/**
 * @brief   Name of floor nodes.
 */
static const char _amiro_proximitysensor_name[] = "Amiro_ProximitySensor";

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   Callback function for the trigger timer.
 *
 * @param[in] flags   Flags to emit for the trigger event.
 *                    Pointer is reinterpreted as integer value.
 */
static void _amiro_proximitysensor_triggercb(virtual_timer_t* timer, void* params)
{
  // local constants
  amiro_proximitysensor_node_t* const fn = (amiro_proximitysensor_node_t*)params;

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
inline struct proximitysensor_i2cchannel _amiro_proximitysensor_Sensor2Channel(const size_t sensor)
{
  struct proximitysensor_i2cchannel i2c;

  switch (sensor) {
    case 0:
      // north-northwest
      i2c.bus = 1;
      i2c.channel = PCA9544A_LLD_CH0;
      break;
    case 1:
      // west-northwest
      i2c.bus = 0;
      i2c.channel = PCA9544A_LLD_CH2;
      break;
    case 2:
      // west-southwest
      i2c.bus = 0;
      i2c.channel = PCA9544A_LLD_CH3;
      break;
    case 3:
      // south-southwest
      i2c.bus = 0;
      i2c.channel = PCA9544A_LLD_CH1;
      break;
    case 4:
      // south-southeast
      i2c.bus = 0;
      i2c.channel = PCA9544A_LLD_CH0;
      break;
    case 5:
      // east-southeast
      i2c.bus = 1;
      i2c.channel = PCA9544A_LLD_CH2;
      break;
    case 6:
      // east-northeast
      i2c.bus = 1;
      i2c.channel = PCA9544A_LLD_CH3;
      break;
    case 7:
      // north-northeast
      i2c.bus = 1;
      i2c.channel = PCA9544A_LLD_CH1;
      break;
    default:
      i2c.bus = ~0;
      i2c.channel = PCA9544A_LLD_CH_NONE;
      break;
  }

  return i2c;
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
inline proximitysensor_proximity_t _amiro_proximitysensor_ProxApplyCalibration(const proximitysensor_proximity_t value, const proximitysensor_proximity_t offset)
{
  const float scalefactor = (float)((proximitysensor_proximity_t)~0) / (float)(((proximitysensor_proximity_t)~0) - offset);
  return (value - offset) * scalefactor;
}

/**
 * @brief   Setup callback function for ProximitySensor nodes.
 *
 * @param[in] node              Pointer to the node object.
 *                              Must not be NULL.
 * @param[in] proximitysensor   Pointer to the ProximitySensor structure.
 *                              Must nor be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _amiro_proximitysensor_Setup(urt_node_t* node, void* proximitysensor)
{
  urtDebugAssert(proximitysensor != NULL);
  (void)node;

  // local constants
  amiro_proximitysensor_node_t* const psn = (amiro_proximitysensor_node_t*)proximitysensor;

  // local variables
  float frequency;

  // set thread name
  chRegSetThreadName(_amiro_proximitysensor_name);

#if (URT_CFG_RPC_ENABLED == true)
  // initialize the calibration service
  urtServiceInit(&psn->proximity.calibration.service, psn->proximity.calibration.service.id, urtThreadGetSelf(), PROXIMITY_SERVICEEVENT);
#endif /* URT_CFG_RPC_ENABLED == true */

  // configure and start every sensor
  for (size_t sensor = 0; sensor < PROXIMITYSENSOR_NUM_SENSORS; ++sensor) {
    // select VCNL
    const struct proximitysensor_i2cchannel i2c_select = _amiro_proximitysensor_Sensor2Channel(sensor);
    pca9544a_lld_setchannel(psn->drivers.mux[i2c_select.bus], i2c_select.channel, TIME_INFINITE);
    // configure sensor
    vcnl4020_lld_writereg(psn->drivers.vcnl[i2c_select.bus], VCNL4020_LLD_REGADDR_PROXRATE, psn->drivers.proximity_frequency, TIME_INFINITE);
    vcnl4020_lld_writereg(psn->drivers.vcnl[i2c_select.bus], VCNL4020_LLD_REGADDR_LEDCURRENT, VCNL4020_LLD_LEDCURRENTREG_200_mA, TIME_INFINITE);
    vcnl4020_lld_writereg(psn->drivers.vcnl[i2c_select.bus], VCNL4020_LLD_REGADDR_ALPARAM, psn->drivers.ambient_params, TIME_INFINITE);
    // start sensor
    vcnl4020_lld_writereg(psn->drivers.vcnl[i2c_select.bus], VCNL4020_LLD_REGADDR_CMD, (VCNL4020_LLD_CMDREG_ALSEN | VCNL4020_LLD_CMDREG_PROXEN | VCNL4020_LLD_CMDREG_SELFTIMED), TIME_INFINITE);
  }

  // activate the proximity timer
  switch (psn->drivers.proximity_frequency) {
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
  aosTimerPeriodicInterval(&psn->proximity.timer, 1.0f / frequency * MICROSECONDS_PER_SECOND + 0.5f, _amiro_proximitysensor_triggercb, psn);

  // activate the ambient timer
  switch (psn->drivers.ambient_params & VCNL4020_LLD_ALPARAMREG_RATE_MASK) {
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
  aosTimerPeriodicInterval(&psn->ambient.timer, 1.0f / frequency * MICROSECONDS_PER_SECOND + 0.5f, _amiro_proximitysensor_triggercb, psn);

  return PROXIMITY_TRIGGEREVENT | AMBIENT_TRIGGEREVENT | PROXIMITY_SERVICEEVENT;
}

/**
 * @brief   Loop callback function for ProximitySensor nodes.
 *
 * @param[in] node              Pointer to the node object.
 *                              Must not be NULL.
 * @param[in] proximitysensor   Pointer to the ProximitySensor structure.
 *                              Must not be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _amiro_proximitysensor_Loop(urt_node_t* node, urt_osEventMask_t event, void* proximitysensor)
{
  urtDebugAssert(proximitysensor != NULL);
  (void)node;

  // local constants
  amiro_proximitysensor_node_t* const psn = (amiro_proximitysensor_node_t*)proximitysensor;

  // if proximity and/or ambient data shall be read
  if (event & (PROXIMITY_TRIGGEREVENT | AMBIENT_TRIGGEREVENT)) {
    urt_osTime_t t;
    proximitysensor_proximitydata_t proxdata;
    proximitysensor_ambientdata_t aldata;

    /* get the current time
     * Since the exact measurement time at the sensor is unknown, this is an
     * optimistic (breifly before this function call) but conservative (before
     * reading the sensors) estimation.
     */
    urtTimeNow(&t);

    // read every sensor
    for (size_t sensor = 0; sensor < PROXIMITYSENSOR_NUM_SENSORS; ++sensor) {
      // select VCNL
      const struct proximitysensor_i2cchannel i2c_select = _amiro_proximitysensor_Sensor2Channel(sensor);
      pca9544a_lld_setchannel(psn->drivers.mux[i2c_select.bus], i2c_select.channel, TIME_INFINITE);
      // read data
      if ((event & PROXIMITY_TRIGGEREVENT) && (event & AMBIENT_TRIGGEREVENT)) {
        vcnl4020_lld_readalsandprox(psn->drivers.vcnl[i2c_select.bus], &aldata.data[sensor], &proxdata.data[sensor], TIME_INFINITE);
      } else if (event & PROXIMITY_TRIGGEREVENT) {
        vcnl4020_lld_readprox(psn->drivers.vcnl[i2c_select.bus], &proxdata.data[sensor], TIME_INFINITE);
      } else if (event & AMBIENT_TRIGGEREVENT) {
        vcnl4020_lld_readals(psn->drivers.vcnl[i2c_select.bus], &aldata.data[sensor], TIME_INFINITE);
      }
      // execute some proximity related logic
      if (event & PROXIMITY_TRIGGEREVENT) {
        // if calibration is in pcocess
        if (psn->proximity.calibration.automatic.measurements > 0) {
          // accumulate sensor data
          psn->proximity.calibration.automatic.accumulated[sensor] += proxdata.data[sensor];
        }
        // apply calibration offset
        proxdata.data[sensor] = _amiro_proximitysensor_ProxApplyCalibration(proxdata.data[sensor], psn->proximity.calibration.offsets.data[sensor]);

      }
    }

    // publish data
    if (event & PROXIMITY_TRIGGEREVENT) {
      urtPublisherPublish(&psn->proximity.publisher, &proxdata, sizeof(proxdata), &t, URT_PUBLISHER_PUBLISH_ENFORCING);

      // if calibration is in process
      if (psn->proximity.calibration.automatic.measurements > 0) {
        // increase measurement counter
        ++psn->proximity.calibration.automatic.counter;
        // if calibration just finished
        if (psn->proximity.calibration.automatic.counter == psn->proximity.calibration.automatic.measurements) {
          // set new offsets
          for (size_t sensor = 0; sensor < PROXIMITYSENSOR_NUM_SENSORS; ++sensor) {
            psn->proximity.calibration.offsets.data[sensor] = psn->proximity.calibration.automatic.accumulated[sensor] / psn->proximity.calibration.automatic.measurements;
          }
          // acquire request
          if (urtServiceAcquireRequest(&psn->proximity.calibration.service, &psn->proximity.calibration.dispatched) == URT_STATUS_OK) {
            // set return data
            proximitysensor_calibration_t* const dispatchedpayload = (proximitysensor_calibration_t*)urtRequestGetPayload(urtServiceDispatchedGetRequest(&psn->proximity.calibration.dispatched));
            dispatchedpayload->response.status = PROXIMITYSENSOR_CALIBSTATUS_OK;
            dispatchedpayload->response.offsets = psn->proximity.calibration.offsets;
            // respond
            urtServiceRespond(&psn->proximity.calibration.dispatched, sizeof(proximitysensor_calibration_t));
          }
          // reset counters
          psn->proximity.calibration.automatic.measurements = 0;
          psn->proximity.calibration.automatic.counter = 0;
          // make the loop check for further requests
          event |= PROXIMITY_SERVICEEVENT;
        }
      }
    }
    if (event & AMBIENT_TRIGGEREVENT) {
      urtPublisherPublish(&psn->ambient.publisher, &aldata, sizeof(aldata), &t, URT_PUBLISHER_PUBLISH_ENFORCING);
    }

    event &= ~(PROXIMITY_TRIGGEREVENT | AMBIENT_TRIGGEREVENT);
  }

  // if a proximity service event occurred and no calibration is ongoing
  if (event & PROXIMITY_SERVICEEVENT && psn->proximity.calibration.automatic.measurements == 0) {
    // dispatch request
    proximitysensor_calibration_t requestdata;
    if (urtServiceDispatch(&psn->proximity.calibration.service, &psn->proximity.calibration.dispatched, &requestdata, NULL, NULL)) {
      urtDebugAssert(urtRequestGetPayloadSize(urtServiceDispatchedGetRequest(&psn->proximity.calibration.dispatched)) == sizeof(proximitysensor_calibration_t));
      switch (requestdata.request.command) {
        case PROXIMITYSENSOR_CALIBRATION_GET:
        {
          // acquire request
          if (urtServiceAcquireRequest(&psn->proximity.calibration.service, &psn->proximity.calibration.dispatched) == URT_STATUS_OK) {
            // set return data
            proximitysensor_calibration_t* const dispatchedpayload = (proximitysensor_calibration_t*)urtRequestGetPayload(urtServiceDispatchedGetRequest(&psn->proximity.calibration.dispatched));
            dispatchedpayload->response.status = PROXIMITYSENSOR_CALIBSTATUS_OK;
            dispatchedpayload->response.offsets = psn->proximity.calibration.offsets;
            // respond
            urtServiceRespond(&psn->proximity.calibration.dispatched, sizeof(proximitysensor_calibration_t));
          }
          break;
        }
        case PROXIMITYSENSOR_CALIBRATION_SETONE:
        {
          // check sensor
          if (requestdata.request.data.offset.sensor < PROXIMITYSENSOR_NUM_SENSORS) {
            // set offset
            psn->proximity.calibration.offsets.data[requestdata.request.data.offset.sensor] = requestdata.request.data.offset.value;
          }
          // acquire request
          if (urtServiceAcquireRequest(&psn->proximity.calibration.service, &psn->proximity.calibration.dispatched) == URT_STATUS_OK) {
            // set return data
            proximitysensor_calibration_t* const dispatchedpayload = (proximitysensor_calibration_t*)urtRequestGetPayload(urtServiceDispatchedGetRequest(&psn->proximity.calibration.dispatched));
            dispatchedpayload->response.status = (requestdata.request.data.offset.sensor < PROXIMITYSENSOR_NUM_SENSORS) ? PROXIMITYSENSOR_CALIBSTATUS_OK : PROXIMITYSENSOR_CALIBSTATUS_FAIL;
            dispatchedpayload->response.offsets = psn->proximity.calibration.offsets;
            // respond
            urtServiceRespond(&psn->proximity.calibration.dispatched, sizeof(proximitysensor_calibration_t));
          }
          break;
        }
        case PROXIMITYSENSOR_CALIBRATION_SETALL:
        {
          // set offsets
          psn->proximity.calibration.offsets = requestdata.request.data.offsets;
          // acquire request
          if (urtServiceAcquireRequest(&psn->proximity.calibration.service, &psn->proximity.calibration.dispatched) == URT_STATUS_OK) {
            // set return data
            proximitysensor_calibration_t* const dispatchedpayload = (proximitysensor_calibration_t*)urtRequestGetPayload(urtServiceDispatchedGetRequest(&psn->proximity.calibration.dispatched));
            dispatchedpayload->response.status = PROXIMITYSENSOR_CALIBSTATUS_OK;
            dispatchedpayload->response.offsets = psn->proximity.calibration.offsets;
            // respond
            urtServiceRespond(&psn->proximity.calibration.dispatched, sizeof(proximitysensor_calibration_t));
          }
          break;
        }
        case PROXIMITYSENSOR_CALIBRATION_AUTO:
        {
          // activate measurement
          psn->proximity.calibration.automatic.measurements = requestdata.request.data.measurements;
          psn->proximity.calibration.automatic.counter = 0;
          for (size_t sensor = 0; sensor < PROXIMITYSENSOR_NUM_SENSORS; ++sensor) {
            psn->proximity.calibration.automatic.accumulated[sensor] = 0;
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
 * @brief   Shutdown callback function for ProximitySensor nodes.
 *
 * @param[in] node              Pointer to the node object.
 *                              Must not be NULL.
 * @param[in] proximitysensor   Pointer to the floor structure.
 *                              Must nor be NULL.
 */
void _amiro_proximitysensor_Shutdown(urt_node_t* node, urt_status_t reason, void* proximitysensor)
{
  urtDebugAssert(proximitysensor != NULL);
  (void)node;
  (void)reason;

  // local constants
  amiro_proximitysensor_node_t* const psn = (amiro_proximitysensor_node_t*)proximitysensor;

  // stop timers
  aosTimerReset(&psn->proximity.timer);
  aosTimerReset(&psn->ambient.timer);

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_amiro_proximitysensor
 * @{
 */

/**
 * @brief   Initialize a floor sensing node.
 *
 * @param[in] proximitysensor       Pointer to the ProximitySensor node to initialize.
 * @param[in] vcnl_driver           Pointer to a buffer of VCNL drivers to use for sensor access.
 * @param[in] mux_driver            Pointer to a buffer of I2C multiplexer drivers to select sensors.
 * @param[in] proximity_frequency   Frequency to configure for the proximity sensors.
 * @param[in] proximity_topic       Pointer to the topic to publish proximity data.
 * @param[in] proximity_serviceid   Service identifier for the proximity calibration service.
 * @param[in] ambient_parameters    Parameters for ambient light measurement.
 * @param[in] ambient_topic         Pointer to the topic to publish ambient data.
 * @param[in] prio                  Priority of the floor sensing node thread.
 */
void amiroProximitySensorInit(amiro_proximitysensor_node_t* proximitysensor,
                         VCNL4020Driver* vcnl_driver[PROXIMITYSENSOR_I2C_BUSSES],
                         PCA9544ADriver* mux_driver[PROXIMITYSENSOR_I2C_BUSSES],
                         vcnl4020_lld_proxratereg_t proximity_frequency,
                         urt_topic_t* proximity_topic,
                         urt_serviceid_t proximity_serviceid,
                         uint8_t ambient_parameters,
                         urt_topic_t* ambient_topic,
                         urt_osThreadPrio_t prio)
{
  urtDebugAssert(proximitysensor != NULL);
  for (size_t sensor = 0; sensor < PROXIMITYSENSOR_I2C_BUSSES; ++sensor) {
    urtDebugAssert(vcnl_driver[sensor] != NULL);
    urtDebugAssert(mux_driver[sensor] != NULL);
  }
  urtDebugAssert(proximity_topic != NULL);
  urtDebugAssert(ambient_topic != NULL);

  // set drivers
  for (size_t sensor = 0; sensor < PROXIMITYSENSOR_I2C_BUSSES; ++sensor) {
    proximitysensor->drivers.vcnl[sensor] = vcnl_driver[sensor];
    proximitysensor->drivers.mux[sensor] = mux_driver[sensor];
  }
  proximitysensor->drivers.proximity_frequency = proximity_frequency;
  proximitysensor->drivers.ambient_params = ambient_parameters;

  // initialize proximity related data
  aosTimerInit(&proximitysensor->proximity.timer);
  urtPublisherInit(&proximitysensor->proximity.publisher, proximity_topic);
  for (size_t sensor = 0; sensor < PROXIMITYSENSOR_NUM_SENSORS; ++sensor) {
    proximitysensor->proximity.calibration.offsets.data[sensor] = 0;
  }
  proximitysensor->proximity.calibration.service.id = proximity_serviceid;
  proximitysensor->proximity.calibration.automatic.measurements = 0;
  proximitysensor->proximity.calibration.automatic.counter = 0;

  // initialize ambient related data
  aosTimerInit(&proximitysensor->ambient.timer);
  urtPublisherInit(&proximitysensor->ambient.publisher, ambient_topic);

  // initialize the node
  urtNodeInit(&proximitysensor->node, (urt_osThread_t*)proximitysensor->thread, sizeof(proximitysensor->thread), prio,
              _amiro_proximitysensor_Setup, proximitysensor,
              _amiro_proximitysensor_Loop, proximitysensor,
              _amiro_proximitysensor_Shutdown, proximitysensor);

  return;
}

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
/**
 * @brief   Print proximity sensor offsets to a stream.
 *
 * @param[in] stream    Stream to proint to.
 * @param[in] offsets   Offsets to print.
 */
void _amiro_proximitysensor_printOffsets(BaseSequentialStream* const stream, const proximitysensor_proximitydata_t* const offsets)
{
  urtDebugAssert(offsets != NULL);

  chprintf(stream, "north-northwest: 0x%04X (%u)\n", offsets->sensors.nnw, offsets->sensors.nnw);
  chprintf(stream, " west-northwest: 0x%04X (%u)\n", offsets->sensors.wnw, offsets->sensors.wnw);
  chprintf(stream, " west-southwest: 0x%04X (%u)\n", offsets->sensors.wsw, offsets->sensors.wsw);
  chprintf(stream, "south-southwest: 0x%04X (%u)\n", offsets->sensors.ssw, offsets->sensors.ssw);
  chprintf(stream, "south-southeast: 0x%04X (%u)\n", offsets->sensors.sse, offsets->sensors.sse);
  chprintf(stream, " east-southeast: 0x%04X (%u)\n", offsets->sensors.ese, offsets->sensors.ese);
  chprintf(stream, " east-northeast: 0x%04X (%u)\n", offsets->sensors.ene, offsets->sensors.ene);
  chprintf(stream, "north-northeast: 0x%04X (%u)\n", offsets->sensors.nne, offsets->sensors.nne);

  return;
}

/**
 * @brief   Shell callback to interact with the ProximitySensor calibration service.
 *
 * @param[in] stream    Shell I/O stream.
 * @param[in] argc      Number of arguments.
 * @param[in] argv      Argument list.
 * @param[in] service   Pointer to the calibration service.
 *
 * @return  Operation result.
 */
int proximitysensorShellCallback_calibrate(BaseSequentialStream* stream, int argc, const char* argv[], urt_service_t* const service)
{
  urtDebugAssert(service != NULL);

  // local constants
  const eventmask_t SERVICEEVENT = EVENT_MASK(2);

  // local variables
  urt_nrtrequest_t request;
  proximitysensor_calibration_t calibdata;
  urtNrtRequestInit(&request, &calibdata);

  // read current offsets
  if ((argc == 2) &&
      (strcmp(argv[1], "--get") == 0 || strcmp(argv[1], "-g") == 0)) {
    // acquire request
    urtRequestAcquire((urt_baserequest_t*)&request);
    proximitysensor_calibration_t* const payload = (proximitysensor_calibration_t*)urtRequestGetPayload((urt_baserequest_t*)&request);
    // set command payload
    payload->request.command = PROXIMITYSENSOR_CALIBRATION_GET;
    // submit request and wait for response
    urtRequestSubmit((urt_baserequest_t*)&request, service, sizeof(proximitysensor_calibration_t), SERVICEEVENT, URT_DELAY_INFINITE);
    urtEventWait(SERVICEEVENT, URT_EVENT_WAIT_ALL, URT_DELAY_INFINITE);
    urtRequestRetrieve((urt_baserequest_t*)&request, URT_REQUEST_RETRIEVE_ENFORCING, NULL, NULL);
    // print response
    if (payload->response.status == PROXIMITYSENSOR_CALIBSTATUS_OK) {
      _amiro_proximitysensor_printOffsets(stream, &payload->response.offsets);
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
    proximitysensor_calibration_t* const payload = (proximitysensor_calibration_t*)urtRequestGetPayload((urt_baserequest_t*)&request);
    // set command payload
    payload->request.command = PROXIMITYSENSOR_CALIBRATION_SETONE;
    if (strcmp(argv[2], "nnw") == 0) {
      payload->request.data.offset.sensor = 0;
    } else if (strcmp(argv[2], "wnw") == 0) {
      payload->request.data.offset.sensor = 1;
    } else if (strcmp(argv[2], "wsw") == 0) {
      payload->request.data.offset.sensor = 2;
    } else if (strcmp(argv[2], "ssw") == 0) {
      payload->request.data.offset.sensor = 3;
    } else if (strcmp(argv[2], "sse") == 0) {
      payload->request.data.offset.sensor = 4;
    } else if (strcmp(argv[2], "ese") == 0) {
      payload->request.data.offset.sensor = 5;
    } else if (strcmp(argv[2], "ene") == 0) {
      payload->request.data.offset.sensor = 6;
    } else if (strcmp(argv[2], "nne") == 0) {
      payload->request.data.offset.sensor = 7;
    } else {
      payload->request.data.offset.sensor = strtol(argv[2], NULL, 0);
    }
    payload->request.data.offset.value = strtol(argv[3], NULL, 0);
    // submit request and wait for response
    urtRequestSubmit((urt_baserequest_t*)&request, service, sizeof(proximitysensor_calibration_t), SERVICEEVENT, URT_DELAY_INFINITE);
    urtEventWait(SERVICEEVENT, URT_EVENT_WAIT_ALL, URT_DELAY_INFINITE);
    urtRequestRetrieve((urt_baserequest_t*)&request, URT_REQUEST_RETRIEVE_ENFORCING, NULL, NULL);
    // print response
    if (payload->response.status == PROXIMITYSENSOR_CALIBSTATUS_OK) {
      _amiro_proximitysensor_printOffsets(stream, &payload->response.offsets);
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
    proximitysensor_calibration_t* const payload = (proximitysensor_calibration_t*)urtRequestGetPayload((urt_baserequest_t*)&request);
    // set command payload
    payload->request.command = PROXIMITYSENSOR_CALIBRATION_AUTO;
    payload->request.data.measurements = strtol(argv[2], NULL, 0);
    // submit request and wait for response
    urtRequestSubmit((urt_baserequest_t*)&request, service, sizeof(proximitysensor_calibration_t), SERVICEEVENT, URT_DELAY_INFINITE);
    urtEventWait(SERVICEEVENT, URT_EVENT_WAIT_ALL, URT_DELAY_INFINITE);
    urtRequestRetrieve((urt_baserequest_t*)&request, URT_REQUEST_RETRIEVE_ENFORCING, NULL, NULL);
    // print response
    if (payload->response.status == PROXIMITYSENSOR_CALIBSTATUS_OK) {
      _amiro_proximitysensor_printOffsets(stream, &payload->response.offsets);
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
    chprintf(stream, "  Manually set a calibration offset.\n");
    chprintf(stream, "  Requires two parameters: SENSOR OFFSET\n");
    chprintf(stream, "  SENSOR may be an integer in range [0, %u] or a string of the set\n", PROXIMITYSENSOR_NUM_SENSORS);
    chprintf(stream, "  {nnw, nne, ene, ese, sse, ssw, wsw, wnw, nnw}\n");
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
