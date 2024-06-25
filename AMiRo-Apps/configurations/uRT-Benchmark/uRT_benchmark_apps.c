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
 * @file    uRT_benchmark_apps.c
 * @brief   µRT benchmark configuration application container.
 */

#include "uRT_benchmark_apps.h"

#include <urtbenchmarkconf.h>
#include <amiroos.h>
#include <uRT_benchmark.h>
#include <stdlib.h>
#include <string.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

#if !defined(URTBENCHMARK_CFG_NUMNODES)
#  error "URTBENCHMARK_CFG_NUMNODES not defined in urtbenchmarkconf.h"
#endif
#if !defined(URTBENCHMARK_CFG_REPETITIONS)
#  error "URTBENCHMARK_CFG_REPETITIONS not defined in urtbenchmarkconf.h"
#endif
#if !defined(URTBENCHMARK_CFG_FREQUENCY)
#  error "URTBENCHMARK_CFG_FREQUENCY not defined in urtbenchmarkconf.h"
#endif

#if (URT_CFG_PUBSUB_ENABLED == true) || (URT_CFG_RPC_ENABLED == true)

#if !defined(URTBENCHMARK_CFG_MAXPAYLOAD)
#  error "URTBENCHMARK_CFG_MAXPAYLOAD not defined in urtbenchmarkconf.h"
#endif
#if !defined(URTBENCHMARK_CFG_QOSTIME) &&                                       \
    (((URT_CFG_PUBSUB_ENABLED == true) &&                                       \
      ((URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) ||                           \
       (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) ||                             \
       (URT_CFG_PUBSUB_QOS_RATECHECKS == true))) ||                             \
     ((URT_CFG_RPC_ENABLED == true) &&                                          \
      ((URT_CFG_RPC_QOS_DEADLINECHECKS == true) ||                              \
       (URT_CFG_RPC_QOS_JITTERCHECKS == true))))
#  error "URTBENCHMARK_CFG_QOSTIME not defined in urtbenchmarkconf.h"
#endif

#endif /* (URT_CFG_PUBSUB_ENABLED == true) || (URT_CFG_RPC_ENABLED == true) */

#if (URT_CFG_PUBSUB_ENABLED == true)

#if !defined(URTBENCHMARK_CFG_NUMPUBLISHERS)
#  error "URTBENCHMARK_CFG_NUMPUBLISHERS not defined in urtbenchmarkconf.h"
#endif
#if !defined(URTBENCHMARK_CFG_NUMSUBSCRIBERS)
#  error "URTBENCHMARK_CFG_NUMSUBSCRIBERS not defined in urtbenchmarkconf.h"
#endif
#if !defined(URTBENCHMARK_CFG_NUMTOPICS)
#  error "URTBENCHMARK_CFG_NUMTOPICS not defined in urtbenchmarkconf.h"
#endif
#if !defined(URTBENCHMARK_CFG_NUMMESSAGES)
#  error "URTBENCHMARK_CFG_NUMMESSAGES not defined in urtbenchmarkconf.h"
#endif

#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true)

#if !defined(URTBENCHMARK_CFG_NUMREQUESTS)
#  error "URTBENCHMARK_CFG_NUMREQUESTS not defined in urtbenchmarkconf.h"
#endif
#if !defined(URTBENCHMARK_CFG_NUMSERVICES)
#  error "URTBENCHMARK_CFG_NUMSERVICES not defined in urtbenchmarkconf.h"
#endif

#endif /* (URT_CFG_RPC_ENABLED == true) */

#if (URT_CFG_PUBSUB_ENABLED == true) || (URT_CFG_RPC_ENABLED == true)

#if !defined(URTBENCHMARK_QOSPSEUDOTIME) || defined(__DOXYGEN__)
#define URTBENCHMARK_QOSPSEUDOTIME                MICROSECONDS_PER_SECOND
#endif

#endif /* (URT_CFG_PUBSUB_ENABLED == true) || (URT_CFG_RPC_ENABLED == true) */

#if !defined(URTBENCHMARK_HEADERWIDTH) || defined(__DOXYGEN__)
#define URTBENCHMARK_HEADERWIDTH                  80
#endif

#define URTBENCHMARK_INTRO()                                                    \
  /* variables */                                                               \
  aos_status_t status = AOS_INVALIDARGUMENTS;                                   \
  urt_osEventListener_t corelistener;                                           \
  urt_osEventListener_t terminationlistener;                                    \
  const urt_osEventMask_t coreevent = _urtbenchmark_nextValidEventMask(EVENT_MASK(0));  \
  const urt_osEventMask_t terminationevent = _urtbenchmark_nextValidEventMask(coreevent); \
  /* setup */                                                                   \
  urtEventListenerInit(&corelistener);                                          \
  urtEventRegister(urtCoreGetEvtSource(), &corelistener, coreevent, 0);         \
  urtEventListenerInit(&terminationlistener);                                   \
  urtEventRegister(&_urtbenchmark_timer.events.termination, &terminationlistener, terminationevent, 0); \
  /* end */

#define URTBENCHMARK_OUTRO()                                                    \
  urtEventUnregister(urtCoreGetEvtSource(), &corelistener);                     \
  urtEventUnregister(&_urtbenchmark_timer.events.termination, &terminationlistener);  \
  urtEventWait(AOS_SHELL_EVENTSMASK_COMMAND, URT_EVENT_WAIT_ANY, URT_DELAY_IMMEDIATE);  \
  /* end */

/******************************************************************************/
/* EXPORTED VARIABLES                                                         */
/******************************************************************************/

/******************************************************************************/
/* LOCAL TYPES                                                                */
/******************************************************************************/

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/

typedef struct {
  urtbenchmark_node_t nodes[URTBENCHMARK_CFG_NUMNODES];
  urtbenchmark_config_t configs[URTBENCHMARK_CFG_NUMNODES];
  urt_delay_t latencybuffer[URTBENCHMARK_CFG_NUMNODES][URTBENCHMARK_CFG_REPETITIONS];
} _urtbenchmark_data_node_t;

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)

typedef struct {
  urtbenchmark_node_t nodes[URTBENCHMARK_CFG_NUMHOPS + 1];
  urtbenchmark_config_t configs[URTBENCHMARK_CFG_NUMHOPS + 1];
  urtbenchmark_configdata_publisher_t publishers[URTBENCHMARK_CFG_NUMHOPS];
  urt_topic_t topics[URTBENCHMARK_CFG_NUMHOPS];
  urtbenchmark_configdata_subscriber_t subscribers[URTBENCHMARK_CFG_NUMHOPS];
  urt_delay_t latencybuffer[URTBENCHMARK_CFG_REPETITIONS];
} _urtbenchmark_data_pubsub_hops_t;

typedef struct {
  urtbenchmark_node_t nodes[2];
  urtbenchmark_config_t configs[2];
  struct {
    uint8_t publisher[URTBENCHMARK_CFG_MAXPAYLOAD];
    uint8_t topic[URTBENCHMARK_CFG_MAXPAYLOAD];
    uint8_t subscriber[URTBENCHMARK_CFG_MAXPAYLOAD];
  } payloads;
  urtbenchmark_configdata_publisher_t publisher;
  urt_topic_t topic;
  urtbenchmark_configdata_subscriber_t subscriber;
  urt_delay_t latencybuffer[URTBENCHMARK_CFG_REPETITIONS];
} _urtbenchmark_data_pubsub_payload_t;

typedef struct {
  urtbenchmark_node_t nodes[2];
  urtbenchmark_config_t configs[2];
  urt_message_t messages[URTBENCHMARK_CFG_NUMMESSAGES - 1];
  urtbenchmark_configdata_publisher_t publisher;
  urt_topic_t topic;
  urtbenchmark_configdata_subscriber_t subscriber;
  urt_delay_t latencybuffer[URTBENCHMARK_CFG_REPETITIONS];
} _urtbenchmark_data_pubsub_message_t;

typedef struct {
  urtbenchmark_node_t nodes[2];
  urtbenchmark_config_t configs[2];
  urt_message_t messages[URTBENCHMARK_CFG_NUMPUBLISHERS - 1];
  urtbenchmark_configdata_publisher_t publishers[URTBENCHMARK_CFG_NUMPUBLISHERS];
  urt_topic_t topic;
  urtbenchmark_configdata_subscriber_t subscriber;
  urt_delay_t latencybuffer[URTBENCHMARK_CFG_NUMPUBLISHERS * URTBENCHMARK_CFG_REPETITIONS];
} _urtbenchmark_data_pubsub_publisher_t;

typedef struct {
  urtbenchmark_node_t nodes[2];
  urtbenchmark_config_t configs[2];
  urtbenchmark_configdata_publisher_t publisher;
  urt_topic_t topic;
  urtbenchmark_configdata_subscriber_t subscribers[URTBENCHMARK_CFG_NUMSUBSCRIBERS];
  urt_delay_t latencybuffer[URTBENCHMARK_CFG_NUMSUBSCRIBERS][URTBENCHMARK_CFG_REPETITIONS];
} _urtbenchmark_data_pubsub_subscriber_t;

typedef struct {
  urtbenchmark_node_t nodes[2];
  urtbenchmark_config_t configs[2];
  urtbenchmark_configdata_publisher_t publishers[URTBENCHMARK_CFG_NUMTOPICS];
  urt_topic_t topics[URTBENCHMARK_CFG_NUMTOPICS];
  urtbenchmark_configdata_subscriber_t subscribers[URTBENCHMARK_CFG_NUMTOPICS];
  urt_delay_t latencybuffer[URTBENCHMARK_CFG_NUMTOPICS][URTBENCHMARK_CFG_REPETITIONS];
} _urtbenchmark_data_pubsub_topic_t;

#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || defined(__DOXYGEN__)
typedef struct {
  urtbenchmark_node_t nodes[2];
  urtbenchmark_config_t configs[2];
  urtbenchmark_configdata_publisher_t publisher;
  urt_topic_t topic;
  urtbenchmark_configdata_subscriber_t subscriber;
  urt_delay_t latencybuffer[URTBENCHMARK_CFG_REPETITIONS][URTBENCHMARK_CFG_QOSSTEPS];
  urt_osTime_t origintime;
} _urtbenchmark_data_pubsub_deadline_t;
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) */

#if (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || defined(__DOXYGEN__)
typedef struct {
  urtbenchmark_node_t nodes[2];
  urtbenchmark_config_t configs[2];
  urtbenchmark_configdata_publisher_t publisher;
  urt_topic_t topic;
  urtbenchmark_configdata_subscriber_t subscriber;
  urt_delay_t latencybuffer[URTBENCHMARK_CFG_REPETITIONS][URTBENCHMARK_CFG_QOSSTEPS+1];
  urt_osTime_t origintime;
} _urtbenchmark_data_pubsub_jitter_t;
#endif /* (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

#if (URT_CFG_PUBSUB_QOS_RATECHECKS == true) || defined(__DOXYGEN__)
typedef struct {
  urtbenchmark_node_t nodes[2];
  urtbenchmark_config_t configs[2];
  urtbenchmark_configdata_publisher_t publisher;
  urt_topic_t topic;
  urtbenchmark_configdata_subscriber_t subscriber;
  urt_delay_t latencybuffer[URTBENCHMARK_CFG_REPETITIONS][URTBENCHMARK_CFG_QOSSTEPS];
  urt_osTime_t origintime;
} _urtbenchmark_data_pubsub_rate_t;
#endif /* (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */

#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)

typedef struct {
  urtbenchmark_node_t nodes[URTBENCHMARK_CFG_NUMHOPS + 1];
  urtbenchmark_config_t configs[URTBENCHMARK_CFG_NUMHOPS + 1];
  urtbenchmark_configdata_request_t requests[URTBENCHMARK_CFG_NUMHOPS];
  urtbenchmark_configdata_service_t services[URTBENCHMARK_CFG_NUMHOPS];
  urt_delay_t latencybuffer[URTBENCHMARK_CFG_REPETITIONS];
} _urtbenchmark_data_rpc_hops_t;

typedef struct {
  urtbenchmark_node_t nodes[2];
  urtbenchmark_config_t configs[2];
  urtbenchmark_configdata_request_t request;
  urtbenchmark_configdata_service_t service;
  struct {
    uint8_t request[URTBENCHMARK_CFG_MAXPAYLOAD];
    uint8_t service[URTBENCHMARK_CFG_MAXPAYLOAD];
  } payloads;
  struct {
    urt_delay_t dispatch[URTBENCHMARK_CFG_REPETITIONS];
    urt_delay_t retrieve[URTBENCHMARK_CFG_REPETITIONS];
  } latencybuffers;
} _urtbenchmark_data_rpc_payload_t;

typedef struct {
  urtbenchmark_node_t nodes[2];
  urtbenchmark_config_t configs[2];
  urtbenchmark_configdata_request_t requests[URTBENCHMARK_CFG_NUMREQUESTS];
  urtbenchmark_configdata_service_t service;
  struct {
    urt_delay_t dispatch[URTBENCHMARK_CFG_NUMREQUESTS * URTBENCHMARK_CFG_REPETITIONS];
    urt_delay_t retrieve[URTBENCHMARK_CFG_NUMREQUESTS][URTBENCHMARK_CFG_REPETITIONS];
  } latencybuffers;
} _urtbenchmark_data_rpc_request_t;

typedef struct {
  urtbenchmark_node_t nodes[2];
  urtbenchmark_config_t configs[2];
  urtbenchmark_configdata_request_t requests[URTBENCHMARK_CFG_NUMSERVICES];
  urtbenchmark_configdata_service_t services[URTBENCHMARK_CFG_NUMSERVICES];
  struct {
    urt_delay_t dispatch[URTBENCHMARK_CFG_NUMSERVICES][URTBENCHMARK_CFG_REPETITIONS];
    urt_delay_t retrieve[URTBENCHMARK_CFG_NUMSERVICES][URTBENCHMARK_CFG_REPETITIONS];
  } latencybuffers;
} _urtbenchmark_data_rpc_service_t;

#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true) || defined(__DOXYGEN__)
typedef struct {
  urtbenchmark_node_t nodes[2];
  urtbenchmark_config_t configs[2];
  urtbenchmark_configdata_request_t request;
  urtbenchmark_configdata_service_t service;
  struct {
    urt_delay_t dispatch[URTBENCHMARK_CFG_REPETITIONS][URTBENCHMARK_CFG_QOSSTEPS];
    urt_delay_t retrieve[URTBENCHMARK_CFG_REPETITIONS][URTBENCHMARK_CFG_QOSSTEPS];
  } latencybuffers;
  urt_delay_t delaybuffer;
} _urtbenchmark_data_rpc_deadline_t;
#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) */

#if (URT_CFG_RPC_QOS_JITTERCHECKS == true) || defined(__DOXYGEN__)
typedef struct {
  urtbenchmark_node_t nodes[2];
  urtbenchmark_config_t configs[2];
  urtbenchmark_configdata_request_t request;
  urtbenchmark_configdata_service_t service;
  struct {
    urt_delay_t dispatch[URTBENCHMARK_CFG_REPETITIONS][URTBENCHMARK_CFG_QOSSTEPS+1];
    urt_delay_t retrieve[URTBENCHMARK_CFG_REPETITIONS][URTBENCHMARK_CFG_QOSSTEPS+1];
  } latencybuffers;
  urt_delay_t delaybuffer;
} _urtbenchmark_data_rpc_jitter_t;
#endif /* (URT_CFG_RPC_QOS_JITTERCHECKS == true) */

#endif /* (URT_CFG_RPC_ENABLED == true) */

static union {
  _urtbenchmark_data_node_t node;

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
  union {
    _urtbenchmark_data_pubsub_hops_t hops;
    _urtbenchmark_data_pubsub_payload_t payload;
    _urtbenchmark_data_pubsub_message_t message;
    _urtbenchmark_data_pubsub_publisher_t publisher;
    _urtbenchmark_data_pubsub_subscriber_t subscriber;
    _urtbenchmark_data_pubsub_topic_t topic;
#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || defined(__DOXYGEN__)
    _urtbenchmark_data_pubsub_deadline_t deadline;
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) */
#if (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || defined(__DOXYGEN__)
    _urtbenchmark_data_pubsub_jitter_t jitter;
#endif /* (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */
#if (URT_CFG_PUBSUB_QOS_RATECHECKS == true) || defined(__DOXYGEN__)
    _urtbenchmark_data_pubsub_rate_t rate;
#endif /* (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */
  } pubsub;
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
  union {
    _urtbenchmark_data_rpc_hops_t hops;
    _urtbenchmark_data_rpc_payload_t payload;
    _urtbenchmark_data_rpc_request_t request;
    _urtbenchmark_data_rpc_service_t service;
#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true) || defined(__DOXYGEN__)
    _urtbenchmark_data_rpc_deadline_t deadline;
#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) */
#if (URT_CFG_RPC_QOS_JITTERCHECKS == true) || defined(__DOXYGEN__)
    _urtbenchmark_data_rpc_jitter_t jitter;
#endif /* (URT_CFG_RPC_QOS_JITTERCHECKS == true) */
  } rpc;
#endif /* (URT_CFG_RPC_ENABLED == true) */
} _urtbenchmark_data;

static struct {
  aos_timer_t timer;
  urt_osTime_t time;
  struct {
    urt_osEventSource_t trigger;
    urt_osEventSource_t termination;
  } events;
  size_t counter;
  size_t iterations;
} _urtbenchmark_timer;

/*
 * forward declarations
 */
static int _urtbenchmark_shellcb_memory(BaseSequentialStream* stream, int argc, const char* argv[]);
static int _urtbenchmark_shellcb_node(BaseSequentialStream* stream, int argc, const char* argv[]);
#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
static int _urtbenchmark_shellcb_pubsub(BaseSequentialStream* stream, int argc, const char* argv[]);
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
static int _urtbenchmark_shellcb_rpc(BaseSequentialStream* stream, int argc, const char* argv[]);
#endif /* (URT_CFG_RPC_ENABLED == true) */
static int _urtbenchmark_shellcb_all(BaseSequentialStream* stream, int argc, const char* argv[]);

static AOS_SHELL_COMMAND(_urtbenchmark_shellcmd_memory, "uRT-Benchmark:Memory", _urtbenchmark_shellcb_memory);
static AOS_SHELL_COMMAND(_urtbenchmark_shellcmd_node, "uRT-Benchmark:Node", _urtbenchmark_shellcb_node);
#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
static AOS_SHELL_COMMAND(_urtbenchmark_shellcmd_pubsub, "uRT-Benchmark:PubSub", _urtbenchmark_shellcb_pubsub);
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
static AOS_SHELL_COMMAND(_urtbenchmark_shellcmd_rpc, "uRT-Benchmark:RPC", _urtbenchmark_shellcb_rpc);
#endif /* (URT_CFG_RPC_ENABLED == true) */
static AOS_SHELL_COMMAND(_urtbenchmark_shellcmd_all, "uRT-Benchmark:all", _urtbenchmark_shellcb_all);

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

urt_osEventMask_t _urtbenchmark_nextValidEventMask(urt_osEventMask_t mask)
{
  aosDbgAssert(AOS_SHELL_EVENTSMASK_COMMAND != 0);

  while (mask & AOS_SHELL_EVENTSMASK_PROHIBITED) {
    mask <<= 1;
  }
  return mask;
}

bool _urtbenchmark_shellcb_checkXRT(const char* s)
{
  if (s == NULL) {
    return false;
  } else {
    return (strcmp(s, "NRT") == 0 || strcmp(s, "nrt") == 0 ||
            strcmp(s, "SRT") == 0 || strcmp(s, "srt") == 0 ||
            strcmp(s, "FRT") == 0 || strcmp(s, "frt") == 0 ||
            strcmp(s, "HRT") == 0 || strcmp(s, "hrt") == 0);
  }
}

#if (URT_CFG_PUBSUB_ENABLED == true || URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
urt_rtclass_t _urtbenchmark_shellcb_extractRtClass(const char* s)
{
  if (strcmp(s, "NRT") == 0 || strcmp(s, "nrt") == 0) {
    return URT_NRT;
  } else if (strcmp(s, "SRT") == 0 || strcmp(s, "srt") == 0) {
    return URT_SRT;
  } else if (strcmp(s, "FRT") == 0 || strcmp(s, "frt") == 0) {
    return URT_FRT;
  } else {
    aosDbgAssert(strcmp(s, "HRT") == 0 || strcmp(s, "hrt") == 0);
    return URT_HRT;
  }
}
#endif /* (URT_CFG_PUBSUB_ENABLED == true || URT_CFG_RPC_ENABLED == true) */

bool _urtbenchmark_shellcb_checkStress(const char* s)
{
  if (s == NULL) {
    return false;
  } else {
    return (strcmp(s, "--stress") == 0 || strcmp(s, "-s") == 0);
  }
}

#if (URT_CFG_PUBSUB_ENABLED == true || URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)
char _urtbenchmark_rtClass2Char(const urt_rtclass_t rt)
{
  switch (rt) {
    case URT_NRT:
      return 'N';
    case URT_SRT:
      return 'S';
    case URT_FRT:
      return 'F';
    case URT_HRT:
      return 'H';
  }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
}
#pragma GCC diagnostic pop
#endif /* (URT_CFG_PUBSUB_ENABLED == true || URT_CFG_RPC_ENABLED == true) */

void _urtbenchmark_waitForTermination(const urt_osEventMask_t event, const bool stress)
{
  aosDbgCheck(event != 0);

  if (stress) {
    // poll actively to stress CPU
    while ((urtEventWait(event, URT_EVENT_WAIT_ANY, URT_DELAY_IMMEDIATE) & event) != event) {
      continue;
    }
  } else {
    // wait for event
    urtEventWait(event, URT_EVENT_WAIT_ANY, URT_DELAY_INFINITE);
  }
}

static void _urtbenchmark_triggercb(virtual_timer_t *timer, void* params)
{
  (void)timer;
  (void)params;

  // if this is not the final repetition
  if (_urtbenchmark_timer.counter < _urtbenchmark_timer.iterations) {
    // fire event
    chEvtBroadcastI(&_urtbenchmark_timer.events.trigger);
    // increment counter
    ++_urtbenchmark_timer.counter;
    // set synthetic information origin time
    aosSysGetUptimeX(&_urtbenchmark_timer.time);
  }

  // if this is the final repetition
  else {
    // reset timer and broadcast termination event
    aosTimerResetI(&_urtbenchmark_timer.timer);
    chEvtBroadcastI(&_urtbenchmark_timer.events.termination);
  }

  return;
}

#if (URT_CFG_PUBSUB_ENABLED == true && (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true || URT_CFG_PUBSUB_QOS_JITTERCHECKS == true || URT_CFG_PUBSUB_QOS_RATECHECKS == true)) || defined(__DOXYGEN__)
static void _urtbenchmark_triggercb_pubsubqos(virtual_timer_t* timer, void* starttime)
{
  (void)timer;

  // if this is not the final repetition
  if (_urtbenchmark_timer.counter < _urtbenchmark_timer.iterations) {
    // increment counter
    ++_urtbenchmark_timer.counter;
    // set synthetic information origin time to last trigger event
    urtTimeSet((urt_osTime_t*)starttime, urtTimeGet(&_urtbenchmark_timer.time));
    urtTimeAdd((urt_osTime_t*)starttime,
               (((_urtbenchmark_timer.counter-1) * _urtbenchmark_timer.counter) / 2) *
               ((URTBENCHMARK_CFG_QOSTIME + URTBENCHMARK_CFG_QOSSTEPS-1) / URTBENCHMARK_CFG_QOSSTEPS));
    // restart single-shot timer
    urt_osTime_t time = _urtbenchmark_timer.time;
    urtTimeAdd(&time, (((_urtbenchmark_timer.counter+1) * (_urtbenchmark_timer.counter+2)) / 2) *
               ((URTBENCHMARK_CFG_QOSTIME + URTBENCHMARK_CFG_QOSSTEPS-1) / URTBENCHMARK_CFG_QOSSTEPS));
    aosTimerSetAbsoluteI(&_urtbenchmark_timer.timer, time, _urtbenchmark_triggercb_pubsubqos, starttime);
    // fire event
    chEvtBroadcastI(&_urtbenchmark_timer.events.trigger);
  }

  // if this is the final repetition
  else {
    // reset timer and broadcast termination event
    aosTimerResetI(&_urtbenchmark_timer.timer);
    chEvtBroadcastI(&_urtbenchmark_timer.events.termination);
  }

  return;
}
#endif /* (URT_CFG_PUBSUB_ENABLED == true && (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true || URT_CFG_PUBSUB_QOS_JITTERCHECKS == true || URT_CFG_PUBSUB_QOS_RATECHECKS == true)) */

#if (URT_CFG_RPC_ENABLED == true && (URT_CFG_RPC_QOS_DEADLINECHECKS == true || URT_CFG_RPC_QOS_JITTERCHECKS == true)) || defined(__DOXYGEN__)
static void _urtbenchmark_triggercb_rpcqos(virtual_timer_t* timer, void* delay)
{
  (void)timer;

  // if this is not the final repetition
  if (_urtbenchmark_timer.counter < _urtbenchmark_timer.iterations) {
    // increment counter
    ++_urtbenchmark_timer.counter;
    // set synthetic delay
    *(urt_delay_t*)delay = _urtbenchmark_timer.counter * ((URTBENCHMARK_CFG_QOSTIME + URTBENCHMARK_CFG_QOSSTEPS-1) / URTBENCHMARK_CFG_QOSSTEPS);
    // restart single-shot timer
    urt_osTime_t time = _urtbenchmark_timer.time;
    urtTimeAdd(&time, (((_urtbenchmark_timer.counter+1) * (_urtbenchmark_timer.counter+2)) / 2) *
               ((URTBENCHMARK_CFG_QOSTIME + URTBENCHMARK_CFG_QOSSTEPS-1) / URTBENCHMARK_CFG_QOSSTEPS));
    aosTimerSetAbsoluteI(&_urtbenchmark_timer.timer, time, _urtbenchmark_triggercb_rpcqos, delay);
    // fire event
    chEvtBroadcastI(&_urtbenchmark_timer.events.trigger);
  }

  // if this is the final repetition
  else {
    // reset timer and broadcast termination event
    aosTimerResetI(&_urtbenchmark_timer.timer);
    chEvtBroadcastI(&_urtbenchmark_timer.events.termination);
  }

  return;
}
#endif /* (URT_CFG_RPC_ENABLED == true && (URT_CFG_RPC_QOS_DEADLINECHECKS == true || URT_CFG_RPC_QOS_JITTERCHECKS == true)) */

void _urtbenchmark_reset(void)
{
  // extern declarations
  extern urt_node_t* _urt_core_nodes;
  extern urt_status_t _urt_core_status;
#if (URT_CFG_PUBSUB_ENABLED == true)
  extern urt_topic_t* _urt_core_topics;
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
#if (URT_CFG_RPC_ENABLED == true)
  extern urt_service_t* _urt_core_services;
#endif /* (URT_CFG_RPC_ENABLED == true) */

  // reset core
  _urt_core_nodes = NULL;
  _urt_core_status = URT_STATUS_OK;
#if (URT_CFG_PUBSUB_ENABLED == true)
  _urt_core_topics = NULL;
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
#if (URT_CFG_RPC_ENABLED == true)
  _urt_core_services = NULL;
#endif /* (URT_CFG_RPC_ENABLED == true) */

  // disacrd any pending events
  urtEventWait(AOS_SHELL_EVENTSMASK_COMMAND, URT_EVENT_WAIT_ANY, URT_DELAY_IMMEDIATE);

  return;
}

int _urtbenchmark_printHeading(BaseSequentialStream* stream, const char heading[], const char c, const size_t width)
{
  aosDbgAssert(stream != NULL);
  aosDbgAssert(heading != NULL);

  // local variabls
  size_t chars = 0;

  // print some delimiters before heading
  for (size_t i = 0; i < 3; ++i) {
    streamPut(stream, c);
    ++chars;
  }
  streamPut(stream, ' ');
  ++chars;
  // print heading
  chars += chprintf(stream, heading);
  // fill up with delimiters
  if (chars< width) {
    streamPut(stream, ' ');
    ++chars;
  }
  while (chars < width) {
    streamPut(stream, c);
    ++chars;
  }
  streamPut(stream, '\n');

  return chars;
}

#if ((URT_CFG_PUBSUB_ENABLED == true) || (URT_CFG_RPC_ENABLED == true)) || defined(__DOXYGEN__)
float _urtbenchmark_usefulnesscb(urt_delay_t dt, void* params)
{
  (void)dt;
  (void)params;

  return 1.0f;
}
#endif /* (URT_CFG_PUBSUB_ENABLED == true) || (URT_CFG_RPC_ENABLED == true) */

/* memory benchmark ************************************************************/

int _urtbenchmark_memory(BaseSequentialStream* const stream)
{
  // print output table header
  chprintf(stream, "MODULE/TYPE\tBYTES\n");

  // core components
  {
    size_t size = 0;
    extern urt_node_t* _urt_core_nodes;
    size += sizeof(_urt_core_nodes);
    extern urt_osEventSource_t _urt_core_evtSource;
    size += sizeof(_urt_core_evtSource);
    extern urt_osMutex_t _urt_core_lock;
    size += sizeof(_urt_core_lock);
    extern urt_status_t _urt_core_status;
    size += sizeof(_urt_core_status);
    extern urt_sync_t* _urt_core_sync;
    size += sizeof(_urt_core_sync);
#if (URT_CFG_PUBSUB_ENABLED == true)
    extern urt_topic_t* _urt_core_topics;
    size += sizeof(_urt_core_topics);
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
#if (URT_CFG_RPC_ENABLED == true)
    extern urt_service_t* _urt_core_services;
    size += sizeof(_urt_core_services);
#endif /* (URT_CFG_RPC_ENABLED == true) */
    chprintf(stream, "core\t%u\n", size);
  }
  chprintf(stream, "node\t%u\n", sizeof(urt_node_t));
#if (URT_CFG_SYNCGROUPS_ENABLED == true)
  chprintf(stream, "syncnode\t%u\n", sizeof(urt_syncnode_t));
  chprintf(stream, "syncgroup\t%u\n", sizeof(urt_syncgroup_t));
#endif /* (URT_CFG_SYNCGROUPS_ENABLED == true) */

#if (URT_CFG_PUBSUB_ENABLED == true)
  // publish-subscribe components
  chprintf(stream, "message\t%u\n", sizeof(urt_message_t));
  chprintf(stream, "topic\t%u\n", sizeof(urt_topic_t));
  chprintf(stream, "publisher\t%u\n", sizeof(urt_publisher_t));
  chprintf(stream, "NRT subscriber\t%u\n", sizeof(urt_nrtsubscriber_t));
  chprintf(stream, "SRT subscriber\t%u\n", sizeof(urt_srtsubscriber_t));
  chprintf(stream, "FRT subscriber\t%u\n", sizeof(urt_frtsubscriber_t));
  chprintf(stream, "HRT subscriber\t%u\n", sizeof(urt_hrtsubscriber_t));
  chprintf(stream, "xRT subscriber\t%u\n", sizeof(urt_subscriber_t));
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true)
  // RPC components
  chprintf(stream, "service\t%u\n", sizeof(urt_service_t));
  chprintf(stream, "NRT request\t%u\n", sizeof(urt_nrtrequest_t));
  chprintf(stream, "SRT request\t%u\n", sizeof(urt_srtrequest_t));
  chprintf(stream, "FRT request\t%u\n", sizeof(urt_frtrequest_t));
  chprintf(stream, "HRT request\t%u\n", sizeof(urt_hrtrequest_t));
  chprintf(stream, "xRT request\t%u\n", sizeof(urt_request_t));
#endif /* (URT_CFG_RPC_ENABLED == true) */

  return AOS_OK;
}

int _urtbenchmark_shellcb_memory(BaseSequentialStream* stream, int argc, const char* argv[])
{
  (void)argc;
  (void)argv;

  return _urtbenchmark_memory(stream);
}

/* node benchmarks *************************************************************/

int _urtbenchmark_node(BaseSequentialStream* const stream,
                       const urt_osEventMask_t coreevent,
                       const urt_osEventMask_t terminationevent,
                       const bool stress)
{
  // local constants
  _urtbenchmark_data_node_t* const bmd = &_urtbenchmark_data.node;

  // print intro
  chprintf(stream, "CPU stress: %s\n", stress ? "enabled" : "disabled");

  // print output table header
  chprintf(stream, "NODES\tNODE\tITERATION\tLATENCY\n");

  // iteratively increase number of nodes
  for (size_t nodes = 1; nodes <= URTBENCHMARK_CFG_NUMNODES; ++nodes) {
    // prepare nodes
    for (size_t node = 0; node < nodes; ++node) {
      ubnConfigInit(&bmd->configs[node]);
#if (CH_CFG_USE_REGISTRY == TRUE)
      chsnprintf(bmd->configs[node].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "node#%u", node+1);
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
      bmd->configs[node].node.trigger = &_urtbenchmark_timer.events.trigger;
      bmd->configs[node].node.time = &_urtbenchmark_timer.time;
      bmd->configs[node].node.latencybuffer = bmd->latencybuffer[node];
      memset(&bmd->latencybuffer[node], 0, sizeof(bmd->latencybuffer[node]));
      bmd->configs[node].node.type = URTBENCHMARK_NODE;
      ubnInit(&bmd->nodes[node], URT_THREAD_PRIO_HIGH_MAX, &bmd->configs[node]);
    }

    // start all nodes
    urtCoreStartNodes();
    urtEventWait(coreevent, URT_EVENT_WAIT_ANY, URT_DELAY_INFINITE);

    // arm the trigger
    _urtbenchmark_timer.counter = 0;
    _urtbenchmark_timer.iterations = URTBENCHMARK_CFG_REPETITIONS;
    aosTimerPeriodicInterval(&_urtbenchmark_timer.timer, MICROSECONDS_PER_SECOND / URTBENCHMARK_CFG_FREQUENCY, _urtbenchmark_triggercb, NULL);

    // wait for benchmark termination
    _urtbenchmark_waitForTermination(terminationevent, stress);

    // stop all nodes and reset
    urtCoreStopNodes(URT_STATUS_OK);
    for (size_t node = 0; node < nodes; ++node) {
      urtThreadJoin(bmd->nodes[node].node.thread);
    }
    _urtbenchmark_reset();

    // print latencies
    for (size_t node = 0; node < nodes; ++node) {
      for (size_t i = 0; i < URTBENCHMARK_CFG_REPETITIONS; ++i) {
        chprintf(stream, "%5u\t%4u\t%9u\t%7u\n", nodes, node+1, i+1, bmd->latencybuffer[node][i]);
      }
    }
  }

  return AOS_OK;
}

int _urtbenchmark_shellcb_node(BaseSequentialStream* stream, int argc, const char* argv[])
{
  URTBENCHMARK_INTRO();

  // print help
  if (aosShellcmdCheckHelp(argc, argv, true) == AOS_SHELL_HELPREQUEST_EXPLICIT) {
    chprintf(stream, "usage: %s [--stress|-s]\n", argv[0]);
    chprintf(stream, "--stress, -s (optional):\n");
    chprintf(stream, "  Stress CPU during benchmark.\n");
    status = AOS_OK;
  } else {
    // benchmark node scaling
    status = _urtbenchmark_node(stream, coreevent, terminationevent, _urtbenchmark_shellcb_checkStress(argv[1]));
  }

  URTBENCHMARK_OUTRO();

  return status;
}

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)

/* publish-subscribe benchmarks ************************************************/

void _urtbenchmark_prepare_publisher(urtbenchmark_configdata_publisher_t* const publisher,
                                     urt_osTime_t* const time,
                                     urt_publisher_publishpolicy_t policy,
                                     uint8_t* payload_buffer,
                                     size_t payload_size,
                                     urt_topicid_t topicid)
{
  urtDebugAssert(publisher != NULL);

  publisher->time = time;
  publisher->policy = policy;
  publisher->payload.buffer = payload_buffer;
  publisher->payload.size = payload_size;
  publisher->topicid = topicid;

  return;
}

void _urtbenchmark_prepare_subscriber(urtbenchmark_configdata_subscriber_t* const subscriber,
                                      urt_delay_t* latencybuffer,
                                      uint8_t* payload_buffer,
                                      size_t payload_size,
                                      urt_topicid_t topicid,
                                      urt_rtclass_t rtclass)
{
  urtDebugAssert(subscriber != NULL);

  switch (rtclass) {
    case URT_NRT:
      break;
    case URT_SRT:
      subscriber->srtsubscriber.usefulnesscb = _urtbenchmark_usefulnesscb;
      subscriber->srtsubscriber.params = NULL;
      break;
    case URT_FRT:
      subscriber->frtsubscriber.deadline = URTBENCHMARK_QOSPSEUDOTIME;
      subscriber->frtsubscriber.jitter = URTBENCHMARK_QOSPSEUDOTIME;
      break;
    case URT_HRT:
      subscriber->hrtsubscriber.deadline = URTBENCHMARK_QOSPSEUDOTIME;
      subscriber->hrtsubscriber.jitter = URTBENCHMARK_QOSPSEUDOTIME;
      subscriber->hrtsubscriber.rate = URTBENCHMARK_QOSPSEUDOTIME;
      break;
  }
  subscriber->latencybuffer = latencybuffer;
  subscriber->payload.buffer = payload_buffer;
  subscriber->payload.size = payload_size;
  subscriber->topicid = topicid;
  subscriber->rtclass = rtclass;

  return;
}

int _urtbenchmark_pubsub_hops(BaseSequentialStream* const stream,
                              const urt_osEventMask_t coreevent,
                              const urt_osEventMask_t terminationevent,
                              const urt_rtclass_t rt,
                              const bool stress)
{
  // local constants
  _urtbenchmark_data_pubsub_hops_t* const bmd = &_urtbenchmark_data.pubsub.hops;

  // print intro
  chprintf(stream, "publisher -> %cRT subscriber | publisher -> ... -> %cRT subscriber\n", _urtbenchmark_rtClass2Char(rt), _urtbenchmark_rtClass2Char(rt));
  chprintf(stream, "CPU stress: %s\n", stress ? "enabled" : "disabled");

  // print output table header
  chprintf(stream, "HOPS\tITERATION\tLATENCY\n");

  // iteratively increase the number of hops
  for (size_t hops = 1; hops <= URTBENCHMARK_CFG_NUMHOPS; ++hops) {
    // prepare topics
    for (size_t topic = 0; topic < hops; ++topic) {
      urtTopicInit(&bmd->topics[topic], topic, NULL);
    }

    // prepare publishers
    for (size_t publisher = 0; publisher < hops; ++publisher) {
      _urtbenchmark_prepare_publisher(&bmd->publishers[publisher],
                                      NULL, URT_PUBLISHER_PUBLISH_ENFORCING, NULL, 0, publisher);
    }

    // prepare subscribers
    for (size_t subscriber = 0; subscriber < hops; ++subscriber) {
      _urtbenchmark_prepare_subscriber(&bmd->subscribers[subscriber],
                                       (subscriber == hops - 1) ? bmd->latencybuffer : NULL,
                                       NULL, 0, subscriber, rt);
    }
    memset(bmd->latencybuffer, 0, sizeof(bmd->latencybuffer));

    // prepare nodes
    for (size_t node = 0; node <= hops; ++node) {
      ubnConfigInit(&bmd->configs[node]);
#if (CH_CFG_USE_REGISTRY == TRUE)
      chsnprintf(bmd->configs[node].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "node%u", node+1);
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
      // set trigger for first node
      if (node == 0) {
        bmd->configs[node].node.trigger = &_urtbenchmark_timer.events.trigger;
      }
      bmd->configs[node].node.type = URTBENCHMARK_PUBSUB_HOPS;
      // add publisher to all but the last node
      if (node < hops) {
        bmd->configs[node].publishers.buffer = &bmd->publishers[node];
        bmd->configs[node].publishers.size = 1;
      }
      // add subscriber to all but the first node
      if (node > 0) {
        bmd->configs[node].subscribers.buffer = &bmd->subscribers[node-1];
        bmd->configs[node].subscribers.size = 1;
      }
      bmd->configs[node].node.type = URTBENCHMARK_PUBSUB_HOPS;
      ubnInit(&bmd->nodes[node], URT_THREAD_PRIO_HIGH_MAX-1, &bmd->configs[node]);
    }

    // start all nodes
    urtCoreStartNodes();
    urtEventWait(coreevent, URT_EVENT_WAIT_ANY, URT_DELAY_INFINITE);

    // arm trigger timer
    _urtbenchmark_timer.counter = 0;
    _urtbenchmark_timer.iterations = URTBENCHMARK_CFG_REPETITIONS;
    aosTimerPeriodicInterval(&_urtbenchmark_timer.timer, MICROSECONDS_PER_SECOND / URTBENCHMARK_CFG_FREQUENCY, _urtbenchmark_triggercb, NULL);

    // wait for benchmark termination
    _urtbenchmark_waitForTermination(terminationevent, stress);

    // stop all nodes and reset
    urtCoreStopNodes(URT_STATUS_OK);
    for (size_t node = 0; node <= hops; ++node) {
      urtThreadJoin(bmd->nodes[node].node.thread);
    }
    _urtbenchmark_reset();

    // print all latencies
    for (size_t i = 0; i < URTBENCHMARK_CFG_REPETITIONS; ++i) {
      chprintf(stream, "%4u\t%9u\t%7u\n", hops, i+1, bmd->latencybuffer[i]);
    }
  }

  return AOS_OK;
}

int _urtbenchmark_pubsub_payload(BaseSequentialStream* const stream,
                                 const urt_osEventMask_t coreevent,
                                 const urt_osEventMask_t terminationevent,
                                 const urt_rtclass_t rt,
                                 const bool stress)
{
  // local constants
  _urtbenchmark_data_pubsub_payload_t* const bmd = &_urtbenchmark_data.pubsub.payload;
  const urt_topicid_t topicid = 0;

  // print intro
  chprintf(stream, "publisher -> topic -> %cRT subscriber\n", _urtbenchmark_rtClass2Char(rt));
  chprintf(stream, "CPU stress: %s\n", stress ? "enabled" : "disabled");

  // print output table header
  chprintf(stream, "PAYLOAD\tITERATION\tLATENCY\n");

  // iteratively increase payload
  for (size_t payload = 0; payload <= URTBENCHMARK_CFG_MAXPAYLOAD; payload += sizeof(size_t)) {
    // prepare topic
    memset(bmd->payloads.topic, 0, sizeof(bmd->payloads.topic));
    urtTopicInit(&bmd->topic, topicid, (payload == 0) ? NULL : bmd->payloads.topic);

    // prepare publisher node
    _urtbenchmark_prepare_publisher(&bmd->publisher, NULL, URT_PUBLISHER_PUBLISH_ENFORCING, (payload == 0) ? NULL : bmd->payloads.publisher, payload, topicid);
    memset(bmd->payloads.publisher, payload % 0x0100, URTBENCHMARK_CFG_MAXPAYLOAD);
    ubnConfigInit(&bmd->configs[0]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[0].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "publisher");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[0].node.trigger = &_urtbenchmark_timer.events.trigger;
    bmd->configs[0].publishers.buffer = &bmd->publisher;
    bmd->configs[0].publishers.size = 1;
    bmd->configs[0].node.type = URTBENCHMARK_PUBSUB_SCALING;
    ubnInit(&bmd->nodes[0], URT_THREAD_PRIO_HIGH_MAX, &bmd->configs[0]);

    // prepare subscriber node
    _urtbenchmark_prepare_subscriber(&bmd->subscriber, bmd->latencybuffer, (payload == 0) ? NULL : bmd->payloads.subscriber, payload, topicid, rt);
    memset(bmd->latencybuffer, 0, sizeof(bmd->latencybuffer));
    memset(bmd->payloads.subscriber, 0, URTBENCHMARK_CFG_MAXPAYLOAD);
    ubnConfigInit(&bmd->configs[1]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[1].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "subscriber");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[1].subscribers.buffer = &bmd->subscriber;
    bmd->configs[1].subscribers.size = 1;
    bmd->configs[1].node.type = URTBENCHMARK_PUBSUB_SCALING;
    ubnInit(&bmd->nodes[1], URT_THREAD_PRIO_HIGH_MAX-1, &bmd->configs[1]);

    // start all nodes
    urtCoreStartNodes();
    urtEventWait(coreevent, URT_EVENT_WAIT_ANY, URT_DELAY_INFINITE);

    // arm trigger timer
    _urtbenchmark_timer.counter = 0;
    _urtbenchmark_timer.iterations = URTBENCHMARK_CFG_REPETITIONS;
    aosTimerPeriodicInterval(&_urtbenchmark_timer.timer, MICROSECONDS_PER_SECOND / URTBENCHMARK_CFG_FREQUENCY, _urtbenchmark_triggercb, NULL);

    // wait for benchmark termination
    _urtbenchmark_waitForTermination(terminationevent, stress);

    // stop all nodes and reset
    urtCoreStopNodes(URT_STATUS_OK);
    urtThreadJoin(bmd->nodes[0].node.thread);
    urtThreadJoin(bmd->nodes[1].node.thread);
    _urtbenchmark_reset();

    // print all latencies
    for (size_t i = 0; i < URTBENCHMARK_CFG_REPETITIONS; ++i) {
      chprintf(stream, "%7u\t%9u\t%7u\n", payload, i+1, bmd->latencybuffer[i]);
    }
  }

  return AOS_OK;
}

int _urtbenchmark_pubsub_message(BaseSequentialStream* const stream,
                                 const urt_osEventMask_t coreevent,
                                 const urt_osEventMask_t terminationevent,
                                 const urt_rtclass_t rt,
                                 const bool stress)
{
  // local constants
  _urtbenchmark_data_pubsub_message_t* const bmd = &_urtbenchmark_data.pubsub.message;
  const urt_topicid_t topicid = 0;

  // print intro
  chprintf(stream, "publisher -> topic -> %cRT subscriber\n", _urtbenchmark_rtClass2Char(rt));
  chprintf(stream, "CPU stress: %s\n", stress ? "enabled" : "disabled");

  // print output table header
  chprintf(stream, "MESSAGES\tITERATION\tLATENCY\n");

  // iteratively increase message buffer
  for (size_t messages = 1; messages <= URTBENCHMARK_CFG_NUMMESSAGES; ++messages) {
    // prepare topic and add messages
    urtTopicInit(&bmd->topic, topicid, NULL);
    for (size_t m = 0; m < messages-1; ++m) {
      urtMessageInit(&bmd->messages[m], NULL);
      urtTopicAddMessages(&bmd->topic, &bmd->messages[m]);
    }

    // prepare publisher node
    _urtbenchmark_prepare_publisher(&bmd->publisher, NULL, URT_PUBLISHER_PUBLISH_ENFORCING, NULL, 0, topicid);
    ubnConfigInit(&bmd->configs[0]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[0].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "publisher");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[0].node.trigger = &_urtbenchmark_timer.events.trigger;
    bmd->configs[0].publishers.buffer = &bmd->publisher;
    bmd->configs[0].publishers.size = 1;
    bmd->configs[0].node.type = URTBENCHMARK_PUBSUB_SCALING;
    ubnInit(&bmd->nodes[0], URT_THREAD_PRIO_HIGH_MAX, &bmd->configs[0]);

    // prepare subscriber node
    _urtbenchmark_prepare_subscriber(&bmd->subscriber, bmd->latencybuffer, NULL, 0, topicid, rt);
    memset(bmd->latencybuffer, 0, sizeof(bmd->latencybuffer));
    ubnConfigInit(&bmd->configs[1]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[1].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "subscriber");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[1].subscribers.buffer = &bmd->subscriber;
    bmd->configs[1].subscribers.size = 1;
    bmd->configs[1].node.type = URTBENCHMARK_PUBSUB_SCALING;
    ubnInit(&bmd->nodes[1], URT_THREAD_PRIO_HIGH_MAX-1, &bmd->configs[1]);

    // start all nodes
    urtCoreStartNodes();
    urtEventWait(coreevent, URT_EVENT_WAIT_ANY, URT_DELAY_INFINITE);

    // arm trigger timer
    _urtbenchmark_timer.counter = 0;
    _urtbenchmark_timer.iterations = URTBENCHMARK_CFG_REPETITIONS;
    aosTimerPeriodicInterval(&_urtbenchmark_timer.timer, MICROSECONDS_PER_SECOND / URTBENCHMARK_CFG_FREQUENCY, _urtbenchmark_triggercb, NULL);

    // wait for benchmark termination
    _urtbenchmark_waitForTermination(terminationevent, stress);

    // stop all nodes and reset
    urtCoreStopNodes(URT_STATUS_OK);
    urtThreadJoin(bmd->nodes[0].node.thread);
    urtThreadJoin(bmd->nodes[1].node.thread);
    _urtbenchmark_reset();

    // print all latencies
    for (size_t i = 0; i < URTBENCHMARK_CFG_REPETITIONS; ++i) {
      chprintf(stream, "%8u\t%9u\t%7u\n", messages, i+1, bmd->latencybuffer[i]);
    }
  }

  return AOS_OK;
}

int _urtbenchmark_pubsub_publisher(BaseSequentialStream* const stream,
                                   const urt_osEventMask_t coreevent,
                                   const urt_osEventMask_t terminationevent,
                                   const urt_rtclass_t rt,
                                   const bool stress)
{
  // local constants
  _urtbenchmark_data_pubsub_publisher_t* const bmd = &_urtbenchmark_data.pubsub.publisher;
  const urt_topicid_t topicid = 0;

  // print intro
  chprintf(stream, "publisher -+\n");
  chprintf(stream, "   ...     +-> topic -> %cRT subscriber\n", _urtbenchmark_rtClass2Char(rt));
  chprintf(stream, "publisher -+\n");
  chprintf(stream, "CPU stress: %s\n", stress ? "enabled" : "disabled");

  // print output table header
  chprintf(stream, "PUBLISHERS\tPUBLISHER\tITERATION\tLATENCY\n");

  // iteratively increase number of publishers
  for (size_t publishers = 1; publishers <= URTBENCHMARK_CFG_NUMPUBLISHERS; ++publishers) {
    // prepare topic and add messages
    urtTopicInit(&bmd->topic, topicid, NULL);
    for (size_t m = 0; m < publishers-1; ++m) {
      urtMessageInit(&bmd->messages[m], NULL);
      urtTopicAddMessages(&bmd->topic, &bmd->messages[m]);
    }

    // prepare publisher node
    for (size_t p = 0; p < publishers; ++p) {
      _urtbenchmark_prepare_publisher(&bmd->publishers[p], &_urtbenchmark_timer.time, URT_PUBLISHER_PUBLISH_ENFORCING, NULL, 0, topicid);
    }
    ubnConfigInit(&bmd->configs[0]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[0].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "publisher");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[0].node.trigger = &_urtbenchmark_timer.events.trigger;
    bmd->configs[0].publishers.buffer = bmd->publishers;
    bmd->configs[0].publishers.size = publishers;
    bmd->configs[0].node.type = URTBENCHMARK_PUBSUB_SCALING;
    ubnInit(&bmd->nodes[0], URT_THREAD_PRIO_HIGH_MAX, &bmd->configs[0]);

    // prepare subscriber node
    _urtbenchmark_prepare_subscriber(&bmd->subscriber, bmd->latencybuffer, NULL, 0, topicid, rt);
    memset(bmd->latencybuffer, 0, sizeof(bmd->latencybuffer));
    ubnConfigInit(&bmd->configs[1]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[1].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "subscriber");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[1].subscribers.buffer = &bmd->subscriber;
    bmd->configs[1].subscribers.size = 1;
    bmd->configs[1].node.type = URTBENCHMARK_PUBSUB_SCALING;
    ubnInit(&bmd->nodes[1], URT_THREAD_PRIO_HIGH_MAX-1, &bmd->configs[1]);

    // start all nodes
    urtCoreStartNodes();
    urtEventWait(coreevent, URT_EVENT_WAIT_ANY, URT_DELAY_INFINITE);

    // arm trigger timer
    _urtbenchmark_timer.counter = 0;
    _urtbenchmark_timer.iterations = URTBENCHMARK_CFG_REPETITIONS;
    aosTimerPeriodicInterval(&_urtbenchmark_timer.timer, MICROSECONDS_PER_SECOND / URTBENCHMARK_CFG_FREQUENCY, _urtbenchmark_triggercb, NULL);

    // wait for benchmark termination
    _urtbenchmark_waitForTermination(terminationevent, stress);

    // stop all nodes and reset
    urtCoreStopNodes(URT_STATUS_OK);
    urtThreadJoin(bmd->nodes[0].node.thread);
    urtThreadJoin(bmd->nodes[1].node.thread);
    _urtbenchmark_reset();

    // print all latencies
    for (size_t p = 0; p < publishers; ++p) {
      for (size_t i = 0; i < URTBENCHMARK_CFG_REPETITIONS; ++i) {
        chprintf(stream, "%10u\t%9u\t%9u\t%7u\n", publishers, p+1, i+1, bmd->latencybuffer[i * publishers + p]);
      }
    }
  }

  return AOS_OK;
}

int _urtbenchmark_pubsub_subscriber(BaseSequentialStream* const stream,
                                    const urt_osEventMask_t coreevent,
                                    const urt_osEventMask_t terminationevent,
                                    const urt_rtclass_t rt,
                                    const bool stress)
{
  // local constants
  _urtbenchmark_data_pubsub_subscriber_t* const bmd = &_urtbenchmark_data.pubsub.subscriber;
  const urt_topicid_t topicid = 0;

  // print intro
  chprintf(stream, "                    +-> %cRT subscriber\n", _urtbenchmark_rtClass2Char(rt));
  chprintf(stream, "publisher -> topic -+          ...\n");
  chprintf(stream, "                    +-> %cRT subscriber\n", _urtbenchmark_rtClass2Char(rt));
  chprintf(stream, "CPU stress: %s\n", stress ? "enabled" : "disabled");

  // print output table header
  chprintf(stream, "SUBSCRIBERS\tSUBSCRIBER\tITERATION\tLATENCY\n");

  // iteratively increase number of subscribers
  for (size_t subscribers = 1; subscribers <= URTBENCHMARK_CFG_NUMSUBSCRIBERS; ++subscribers) {
    // prepare topic
    urtTopicInit(&bmd->topic, topicid, NULL);

    // prepare publisher node
    _urtbenchmark_prepare_publisher(&bmd->publisher, NULL, URT_PUBLISHER_PUBLISH_ENFORCING, NULL, 0, topicid);
    ubnConfigInit(&bmd->configs[0]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[0].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "publisher");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[0].node.trigger = &_urtbenchmark_timer.events.trigger;
    bmd->configs[0].publishers.buffer = &bmd->publisher;
    bmd->configs[0].publishers.size = 1;
    bmd->configs[0].node.type = URTBENCHMARK_PUBSUB_SCALING;
    ubnInit(&bmd->nodes[0], URT_THREAD_PRIO_HIGH_MAX, &bmd->configs[0]);

    // prepare subscriber node
    for (size_t s = 0; s < subscribers; ++s) {
      _urtbenchmark_prepare_subscriber(&bmd->subscribers[s], bmd->latencybuffer[s], NULL, 0, topicid, rt);
      memset(bmd->latencybuffer[s], 0, sizeof(bmd->latencybuffer[s]));
    }
    ubnConfigInit(&bmd->configs[1]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[1].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "subscriber");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[1].subscribers.buffer = bmd->subscribers;
    bmd->configs[1].subscribers.size = subscribers;
    bmd->configs[1].node.type = URTBENCHMARK_PUBSUB_SCALING;
    ubnInit(&bmd->nodes[1], URT_THREAD_PRIO_HIGH_MAX-1, &bmd->configs[1]);

    // start all nodes
    urtCoreStartNodes();
    urtEventWait(coreevent, URT_EVENT_WAIT_ANY, URT_DELAY_INFINITE);

    // arm trigger timer
    _urtbenchmark_timer.counter = 0;
    _urtbenchmark_timer.iterations = URTBENCHMARK_CFG_REPETITIONS;
    aosTimerPeriodicInterval(&_urtbenchmark_timer.timer, MICROSECONDS_PER_SECOND / URTBENCHMARK_CFG_FREQUENCY, _urtbenchmark_triggercb, NULL);

    // wait for benchmark termination
    _urtbenchmark_waitForTermination(terminationevent, stress);

    // stop all nodes and reset
    urtCoreStopNodes(URT_STATUS_OK);
    urtThreadJoin(bmd->nodes[0].node.thread);
    urtThreadJoin(bmd->nodes[1].node.thread);
    _urtbenchmark_reset();

    // print all latencies
    for (size_t s = 0; s < subscribers; ++s) {
      for (size_t i = 0; i < URTBENCHMARK_CFG_REPETITIONS; ++i) {
        chprintf(stream, "%11u\t%10u\t%9u\t%7u\n", subscribers, s+1, i+1, bmd->latencybuffer[s][i]);
      }
    }
  }

  return AOS_OK;
}

int _urtbenchmark_pubsub_topic(BaseSequentialStream* const stream,
                               const urt_osEventMask_t coreevent,
                               const urt_osEventMask_t terminationevent,
                               const urt_rtclass_t rt,
                               const bool stress)
{
  // local constants
  _urtbenchmark_data_pubsub_topic_t* const bmd = &_urtbenchmark_data.pubsub.topic;

  // print intro
  chprintf(stream, "publisher -> topic -> %cRT subscriber\n", _urtbenchmark_rtClass2Char(rt));
  chprintf(stream, "   ...        ...           ...\n");
  chprintf(stream, "publisher -> topic -> %cRT subscriber\n", _urtbenchmark_rtClass2Char(rt));
  chprintf(stream, "CPU stress: %s\n", stress ? "enabled" : "disabled");

  // print output table header
  chprintf(stream, "TOPICS\tTOPIC\tITERATION\tLATENCY\n");

  // iteratively increase number of topic
  for (size_t topics = 1; topics <= URTBENCHMARK_CFG_NUMTOPICS; ++topics) {
    // prepate topics
    for (size_t t = 0; t < topics; ++t) {
      urtTopicInit(&bmd->topics[t], t, NULL);
    }

    // prepare publisher node
    for (size_t t = 0; t < topics; ++t) {
      _urtbenchmark_prepare_publisher(&bmd->publishers[t], &_urtbenchmark_timer.time, URT_PUBLISHER_PUBLISH_ENFORCING, NULL, 0, t);
    }
    ubnConfigInit(&bmd->configs[0]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[0].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "publisher");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[0].node.trigger = &_urtbenchmark_timer.events.trigger;
    bmd->configs[0].publishers.buffer = bmd->publishers;
    bmd->configs[0].publishers.size = topics;
    bmd->configs[0].node.type = URTBENCHMARK_PUBSUB_SCALING;
    ubnInit(&bmd->nodes[0], URT_THREAD_PRIO_HIGH_MAX, &bmd->configs[0]);

    // prepare subscriber node
    for (size_t t = 0; t < topics; ++t) {
      _urtbenchmark_prepare_subscriber(&bmd->subscribers[t], bmd->latencybuffer[t], NULL, 0, t, rt);
      memset(bmd->latencybuffer[t], 0, sizeof(bmd->latencybuffer[t]));
    }
    ubnConfigInit(&bmd->configs[1]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[1].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "subscriber");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[1].subscribers.buffer = bmd->subscribers;
    bmd->configs[1].subscribers.size = topics;
    bmd->configs[1].node.type = URTBENCHMARK_PUBSUB_SCALING;
    ubnInit(&bmd->nodes[1], URT_THREAD_PRIO_HIGH_MAX-1, &bmd->configs[1]);

    // start all nodes
    urtCoreStartNodes();
    urtEventWait(coreevent, URT_EVENT_WAIT_ANY, URT_DELAY_INFINITE);

    // arm trigger timer
    _urtbenchmark_timer.counter = 0;
    _urtbenchmark_timer.iterations = URTBENCHMARK_CFG_REPETITIONS;
    aosTimerPeriodicInterval(&_urtbenchmark_timer.timer, MICROSECONDS_PER_SECOND / URTBENCHMARK_CFG_FREQUENCY, _urtbenchmark_triggercb, NULL);

    // wait for benchmark termination
    _urtbenchmark_waitForTermination(terminationevent, stress);

    // stop all nodes and reset
    urtCoreStopNodes(URT_STATUS_OK);
    urtThreadJoin(bmd->nodes[0].node.thread);
    urtThreadJoin(bmd->nodes[1].node.thread);
    _urtbenchmark_reset();

    // print all latencies
    for (size_t t = 0; t < topics; ++t) {
      for (size_t i = 0; i < URTBENCHMARK_CFG_REPETITIONS; ++i) {
        chprintf(stream, "%6u\t%5u\t%9u\t%7u\n", topics, t+1, i+1, bmd->latencybuffer[t][i]);
      }
    }
  }

  return AOS_OK;
}

#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) || defined(__DOXYGEN__)
int _urtbenchmark_pubsub_qos_deadline(BaseSequentialStream* const stream,
                                      const urt_osEventMask_t coreevent,
                                      const urt_osEventMask_t terminationevent,
                                      const bool stress)
{
  // local constants
  _urtbenchmark_data_pubsub_deadline_t* const bmd = &_urtbenchmark_data.pubsub.deadline;
  const urt_topicid_t topicid = 0;
  // local variables
  size_t violations = 0;

  // print intro
  chprintf(stream, "publisher -> topic -> HRT subscriber\n");
  chprintf(stream, "CPU stress: %s\n", stress ? "enabled" : "disabled");
  chprintf(stream, "critical latency: %u us\n", URTBENCHMARK_CFG_QOSTIME);

  // repeat the benchmark
  for (size_t repetition = 0; repetition < URTBENCHMARK_CFG_REPETITIONS; ++repetition) {
    // prepate topic
    urtTopicInit(&bmd->topic, topicid, NULL);

    // prepare publisher node
    _urtbenchmark_prepare_publisher(&bmd->publisher, &bmd->origintime, URT_PUBLISHER_PUBLISH_ENFORCING, NULL, 0, topicid);
    ubnConfigInit(&bmd->configs[0]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[0].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "publisher");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[0].node.trigger = &_urtbenchmark_timer.events.trigger;
    bmd->configs[0].publishers.buffer = &bmd->publisher;
    bmd->configs[0].publishers.size = 1;
    bmd->configs[0].node.type = URTBENCHMARK_PUBSUB_QOS;
    ubnInit(&bmd->nodes[0], URT_THREAD_PRIO_HIGH_MAX, &bmd->configs[0]);

    // prepare subscriber node
    _urtbenchmark_prepare_subscriber(&bmd->subscriber, bmd->latencybuffer[repetition], NULL, 0, topicid, URT_HRT);
    bmd->subscriber.hrtsubscriber.deadline = URTBENCHMARK_CFG_QOSTIME;
    bmd->subscriber.hrtsubscriber.jitter = 0;
    bmd->subscriber.hrtsubscriber.rate = 0;
    memset(bmd->latencybuffer[repetition], 0, sizeof(bmd->latencybuffer[repetition]));
    ubnConfigInit(&bmd->configs[1]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[1].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "subscriber");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[1].subscribers.buffer = &bmd->subscriber;
    bmd->configs[1].subscribers.size = 1;
    bmd->configs[1].node.type = URTBENCHMARK_PUBSUB_QOS;
    ubnInit(&bmd->nodes[1], URT_THREAD_PRIO_HIGH_MAX-1, &bmd->configs[1]);

    // start all nodes
    urtCoreStartNodes();
    urtEventWait(coreevent, URT_EVENT_WAIT_ANY, URT_DELAY_INFINITE);

    // arm trigger timer
    _urtbenchmark_timer.counter = 0;
    _urtbenchmark_timer.iterations = URTBENCHMARK_CFG_QOSSTEPS;
    aosSysGetUptime(&_urtbenchmark_timer.time);
    urt_osTime_t time = _urtbenchmark_timer.time;
    urtTimeAdd(&time, (URTBENCHMARK_CFG_QOSTIME + URTBENCHMARK_CFG_QOSSTEPS-1) / URTBENCHMARK_CFG_QOSSTEPS);
    aosTimerSetAbsolute(&_urtbenchmark_timer.timer, time, _urtbenchmark_triggercb_pubsubqos, &bmd->origintime);

    // wait for benchmark termination
    _urtbenchmark_waitForTermination(terminationevent, stress);
    if (urtCoreGetStatus() == URT_STATUS_DEADLINEVIOLATION) {
      ++violations;
    }

    // stop all nodes and reset
    urtCoreStopNodes(URT_STATUS_OK);
    urtThreadJoin(bmd->nodes[0].node.thread);
    urtThreadJoin(bmd->nodes[1].node.thread);
    _urtbenchmark_reset();
  }

  // print all latencies
  chprintf(stream, "STEP\tITERATION\tLATENCY\n");
  for (size_t s = 0; s < URTBENCHMARK_CFG_QOSSTEPS; ++s) {
    for (size_t i = 0; i < URTBENCHMARK_CFG_REPETITIONS; ++i) {
      chprintf(stream, "%4u\t%9u\t%7u\n", s+1, i+1, bmd->latencybuffer[i][s]);
    }
  }
  chprintf(stream, "violations: %u/%u\n", violations, URTBENCHMARK_CFG_REPETITIONS);

  return AOS_OK;
}
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) */

#if (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) || defined(__DOXYGEN__)
int _urtbenchmark_pubsub_qos_jitter(BaseSequentialStream* const stream,
                                    const urt_osEventMask_t coreevent,
                                    const urt_osEventMask_t terminationevent,
                                    const bool stress)
{
  // local constants
  _urtbenchmark_data_pubsub_jitter_t* const bmd = &_urtbenchmark_data.pubsub.jitter;
  const urt_topicid_t topicid = 0;
  // local variables
  size_t violations = 0;

  // print intro
  chprintf(stream, "publisher -> topic -> HRT subscriber\n");
  chprintf(stream, "CPU stress: %s\n", stress ? "enabled" : "disabled");
  chprintf(stream, "critical jitter: %u us\n", URTBENCHMARK_CFG_QOSTIME);

  // repeat the benchmark
  for (size_t repetition = 0; repetition < URTBENCHMARK_CFG_REPETITIONS; ++repetition) {
    // prepate topic
    urtTopicInit(&bmd->topic, topicid, NULL);

    // prepare publisher node
    _urtbenchmark_prepare_publisher(&bmd->publisher, &bmd->origintime, URT_PUBLISHER_PUBLISH_ENFORCING, NULL, 0, topicid);
    ubnConfigInit(&bmd->configs[0]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[0].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "publisher");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[0].node.trigger = &_urtbenchmark_timer.events.trigger;
    bmd->configs[0].publishers.buffer = &bmd->publisher;
    bmd->configs[0].publishers.size = 1;
    bmd->configs[0].node.type = URTBENCHMARK_PUBSUB_QOS;
    ubnInit(&bmd->nodes[0], URT_THREAD_PRIO_HIGH_MAX, &bmd->configs[0]);

    // prepare subscriber node
    _urtbenchmark_prepare_subscriber(&bmd->subscriber, bmd->latencybuffer[repetition], NULL, 0, topicid, URT_HRT);
    bmd->subscriber.hrtsubscriber.deadline = 0;
    bmd->subscriber.hrtsubscriber.jitter = URTBENCHMARK_CFG_QOSTIME;
    bmd->subscriber.hrtsubscriber.rate = 0;
    memset(bmd->latencybuffer[repetition], 0, sizeof(bmd->latencybuffer[repetition]));
    ubnConfigInit(&bmd->configs[1]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[1].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "subscriber");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[1].subscribers.buffer = &bmd->subscriber;
    bmd->configs[1].subscribers.size = 1;
    bmd->configs[1].node.type = URTBENCHMARK_PUBSUB_QOS;
    ubnInit(&bmd->nodes[1], URT_THREAD_PRIO_HIGH_MAX-1, &bmd->configs[1]);

    // start all nodes
    urtCoreStartNodes();
    urtEventWait(coreevent, URT_EVENT_WAIT_ANY, URT_DELAY_INFINITE);

    // arm trigger timer
    _urtbenchmark_timer.counter = 0;
    _urtbenchmark_timer.iterations = URTBENCHMARK_CFG_QOSSTEPS + 1;
    aosSysGetUptime(&_urtbenchmark_timer.time);
    urt_osTime_t time = _urtbenchmark_timer.time;
    urtTimeAdd(&time, (URTBENCHMARK_CFG_QOSTIME + URTBENCHMARK_CFG_QOSSTEPS-1) / URTBENCHMARK_CFG_QOSSTEPS);
    aosTimerSetAbsolute(&_urtbenchmark_timer.timer, time, _urtbenchmark_triggercb_pubsubqos, &bmd->origintime);

    // wait for benchmark termination
    _urtbenchmark_waitForTermination(terminationevent, stress);
    if (urtCoreGetStatus() == URT_STATUS_JITTERVIOLATION) {
      ++violations;
    }

    // stop all nodes and reset
    urtCoreStopNodes(URT_STATUS_OK);
    urtThreadJoin(bmd->nodes[0].node.thread);
    urtThreadJoin(bmd->nodes[1].node.thread);
    _urtbenchmark_reset();
  }

  // print all latencies
  chprintf(stream, "STEP\tITERATION\tLATENCY\tJITTER\n");
  for (size_t s = 0; s < URTBENCHMARK_CFG_QOSSTEPS + 1; ++s) {
    for (size_t i = 0; i < URTBENCHMARK_CFG_REPETITIONS; ++i) {
      chprintf(stream, "%4u\t%9u\t%7u\t%6u\n", s+1, i+1, bmd->latencybuffer[i][s], (bmd->latencybuffer[i][s] != 0) ? bmd->latencybuffer[i][s] - bmd->latencybuffer[i][0] : 0);
    }
  }
  chprintf(stream, "violations: %u/%u\n", violations, URTBENCHMARK_CFG_REPETITIONS);

  return AOS_OK;
}
#endif /* (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

#if (URT_CFG_PUBSUB_QOS_RATECHECKS == true) || defined(__DOXYGEN__)
int _urtbenchmark_pubsub_qos_rate(BaseSequentialStream* const stream,
                                  const urt_osEventMask_t coreevent,
                                  const urt_osEventMask_t terminationevent,
                                  const bool stress)
{
  // local constants
  _urtbenchmark_data_pubsub_rate_t* const bmd = &_urtbenchmark_data.pubsub.rate;
  const urt_topicid_t topicid = 0;
  // local variables
  size_t violations = 0;

  // print intro
  chprintf(stream, "publisher -> topic -> HRT subscriber\n");
  chprintf(stream, "CPU stress: %s\n", stress ? "enabled" : "disabled");
  chprintf(stream, "critical rate: %u us\n", URTBENCHMARK_CFG_QOSTIME);

  // repeat the benchmark
  for (size_t repetition = 0; repetition < URTBENCHMARK_CFG_REPETITIONS; ++repetition) {
    // prepate topic
    urtTopicInit(&bmd->topic, topicid, NULL);

    // prepare publisher node
    _urtbenchmark_prepare_publisher(&bmd->publisher, NULL, URT_PUBLISHER_PUBLISH_ENFORCING, NULL, 0, topicid);
    ubnConfigInit(&bmd->configs[0]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[0].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "publisher");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[0].node.trigger = &_urtbenchmark_timer.events.trigger;
    bmd->configs[0].publishers.buffer = &bmd->publisher;
    bmd->configs[0].publishers.size = 1;
    bmd->configs[0].node.type = URTBENCHMARK_PUBSUB_QOS;
    ubnInit(&bmd->nodes[0], URT_THREAD_PRIO_HIGH_MAX, &bmd->configs[0]);

    // prepare subscriber node
    _urtbenchmark_prepare_subscriber(&bmd->subscriber, bmd->latencybuffer[repetition], NULL, 0, topicid, URT_HRT);
    bmd->subscriber.hrtsubscriber.deadline = 0;
    bmd->subscriber.hrtsubscriber.jitter = 0;
    bmd->subscriber.hrtsubscriber.rate = URTBENCHMARK_CFG_QOSTIME;
    memset(bmd->latencybuffer[repetition], 0, sizeof(bmd->latencybuffer[repetition]));
    ubnConfigInit(&bmd->configs[1]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[1].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "subscriber");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[1].subscribers.buffer = &bmd->subscriber;
    bmd->configs[1].subscribers.size = 1;
    bmd->configs[1].node.type = URTBENCHMARK_PUBSUB_QOS;
    ubnInit(&bmd->nodes[1], URT_THREAD_PRIO_HIGH_MAX-1, &bmd->configs[1]);

    // start all nodes
    urtCoreStartNodes();
    urtEventWait(coreevent, URT_EVENT_WAIT_ANY, URT_DELAY_INFINITE);

    // arm trigger timer
    _urtbenchmark_timer.counter = 0;
    _urtbenchmark_timer.iterations = URTBENCHMARK_CFG_QOSSTEPS;
    aosSysGetUptime(&_urtbenchmark_timer.time);
    urt_osTime_t time = _urtbenchmark_timer.time;
    urtTimeAdd(&time, (URTBENCHMARK_CFG_QOSTIME + URTBENCHMARK_CFG_QOSSTEPS-1) / URTBENCHMARK_CFG_QOSSTEPS);
    aosTimerSetAbsolute(&_urtbenchmark_timer.timer, time, _urtbenchmark_triggercb_pubsubqos, &bmd->origintime);

    // wait for benchmark termination
    _urtbenchmark_waitForTermination(terminationevent, stress);
    if (urtCoreGetStatus() == URT_STATUS_RATEVIOLATION) {
      ++violations;
    }

    // stop all nodes and reset
    urtCoreStopNodes(URT_STATUS_OK);
    urtThreadJoin(bmd->nodes[0].node.thread);
    urtThreadJoin(bmd->nodes[1].node.thread);
    _urtbenchmark_reset();
  }

  // print all latencies
  chprintf(stream, "STEP\tITERATION\tLATENCY\n");
  for (size_t s = 0; s < URTBENCHMARK_CFG_QOSSTEPS; ++s) {
    for (size_t i = 0; i < URTBENCHMARK_CFG_REPETITIONS; ++i) {
      chprintf(stream, "%4u\t%9u\t%7u\n", s+1, i+1, bmd->latencybuffer[i][s]);
    }
  }
  chprintf(stream, "violations: %u/%u\n", violations, URTBENCHMARK_CFG_REPETITIONS);

  return AOS_OK;
}
#endif /* (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */

static int _urtbenchmark_shellcb_pubsub(BaseSequentialStream* stream, int argc, const char* argv[])
{
  URTBENCHMARK_INTRO();

  // benchmark number of hops
  if ((argc == 3 || argc == 4) &&
      (strcmp(argv[1], "--hops") == 0) &&
      _urtbenchmark_shellcb_checkXRT(argv[2])) {
    status = _urtbenchmark_pubsub_hops(stream, coreevent, terminationevent,
                                       _urtbenchmark_shellcb_extractRtClass(argv[2]),
                                       _urtbenchmark_shellcb_checkStress(argv[3]));
  }

  // benchmark payload scaling
  else if ((argc == 3 || argc == 4) &&
           (strcmp(argv[1], "--payload") == 0) &&
           _urtbenchmark_shellcb_checkXRT(argv[2])) {
    status = _urtbenchmark_pubsub_payload(stream, coreevent, terminationevent,
                                          _urtbenchmark_shellcb_extractRtClass(argv[2]),
                                          _urtbenchmark_shellcb_checkStress(argv[3]));
  }

  // benchmark message scaling
  else if ((argc == 3 || argc == 4) &&
           (strcmp(argv[1], "--message") == 0) &&
           _urtbenchmark_shellcb_checkXRT(argv[2])) {
    status = _urtbenchmark_pubsub_message(stream, coreevent, terminationevent,
                                          _urtbenchmark_shellcb_extractRtClass(argv[2]),
                                          _urtbenchmark_shellcb_checkStress(argv[3]));
  }

  // benchmark publisher scaling
  else if ((argc == 3 || argc == 4) &&
           (strcmp(argv[1], "--publisher") == 0) &&
           _urtbenchmark_shellcb_checkXRT(argv[2])) {
    status = _urtbenchmark_pubsub_publisher(stream, coreevent, terminationevent,
                                            _urtbenchmark_shellcb_extractRtClass(argv[2]),
                                            _urtbenchmark_shellcb_checkStress(argv[3]));
  }

  // benchmark subscriber scaling
  else if ((argc == 3 || argc == 4) &&
           (strcmp(argv[1], "--subscriber") == 0) &&
           _urtbenchmark_shellcb_checkXRT(argv[2])) {
    status = _urtbenchmark_pubsub_subscriber(stream, coreevent, terminationevent,
                                             _urtbenchmark_shellcb_extractRtClass(argv[2]),
                                             _urtbenchmark_shellcb_checkStress(argv[3]));
  }

  // benchmark topic scaling
  else if ((argc == 3 || argc == 4) &&
           (strcmp(argv[1], "--topic") == 0) &&
           _urtbenchmark_shellcb_checkXRT(argv[2])) {
    status = _urtbenchmark_pubsub_topic(stream, coreevent, terminationevent,
                                        _urtbenchmark_shellcb_extractRtClass(argv[2]),
                                        _urtbenchmark_shellcb_checkStress(argv[3]));
  }

#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true)
  // verify deadline checks
  else if ((argc == 3 || argc == 4) &&
           ((strcmp(argv[1], "--QoS") == 0 || strcmp(argv[1], "--qos") == 0)) &&
           (strcmp(argv[2], "deadline") == 0)) {
    status = _urtbenchmark_pubsub_qos_deadline(stream, coreevent, terminationevent,
                                               _urtbenchmark_shellcb_checkStress(argv[3]));
  }
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) */

#if (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
  // verify jitter checks
  else if ((argc == 3 || argc == 4) &&
           ((strcmp(argv[1], "--QoS") == 0 || strcmp(argv[1], "--qos") == 0)) &&
           (strcmp(argv[2], "jitter") == 0)) {
    status = _urtbenchmark_pubsub_qos_jitter(stream, coreevent, terminationevent,
                                             _urtbenchmark_shellcb_checkStress(argv[3]));
  }
#endif /* (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

#if (URT_CFG_PUBSUB_QOS_RATECHECKS == true)
  // verify rate checks
  else if ((argc == 3 || argc == 4) &&
           ((strcmp(argv[1], "--QoS") == 0 || strcmp(argv[1], "--qos") == 0)) &&
           (strcmp(argv[2], "rate") == 0)) {
    status = _urtbenchmark_pubsub_qos_rate(stream, coreevent, terminationevent,
                                           _urtbenchmark_shellcb_checkStress(argv[3]));
  }
#endif /* (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */

  // benchmark all
  else if ((argc == 2 || argc == 3) &&
           (strcmp(argv[1], "--all") == 0)) {
    const char d = '-';
    const bool stresscpu = _urtbenchmark_shellcb_checkStress(argv[2]);

    // hops benchmarks
    _urtbenchmark_printHeading(stream, "HOPS (NRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_hops(stream, coreevent, terminationevent, URT_NRT, stresscpu);
    _urtbenchmark_printHeading(stream, "HOPS (SRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_hops(stream, coreevent, terminationevent, URT_SRT, stresscpu);
    _urtbenchmark_printHeading(stream, "HOPS (FRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_hops(stream, coreevent, terminationevent, URT_FRT, stresscpu);
    _urtbenchmark_printHeading(stream, "HOPS (HRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_hops(stream, coreevent, terminationevent, URT_HRT, stresscpu);

    // payload benchmarks
    _urtbenchmark_printHeading(stream, "PAYLOAD (NRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_payload(stream, coreevent, terminationevent, URT_NRT, stresscpu);
    _urtbenchmark_printHeading(stream, "PAYLOAD (SRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_payload(stream, coreevent, terminationevent, URT_SRT, stresscpu);
    _urtbenchmark_printHeading(stream, "PAYLOAD (FRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_payload(stream, coreevent, terminationevent, URT_FRT, stresscpu);
    _urtbenchmark_printHeading(stream, "PAYLOAD (HRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_payload(stream, coreevent, terminationevent, URT_HRT, stresscpu);

    // message buffer benchmarks
    _urtbenchmark_printHeading(stream, "MESSAGE (NRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_message(stream, coreevent, terminationevent, URT_NRT, stresscpu);
    _urtbenchmark_printHeading(stream, "MESSAGE (SRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_message(stream, coreevent, terminationevent, URT_SRT, stresscpu);
    _urtbenchmark_printHeading(stream, "MESSAGE (FRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_message(stream, coreevent, terminationevent, URT_FRT, stresscpu);
    _urtbenchmark_printHeading(stream, "MESSAGE (HRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_message(stream, coreevent, terminationevent, URT_HRT, stresscpu);

    // publisher benchmark
    _urtbenchmark_printHeading(stream, "PUBLISHER (NRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_publisher(stream, coreevent, terminationevent, URT_NRT, stresscpu);
    _urtbenchmark_printHeading(stream, "PUBLISHER (SRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_publisher(stream, coreevent, terminationevent, URT_SRT, stresscpu);
    _urtbenchmark_printHeading(stream, "PUBLISHER (FRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_publisher(stream, coreevent, terminationevent, URT_FRT, stresscpu);
    _urtbenchmark_printHeading(stream, "PUBLISHER (HRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_publisher(stream, coreevent, terminationevent, URT_HRT, stresscpu);

    // subscriber benchmarks
    _urtbenchmark_printHeading(stream, "SUBSCRIBER (NRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_subscriber(stream, coreevent, terminationevent, URT_NRT, stresscpu);
    _urtbenchmark_printHeading(stream, "SUBSCRIBER (SRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_subscriber(stream, coreevent, terminationevent, URT_SRT, stresscpu);
    _urtbenchmark_printHeading(stream, "SUBSCRIBER (FRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_subscriber(stream, coreevent, terminationevent, URT_FRT, stresscpu);
    _urtbenchmark_printHeading(stream, "SUBSCRIBER (HRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_subscriber(stream, coreevent, terminationevent, URT_HRT, stresscpu);

    // topic benchmark
    _urtbenchmark_printHeading(stream, "TOPIC (NRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_topic(stream, coreevent, terminationevent, URT_NRT, stresscpu);
    _urtbenchmark_printHeading(stream, "TOPIC (SRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_topic(stream, coreevent, terminationevent, URT_SRT, stresscpu);
    _urtbenchmark_printHeading(stream, "TOPIC (FRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_topic(stream, coreevent, terminationevent, URT_FRT, stresscpu);
    _urtbenchmark_printHeading(stream, "TOPIC (HRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_topic(stream, coreevent, terminationevent, URT_HRT, stresscpu);

#if (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true)
    // deadline QoS benchmark
    _urtbenchmark_printHeading(stream, "DEADLINE", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_qos_deadline(stream, coreevent, terminationevent, stresscpu);
#endif /* (URT_CFG_PUBSUB_QOS_DEADLINECHECKS == true) */

#if (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true)
    // jitter QoS benchmark
    _urtbenchmark_printHeading(stream, "JITTER", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_qos_jitter(stream, coreevent, terminationevent, stresscpu);
#endif /* (URT_CFG_PUBSUB_QOS_JITTERCHECKS == true) */

#if (URT_CFG_PUBSUB_QOS_RATECHECKS == true)
    // rate QoS benchmark
    _urtbenchmark_printHeading(stream, "RATE", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_pubsub_qos_rate(stream, coreevent, terminationevent, stresscpu);
#endif /* (URT_CFG_PUBSUB_QOS_RATECHECKS == true) */

    status = AOS_OK;
  }

  // print help
  const bool printhelp = (aosShellcmdCheckHelp(argc, argv, true) != AOS_SHELL_HELPREQUEST_NONE);
  if (printhelp) {
    status = AOS_OK;
  }
  if (printhelp || status == AOS_INVALIDARGUMENTS) {
    chprintf(stream, "usage: %s BENCHMARK [OPTION] [--stress|-s]\n", argv[0]);
    chprintf(stream, "BENCHMARK:\n");
    chprintf(stream, "  --hops\n");
    chprintf(stream, "    Benchmark message propagation latency over multiple hops.\n");
    chprintf(stream, "    OPTION (required):\n");
    chprintf(stream, "      NRT, nrt - Use NRT subscribers for benchmarking.\n");
    chprintf(stream, "      SRT, srt - Use SRT subscribers for benchmarking.\n");
    chprintf(stream, "      FRT, frt - Use FRT subscribers for benchmarking.\n");
    chprintf(stream, "      HRT, hrt - Use HRT subscribers for benchmarking.\n");
    chprintf(stream, "  --payload\n");
    chprintf(stream, "    Benchmark message payload scaling.\n");
    chprintf(stream, "    OPTION (required):\n");
    chprintf(stream, "      NRT, nrt - Use an NRT subscriber for benchmarking.\n");
    chprintf(stream, "      SRT, srt - Use an SRT subscriber for benchmarking.\n");
    chprintf(stream, "      FRT, frt - Use an FRT subscriber for benchmarking.\n");
    chprintf(stream, "      HRT, hrt - Use an HRT subscriber for benchmarking.\n");
    chprintf(stream, "  --message\n");
    chprintf(stream, "    Benchmark scaling of number of messages per topic.\n");
    chprintf(stream, "    OPTION (required):\n");
    chprintf(stream, "      NRT, nrt - Use an NRT subscriber for benchmarking.\n");
    chprintf(stream, "      SRT, srt - Use an SRT subscriber for benchmarking.\n");
    chprintf(stream, "      FRT, frt - Use an FRT subscriber for benchmarking.\n");
    chprintf(stream, "      HRT, hrt - Use an HRT subscriber for benchmarking.\n");
    chprintf(stream, "  --publisher\n");
    chprintf(stream, "    Benchmark scaling of number of publishers.\n");
    chprintf(stream, "    OPTION (required):\n");
    chprintf(stream, "      NRT, nrt - Use an NRT subscriber for benchmarking.\n");
    chprintf(stream, "      SRT, srt - Use an SRT subscriber for benchmarking.\n");
    chprintf(stream, "      FRT, frt - Use an FRT subscriber for benchmarking.\n");
    chprintf(stream, "      HRT, hrt - Use an HRT subscriber for benchmarking.\n");
    chprintf(stream, "  --subscriber\n");
    chprintf(stream, "    Benchmark scaling of number of subscribers.\n");
    chprintf(stream, "    OPTION (required):\n");
    chprintf(stream, "      NRT, nrt - Use NRT subscribers for benchmarking.\n");
    chprintf(stream, "      SRT, srt - Use SRT subscribers for benchmarking.\n");
    chprintf(stream, "      FRT, frt - Use FRT subscribers for benchmarking.\n");
    chprintf(stream, "      HRT, hrt - Use HRT subscribers for benchmarking.\n");
    chprintf(stream, "  --topic\n");
    chprintf(stream, "    Benchmark scaling of number of topics.\n");
    chprintf(stream, "    OPTION (required):\n");
    chprintf(stream, "      NRT, nrt - Use an NRT subscriber for benchmarking.\n");
    chprintf(stream, "      SRT, srt - Use an SRT subscriber for benchmarking.\n");
    chprintf(stream, "      FRT, frt - Use an FRT subscriber for benchmarking.\n");
    chprintf(stream, "      HRT, hrt - Use an HRT subscriber for benchmarking.\n");
    chprintf(stream, "  --QoS, --qos\n");
    chprintf(stream, "    Benchmark Quality of Service (QoS) performance.\n");
    chprintf(stream, "    OPTION (required):\n");
    chprintf(stream, "      deadline - Benchmark deadline checks.\n");
    chprintf(stream, "      jitter   - Benchmark jitter checks.\n");
    chprintf(stream, "      rate     - Benchmark rate checks.\n");
    chprintf(stream, "  --all\n");
    chprintf(stream, "    Run all benchmarks.\n");
    chprintf(stream, "  --help, -h\n");
    chprintf(stream, "    Print this help text.\n");
    chprintf(stream, "--stress, -s (optional):\n");
    chprintf(stream, "  Stress CPU during benchmarks.\n");
  }

  URTBENCHMARK_OUTRO();

  return status;
}

#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)

void _urtbenchmark_prepare_request(urtbenchmark_configdata_request_t* const request,
                                   urt_delay_t* latencybuffer,
                                   uint8_t* payload_buffer,
                                   size_t payload_size,
                                   urt_serviceid_t serviceid,
                                   urt_request_retrievepolicy_t policy,
                                   urt_rtclass_t rtclass)
{
  switch (rtclass) {
    case URT_NRT:
      break;
    case URT_SRT:
      request->srtrequest.usefulnesscb = _urtbenchmark_usefulnesscb;
      request->srtrequest.params = NULL;
      break;
    case URT_FRT:
      request->frtrequest.deadline = URTBENCHMARK_QOSPSEUDOTIME;
      request->frtrequest.jitter = URTBENCHMARK_QOSPSEUDOTIME;
      break;
    case URT_HRT:
      request->hrtrequest.deadline = URTBENCHMARK_QOSPSEUDOTIME;
      request->hrtrequest.jitter = URTBENCHMARK_QOSPSEUDOTIME;
      break;
  }
  request->service = NULL;
  request->latencybuffer = latencybuffer;
  request->payload.buffer = payload_buffer;
  request->payload.size = payload_size;
  request->serviceid = serviceid;
  request->policy = policy;
  request->rtclass = rtclass;

  return;
}

void _urtbenchmark_prepare_service(urtbenchmark_configdata_service_t* const service,
                                   uint8_t* payload_buffer,
                                   size_t payload_size,
                                   urt_delay_t* delaybuffer,
                                   urt_delay_t* latencybuffer,
                                   urt_serviceid_t serviceid)
{
  service->payload.buffer = payload_buffer;
  service->payload.size = payload_size;
  service->delay = delaybuffer;
  service->latencybuffer = latencybuffer;
  service->serviceid = serviceid;

  return;
}

int _urtbenchmark_rpc_hops(BaseSequentialStream* const stream,
                           const urt_osEventMask_t coreevent,
                           const urt_osEventMask_t terminationevent,
                           const urt_rtclass_t rt,
                           const bool stress)
{
  // local constants
  _urtbenchmark_data_rpc_hops_t* const bmd = &_urtbenchmark_data.rpc.hops;

  // print intro
  chprintf(stream, "%cRT request <-> service | %cRT request <-> ... <-> service\n", _urtbenchmark_rtClass2Char(rt), _urtbenchmark_rtClass2Char(rt));
  chprintf(stream, "CPU stress: %s\n", stress ? "enabled" : "disabled");

  // print output table header
  chprintf(stream, "HOPS\tITERATION\tRETRIEVE\n");

  // iteratively increase number of hops
  for (size_t hops = 1; hops <= URTBENCHMARK_CFG_NUMHOPS; ++hops) {
    // prepare requests
    for (size_t request = 0; request < hops; ++request) {
      _urtbenchmark_prepare_request(&bmd->requests[request],
                                    (request == 0) ? bmd->latencybuffer : NULL,
                                    NULL, 0, request, URT_REQUEST_RETRIEVE_DETERMINED, rt);
    }
    memset(bmd->latencybuffer, 0, sizeof(bmd->latencybuffer));

    // prepare services
    for (size_t service = 0; service < hops; ++service) {
      _urtbenchmark_prepare_service(&bmd->services[service],
                                    NULL, 0, NULL, NULL, service);
    }

    // prepare nodes
    for (size_t node = 0; node <= hops; ++node) {
      ubnConfigInit(&bmd->configs[node]);
#if (CH_CFG_USE_REGISTRY == TRUE)
      chsnprintf(bmd->configs[node].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "node%u", node+1);
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
      // set trigger for first node
      if (node == 0) {
        bmd->configs[node].node.trigger = &_urtbenchmark_timer.events.trigger;
      }
      bmd->configs[node].node.type =URTBENCHMARK_RPC_HOPS;
      // add request to all but the last node
      if (node < hops) {
        bmd->configs[node].requests.buffer = &bmd->requests[node];
        bmd->configs[node].requests.size = 1;
      }
      // add services to all but the first node
      if (node > 0) {
        bmd->configs[node].services.buffer = &bmd->services[node-1];
        bmd->configs[node].services.size = 1;
      }
      bmd->configs[node].node.type = URTBENCHMARK_RPC_HOPS;
      ubnInit(&bmd->nodes[node], URT_THREAD_PRIO_HIGH_MAX-1, &bmd->configs[node]);
    }

    // start all nodes
    urtCoreStartNodes();
    urtEventWait(coreevent, URT_EVENT_WAIT_ANY, URT_DELAY_INFINITE);

    // arm trigger timer
    _urtbenchmark_timer.counter = 0;
    _urtbenchmark_timer.iterations = URTBENCHMARK_CFG_REPETITIONS;
    aosTimerPeriodicInterval(&_urtbenchmark_timer.timer, MICROSECONDS_PER_SECOND / URTBENCHMARK_CFG_FREQUENCY, _urtbenchmark_triggercb, NULL);

    // wait for benchmark termination
    _urtbenchmark_waitForTermination(terminationevent, stress);

    // stop all nodes and reset
    urtCoreStopNodes(URT_STATUS_OK);
    for (size_t node = 0; node <= hops; ++node) {
      urtThreadJoin(bmd->nodes[node].node.thread);
    }
    _urtbenchmark_reset();

    // print all latencies
    for (size_t i = 0; i < URTBENCHMARK_CFG_REPETITIONS; ++i) {
      chprintf(stream, "%4u\t%9u\t%8u\n", hops, i+1, bmd->latencybuffer[i]);
    }
  }

  return AOS_OK;
}

int _urtbenchmark_rpc_payload(BaseSequentialStream* const stream,
                              const urt_osEventMask_t coreevent,
                              const urt_osEventMask_t terminationevent,
                              const urt_rtclass_t rt,
                              const bool stress)
{
  // local constants
  _urtbenchmark_data_rpc_payload_t* const bmd = &_urtbenchmark_data.rpc.payload;
  const urt_serviceid_t serviceid = 0;

  // print intro
  chprintf(stream, "%cRT request <-> service\n", _urtbenchmark_rtClass2Char(rt));
  chprintf(stream, "CPU stress: %s\n", stress ? "enabled" : "disabled");

  // print output table header
  chprintf(stream, "PAYLOAD\tITERATION\tDISPATCH\tRETRIEVE\n");

  // iteratively increase payload
  for (size_t payload = 0; payload <= URTBENCHMARK_CFG_MAXPAYLOAD; payload += sizeof(size_t)) {
    // prepare request node
    _urtbenchmark_prepare_request(&bmd->request, bmd->latencybuffers.retrieve, (payload == 0) ? NULL : bmd->payloads.request, payload, serviceid, URT_REQUEST_RETRIEVE_DETERMINED, rt);
    memset(bmd->latencybuffers.retrieve, 0, sizeof(bmd->latencybuffers.retrieve));
    memset(bmd->payloads.request, 0, URTBENCHMARK_CFG_MAXPAYLOAD);
    ubnConfigInit(&bmd->configs[0]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[0].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "request");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[0].node.trigger = &_urtbenchmark_timer.events.trigger;
    bmd->configs[0].requests.buffer = &bmd->request;
    bmd->configs[0].requests.size = 1;
    bmd->configs[0].node.type = URTBENCHMARK_RPC_SCALING;
    ubnInit(&bmd->nodes[0], URT_THREAD_PRIO_HIGH_MAX, &bmd->configs[0]);

    // prepare service node
    _urtbenchmark_prepare_service(&bmd->service, (payload == 0) ? NULL : bmd->payloads.service, payload, NULL, bmd->latencybuffers.dispatch, serviceid);
    memset(bmd->payloads.service, 0, URTBENCHMARK_CFG_MAXPAYLOAD);
    memset(bmd->latencybuffers.dispatch, 0, sizeof(bmd->latencybuffers.dispatch));
    ubnConfigInit(&bmd->configs[1]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[1].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "service");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[1].services.buffer = &bmd->service;
    bmd->configs[1].services.size = 1;
    bmd->configs[1].node.type = URTBENCHMARK_RPC_SCALING;
    ubnInit(&bmd->nodes[1], URT_THREAD_PRIO_HIGH_MAX, &bmd->configs[1]);

    // start all nodes
    urtCoreStartNodes();
    urtEventWait(coreevent, URT_EVENT_WAIT_ANY, URT_DELAY_INFINITE);

    // arm trigger timer
    _urtbenchmark_timer.counter = 0;
    _urtbenchmark_timer.iterations = URTBENCHMARK_CFG_REPETITIONS;
    aosTimerPeriodicInterval(&_urtbenchmark_timer.timer, MICROSECONDS_PER_SECOND / URTBENCHMARK_CFG_FREQUENCY, _urtbenchmark_triggercb, NULL);

    // wait for benchmark termination
    _urtbenchmark_waitForTermination(terminationevent, stress);

    // stop all nodes and reset
    urtCoreStopNodes(URT_STATUS_OK);
    urtThreadJoin(bmd->nodes[0].node.thread);
    urtThreadJoin(bmd->nodes[1].node.thread);
    _urtbenchmark_reset();

    // print all latencies
    for (size_t i = 0; i < URTBENCHMARK_CFG_REPETITIONS; ++i) {
      chprintf(stream, "%7u\t%9u\t%8u\t%8u\n", payload, i+1, bmd->latencybuffers.dispatch[i], bmd->latencybuffers.retrieve[i]);
    }
  }

  return AOS_OK;
}

int _urtbenchmark_rpc_request(BaseSequentialStream* const stream,
                              const urt_osEventMask_t coreevent,
                              const urt_osEventMask_t terminationevent,
                              const urt_rtclass_t rt,
                              const bool stress)
{
  // local constants
  _urtbenchmark_data_rpc_request_t* const bmd = &_urtbenchmark_data.rpc.request;
  const urt_serviceid_t serviceid = 0;

  // print intro
  chprintf(stream, "%cRT request <-+\n", _urtbenchmark_rtClass2Char(rt));
  chprintf(stream,  "    ...       +-> service\n");
  chprintf(stream, "%cRT request <-+\n", _urtbenchmark_rtClass2Char(rt));
  chprintf(stream, "CPU stress: %s\n", stress ? "enabled" : "disabled");

  // print output table header
  chprintf(stream, "REQUESTS\tREQUEST\tITERATION\tDISPATCH\tRETRIEVE\n");

  // iteratively increase requests
  for (size_t requests = 1; requests <= URTBENCHMARK_CFG_NUMREQUESTS; ++requests) {
    // prepare request node
    for (size_t r = 0; r < requests; ++r) {
      _urtbenchmark_prepare_request(&bmd->requests[r], bmd->latencybuffers.retrieve[r], NULL, 0, serviceid, URT_REQUEST_RETRIEVE_DETERMINED, rt);
      memset(bmd->latencybuffers.retrieve[r], 0, sizeof(bmd->latencybuffers.retrieve[r]));
    }
    ubnConfigInit(&bmd->configs[0]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[0].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "request");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[0].node.trigger = &_urtbenchmark_timer.events.trigger;
    bmd->configs[0].requests.buffer = bmd->requests;
    bmd->configs[0].requests.size = requests;
    bmd->configs[0].node.type = URTBENCHMARK_RPC_SCALING;
    ubnInit(&bmd->nodes[0], URT_THREAD_PRIO_HIGH_MAX, &bmd->configs[0]);

    // prepare service node
    _urtbenchmark_prepare_service(&bmd->service, NULL, 0, NULL, bmd->latencybuffers.dispatch, serviceid);
    memset(bmd->latencybuffers.dispatch, 0, sizeof(bmd->latencybuffers.dispatch));
    ubnConfigInit(&bmd->configs[1]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[1].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "service");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[1].services.buffer = &bmd->service;
    bmd->configs[1].services.size = 1;
    bmd->configs[1].node.type = URTBENCHMARK_RPC_SCALING;
    ubnInit(&bmd->nodes[1], URT_THREAD_PRIO_HIGH_MAX, &bmd->configs[1]);

    // start all nodes
    urtCoreStartNodes();
    urtEventWait(coreevent, URT_EVENT_WAIT_ANY, URT_DELAY_INFINITE);

    // arm trigger timer
    _urtbenchmark_timer.counter = 0;
    _urtbenchmark_timer.iterations = URTBENCHMARK_CFG_REPETITIONS;
    aosTimerPeriodicInterval(&_urtbenchmark_timer.timer, MICROSECONDS_PER_SECOND / URTBENCHMARK_CFG_FREQUENCY, _urtbenchmark_triggercb, NULL);

    // wait for benchmark termination
    _urtbenchmark_waitForTermination(terminationevent, stress);

    // stop all nodes and reset
    urtCoreStopNodes(URT_STATUS_OK);
    urtThreadJoin(bmd->nodes[0].node.thread);
    urtThreadJoin(bmd->nodes[1].node.thread);
    _urtbenchmark_reset();

    // print all latencies
    for (size_t r = 0; r < requests; ++r) {
      for (size_t i = 0; i < URTBENCHMARK_CFG_REPETITIONS; ++i) {
        chprintf(stream, "%8u\t%7u\t%9u\t%8u\t%8u\n", requests, r+1, i+1, bmd->latencybuffers.dispatch[i * requests + r], bmd->latencybuffers.retrieve[r][i]);
      }
    }
  }

  return AOS_OK;
}

int _urtbenchmark_rpc_service(BaseSequentialStream* const stream,
                              const urt_osEventMask_t coreevent,
                              const urt_osEventMask_t terminationevent,
                              const urt_rtclass_t rt,
                              const bool stress)
{
  // local constants
  _urtbenchmark_data_rpc_service_t* const bmd = &_urtbenchmark_data.rpc.service;

  // print intro
  chprintf(stream, "%cRT request <-> service\n", _urtbenchmark_rtClass2Char(rt));
  chprintf(stream, "    ...           ...\n");
  chprintf(stream, "%cRT request <-> service\n", _urtbenchmark_rtClass2Char(rt));
  chprintf(stream, "CPU stress: %s\n", stress ? "enabled" : "disabled");

  // print output table header
  chprintf(stream, "SERVICES\tSERVICE\tITERATION\tDISPATCH\tRETRIEVE\n");

  // iteratively increase requests
  for (size_t services = 1; services <= URTBENCHMARK_CFG_NUMSERVICES; ++services) {
    // prepare request node
    for (size_t s = 0; s < services; ++s) {
      _urtbenchmark_prepare_request(&bmd->requests[s], bmd->latencybuffers.retrieve[s], NULL, 0, s, URT_REQUEST_RETRIEVE_DETERMINED, rt);
      memset(bmd->latencybuffers.retrieve[s], 0, sizeof(bmd->latencybuffers.retrieve[s]));
    }
    ubnConfigInit(&bmd->configs[0]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[0].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "request");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[0].node.trigger = &_urtbenchmark_timer.events.trigger;
    bmd->configs[0].requests.buffer = bmd->requests;
    bmd->configs[0].requests.size = services;
    bmd->configs[0].node.type = URTBENCHMARK_RPC_SCALING;
    ubnInit(&bmd->nodes[0], URT_THREAD_PRIO_HIGH_MAX, &bmd->configs[0]);

    // prepare service node
    for (size_t s = 0; s < services; ++s) {
      _urtbenchmark_prepare_service(&bmd->services[s], NULL, 0, NULL, bmd->latencybuffers.dispatch[s], s);
    }
    ubnConfigInit(&bmd->configs[1]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[1].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "service");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[1].services.buffer = bmd->services;
    bmd->configs[1].services.size = services;
    bmd->configs[1].node.type = URTBENCHMARK_RPC_SCALING;
    ubnInit(&bmd->nodes[1], URT_THREAD_PRIO_HIGH_MAX, &bmd->configs[1]);

    // start all nodes
    urtCoreStartNodes();
    urtEventWait(coreevent, URT_EVENT_WAIT_ANY, URT_DELAY_INFINITE);

    // arm trigger timer
    _urtbenchmark_timer.counter = 0;
    _urtbenchmark_timer.iterations = URTBENCHMARK_CFG_REPETITIONS;
    aosTimerPeriodicInterval(&_urtbenchmark_timer.timer, MICROSECONDS_PER_SECOND / URTBENCHMARK_CFG_FREQUENCY, _urtbenchmark_triggercb, NULL);

    // wait for benchmark termination
    _urtbenchmark_waitForTermination(terminationevent, stress);

    // stop all nodes and reset
    urtCoreStopNodes(URT_STATUS_OK);
    urtThreadJoin(bmd->nodes[0].node.thread);
    urtThreadJoin(bmd->nodes[1].node.thread);
    _urtbenchmark_reset();

    // print all latencies
    for (size_t s = 0; s < services; ++s) {
      for (size_t i = 0; i < URTBENCHMARK_CFG_REPETITIONS; ++i) {
        chprintf(stream, "%8u\t%7u\t%9u\t%8u\t%8u\n", services, s+1, i+1, bmd->latencybuffers.dispatch[s][i], bmd->latencybuffers.retrieve[s][i]);
      }
    }
  }

  return AOS_OK;
}

#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true) || defined(__DOXYGEN__)
int _urtbenchmark_rpc_qos_deadline(BaseSequentialStream* const stream,
                                   const urt_osEventMask_t coreevent,
                                   const urt_osEventMask_t terminationevent,
                                   const bool stress)
{
  // local constants
  _urtbenchmark_data_rpc_deadline_t* const bmd = &_urtbenchmark_data.rpc.deadline;
  const urt_serviceid_t serviceid = 0;
  // local variables
  size_t violations = 0;

  // print intro
  chprintf(stream, "HRT request <-> service\n");
  chprintf(stream, "CPU stress: %s\n", stress ? "enabled" : "disabled");
  chprintf(stream, "critical latency: %u us\n", URTBENCHMARK_CFG_QOSTIME);

  // repeat the benchmark
  for (size_t repetition = 0; repetition < URTBENCHMARK_CFG_REPETITIONS; ++repetition) {
    // prepare request node
    _urtbenchmark_prepare_request(&bmd->request, bmd->latencybuffers.retrieve[repetition], NULL, 0, serviceid, URT_REQUEST_RETRIEVE_DETERMINED, URT_HRT);
    bmd->request.hrtrequest.deadline = URTBENCHMARK_CFG_QOSTIME;
    bmd->request.hrtrequest.jitter = 0;
    memset(bmd->latencybuffers.retrieve[repetition], 0, sizeof(bmd->latencybuffers.retrieve[repetition]));
    ubnConfigInit(&bmd->configs[0]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[0].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "request");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[0].node.trigger = &_urtbenchmark_timer.events.trigger;
    bmd->configs[0].requests.buffer = &bmd->request;
    bmd->configs[0].requests.size = 1;
    bmd->configs[0].node.type = URTBENCHMARK_RPC_QOS;
    ubnInit(&bmd->nodes[0], URT_THREAD_PRIO_HIGH_MAX, &bmd->configs[0]);

    // prepare service node
    _urtbenchmark_prepare_service(&bmd->service, NULL, 0, &bmd->delaybuffer, bmd->latencybuffers.dispatch[repetition], serviceid);
    memset(bmd->latencybuffers.dispatch[repetition], 0, sizeof(bmd->latencybuffers.dispatch[repetition]));
    ubnConfigInit(&bmd->configs[1]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[1].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "service");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[1].services.buffer = &bmd->service;
    bmd->configs[1].services.size = 1;
    bmd->configs[1].node.type = URTBENCHMARK_RPC_QOS;
    ubnInit(&bmd->nodes[1], URT_THREAD_PRIO_HIGH_MAX, &bmd->configs[1]);

    // start all nodes
    urtCoreStartNodes();
    urtEventWait(coreevent, URT_EVENT_WAIT_ANY, URT_DELAY_INFINITE);

    // arm trigger timer
    _urtbenchmark_timer.counter = 0;
    _urtbenchmark_timer.iterations = URTBENCHMARK_CFG_QOSSTEPS;
    aosSysGetUptime(&_urtbenchmark_timer.time);
    urt_osTime_t time = _urtbenchmark_timer.time;
    urtTimeAdd(&time, (URTBENCHMARK_CFG_QOSTIME + URTBENCHMARK_CFG_QOSSTEPS-1) / URTBENCHMARK_CFG_QOSSTEPS);
    aosTimerSetAbsolute(&_urtbenchmark_timer.timer, time, _urtbenchmark_triggercb_rpcqos, &bmd->delaybuffer);

    // wait for benchmark termination
    _urtbenchmark_waitForTermination(terminationevent, stress);
    if (urtCoreGetStatus() == URT_STATUS_DEADLINEVIOLATION) {
      ++violations;
    }

    // stop all nodes and reset
    urtCoreStopNodes(URT_STATUS_OK);
    urtThreadJoin(bmd->nodes[0].node.thread);
    urtThreadJoin(bmd->nodes[1].node.thread);;
    _urtbenchmark_reset();
  }

  // print all latencies
  chprintf(stream, "STEP\tITERATION\tDISPATCH\tRETRIEVE\n");
  for (size_t s = 0; s < URTBENCHMARK_CFG_QOSSTEPS; ++s) {
    for (size_t i = 0; i < URTBENCHMARK_CFG_REPETITIONS; ++i) {
      chprintf(stream, "%4u%9u\t%8u\t%8u\n", s+1, i+1, bmd->latencybuffers.dispatch[i][s], bmd->latencybuffers.retrieve[i][s]);
    }
  }
  chprintf(stream, "violations: %u/%u\n", violations, URTBENCHMARK_CFG_REPETITIONS);

  return AOS_OK;
}
#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) */

#if (URT_CFG_RPC_QOS_JITTERCHECKS == true) || defined(__DOXYGEN__)
int _urtbenchmark_rpc_qos_jitter(BaseSequentialStream* const stream,
                                 const urt_osEventMask_t coreevent,
                                 const urt_osEventMask_t terminationevent,
                                 const bool stress)
{
  // local constants
  _urtbenchmark_data_rpc_jitter_t* const bmd = &_urtbenchmark_data.rpc.jitter;
  const urt_serviceid_t serviceid = 0;
  // local variables
  size_t violations = 0;

  // print intro
  chprintf(stream, "HRT request <-> service\n");
  chprintf(stream, "CPU stress: %s\n", stress ? "enabled" : "disabled");
  chprintf(stream, "critical jitter: %u us\n", URTBENCHMARK_CFG_QOSTIME);

  // repeat the benchmark
  for (size_t repetition = 0; repetition < URTBENCHMARK_CFG_REPETITIONS; ++repetition) {
    // prepare request node
    _urtbenchmark_prepare_request(&bmd->request, bmd->latencybuffers.retrieve[repetition], NULL, 0, serviceid, URT_REQUEST_RETRIEVE_DETERMINED, URT_HRT);
    bmd->request.hrtrequest.deadline = 0;
    bmd->request.hrtrequest.jitter = URTBENCHMARK_CFG_QOSTIME;
    memset(bmd->latencybuffers.retrieve[repetition], 0, sizeof(bmd->latencybuffers.retrieve[repetition]));
    ubnConfigInit(&bmd->configs[0]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[0].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "request");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[0].node.trigger = &_urtbenchmark_timer.events.trigger;
    bmd->configs[0].requests.buffer = &bmd->request;
    bmd->configs[0].requests.size = 1;
    bmd->configs[0].node.type = URTBENCHMARK_RPC_QOS;
    ubnInit(&bmd->nodes[0], URT_THREAD_PRIO_HIGH_MAX, &bmd->configs[0]);

    // prepare service node
    _urtbenchmark_prepare_service(&bmd->service, NULL, 0, &bmd->delaybuffer, bmd->latencybuffers.dispatch[repetition], serviceid);
    memset(bmd->latencybuffers.dispatch[repetition], 0, sizeof(bmd->latencybuffers.dispatch[repetition]));
    ubnConfigInit(&bmd->configs[1]);
#if (CH_CFG_USE_REGISTRY == TRUE)
    chsnprintf(bmd->configs[1].node.name, URTBENCHMARK_CFG_THREADNAMELENGTH, "service");
#endif /* (CH_CFG_USE_REGISTRY == TRUE) */
    bmd->configs[1].services.buffer = &bmd->service;
    bmd->configs[1].services.size = 1;
    bmd->configs[1].node.type = URTBENCHMARK_RPC_QOS;
    ubnInit(&bmd->nodes[1], URT_THREAD_PRIO_HIGH_MAX, &bmd->configs[1]);

    // start all nodes
    urtCoreStartNodes();
    urtEventWait(coreevent, URT_EVENT_WAIT_ANY, URT_DELAY_INFINITE);

    // arm trigger timer
    _urtbenchmark_timer.counter = 0;
    _urtbenchmark_timer.iterations = URTBENCHMARK_CFG_QOSSTEPS + 1;
    aosSysGetUptime(&_urtbenchmark_timer.time);
    urt_osTime_t time = _urtbenchmark_timer.time;
    urtTimeAdd(&time, (URTBENCHMARK_CFG_QOSTIME + URTBENCHMARK_CFG_QOSSTEPS-1) / URTBENCHMARK_CFG_QOSSTEPS);
    aosTimerSetAbsolute(&_urtbenchmark_timer.timer, time, _urtbenchmark_triggercb_rpcqos, &bmd->delaybuffer);

    // wait for benchmark termination
    _urtbenchmark_waitForTermination(terminationevent, stress);
    if (urtCoreGetStatus() == URT_STATUS_JITTERVIOLATION) {
      ++violations;
    }

    // stop all nodes and reset
    urtCoreStopNodes(URT_STATUS_OK);
    urtThreadJoin(bmd->nodes[0].node.thread);
    urtThreadJoin(bmd->nodes[1].node.thread);
    _urtbenchmark_reset();
  }

  // print all latencies
  chprintf(stream, "STEP\tITERATION\tDISPATCH\tRETRIEVE\tJITTER\n");
  for (size_t s = 0; s < URTBENCHMARK_CFG_QOSSTEPS + 1; ++s) {
    for(size_t i = 0; i < URTBENCHMARK_CFG_REPETITIONS; ++i) {
      chprintf(stream, "%4u\t%9u\t%8u\t%8u\t%6u\n", s+1, i+1, bmd->latencybuffers.dispatch[i][s], bmd->latencybuffers.retrieve[i][s], (bmd->latencybuffers.retrieve[i][s] != 0) ? bmd->latencybuffers.retrieve[i][s] - bmd->latencybuffers.retrieve[i][0] : 0);
    }
  }
  chprintf(stream, "violations: %u/%u\n", violations, URTBENCHMARK_CFG_REPETITIONS);

  return AOS_OK;
}
#endif /* (URT_CFG_RPC_QOS_JITTERCHECKS == true) */

static int _urtbenchmark_shellcb_rpc(BaseSequentialStream* stream, int argc, const char* argv[])
{
  URTBENCHMARK_INTRO();

  // benchmark number of hops
  if ((argc == 3 || argc == 4) &&
      (strcmp(argv[1], "--hops") == 0) &&
      _urtbenchmark_shellcb_checkXRT(argv[2])) {
    status = _urtbenchmark_rpc_hops(stream, coreevent, terminationevent,
                                    _urtbenchmark_shellcb_extractRtClass(argv[2]),
                                    _urtbenchmark_shellcb_checkStress(argv[3]));
  }

  // benchmark payload scaling
  else if ((argc == 3 || argc == 4) &&
           (strcmp(argv[1], "--payload") == 0) &&
           _urtbenchmark_shellcb_checkXRT(argv[2])) {
    status = _urtbenchmark_rpc_payload(stream, coreevent, terminationevent,
                                       _urtbenchmark_shellcb_extractRtClass(argv[2]),
                                       _urtbenchmark_shellcb_checkStress(argv[3]));
  }

  // benchmark request scaling
  else if ((argc == 3 || argc == 4) &&
           (strcmp(argv[1], "--request") == 0) &&
           _urtbenchmark_shellcb_checkXRT(argv[2])) {
    status = _urtbenchmark_rpc_request(stream, coreevent, terminationevent,
                                       _urtbenchmark_shellcb_extractRtClass(argv[2]),
                                       _urtbenchmark_shellcb_checkStress(argv[3]));
  }

  // benchmark service scaling
  else if ((argc == 3 || argc == 4) &&
           (strcmp(argv[1], "--service") == 0) &&
           _urtbenchmark_shellcb_checkXRT(argv[2])) {
    status = _urtbenchmark_rpc_service(stream, coreevent, terminationevent,
                                       _urtbenchmark_shellcb_extractRtClass(argv[2]),
                                       _urtbenchmark_shellcb_checkStress(argv[3]));
  }

#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true)
  else if ((argc == 3 || argc == 4) &&
           ((strcmp(argv[1], "--QoS") == 0 || strcmp(argv[1], "--qos") == 0)) &&
           (strcmp(argv[2], "deadline") == 0)) {
    status = _urtbenchmark_rpc_qos_deadline(stream, coreevent, terminationevent,
                                            _urtbenchmark_shellcb_checkStress(argv[3]));
  }
#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) */

#if (URT_CFG_RPC_QOS_JITTERCHECKS == true)
  else if ((argc == 3 || argc == 4) &&
           ((strcmp(argv[1], "--QoS") == 0 || strcmp(argv[1], "--qos") == 0)) &&
           (strcmp(argv[2], "jitter") == 0)) {
    status = _urtbenchmark_rpc_qos_jitter(stream, coreevent, terminationevent,
                                          _urtbenchmark_shellcb_checkStress(argv[3]));
  }
#endif /* (URT_CFG_RPC_QOS_JITTERCHECKS == true) */

  // benchmark all
  else if ((argc == 2 || argc == 3) &&
           (strcmp(argv[1], "--all") == 0)) {
    const char d = '-';
    const bool stresscpu = _urtbenchmark_shellcb_checkStress(argv[2]);

    // ping-self benchmarks
    _urtbenchmark_printHeading(stream, "HOPS (NRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_rpc_hops(stream, coreevent, terminationevent, URT_NRT, stresscpu);
    _urtbenchmark_printHeading(stream, "HOPS (SRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_rpc_hops(stream, coreevent, terminationevent, URT_SRT, stresscpu);
    _urtbenchmark_printHeading(stream, "HOPS (FRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_rpc_hops(stream, coreevent, terminationevent, URT_FRT, stresscpu);
    _urtbenchmark_printHeading(stream, "HOPS (HRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_rpc_hops(stream, coreevent, terminationevent, URT_HRT, stresscpu);

    // payload benchmarks
    _urtbenchmark_printHeading(stream, "PAYLOAD (NRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_rpc_payload(stream, coreevent, terminationevent, URT_NRT, stresscpu);
    _urtbenchmark_printHeading(stream, "PAYLOAD (SRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_rpc_payload(stream, coreevent, terminationevent, URT_SRT, stresscpu);
    _urtbenchmark_printHeading(stream, "PAYLOAD (FRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_rpc_payload(stream, coreevent, terminationevent, URT_FRT, stresscpu);
    _urtbenchmark_printHeading(stream, "PAYLOAD (HRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_rpc_payload(stream, coreevent, terminationevent, URT_HRT, stresscpu);

    // request benchmarks
    _urtbenchmark_printHeading(stream, "REQUEST (NRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_rpc_request(stream, coreevent, terminationevent, URT_NRT, stresscpu);
    _urtbenchmark_printHeading(stream, "REQUEST (SRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_rpc_request(stream, coreevent, terminationevent, URT_SRT, stresscpu);
    _urtbenchmark_printHeading(stream, "REQUEST (FRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_rpc_request(stream, coreevent, terminationevent, URT_FRT, stresscpu);
    _urtbenchmark_printHeading(stream, "REQUEST (HRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_rpc_request(stream, coreevent, terminationevent, URT_HRT, stresscpu);

    // service benchmark
    _urtbenchmark_printHeading(stream, "SERVICE (NRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_rpc_service(stream, coreevent, terminationevent, URT_NRT, stresscpu);
    _urtbenchmark_printHeading(stream, "SERVICE (SRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_rpc_service(stream, coreevent, terminationevent, URT_SRT, stresscpu);
    _urtbenchmark_printHeading(stream, "SERVICE (FRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_rpc_service(stream, coreevent, terminationevent, URT_FRT, stresscpu);
    _urtbenchmark_printHeading(stream, "SERVICE (HRT)", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_rpc_service(stream, coreevent, terminationevent, URT_HRT, stresscpu);

#if (URT_CFG_RPC_QOS_DEADLINECHECKS == true)
    // deadline QoS benchmark
    _urtbenchmark_printHeading(stream, "DEADLINE", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_rpc_qos_deadline(stream, coreevent, terminationevent, stresscpu);
#endif /* (URT_CFG_RPC_QOS_DEADLINECHECKS == true) */

#if (URT_CFG_RPC_QOS_JITTERCHECKS == true)
    // jitter QoS benchmark
    _urtbenchmark_printHeading(stream, "JITTER", d, URTBENCHMARK_HEADERWIDTH);
    _urtbenchmark_rpc_qos_jitter(stream, coreevent, terminationevent, stresscpu);
#endif /* (URT_CFG_RPC_QOS_JITTERCHECKS == true) */

    status = AOS_OK;
  }

  // print help
  const bool printhelp = (aosShellcmdCheckHelp(argc, argv, true) != AOS_SHELL_HELPREQUEST_NONE);
  if (printhelp) {
    status = AOS_OK;
  }
  if (printhelp || status == AOS_INVALIDARGUMENTS) {
    chprintf(stream, "usage: %s BENCHMARK [OPTION] [--stress|-s]\n", argv[0]);
    chprintf(stream, "BENCHMARK:\n");
    chprintf(stream, "  --hops\n");
    chprintf(stream, "    Benchmark request propagation latency over multiple hops.\n");
    chprintf(stream, "    OPTION (required):\n");
    chprintf(stream, "      NRT, nrt - Use NRT requests for benchmarking.\n");
    chprintf(stream, "      SRT, srt - Use SRT requests for benchmarking.\n");
    chprintf(stream, "      FRT, frt - Use FRT requests for benchmarking.\n");
    chprintf(stream, "      HRT, hrt - Use HRT requests for benchmarking.\n");
    chprintf(stream, "  --payload\n");
    chprintf(stream, "    Benchmark message payload scaling.\n");
    chprintf(stream, "    OPTION (required):\n");
    chprintf(stream, "      NRT, nrt - Use an NRT request for benchmarking.\n");
    chprintf(stream, "      SRT, srt - Use an SRT request for benchmarking.\n");
    chprintf(stream, "      FRT, frt - Use an FRT request for benchmarking.\n");
    chprintf(stream, "      HRT, hrt - Use an HRT request for benchmarking.\n");
    chprintf(stream, "  --request\n");
    chprintf(stream, "    Benchmark scaling of number of requests.\n");
    chprintf(stream, "    OPTION (required):\n");
    chprintf(stream, "      NRT, nrt - Use NRT requests for benchmarking.\n");
    chprintf(stream, "      SRT, srt - Use SRT requests for benchmarking.\n");
    chprintf(stream, "      FRT, frt - Use FRT requests for benchmarking.\n");
    chprintf(stream, "      HRT, hrt - Use HRT requests for benchmarking.\n");
    chprintf(stream, "  --service\n");
    chprintf(stream, "    Benchmark scaling of number of services.\n");
    chprintf(stream, "    OPTION (required):\n");
    chprintf(stream, "      NRT, nrt - Use NRT requests for benchmarking.\n");
    chprintf(stream, "      SRT, srt - Use SRT requests for benchmarking.\n");
    chprintf(stream, "      FRT, frt - Use FRT requests for benchmarking.\n");
    chprintf(stream, "      HRT, hrt - Use HRT requests for benchmarking.\n");
    chprintf(stream, "  --QoS, --qos\n");
    chprintf(stream, "    Benchmark Quality of Service (QoS) performance.\n");
    chprintf(stream, "    OPTION (required):\n");
    chprintf(stream, "      deadline - Benchmark deadline checks.\n");
    chprintf(stream, "      jitter   - Benchmark jitter checks.\n");
    chprintf(stream, "  --all\n");
    chprintf(stream, "    Run all benchmarks.\n");
    chprintf(stream, "  --help, -h\n");
    chprintf(stream, "    Print this help text.\n");
    chprintf(stream, "--stress, -s (optional):\n");
    chprintf(stream, "  Stress CPU during benchmarks.\n");
  }

  URTBENCHMARK_OUTRO();

  return status;
}

#endif /* (URT_CFG_RPC_ENABLED == true) */

static int _urtbenchmark_shellcb_all(BaseSequentialStream* stream, int argc, const char* argv[])
{
  // print help
  if (aosShellcmdCheckHelp(argc, argv, true) == AOS_SHELL_HELPREQUEST_EXPLICIT) {
    chprintf(stream, "usage: %s [--stress|-s]\n", argv[0]);
    chprintf(stream, "--stress, -s (optional):\n");
    chprintf(stream, "  Stress CPU during benchmark.\n");
    return  AOS_OK;
  }

  // local variables
  const char d = '#';
  const char* targv[AMIROOS_CFG_SHELL_MAXARGS] = {NULL};

  // core benchmarks
  targv[0] = _urtbenchmark_shellcmd_memory.name;
  _urtbenchmark_printHeading(stream, "MEMORY", d, URTBENCHMARK_HEADERWIDTH);
  _urtbenchmark_shellcb_memory(stream, 1, targv);
  targv[0] = _urtbenchmark_shellcmd_node.name;
  targv[1] = argv[1];
  _urtbenchmark_printHeading(stream, "NODE", d, URTBENCHMARK_HEADERWIDTH);
  _urtbenchmark_shellcb_node(stream, (targv[1] == NULL) ? 1 : 2, targv);

#if (URT_CFG_PUBSUB_ENABLED == true)
  // publish-subscribe benchmarks
  _urtbenchmark_printHeading(stream, "PUBLISH-SUBSCRIBE", d, URTBENCHMARK_HEADERWIDTH);
  targv[0] = _urtbenchmark_shellcmd_pubsub.name;
  targv[1] = "--all";
  targv[2] = argv[1];
  _urtbenchmark_shellcb_pubsub(stream, (targv[2] == NULL) ? 2 : 3, targv);
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true)
  // publish-subscribe benchmarks
  _urtbenchmark_printHeading(stream, "RPC", d, URTBENCHMARK_HEADERWIDTH);
  targv[0] = _urtbenchmark_shellcmd_rpc.name;
  targv[1] = "--all";
  targv[2] = argv[1];
  _urtbenchmark_shellcb_rpc(stream, (targv[2] == NULL) ? 2 : 3, targv);
#endif /* (URT_CFG_RPC_ENABLED == true) */

  return AOS_OK;
}

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup configs_urtbenchmark
 * @{
 */

/**
 * @brief   Initializes all data and applications for the benchmark configuration.
 */
void urtbenchmarkAppsInit(void)
{
  // initialize static data
  aosTimerInit(&_urtbenchmark_timer.timer);
  urtEventSourceInit(&_urtbenchmark_timer.events.trigger);
  urtEventSourceInit(&_urtbenchmark_timer.events.termination);

  // add shell commands
  aosShellAddCommand(&_urtbenchmark_shellcmd_memory);
  aosShellAddCommand(&_urtbenchmark_shellcmd_node);
#if (URT_CFG_PUBSUB_ENABLED == true)
  aosShellAddCommand(&_urtbenchmark_shellcmd_pubsub);
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
#if (URT_CFG_RPC_ENABLED == true)
  aosShellAddCommand(&_urtbenchmark_shellcmd_rpc);
#endif /* (URT_CFG_RPC_ENABLED == true) */
  aosShellAddCommand(&_urtbenchmark_shellcmd_all);

  return;
}

/** @} */
