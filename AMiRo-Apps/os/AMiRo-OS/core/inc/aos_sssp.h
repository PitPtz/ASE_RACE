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
 * @file    aos_sssp.h
 * @brief   SSSP related macros, structures and functions.
 *
 * @addtogroup core_sssp
 * @{
 */

#ifndef AMIROOS_SSSP_H
#define AMIROOS_SSSP_H

#include <aosconf.h>

#if (AMIROOS_CFG_SSSP_ENABLE == true) || defined(__DOXYGEN__)

#include <stdint.h>
#include <hal.h>
#include <amiro-lld.h>
#include "aos_types.h"
#include "aos_time.h"

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief   Major version of the implemented SSSP.
 */
#define AOS_SSSP_VERSION_MAJOR                  1

/**
 * @brief   Minor version of the implemented SSSP.
 */
#define AOS_SSSP_VERSION_MINOR                  4

/**
 * @brief   Time of delays in microseconds.
 */
#define AOS_SSSP_DELAY                          AMIROOS_CFG_SSSP_SIGNALDELAY

/**
 * @brief   Timeout delay in microseconds according to SSSP.
 * @details SSSP 1.x defines timeouts to be ten times longer than the signal
 *          delay time.
 */
#define AOS_SSSP_TIMEOUT                        (10 * AOS_SSSP_DELAY)

/**
 * @brief   Value to indicate an invalid module ID.
 */
#define AOS_SSSP_MODULEID_INVALID               ((aos_ssspmoduleid_t)0)

/**
 * @brief   Value for broadcasting, adressing all modules.
 * @note    This value is identical to @p AOS_SSSP_MODULEID_INVALID since in the
 *          case that no module IDs were applied, addressing of specific modules
 *          is not possible, thus every communication would be a broadcast.
 */
#define AOS_SSSP_MODULEID_BROADCAST             ((aos_ssspmoduleid_t)0)

#if (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) || defined(__DOXYGEN__)

/**
 * @brief   Synchronization event flag, indicating that the master just triggered a synchronization.
 */
#define AOS_SSSP_EVENTFLAGS_SYNC_MASTER         ((eventflags_t)1 << 0)

/**
 * @brief   Synchronization event flag, indicating that the slave just registerd a synchronization.
 */
#define AOS_SSSP_EVENTFLAGS_SYNC_SLAVE          ((eventflags_t)1 << 1)

/**
 * @brief   Synchronization event flag, indicating that time critically misaligned.
 */
#define AOS_SSSP_EVENTFLAGS_SYNC_CRITICALSKEW   ((eventflags_t)1 << 2)

#endif /* (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) */

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

#if (!(AMIROOS_CFG_SSSP_MODULEIDWIDTH == 8) &&                                  \
   !(AMIROOS_CFG_SSSP_MODULEIDWIDTH == 16)  &&                                  \
   !(AMIROOS_CFG_SSSP_MODULEIDWIDTH == 32)  &&                                  \
   !(AMIROOS_CFG_SSSP_MODULEIDWIDTH == 64))
# error "AMIROOS_CFG_SSSP_MODULEIDWIDTH set to an invalid value."
#endif

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

/**
 * @brief   Enumeration of the several stages of SSSP.
 */
typedef enum {
  AOS_SSSP_STAGE_UNDEFINED                    = 0x00000000, /**< Identifier of yet undefined stage variable.                                          */
  AOS_SSSP_STAGE_STARTUP                      = 0x10000000, /**< Identifier of SSSP startup phase.                                                    */
  AOS_SSSP_STAGE_STARTUP_1                    = 0x11000000, /**< Identifier of SSSP startup phase stage 1.                                            */
  AOS_SSSP_STAGE_STARTUP_1_1                  = 0x11100000, /**< Identifier of SSSP startup phase stage 1-1.                                          */
  AOS_SSSP_STAGE_STARTUP_1_2                  = 0x11200000, /**< Identifier of SSSP startup phase stage 1-2.                                          */
  AOS_SSSP_STAGE_STARTUP_1_3                  = 0x11300000, /**< Identifier of SSSP startup phase stage 1-3.                                          */
  AOS_SSSP_STAGE_STARTUP_2                    = 0x12000000, /**< Identifier of SSSP startup phase stage 2.                                            */
  AOS_SSSP_STAGE_STARTUP_2_1                  = 0x12100000, /**< Identifier of SSSP startup phase stage 2-1.                                          */
  AOS_SSSP_STAGE_STARTUP_2_2                  = 0x12200000, /**< Identifier of SSSP startup phase stage 2-2.                                          */
  AOS_SSSP_STAGE_STARTUP_3                    = 0x13000000, /**< Identifier of SSSP startup phase stage 3.                                            */
  AOS_SSSP_STAGE_STARTUP_3_1                  = 0x13100000, /**< Identifier of SSSP startup phase stage 3-1.                                          */
  AOS_SSSP_STAGE_STARTUP_3_1_BROADCASTINIT    = 0x13110001, /**< Identifier of SSSP startup phase stage 3-1 broadcast initialization (master only).   */
  AOS_SSSP_STAGE_STARTUP_3_1_WAITFORINIT      = 0x13110002, /**< Identifier of SSSP startup phase stage 3-1 waiting for broadcast (non-master only).  */
  AOS_SSSP_STAGE_STARTUP_3_2                  = 0x13200000, /**< Identifier of SSSP startup phase stage 3-2.                                          */
  AOS_SSSP_STAGE_STARTUP_3_3                  = 0x13300000, /**< Identifier of SSSP startup phase stage 3-3.                                          */
  AOS_SSSP_STAGE_STARTUP_3_3_WAIT4EVENT       = 0x13310000, /**< Identifier of SSSP startup phase stage 3-3 waiting for an event to occurr.           */
  AOS_SSSP_STAGE_STARTUP_3_3_BROADCASTID      = 0x13320000, /**< Identifier of SSSP startup phase stage 3-3 broadcasting identifier.                  */
  AOS_SSSP_STAGE_STARTUP_3_4                  = 0x13400000, /**< Identifier of SSSP startup phase stage 3-4.                                          */
  AOS_SSSP_STAGE_STARTUP_3_4_COMPLETION       = 0x13410000, /**< Identifier of SSSP startup phase stage 3-4 completion.                               */
  AOS_SSSP_STAGE_STARTUP_3_4_ABORTION         = 0x13420000, /**< Identifier of SSSP startup phase stage 3-4 abortion.                                 */
  AOS_SSSP_STAGE_STARTUP_3_4_ABORTION_INIT    = 0x13421000, /**< Identifier of SSSP startup phase stage 3-4 abortion initialization.                  */
  AOS_SSSP_STAGE_STARTUP_3_4_ABORTION_SYNC    = 0x13422000, /**< Identifier of SSSP startup phase stage 3-4 abortion synchronization.                 */
  AOS_SSSP_STAGE_OPERATION                    = 0x20000000, /**< Identifier of SSSP operation phase.                                                  */
  AOS_SSSP_STAGE_SHUTDOWN                     = 0x30000000, /**< Identifier of SSSP shutdown phase.                                                   */
  AOS_SSSP_STAGE_SHUTDOWN_1                   = 0x31000000, /**< Identifier of SSSP shutdown phase stage 1.                                           */
  AOS_SSSP_STAGE_SHUTDOWN_1_1                 = 0x31100000, /**< Identifier of SSSP shutdown phase stage 1-1.                                         */
  AOS_SSSP_STAGE_SHUTDOWN_1_2                 = 0x31200000, /**< Identifier of SSSP shutdown phase stage 1-2.                                         */
  AOS_SSSP_STAGE_SHUTDOWN_1_3                 = 0x31300000, /**< Identifier of SSSP shutdown phase stage 1-3.                                         */
  AOS_SSSP_STAGE_SHUTDOWN_2                   = 0x32000000, /**< Identifier of SSSP shutdown phase stage 2.                                           */
  AOS_SSSP_STAGE_SHUTDOWN_2_1                 = 0x32100000, /**< Identifier of SSSP shutdown phase stage 2-1.                                         */
  AOS_SSSP_STAGE_SHUTDOWN_2_2                 = 0x32200000, /**< Identifier of SSSP shutdown phase stage 2-2.                                         */
  AOS_SSSP_STAGE_SHUTDOWN_2_3                 = 0x32300000, /**< Identifier of SSSP shutdown phase stage 2-3.                                         */
} aos_ssspstage_t;

/**
 * @brief   Type to represent module IDs.
 */
#if (AMIROOS_CFG_SSSP_MODULEIDWIDTH == 8)
typedef uint8_t aos_ssspmoduleid_t;
#elif (AMIROOS_CFG_SSSP_MODULEIDWIDTH == 16) || defined(__DOXYGEN__)
typedef uint16_t aos_ssspmoduleid_t;
#elif (AMIROOS_CFG_SSSP_MODULEIDWIDTH == 32)
typedef uint32_t aos_ssspmoduleid_t;
#elif (AMIROOS_CFG_SSSP_MODULEIDWIDTH == 64)
typedef uint64_t aos_ssspmoduleid_t;
#endif

/**
 * @brief   Data structure holding module-wide SSSP information.
 */
typedef struct {
  /**
   * @brief   Current execution stage of SSSP.
   */
  aos_ssspstage_t stage;

  /**
   * @brief   Module identifier.
   * @details A value of @p AOS_SSSP_MODULEID_INVALID indicates an uninitialized
   *          ID.
   */
  aos_ssspmoduleid_t moduleId;

#if (AMIROOS_CFG_SSSP_MSI == true) || defined(__DOXYGEN__)
  /**
   * @brief   Number of SSSP modules.
   * @details A value of 0 indicates an uninitialize or invalid count.
   *          If moduleId is invalid, but this variable is greater than zero
   *          (MSI failed), the sccumulated number of modules is a lower bound.
   */
  aos_ssspmoduleid_t modulecount;
#endif /* (AMIROOS_CFG_SSSP_MSI == true) */

#if (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) || defined(__DOXYGEN__)
  /**
   * @brief   Event source to notify about synchronization events.
   */
  event_source_t syncsource;
#endif /* (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) */

} aos_ssspdata_t;

#if (AMIROOS_CFG_SSSP_MSI == true) || defined(__DOXYGEN__)

/**
 * @brief   Type to qualify attempts to transmitt/receive mesages via BCB.
 */
typedef enum {
  AOS_SSSP_BCB_SUCCESS,     /**< BCB access was sucessfull. */
  AOS_SSSP_BCB_ERROR,       /**< BCB access failed or no message was available. */
  AOS_SSSP_BCB_INVALIDMSG,  /**< Invalid BCB message detected. */
} aos_ssspbcbstatus_t;

/**
 * @brief   Data structure to be transmitted via BCB during module stack
 *          initialization (MSI).
 */
typedef struct __PACKED {

  /**
   * @brief   Type of the message.
   */
  uint8_t type;

  /**
   * @brief   Message payload.
   * @details Payload depends on message type.
   */
  union __PACKED {
    /**
     * @brief   Transmitted ID.
     * @details This field is only used for @p SSSP_MSI_ID type messages.
     */
    aos_ssspmoduleid_t id;
  } payload;

} aos_ssspbcbmessage_t;

/**
 * @brief   Data structure required for the module stack initialization (MSI) phase.
 */
typedef struct {

  /**
   * @brief   Timout related data.
   */
  struct {
    /**
     * @brief   Timer to trigger potential timeouts.
     * @details This timer must not be an AMiRo-OS timer, since delays occurr
     *          before system is initialized.
     */
    virtual_timer_t timer;

    /**
     * @brief   Event source to be triggered on timeout.
     */
    event_source_t eventsource;

    /**
     * @brief   Event listener to inform the MSI thread about timeout events.
     */
    event_listener_t eventlistener;
  } timeout;

  /**
   * @brief   Signal related data.
   */
  struct {
    /**
     * @brief   Pointer to an existing event listener, which informs about GPIO
     *          signal events.
     */
    event_listener_t* eventlistener;
  } signals;

  /**
   * @brief   BCB related data.
   */
  struct {
    /**
     * @brief   Message object.
     */
    aos_ssspbcbmessage_t message;

    /**
     * @brief   Buffer for (de)serialization BCB messages.
     */
    uint8_t buffer[sizeof(aos_ssspbcbmessage_t)];

    /**
     * @brief   Return status of last BCB access.
     */
    aos_ssspbcbstatus_t status;

    /**
     * @brief   The most recently received identifier.
     */
    aos_ssspmoduleid_t lastid;
  } bcb;

} aos_ssspmsidata_t;

#endif /* (AMIROOS_CFG_SSSP_MSI == true) */

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

  /*
   * module specific interface functions
   */
#if !defined(moduleSsspSignalPD) || defined(__DOXYGEN__)
  /**
   * @brief   Retrieve the control GPIO object for the PD signal.
   * @return  Pounter to the control GPIO object.
   */
  apalControlGpio_t* moduleSsspSignalPD(void);
#endif /* !defined(moduleSsspSignalPD) */
#if !defined(moduleSsspEventflagPD) || defined(__DOXYGEN__)
  /**
   * @brief   Retrieve the GPIO event flag set by a PD signal edge.
   * @return  Eventflag set on PD events.
   */
  eventflags_t moduleSsspEventflagPD(void);
#endif /* !defined(moduleSsspEventflagPD) */

#if !defined(moduleSsspSignalS) || defined(__DOXYGEN__)
  /**
   * @brief   Retrieve the control GPIO object for the S signal.
   * @return  Pounter to the control GPIO object.
   */
  apalControlGpio_t* moduleSsspSignalS(void);
#endif /* !defined(moduleSsspSignalS) */
#if !defined(moduleSsspEventflagS) || defined(__DOXYGEN__)
  /**
   * @brief   Retrieve the GPIO event flag set by a S signal edge.
   * @return  Eventflag set on S events.
   */
  eventflags_t moduleSsspEventflagS(void);
#endif /* !defined(moduleSsspEventflagS) */

#if (AMIROOS_CFG_SSSP_MSI == true) || defined(__DOXYGEN__)

#if (AMIROOS_CFG_SSSP_STACK_START != true) || defined(__DOXYGEN__)
#if !defined(moduleSsspSignalDN) || defined(__DOXYGEN__)
  /**
   * @brief   Retrieve the control GPIO object for the DN signal.
   * @return  Pounter to the control GPIO object.
   */
  apalControlGpio_t* moduleSsspSignalDN(void);
#endif /* !defined(moduleSsspSignalDN) */
#if !defined(moduleSsspEventflagDN) || defined(__DOXYGEN__)
  /**
   * @brief   Retrieve the GPIO event flag set by a DN signal edge.
   * @return  Eventflag set on DN events.
   */
  eventflags_t moduleSsspEventflagDN(void);
#endif /* !defined(moduleSsspEventflagDN) */
#endif /* (AMIROOS_CFG_SSSP_STACK_START != true) */

#if (AMIROOS_CFG_SSSP_STACK_END != true) || defined(__DOXYGEN__)
#if !defined(moduleSsspSignalUP) || defined(__DOXYGEN__)
  /**
   * @brief   Retrieve the control GPIO object for the UP signal.
   * @return  Pounter to the control GPIO object.
   */
  apalControlGpio_t* moduleSsspSignalUP(void);
#endif /* !defined(moduleSsspSignalUP) */
#if !defined(moduleSsspEventflagUP) || defined(__DOXYGEN__)
  /**
   * @brief   Retrieve the GPIO event flag set by a UP signal edge.
   * @return  Eventflag set on UP events.
   */
  eventflags_t moduleSsspEventflagUP(void);
#endif /* !defined(moduleSsspEventflagUP) */
#endif /* (AMIROOS_CFG_SSSP_STACK_END != true) */

#if !defined(moduleSsspBcbSetup) || defined(__DOXYGEN__)
  /**
   * @brief   Setup the BCB.
   *
   * @param[in] data    Optional BCB data.
   */
  void moduleSsspBcbSetup(void* data);
#endif /* !defined(moduleSsspBcbSetup) */

#if !defined(moduleSsspBcbRelease) || defined(__DOXYGEN__)
  /**
   * @brief   Release the BCB.
   *
   * @param[in] data    Optional BCB data.
   */
  void moduleSsspBcbRelease(void* data);
#endif /* !defined(moduleSsspBcbRelease) */

#if !defined(moduleSsspBcbTransmit) || defined(__DOXYGEN__)
  /**
   * @brief   Transmit data via BCB.
   *
   * @param[in] buffer  Pointer to a buffer, that holds the data to be sent.
   * @param[in] length  Length of the buffer in bytes.
   * @param[in] data    Optional BCB data.
   *
   * @return  Status value, indicating whether the operation was successful.
   */
  aos_ssspbcbstatus_t moduleSsspBcbTransmit(const uint8_t* buffer, size_t length, void* data);
#endif /* !defined(moduleSsspBcbTransmit) */

#if !defined(moduleSsspBcbReceive) || defined(__DOXYGEN__)
  /**
   * @brief   Receive a message from BCB.
   * @note    If a message is received, which is not defined by SSSP, the data
   *          may be irreversibly discarded by this function.
   *
   * @param[out] buffer   Pointer to a buffer to store the received data to.
   * @param[in]  length   Number of expected bytes to receive.
   *                      Obviously, the buffer must be large enough.
   * @param[in]  data     Optional BCB data.
   *
   * @return  Status value, indicating whether the operation was successful.
   */
  aos_ssspbcbstatus_t moduleSsspBcbReceive(uint8_t* buffer, size_t length, void* data);
#endif /* !defined(moduleSsspBcbReceive) */

#endif /* (AMIROOS_CFG_SSSP_MSI == true) */

  /*
   * API functions
   */
  void aosSsspInit(aos_timestamp_t* system_uptime);
  aos_status_t aosSsspProceed(event_listener_t* listener, eventmask_t mask, eventmask_t* received);
#if (AMIROOS_CFG_SSSP_MSI == true) || defined(__DOXYGEN__)
  void aosSsspMsiInit(aos_ssspmsidata_t* msidata, eventmask_t delayMask, eventmask_t timeoutMask, event_listener_t* gpioListener);
  void aosSsspMsi(aos_ssspmsidata_t* msidata, void* bcbdata, eventmask_t* received);
#endif /* (AMIROOS_CFG_SSSP_MSI == true) */
#if ((AMIROOS_CFG_SSSP_MASTER != true) && (AMIROOS_CFG_PROFILE == true)) || defined(__DOXYGEN__)
  float aosSsspGetSyncSkew(void);
#endif /* (AMIROOS_CFG_SSSP_MASTER != true) && (AMIROOS_CFG_PROFILE == true) */
  void aosSsspShutdownInit(bool active);
#if (AMIROOS_CFG_SSSP_SHUTDOWN == true) || defined(__DOXYGEN__)
  void aosSsspShutdownBroadcastIdentifier(unsigned int identifier);
  eventmask_t aosSsspShutdownWaitForIdentifierPulse(event_listener_t* gpiolistener, eventflags_t sflags, eventmask_t timermask, unsigned int* identifier);
#endif /* (AMIROOS_CFG_SSSP_SHUTDOWN == true) */

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */

#endif /* AMIROOS_SSSP_H */

/** @} */
