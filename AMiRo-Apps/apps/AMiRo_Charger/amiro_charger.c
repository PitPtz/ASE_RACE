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
 * @file    amiro_charger.c
 */

#include "amiro_charger.h"
#include <amiroos.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/**
 * @brief   Event mask of the ADC analog watchdog event.
 */
#define AMIRO_CHARGER_ADC_WATCHDOG_EVENT         (urtCoreGetEventMask() << 1)

/**
 * @brief   Event mask of the event broadcasting the battery level.
 */
#define BATTERYLEVEL_EVENT         (urtCoreGetEventMask() << 2)

/**
 * @brief   Number of ADC measurements required to detect a stable voltage.
 * @note    Regardles of the specified value, memory requirements are constant.
 */
#if !defined(AMIRO_CHARGER_ADC_STABLE_ITERATIONS) || defined(__DOXYGEN__)
#define AMIRO_CHARGER_ADC_STABLE_ITERATIONS      100
#endif /* !defined(AMIRO_CHARGER_ADC_STABLE_ITERATIONS) */

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
 * @brief   Name of AMiRo charger nodes.
 */
static const char _amiro_charger_name[] = "Amiro_Charger";

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

#if (defined(BOARD_POWERMANAGEMENT_1_1) || defined(BOARD_POWERMANAGEMENT_1_2))  \
    || defined(__DOXYGEN__)

/**
 * @brief   Callback function for the trigger timer.
 *
 * @param[in] flags   Flags to emit for the trigger event.
 *                    Pointer is reinterpreted as integer value.
 */
static void _amirocharger_triggercb(virtual_timer_t* timer, void* params)
{
  amiro_charger_node_t* const acn = (amiro_charger_node_t*)params;
  // signal node thread to read proximity data
  if (timer == &acn->timer.vt) {
    urtEventSignal(acn->node.thread, BATTERYLEVEL_EVENT);
  }
  return;
}

/**
 * @brief   Retrieves and sets the average battery level over rear and front battery in percentage and mAh.
 */
void amirocharger_batterylevel_percentage(void* acn) {
  // uint16_t maxCapacity = 989; // Usually at 989 (Front) and 988 (Rear)
  uint16_t remainingCapacity_front;
  uint16_t remainingCapacity_rear;
  
  // bq27500_lld_std_command(&moduleLldFuelGaugeFront, BQ27500_LLD_STD_CMD_FullChargeCapacity,
  //                                   &maxCapacity, 2.0f * MICROSECONDS_PER_SECOND);
  bq27500_lld_std_command(&moduleLldFuelGaugeFront, BQ27500_LLD_STD_CMD_RemainingCapacity,
                                    &remainingCapacity_front, MICROSECONDS_PER_SECOND);
  bq27500_lld_std_command(&moduleLldFuelGaugeRear, BQ27500_LLD_STD_CMD_RemainingCapacity,
                                    &remainingCapacity_rear, MICROSECONDS_PER_SECOND);   
  //urtPrintf("Remaining Battery Average [%%]: %u%%\n", (100 * (remainingCapacity_front + remainingCapacity_rear)) / (989 + 988) );
  
  ((amiro_charger_node_t*)acn)->battery_data.size = (remainingCapacity_front + remainingCapacity_rear) / 2;
  ((amiro_charger_node_t*)acn)->battery_data.percentage = (100 * (remainingCapacity_front + remainingCapacity_rear)) / (989 + 988);

  return;
}

/**
 * @brief   ADC analog watchdog callback function.
 *
 * @param[in] adcp  Pointer to the ADC driver.
 * @param[in] err   ADC error value.
 */
static void _amiro_chargerAdcAwdCallback(ADCDriver* adcp, adcerror_t err)
{
  // if this is an watchdog event
  if (err == ADC_ERR_AWD) {
    chSysLockFromISR();
    // if the listener thread is currently set
    if (adcp->listener != NULL) {
      chEvtSignalI(adcp->listener, AMIRO_CHARGER_ADC_WATCHDOG_EVENT);
      adcp->listener = NULL;
    }
    chSysUnlockFromISR();
  }

  return;
}

/**
 * @brief   Read ADC until a stable value is detected.
 *
 * @param[in] acn       Pointer to the calling AMiRo charger node.
 */
inline void _amiro_chargerWaitForStableADC(amiro_charger_node_t* const acn)
{
  urtDebugAssert(acn != NULL);

  // local variables
  bool stable = false;
  adcsample_t last;

  // sample ADC synchronously multiple times
  acn->pmdata->adc.cgroup->circular = false;
  acn->pmdata->adc.cgroup->error_cb = NULL;
  acn->pmdata->adc.cgroup->htr = ADC_HTR((adcsample_t)~0);
  acn->pmdata->adc.cgroup->ltr = ADC_LTR((adcsample_t)0);
  while (!stable) {
    stable = true;
    // sample ADC synchronously multiple times and check whether all samples are at the same side of the threshold
    for (size_t sample = 0; sample < AMIRO_CHARGER_ADC_STABLE_ITERATIONS; ++sample) {
      adcConvert(acn->pmdata->adc.driver, acn->pmdata->adc.cgroup, acn->pmdata->adc.buffer, sizeof(acn->pmdata->adc.buffer)/sizeof(acn->pmdata->adc.buffer[0]));
      if (sample == 0) {
        last = acn->pmdata->adc.buffer[0];
      }
      if ((acn->pmdata->adc.buffer[0] == acn->pmdata->adc.threshold) ||
          (acn->pmdata->adc.buffer[0] < acn->pmdata->adc.threshold && last > acn->pmdata->adc.threshold) ||
          (acn->pmdata->adc.buffer[0] > acn->pmdata->adc.threshold && last < acn->pmdata->adc.threshold)) {
        stable = false;
        break;
      }
    }
  }

  return;
}

/**
 * @brief   Set chargers and LED.
 *
 * @param[in] acn     Pointer to the calling AMiRo charger node.
 * @param[in] enable  Indicator, whether charging shall be enabled or disabled.
 */
inline void _amiro_chargerSetChargers(amiro_charger_node_t* const acn, bool enable)
{
  urtDebugAssert(acn != NULL);

  // enable all chargers
  amiro_charger_bq241xx_t* bq = acn->pmdata->chargers;
  while (bq != NULL) {
    bq241xx_lld_set_enabled(bq->bq241xx, enable ? BQ241xx_LLD_ENABLED : BQ241xx_LLD_DISABLED);
    bq = bq->next;
  }

  // set LED
  if (acn->led != NULL) {
    led_lld_set(acn->led, enable ? LED_LLD_STATE_ON : LED_LLD_STATE_OFF);
  }

  return;
}

/**
 * @brief   Start continouos ADC sampling with watchdog (AWD) enabled.
 *
 * @param[in] acn   Pointer to the calling AMiRo charger node.
 */
static inline void _amiro_chargerStartAWD(amiro_charger_node_t* const acn)
{
  urtDebugAssert(acn != NULL);
  urtDebugAssert(acn->pmdata->adc.buffer[0] != acn->pmdata->adc.threshold);

  // set threshold values
  if (acn->pmdata->adc.buffer[0] > acn->pmdata->adc.threshold) {
    acn->pmdata->adc.cgroup->htr = ADC_HTR((adcsample_t)~0);
    acn->pmdata->adc.cgroup->ltr = ADC_LTR(acn->pmdata->adc.threshold);
  } else {
    acn->pmdata->adc.cgroup->htr = ADC_HTR(acn->pmdata->adc.threshold);
    acn->pmdata->adc.cgroup->ltr = ADC_LTR((adcsample_t)0);
  }
  // configure ADC
  acn->pmdata->adc.cgroup->circular = true;
  acn->pmdata->adc.cgroup->error_cb = _amiro_chargerAdcAwdCallback;
  acn->pmdata->adc.driver->listener = urtThreadGetSelf();

  // start continuous sampling
  adcStartConversion(acn->pmdata->adc.driver, acn->pmdata->adc.cgroup, acn->pmdata->adc.buffer, sizeof(acn->pmdata->adc.buffer) / sizeof(acn->pmdata->adc.buffer[0]));

  return;
}

/**
 * @brief   Setup callback function for AMiRo charger nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] acn     Pointer to the AMiRo charger structure.
 *                    Must not be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _amiro_charger_Setup(urt_node_t* node, void* acn)
{
  urtDebugAssert(acn != NULL);
  (void)node;
  (void)acn;

  // set thread name
  chRegSetThreadName(_amiro_charger_name);

  // enforce initial measurement
  urtEventSignal(urtThreadGetSelf(), AMIRO_CHARGER_ADC_WATCHDOG_EVENT);

  // activate a timer for battery status
  aosTimerPeriodicInterval(&((amiro_charger_node_t*)acn)->timer,
                           1000.0f * MICROSECONDS_PER_SECOND,
                           _amirocharger_triggercb, acn);

  return AMIRO_CHARGER_ADC_WATCHDOG_EVENT | BATTERYLEVEL_EVENT;
}

/**
 * @brief   Loop callback function for AMiRo charger nodes.
 *
 * @param[in] node  Pointer to the node object.
 *                  Must not be NULL.
 * @param[in] acn   Pointer to the AMiRo charger structure.
 *                  Must not be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _amiro_charger_Loop(urt_node_t* node, urt_osEventMask_t event, void* acn)
{
  urtDebugAssert(acn != NULL);
  (void)node;

  // local constants
  amiro_charger_node_t* const charger = (amiro_charger_node_t*)acn;

  switch(event) {
    case AMIRO_CHARGER_ADC_WATCHDOG_EVENT:
    {
      // disable chargers
      _amiro_chargerSetChargers(charger, false);

      // make sure the voltage is stable
      _amiro_chargerWaitForStableADC(charger);

      // set chargers
      _amiro_chargerSetChargers(charger, charger->pmdata->adc.buffer[0] > charger->pmdata->adc.threshold);

      // publish a message
      // if (charger->publisher.topic != NULL) {
      //   const chargedata_si cdata = {.volts = charger->pmdata->adc.converter(charger->pmdata->adc.buffer[0]),
      //                                .charging = charger->pmdata->adc.buffer[0] > charger->pmdata->adc.threshold};
      //   urt_osTime_t t;
      //   urtTimeNow(&t);
      //   urtPublisherPublish(&charger->publisher, &cdata, sizeof(chargedata_u), &t, URT_PUBLISHER_PUBLISH_ENFORCING);
      // }

      // start continuous sampling again
      _amiro_chargerStartAWD(charger);

      break;
    }
    case BATTERYLEVEL_EVENT:
    {
      if (charger->publisher.topic != NULL) {
        amirocharger_batterylevel_percentage(acn);
        urtTimeNow(&charger->publish_time);
        // publish the battery data
        urtPublisherPublish(&charger->publisher,
                          &charger->battery_data,
                          sizeof(charger->battery_data),
                          &charger->publish_time,
                          URT_PUBLISHER_PUBLISH_LAZY);
      }
      event &= ~BATTERYLEVEL_EVENT;
      break;
    }
    default:
      // this must never happen
      urtDebugAssert(false);
      break;
  }
  return AMIRO_CHARGER_ADC_WATCHDOG_EVENT | BATTERYLEVEL_EVENT;
}

/**
 * @brief   Shutdown callback function for AMiRo charger nodes.
 *
 * @param[in] node  Pointer to the node object.
 *                  Must not be NULL.
 * @param[in] acn   Pointer to the AMiRo charger structure.
 *                  Must nor be NULL.
 */
void _amiro_charger_Shutdown(urt_node_t* node, urt_status_t reason, void* acn)
{
  urtDebugAssert(acn != NULL);

  (void)node;
  (void)reason;

  // local constants
  amiro_charger_node_t* const charger = (amiro_charger_node_t*)acn;

  // disable chargers
  _amiro_chargerSetChargers(charger, false);

  // stop ADC
  adcStopConversion(charger->pmdata->adc.driver);

  // stop timer
  aosTimerReset(&charger->timer);

  return;
}

#endif /* defined(BOARD_POWERMANAGEMENT_1_1) || defined(BOARD_POWERMANAGEMENT_1_2) */

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_amiro_charger
 * @{
 */

#if (defined(BOARD_POWERMANAGEMENT_1_1) || defined(BOARD_POWERMANAGEMENT_1_2))  \
    || defined(__DOXYGEN__)

/**
 * @brief   Initialization function of PowerManagement related data.
 *
 * @param[in,out] data        Pointer to the object to initialize.
 *                            Must not be NULL.
 * @param[in]     adc         Pointer to the ADC driver to monitor.
 *                            Must not be NULL.
 * @param[in]     cgroup      Pointer to the ADC conversion group to use for the ADC.
 *                            Must not be NULL.
 * @param[in]     threshold   Threshold value to enable/disable chargers.
 * @param[in]     converter   Function pointer to convert ADC sample data to voltage.
 *                            Must not be NULL.
 * @param[in]     chargers    List of BQ241xx chargers.
 *                            May be NULL:
 */
void amiroChargerPmDataInit(amiro_charger_powermanagementdata_t* data, ADCDriver* adc, ADCConversionGroup* cgroup, adcsample_t threshold, amirochargerADC2V_f converter, amiro_charger_bq241xx_t* chargers)
{
  urtDebugAssert(data != NULL);
  urtDebugAssert(adc != NULL);
  urtDebugAssert(cgroup != NULL);
  urtDebugAssert(converter != NULL);

  data->adc.driver = adc;
  data->adc.cgroup = cgroup;
  data->adc.threshold = threshold;
  data->adc.converter = converter;
  data->chargers = chargers;

  return;
}

#endif /* defined(BOARD_POWERMANAGEMENT_1_1) || defined(BOARD_POWERMANAGEMENT_1_2) */

/**
 * @brief   Initialization method of the AMiRo charger application.
 *
 * @param[in,out] acn     AMiRo charger structure to nitialize.
 *                        Must not be NULL.
 * @param[in]     led     Pointer to a LED to turn when charging.
 *                        May be NULL.
 * @param[in]     topic   Pointer to a topic to broadcast information on.
 *                        May be NULL.
 * @param[in]     prio    Priority of the node execution thread.
 * @param[in]     data    Pointer to further data.
 *                        For PowerManagement modules, this must point to a  @p amiro_charger_powermanagementdata structure.
 */
void amiroChargerInit(amiro_charger_node_t* acn, LEDDriver* led, urt_topic_t* topic, urt_osThreadPrio_t prio, void* data)
{
  urtDebugAssert(acn != NULL);
#if defined(BOARD_POWERMANAGEMENT_1_1) || defined(BOARD_POWERMANAGEMENT_1_2)
  urtDebugAssert(data != NULL);
#endif /* defined(BOARD_POWERMANAGEMENT_1_1) || defined(BOARD_POWERMANAGEMENT_1_2) */

  // set the LED driver
  acn->led = led;

#if (URT_CFG_PUBSUB_ENABLED == true)
  // set the topic and initialize publisher
  if (topic != NULL) {
    urtPublisherInit(&acn->publisher, topic);
  } else {
    acn->publisher.topic = NULL;
  }
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if defined(BOARD_POWERMANAGEMENT_1_1) || defined(BOARD_POWERMANAGEMENT_1_2)
  // set PowerManagement related data
  acn->pmdata = (amiro_charger_powermanagementdata_t*)data;
  urtDebugAssert(acn->pmdata->adc.driver != NULL);
  urtDebugAssert(acn->pmdata->adc.cgroup != NULL);
  urtDebugAssert(acn->pmdata->adc.converter != NULL);
#endif /* defined(BOARD_POWERMANAGEMENT_1_1) || defined(BOARD_POWERMANAGEMENT_1_2) */

  // initialize the timer
  aosTimerInit(&acn->timer);
  
  // initialize the node
  urtNodeInit(&acn->node, (urt_osThread_t*)acn->thread, sizeof(acn->thread), prio,
              _amiro_charger_Setup, acn,
              _amiro_charger_Loop, acn,
              _amiro_charger_Shutdown, acn);

  return;
}

/** @} */

int batteryShellCallback_getPercentage(BaseSequentialStream* stream, amiro_charger_node_t* acn)
{
  urtDebugAssert(acn != NULL);
  amirocharger_batterylevel_percentage(acn);

  chprintf(stream, "Remaining Battery: %i%%\n", acn->battery_data.percentage);
  return AOS_OK;
}