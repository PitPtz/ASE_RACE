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
 * @file    aos_system.h
 * @brief   System macros and structures.
 *
 * @addtogroup core_system
 * @{
 */

#ifndef AMIROOS_SYSTEM_H
#define AMIROOS_SYSTEM_H

#include <aosconf.h>
#include "aos_iostream.h"
#include "aos_time.h"
#include "aos_shell.h"
#include "aos_sssp.h"
#include "bootloader/aos_bootloader.h"
#include <chprintf.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief   Resolution of the system time measurement.
 */
#define AOS_SYSTEM_TIME_RESOLUTION              ((MICROSECONDS_PER_SECOND + CH_CFG_ST_FREQUENCY - 1) / CH_CFG_ST_FREQUENCY)

/**
 * @brief   System event flag mask for shutdown related events.
 */
#define AOS_SYSTEM_EVENTFLAGS_SHUTDOWN_MASK     ((eventflags_t)0xFF)

/**
 * @brief   System event flag which is emitted when an unspecified shutdown was
 *          initiated.
 */
#define AOS_SYSTEM_EVENTFLAGS_SHUTDOWN          ((eventflags_t)(1 << 0))

/**
 * @brief   System event flag which is emitted when a system restart was
 *          initiated.
 */
#define AOS_SYSTEM_EVENTFLAGS_SHUTDOWN_RESTART  ((eventflags_t)(1 << 1))

#if (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_AMiRoBLT) || defined(__DOXYGEN__)

/**
 * @brief   System event flag which is emitted when a shutdown to transportation
 *          mode was initiated.
 */
#define AOS_SYSTEM_EVENTFLAGS_SHUTDOWN_TRANSPORTATION ((eventflags_t)(1 << 2))

/**
 * @brief   System event flag which is emitted when a shutdown to deepsleep mode
 *          was initiated.
 */
#define AOS_SYSTEM_EVENTFLAGS_SHUTDOWN_DEEPSLEEP  ((eventflags_t)(1 << 3))

/**
 * @brief   System event flag which is emitted when a shutdown to hibernate mode
 *          was initiated.
 */
#define AOS_SYSTEM_EVENTFLAGS_SHUTDOWN_HIBERNATE  ((eventflags_t)(1 << 4))

#endif /* (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_AMiRoBLT) */

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

/**
 * @brief   Enumerator to identify shutdown types.
 */
typedef enum aos_shutdown {
  AOS_SHUTDOWN_NONE           = 0x00,   /**< Default value if no shutdown action was initiated */
#if (AMIROOS_CFG_SSSP_ENABLE == true) || defined(__DOXYGEN__)
  AOS_SHUTDOWN_PASSIVE        = 0x10,   /**< Passive shutdown (initiated by another module). */
#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */
#if (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_NONE) || defined (__DOXYGEN__)
  AOS_SHUTDOWN_ACTIVE         = 0x20,   /**< Active shutdown. */
#endif /* (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_NONE) */
  AOS_SHUTDOWN_RESTART        = 0x21,   /**< Active system restart request. */
#if (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_AMiRoBLT) || defined (__DOXYGEN__)
  AOS_SHUTDOWN_HIBERNATE      = 0x22,   /**< Active shutdown to hibernate mode. */
  AOS_SHUTDOWN_DEEPSLEEP      = 0x23,   /**< Active shutdown to deepsleep mode. */
  AOS_SHUTDOWN_TRANSPORTATION = 0x24,   /**< Active shutdown to transportation mode. */
#endif /* (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_AMiRoBLT) */
} aos_shutdown_t;

/**
 * @brief   AMiRo-OS base system structure.
 */
typedef struct aos_system {

  /**
   * @brief   System I/O stream.
   */
  AosOStream iostream;

  /**
   * @brief   Event structure.
   */
  struct {

    /**
     * @brief   GPIO event source.
     */
    event_source_t gpio;

    /**
     * @brief   OS event source.
     */
    event_source_t os;
  } events;

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)

  /**
   * @brief   System shell object.
   */
  aos_shell_t shell;

#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */

#if (AMIROOS_CFG_SSSP_ENABLE == true) || defined(__DOXYGEN__)

  /**
   * @brief   SSSP relevant data.
   */
  aos_ssspdata_t sssp;

#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */

} aos_system_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/**
 * @brief   Printf function that uses the module I/O stream.
 *
 * @param[in] fmt   Formatted string to print.
 *
 * @return  Number of printed characters.
 */
#define aosprintf_module(fmt, ...)              chprintf((BaseSequentialStream*)&aos.iostream, fmt, ##__VA_ARGS__)

/**
 * @brief   Flush all output to the module I/O stream.
 */
#define aosflush_module()                       aosOStreamFlush(&aos.iostream)

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)

/**
 * @brief   Printf function that uses the system shell stream.
 *
 * @param[in] fmt   Formatted string to print.
 *
 * @return  Number of printed characters.
 */
#define aosprintf_shell(fmt, ...)               chprintf((BaseSequentialStream*)&aos.shell.stream, fmt, ##__VA_ARGS__)

/**
 * @brief   Flush all output to the system shell stream.
 */
#define aosflush_shell()                        aosShellStreamFlush(aosShellGetStream(&aos.shell))

#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */

/**
 * @brief   Printf function that uses the default system stream.
 * @details If a system shell is available and running, output is printed to its
 *          stream. Otherwise, output is printed to the module I/O stream.
 *
 * @param[in] fmt   Formatted string to print.
 *
 * @return  Number of printed characters.
 */
#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
#define aosprintf(fmt, ...) (                                                   \
  (aos.shell.state == AOS_SHELL_STATE_RUNNING) ? aosprintf_shell(fmt, ##__VA_ARGS__) : aosprintf_module(fmt, ##__VA_ARGS__) \
)
#else
#define aosprintf(fmt, ...)                     aosprintf_module(fmt, ##__VA_ARGS__)
#endif

/**
 * @brief   Flush all output to the default system system stream.
 * @details If a system shell is available and running, its stream is flushed.
 *          Otherwise the module I/O stream is flushed.
 */
#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
#define aosflush() (                                                            \
  (aos.shell.state == AOS_SHELL_STATE_RUNNING) ? aosflush_shell() : aosflush_module() \
)
#else
#define aosflush()                              aosflush_module()
#endif

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

/**
 * @brief   Global system object.
 */
extern aos_system_t aos;

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void aosSysInit(void);
  void aosSysStart(void);
  void aosSysStartUptimeS(void);
  void aosSysGetUptimeX(aos_timestamp_t* ut);
#if (HAL_USE_RTC == TRUE) || defined(__DOXYGEN__)
  void aosSysGetDateTime(struct tm* dt);
  void aosSysSetDateTime(struct tm* dt);
#endif /* (HAL_USE_RTC == TRUE) */
  void aosSysShutdownInit(aos_shutdown_t shutdown);
  void aosSysStop(void);
  void aosSysDeinit(void);
#if ((AMIROOS_CFG_SSSP_ENABLE == true) &&                                       \
     (AMIROOS_CFG_SSSP_SHUTDOWN != true)) ||                                    \
    ((AMIROOS_CFG_SSSP_ENABLE != true) &&                                       \
     (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_AMiRoBLT)) ||                    \
    defined(__DOXYGEN__)
  void aosSysShutdownToBootloader(aos_shutdown_t shutdown);
#endif /* ((AMIROOS_CFG_SSSP_ENABLE == true) &&
           (AMIROOS_CFG_SSSP_SHUTDOWN != true)) ||
          ((AMIROOS_CFG_SSSP_ENABLE != true) &&
           (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_AMiRoBLT)) */
  palcallback_t aosSysGetStdGpioCallback(void);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/**
 * @brief   Retrieves the system uptime.
 *
 * @param[out] ut   Pointer to the system uptime.
 */
static inline void aosSysGetUptime(aos_timestamp_t* ut)
{
  chSysLock();
  aosSysGetUptimeX(ut);
  chSysUnlock();
}

#endif /* AMIROOS_SYSTEM_H */

/** @} */
