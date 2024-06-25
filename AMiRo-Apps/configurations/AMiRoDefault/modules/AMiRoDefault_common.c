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
 * @file    AMiRoDefault_common.c
 * @brief   AMiRoDefault common code.
 */

#include "AMiRoDefault_common.h"

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/**
 * @brief   Number of additional FBCAN buffer elemts to provide.
 */
#if !defined(AMIRODEFAULT_FBCANBUFFER_SIZE) || defined(__DOXYGEN__)
#define AMIRODEFAULT_FBCANBUFFER_SIZE           2
#endif

/**
 * @brief   Number of µRT instances to synchronize.
 */
#if !defined(AMIRODOEFAULT_SYNC_INSTANCES) || defined(__DOXYGEN__)
#if (AMIROOS_CFG_SSSP_ENABLE == true) || defined(__DOXYGEN__)
#define AMIRODOEFAULT_SYNC_INSTANCES            (aos.sssp.modulecount)
#else
#define AMIRODOEFAULT_SYNC_INSTANCES            3
#endif
#endif

/**
 * @brief   CAN frame identifier for synchronization messages.
 */
#if !defined(AMIRODEFAULT_SYNC_CANID) || defined(__DOXYGEN__)
#define AMIRODEFAULT_SYNC_CANID                 0x400
#endif

/******************************************************************************/
/* EXPORTED VARIABLES                                                         */
/******************************************************************************/

urt_sync_t robotsync;

/******************************************************************************/
/* LOCAL TYPES                                                                */
/******************************************************************************/

/**
 * @brief   Data for robot-wide µRT synchronization.
 */
struct robotsync_data {
  /**
   * @brief   Counter of synchronized modules.
   */
  unsigned int synced;

  /**
   * @brief   Event source to use for broadcasting synchronization events.
   */
  urt_osEventSource_t* evtsource;

  /**
   * @brief   Flags to broadcast in case of a successful synchronization.
   */
  urt_osEventFlags_t successflags;

  /**
   * @brief   Flags to broadcast in case of a failed synchronization.
   */
  urt_osEventFlags_t errorflags;

  /**
   * @brief   Filter for FBCAN.
   */
  aos_fbcan_filter_t fbcanfilter;
};

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/

#if (AMIRODEFAULT_FBCANBUFFER_SIZE > 0) || defined(__DOXYGEN__)
/**
 * @brief   Transmission frames to add to the FBCAN buffer.
 */
aos_fbcan_txframe_t _fbcanbuffer[AMIRODEFAULT_FBCANBUFFER_SIZE];
#endif /* (AMIRODEFAULT_FBCANBUFFER_SIZE > 0) */

/**
 * @brief   Synchronization data.
 */
static struct robotsync_data _robotsync_data;

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   Callback function to be executed when receiving synchronization
 *          frames via CAN.
 *
 * @param[in] frame   Received frame.
 * @param[in] params  Synchronization data.
 */
void _robotsync_cancb(const CANRxFrame* frame, void* params)
{
  (void)frame;

  struct robotsync_data* const data = params;

  // if all modules are synced
  if (++data->synced == AMIRODOEFAULT_SYNC_INSTANCES) {
    // emit success event
    chEvtBroadcastFlagsI(data->evtsource, data->successflags);
    // reset data
    data->synced = 0;
    data->evtsource = NULL;
    data->successflags = 0;
    data->errorflags = 0;
  }

  return;
}

/**
 * @brief   Callback function to be executed during µRT synchronization.
 *
 * @param[in] stage         Synchronization stage of this call.
 * @param[in] eventsource   Event source to broadcast.
 * @param[in] successflags  Flags to broadcast as soon as the synchronization is completed successfully.
 * @param[in] errorflags    Flags to broadcats if an error is encountered.
 * @param[in] params        Pointer to synchronizatoin data.
 *
 * @return  Status indicating the success of the call.
 */
urt_status_t _robotsync_cb(urt_syncstage_t stage, urt_osEventSource_t* eventsource, urt_osEventFlags_t successflags, urt_osEventFlags_t errorflags, void* params)
{
  struct robotsync_data* const data = params;

  switch (stage) {
    case URT_SYNCSTAGE_INITIAL:
    {
      aosDbgAssert(data->evtsource == NULL);
      aosDbgAssert(data->successflags == 0);
      aosDbgAssert(data->errorflags == 0);
      // do nothing
      return URT_STATUS_SYNC_PENDING;
    }
    case URT_SYNCSTAGE_FINAL:
    {
      // acquire CAN frame
      CANTxFrame* frame = aosFBCanAcquireTxFrame(&MODULE_HAL_CAN, TIME_IMMEDIATE);
      if (frame == NULL) {
        // emit error event and return
        urtEventSourceBroadcast(eventsource, errorflags);
        return URT_STATUS_SYNC_ERROR;
      }
      // emit CAN message
      frame->DLC = 0;
      frame->IDE = data->fbcanfilter.filter.std.ide;
      frame->RTR = data->fbcanfilter.filter.std.rtr;
      frame->SID = data->fbcanfilter.filter.std.id;
      aosFBCanTransmit(&MODULE_HAL_CAN, frame);
      // increment synced counter
      chSysLock();
      const bool sync_complete = ++data->synced == AMIRODOEFAULT_SYNC_INSTANCES;
      chSysUnlock();
      // if synchronization is complete
      if (sync_complete) {
        // emit success event
        urtEventSourceBroadcast(eventsource, successflags);
        // reset data
        data->synced = 0;
        data->evtsource = NULL;
        data->successflags = 0;
        data->errorflags = 0;
        return URT_STATUS_OK;
      } else {
        // store event data and return
        data->evtsource = eventsource;
        data->successflags = successflags;
        data->errorflags = errorflags;
        return URT_STATUS_SYNC_PENDING;
      }
      break;
    }
  }

  // this shoud never occurr
  return URT_STATUS_SYNC_ERROR;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup configs_amirodefault_modules
 * @{
 */

/**
 * @brief   Initializes all data applications for the AMiRo default configuration.
 */
void appsCommonInit(void)
{
#if (AMIRODEFAULT_FBCANBUFFER_SIZE > 0)
  // provide FBCAN buffer elements
  aosFBCanProvideBuffer(&MODULE_HAL_CAN, _fbcanbuffer, AMIRODEFAULT_FBCANBUFFER_SIZE);
#endif /* (AMIRODEFAULT_FBCANBUFFER_SIZE > 0) */

  // initialize synchronization data
  _robotsync_data.synced = 0;
  _robotsync_data.evtsource = NULL;
  _robotsync_data.successflags = 0;
  _robotsync_data.errorflags = 0;
  _robotsync_data.fbcanfilter.mask.std.id = 0x7FF;
  _robotsync_data.fbcanfilter.mask.std.ide = CAN_IDE_STD;
  _robotsync_data.fbcanfilter.mask.std.rtr = CAN_RTR_DATA;
  _robotsync_data.fbcanfilter.filter.std.id = AMIRODEFAULT_SYNC_CANID;
  _robotsync_data.fbcanfilter.filter.std.ide = CAN_IDE_STD;
  _robotsync_data.fbcanfilter.filter.std.rtr = CAN_RTR_DATA;
  _robotsync_data.fbcanfilter.callback = _robotsync_cancb;
  _robotsync_data.fbcanfilter.cbparams = &_robotsync_data;
  urtSyncInit(&robotsync, _robotsync_cb, &_robotsync_data);
  aosFBCanAddFilter(&MODULE_HAL_CAN, &_robotsync_data.fbcanfilter);

  return;
}

/** @} */
