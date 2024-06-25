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
 * @file    aos_sssp.c
 * @brief   SSSP related code.
 */

#include <amiroos.h>

#if (AMIROOS_CFG_SSSP_ENABLE == true) || defined(__DOXYGEN__)

#include <math.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

#if ((AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) && (AMIROOS_CFG_SSSP_MASTER != true) && \
     (AMIROOS_CFG_PROFILE == true)) || defined(__DOXYGEN__)

/**
 * @brief   Weighting factor for smoothing the @p _syncskew value.
 *
 * @param[in] dt    Differnce between _syncskew value and latest offset reading.
 */
#if !defined(SYNCSKEW_SMOOTHFACTOR) || defined(__DOXYGEN__)
#define SYNCSKEW_SMOOTHFACTOR(dt)               ((fabsf(dt) < (float)chTimeI2US(CH_CFG_ST_TIMEDELTA)) ? (fabsf(dt) / (float)chTimeI2US(CH_CFG_ST_TIMEDELTA)) : 1.0f)
#endif

#endif /* (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) && (AMIROOS_CFG_SSSP_MASTER != true) && (AMIROOS_CFG_PROFILE == true) */

/******************************************************************************/
/* EXPORTED VARIABLES                                                         */
/******************************************************************************/

/******************************************************************************/
/* LOCAL TYPES                                                                */
/******************************************************************************/

/**
 * @brief   Available BCB messages.
 */
enum {
  AOS_SSSP_BCBMESSAGE_INVALID   = 0,  /**< Invalid BCB message.         */
  AOS_SSSP_BCBMESSAGE_MSIINIT   = 1,  /**< MSI initialization message.  */
  AOS_SSSP_BCBMESSAGE_MSIID     = 2,  /**< MSI identifier message.      */
  AOS_SSSP_BCBMESSAGE_MSIABORT  = 3,  /**< MSI abort message.           */
};

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/

/**
 * @brief   Pointer to the system uptime.
 */
static aos_timestamp_t* _uptime;

#if (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) || defined(__DOXYGEN__)
#if (AMIROOS_CFG_SSSP_MASTER == true) || defined(__DOXYGEN__)

/**
 * @brief   Timer to drive the S signal for system wide time synchronization
 *          during operation phase.
 */
static virtual_timer_t _synctimer;

/**
 * @brief   Last uptime of system wide time synchronization.
 */
static aos_timestamp_t _synctime;

#endif /* (AMIROOS_CFG_SSSP_MASTER == true) */

#if ((AMIROOS_CFG_SSSP_MASTER != true) && (AMIROOS_CFG_PROFILE == true)) ||     \
    defined(__DOXYGEN__)

/**
 * @brief   Offset between local clock and system wide synchronization signal.
 */
static float _syncskew;

#endif /* (AMIROOS_CFG_SSSP_MASTER != true) && (AMIROOS_CFG_PROFILE == true) */
#endif /* (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) */

/**
 * @brief   A timer event based delays.
 * @details This timer must not be an AMiRo-OS timer, since delays occurr before
 *          system is initialized.
 */
static virtual_timer_t _delayTimer;

/**
 * @brief   Event source for the delay timer.
 */
static event_source_t _delayEventSource;

/**
 * @brief   Event listener for the delay event.
 */
static event_listener_t _delayEventListener;

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

#if (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) || defined(__DOXYGEN__)
#if (AMIROOS_CFG_SSSP_MASTER != true) || defined(__DOXYGEN__)

/**
 * @brief   Callback function for the S signal interrupt.
 *
 * @param[in] args   Pointer to the GPIO line identifier.
 */
static void _aosSsspGpioCallbackSSignal(void *args)
{
  aosDbgCheck((args != NULL) && (*((ioline_t*)args) != PAL_NOLINE) && (PAL_PAD(*((ioline_t*)args)) < sizeof(eventflags_t) * 8));

  chSysLockFromISR();

  // if the system is in operation phase
  if (aos.sssp.stage == AOS_SSSP_STAGE_OPERATION) {
    // read signal S
    apalControlGpioState_t s;
    apalControlGpioGet(moduleSsspSignalS(), &s);
    // if S was toggled from on to off
    if (s == APAL_GPIO_OFF) {
      // get current uptime
      aos_timestamp_t t;
      aosSysGetUptimeX(&t);
      // align the uptime with the synchronization period
      aosTimestampSet(t, aosTimestampGet(t) % AMIROOS_CFG_SSSP_SYSSYNCPERIOD);
      if (aosTimestampGet(t) < AMIROOS_CFG_SSSP_SYSSYNCPERIOD / 2) {
        aosTimestampSet(*_uptime, aosTimestampGet(*_uptime) - aosTimestampGet(t));
#if (AMIROOS_CFG_PROFILE == true)
        if (isnan(_syncskew)) {
          _syncskew = aosTimestampGet(t);
        } else {
          const float skewdiff = (float)aosTimestampGet(t) - _syncskew;
          const float factor = SYNCSKEW_SMOOTHFACTOR(skewdiff);
          _syncskew = ((1.0f - factor) * _syncskew) + (factor * (float)aosTimestampGet(t));
        }
#endif /* (AMIROOS_CFG_PROFILE == true) */
      } else {
        aosTimestampSet(t, AMIROOS_CFG_SSSP_SYSSYNCPERIOD - aosTimestampGet(t));
        aosTimestampAdd(*_uptime, t);
#if (AMIROOS_CFG_PROFILE == true)
        if (isnan(_syncskew)) {
          _syncskew = aosTimestampGet(t) * -1.0f;
        } else {
          const float skewdiff = (aosTimestampGet(t) * -1.0f) - _syncskew;
          const float factor = SYNCSKEW_SMOOTHFACTOR(skewdiff);
          _syncskew = ((1.0f - factor) * _syncskew) - (factor * aosTimestampGet(t));
        }
#endif /* (AMIROOS_CFG_PROFILE == true) */
      }
      // broadcast synchronization event
      chEvtBroadcastFlagsI(&aos.sssp.syncsource, AOS_SSSP_EVENTFLAGS_SYNC_SLAVE |
                           ((aosTimestampGet(t) > chTimeI2US(CH_CFG_ST_TIMEDELTA)) ? AOS_SSSP_EVENTFLAGS_SYNC_CRITICALSKEW : 0));
    }
  }

  // broadcast GPIO event
  chEvtBroadcastFlagsI(&aos.events.gpio, AOS_GPIOEVENT_FLAG(PAL_PAD(*((ioline_t*)args))));
  chSysUnlockFromISR();

  return;
}

#endif /* (AMIROOS_CFG_SSSP_MASTER != true) */

#if (AMIROOS_CFG_SSSP_MASTER == true) || defined (__DOXYGEN__)

/**
 * @brief   Periodic system synchronization callback function.
 * @details Toggles the S signal and reconfigures the system synchronization
 *          timer.
 *
 * @param[in] timer   Pointer to the calling virtual timer.
 * @param[in] par     Unused parameters.
 */
static void _aosSsspSyncTimerCallback(virtual_timer_t* timer, void* par)
{
  (void)timer;
  (void)par;

  // local variables
  apalControlGpioState_t state;
  aos_timestamp_t uptime;

  chSysLockFromISR();
  // read and toggle signal S
  apalControlGpioGet(moduleSsspSignalS(), &state);
  apalGpioToggle(moduleSsspSignalS()->gpio);
  // if S was toggled from off to on
  if (state != APAL_GPIO_ON) {
    // reconfigure the timer precisely, because the logically falling edge (next interrupt) snychronizes the system time
    aosTimestampAddLongInterval(_synctime, AMIROOS_CFG_SSSP_SYSSYNCPERIOD);
    aosSysGetUptimeX(&uptime);
    chVTSetI(&_synctimer, chTimeUS2I((time_usecs_t)aosTimestampDiff(uptime, _synctime)), _aosSsspSyncTimerCallback, NULL);
  }
  // if S was toggled from on to off
  else /* if (state == APAL_GPIO_OFF) */ {
    // reconfigure the timer (lazy)
    chVTSetI(&_synctimer, chTimeUS2I(AMIROOS_CFG_SSSP_SYSSYNCPERIOD / 2), _aosSsspSyncTimerCallback, NULL);
    // broadcast synchronization event
    chEvtBroadcastFlagsI(&aos.sssp.syncsource, AOS_SSSP_EVENTFLAGS_SYNC_MASTER);
  }
  chSysUnlockFromISR();

  return;
}

/**
 * @brief   Start the timer that toggles S for synchronization of the system.
 * @note    Must be called from a locked context.
 */
void _aosSsspSyncTimerStartS(void)
{
  chDbgCheckClassS();

  // start synchronization timer (lazy)
  chVTSetI(&_synctimer, chTimeUS2I(AMIROOS_CFG_SSSP_SYSSYNCPERIOD / 2), _aosSsspSyncTimerCallback, NULL);

  return;
}

#endif /* (AMIROOS_CFG_SSSP_MASTER == true) */
#endif /* (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) */

/**
 * @brief   General callback function to be used for any local timers.
 *
 * @param[in] timer   Pointer to the calling virtual timer.
 * @param[in] par     A pointer to an @p event_source_t to be fired.
 */
static void _aosSsspTimerCallback(virtual_timer_t* timer, void* par)
{
  aosDbgCheck(par != NULL);

  (void)timer;

  chSysLockFromISR();
  chEvtBroadcastI((event_source_t*)par);
  chSysUnlockFromISR();

  return;
}

/**
 * @brief   Waits for the S signal to switch to the desired state.
 *
 * @param[in]   listener      Pointer to the GPIO event listener to be used.
 * @param[in]   signalstate   Desired state of the S signal ti be waited for.
 * @param[out]  received      Output variable to store the received event mask
 *                            to.
 *
 * @return  Status, indicating whether the expected event was received.
 * @retval AOS_SUCCESS  The desired event has occurred.
 * @retval AOS_FAILURE  Some other event has occurred.
 */
aos_status_t _aosSsspWaitForS(event_listener_t* listener, apalControlGpioState_t signalstate, eventmask_t* received)
{
  aosDbgCheck(listener != NULL);
  aosDbgCheck(received != NULL);

  // local variables
  aos_status_t status = AOS_FAILURE;

  // wait for the next event (do not apply any filters in order not to miss any events)
  *received = chEvtWaitOne(ALL_EVENTS);
  // if the correct event was triggered
  if (*received & listener->events) {
    apalControlGpioState_t s;
    apalControlGpioGet(moduleSsspSignalS(), &s);
    chSysLock();
    // only check for the expected event
    if ((listener->flags & moduleSsspEventflagS()) && (s == signalstate)) {
      // unset the expected flags but keep any other ones
      listener->flags &= ~moduleSsspEventflagS();
      status = AOS_SUCCESS;
    }
    // if no further flags are set
    if (listener->flags == 0) {
      // clear event
      *received &= ~(listener->events);
    }
    chSysUnlock();
  }

  return status;
}

/**
 * @brief   Uses the local delay timer to setup a timed event and waits for it
 *          to fire.
 *
 * @param[in]   dt          Delay time in microseconds to be set to the timer.
 * @param[in]   mask        Event mask to be used for the delay timer.
 * @param[out]  received    Output variable to store the received event mask to.
 *
 * @return  Status, indicating whether the expected event (delay timer) was
 *          received.
 * @retval AOS_SUCCESS  The expected event (delay timer) was received.
 * @retval AOS_FAILURE  Some other event was received.
 */
aos_status_t _aosSsspDelay(uint32_t dt, eventmask_t mask, eventmask_t* received)
{
  aosDbgCheck(dt != TIME_IMMEDIATE);
  aosDbgCheck(mask != 0);
  aosDbgCheck(received != NULL);

  // arm the delay timer once
  if (!chVTIsArmed(&_delayTimer)) {
    chEvtRegisterMask(&_delayEventSource, &_delayEventListener, mask);
    chVTSet(&_delayTimer, chTimeUS2I(dt), _aosSsspTimerCallback, &_delayEventSource);
  }

  // wait for any event to occur (do not apply any filters in order not to miss any events)
  *received = chEvtWaitOne(ALL_EVENTS);

  // if the timer event was received, cleanup
  if (*received & _delayEventListener.events) {
    chEvtUnregister(&_delayEventSource, &_delayEventListener);
    *received &= ~(_delayEventListener.events);
    return AOS_SUCCESS;
  }

  return AOS_FAILURE;
}

#if (AMIROOS_CFG_SSSP_MSI == true) || defined(__DOXYGEN__)

/**
 * @brief   Serialize a BCB message.
 * @details The individual primitives are serialized in big-endian fashion.
 *
 * @param[out]  buffer    Buffer to write the serialized message to.
 * @param[in]   message   The message to be serialized.
 *
 * @return  Status, indicating whether the message could be serialized.
 * @retval AOS_SUCCESS  Message was serialized and stored to the buffer.
 * @retval AOS_FAILURE  Message could not be serialized.
 */
aos_status_t _aosSsspSerializeBcbMessage(uint8_t* buffer, aos_ssspbcbmessage_t message)
{
  aosDbgCheck(buffer != NULL);

  // encode the message depending on its type
  switch (message.type) {
    case AOS_SSSP_BCBMESSAGE_MSIINIT:
    case AOS_SSSP_BCBMESSAGE_MSIABORT:
    {
      buffer[0] = message.type;
      return AOS_SUCCESS;
    }
    case AOS_SSSP_BCBMESSAGE_MSIID:
    {
      buffer[0] = message.type;
      for (size_t byte = 0; byte < sizeof(aos_ssspmoduleid_t); ++byte) {
        buffer[byte+1] = (message.payload.id >> ((sizeof(aos_ssspmoduleid_t) - (byte+1)) * 8)) & 0xFF;
      }
      return AOS_SUCCESS;
    }
    default:
    {
      return AOS_FAILURE;
    }
  }
}

/**
 * @brief   Deserialize a BCB message.
 * @details The individual primitives must be serialized in big-endian fashion.
 *
 * @param[out]  message   Message object to be filled.
 * @param[in]   buffer    Buffer holding the serialized message.
 *
 * @return  Status, indicating whether the buffer could be deserialized.
 * @retval AOS_SUCCESS  Buffer was deserialized and stored to the message.
 * @retval AOS_FAILURE  Buffer could not be deserialized.
 */
aos_status_t _aosSsspDeserializeBcbMessage(aos_ssspbcbmessage_t* message, uint8_t* buffer)
{
  aosDbgCheck(message != NULL);
  aosDbgCheck(buffer != NULL);

  // only decode the first byte, which contains the message type
  message->type = buffer[0];

  // decode the message depedning on its type
  switch (message->type) {
    case AOS_SSSP_BCBMESSAGE_MSIINIT:
    case AOS_SSSP_BCBMESSAGE_MSIABORT:
    {
      return AOS_SUCCESS;
    }
    case AOS_SSSP_BCBMESSAGE_MSIID:
    {
      message->payload.id = 0;
      for (size_t byte = 0; byte < sizeof(aos_ssspmoduleid_t); ++byte) {
        message->payload.id |= (aos_ssspmoduleid_t)(buffer[byte+1] << ((sizeof(aos_ssspmoduleid_t) - (byte+1)) * 8));
      }
      return AOS_SUCCESS;
    }
    default:
    {
      message->type = AOS_SSSP_BCBMESSAGE_INVALID;
      return AOS_FAILURE;
    }
  }
}

/**
 * @brief   Common outro of the module stack initialization (MSI).
 * @details This function must be called on failure, success and if MSI is
 *          skipped.
 *
 * @param[in] msidata   Pointer to the temporary data structure used during MSI.
 */
void _aosSsspMsiOutro(aos_ssspmsidata_t* msidata)
{
  aosDbgCheck(msidata != NULL);

  // local variables
  eventmask_t mask = 0;

  // start the internal uptime aggregation
  chSysLock();
  aosSysStartUptimeS();
#if (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) && (AMIROOS_CFG_SSSP_MASTER == true)
  // start toggling S for system synchronization
  _aosSsspSyncTimerStartS();
#endif /* (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) && (AMIROOS_CFG_SSSP_MASTER == true) */
  chSysUnlock();

  // reset delay timer
  chVTReset(&_delayTimer);
  // unregister delay events
  mask |= _delayEventListener.events;
  chEvtUnregister(&_delayEventSource, &_delayEventListener);

#if (AMIROOS_CFG_SSSP_STACK_END != true)
  // deactivate UP signal
  apalControlGpioSet(moduleSsspSignalUP(), APAL_GPIO_OFF);
#endif /* (AMIROOS_CFG_SSSP_STACK_END != true) */

  // reset timeout timer
  chVTReset(&msidata->timeout.timer);
  // unregister timeout events
  mask |= msidata->timeout.eventlistener.events;
  chEvtUnregister(&msidata->timeout.eventsource, &msidata->timeout.eventlistener);

  // discard any pending timing events
  chEvtWaitAnyTimeout(mask, TIME_IMMEDIATE);

  // proceed to operation phase
  aos.sssp.stage = AOS_SSSP_STAGE_OPERATION;

  return;
}

#endif /* (AMIROOS_CFG_SSSP_MSI == true) */

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup core_sssp
 * @{
 */

/**
 * @brief   Initialize all SSSP related data.
 *
 * @param[in] system_uptime   Pointer to the system uptime instance.
 */
void aosSsspInit(aos_timestamp_t* system_uptime)
{
  aosDbgCheck(system_uptime != NULL);

#if (AMIROOS_CFG_SSSP_STARTUP == true)
  // AMiRo-OS has to perform the basic initialization
  aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_1_1;
#else /* (AMIROOS_CFG_SSSP_STARTUP == true) */
  // basic initialization was already performed by a bootloader
  aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_2_1;
#endif /* (AMIROOS_CFG_SSSP_STARTUP == true) */

  // module ID is initialized as 'invalid'
  aos.sssp.moduleId = AOS_SSSP_MODULEID_INVALID;
#if (AMIROOS_CFG_SSSP_MSI == true)
  aos.sssp.modulecount = 0;
#endif /* (AMIROOS_CFG_SSSP_MSI == true) */
#if (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0)
  // initialize synchronization event source
  chEvtObjectInit(&aos.sssp.syncsource);
#endif /* (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) */

#if (AMIROOS_CFG_DBG == true)
  // check all SSSP signals for correct state
  {
    apalControlGpioState_t state;
    apalControlGpioGet(moduleSsspSignalS(), &state);
    aosDbgAssert(state == APAL_GPIO_ON);
    apalControlGpioGet(moduleSsspSignalPD(), &state);
    aosDbgAssert(state == APAL_GPIO_OFF);
#if (AMIROOS_CFG_SSSP_MSI == true)
#if (AMIROOS_CFG_SSSP_STACK_START != true)
    apalControlGpioGet(moduleSsspSignalDN(), &state);
    aosDbgAssert(state == APAL_GPIO_OFF);
#endif /* (AMIROOS_CFG_SSSP_STACK_START != true) */
#if (AMIROOS_CFG_SSSP_STACK_END != true)
    apalControlGpioGet(moduleSsspSignalUP(), &state);
    aosDbgAssert(state == APAL_GPIO_OFF);
#endif /* (AMIROOS_CFG_SSSP_END_START != true) */
#endif /* (AMIROOS_CFG_SSSP_MSI == true) */
  }
#endif /* (AMIROOS_CFG_DBG == true) */

  // initialize static variables
  _uptime = system_uptime;
#if (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0)
#if (AMIROOS_CFG_SSSP_MASTER == true)
  chVTObjectInit(&_synctimer);
  aosTimestampSet(_synctime, 0);
#endif /* (AMIROOS_CFG_SSSP_MASTER == true) */
#if (AMIROOS_CFG_SSSP_MASTER != true) && (AMIROOS_CFG_PROFILE == true)
  _syncskew = NAN;
#endif /* (AMIROOS_CFG_SSSP_MASTER != true) && (AMIROOS_CFG_PROFILE == true) */
#endif /* (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) */
  chVTObjectInit(&_delayTimer);
  chEvtObjectInit(&_delayEventSource);

  // signal interrupt setup
  palSetLineCallback(moduleSsspSignalPD()->gpio->line, aosSysGetStdGpioCallback(), &moduleSsspSignalPD()->gpio->line);
  palEnableLineEvent(moduleSsspSignalPD()->gpio->line, APAL2CH_EDGE(moduleSsspSignalPD()->meta.edge));
#if (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) && (AMIROOS_CFG_SSSP_MASTER != true)
  palSetLineCallback(moduleSsspSignalS()->gpio->line, _aosSsspGpioCallbackSSignal, &moduleSsspSignalS()->gpio->line);
#else /* (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) && (AMIROOS_CFG_SSSP_MASTER != true) */
  palSetLineCallback(moduleSsspSignalS()->gpio->line, aosSysGetStdGpioCallback(), &moduleSsspSignalS()->gpio->line);
#endif /* (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) && (AMIROOS_CFG_SSSP_MASTER != true) */
  palEnableLineEvent(moduleSsspSignalS()->gpio->line, APAL2CH_EDGE(moduleSsspSignalS()->meta.edge));
#if (AMIROOS_CFG_SSSP_MSI == true)
#if (AMIROOS_CFG_SSSP_STACK_START != true)
  palSetLineCallback(moduleSsspSignalDN()->gpio->line, aosSysGetStdGpioCallback(), &moduleSsspSignalDN()->gpio->line);
  palEnableLineEvent(moduleSsspSignalDN()->gpio->line, APAL2CH_EDGE(moduleSsspSignalDN()->meta.edge));
#endif /* (AMIROOS_CFG_SSSP_STACK_START != true) */
#if (AMIROOS_CFG_SSSP_STACK_END != true)
  palSetLineCallback(moduleSsspSignalUP()->gpio->line, aosSysGetStdGpioCallback(), &moduleSsspSignalUP()->gpio->line);
  palEnableLineEvent(moduleSsspSignalUP()->gpio->line, APAL2CH_EDGE(moduleSsspSignalUP()->meta.edge));
#endif /* (AMIROOS_CFG_SSSP_STACK_END != true) */
#endif /* (AMIROOS_CFG_SSSP_MSI == true) */

  return;
}

/**
 * @brief   Proceed to the next SSSP stage.
 * @note    Calling this function when in operation phase is invalid.
 *          The function aosSsspShutdownInit() must be used instead.
 *
 * @param[in]   listener  An optional listener, in case the system has to wait
 *                        for a specific event.
 * @param[in]   mask      If the stage transition involves a timer event
 *                        (handled internally by this function), the given input
 *                        value of this parameter defines the mask to be used
 *                        for that event. In case a listener was specified, the
 *                        according mask is retrieved from the listener but this
 *                        parameter is still used as input for sanity checks.
 * @param[out]  received  Output variable to store the received event mask to.
 *
 * @return  Status, indicating whether proceeding in the protocol was
 *          successful.
 */
aos_status_t aosSsspProceed(event_listener_t* listener, eventmask_t mask, eventmask_t* received)
{
  // local variables
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
  static aos_ssspstage_t laststage = AOS_SSSP_STAGE_UNDEFINED;
#pragma GCC diagnostic pop
  aos_ssspstage_t nextstage = aos.sssp.stage;

  // behaviour depends on current SSSP stage of the system
  switch (aos.sssp.stage) {
#if (AMIROOS_CFG_SSSP_STARTUP == true)

    /*
     * Deactivate S signal.
     * Master node delays execution by one period AOS_SSSP_DELAY.
     */
    case AOS_SSSP_STAGE_STARTUP_1_1:
    {
#if (AMIROOS_CFG_SSSP_MASTER == true)
      aosDbgCheck(listener == NULL);
      aosDbgCheck(mask != 0);
      aosDbgCheck(received != NULL);

      // delay execution and deactivate the S signal on success
      if (_aosSsspDelay(AOS_SSSP_DELAY, mask, received) == AOS_SUCCESS) {
        apalControlGpioSet(moduleSsspSignalS(), APAL_GPIO_OFF);
        nextstage = AOS_SSSP_STAGE_STARTUP_1_2;
      }
#else /* (AMIROOS_CFG_SSSP_MASTER == true) */
      aosDbgCheck(listener == NULL);
      aosDbgCheck(mask == 0);
      aosDbgCheck(received == NULL);

      // deactivate S signal and proceed
      apalControlGpioSet(moduleSsspSignalS(), APAL_GPIO_OFF);
      nextstage = AOS_SSSP_STAGE_STARTUP_1_2;
#endif /* (AMIROOS_CFG_SSSP_MASTER == true) */
      break;
    }

    /*
     * Wait for the S signal to become inactive.
     */
    case AOS_SSSP_STAGE_STARTUP_1_2:
    {
      aosDbgCheck(listener != NULL);
      aosDbgCheck(mask == listener->events);
      aosDbgCheck(received != NULL);

      // wait for the S signal to become inactive and proceed on success
      if (_aosSsspWaitForS(listener, APAL_GPIO_OFF, received) == AOS_SUCCESS) {
        nextstage = AOS_SSSP_STAGE_STARTUP_1_3;
      }
      break;
    }

    /*
     * Wait for the S signal to be activated by the master module.
     * The master delays execution by one period AOS_SSSP_DELAY.
     */
    case AOS_SSSP_STAGE_STARTUP_1_3:
    {
#if (AMIROOS_CFG_SSSP_MASTER == true)
      aosDbgCheck(listener == NULL);
      aosDbgCheck(mask != 0);
      aosDbgCheck(received != NULL);

      // delay execution and deactivate the S signal on success
      if (_aosSsspDelay(AOS_SSSP_DELAY, mask, received) == AOS_SUCCESS) {
        apalControlGpioSet(moduleSsspSignalS(), APAL_GPIO_ON);
        nextstage = AOS_SSSP_STAGE_STARTUP_2_1;
      }
#else /* (AMIROOS_CFG_SSSP_MASTER == true) */
      aosDbgCheck(listener != NULL);
      aosDbgCheck(mask == listener->events);
      aosDbgCheck(received != NULL);

      // wait for the S signal to become active and activate it as well and proceed on success
      if (_aosSsspWaitForS(listener, APAL_GPIO_ON, received) == AOS_SUCCESS) {
        apalControlGpioSet(moduleSsspSignalS(), APAL_GPIO_ON);
        nextstage = AOS_SSSP_STAGE_STARTUP_2_1;
      }
#endif /* (AMIROOS_CFG_SSSP_MASTER == true) */
      break;
    }

#endif /* (AMIROOS_CFG_SSSP_STARTUP == true) */

    /*
     * Deactivate S signal.
     * Master node delays execution by one period AOS_SSSP_DELAY
     */
    case AOS_SSSP_STAGE_STARTUP_2_1:
    {
#if (AMIROOS_CFG_SSSP_MASTER == true)
      aosDbgCheck(listener == NULL);
      aosDbgCheck(mask != 0);
      aosDbgCheck(received != NULL);

      // delay execution and deactivate the S signal on success
      if (_aosSsspDelay(AOS_SSSP_DELAY, mask, received) == AOS_SUCCESS) {
        apalControlGpioSet(moduleSsspSignalS(), APAL_GPIO_OFF);
        nextstage = AOS_SSSP_STAGE_STARTUP_2_2;
      } else {
        aosDbgPrintf(".");
      }
#else /* (AMIROOS_CFG_SSSP_MASTER == true) */
      aosDbgCheck(listener == NULL);
      aosDbgCheck(mask == 0);
      aosDbgCheck(received == NULL);

      // deactivate S signal and proceed
      apalControlGpioSet(moduleSsspSignalS(), APAL_GPIO_OFF);
      nextstage = AOS_SSSP_STAGE_STARTUP_2_2;
      mask = 0;
#endif /* (AMIROOS_CFG_SSSP_MASTER == true) */
      break;
    }

    /*
     * Wait for the S signal to become inactive.
     * When proceeding to operation phase, the master starts toggling S for
     * synchronization
     */
    case AOS_SSSP_STAGE_STARTUP_2_2:
    {
      aosDbgCheck(listener != NULL);
      aosDbgCheck(mask == listener->events);
      aosDbgCheck(received != NULL);

      // wait for the S signal to become inactive and proceed on success
      if (_aosSsspWaitForS(listener, APAL_GPIO_OFF, received) == AOS_SUCCESS) {
#if (AMIROOS_CFG_SSSP_MSI == true)
        nextstage = AOS_SSSP_STAGE_STARTUP_3;
#else /* (AMIROOS_CFG_SSSP_MSI == true) */
        // start the internal uptime aggregation
        chSysLock();
        aosSysStartUptimeS();
#if (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) && (AMIROOS_CFG_SSSP_MASTER == true)
        // start toggling S for system synchronization
        _aosSsspSyncTimerStartS();
#endif /* (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) && (AMIROOS_CFG_SSSP_MASTER == true) */
        chSysUnlock();
        nextstage = AOS_SSSP_STAGE_OPERATION;
#endif /* (AMIROOS_CFG_SSSP_MSI == true) */
      }
      break;
    }

    /*
     * PLEASE NOTE:
     * Stage transitions during MSI are not hanlded by thins function but are
     * executed in aosSsspMsi().
     */

    /*
     * Invalid operation.
     * Shutdon must be initiatid via the aosSsspShutdownInit() function.
     */
    case AOS_SSSP_STAGE_OPERATION:
    {
      aosDbgAssertMsg(false, "in order to exit operation phase, aosSsspShutdownInit() function must be used");
      break;
    }

    /*
     * Delay execution by one perion AOS_SSSP_DELAY, deactivate the SYNC signal
     * and proceed.
     * NOTE: Actually only the module that initiated the shutdown has to delay
     *       execution here. For the sake of simlicity though, each module
     *       delays execution.
     */
    case AOS_SSSP_STAGE_SHUTDOWN_1_2:
    {
      aosDbgCheck(listener == NULL);
      aosDbgCheck(mask == 0);
      aosDbgCheck(received == NULL);

      // delay execution
      aosThdUSleep(AOS_SSSP_DELAY);
      // deactivate S signal
      apalControlGpioSet(moduleSsspSignalS(), APAL_GPIO_OFF);
      nextstage = AOS_SSSP_STAGE_SHUTDOWN_1_3;
      break;
    }

#if (AMIROOS_CFG_SSSP_SHUTDOWN == true)

    /*
     * Wait for S signal to become inactive.
     */
    case AOS_SSSP_STAGE_SHUTDOWN_1_3:
    {
      aosDbgCheck(listener != NULL);
      aosDbgCheck(mask == listener->events);
      aosDbgCheck(received != NULL);

      // wait for the S signal to become inactive and proceed on success
      if (_aosSsspWaitForS(listener, APAL_GPIO_OFF, received) == AOS_SUCCESS) {
        nextstage = AOS_SSSP_STAGE_SHUTDOWN_2_1;
      }
      break;
    }

    /*
     * Evaluate PD signal to determine whether a shutdown or restart is
     * requested. The logical state is returned by the function call.
     */
    case AOS_SSSP_STAGE_SHUTDOWN_2_1:
    {
      aosDbgCheck(listener == NULL);
      aosDbgCheck(mask == 0);
      aosDbgCheck(received != NULL);

      // evaluate PD signal
      apalControlGpioState_t pd;
      apalControlGpioGet(moduleSsspSignalPD(), &pd);
      *received = (pd == APAL_GPIO_ON) ? (eventmask_t)~0 : 0;
      nextstage = AOS_SSSP_STAGE_SHUTDOWN_2_2;
      break;
    }

    /*
     * Proceed with no further actions required.
     */
    case AOS_SSSP_STAGE_SHUTDOWN_2_2:
    {
      aosDbgCheck(listener == NULL);
      aosDbgCheck(mask == 0);
      aosDbgCheck(received == NULL);

      nextstage = AOS_SSSP_STAGE_SHUTDOWN_2_3;
      break;
    }

#endif /* (AMIROOS_CFG_SSSP_SHUTDOWN == true) */

    default:
    {
      // this case must never occur!
      aosDbgAssertMsg(false, "Invalid SSSP stage tranition occurred");
      break;
    }
  } /* end of switch (aos.sssp.stage) */

  // outro and return
  laststage = aos.sssp.stage;
  const aos_status_t status = (nextstage > aos.sssp.stage) ? AOS_SUCCESS : AOS_FAILURE;
  aos.sssp.stage = nextstage;

  return status;
}

#if (AMIROOS_CFG_SSSP_MSI == true) || defined(__DOXYGEN__)

/**
 * @brief   Initialize MSI data structure.
 *
 * @param[out]  msidata       Pointer to the MSI data structure to be
 *                            initialized.
 * @param[in]   delayMask     Event maks used for delay events.
 * @param[in]   timeoutMask   Event mask used for timeout events.
 * @param[in]   gpioListener  Event listener for GPIO events.
 */
void aosSsspMsiInit(aos_ssspmsidata_t* msidata, eventmask_t delayMask, eventmask_t timeoutMask, event_listener_t* gpioListener)
{
  aosDbgCheck(msidata != NULL);
  aosDbgCheck(gpioListener != NULL);

  // register delay events
  chEvtRegisterMask(&_delayEventSource, &_delayEventListener, delayMask);

  // initialize timeout related data
  chVTObjectInit(&msidata->timeout.timer);
  chEvtObjectInit(&msidata->timeout.eventsource);
  chEvtRegisterMask(&msidata->timeout.eventsource, &msidata->timeout.eventlistener, timeoutMask);

  // set GPIO event information
  msidata->signals.eventlistener = gpioListener;

  // initialize BCB related data
  msidata->bcb.lastid = AOS_SSSP_MODULEID_INVALID;

  return;
}

/**
 * @brief   Execute module stack initialization (MSI).
 *
 * @param[in]   msidata   Data required for MSI.
 * @param[in]   bcbdata   BCB specific data of @p NULL.
 * @param[out]  received  Output variable to store the received event mask to.
 */
void aosSsspMsi(aos_ssspmsidata_t* msidata, void* bcbdata, eventmask_t* received)
{
  aosDbgCheck(msidata != NULL);
  aosDbgCheck(received != NULL);

  /*
   * execute anything that does not depend on BCB messages or events
   */
  switch (aos.sssp.stage) {
    case AOS_SSSP_STAGE_STARTUP_3:
    case AOS_SSSP_STAGE_STARTUP_3_1:
    {
      aosDbgPrintf("SSSP: MSI init\n");
      // setup timeout timer
      chVTSet(&msidata->timeout.timer, chTimeUS2I(AOS_SSSP_TIMEOUT), _aosSsspTimerCallback, &msidata->timeout.eventsource);
      // proceed
#if (AMIROOS_CFG_SSSP_MASTER == true)
      aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_1_BROADCASTINIT;
#else /* (AMIROOS_CFG_SSSP_MASTER == true) */
      aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_1_WAITFORINIT;
#endif /* (AMIROOS_CFG_SSSP_MASTER == true) */
      break;
    }

#if (AMIROOS_CFG_SSSP_MASTER == true)
    case AOS_SSSP_STAGE_STARTUP_3_1_BROADCASTINIT:
    {
      aosDbgPrintf("%u\tsend INIT\t", chVTGetSystemTime());
      // send init command
      msidata->bcb.message.type = AOS_SSSP_BCBMESSAGE_MSIINIT;
      _aosSsspSerializeBcbMessage(msidata->bcb.buffer, msidata->bcb.message);
      if (moduleSsspBcbTransmit(msidata->bcb.buffer, sizeof(aos_ssspbcbmessage_t), bcbdata) == AOS_SSSP_BCB_SUCCESS) {
        aosDbgPrintf("ok\tS+\t");
        // activate S
        apalControlGpioSet(moduleSsspSignalS(), APAL_GPIO_ON);
        // restart timeout timer
        chVTSet(&msidata->timeout.timer, chTimeUS2I(AOS_SSSP_TIMEOUT), _aosSsspTimerCallback, &msidata->timeout.eventsource);
        // proceed
#if (AMIROOS_CFG_SSSP_STACK_START == true)
        aosDbgPrintf("->start\n");
        aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_2;
#else /* (AMIROOS_CFG_SSSP_STACK_START == true) */
        aosDbgPrintf("->count\n");
        aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_3_WAIT4EVENT;
#endif /* (AMIROOS_CFG_SSSP_STACK_START == true) */
      } else {
        aosDbgPrintf("fail\t->retry\n");
        // retry in next iteration
      }
      break;
    }
#endif /* (AMIROOS_CFG_SSSP_MASTER == true) */

#if (AMIROOS_CFG_SSSP_STACK_START == true)
    case AOS_SSSP_STAGE_STARTUP_3_2:
    {
      aosDbgPrintf("%u\tstart\t", chVTGetSystemTime());
      // set the own module ID
      aos.sssp.moduleId = 1;
      // broadcast the new ID
      msidata->bcb.message.type = AOS_SSSP_BCBMESSAGE_MSIID;
      msidata->bcb.message.payload.id = aos.sssp.moduleId;
      aosDbgPrintf("bc ID (%u)\t", msidata->bcb.message.payload.id);
      _aosSsspSerializeBcbMessage(msidata->bcb.buffer, msidata->bcb.message);
      if (moduleSsspBcbTransmit(msidata->bcb.buffer, sizeof(aos_ssspbcbmessage_t), bcbdata) == AOS_SSSP_BCB_SUCCESS) {
        aosDbgPrintf("ok\t->count\n");
        // restart timeout timer
        chVTSet(&msidata->timeout.timer, chTimeUS2I(AOS_SSSP_TIMEOUT), _aosSsspTimerCallback, &msidata->timeout.eventsource);
        // set delay timer so UP will be activated and S deactivated later
        chVTSet(&_delayTimer, chTimeUS2I(AOS_SSSP_DELAY), _aosSsspTimerCallback, &_delayEventSource);
        // store the transmitted ID
        msidata->bcb.lastid = msidata->bcb.message.payload.id;
        // increase the module counter
        ++aos.sssp.modulecount;
        // proceed
        aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_3_WAIT4EVENT;
      } else {
        aosDbgPrintf("fail\t->retry\n");
        // retry next iteration
      }
      break;
    }
#endif /* (AMIROOS_CFG_SSSP_STACK_START == true) */

#if (AMIROOS_CFG_SSSP_STACK_START != true)
    case AOS_SSSP_STAGE_STARTUP_3_3_BROADCASTID:
    {
      // broadcast ID
      msidata->bcb.message.type = AOS_SSSP_BCBMESSAGE_MSIID;
      msidata->bcb.message.payload.id = aos.sssp.moduleId;
      aosDbgPrintf("%u\tbc ID (%u)\t", chVTGetSystemTime(), msidata->bcb.message.payload.id);
      _aosSsspSerializeBcbMessage(msidata->bcb.buffer, msidata->bcb.message);
      if (moduleSsspBcbTransmit(msidata->bcb.buffer, sizeof(aos_ssspbcbmessage_t), bcbdata) == AOS_SSSP_BCB_SUCCESS) {
        aosDbgPrintf("ok\t->count\n");
        // restart timeout timer
        chVTSet(&msidata->timeout.timer, chTimeUS2I(AOS_SSSP_TIMEOUT), _aosSsspTimerCallback, &msidata->timeout.eventsource);
        // set delay timer so UP will be activated and S deactivated later
        chVTSet(&_delayTimer, chTimeUS2I(AOS_SSSP_DELAY), _aosSsspTimerCallback, &_delayEventSource);
        // store the transmitted ID
        msidata->bcb.lastid = msidata->bcb.message.payload.id;
        // increase the module counter
        ++aos.sssp.modulecount;
        // return to receiving
        aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_3_WAIT4EVENT;
      } else {
        aosDbgPrintf("fail\t->retry\n");
        // retry next iteration
      }
      break;
    }
#endif /* (AMIROOS_CFG_SSSP_STACK_START != true) */

    case AOS_SSSP_STAGE_STARTUP_3_4_ABORTION_INIT:
    {
      aosDbgPrintf("%u\tabort\t", chVTGetSystemTime());
      // reset timeout timer
      chVTReset(&msidata->timeout.timer);
      // transmit abort message
      msidata->bcb.message.type = AOS_SSSP_BCBMESSAGE_MSIABORT;
      _aosSsspSerializeBcbMessage(msidata->bcb.buffer, msidata->bcb.message);
      if (moduleSsspBcbTransmit(msidata->bcb.buffer, sizeof(aos_ssspbcbmessage_t), bcbdata) == AOS_SSSP_BCB_SUCCESS) {
        aosDbgPrintf("ok\tS-\t->sync\n");
        // deactivate S
        apalControlGpioSet(moduleSsspSignalS(), APAL_GPIO_OFF);
        // proceed
        aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_4_ABORTION_SYNC;
      } else {
        aosDbgPrintf("fail\t->retry\n");
        // retry next iteration
      }
      break;
    }

    default:
    {
      // all other stages depend on events or BCB messages and are thus handled below
      break;
    }
  }

  /*
   * handle BCB messages
   */
  msidata->bcb.status = moduleSsspBcbReceive(msidata->bcb.buffer, sizeof(aos_ssspbcbmessage_t), bcbdata);
  switch (msidata->bcb.status) {

    /* VALID BCB MESSAGE
     * A message was received via BCB that seems to be plausible.
     * The first BCB message initiates the sequence once.
     * During MSI an arbitrary number of IDs will be received.
     * Abort messages can be received at any time.
     */
    case AOS_SSSP_BCB_SUCCESS:
    {
      aosDbgPrintf("%u\tBCB\t", chVTGetSystemTime());
      // deserialize message and handle types differently
      _aosSsspDeserializeBcbMessage(&msidata->bcb.message, msidata->bcb.buffer);
      switch (msidata->bcb.message.type) {
        // initialization message
        case AOS_SSSP_BCBMESSAGE_MSIINIT:
        {
#if (AMIROOS_CFG_SSSP_MASTER == true)
          aosDbgPrintf("INIT\t->abort\n");
          // initiate abortion
          aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_4_ABORTION_INIT;
#else /* (AMIROOS_CFG_SSSP_MASTER == true) */
          aosDbgPrintf("INIT\t");
          // check for correct SSSP stage
          if (aos.sssp.stage == AOS_SSSP_STAGE_STARTUP_3_1_WAITFORINIT) {
            aosDbgPrintf("S+\t");
            // activate S
            apalControlGpioSet(moduleSsspSignalS(), APAL_GPIO_ON);
            // restart timeout timer
            chVTSet(&msidata->timeout.timer, chTimeUS2I(AOS_SSSP_TIMEOUT), _aosSsspTimerCallback, &msidata->timeout.eventsource);
            // proceed
#if (AMIROOS_CFG_SSSP_STACK_START == true)
            aosDbgPrintf("->start\n");
            aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_2;
#else /* (AMIROOS_CFG_SSSP_STACK_START == true) */
            aosDbgPrintf("->count\n");
            aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_3_WAIT4EVENT;
#endif /* (AMIROOS_CFG_SSSP_STACK_START == true) */
          } else {
            aosDbgPrintf("->abort\n");
            // initiate abortion
            aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_4_ABORTION_INIT;
          }
#endif /* (AMIROOS_CFG_SSSP_MASTER == true) */
          break;
        }
        // ID message
        case AOS_SSSP_BCBMESSAGE_MSIID:
        {
          aosDbgPrintf("ID (%u)\t", msidata->bcb.message.payload.id);
          // check for correct stage and whether ID was increased
          if (aos.sssp.stage == AOS_SSSP_STAGE_STARTUP_3_3_WAIT4EVENT &&
              msidata->bcb.message.payload.id > msidata->bcb.lastid) {
            aosDbgPrintf("ok\n");
            // restart timeout timer
            chVTSet(&msidata->timeout.timer, chTimeUS2I(AOS_SSSP_TIMEOUT), _aosSsspTimerCallback, &msidata->timeout.eventsource);
            // store ID
            msidata->bcb.lastid = msidata->bcb.message.payload.id;
            // increase the module counter
            ++aos.sssp.modulecount;
          } else {
            aosDbgPrintf("fail\t->abort\n");
            // initiate abortion
            aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_4_ABORTION_INIT;
          }
          break;
        }
        // abort message
        case AOS_SSSP_BCBMESSAGE_MSIABORT:
        {
          aosDbgPrintf("ABORT\tS-\t->sync\n");
          // reset timeout timer
          chVTReset(&msidata->timeout.timer);
          // deactivate S
          apalControlGpioSet(moduleSsspSignalS(), APAL_GPIO_OFF);
          // proceed to abort synchronization
          aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_4_ABORTION_SYNC;
          break;
        }
        // invalid message
        default:
        {
          aosDbgPrintf("invalid\t->abort\n");
          // initiate abortion
          aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_4_ABORTION_INIT;
          break;
        }
      }
      return;
    }

    /* INVALID BCB MESSAGE
     * An obviously invalid message was received via BCB.
     */
    case AOS_SSSP_BCB_INVALIDMSG:
    {
      // error but ignore
      aosDbgPrintf("%u\tBCB\tWARNING: unknown message received\n", chVTGetSystemTime());
      break;
    }

    /* NO BCB MESSAGE
     * No message was received via BCB at all.
     */
    case AOS_SSSP_BCB_ERROR:
    {
      // do nothing and proceed
      break;
    }
  }

  /*
   * handle events
   */
  *received = chEvtWaitOneTimeout(ALL_EVENTS, TIME_IMMEDIATE);
  /* TIMEOUT EVENT
   * Timeouts can occur at any time during MSI after they have been initialized.
   * As soon as the module has made it to the 3.4 completion stage, there will
   * be no more timeouts, though.
   */
  if (*received & msidata->timeout.eventlistener.events) {
    aosDbgPrintf("%u\ttimeout\t", chVTGetSystemTime());
    // special cases
    if (aos.sssp.stage == AOS_SSSP_STAGE_STARTUP_3_1_WAITFORINIT ||
        aos.sssp.stage == AOS_SSSP_STAGE_STARTUP_3_4_COMPLETION) {
      // master did not initiate the sequence
      if (aos.sssp.stage == AOS_SSSP_STAGE_STARTUP_3_1_WAITFORINIT) {
        aosDbgPrintf("no init\t->operation\n");
        // invalidate module ID
        aos.sssp.moduleId = AOS_SSSP_MODULEID_INVALID;
        aosDbgPrintf("SSSP: MSI skipped\n");
      }
      // no abort messages received after completion
      else /* if (aos.sssp.stage == AOS_SSSP_STAGE_STARTUP_3_4_COMPLETION) */ {
        aosDbgPrintf("success\t->operation\n");
        aosDbgPrintf("SSSP: MSI completed\n");
      }
      // cleanup and proceed to operation phase
      _aosSsspMsiOutro(msidata);
    }
    // general timeout
    else {
      aosDbgPrintf("->abort\n");
      // initiate active abort
      aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_4_ABORTION_INIT;
    }
    // clear event
    chEvtGetAndClearFlags(&msidata->timeout.eventlistener);
    *received &= ~(msidata->timeout.eventlistener.events);
  }
  /* DELAY EVENT
   * Delays are only used to toggle the UP signal.
   */
  else if (*received & _delayEventListener.events) {
    aosDbgPrintf("%u\tdelay\t", chVTGetSystemTime());
#if (AMIROOS_CFG_SSSP_STACK_END == true)
    apalControlGpioState_t s;
    aosDbgPrintf("S-\t");
    // deactivate S
    apalControlGpioSet(moduleSsspSignalS(), APAL_GPIO_OFF);
    // verify deactivation
    apalControlGpioGet(moduleSsspSignalS(), &s);
    if (s == APAL_GPIO_OFF) {
      aosDbgPrintf("ok\t->done\n");
      // set the timeout timer so no abort messages are missed
      chVTSet(&msidata->timeout.timer, chTimeUS2I(AOS_SSSP_TIMEOUT), _aosSsspTimerCallback, &msidata->timeout.eventsource);
      // proceed to the completion stage
      aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_4_COMPLETION;
    } else {
      aosDbgPrintf("fail\t->abort\n");
      // initiate active abort
      aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_4_ABORTION_INIT;
    }
#else /* (AMIROOS_CFG_SSSP_STACK_END == true) */
    apalControlGpioState_t s;
    // read the current state of the UP signal
    apalControlGpioGet(moduleSsspSignalUP(), &s);
    if (s == APAL_GPIO_OFF) {
      aosDbgPrintf("UP+ & S-\n");
      // activate UP in order to trigger the next module
      apalControlGpioSet(moduleSsspSignalUP(), APAL_GPIO_ON);
      // deactivate S
      apalControlGpioSet(moduleSsspSignalS(), APAL_GPIO_OFF);
      // set the delay timer so the UP signal will get deactivated again later
      chVTSet(&_delayTimer, chTimeUS2I(AOS_SSSP_DELAY), _aosSsspTimerCallback, &_delayEventSource);
    } else {
      aosDbgPrintf("UP-\n");
      // deactvate UP
      apalControlGpioSet(moduleSsspSignalUP(), APAL_GPIO_OFF);
    }
#endif /* (AMIROOS_CFG_SSSP_STACK_END == true) */
    // clear event
    chEvtGetAndClearFlags(&_delayEventListener);
    *received &= ~(_delayEventListener.events);
  }
  /* GPIO EVENT
   * GPIO events can occurr at any time, but only DN and S events are expected.
   * DN may be activated only once per module to trigger the counting process
   * and is deactivated briefly after activation.
   * S must only deactivate when MSI is comleted by the last module in the stack.
   */
  else if (*received & msidata->signals.eventlistener->events) {
    aosDbgPrintf("%u\tGPIO\t", chVTGetSystemTime());
    apalControlGpioState_t s;
    // fetch event flags
    eventflags_t flags = chEvtGetAndClearFlags(msidata->signals.eventlistener);
#if (AMIROOS_CFG_SSSP_STACK_START != true)
    // if the DN signal was triggered
    if (flags & moduleSsspEventflagDN()) {
      // read the signal state
      apalControlGpioGet(moduleSsspSignalDN(), &s);
      // rising edge
      if (s == APAL_GPIO_ON) {
        aosDbgPrintf("+DN\t");
        // Correct SSSP stage and no ID assigned yet?
        if (aos.sssp.stage == AOS_SSSP_STAGE_STARTUP_3_3_WAIT4EVENT &&
            aos.sssp.moduleId == AOS_SSSP_MODULEID_INVALID) {
          // set the own module ID
          aos.sssp.moduleId = msidata->bcb.lastid + 1;
          aosDbgPrintf("set ID (%u)\t->bc ID\n", aos.sssp.moduleId);
          // proceed to broadcasting
          aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_3_BROADCASTID;
        }
        // incorrect SSSP stage or ID already assigned (triggered for a second time)
        else {
          aosDbgPrintf("->abort\n");
          // initiate active abort
          aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_4_ABORTION_INIT;
        }
      }
      // falling egde
      else {
        aosDbgPrintf("-DN\n");
        // ignored
      }
      // clear DN flags
      flags &= ~moduleSsspEventflagDN();
    }
#endif /* (AMIROOS_CFG_SSSP_STACK_START != true) */
#if (AMIROOS_CFG_SSSP_STACK_END != true)
    // if the UP signal was triggered
    if (flags & moduleSsspEventflagUP()) {
      /*
       * The UP signal is acting as output only.
       * Any events resulting from toggling the signal are thus ignored.
       */
      aosDbgPrintf("?UP\n");
      // clear UP flags
      flags &= ~moduleSsspEventflagUP();
    }
#endif /* (AMIROOS_CFG_SSSP_STACK_END != true) */
    // if the S signal was triggered
    if (flags & moduleSsspEventflagS()) {
      // read the signal state
      apalControlGpioGet(moduleSsspSignalS(), &s);
      // rising edge
      if (s == APAL_GPIO_ON) {
        aosDbgPrintf("+S\n");
        // ignored
      }
      // falling edge
      else {
        aosDbgPrintf("-S\t");
        // special case
        if (aos.sssp.stage == AOS_SSSP_STAGE_STARTUP_3_4_ABORTION_SYNC) {
          aosDbgPrintf("->operation\n");
          // invalidate module ID
          aos.sssp.moduleId = AOS_SSSP_MODULEID_INVALID;
          // proceed
          aosDbgPrintf("SSSP: MSI aborted\n");
          _aosSsspMsiOutro(msidata);
        }
#if (AMIROOS_CFG_SSSP_STACK_END == true)
        // Correct SSSP stage?
        else if (aos.sssp.stage == AOS_SSSP_STAGE_STARTUP_3_4_COMPLETION) {
          aosDbgPrintf("ok\n");
          // ignored
        } else {
          aosDbgPrintf("->abort\n");
          // initialize abortion
          aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_4_ABORTION_INIT;
        }
#else /* (AMIROOS_CFG_SSSP_STACK_END == true) */
        // Correct SSSP stage and ID assigned?
        else if (aos.sssp.stage == AOS_SSSP_STAGE_STARTUP_3_3_WAIT4EVENT &&
                 aos.sssp.moduleId != AOS_SSSP_MODULEID_INVALID) {
          aosDbgPrintf("->done\n");
          // set the timeout timer so no abort messages are missed
          chVTSet(&msidata->timeout.timer, chTimeUS2I(AOS_SSSP_TIMEOUT), _aosSsspTimerCallback, &msidata->timeout.eventsource);
          // proceed to the completion stage
          aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_4_COMPLETION;
        } else {
          aosDbgPrintf("->abort\n");
          // initiate abortion
          aos.sssp.stage = AOS_SSSP_STAGE_STARTUP_3_4_ABORTION_INIT;
        }
#endif /* (AMIROOS_CFG_SSSP_STACK_END == true) */
      }
      // clear S flags
      flags &= ~moduleSsspEventflagS();
    }
    // any further flags set?
    if (flags) {
      aosDbgPrintf("0x%08X\n", flags);
      // reassign remaining flags and do not clear event mask
      msidata->signals.eventlistener->flags |= flags;
    } else {
      // clear event
      *received &= ~(msidata->signals.eventlistener->events);
    }
  }
  /* OTHER EVENT
   * Any other events may occur as well but are not handled by this function
   */
  else if (*received != 0){
    aosDbgPrintf("%u\tOTHER\t0x%08X\n", chVTGetSystemTime(), *received);
    // do not clear the event mask
  }

  return;
}

#endif /* (AMIROOS_CFG_SSSP_MSI == true) */

#if ((AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) && (AMIROOS_CFG_SSSP_MASTER != true) && \
     (AMIROOS_CFG_PROFILE == true)) || defined(__DOXYGEN__)

/**
 * @brief   Retrieve the offset between local clock and system synchronization
 * signal.
 *
 * @return
 */
float aosSsspGetSyncSkew(void)
{
  return _syncskew;
}

#endif /* (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) && (AMIROOS_CFG_SSSP_MASTER != true) && (AMIROOS_CFG_PROFILE == true) */

/**
 * @brief   Leave the operation pahse and initiate or acknowledge shutdown.
 *
 * @param[in] active  Flag, indicating whether the shutdon shall be activeliy
 *                    initiated (true) or a passive request was received
 *                    (false).
 */
void aosSsspShutdownInit(bool active)
{
  aosDbgAssert(aos.sssp.stage == AOS_SSSP_STAGE_OPERATION);

  // proceed to shutdown phase
  aos.sssp.stage = AOS_SSSP_STAGE_SHUTDOWN_1_1;

  // activate PD if this was an active shutdown initiation
  if (active) {
    apalControlGpioSet(moduleSsspSignalPD(), APAL_GPIO_ON);
  }

#if (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) && (AMIROOS_CFG_SSSP_MASTER == true)
  // stop toggling S
  chVTReset(&_synctimer);
#endif /* (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) && (AMIROOS_CFG_SSSP_MASTER == true) */

  // activate S
  apalControlGpioSet(moduleSsspSignalS(), APAL_GPIO_ON);

  // proceed in the shutdown phase
  aos.sssp.stage = AOS_SSSP_STAGE_SHUTDOWN_1_2;

  return;
}

#if (AMIROOS_CFG_SSSP_SHUTDOWN == true) || defined(__DOXYGEN__)

/**
 * @brief   Sequnetially broadcast an identifier via S to specify a specific way
 *          to shutdown or restart.
 *
 * @param[in] identifier  Identifier to be broadcasted.
 */
void aosSsspShutdownBroadcastIdentifier(unsigned int identifier)
{
  // only broadcast anything if a identifier greater than 0 was specified
  if (identifier > 0) {
    // broadcast identifier
    for (unsigned int pulse = 0; pulse < identifier; ++pulse) {
      // wait one delay time with S being deactivated
      aosThdUSleep(AOS_SSSP_DELAY);

      // activate S for one AOS_SSSP_DELAY.
      apalControlGpioSet(moduleSsspSignalS(), APAL_GPIO_ON);
      aosThdUSleep(AOS_SSSP_DELAY);
      apalControlGpioSet(moduleSsspSignalS(), APAL_GPIO_OFF);
    }
  }

  // let a timeout pass and return
  aosThdUSleep(AOS_SSSP_TIMEOUT);

  return;
}

/**
 * @brief   Wait for a pulse during the SSSP shutdown disambiguation procedure.
 * @details Whenever a pulse is received, the identifier is incremented.
 *          Otherwise the internal timer is configured and will eventually
 *          trigger a timeout event.
 *
 * @param[in]     gpiolistener  Event listener for GPIO events.
 * @param[in]     sflags        Event flags to be set by edges on the S signal.
 * @param[in]     timermask     Event maks to use for the internal timer.
 * @param[in,out] identifier    Identifier variable to increment.
 *
 * @return  The event mask of whatever event was received during this function
 *          call.
 */
eventmask_t aosSsspShutdownWaitForIdentifierPulse(event_listener_t* gpiolistener, const eventflags_t sflags, eventmask_t timermask, unsigned int* identifier)
{
  aosDbgCheck(gpiolistener != NULL);
  aosDbgCheck(sflags != 0);
  aosDbgCheck(timermask != 0);
  aosDbgCheck(identifier != NULL);

  // local variables
  eventmask_t mask;
  eventflags_t flags;
  apalControlGpioState_t sstate;

  // arm the timer once
  if (!chVTIsArmed(&_delayTimer)) {
    chEvtRegisterMask(&_delayEventSource, &_delayEventListener, timermask);
    chVtSet(&_delayTimer, chTimeUS2I(AOS_SSSP_TIMEOUT), _aosSsspTimerCallback, &_delayEventSource);
  }

  // wait for any event to occur (do not apply any filters in order not to miss any events)
  mask = chEvtWaitOne(ALL_EVENTS);

  // GPIO event
  if (mask & gpiolistener->events) {
    // retrieve flags without clearing them
    flags = gpiolistener->flags;
    apalControlGpioGet(moduleSsspSignalS(), &sstate);
    if (flags == sflags) {
      apalControlGpioGet(moduleSsspSignalS(), &sstate);
      // if this was the end of a pulse
      if (sstate == APAL_GPIO_OFF) {
        // restart the timer
        chVtSet(&_delayTimer, chTimeUS2I(AOS_SSSP_TIMEOUT), _aosSsspTimerCallback, &_delayEventSource);
        // increment the identifier
        ++(*identifier);
      }
    }
  }
  // timer event
  else if (mask & _delayEventListener.events) {
    // unregister event
    chEvtUnregister(&_delayEventSource, &_delayEventListener);
  }
  // any further events must be handled externally

  return mask;
}

#endif /* (AMIROOS_CFG_SSSP_SHUTDOWN == true) */

/** @} */

#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */
