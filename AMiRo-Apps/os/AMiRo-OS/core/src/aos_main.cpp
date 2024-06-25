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
 * @file    aos_main.cpp
 * @brief   Main function.
 */

#include <amiroos.h>

/*
 * hook to add further includes
 */
#if defined(AMIROOS_CFG_MAIN_EXTRA_INCLUDE_HEADER)
# define __include(file) <file>
# define _include(file) __include(file)
# include _include(AMIROOS_CFG_MAIN_EXTRA_INCLUDE_HEADER)
# undef _include
# undef __include
#endif /* defined(AMIROOS_CFG_MAIN_EXTRA_INCLUDE_HEADER) */

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/**
 * @brief   Event mask to identify GPIO events.
 */
#define EVENTMASK_GPIO                          EVENT_MASK(0)

/**
 * @brief   Event mask to identify OS events.
 */
#define EVENTMASK_OS                            EVENT_MASK(1)

#if (AMIROOS_CFG_SSSP_ENABLE == true) || defined(__DOXYGEN__)
#if (AMIROOS_CFG_SSSP_MSI == true) || defined(__DOXYGEN__)

/**
 * @brief   Event mask to identify SSSP timeout events (MSI only).
 */
#define EVENTMASK_SSSPTIMEOUT                   EVENT_MASK(2)

#endif /* (AMIROOS_CFG_SSSP_MSI == true) */

/**
 * @brief   Event mask to identify SSSP delay events.
 */
#define EVENTMASK_SSSPDELAY                     EVENT_MASK(3)

#if (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) || defined(__DOXYGEN__)

/**
 * @brief   Event mask to identify SSSP synchronization events.
 */
#define EVENTMASK_SSSPSYNC                      EVENT_MASK(2)

#endif /* (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) */
#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */

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
 * @brief   Listener object for GPIO events.
 */
static event_listener_t _eventListenerGPIO;

/**
 * @brief   Listener object for OS events.
 */
static event_listener_t _eventListenerOS;

#if defined(MODULE_HAL_SERIAL) || defined(__DOXYGEN__)

/**
 * @brief   I/O channel for the programmer interface.
 */
static AosIOChannel _stdiochannel;

#endif /* defined(MODULE_HAL_SERIAL) */

/*
 * hook to add further static variables
 */
#if defined(AMIROOS_CFG_MAIN_EXTRA_STATIC_VARIABLES)
AMIROOS_CFG_MAIN_EXTRA_STATIC_VARIABLES
#endif /* defined(AMIROOS_CFG_MAIN_EXTRA_STATIC_VARIABLES) */

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   Prints an error message about an unexpected event.
 *
 * @param[in] mask    The event mask.
 * @param[in] flags   The event flags.
 */
#define _aosMainUnexpectedEventError(mask, flags)                               \
  aosDbgPrintf("CTRL (%u): unexpected/unknown event received. mask: 0x%08X; flags: 0x%08X\n", __LINE__, mask, flags)

#if ((AMIROOS_CFG_SSSP_ENABLE == true) && (HAL_USE_RTC == TRUE)) ||             \
    defined(__DOXYGEN__)
#if (AMIROOS_CFG_SSSP_MASTER == true) || defined(__DOXYGEN__)

/**
 * @brief   Converter function to encode a TM value to a single unsigned 64 bit integer.
 *
 * @details Contents of the TM struct are mapped as follows:
 *            bits  |63     62|61      53|52    50|49         26|25     22|21     17|16     12|11      6|5       0|
 *           \#bits |       2 |        9 |      3 |          24 |       4 |       5 |       5 |       6 |       6 |
 *            value |   isdst |     yday |   wday |        year |     mon |    mday |    hour |     min |     sec |
 *            range | special | [0, 365] | [0, 6] | [1900, ...] | [0, 11] | [1, 31] | [0, 23] | [0, 59] | [0, 61] |
 *          The Daylight Saving Time Flag (isdsst) is encoded as follows:
 *            DST not in effect         -> 0
 *            DST in effect             -> 1
 *            no information available  -> 2
 *
 * @param[in] src   Pointer to the TM struct to encode.
 *
 * @return  An unsigned 64 bit integer, which holds the encoded time value.
 */
uint64_t _aosMainTM2U64(struct tm* src)
{
  aosDbgCheck(src != NULL);

  return (((uint64_t)(src->tm_sec  & 0x0000003F) << (0))  |
          ((uint64_t)(src->tm_min  & 0x0000003F) << (6))  |
          ((uint64_t)(src->tm_hour & 0x0000001F) << (12)) |
          ((uint64_t)(src->tm_mday & 0x0000001F) << (17)) |
          ((uint64_t)(src->tm_mon  & 0x0000000F) << (22)) |
          ((uint64_t)(src->tm_year & 0x00FFFFFF) << (26)) |
          ((uint64_t)(src->tm_wday & 0x00000007) << (50)) |
          ((uint64_t)(src->tm_yday & 0x000001FF) << (53)) |
          ((uint64_t)((src->tm_isdst == 0) ? 0 : (src->tm_isdst > 0) ? 1 : 2) << (62)));
}

/**
 * @brief   Serializes 64 bit unsigned integer input and stores it in a byte
 *          array.
 * @details Serialization is performed in big-endian fashion.
 *
 * @param[out]  dst   Pointer to the output buffer.
 * @param[in]   src   Data to be serialized.
 */
void _aosMainSerializeU64(uint8_t* dst, const uint64_t src)
{
  aosDbgCheck(dst != NULL);

  for (uint8_t byte = 0; byte < sizeof(uint64_t); ++byte) {
    dst[byte] = ((src >> ((sizeof(uint64_t) - (byte+1)) * 8)) & 0xFF);
  }

  return;
}

#endif /* (AMIROOS_CFG_SSSP_MASTER == true) */
#if (AMIROOS_CFG_SSSP_MASTER != true) || defined(__DOXYGEN__)

/**
 * @brief   Deserialize 64 bit unsigned integer data from a buffer.
 * @details Data is assumed to be serialized in big-endian fashion.
 *
 * @param[in] src   Pointer to the buffer of data to be deserialzed.
 *
 * @return    Deserialized 64 bit data.
 */
uint64_t _aosMainDeserializeU64(uint8_t* src)
{
  aosDbgCheck(src != NULL);

  uint64_t result = 0;
  for (uint8_t byte = 0; byte < sizeof(uint64_t); ++byte) {
    result |= ((uint64_t)src[byte] << ((sizeof(uint64_t) - (byte+1)) * 8));
  }

  return result;
}

/**
 * @brief   Converter functiomn to retrieve the encoded TM value from an
 *          unsigned 64 bit integer.
 *
 * @details For information on the encoding, please refer to @p _TM2U64
 *          function.
 *
 * @param[out] dst  The TM struct to fill with the decoded values.
 * @param[in]  src  Unsigned 64 bit integer holding the encoded TM value.
 */
void _aosMainU642TM(struct tm* dst, const uint64_t src)
{
  aosDbgCheck(dst != NULL);

  dst->tm_sec  = (src >> 0)  & 0x0000003F;
  dst->tm_min  = (src >> 6)  & 0x0000003F;
  dst->tm_hour = (src >> 12) & 0x0000001F;
  dst->tm_mday = (src >> 17) & 0x0000001F;
  dst->tm_mon  = (src >> 22) & 0x0000000F;
  dst->tm_year = (src >> 26) & 0x00FFFFFF;
  dst->tm_wday = (src >> 50) & 0x00000007;
  dst->tm_yday = (src >> 53) & 0x000001FF;
  dst->tm_isdst = (((src >> 62) & 0x03) == 0) ? 0 : (((src >> 62) & 0x03) > 0) ? 1 : -1;

  return;
}

#endif /* (AMIROOS_CFG_SSSP_MASTER != true) */
#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) && (HAL_USE_RTC == TRUE) */

/**
 * @brief   Application entry point.
 * @ingroup system
 *
 * @return  Status value, indicating success or an error state.
 */
int main(void)
{
  // local variables
  eventmask_t eventmask = 0;
  eventflags_t eventflags = 0;
  aos_shutdown_t shutdown = AOS_SHUTDOWN_NONE;
#if defined(AMIROOS_CFG_MAIN_EXTRA_THREAD_VARIABLES)
  AMIROOS_CFG_MAIN_EXTRA_THREAD_VARIABLES
#endif /* defined(AMIROOS_CFG_MAIN_EXTRA_THREAD_VARIABLES) */

  /*
   * ##########################################################################
   * # system initialization                                                  #
   * ##########################################################################
   */

  /* hardware, kernel, and operating system initialization */
  // ChibiOS/HAL and custom hal additions (if any)
  halInit();
#if defined(MODULE_INIT_HAL_EXTRA)
  MODULE_INIT_HAL_EXTRA();
#endif /* defined(MODULE_INIT_HAL_EXTRA) */

  // ChibiOS/RT kernel and custom kernel additions (if any)
  chSysInit();
#if defined(MODULE_INIT_KERNEL_EXTRA)
  MODULE_INIT_KERNEL_EXTRA();
#endif /* defined(MODULE_INIT_KERNEL_EXTRA) */

  // AMiRo-OS, additional interrupts and custom OS additions (if any)
  aosSysInit();
#if defined(MODULE_INIT_INTERRUPTS)
  MODULE_INIT_INTERRUPTS();
#endif
#if defined(MODULE_INIT_OS_EXTRA)
  MODULE_INIT_OS_EXTRA();
#endif /* defined(MODULE_INIT_OS_EXTRA) */

  /* event associations */
#if (AMIROOS_CFG_SSSP_ENABLE == true)
  {
    eventflags_t flagsmask = AMIROOS_CFG_MAIN_LOOP_GPIOEVENT_FLAGSMASK | moduleSsspEventflagPD() | moduleSsspEventflagS();
#if (AMIROOS_CFG_SSSP_MSI == true)
#if (AMIROOS_CFG_SSSP_STACK_START != true)
    flagsmask |= moduleSsspEventflagDN();
#endif /* (AMIROOS_CFG_SSSP_STACK_START != true) */
#if (AMIROOS_CFG_SSSP_STACK_END != true)
    flagsmask |= moduleSsspEventflagUP();
#endif /* (AMIROOS_CFG_SSSP_STACK_END != true) */
#endif /* (AMIROOS_CFG_SSSP_MSI == true) */
    chEvtRegisterMaskWithFlags(&aos.events.gpio, &_eventListenerGPIO, EVENTMASK_GPIO, flagsmask);
  }
#else /* (AMIROOS_CFG_SSSP_ENABLE == true) */
  if (AMIROOS_CFG_MAIN_LOOP_GPIOEVENT_FLAGSMASK) {
    chEvtRegisterMaskWithFlags(&aos.events.gpio, &_eventListenerGPIO, EVENTMASK_GPIO, AMIROOS_CFG_MAIN_LOOP_GPIOEVENT_FLAGSMASK);
  }
#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */
  chEvtRegisterMask(&aos.events.os, &_eventListenerOS, EVENTMASK_OS);

#if (AMIROOS_CFG_SSSP_ENABLE == true) && (AMIROOS_CFG_SSSP_STARTUP == true)

  /* perform SSSP basic initialization stage */

#if defined(MODULE_SSSP_BASICINIT_HOOK)
#if defined(MODULE_SSSP_BASICINIT_HOOK_ARGS)
  MODULE_SSSP_BASICINIT_HOOK(MODULE_SSSP_BASICINIT_HOOK_ARGS);
#else /* defined(MODULE_SSSP_BASICINIT_HOOK_ARGS) */
  MODULE_SSSP_BASICINIT_HOOK();
#endif /* defined(MODULE_SSSP_BASICINIT_HOOK_ARGS) */
#endif /* defined(MODULE_SSSP_BASICINIT_HOOK) */

  // proceed to startup stage 1.2
#if (AMIROOS_CFG_SSSP_MASTER == true)
  while ((shutdown == AOS_SHUTDOWN_NONE) && (aosSsspProceed(NULL, EVENTMASK_SSSPDELAY, &eventmask) != AOS_SUCCESS)) {
    /*
     * This code is executed if the received event was not about the delay.
     * The received event could be casued by any listener.
     */
    // GPIO event
    if (eventmask & _eventListenerGPIO.events) {
      eventflags = chEvtGetAndClearFlags(&_eventListenerGPIO);
      // PD event
      if (eventflags & moduleSsspEventflagPD()) {
        shutdown = AOS_SHUTDOWN_PASSIVE;
      } else {
#if defined(MODULE_SSSP_STARTUP_1_1_GPIOEVENT_HOOK)
        MODULE_SSSP_STARTUP_1_1_GPIOEVENT_HOOK(eventmask, eventflags);
#else /* defined(MODULE_SSSP_STARTUP_1_1_GPIOEVENT_HOOK) */
        /* silently ignore any other GPIO events */
#endif /* defined(MODULE_SSSP_STARTUP_1_1_GPIOEVENT_HOOK) */
      }
    }
    // OS event
    else if (eventmask & _eventListenerOS.events) {
      eventflags = chEvtGetAndClearFlags(&_eventListenerOS);
      _aosMainUnexpectedEventError(eventmask, eventflags);
    }
    // unknown event
    else {
      _aosMainUnexpectedEventError(eventmask, 0);
    }
  }
  if (shutdown == AOS_SHUTDOWN_NONE) {
    aosDbgAssert(aos.sssp.stage == AOS_SSSP_STAGE_STARTUP_1_2);
  }
#else /* (AMIROOS_CFG_SSSP_MASTER == true) */
  if (shutdown == AOS_SHUTDOWN_NONE) {
    aosSsspProceed(NULL, 0, NULL);
    aosDbgAssert(aos.sssp.stage == AOS_SSSP_STAGE_STARTUP_1_2);
  }
#endif /* (AMIROOS_CFG_SSSP_MASTER == true) */

  // proceed to startup stage 1.3
  while ((shutdown == AOS_SHUTDOWN_NONE) && (aosSsspProceed(&_eventListenerGPIO, EVENTMASK_GPIO, &eventmask) != AOS_SUCCESS)) {
    /*
     * This code is executed if the received event was not about a deactivation
     * of the snychronization signal. The received event could be caused by any
     * listener.
     */
    // GPIO event
    if (eventmask & _eventListenerGPIO.events) {
      eventflags = chEvtGetAndClearFlags(&_eventListenerGPIO);
      // PD event
      if (eventflags & moduleSsspEventflagPD()) {
        shutdown = AOS_SHUTDOWN_PASSIVE;
      } else {
#if defined(MODULE_SSSP_STARTUP_1_2_GPIOEVENT_HOOK)
        MODULE_SSSP_STARTUP_1_2_GPIOEVENT_HOOK(eventmask, eventflags);
#else /* defined(MODULE_SSSP_STARTUP_1_2_GPIOEVENT_HOOK) */
        /* silently ignore any other GPIO events */
#endif /* defined(MODULE_SSSP_STARTUP_1_2_GPIOEVENT_HOOK) */
      }
    }
    // OS event
    else if (eventmask & _eventListenerOS.events) {
      eventflags = chEvtGetAndClearFlags(&_eventListenerOS);
      _aosMainUnexpectedEventError(eventmask, eventflags);
    }
    // unknown event
    else {
      _aosMainUnexpectedEventError(eventmask, 0);
    }
  }
  if (shutdown == AOS_SHUTDOWN_NONE) {
    aosDbgAssert(aos.sssp.stage == AOS_SSSP_STAGE_STARTUP_1_3);
  }

  // proceed to startup stage 2.1
#if (AMIROOS_CFG_SSSP_MASTER == true)
  while ((shutdown == AOS_SHUTDOWN_NONE) && (aosSsspProceed(NULL, EVENTMASK_SSSPDELAY, &eventmask) != AOS_SUCCESS)) {
    /*
     * This code is executed if the received event was not about the delay.
     * The received event could be caused by any listener.
     */
    // GPIO event
    if (eventmask & _eventListenerGPIO.events) {
      eventflags = chEvtGetAndClearFlags(&_eventListenerGPIO);
      // PD event
      if (eventflags & moduleSsspEventflagPD()) {
        shutdown = AOS_SHUTDOWN_PASSIVE;
      } else {
#if defined(MODULE_SSSP_STARTUP_1_3_GPIOEVENT_HOOK)
        MODULE_SSSP_STARTUP_1_3_GPIOEVENT_HOOK(eventmask, eventflags);
#else /* defined(MODULE_SSSP_STARTUP_1_3_GPIOEVENT_HOOK) */
        /* silently ignore any other GPIO events */
#endif /* defined(MODULE_SSSP_STARTUP_1_3_GPIOEVENT_HOOK) */
      }
    }
    // OS event
    else if (eventmask & _eventListenerOS.events) {
      eventflags = chEvtGetAndClearFlags(&_eventListenerOS);
      _aosMainUnexpectedEventError(eventmask, eventflags);
    }
    // unknown event
    else {
      _aosMainUnexpectedEventError(eventmask, 0);
    }
  }
#else /* (AMIROOS_CFG_SSSP_MASTER == true) */
  while ((shutdown == AOS_SHUTDOWN_NONE) && (aosSsspProceed(&_eventListenerGPIO, EVENTMASK_GPIO, &eventmask) != AOS_SUCCESS)) {
    /*
     * This code is executed if the received event was not about a deactivation
     * of the snychronization signal. The received event could be caused by any
     * listener.
     */
    // GPIO event
    if (eventmask & _eventListenerGPIO.events) {
      eventflags = chEvtGetAndClearFlags(&_eventListenerGPIO);
      // PD event
      if (eventflags & moduleSsspEventflagPD()) {
        shutdown = AOS_SHUTDOWN_PASSIVE;
      } else {
#if defined(MODULE_SSSP_STARTUP_1_2_GPIOEVENT_HOOK)
        MODULE_SSSP_STARTUP_1_2_GPIOEVENT_HOOK(eventmask, eventflags);
#else /* defined(MODULE_SSSP_STARTUP_1_2_GPIOEVENT_HOOK) */
        /* silently ignore any other GPIO events */
#endif /* defined(MODULE_SSSP_STARTUP_1_2_GPIOEVENT_HOOK) */
      }
    }
    // OS event
    else if (eventmask & _eventListenerOS.events) {
      eventflags = chEvtGetAndClearFlags(&_eventListenerOS);
      _aosMainUnexpectedEventError(eventmask, eventflags);
    }
    // unknown event
    else {
      _aosMainUnexpectedEventError(eventmask, 0);
    }
  }
#endif /* (AMIROOS_CFG_SSSP_MASTER == true) */
  if (shutdown == AOS_SHUTDOWN_NONE) {
    aosDbgAssert(aos.sssp.stage == AOS_SSSP_STAGE_STARTUP_2_1);
  }

#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) &&
          (AMIROOS_CFG_SSSP_STARTUP == true) */

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_1)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_1_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_1(AMIROOS_CFG_MAIN_INIT_HOOK_1_ARGS);
#else /* defined(AMIROOS_CFG_MAIN_INIT_HOOK_1_ARGS) */
  AMIROOS_CFG_MAIN_INIT_HOOK_1();
#endif /* defined(AMIROOS_CFG_MAIN_INIT_HOOK_1_ARGS) */
#endif /* defined(AMIROOS_CFG_MAIN_INIT_HOOK_1) */

  /* periphery communication interfaces initialization */
#if defined(MODULE_INIT_PERIPHERY_INTERFACES)
  // module specific initialization
  MODULE_INIT_PERIPHERY_INTERFACES();
#endif /* defined(MODULE_INIT_PERIPHERY_INTERFACES) */
#if defined(MODULE_HAL_SERIAL)
  // user interface
  aosIOChannelInit(&_stdiochannel, (BaseAsynchronousChannel*)&MODULE_HAL_SERIAL, AOS_IOCHANNEL_OUTPUT);
  aosOStreamAddChannel(&aos.iostream, &_stdiochannel);
#endif /* defined(MODULE_HAL_SERIAL) */

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_2)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_2_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_2(AMIROOS_CFG_MAIN_INIT_HOOK_2_ARGS);
#else /* defined(AMIROOS_CFG_MAIN_INIT_HOOK_2_ARGS) */
  AMIROOS_CFG_MAIN_INIT_HOOK_2();
#endif /* defined(AMIROOS_CFG_MAIN_INIT_HOOK_2_ARGS) */
#endif /* defined(AMIROOS_CFG_MAIN_INIT_HOOK_2) */

  /* module is ready -> print welcome prompt */
  aosprintf_module("\n");
  aosprintf_module("######################################################################\n");
  aosprintf_module("# AMiRo-OS is an operating system designed for the Autonomous Mini   #\n");
  aosprintf_module("# Robot (AMiRo) platform.                                            #\n");
  aosprintf_module("# Copyright (C) 2016..2022  Thomas Schöpping et al.                  #\n");
  aosprintf_module("#                                                                    #\n");
  aosprintf_module("# This is free software; see the source for copying conditions.      #\n");
  aosprintf_module("# There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR  #\n");
  aosprintf_module("# A PARTICULAR PURPOSE.                                              #\n");
  aosprintf_module("# The development of this software was supported by the Excellence   #\n");
  aosprintf_module("# Cluster EXC 227 Cognitive Interaction Technology. The Excellence   #\n");
  aosprintf_module("# Cluster EXC 227 is a grant of the Deutsche Forschungsgemeinschaft  #\n");
  aosprintf_module("# (DFG) in the context of the German Excellence Initiative.          #\n");
  aosprintf_module("######################################################################\n");
  aosprintf_module("\n");

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_3)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_3_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_3(AMIROOS_CFG_MAIN_INIT_HOOK_3_ARGS);
#else /* defined(AMIROOS_CFG_MAIN_INIT_HOOK_3_ARGS) */
  AMIROOS_CFG_MAIN_INIT_HOOK_3();
#endif /* defined(AMIROOS_CFG_MAIN_INIT_HOOK_3_ARGS) */
#endif /* defined(AMIROOS_CFG_MAIN_INIT_HOOK_3) */

#if (AMIROOS_CFG_TESTS_ENABLE == true)
#if defined(MODULE_INIT_TESTS)
  MODULE_INIT_TESTS();
#else /* defined(MODULE_INIT_TESTS) */
# warning "AMIROOS_CFG_TESTS_ENABLE set to true, but MODULE_INIT_TESTS() not defined"
#endif /* defined(MODULE_INIT_TESTS) */
#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_4)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_4_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_4(AMIROOS_CFG_MAIN_INIT_HOOK_4_ARGS);
#else /* defined(AMIROOS_CFG_MAIN_INIT_HOOK_4_ARGS) */
  AMIROOS_CFG_MAIN_INIT_HOOK_4();
#endif /* defined(AMIROOS_CFG_MAIN_INIT_HOOK_4_ARGS) */
#endif /* defined(AMIROOS_CFG_MAIN_INIT_HOOK_4) */

#if (AMIROOS_CFG_SSSP_ENABLE == true)

  // proceed to startup stage 2.2
#if (AMIROOS_CFG_SSSP_MASTER == true)
  while ((shutdown == AOS_SHUTDOWN_NONE) && (aosSsspProceed(NULL, EVENTMASK_SSSPDELAY, &eventmask) != AOS_SUCCESS)) {
    /*
     * This code is executed if the received event was not about the delay.
     * The received event could be caused by any listener.
     */
    // GPIO event
    if (eventmask & _eventListenerGPIO.events) {
      eventflags = chEvtGetAndClearFlags(&_eventListenerGPIO);
      // PD event
      if (eventflags & moduleSsspEventflagPD()) {
        shutdown = AOS_SHUTDOWN_PASSIVE;
      } else {
#if defined(MODULE_SSSP_STARTUP_1_3_GPIOEVENT_HOOK)
        MODULE_SSSP_STARTUP_1_3_GPIOEVENT_HOOK(eventmask, eventflags);
#else /* defined(MODULE_SSSP_STARTUP_1_3_GPIOEVENT_HOOK) */
        /* silently ignore any other GPIO events */
#endif /* defined(MODULE_SSSP_STARTUP_1_3_GPIOEVENT_HOOK) */
      }
    }
    // OS event
    else if (eventmask & _eventListenerOS.events) {
      eventflags = chEvtGetAndClearFlags(&_eventListenerOS);
      _aosMainUnexpectedEventError(eventmask, eventflags);
    }
    // unknown event
    else {
      _aosMainUnexpectedEventError(eventmask, 0);
    }
  }
  if (shutdown == AOS_SHUTDOWN_NONE) {
    aosDbgAssert(aos.sssp.stage == AOS_SSSP_STAGE_STARTUP_2_2);
  }
#else /* (AMIROOS_CFG_SSSP_MASTER == true) */
  if (shutdown == AOS_SHUTDOWN_NONE) {
    aosSsspProceed(NULL, 0, NULL);
    aosDbgAssert(aos.sssp.stage == AOS_SSSP_STAGE_STARTUP_2_2);
  }
#endif /* (AMIROOS_CFG_SSSP_MASTER == true) */

  // proceed to startup stage 3 (MSI is enabled), or to operation phase (no MSI)
  while ((shutdown == AOS_SHUTDOWN_NONE) && (aosSsspProceed(&_eventListenerGPIO, EVENTMASK_GPIO, &eventmask) != AOS_SUCCESS)) {
    /*
     * This code is executed if the received event was not about a deactivation of the snychronization signal.
     * The received event could be caused by any listener.
     */
    // GPIO event
    if (eventmask & _eventListenerGPIO.events) {
      eventflags = chEvtGetAndClearFlags(&_eventListenerGPIO);
      // PD event
      if (eventflags & moduleSsspEventflagPD()) {
        shutdown = AOS_SHUTDOWN_PASSIVE;
      } else {
#if defined(MODULE_SSSP_STARTUP_2_2_GPIOEVENT_HOOK)
        MODULE_SSSP_STARTUP_2_2_GPIOEVENT_HOOK(eventmask, eventflags);
#else /* defined(MODULE_SSSP_STARTUP_2_2_GPIOEVENT_HOOK) */
        /* silently ignore any other GPIO events */
#endif /* defined(MODULE_SSSP_STARTUP_2_2_GPIOEVENT_HOOK) */
      }
    }
    // OS event
    else if (eventmask & _eventListenerOS.events) {
      eventflags = chEvtGetAndClearFlags(&_eventListenerOS);
      _aosMainUnexpectedEventError(eventmask, eventflags);
    }
    // unknown event
    else {
      _aosMainUnexpectedEventError(eventmask, 0);
    }
  }
  if (shutdown == AOS_SHUTDOWN_NONE) {
#if (AMIROOS_CFG_SSSP_MSI == true)
    aosDbgAssert(aos.sssp.stage == AOS_SSSP_STAGE_STARTUP_3);
#else /* (AMIROOS_CFG_SSSP_MSI == true) */
    aosDbgAssert(aos.sssp.stage == AOS_SSSP_STAGE_OPERATION);
#endif /* (AMIROOS_CFG_SSSP_MSI == true) */
  }

  /*
   * There must be no delay at this point, thus no hook allowed.
   */

#if (AMIROOS_CFG_SSSP_MSI == true)
  {
    // setup BCB
#if defined(MODULE_SSSP_BCBDATA)
    MODULE_SSSP_BCBDATA bcbdata;
    moduleSsspBcbSetup(&bcbdata);
#else /* defined(MODULE_SSSP_BCBDATA) */
    moduleSsspBcbSetup(NULL);
#endif /* defined(MODULE_SSSP_BCBDATA) */

    // initialize temporary MSI data
    aos_ssspmsidata_t msidata;
    aosSsspMsiInit(&msidata, EVENTMASK_SSSPDELAY, EVENTMASK_SSSPTIMEOUT, &_eventListenerGPIO);

    // execute module stack initialization (MSI) but react to unrelated events as well
    while ((shutdown == AOS_SHUTDOWN_NONE) && (aos.sssp.stage != AOS_SSSP_STAGE_OPERATION)) {
      // execute MSI routine
#if defined(MODULE_SSSP_BCBDATA)
      aosSsspMsi(&msidata, &bcbdata, &eventmask);
#else /* defined(MODULE_SSSP_BCBDATA) */
      aosSsspMsi(&msidata, NULL, &eventmask);
#endif /* defined(MODULE_SSSP_BCBDATA) */
      // handle event (if any)
      if (eventmask) {
        // GPIO event
        if (eventmask & _eventListenerGPIO.events) {
          eventflags = chEvtGetAndClearFlags(&_eventListenerGPIO);
          // PD event
          if (eventflags & moduleSsspEventflagPD()) {
            shutdown = AOS_SHUTDOWN_PASSIVE;
          } else {
#if defined(MODULE_SSSP_STARTUP_3_GPIOEVENT_HOOK)
            MODULE_SSSP_STARTUP_3_GPIOEVENT_HOOK(eventmask, eventflags);
#else /* defined(MODULE_SSSP_STARTUP_3_GPIOEVENT_HOOK) */
            /* silently ignore any other GPIO events */
#endif /* defined(MODULE_SSSP_STARTUP_3_GPIOEVENT_HOOK) */
          }
        }
        // OS event
        else if (eventmask & _eventListenerOS.events) {
          eventflags = chEvtGetAndClearFlags(&_eventListenerOS);
          _aosMainUnexpectedEventError(eventmask, eventflags);
        }
        // unknown event
        else {
          _aosMainUnexpectedEventError(eventmask, 0);
        }
      }
    }

#if (HAL_USE_RTC == TRUE)

    /*
     * calendar synchronization
     *
     * Although calendar synchronization does not inherently rely on SSSP, this
     * implementation uses the moduleSsspBcbX() functions, defined by SSSP in
     * order to save redundant code. Furthermore, it is rather unlikely that
     * system wide synchronization of calendars is desired when SSSP is disabled
     * anyway.
     */
    if (shutdown == AOS_SHUTDOWN_NONE) {
#if (AMIROOS_CFG_SSSP_MASTER == true)
      // temporary variables
      struct tm t;
      uint8_t buffer[sizeof(uint64_t)];

      // get current date/time
      aosSysGetDateTime(&t);
      // encode & serialize
      _aosMainSerializeU64(buffer, _aosMainTM2U64(&t));
      // transmit
      aosDbgPrintf("transmitting current date/time...\t");
#if defined(MODULE_SSSP_BCBDATA)
      if (moduleSsspBcbTransmit(buffer, sizeof(uint64_t), &bcbdata) == AOS_SSSP_BCB_SUCCESS) {
#else /* defined(MODULE_SSSP_BCBDATA) */
      if (moduleSsspBcbTransmit(buffer, sizeof(uint64_t), NULL) == AOS_SSSP_BCB_SUCCESS) {
#endif /* defined(MODULE_SSSP_BCBDATA) */
        aosDbgPrintf("ok\n");
      } else {
        aosDbgPrintf("fail\n");
      }
#else /* (AMIROOS_CFG_SSSP_MASTER == true) */
      // temporary variables
      aos_timestamp_t start, now;
      aos_ssspbcbstatus_t bcbstatus;
      uint8_t buffer[sizeof(uint64_t)];
      struct tm t;

      // receive
      aosDbgPrintf("receiving current date/time...\t");
      aosSysGetUptime(&start);
      do {
#if defined(MODULE_SSSP_BCBDATA)
        bcbstatus = moduleSsspBcbReceive(buffer, sizeof(uint64_t), &bcbdata);
#else /* #if defined(MODULE_SSSP_BCBDATA) */
        bcbstatus = moduleSsspBcbReceive(buffer, sizeof(uint64_t), NULL);
#endif /* #if defined(MODULE_SSSP_BCBDATA) */
        aosSysGetUptime(&now);
      } while ((bcbstatus != AOS_SSSP_BCB_SUCCESS) && (aosTimestampDiff(start, now) < AOS_SSSP_TIMEOUT));
      // if something was received
      if (bcbstatus == AOS_SSSP_BCB_SUCCESS) {
        // deserialize & decode
        _aosMainU642TM(&t, _aosMainDeserializeU64(buffer));
        // set current date/time
        aosSysSetDateTime(&t);
        aosDbgPrintf("ok\n");
      } else {
        aosDbgPrintf("fail\n");
      }
#endif /* (AMIROOS_CFG_SSSP_MASTER == true) */
    }

#endif /* (HAL_USE_RTC == TRUE) */

  // release BCB
#if defined(MODULE_SSSP_BCBDATA)
    moduleSsspBcbRelease(&bcbdata);
#else /* defined(MODULE_SSSP_BCBDATA) */
    moduleSsspBcbRelease(NULL);
#endif /* defined(MODULE_SSSP_BCBDATA) */
  }

#endif /* (AMIROOS_CFG_SSSP_MSI == true) */

  /* ignore all SSSP signal events, except for PD and configured events */
  eventflags = moduleSsspEventflagS();
#if (AMIROOS_CFG_SSSP_MSI == true)
#if (AMIROOS_CFG_SSSP_STACK_START != true)
  eventflags |= moduleSsspEventflagDN();
#endif /* (AMIROOS_CFG_SSSP_STACK_START != true) */
#if (AMIROOS_CFG_SSSP_STACK_END != true)
  eventflags |= moduleSsspEventflagUP();
#endif /* (AMIROOS_CFG_SSSP_STACK_END != true) */
#endif /* (AMIROOS_CFG_SSSP_MSI == true) */
  eventflags &= ~((eventflags_t)AMIROOS_CFG_MAIN_LOOP_GPIOEVENT_FLAGSMASK);
  _eventListenerGPIO.wflags &= ~eventflags;

#if (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0)

  /* register to synchronization event */
  event_listener_t ssspsynclistener;
#if (AMIROOS_CFG_DBG == true)
  chEvtRegisterMask(&aos.sssp.syncsource, &ssspsynclistener, EVENTMASK_SSSPSYNC);
  // wait for first synchronization
  aosDbgPrintf("wait for first synchronization...");
  do {
    eventmask = chEvtWaitOne(ALL_EVENTS);
    // GPIO event
    if (eventmask & _eventListenerGPIO.events) {
      eventflags = chEvtGetAndClearFlags(&_eventListenerGPIO);
      // PD event
      if (eventflags & moduleSsspEventflagPD()) {
        shutdown = AOS_SHUTDOWN_PASSIVE;
      }
      /* silently ignore any other GPIO events */
    }
    // OS event
    else if (eventmask & _eventListenerOS.events) {
      eventflags = chEvtGetAndClearFlags(&_eventListenerOS);
      _aosMainUnexpectedEventError(eventmask, eventflags);
    }
    // SSSP synchronization event
    else if (eventmask & ssspsynclistener.events) {
      eventflags = chEvtGetAndClearFlags(&ssspsynclistener);
      // from now on, only critical events shall be received
      ssspsynclistener.wflags = AOS_SSSP_EVENTFLAGS_SYNC_CRITICALSKEW;
      aosDbgPrintf("\tok\n");
    }
    // unknown event
    else {
      _aosMainUnexpectedEventError(eventmask, 0);
    }
  } while (shutdown == AOS_SHUTDOWN_NONE && eventmask != ssspsynclistener.events);
#else /* (AMIROOS_CFG_DBG == true) */
  // critical events only
  chEvtRegisterMaskWithFlags(&aos.sssp.syncsource, &ssspsynclistener, EVENTMASK_SSSPSYNC, AOS_SSSP_EVENTFLAGS_SYNC_CRITICALSKEW);
#endif /* (AMIROOS_CFG_DBG == true) */
#endif /* (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) */

#if (AMIROOS_CFG_SSSP_MSI == true) || (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0)
  // print another newline to clearly separate debugging messages
  aosDbgPrintf("\n");
#endif /* (AMIROOS_CFG_SSSP_MSI == true) || (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) */

#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */

  /* completely start AMiRo-OS */
  if (shutdown == AOS_SHUTDOWN_NONE) {
    aosSysStart();
  }

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_5)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_5_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_5(AMIROOS_CFG_MAIN_INIT_HOOK_5_ARGS);
#else /* defined(AMIROOS_CFG_MAIN_INIT_HOOK_5_ARGS) */
  AMIROOS_CFG_MAIN_INIT_HOOK_5();
#endif /* defined(AMIROOS_CFG_MAIN_INIT_HOOK_5_ARGS) */
#endif /* defined(AMIROOS_CFG_MAIN_INIT_HOOK_5) */

  /*
   * ##########################################################################
   * # infinite loop                                                          #
   * ##########################################################################
   */

  // sleep until a shutdown event is received
  while (shutdown == AOS_SHUTDOWN_NONE) {
    // wait for an event
#if (AMIROOS_CFG_MAIN_LOOP_TIMEOUT != 0)
    eventmask = chEvtWaitOneTimeout(ALL_EVENTS, chTimeUS2I(AMIROOS_CFG_MAIN_LOOP_TIMEOUT));
#else /* (AMIROOS_CFG_MAIN_LOOP_TIMEOUT != 0) */
    eventmask = chEvtWaitOne(ALL_EVENTS);
#endif /* (AMIROOS_CFG_MAIN_LOOP_TIMEOUT != 0) */

#if defined(AMIROOS_CFG_MAIN_LOOP_HOOK_1)
#if defined(AMIROOS_CFG_MAIN_LOOP_HOOK_1_ARGS)
    AMIROOS_CFG_MAIN_LOOP_HOOK_1(AMIROOS_CFG_MAIN_LOOP_HOOK_1_ARGS);
#else /* defined(AMIROOS_CFG_MAIN_LOOP_HOOK_1_ARGS) */
    AMIROOS_CFG_MAIN_LOOP_HOOK_1();
#endif /* defined(AMIROOS_CFG_MAIN_LOOP_HOOK_1_ARGS) */
#endif /* defined(AMIROOS_CFG_MAIN_LOOP_HOOK_1) */

    switch (eventmask) {
      // if this was an GPIO event
      case EVENTMASK_GPIO:
        // evaluate flags
        eventflags = chEvtGetAndClearFlags(&_eventListenerGPIO);
#if (AMIROOS_CFG_SSSP_ENABLE == true)
        // PD event
        if (eventflags & moduleSsspEventflagPD()) {
          shutdown = AOS_SHUTDOWN_PASSIVE;
        }
        // all other events
#if defined(MODULE_MAIN_LOOP_GPIOEVENT)
        else {
          MODULE_MAIN_LOOP_GPIOEVENT(eventflags);
        }
#endif /* defined(MODULE_MAIN_LOOP_GPIOEVENT) */
#else /* (AMIROOS_CFG_SSSP_ENABLE == true) */
#if defined(MODULE_MAIN_LOOP_GPIOEVENT)
        MODULE_MAIN_LOOP_GPIOEVENT(eventflags);
#endif /* defined(MODULE_MAIN_LOOP_GPIOEVENT) */
#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */
        break;

      // if this was an OS event
      case EVENTMASK_OS:
        // evaluate flags
        eventflags = chEvtGetAndClearFlags(&_eventListenerOS);
        switch (eventflags) {
#if (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_NONE)
          case AOS_SYSTEM_EVENTFLAGS_SHUTDOWN:
            shutdown = AOS_SHUTDOWN_ACTIVE;
            break;
          case AOS_SYSTEM_EVENTFLAGS_SHUTDOWN_RESTART:
            shutdown = AOS_SHUTDOWN_RESTART;
            break;
#elif (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_AMiRoBLT)
          case AOS_SYSTEM_EVENTFLAGS_SHUTDOWN:
            _aosMainUnexpectedEventError(eventmask, eventflags);
            break;
          case AOS_SYSTEM_EVENTFLAGS_SHUTDOWN_RESTART:
            shutdown = AOS_SHUTDOWN_RESTART;
            break;
          case AOS_SYSTEM_EVENTFLAGS_SHUTDOWN_HIBERNATE:
            shutdown = AOS_SHUTDOWN_HIBERNATE;
            break;
          case AOS_SYSTEM_EVENTFLAGS_SHUTDOWN_DEEPSLEEP:
            shutdown = AOS_SHUTDOWN_DEEPSLEEP;
            break;
          case AOS_SYSTEM_EVENTFLAGS_SHUTDOWN_TRANSPORTATION:
            shutdown = AOS_SHUTDOWN_TRANSPORTATION;
            break;
#endif /* (AMIROOS_CFG_BOOTLOADER == X) */
          default:
            _aosMainUnexpectedEventError(eventmask, eventflags);
            break;
        }
        break;

#if (AMIROOS_CFG_SSSP_ENABLE == true) && (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0)
      // if this was an SSSP synchronization event
      case EVENTMASK_SSSPSYNC:
        // evaluate flags
        eventflags = chEvtGetAndClearFlags(&ssspsynclistener);
#if defined(MODULE_MAIN_LOOP_SSSPSYNCEVENT)
        MODULE_MAIN_LOOP_SSSPSYNCEVENT(eventflags);
#endif /* defined(MODULE_MAIN_LOOP_SSSPSYNCEVENT) */
        if (eventflags & AOS_SSSP_EVENTFLAGS_SYNC_CRITICALSKEW) {
#if (AMIROOS_CFG_SSSP_MASTER != true) && (AMIROOS_CFG_PROFILE == true)
          aosDbgPrintf("CTRL: critical SSSP synchronization (%f).\n", aosSsspGetSyncSkew());
#else /* (AMIROOS_CFG_SSSP_MASTER != true) && (AMIROOS_CFG_PROFILE == true) */
          aosDbgPrintf("CTRL: critical SSSP synchronization.\n");
#endif /* (AMIROOS_CFG_SSSP_MASTER != true) && (AMIROOS_CFG_PROFILE == true) */
        }
        break;
#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) && (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) */

#if (AMIROOS_CFG_MAIN_LOOP_TIMEOUT == 0)
      // if no event occurred (timeout)
      case 0:
        eventflags = 0;
        _aosMainUnexpectedEventError(eventmask, eventflags);
        break;
#endif /* (AMIROOS_CFG_MAIN_LOOP_TIMEOUT == 0) */

      // if this was any other event
      default:
#if defined(AMIROOS_CFG_MAIN_LOOP_EVENTHOOK)
#if defined(AMIROOS_CFG_MAIN_LOOP_EVENTHOOK_ARGS)
        AMIROOS_CFG_MAIN_LOOP_EVENTHOOK(eventmask, AMIROOS_CFG_MAIN_LOOP_EVENTHOOK_ARGS);
#else /* defined(AMIROOS_CFG_MAIN_LOOP_EVENTHOOK_ARGS) */
        AMIROOS_CFG_MAIN_LOOP_EVENTHOOK(eventmask);
#endif /* defined(AMIROOS_CFG_MAIN_LOOP_EVENTHOOK_ARGS) */
#else /* defined(AMIROOS_CFG_MAIN_LOOP_EVENT) */
        eventflags = 0;
        _aosMainUnexpectedEventError(eventmask, eventflags);
#endif /* defined(AMIROOS_CFG_MAIN_LOOP_EVENT) */
        break;
    }

#if (AMIROOS_CFG_SSSP_ENABLE == true)

    /*
     * Re-enable the S signal GPIO event.
     * Note that events for the optional signals UP and DN are not enabled as they are not utilized during SSSP shutdown phase.
     */
    _eventListenerGPIO.wflags |= moduleSsspEventflagS();

#if (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0)
    // unregister synchronization event
    chEvtUnregister(&aos.sssp.syncsource, &ssspsynclistener);
#endif /* (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) */

#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */

#if defined(AMIROOS_CFG_MAIN_LOOP_HOOK_2)
#if defined(AMIROOS_CFG_MAIN_LOOP_HOOK_2_ARGS)
    AMIROOS_CFG_MAIN_LOOP_HOOK_2(AMIROOS_CFG_MAIN_LOOP_HOOK_2_ARGS);
#else /* defined(AMIROOS_CFG_MAIN_LOOP_HOOK_2_ARGS) */
    AMIROOS_CFG_MAIN_LOOP_HOOK_2();
#endif /* defined(AMIROOS_CFG_MAIN_LOOP_HOOK_2_ARGS) */
#endif /* defined(AMIROOS_CFG_MAIN_LOOP_HOOK_2) */
  }

  /*
   * ##########################################################################
   * # system shutdown                                                        #
   * ##########################################################################
   */

  aosDbgAssert(shutdown != AOS_SHUTDOWN_NONE);

  // initialize/acknowledge shutdown
  aosSysShutdownInit(shutdown);
#if (AMIROOS_CFG_SSSP_ENABLE == true)
  aosDbgAssert(aos.sssp.stage == AOS_SSSP_STAGE_SHUTDOWN_1_2);
#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */

#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1)
#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1_ARGS)
  AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1_ARGS);
#else /* defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1_ARGS) */
  AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1();
#endif /* defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1_ARGS) */
#endif /* defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1) */

  // wait for all threads to terminate
  aosSysStop();

#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_2)
#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_2_ARGS)
  AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_2(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_2_ARGS);
#else /* defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_2_ARGS) */
  AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_2();
#endif /* defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_2_ARGS) */
#endif /* defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_2) */

  // deinitialize system
  aosSysDeinit();

#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_3)
#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_3_ARGS)
  AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_3(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_3_ARGS);
#else /* defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_3_ARGS) */
  AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_3();
#endif /* defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_3_ARGS) */
#endif /* defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_3) */

  /* stop all periphery communication interfaces */
#if defined(MODULE_SHUTDOWN_PERIPHERY_INTERFACES)
  MODULE_SHUTDOWN_PERIPHERY_INTERFACES();
#endif /* defined(MODULE_SHUTDOWN_PERIPHERY_INTERFACES) */

#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_4)
#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_4_ARGS)
  AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_4(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_4_ARGS);
#else /* defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_4_ARGS) */
  AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_4();
#endif /* defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_4_ARGS) */
#endif /* defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_4) */

#if (AMIROOS_CFG_SSSP_ENABLE == true)

  // proceed to SSSP shutdown stage 1.3
  aosSsspProceed(NULL, 0, NULL);
  aosDbgAssert(aos.sssp.stage == AOS_SSSP_STAGE_SHUTDOWN_1_3);

#if (AMIROOS_CFG_SSSP_SHUTDOWN == true)

  /* AMiRo-OS performs SSSP shutdown phase
   * NOTE:
   * The initiating module could deactivate PD at this point to indicate a
   * restart rather than a shutdown request. AMiRo-OS does not support restart
   * though, thus the signal is not changed.
   */

  // proceed to SSSP shutdown stage 2.1
  while (aosSsspProceed(&_eventListenerGPIO, EVENTMASK_GPIO, &eventmask) != AOS_SUCCESS) {
    /*
     * This code is executed if the received event was not about a deactivation
     * of the synchronization signal. The received event could be caused by any
     * listener.
     */
    // GPIO event
    if (eventmask & _eventListenerGPIO.events) {
      eventflags = chEvtGetAndClearFlags(&_eventListenerGPIO);
#if defined(MODULE_SSSP_SHUTDOWN_1_3_GPIOEVENT_HOOK)
        MODULE_SSSP_SHUTDOWN_1_3_GPIOEVENT_HOOK(eventmask, eventflags);
#else /* defined(MODULE_SSSP_SHUTDOWN_1_3_GPIOEVENT_HOOK) */
        /* silently ignore any other GPIO events */
#endif /* defined(MODULE_SSSP_SHUTDOWN_1_3_GPIOEVENT_HOOK) */
    }
    // OS event
    else if (eventmask & _eventListenerOS.events) {
      eventflags = chEvtGetAndClearFlags(&_eventListenerOS);
      _aosMainUnexpectedEventError(eventmask, eventflags);
    }
    // unknown event
    else {
      _aosMainUnexpectedEventError(eventmask, 0);
    }
  }
  aosDbgAssert(aos.sssp.stage == AOS_SSSP_STAGE_SHUTDOWN_2_1);

  // proceed to SSSP shutdown stage 2.2
  aosSsspProceed(NULL, 0, &eventmask);
  aosDbgAssert(aos.sssp.stage == AOS_SSSP_STAGE_SHUTDOWN_2_2);
  if (shutdown == AOS_SHUTDOWN_PASSIVE) {
    aosDbgPrintf("%s request received\n", eventmask ? "shutdown" : "restart");
  }
  /* NOTE:
   * Actually the call of aosSsspProceed() returned an indicator, whether a
   * shutdown or restart has been initiated. Since AMiRo-OS does not support
   * restart, this return value is ignored and shutdown is assumed.
   */

  // the initiating module broadcasts the shutdown identifier
  if (shutdown != AOS_SHUTDOWN_PASSIVE) {
    // since AMiRo-OS does not support multiple ways to shutdown or restart, the special identifier 0 is broadcasted.
    aosSsspShutdownBroadcastIdentifier(0);
  }
  // passive modules receive the broadcasted identifier
  else {
    unsigned int identifier = 0;

    // receive the identifier, which specifies the type of shutdown/restart
    while ((eventmask = aosSsspShutdownWaitForIdentifierPulse(&_eventListenerGPIO, moduleSsspEventflagS(), EVENTMASK_SSSPDELAY, &identifier)) != EVENTMASK_SSSPDELAY) {
      // GPIO event
      if (eventmask & _eventListenerGPIO.events) {
        eventflags = chEvtGetAndClearFlags(&_eventListenerGPIO);
#if defined(MODULE_SSSP_SHUTDOWN_2_2_GPIOEVENT_HOOK)
        MODULE_SSSP_SHUTDOWN_2_2_GPIOEVENT_HOOK(eventmask, eventflags);
#else /* defined(MODULE_SSSP_SHUTDOWN_2_2_GPIOEVENT_HOOK) */
        /* silently ignore any other GPIO events */
#endif /* defined(MODULE_SSSP_SHUTDOWN_2_2_GPIOEVENT_HOOK) */
        /* silently ignore any GPIO events */
      }
      // OS event
      else if (eventmask & _eventListenerOS.events) {
        eventflags = chEvtGetAndClearFlags(&_eventListenerOS);
        _aosMainUnexpectedEventError(eventmask, eventflags);
      }
      // unknown event
      else {
        _aosMainUnexpectedEventError(eventmask, 0);
      }
    }

    // Since AMiRo-OS does not support multiple ways to shutdown or restart, the identifier is ignored.
  }

  // proceed to SSSP shutdown stage 2.3
  aosSsspProceed(NULL, 0, NULL);
  aosDbgAssert(aos.sssp.stage == AOS_SSSP_STAGE_SHUTDOWN_2_3);

  /*
   * Since AMiRo-OS does only support one basic shutdown and no restart at all,
   * the final shutdown is kept very simple.
   */

  // disable interrupts
  irqDeinit();
  chSysDisable();

  chThdExit(MSG_OK);
  return MSG_OK;

#else /* (AMIROOS_CFG_SSSP_SHUTDOWN == true) */

  /* hand over to the bootloader to perform SSSP shutdown phase */
  aosSysShutdownToBootloader(shutdown);

  aosDbgAssertMsg(false, "AMiRo-OS must not proceed to this point!");
  chThdExit(MSG_RESET);
  return MSG_RESET;

#endif /* (AMIROOS_CFG_SSSP_SHUTDOWN == true) */

#else /* (AMIROOS_CFG_SSSP_ENABLE == true) */

  /*
   * Depending on the configured bootloader, trivial shutdown is either executed
   * by AMiRo-OS or by that bootloader.
   */
#if (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_NONE)

  /* AMiRo-OS performs trivial shutdown */

  // disable interrupts
  irqDeinit();
  chSysDisable();

  // if a restart was requested
  if (shutdown == AOS_SHUTDOWN_RESTART) {
    // reset the MCU
    __NVIC_SystemReset();
  }

  chThdExit(MSG_OK);
  return MSG_OK;

#elif (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_AMiRoBLT)

  /* AMiRo-BLT performns shutdown */
  aosSysShutdownToBootloader(shutdown);

  aosDbgAssertMsg(false, "AMiRo-OS must not proceed to this point!");
  chThdExit(MSG_RESET);
  return MSG_RESET;

#endif /* (AMIROOS_CFG_BOOTLOADER == X) */
#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
