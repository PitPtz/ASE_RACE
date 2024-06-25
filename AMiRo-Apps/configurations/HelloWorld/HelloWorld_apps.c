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
 * @file    HelloWorld_apps.c
 * @brief   HelloWorld configuration application container.
 */

#include <HelloWorld_apps.h>
#include <helloworld.h>
#include <stdlib.h>
#include <string.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/**
 * @brief   Event mask to use for the execution termination timer.
 */
#define STOPEVENT                               (urt_osEventMask_t)(1 << 8)

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)

#if !defined(HELLOWORLD_TOPICID) || defined(__DOXYGEN__)
/**
 * @brief   Default topic identifier.
 */
#define HELLOWORLD_TOPICID                      123
#endif /* !defined(HELLOWORLD_TOPICID) */

#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)

#if !defined(HELLOWORLD_SERVICEID) || defined(__DOXYGEN__)
/**
 * @brief   Default service identifier.
 */
#define HELLOWORLD_SERVICEID                    123
#endif /* !defined(HELLOWORLD_SERVICEID) */

#endif /* (URT_CFG_RPC_ENABLED == true) */

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
 * forward declarations
 */
static int _helloworld_messageshellcb(BaseSequentialStream* stream, int argc, const char* argv[]);
#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
static int _helloworld_pubsubshellcb(BaseSequentialStream* stream, int argc, const char* argv[]);
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
static int _helloworld_rpcshellcb(BaseSequentialStream* stream, int argc, const char* argv[]);
#endif /* (URT_CFG_RPC_ENABLED == true) */

/**
 * @brief   Event source for the periodic trigger.
 */
urt_osEventSource_t _helloworld_triggerSource;

/**
 * @brief   Event source for execution termination.
 */
urt_osEventSource_t _helloworld_terminationSource;

/**
 * @brief   Timer to trigger tests.
 */
aos_timer_t _helloworld_triggertimer;

/**
 * @brief   Timer to terminate test execution.
 */
aos_timer_t _helloworld_terminationtimer;

/**
 * @brief   Master node object.
 */
helloworld_master_t _helloworld_master;

/**
 * @brief   Slave node object.
 */
helloworld_slave_t _helloworld_slave;

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)

/**
 * @brief   Topic for publish-subscribe testing.
 */
urt_topic_t _helloworld_topic;

/**
 * @brief   Additional messages to provide to the topic.
 */
urt_message_t _helloworld_messages[2];

#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

/**
 * @brief   Shell command for message printing.
 */
static AOS_SHELL_COMMAND(_helloworld_messageshellcmd, "HelloWorld:Message", _helloworld_messageshellcb);

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
/**
 * @brief   Shell command for publish-subscribe testing.
 */
static AOS_SHELL_COMMAND(_helloworld_pubsubshellcmd, "HelloWorld:PubSub", _helloworld_pubsubshellcb);
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
/**
 * @brief   Shell command for RPC testing.
 */
static AOS_SHELL_COMMAND(_helloworld_rpcshellcmd, "HelloWorld:RPC", _helloworld_rpcshellcb);
#endif /* (URT_CFG_RPC_ENABLED == true) */

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   Callback function for the trigger timer.
 *
 * @param[in] flags   Flags to emit for the trigger event.
 *                    Pointer is reinterpreted as integer value.
 */
static void _helloworld_triggercb(virtual_timer_t* timer, void* flags)
{
  (void)timer;

  // broadcast event
  chEvtBroadcastFlagsI(&_helloworld_triggerSource, (urt_osEventFlags_t)flags);

  return;
}

/**
 * @brief   Callback function for the execution termination timer.
 *
 * @param[in] params  Unused parameter.
 */
static void _helloworld_terminationcb(virtual_timer_t* timer, void* params)
{
  (void)timer;
  (void)params;

  // reset periodic timer
  aosTimerResetI(&_helloworld_triggertimer);

  // broadcast event
  chEvtBroadcastI(&_helloworld_terminationSource);

  return;
}

/**
 * @brief   Shell callback function for message printing.
 *
 * @param[in] stream  Stream to use for output.
 * @param[in] argc    Number of arguments.
 * @param[in] argv    List of arguments.
 *
 * @return  Status value indicating the result of the function call.
 */
static int _helloworld_messageshellcb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  // print help text
  if (argc != 3 || aosShellcmdCheckHelp(argc, argv, true) != AOS_SHELL_HELPREQUEST_NONE) {
    chprintf(stream, "Usage: %s DURATION FREQUENCY\n", argv[0]);
    chprintf(stream, "  DURATION\n");
    chprintf(stream, "    Duration (in seconds) how long the HelloWorld application will run.\n");
    chprintf(stream, "    The value is limited to range (0..60].\n");
    chprintf(stream, "  FREQUENCY\n");
    chprintf(stream, "    Frequency (in Hz) at which messages shall be printed.\n");
    return (aosShellcmdCheckHelp(argc, argv, true) == AOS_SHELL_HELPREQUEST_EXPLICIT) ? AOS_OK : AOS_INVALIDARGUMENTS;
  }

  // parse arguments
  const float duration = atof(argv[1]);
  const float frequency = atof(argv[2]);
  if (duration <= 0 || duration > 60 || frequency <= 0) {
    return AOS_INVALIDARGUMENTS;
  }
  const aos_longinterval_t period = (1.0f / frequency) * MICROSECONDS_PER_SECOND + 0.5f;
  if (period < AOS_TIMER_MIN_PRIOD_US) {
    return AOS_INVALIDARGUMENTS;
  }

  // register to execution event
  event_listener_t listener;
  aosShellEventRegisterMask(&_helloworld_terminationSource, &listener, STOPEVENT);

#if ((URT_CFG_PUBSUB_ENABLED == true) && (URT_CFG_PUBSUB_PROFILING == true)) || \
    ((URT_CFG_RPC_ENABLED == true) && (URT_CFG_RPC_PROFILING == true))
  // profiling data
  urt_node_profilingdata_t master_node = _helloworld_master.node.profiling;
#endif /* ((URT_CFG_PUBSUB_ENABLED == true) && (URT_CFG_PUBSUB_PROFILING == true)) ||
          ((URT_CFG_RPC_ENABLED == true) && (URT_CFG_RPC_PROFILING == true)) */

  // activate the timers
  chSysLock();
  aosTimerPeriodicLongIntervalI(&_helloworld_triggertimer, period, _helloworld_triggercb, (void*)HELLOWORLD_TRIGGERFLAG_MESSAGE);
  aosTimerSetLongIntervalI(&_helloworld_terminationtimer, duration * MICROSECONDS_PER_SECOND + 0.5f, _helloworld_terminationcb, NULL);
  chSysUnlock();

  // wait for the execution to finish
  const eventmask_t event = chEvtWaitOne(STOPEVENT | AOS_SHELL_EVENT_OS);

  // unregister from execution event
  aosShellEventUnregister(&_helloworld_terminationSource, &listener);

  // error if an unexpected event is received
  if (event & AOS_SHELL_EVENTSMASK_PROHIBITED) {
    chEvtSignal(chThdGetSelfX(), event);
    return AOS_ERROR;
  }

#if ((URT_CFG_PUBSUB_ENABLED == true) && (URT_CFG_PUBSUB_PROFILING == true)) || \
    ((URT_CFG_RPC_ENABLED == true) && (URT_CFG_RPC_PROFILING == true))
  // brief delay to ensure all pending output is printed
  urtThreadMSleep(100);
  streamPut(stream, '\n');

  // print master node information
  chprintf(stream, "MASTER NODE:\n");
  master_node.loops = _helloworld_master.node.profiling.loops - master_node.loops;
  chprintf(stream, "  loops: %u\n", master_node.loops);
#endif /* ((URT_CFG_PUBSUB_ENABLED == true) && (URT_CFG_PUBSUB_PROFILING == true)) ||
          ((URT_CFG_RPC_ENABLED == true) && (URT_CFG_RPC_PROFILING == true)) */

  return AOS_OK;
}

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
/**
 * @brief   Shell callback function for publish-subscribe testing.
 *
 * @param[in] stream  Stream to use for output.
 * @param[in] argc    Number of arguments.
 * @param[in] argv    List of arguments.
 *
 * @return  Status value indicating the result of the function call.
 */
static int _helloworld_pubsubshellcb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  // print help text
  if (argc != 3 || aosShellcmdCheckHelp(argc, argv, true) != AOS_SHELL_HELPREQUEST_NONE) {
    chprintf(stream, "Usage: %s DURATION FREQUENCY\n", argv[0]);
    chprintf(stream, "  DURATION\n");
    chprintf(stream, "    Duration (in seconds) how long the HelloWorld application will run.\n");
    chprintf(stream, "    The value is limited to range (0..60].\n");
    chprintf(stream, "  FREQUENCY\n");
    chprintf(stream, "    Frequency (in Hz) at which messages shall be published.\n");
    return (aosShellcmdCheckHelp(argc, argv, true) == AOS_SHELL_HELPREQUEST_EXPLICIT) ? AOS_OK : AOS_INVALIDARGUMENTS;
  }

  // parse arguments
  const float duration = atof(argv[1]);
  const float frequency = atof(argv[2]);
  if (duration <= 0 || duration > 60 || frequency <= 0) {
    return AOS_INVALIDARGUMENTS;
  }
  const aos_longinterval_t period = (1.0f / frequency) * MICROSECONDS_PER_SECOND + 0.5f;
  if (period < AOS_TIMER_MIN_PRIOD_US) {
    return AOS_INVALIDARGUMENTS;
  }

  // register to execution event
  event_listener_t listener;
  aosShellEventRegisterMask(&_helloworld_terminationSource, &listener, STOPEVENT);

#if (URT_CFG_PUBSUB_PROFILING == true)
  // profiling data
  urt_node_profilingdata_t master_node = _helloworld_master.node.profiling;
  urt_node_profilingdata_t slave_node = _helloworld_slave.node.profiling;
  urt_publisher_profilingdata_t publisher_data = _helloworld_master.publisher.profiling;
  urt_topic_profilingdata_t topic_data = _helloworld_topic.profiling;
  urt_basesubscriber_profilingdata_t nrtsubscriber_data =_helloworld_slave.subscribers.nrt.profiling;
  urt_basesubscriber_profilingdata_t srtsubscriber_data =_helloworld_slave.subscribers.srt.profiling;
  urt_basesubscriber_profilingdata_t frtsubscriber_data =_helloworld_slave.subscribers.frt.profiling;
  urt_basesubscriber_profilingdata_t hrtsubscriber_data =_helloworld_slave.subscribers.hrt.profiling;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

  // activate the timers
  chSysLock();
  aosTimerPeriodicLongIntervalI(&_helloworld_triggertimer, period, _helloworld_triggercb, (void*)HELLOWORLD_TRIGGERFLAG_PUBSUB);
  aosTimerSetLongIntervalI(&_helloworld_terminationtimer, duration * MICROSECONDS_PER_SECOND + 0.5f, _helloworld_terminationcb, NULL);
  chSysUnlock();

  // wait for the execution to finish
  const eventmask_t event = chEvtWaitOne(STOPEVENT | AOS_SHELL_EVENT_OS);

  // unregister from execution event
  aosShellEventUnregister(&_helloworld_terminationSource, &listener);

  // error if an unexpected event is received
  if (event & AOS_SHELL_EVENTSMASK_PROHIBITED) {
    chEvtSignal(chThdGetSelfX(), event);
    return AOS_ERROR;
  }

#if (URT_CFG_PUBSUB_PROFILING == true)
  // brief delay to ensure all pending output is printed
  urtThreadMSleep(100);
  streamPut(stream, '\n');

  // print master node information
  chprintf(stream, "MASTER NODE:\n");
  master_node.loops = _helloworld_master.node.profiling.loops - master_node.loops;
  chprintf(stream, "  loops: %u\n", master_node.loops);

  // print slave node information
  chprintf(stream, "SLAVE NODE:\n");
  slave_node.loops = _helloworld_slave.node.profiling.loops - slave_node.loops;
  chprintf(stream, "  loops: %u\n", slave_node.loops);

  // print publisher information
  chprintf(stream, "PUBLISHER:\n");
  publisher_data.publishAttempts = _helloworld_master.publisher.profiling.publishAttempts - publisher_data.publishAttempts;
  publisher_data.publishFails = _helloworld_master.publisher.profiling.publishFails - publisher_data.publishFails;
  chprintf(stream, "  attempts: %u\n", publisher_data.publishAttempts);
  chprintf(stream, "  fails:    %u\n", publisher_data.publishFails);
  if (publisher_data.publishAttempts > 0) {
    chprintf(stream, "  ratio:    %.2f%%\n", (1.0f - ((float)publisher_data.publishFails / (float)publisher_data.publishAttempts)) * 100.0f);
  }
  chprintf(stream, "  rate:     %.2f Hz\n", (float)(publisher_data.publishAttempts - publisher_data.publishFails) / duration);

  // print topic information
  chprintf(stream, "TOPIC:\n");
  topic_data.numMessagesPublished = _helloworld_topic.profiling.numMessagesPublished - topic_data.numMessagesPublished;
  topic_data.numMessagesDiscarded = _helloworld_topic.profiling.numMessagesDiscarded - topic_data.numMessagesDiscarded;
  chprintf(stream, "  published: %u\n", topic_data.numMessagesPublished);
  chprintf(stream, "  discarded: %u\n", topic_data.numMessagesDiscarded);
  if (topic_data.numMessagesPublished > 0) {
    chprintf(stream, "  ratio:     %.2f%%\n", (1.0f - ((float)topic_data.numMessagesDiscarded / (float)topic_data.numMessagesPublished)) * 100.0f);
  }
  chprintf(stream, "  rate:      %.2f Hz\n", (float)topic_data.numMessagesPublished / duration);

  // prtint NRT subscriber information
  chprintf(stream, "NRT SUBSCRIBER:\n");
  nrtsubscriber_data.minLatency = _helloworld_slave.subscribers.nrt.profiling.minLatency;
  nrtsubscriber_data.maxLatency = _helloworld_slave.subscribers.nrt.profiling.maxLatency;
  nrtsubscriber_data.sumLatencies = _helloworld_slave.subscribers.nrt.profiling.sumLatencies - nrtsubscriber_data.sumLatencies;
  nrtsubscriber_data.numMessagesReceived = _helloworld_slave.subscribers.nrt.profiling.numMessagesReceived - nrtsubscriber_data.numMessagesReceived;
  chprintf(stream, "  received:    %u\n", nrtsubscriber_data.numMessagesReceived);
  chprintf(stream, "  ratio:       %.2f%%\n", ((float)nrtsubscriber_data.numMessagesReceived / (float)topic_data.numMessagesPublished) * 100.0f);
  if (nrtsubscriber_data.numMessagesReceived > 0) {
    chprintf(stream, "  min latency: %u us\n", nrtsubscriber_data.minLatency);
    chprintf(stream, "  max latency: %u us\n", nrtsubscriber_data.maxLatency);
    chprintf(stream, "  avg latency: %.2f us\n", (float)nrtsubscriber_data.sumLatencies / (float)nrtsubscriber_data.numMessagesReceived);
    chprintf(stream, "  jitter:      %u us\n", nrtsubscriber_data.maxLatency - nrtsubscriber_data.minLatency);
  }
  chprintf(stream, "  rate:        %.2f Hz\n", (float)nrtsubscriber_data.numMessagesReceived / duration);

  // prtint SRT subscriber information
  chprintf(stream, "SRT SUBSCRIBER:\n");
  srtsubscriber_data.minLatency = _helloworld_slave.subscribers.srt.profiling.minLatency;
  srtsubscriber_data.maxLatency = _helloworld_slave.subscribers.srt.profiling.maxLatency;
  srtsubscriber_data.sumLatencies = _helloworld_slave.subscribers.srt.profiling.sumLatencies - srtsubscriber_data.sumLatencies;
  srtsubscriber_data.numMessagesReceived = _helloworld_slave.subscribers.srt.profiling.numMessagesReceived - srtsubscriber_data.numMessagesReceived;
  chprintf(stream, "  received:    %u\n", srtsubscriber_data.numMessagesReceived);
  chprintf(stream, "  ratio:       %.2f%%\n", ((float)srtsubscriber_data.numMessagesReceived / (float)topic_data.numMessagesPublished) * 100.0f);
  if (srtsubscriber_data.numMessagesReceived > 0) {
    chprintf(stream, "  min latency: %u us\n", srtsubscriber_data.minLatency);
    chprintf(stream, "  max latency: %u us\n", srtsubscriber_data.maxLatency);
    chprintf(stream, "  avg latency: %.2f us\n", (float)srtsubscriber_data.sumLatencies / (float)srtsubscriber_data.numMessagesReceived);
    chprintf(stream, "  jitter:      %u us\n", srtsubscriber_data.maxLatency - srtsubscriber_data.minLatency);
  }
  chprintf(stream, "  rate:        %.2f Hz\n", (float)srtsubscriber_data.numMessagesReceived / duration);

  // prtint FRT subscriber information
  chprintf(stream, "FRT SUBSCRIBER:\n");
  frtsubscriber_data.minLatency = _helloworld_slave.subscribers.frt.profiling.minLatency;
  frtsubscriber_data.maxLatency = _helloworld_slave.subscribers.frt.profiling.maxLatency;
  frtsubscriber_data.sumLatencies = _helloworld_slave.subscribers.frt.profiling.sumLatencies - frtsubscriber_data.sumLatencies;
  frtsubscriber_data.numMessagesReceived = _helloworld_slave.subscribers.frt.profiling.numMessagesReceived - frtsubscriber_data.numMessagesReceived;
  chprintf(stream, "  received:    %u\n", frtsubscriber_data.numMessagesReceived);
  chprintf(stream, "  ratio:       %.2f%%\n", ((float)frtsubscriber_data.numMessagesReceived / (float)topic_data.numMessagesPublished) * 100.0f);
  if (frtsubscriber_data.numMessagesReceived > 0) {
    chprintf(stream, "  min latency: %u us\n", frtsubscriber_data.minLatency);
    chprintf(stream, "  max latency: %u us\n", frtsubscriber_data.maxLatency);
    chprintf(stream, "  avg latency: %.2f us\n", (float)frtsubscriber_data.sumLatencies / (float)frtsubscriber_data.numMessagesReceived);
    chprintf(stream, "  jitter:      %u us\n", frtsubscriber_data.maxLatency - frtsubscriber_data.minLatency);
  }
  chprintf(stream, "  rate:        %.2f Hz\n", (float)frtsubscriber_data.numMessagesReceived / duration);

  // prtint HRT subscriber information
  chprintf(stream, "HRT SUBSCRIBER:\n");
  hrtsubscriber_data.minLatency = _helloworld_slave.subscribers.hrt.profiling.minLatency;
  hrtsubscriber_data.maxLatency = _helloworld_slave.subscribers.hrt.profiling.maxLatency;
  hrtsubscriber_data.sumLatencies = _helloworld_slave.subscribers.hrt.profiling.sumLatencies - hrtsubscriber_data.sumLatencies;
  hrtsubscriber_data.numMessagesReceived = _helloworld_slave.subscribers.hrt.profiling.numMessagesReceived - hrtsubscriber_data.numMessagesReceived;
  chprintf(stream, "  received:    %u\n", hrtsubscriber_data.numMessagesReceived);
  chprintf(stream, "  ratio:       %.2f%%\n", ((float)hrtsubscriber_data.numMessagesReceived / (float)topic_data.numMessagesPublished) * 100.0f);
  if (hrtsubscriber_data.numMessagesReceived > 0) {
    chprintf(stream, "  min latency: %u us\n", hrtsubscriber_data.minLatency);
    chprintf(stream, "  max latency: %u us\n", hrtsubscriber_data.maxLatency);
    chprintf(stream, "  avg latency: %.2f us\n", (float)hrtsubscriber_data.sumLatencies / (float)hrtsubscriber_data.numMessagesReceived);
    chprintf(stream, "  jitter:      %u us\n", hrtsubscriber_data.maxLatency - hrtsubscriber_data.minLatency);
  }
  chprintf(stream, "  rate:        %.2f Hz\n", (float)hrtsubscriber_data.numMessagesReceived / duration);
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

  return AOS_OK;
}
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
/**
 * @brief   Shell callback function for RPC testing.
 *
 * @param[in] stream  Stream to use for output.
 * @param[in] argc    Number of arguments.
 * @param[in] argv    List of arguments.
 *
 * @return  Status value indicating the result of the function call.
 */
static int _helloworld_rpcshellcb(BaseSequentialStream* stream, int argc, const char* argv[])
{
  // print help text
  if (argc != 3 || aosShellcmdCheckHelp(argc, argv, true) != AOS_SHELL_HELPREQUEST_NONE) {
    chprintf(stream, "Usage: %s DURATION FREQUENCY\n", argv[0]);
    chprintf(stream, "  DURATION\n");
    chprintf(stream, "    Duration (in seconds) how long the HelloWorld application will run.\n");
    chprintf(stream, "    The value is limited to range (0..60].\n");
    chprintf(stream, "  FREQUENCY\n");
    chprintf(stream, "    Frequency (in Hz) at which requests shall be submitted.\n");
    return (aosShellcmdCheckHelp(argc, argv, true) == AOS_SHELL_HELPREQUEST_EXPLICIT) ? AOS_OK : AOS_INVALIDARGUMENTS;
  }

  // parse arguments
  const float duration = atof(argv[1]);
  const float frequency = atof(argv[2]);
  if (duration <= 0 || duration > 60 || frequency <= 0) {
    return AOS_INVALIDARGUMENTS;
  }
  const aos_longinterval_t period = (1.0f / frequency) * MICROSECONDS_PER_SECOND + 0.5f;
  if (period < AOS_TIMER_MIN_PRIOD_US) {
    return AOS_INVALIDARGUMENTS;
  }

  // register to execution event
  event_listener_t listener;
  aosShellEventRegisterMask(&_helloworld_terminationSource, &listener, STOPEVENT);

  // profiling data)
#if (URT_CFG_RPC_PROFILING == true)
  urt_node_profilingdata_t master_node = _helloworld_master.node.profiling;
  urt_node_profilingdata_t slave_node = _helloworld_slave.node.profiling;
  urt_service_profilingdata_t service_data = _helloworld_slave.service.profiling;
  urt_baserequest_profilingdata_t nrtrequest_data = _helloworld_master.requests.nrt.profiling;
  urt_baserequest_profilingdata_t srtrequest_data = _helloworld_master.requests.srt.profiling;
  urt_baserequest_profilingdata_t frtrequest_data = _helloworld_master.requests.frt.profiling;
  urt_baserequest_profilingdata_t hrtrequest_data = _helloworld_master.requests.hrt.profiling;
#endif /* (URT_CFG_RPC_PROFILING == true) */

  // activate the timers
  chSysLock();
  aosTimerPeriodicLongIntervalI(&_helloworld_triggertimer, period, _helloworld_triggercb, (void*)HELLOWORLD_TRIGGERFLAG_RPC);
  aosTimerSetLongIntervalI(&_helloworld_terminationtimer, duration * MICROSECONDS_PER_SECOND + 0.5f, _helloworld_terminationcb, NULL);
  chSysUnlock();

  // wait for the execution to finish
  const eventmask_t event = chEvtWaitOne(STOPEVENT | AOS_SHELL_EVENT_OS);

  // unregister from execution event
  aosShellEventUnregister(&_helloworld_terminationSource, &listener);

  // error if an unexpected event is received
  if (event & AOS_SHELL_EVENTSMASK_PROHIBITED) {
    chEvtSignal(chThdGetSelfX(), event);
    return AOS_ERROR;
  }

#if (URT_CFG_RPC_PROFILING == true)
  // brief delay to ensure all pending output is printed
  urtThreadMSleep(100);
  streamPut(stream, '\n');

  // print master node information
  chprintf(stream, "MASTER NODE:\n");
  master_node.loops = _helloworld_master.node.profiling.loops - master_node.loops;
  chprintf(stream, "  loops: %u\n", master_node.loops);

  // print slave node information
  chprintf(stream, "SLAVE NODE:\n");
  slave_node.loops = _helloworld_slave.node.profiling.loops - slave_node.loops;
  chprintf(stream, "  loops: %u\n", slave_node.loops);

  // print service information
  chprintf(stream, "SERVICE:\n");
  service_data.numCalls = _helloworld_slave.service.profiling.numCalls - service_data.numCalls;
  service_data.numOwnershipLost = _helloworld_slave.service.profiling.numOwnershipLost - service_data.numOwnershipLost;
  chprintf(stream, "  num calls: %u\n", service_data.numCalls);
  chprintf(stream, "  num lost:  %u\n", service_data.numOwnershipLost);
  if (service_data.numCalls > 0) {
    chprintf(stream, "  ratio:     %.2f%%\n", (1.0f - ((float)service_data.numOwnershipLost / (float)service_data.numCalls)) * 100.0f);
  }

  // print NRT request information
  chprintf(stream, "NRT REQUEST:\n");
  nrtrequest_data.minLatency = _helloworld_master.requests.nrt.profiling.minLatency;
  nrtrequest_data.maxLatency = _helloworld_master.requests.nrt.profiling.maxLatency;
  nrtrequest_data.sumLatencies = _helloworld_master.requests.nrt.profiling.sumLatencies - nrtrequest_data.sumLatencies;
  nrtrequest_data.numCalls = _helloworld_master.requests.nrt.profiling.numCalls - nrtrequest_data.numCalls;
  nrtrequest_data.numFails = _helloworld_master.requests.nrt.profiling.numFails - nrtrequest_data.numFails;
  chprintf(stream, "  num calls:   %u\n", nrtrequest_data.numCalls);
  chprintf(stream, "  num fails:   %u\n", nrtrequest_data.numFails);
  if (nrtrequest_data.numCalls > 0) {
    chprintf(stream, "  ratio:       %.2f%%\n", (1.0f - ((float)nrtrequest_data.numFails / (float)nrtrequest_data.numCalls)) * 100.0f);
    if (nrtrequest_data.numCalls > nrtrequest_data.numFails) {
      chprintf(stream, "  min latency: %u us\n", nrtrequest_data.minLatency);
      chprintf(stream, "  max latency: %u us\n", nrtrequest_data.maxLatency);
      chprintf(stream, "  avg latency: %.2f us\n", (float)nrtrequest_data.sumLatencies / (float)(nrtrequest_data.numCalls - nrtrequest_data.numFails));
      chprintf(stream, "  jitter:      %u us\n", nrtrequest_data.maxLatency - nrtrequest_data.minLatency);
    }
  }
  chprintf(stream, "  rate:        %.2f Hz\n", (float)nrtrequest_data.numCalls / duration);

  // print SRT request information
  chprintf(stream, "SRT REQUEST:\n");
  srtrequest_data.minLatency = _helloworld_master.requests.srt.profiling.minLatency;
  srtrequest_data.maxLatency = _helloworld_master.requests.srt.profiling.maxLatency;
  srtrequest_data.sumLatencies = _helloworld_master.requests.srt.profiling.sumLatencies - srtrequest_data.sumLatencies;
  srtrequest_data.numCalls = _helloworld_master.requests.srt.profiling.numCalls - srtrequest_data.numCalls;
  srtrequest_data.numFails = _helloworld_master.requests.srt.profiling.numFails - srtrequest_data.numFails;
  chprintf(stream, "  num calls:   %u\n", srtrequest_data.numCalls);
  chprintf(stream, "  num fails:   %u\n", srtrequest_data.numFails);
  if (srtrequest_data.numCalls > 0) {
    chprintf(stream, "  ratio:       %.2f%%\n", (1.0f - ((float)srtrequest_data.numFails / (float)srtrequest_data.numCalls)) * 100.0f);
    if (srtrequest_data.numCalls > srtrequest_data.numFails) {
      chprintf(stream, "  min latency: %u us\n", srtrequest_data.minLatency);
      chprintf(stream, "  max latency: %u us\n", srtrequest_data.maxLatency);
      chprintf(stream, "  avg latency: %.2f us\n", (float)srtrequest_data.sumLatencies / (float)(srtrequest_data.numCalls - srtrequest_data.numFails));
      chprintf(stream, "  jitter:      %u us\n", srtrequest_data.maxLatency - srtrequest_data.minLatency);
    }
  }
  chprintf(stream, "  rate:        %.2f Hz\n", (float)srtrequest_data.numCalls / duration);

  // print FRT request information
  chprintf(stream, "FRT REQUEST:\n");
  frtrequest_data.minLatency = _helloworld_master.requests.frt.profiling.minLatency;
  frtrequest_data.maxLatency = _helloworld_master.requests.frt.profiling.maxLatency;
  frtrequest_data.sumLatencies = _helloworld_master.requests.frt.profiling.sumLatencies - frtrequest_data.sumLatencies;
  frtrequest_data.numCalls = _helloworld_master.requests.frt.profiling.numCalls - frtrequest_data.numCalls;
  frtrequest_data.numFails = _helloworld_master.requests.frt.profiling.numFails - frtrequest_data.numFails;
  chprintf(stream, "  num calls:   %u\n", frtrequest_data.numCalls);
  chprintf(stream, "  num fails:   %u\n", frtrequest_data.numFails);
  if (frtrequest_data.numCalls > 0) {
    chprintf(stream, "  ratio:       %.2f%%\n", (1.0f - ((float)frtrequest_data.numFails / (float)frtrequest_data.numCalls)) * 100.0f);
    if (frtrequest_data.numCalls > frtrequest_data.numFails) {
      chprintf(stream, "  min latency: %u us\n", frtrequest_data.minLatency);
      chprintf(stream, "  max latency: %u us\n", frtrequest_data.maxLatency);
      chprintf(stream, "  avg latency: %.2f us\n", (float)frtrequest_data.sumLatencies / (float)(frtrequest_data.numCalls - frtrequest_data.numFails));
      chprintf(stream, "  jitter:      %u us\n", frtrequest_data.maxLatency - frtrequest_data.minLatency);
    }
  }
  chprintf(stream, "  rate:        %.2f Hz\n", (float)frtrequest_data.numCalls / duration);

  // print HRT request information
  chprintf(stream, "HRT REQUEST:\n");
  hrtrequest_data.minLatency = _helloworld_master.requests.hrt.profiling.minLatency;
  hrtrequest_data.maxLatency = _helloworld_master.requests.hrt.profiling.maxLatency;
  hrtrequest_data.sumLatencies = _helloworld_master.requests.hrt.profiling.sumLatencies - hrtrequest_data.sumLatencies;
  hrtrequest_data.numCalls = _helloworld_master.requests.hrt.profiling.numCalls - hrtrequest_data.numCalls;
  hrtrequest_data.numFails = _helloworld_master.requests.hrt.profiling.numFails - hrtrequest_data.numFails;
  chprintf(stream, "  num calls:   %u\n", hrtrequest_data.numCalls);
  chprintf(stream, "  num fails:   %u\n", hrtrequest_data.numFails);
  if (hrtrequest_data.numCalls > 0) {
    chprintf(stream, "  ratio:       %.2f%%\n", (1.0f - ((float)hrtrequest_data.numFails / (float)hrtrequest_data.numCalls)) * 100.0f);
    if (hrtrequest_data.numCalls > hrtrequest_data.numFails) {
      chprintf(stream, "  min latency: %u us\n", hrtrequest_data.minLatency);
      chprintf(stream, "  max latency: %u us\n", hrtrequest_data.maxLatency);
      chprintf(stream, "  avg latency: %.2f us\n", (float)hrtrequest_data.sumLatencies / (float)(hrtrequest_data.numCalls - hrtrequest_data.numFails));
      chprintf(stream, "  jitter:      %u us\n", hrtrequest_data.maxLatency - hrtrequest_data.minLatency);
    }
  }
  chprintf(stream, "  rate:        %.2f Hz\n", (float)frtrequest_data.numCalls / duration);

#endif /* (URT_CFG_RPC_PROFILING == true) */

  return AOS_OK;
}
#endif /* (URT_CFG_RPC_ENABLED == true) */

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup configs_helloworld
 * @{
 */

/**
 * @brief   Initializes all data applications for the HelloWorld configuration.
 */
void helloworldAppsInit(void)
{
  // initialize event sources and timers
  urtEventSourceInit(&_helloworld_triggerSource);
  urtEventSourceInit(&_helloworld_terminationSource);
  aosTimerInit(&_helloworld_triggertimer);
  aosTimerInit(&_helloworld_terminationtimer);

#if (URT_CFG_PUBSUB_ENABLED == true)
  // initialize messages and topic
  for (size_t m = 0; m < sizeof(_helloworld_messages)/sizeof(urt_message_t); ++m) {
    urtMessageInit(&_helloworld_messages[m], NULL);
    if (m > 0) {
      _helloworld_messages[m-1].next = &_helloworld_messages[m];
    }
  }
  urtTopicInit(&_helloworld_topic, HELLOWORLD_TOPICID, NULL);
  urtTopicAddMessages(&_helloworld_topic, _helloworld_messages);
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

  // initialize master and slave node
  helloWorldMasterInit(&_helloworld_master, URT_THREAD_PRIO_NORMAL_MAX, &_helloworld_triggerSource,
#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
                       HELLOWORLD_TOPICID,
#else /* (URT_CFG_PUBSUB_ENABLED == true) */
                       0,
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
                       HELLOWORLD_SERVICEID);
#else /* (URT_CFG_RPC_ENABLED == true) */
                       0);
#endif /* (URT_CFG_RPC_ENABLED == true) */

  helloWorldSlaveInit(&_helloworld_slave, URT_THREAD_PRIO_NORMAL_MIN,
#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
                      HELLOWORLD_TOPICID,
#else /* (URT_CFG_PUBSUB_ENABLED == true) */
                      0,
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
                      HELLOWORLD_SERVICEID);
#else /* (URT_CFG_RPC_ENABLED == true) */
                      0);
#endif /* (URT_CFG_RPC_ENABLED == true) */

  // add shell commands
  aosShellAddCommand(&_helloworld_messageshellcmd);
#if (URT_CFG_PUBSUB_ENABLED == true)
  aosShellAddCommand(&_helloworld_pubsubshellcmd);
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
#if (URT_CFG_RPC_ENABLED == true)
  aosShellAddCommand(&_helloworld_rpcshellcmd);
#endif /* (URT_CFG_RPC_ENABLED == true) */

  return;
}

/** @} */
