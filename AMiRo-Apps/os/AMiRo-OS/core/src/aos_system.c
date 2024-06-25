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
 * @file    aos_system.c
 * @brief   System code.
 * @details Contains system initialization and shutdown routines
 *          and system shell commands.
 */

#include <amiroos.h>

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#if (AMIROOS_CFG_TESTS_ENABLE == true)
#include <ch_test.h>
#if (AMIROOS_CFG_TESTS_ENABLE_KERNEL == true)
#include <rt_test_root.h>
#include <oslib_test_root.h>
#endif /* (AMIROOS_CFG_TESTS_ENABLE_KERNEL == true) */
#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/**
 * @brief   Period of the system timer.
 */
#define SYSTIMER_PERIOD               (TIME_MAX_SYSTIME / 2)

/**
 * @brief   Width of the printable system info text.
 */
#define SYSTEM_INFO_WIDTH             80

/**
 * @brief   Position at which optional text shall be printed in separator lines.
 */
#define SYSTEM_INFO_SEPARATOR_TEXTPOS 4

/**
 * @brief   Width of the name column of the system info table.
 */
#define SYSTEM_INFO_NAMEWIDTH         30

/**
 * @brief   Number of columns to be printed for the module:memdump shell command.
 */
#define SYSTEM_MEMDUMP_COLUMNS        16

/*
 * sanity checks
 */
#if !(SYSTEM_INFO_WIDTH > SYSTEM_INFO_SEPARATOR_TEXTPOS)
# error "SYSTEM_INFO_WIDTH must be larger than SYSTEM_INFO_SEPARATOR_TEXTPOS"
#endif
#if !(SYSTEM_INFO_WIDTH > SYSTEM_INFO_NAMEWIDTH)
# error "SYSTEM_INFO_WIDTH must be larger than SYSTEM_INFO_NAMEWIDTH"
#endif

/******************************************************************************/
/* EXPORTED VARIABLES                                                         */
/******************************************************************************/

aos_system_t aos;

/******************************************************************************/
/* LOCAL TYPES                                                                */
/******************************************************************************/

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/

/*
 * forward declarations
 */
#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
static int _aosSysShellcmdConfigCallback(BaseSequentialStream* stream, int argc, const char* argv[]);
static int _aosSysShellcmdInfoCallback(BaseSequentialStream* stream, int argc, const char* argv[]);
static int _aosSysShellcmdShutdownCallback(BaseSequentialStream* stream, int argc, const char* argv[]);
static int _aosSysShellcmdMemdumpCallback(BaseSequentialStream* stream, int argc, const char* argv[]);
#if (((CH_CFG_USE_REGISTRY == TRUE) || (CH_CFG_USE_THREADHIERARCHY == TRUE)) && (CH_DBG_STATISTICS == TRUE)) || defined(__DOXYGEN__)
static int _aosSysShellcmdTopCallback(BaseSequentialStream* stream, int argc, const char* argv[]);
#endif /* ((CH_CFG_USE_REGISTRY == TRUE) || (CH_CFG_USE_THREADHIERARCHY == TRUE)) && (CH_DBG_STATISTICS == TRUE) */
#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_KERNEL == true)) || defined(__DOXYGEN__)
static int _aosSysShellcmdKerneltestCallback(BaseSequentialStream* stream, int argc, const char* argv[]);
#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_KERNEL == true) */
#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */

/**
 * @brief   Timer to accumulate system uptime.
 */
static virtual_timer_t _systimer;

/**
 * @brief   Accumulated system uptime.
 */
static aos_timestamp_t _uptime;

/**
 * @brief   Timer register value of last accumulation.
 */
static systime_t _synctime;

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)

/**
 * @brief   System shell name.
 */
static const char _shell_name[] = "system shell";

#if defined(MODULE_HAL_SERIAL) || defined(__DOXYGEN__)

/**
 * @brief   I/O shell channel for the programmer interface.
 */
static AosShellChannel _stdshellchannel;

#endif /* defined(MODULE_HAL_SERIAL) */

/**
 * @brief   Shell command to retrieve system information.
 */
static AOS_SHELL_COMMAND(_shellcmd_info, "module:info", _aosSysShellcmdInfoCallback);

/**
 * @brief   Shell command to set or retrieve system configuration.
 */
static AOS_SHELL_COMMAND(_shellcmd_config, "module:config", _aosSysShellcmdConfigCallback);

/**
 * @brief   Shell command to shutdown the system.
 */
#if (AMIROOS_CFG_SSSP_ENABLE == true) || defined(__DOXYGEN__)
static AOS_SHELL_COMMAND(_shellcmd_shutdown, "system:shutdown", _aosSysShellcmdShutdownCallback);
#else /* (AMIROOS_CFG_SSSP_ENABLE == true) */
static AOS_SHELL_COMMAND(_shellcmd_shutdown, "module:shutdown", _aosSysShellcmdShutdownCallback);
#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */

/**
 * @brief   Shell command to print raw memory content.
 */
static AOS_SHELL_COMMAND(_shellcmd_memdump, "module:memdump", _aosSysShellcmdMemdumpCallback);

#if (((CH_CFG_USE_REGISTRY == TRUE) || (CH_CFG_USE_THREADHIERARCHY == TRUE)) && \
     (CH_DBG_STATISTICS == TRUE)) || defined(__DOXYGEN__)

/**
 * @brief   Shell command to print table of processes
 */
static AOS_SHELL_COMMAND(_shellcmd_top, "module:top", _aosSysShellcmdTopCallback);

#endif /* ((CH_CFG_USE_REGISTRY == TRUE) ||
           (CH_CFG_USE_THREADHIERARCHY == TRUE)) &&
          (CH_DBG_STATISTICS == TRUE) */

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_KERNEL == true)) || defined(__DOXYGEN__)

/**
 * @brief   Shell command to run a test of the ChibiOS/RT kernel.
 */
static AOS_SHELL_COMMAND(_shellcmd_kerneltest, "kernel:test", _aosSysShellcmdKerneltestCallback);

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_KERNEL == true) */
#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   Print a separator line.
 *
 * @param[in] stream    Stream to print to or NULL to print to all system
 *                      streams.
 * @param[in] c         Character to use.
 * @param[in] text      Optional headline text to be printed.
 *                      May be NULL.
 *
 * @return  Number of characters printed.
 */
unsigned int _aosSysPrintSystemInfoSeparator(BaseSequentialStream* stream, const char c, const char text[])
{
  aosDbgCheck(stream != NULL);

  // local variables
  unsigned int n = 0;

  // if a text was specified
  if (text) {
#if (SYSTEM_INFO_SEPARATOR_TEXTPOS > 0)
    // print leading seperator characters
    for (; n < SYSTEM_INFO_SEPARATOR_TEXTPOS - 1; ++n) {
      streamPut(stream, (uint8_t)c);
    }
    // print an additional whitespace
    streamPut(stream, (uint8_t)' ');
    ++n;
#endif /* SYSTEM_INFO_SEPARATOR_TEXTPOS > 0 */
    // print the text
    n += chprintf(stream, "%s", text);
    // append another whitespace if there is width left
    if (n < SYSTEM_INFO_WIDTH) {
      streamPut(stream, (uint8_t)' ');
      ++n;
    }
  }

  // print the separator character until the desired line width is reached
  for (; n < SYSTEM_INFO_WIDTH; ++n) {
    streamPut(stream, (uint8_t)c);
  }
  streamPut(stream, '\n');

  return n+1;
}

/**
 * @brief   Print a system information line.
 * @details Prints a system information line with the following format:
 *            "name[spaces]fmt"
 *          The combined width of "name[spaces]" can be specified in order to
 *          align 'fmt' on multiple lines.
 *          Note that there is not trailing newline added implicitely.
 *
 * @param[in] stream      Stream to print to or NULL to print to all system
 *                        streams.
 * @param[in] name        Name of the entry/line.
 *                        May be NULL.
 * @param[in] fmt         Formatted string of information content.
 *
 * @return  Number of characters printed.
 */
unsigned int _aosSysPrintSystemInfoLine(BaseSequentialStream* stream, const char* name, const char* fmt, ...)
{
  aosDbgCheck(stream != NULL);

  unsigned int n = 0;
  va_list ap;

  va_start(ap, fmt);
  if (name != NULL) {
    n += (unsigned int)chprintf(stream, name);
  }
  if (n > 0) {
    // print at least a single space character
    streamPut(stream, ' ');
    ++n;
  }
  // fill with spaces
  for (; n < SYSTEM_INFO_NAMEWIDTH; ++n) {
    streamPut(stream, ' ');
  }
  n += (unsigned int)chvprintf(stream, fmt, ap);
  va_end(ap);

  streamPut(stream, '\n');
  ++n;

  return n;
}

/**
 * @brief   Prints information about the system.
 * @param stream      Stream to print to.
 * @param separator   Character for printing separator lines.
 */
void _aosSysPrintSystemInfo(BaseSequentialStream* stream, const char separator)
{
  aosDbgCheck(stream != NULL);

  /* print hardware information */
  _aosSysPrintSystemInfoSeparator(stream, separator, "HARDWARE");
  _aosSysPrintSystemInfoLine(stream, "module name", "%s", BOARD_NAME);
#if defined(PLATFORM_NAME)
  _aosSysPrintSystemInfoLine(stream, "platform", "%s", PLATFORM_NAME);
#endif /* defined(PLATFORM_NAME) */
#if defined(PORT_CORE_VARIANT_NAME)
  _aosSysPrintSystemInfoLine(stream, "core variant", "%s", PORT_CORE_VARIANT_NAME);
#endif /* defined(PORT_CORE_VARIANT_NAME) */
  _aosSysPrintSystemInfoLine(stream, "architecture", "%s", PORT_ARCHITECTURE_NAME);
  _aosSysPrintSystemInfoLine(stream, "core frequency", "%u MHz", SystemCoreClock / 1000000);
  {
    // calculate RAM size (necessary to remove redundant memory ranges)
    extern uint8_t __ram0_base__, __ram0_end__;
    extern uint8_t __ram1_base__, __ram1_end__;
    extern uint8_t __ram2_base__, __ram2_end__;
    extern uint8_t __ram3_base__, __ram3_end__;
    extern uint8_t __ram4_base__, __ram4_end__;
    extern uint8_t __ram5_base__, __ram5_end__;
    extern uint8_t __ram6_base__, __ram6_end__;
    extern uint8_t __ram7_base__, __ram7_end__;
    struct ram {void* const base; void* const end; size_t size;};
    struct ram ram[] = {{(void*)&__ram0_base__, (void*)&__ram0_end__, (void*)&__ram0_end__ - (void*)&__ram0_base__},
                        {(void*)&__ram1_base__, (void*)&__ram1_end__, (void*)&__ram1_end__ - (void*)&__ram1_base__},
                        {(void*)&__ram2_base__, (void*)&__ram2_end__, (void*)&__ram2_end__ - (void*)&__ram2_base__},
                        {(void*)&__ram3_base__, (void*)&__ram3_end__, (void*)&__ram3_end__ - (void*)&__ram3_base__},
                        {(void*)&__ram4_base__, (void*)&__ram4_end__, (void*)&__ram4_end__ - (void*)&__ram4_base__},
                        {(void*)&__ram5_base__, (void*)&__ram5_end__, (void*)&__ram5_end__ - (void*)&__ram5_base__},
                        {(void*)&__ram6_base__, (void*)&__ram6_end__, (void*)&__ram6_end__ - (void*)&__ram6_base__},
                        {(void*)&__ram7_base__, (void*)&__ram7_end__, (void*)&__ram7_end__ - (void*)&__ram7_base__}};
    size_t ram_size = 0;
    // adapt each RAM section
    for (size_t i = 0; i < sizeof(ram) / sizeof(struct ram); ++i) {
      if (ram[i].size == 0) {
        continue;
      }
      // compare to all remaining RAM sections
      for (size_t j = i+1; j < sizeof(ram) / sizeof(struct ram); ++j) {
        if (ram[j].size == 0) {
          continue;
        }
        // if i overlaps j, reduce size of i
        if ((ram[i].base >= ram[j].base && ram[i].base <= ram[j].end) ||
            (ram[i].end >= ram[j].base && ram[i].end <= ram[j].end)) {
          const void* overlap_base = (ram[i].base >= ram[j].base) ? ram[i].base : ram[j].base;
          const void* overlap_end = (ram[i].end < ram[j].end) ? ram[i].end : ram[j].end;
          const size_t overlap_size = overlap_end - overlap_base;
          ram[i].size = (overlap_size <= ram[i].size) ? ram[i].size - overlap_size : 0;
        }
      }
      ram_size += ram[i].size;
    }
    _aosSysPrintSystemInfoLine(stream, "RAM", "%u kiB", ram_size / 1024);
  }

  /* print bootloader information */
#if (AMIROOS_CFG_BOOTLOADER != AOS_BOOTLOADER_NONE)
  _aosSysPrintSystemInfoSeparator(stream, separator, "BOOTLOADER");
#endif /* (AMIROOS_CFG_BOOTLOADER != AOS_BOOTLOADER_NONE) */
#if (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_AMiRoBLT)
  if (BL_CALLBACK_TABLE_ADDRESS->magicNumber == BL_MAGIC_NUMBER) {
    _aosSysPrintSystemInfoLine(stream, "AMiRo-BLT", "%u.%u.%u %s (SSSP %u.%u)", BL_CALLBACK_TABLE_ADDRESS->vBootloader.major, BL_CALLBACK_TABLE_ADDRESS->vBootloader.minor, BL_CALLBACK_TABLE_ADDRESS->vBootloader.patch,
                               (BL_CALLBACK_TABLE_ADDRESS->vBootloader.identifier == BL_VERSION_ID_AMiRoBLT_Release) ? "stable" :
                               (BL_CALLBACK_TABLE_ADDRESS->vBootloader.identifier == BL_VERSION_ID_AMiRoBLT_ReleaseCandidate) ? "release candidate" :
                               (BL_CALLBACK_TABLE_ADDRESS->vBootloader.identifier == BL_VERSION_ID_AMiRoBLT_Beta) ? "beta" :
                               (BL_CALLBACK_TABLE_ADDRESS->vBootloader.identifier == BL_VERSION_ID_AMiRoBLT_Alpha) ? "alpha" :
                               (BL_CALLBACK_TABLE_ADDRESS->vBootloader.identifier == BL_VERSION_ID_AMiRoBLT_PreAlpha) ? "pre-alpha" :
                               "<release type unknown>",
                               BL_CALLBACK_TABLE_ADDRESS->vSSSP.major, BL_CALLBACK_TABLE_ADDRESS->vSSSP.minor);
#if (AMIROOS_CFG_SSSP_ENABLE == true)
    if (BL_CALLBACK_TABLE_ADDRESS->vSSSP.major != AOS_SSSP_VERSION_MAJOR) {
      chprintf(stream, "WARNING: AMiRo-BLT and AMiRo-OS implement incompatible SSSP versions!\n");
    }
#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */
    _aosSysPrintSystemInfoLine(stream, "compiler", "%s %u.%u.%u", (BL_CALLBACK_TABLE_ADDRESS->vCompiler.identifier == BL_VERSION_ID_GCC) ? "GCC" : "<compiler unknown>", BL_CALLBACK_TABLE_ADDRESS->vCompiler.major, BL_CALLBACK_TABLE_ADDRESS->vCompiler.minor, BL_CALLBACK_TABLE_ADDRESS->vCompiler.patch); // TODO: support other compilers than GCC
  } else {
    chprintf(stream, "WARNING: AMiRo-BLT incompatible or not available.\n");
  }
#elif (AMIROOS_CFG_BOOTLOADER != AOS_BOOTLOADER_NONE)
# error "AMIROOS_CFG_BOOTLOADER set to invalid value"
#endif /* (AMIROOS_CFG_BOOTLOADER == X) */

  /* print operating system information */
  _aosSysPrintSystemInfoSeparator(stream, separator, "OPERATING SYSTEM");
#if (AMIROOS_CFG_SSSP_ENABLE == true)
  _aosSysPrintSystemInfoLine(stream, "AMiRo-OS" , "%u.%u.%u %s (SSSP %u.%u)", AMIROOS_VERSION_MAJOR, AMIROOS_VERSION_MINOR, AMIROOS_VERSION_PATCH, AMIROOS_RELEASE_TYPE, AOS_SSSP_VERSION_MAJOR, AOS_SSSP_VERSION_MINOR);
#else /* (AMIROOS_CFG_SSSP_ENABLE == true) */
  _aosSysPrintSystemInfoLine(stream, "AMiRo-OS" , "%u.%u.%u %s", AMIROOS_VERSION_MAJOR, AMIROOS_VERSION_MINOR, AMIROOS_VERSION_PATCH, AMIROOS_RELEASE_TYPE);
#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */
  _aosSysPrintSystemInfoLine(stream, "AMiRo-LLD" , "%u.%u.%u %s (periphAL %u.%u)", AMIROLLD_VERSION_MAJOR, AMIROLLD_VERSION_MINOR, AMIROLLD_VERSION_PATCH, AMIROLLD_RELEASE_TYPE, PERIPHAL_VERSION_MAJOR, PERIPHAL_VERSION_MINOR);
  _aosSysPrintSystemInfoLine(stream, "ChibiOS/RT" , "%u.%u.%u %s", CH_KERNEL_MAJOR, CH_KERNEL_MINOR, CH_KERNEL_PATCH, (CH_KERNEL_STABLE == 1) ? "stable" : "non-stable");
  _aosSysPrintSystemInfoLine(stream, "ChibiOS/HAL", "%u.%u.%u %s", CH_HAL_MAJOR, CH_HAL_MINOR, CH_HAL_PATCH, (CH_HAL_STABLE == 1) ? "stable" : "non-stable");
  _aosSysPrintSystemInfoLine(stream, "build type", "%s", (AMIROOS_CFG_DBG == true) ? "debug" : "release");
  _aosSysPrintSystemInfoLine(stream, "compiler" , "%s %u.%u.%u", "GCC", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__); // TODO: support other compilers than GCC
  _aosSysPrintSystemInfoLine(stream, "compiled" , "%s - %s", __DATE__, __TIME__);

  /* print SSSP information */
#if (AMIROOS_CFG_SSSP_ENABLE == true)
  _aosSysPrintSystemInfoSeparator(stream, separator, "SSSP");
  _aosSysPrintSystemInfoLine(stream, "version", "%u.%u", AOS_SSSP_VERSION_MAJOR, AOS_SSSP_VERSION_MINOR);
  if (aos.sssp.moduleId != AOS_SSSP_MODULEID_INVALID) {
    _aosSysPrintSystemInfoLine(stream, "module ID", "%u", aos.sssp.moduleId);
  } else {
    _aosSysPrintSystemInfoLine(stream, "module ID", "not available");
  }
#if (AMIROOS_CFG_SSSP_MSI == true)
  _aosSysPrintSystemInfoLine(stream, "#modules", "%u", aos.sssp.modulecount);
#endif /* (AMIROOS_CFG_SSSP_MSI == true) */
#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */

  /* print calendar and timing information */
#if (HAL_USE_RTC == TRUE)
  _aosSysPrintSystemInfoSeparator(stream, separator, "CALENDAR & TIMING");
  {
    struct tm dt;
    aosSysGetDateTime(&dt);
    _aosSysPrintSystemInfoLine(stream, "date", "%04u-%02u-%02u (%s)",
                               dt.tm_year + 1900,
                               dt.tm_mon + 1,
                               dt.tm_mday,
                               (dt.tm_wday == 0) ? "Sunday" : (dt.tm_wday == 1) ? "Monday" : (dt.tm_wday == 2) ? "Tuesday" : (dt.tm_wday == 3) ? "Wednesday" : (dt.tm_wday == 4) ? "Thursday" : (dt.tm_wday == 5) ? "Friday" : "Saturday");
    _aosSysPrintSystemInfoLine(stream, "time", "%02u:%02u:%02u", dt.tm_hour, dt.tm_min, dt.tm_sec);
  }
#else /* (HAL_USE_RTC == TRUE) */
  _aosSysPrintSystemInfoSeparator(stream, separator, "TIMING");
#endif /* (HAL_USE_RTC == TRUE) */
  _aosSysPrintSystemInfoLine(stream, "module time resolution", "%u us", AOS_SYSTEM_TIME_RESOLUTION);
  _aosSysPrintSystemInfoLine(stream, "maximum expected jitter", "%u us (%u clock cycles)", TIME_I2US(CH_CFG_ST_TIMEDELTA), US2RTC(SystemCoreClock, TIME_I2US(CH_CFG_ST_TIMEDELTA)));
  {
    aos_timestamp_t uptime;
    aosSysGetUptime(&uptime);
    _aosSysPrintSystemInfoLine(stream, "system uptime (d:h:m:s:ms:us)", "%u:%02u:%02u:%02u:%03u:%03u",
                               (uint32_t)(aosTimestampGet(uptime) / MICROSECONDS_PER_DAY),
                               (uint8_t)(aosTimestampGet(uptime) % MICROSECONDS_PER_DAY / MICROSECONDS_PER_HOUR),
                               (uint8_t)(aosTimestampGet(uptime) % MICROSECONDS_PER_HOUR / MICROSECONDS_PER_MINUTE),
                               (uint8_t)(aosTimestampGet(uptime) % MICROSECONDS_PER_MINUTE / MICROSECONDS_PER_SECOND),
                               (uint16_t)(aosTimestampGet(uptime) % MICROSECONDS_PER_SECOND / MICROSECONDS_PER_MILLISECOND),
                               (uint16_t)(aosTimestampGet(uptime) % MICROSECONDS_PER_MILLISECOND / MICROSECONDS_PER_MICROSECOND));
  }
#if (AMIROOS_CFG_SSSP_ENABLE == true) && (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) && (AMIROOS_CFG_SSSP_MASTER != true) && (AMIROOS_CFG_PROFILE == true)
  _aosSysPrintSystemInfoLine(stream, "SSSP Synchronization Offset", "%.3f us per %u us", aosSsspGetSyncSkew(), AMIROOS_CFG_SSSP_SYSSYNCPERIOD);
#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) && (AMIROOS_CFG_SSSP_SYSSYNCPERIOD > 0) && (AMIROOS_CFG_SSSP_MASTER != true) && (AMIROOS_CFG_PROFILE == true) */

  /* print runtime information */
  _aosSysPrintSystemInfoSeparator(stream, separator, "RUNTIME INFORMATION");
  _aosSysPrintSystemInfoLine(stream, "active threads", "%u", ch_system.threadcount);
#if (CH_DBG_STATISTICS == TRUE)
  {
    const char warnmsg[] = "  WARNING: Maximum expected jitter has been exceeded!";
    rtcnt_t cnt = currcore->kernel_stats.m_crit_thd.worst;
    _aosSysPrintSystemInfoLine(stream, "worst thread critical zone", "%u us (%u clock cycles)%s",
                               RTC2US(SystemCoreClock, cnt), cnt,
                               (RTC2US(SystemCoreClock, cnt) > TIME_I2US(CH_CFG_ST_TIMEDELTA)) ? warnmsg : "");
    cnt = currcore->kernel_stats.m_crit_isr.worst;
    _aosSysPrintSystemInfoLine(stream, "worst ISR critical zone", "%u us (%u clock cycles)%s",
                               RTC2US(SystemCoreClock, cnt), cnt,
                               (RTC2US(SystemCoreClock, cnt) > TIME_I2US(CH_CFG_ST_TIMEDELTA)) ? warnmsg : "");
  }
#endif /* (CH_DBG_STATISTICS == TRUE) */

  /* print custom information last */
#if defined(AMIROOS_CFG_SYSINFO_HOOK)
  // print module specific information
  _aosSysPrintSystemInfoSeparator(stream, separator, "MODULE CUSTOM INFO");
  {
    AMIROOS_CFG_SYSINFO_HOOK();
  }
#endif /* defined(AMIROOS_CFG_SYSINFO_HOOK) */

  /* print closing separator line */
  _aosSysPrintSystemInfoSeparator(stream, separator, NULL);

  return;
}

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
/**
 * @brief   Callback function for the system:config shell command.
 *
 * @param[in] stream    The I/O stream to use.
 * @param[in] argc      Number of arguments.
 * @param[in] argv      List of pointers to the arguments.
 *
 * @return              An exit status.
 * @retval  AOS_OK                  The command was executed successfuly.
 * @retval  AOS_INVALIDARGUMENTS    There was an issue with the arguemnts.
 */
static int _aosSysShellcmdConfigCallback(BaseSequentialStream* stream, int argc, const char* argv[])
{
  aosDbgCheck(stream != NULL);

  // local variables
  int retval = AOS_INVALIDARGUMENTS;

  // if there are additional arguments
  if (argc > 1) {
# if (HAL_USE_RTC == TRUE)
    // if the user wants to configure the date or time
    if (strcmp(argv[1], "--date&time") == 0 && argc == 4) {
      struct tm dt;
      aosSysGetDateTime(&dt);
      int val = atoi(argv[3]);
      if (strcmp(argv[2], "year") == 0 && val >= 1900) {
        dt.tm_year = val - 1900;
      }
      else if (strcmp(argv[2], "month") == 0 && val > 0 && val <= 12) {
        dt.tm_mon = val - 1;
      }
      else if (strcmp(argv[2], "day") == 0 && val > 0 && val <= 31) {
        dt.tm_mday = val;
      }
      else if (strcmp(argv[2], "hour") == 0 && val >= 0 && val < 24) {
        dt.tm_hour = val;
      }
      else if (strcmp(argv[2], "minute") == 0 && val >= 0 && val < 60) {
        dt.tm_min = val;
      }
      else if (strcmp(argv[2], "second") == 0 && val >= 0 && val < 60) {
        dt.tm_sec = val;
      }
      else {
        chprintf(stream, "unknown option '%s' or invalid value '%s'\n", argv[2], argv[3]);
        return AOS_INVALIDARGUMENTS;
      }
      dt.tm_wday = aosTimeDayOfWeekFromDate((uint16_t)dt.tm_mday, (uint8_t)dt.tm_mon+1, (uint16_t)dt.tm_year+1900) % DAYS_PER_WEEK;
      aosSysSetDateTime(&dt);

      // read and print new date and time
      aosSysGetDateTime(&dt);
      chprintf(stream, "date/time set to %04u-%02u-%02u %02u:%02u:%02u\n",
               dt.tm_year+1900, dt.tm_mon+1, dt.tm_mday,
               dt.tm_hour, dt.tm_min, dt.tm_sec);

      retval = AOS_OK;
    }
#endif /* (HAL_USE_RTC == TRUE) */
  }

  // print help, if required
  if (retval == AOS_INVALIDARGUMENTS) {
    chprintf(stream, "Usage: %s OPTION\n", argv[0]);
    chprintf(stream, "Options:\n");
    chprintf(stream, "  --help, -h\n");
    chprintf(stream, "    Print this help text.\n");
#if (HAL_USE_RTC == TRUE)
    chprintf(stream, "  --date&time OPT VAL\n");
    chprintf(stream, "    Set the date/time value of OPT to VAL.\n");
    chprintf(stream, "    Possible OPTs are:\n");
    chprintf(stream, "      year\n");
    chprintf(stream, "      month\n");
    chprintf(stream, "      day\n");
    chprintf(stream, "      hour\n");
    chprintf(stream, "      minute\n");
    chprintf(stream, "      second\n");
#endif /* (HAL_USE_RTC == TRUE) */
  }

  return (aosShellcmdCheckHelp(argc, argv, true) == AOS_SHELL_HELPREQUEST_EXPLICIT) ? AOS_OK : retval;
}

/**
 * @brief   Callback function for the system:info shell command.
 *
 * @param[in] stream    The I/O stream to use.
 * @param[in] argc      Number of arguments.
 * @param[in] argv      List of pointers to the arguments.
 *
 * @return            An exit status.
 * @retval  AOS_OK    The command was executed successfully.
 */
static int _aosSysShellcmdInfoCallback(BaseSequentialStream* stream, int argc, const char* argv[])
{
  aosDbgCheck(stream != NULL);

  (void)argc;
  (void)argv;

  // print standard system information
  _aosSysPrintSystemInfo(stream, '-');

  return AOS_OK;
}

/**
 * @brief   Callback function for the sytem:shutdown or module:shutdown shell
 *          command.
 *
 * @param[in] stream    The I/O stream to use.
 * @param[in] argc      Number of arguments.
 * @param[in] argv      List of pointers to the arguments.
 *
 * @return              An exit status.
 * @retval  AOS_OK                  The command was executed successfully.
 * @retval  AOS_INVALIDARGUMENTS   There was an issue with the arguments.
 */
static int _aosSysShellcmdShutdownCallback(BaseSequentialStream* stream, int argc, const char* argv[])
{
  aosDbgCheck(stream != NULL);

#if (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_NONE)

  // print help text
  if (aosShellcmdCheckHelp(argc, argv, false) == AOS_SHELL_HELPREQUEST_EXPLICIT) {
    chprintf(stream, "Usage: %s OPTION\n", argv[0]);
    chprintf(stream, "  Terminate all threads and shutdown module.\n");
    chprintf(stream, "Options:\n");
    chprintf(stream, "  --help\n");
    chprintf(stream, "    Print this help text.\n");
    chprintf(stream, "  --restart, -r\n");
    chprintf(stream, "    Restart after shutdown.\n");

    return AOS_OK;
  }
  // if no argument was given
  else if (argc == 1) {
    // broadcast shutdown event
    chEvtBroadcastFlags(&aos.events.os, AOS_SYSTEM_EVENTFLAGS_SHUTDOWN);
    // set terminate flag so no further prompt will be printed
    chThdTerminate(chThdGetSelfX());
    return AOS_OK;
  }
  // handle 'restart' argument
  else if (argc == 2 &&
      (strcmp(argv[1], "-r") == 0 || strcmp(argv[1], "--restart") == 0)) {
    // broadcast shutdown event
    chEvtBroadcastFlags(&aos.events.os, AOS_SYSTEM_EVENTFLAGS_SHUTDOWN_RESTART);
    // set terminate flag so no further prompt will be printed
    chThdTerminate(chThdGetSelfX());
    return AOS_OK;
  }
  else {
    chprintf(stream, "unknown argument '%s'\n", argv[1]);
    return AOS_INVALIDARGUMENTS;
  }

#elif (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_AMiRoBLT)

  // print help text
  if (aosShellcmdCheckHelp(argc, argv, false) != AOS_SHELL_HELPREQUEST_NONE) {
    chprintf(stream, "Usage: %s OPTION\n", argv[0]);
    chprintf(stream, "Options:\n");
    chprintf(stream, "  --help\n");
    chprintf(stream, "    Print this help text.\n");
    chprintf(stream, "  --hibernate, -h\n");
    chprintf(stream, "    Shutdown to hibernate mode.\n");
    chprintf(stream, "    Least energy saving, but allows charging via pins.\n");
    chprintf(stream, "  --deepsleep, -d\n");
    chprintf(stream, "    Shutdown to deepsleep mode.\n");
    chprintf(stream, "    Minimum energy consumption while allowing charging via plug.\n");
    chprintf(stream, "  --transportation, -t\n");
    chprintf(stream, "    Shutdown to transportation mode.\n");
    chprintf(stream, "    Minimum energy consumption with all interrupts disabled (no charging).\n");
    chprintf(stream, "  --restart, -r\n");
    chprintf(stream, "    Shutdown and restart system.\n");

    return (aosShellcmdCheckHelp(argc, argv, false) == AOS_SHELL_HELPREQUEST_EXPLICIT) ? AOS_OK : AOS_INVALIDARGUMENTS;
  }
  // handle argument
  else {
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--hibernate") == 0) {
      // broadcast shutdown event
      chEvtBroadcastFlags(&aos.events.os, AOS_SYSTEM_EVENTFLAGS_SHUTDOWN_HIBERNATE);
      // set terminate flag so no further prompt will be printed
      chThdTerminate(chThdGetSelfX());
      return AOS_OK;
    }
    else if (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--deepsleep") == 0) {
      // broadcast shutdown event
      chEvtBroadcastFlags(&aos.events.os, AOS_SYSTEM_EVENTFLAGS_SHUTDOWN_DEEPSLEEP);
      // set terminate flag so no further prompt will be printed
      chThdTerminate(chThdGetSelfX());
      return AOS_OK;
    }
    else if (strcmp(argv[1], "-t") == 0 || strcmp(argv[1], "--transportation") == 0) {
      // broadcast shutdown event
      chEvtBroadcastFlags(&aos.events.os, AOS_SYSTEM_EVENTFLAGS_SHUTDOWN_TRANSPORTATION);
      // set terminate flag so no further prompt will be printed
      chThdTerminate(chThdGetSelfX());
      return AOS_OK;
    }
    else if (strcmp(argv[1], "-r") == 0 || strcmp(argv[1], "--restart") == 0) {
      // broadcast shutdown event
      chEvtBroadcastFlags(&aos.events.os, AOS_SYSTEM_EVENTFLAGS_SHUTDOWN_RESTART);
      // set terminate flag so no further prompt will be printed
      chThdTerminate(chThdGetSelfX());
      return AOS_OK;
    }
    else {
      chprintf(stream, "unknown argument '%s'\n", argv[1]);
      return AOS_INVALIDARGUMENTS;
    }
  }

#endif /* (AMIROOS_CFG_BOOTLOADER == X) */
}

/**
 * @brief   Convert a size in bytes to a value in kiB with specified number of
 *          decimals and an optional nearest integer floating-point operator
 *          (nifpo).
 *
 * @param[in] size      Number of bytes to convert.
 * @param[in] decimals  Number of decimals of the result.
 * @param[in] nifpo     Optional function pointer for rounding, ceiling, etc.
 *                      May be NULL, defaults to round().
 *
 * @return  Converted size in kiB with specified number of decimals.
 */
float _aosSysShellcmdMemdumpCallback_BtokiB(const size_t size, const unsigned int decimals, float(*nifpo)(float))
{
  const float factor = powf(10.f, decimals);
  if (!nifpo) {
    nifpo = roundf;
  }

  return nifpo(factor * ((float)size/(float)1024)) / factor;
}

/**
 * @brief   Print memory data to a stream.
 *
 * @param[in] stream  Stream to print to.
 * @param[in] base    Base address.
 * @param[in] end     End address.
 */
void _aosSysShellcmdMemdumpCallback_printDump(BaseSequentialStream* stream, void const* const base, void const* const end)
{
  aosDbgCheck(stream != NULL);
  aosDbgCheck(base <= end);

  for (uint8_t* addr = (uint8_t*)base - ((size_t)base % SYSTEM_MEMDUMP_COLUMNS); addr < (uint8_t*)end; ++addr) {
    if ((size_t)addr % SYSTEM_MEMDUMP_COLUMNS == 0) {
      if (addr > (uint8_t*)base) {
        streamPut(stream, '\n');
      }
      chprintf(stream, "0x%08X", addr);
    }
    if ((size_t)addr % sizeof(void*) == 0) {
      streamPut(stream, ' ');
    }
    if ((void*)addr < base) {
      chprintf(stream, "   ");
    } else {
      chprintf(stream, " %02X", *addr);
    }
  }

  const size_t size = end - base;
  if (size > 0) {
    streamPut(stream, '\n');
  }
  chprintf(stream, "size: %u B (%.2f kiB)\n", size, _aosSysShellcmdMemdumpCallback_BtokiB(size, 2, ceilf));

  return;
}

/**
 * @brief   Callback function for the module:memdump shell command.
 *
 * @param[in] stream    The I/O stream to use.
 * @param[in] argc      Number of arguments.
 * @param[in] argv      List of pointers to the arguments.
 *
 * @return              An exit status.
 * @retval  AOS_OK                 The command was executed successfully.
 * @retval  AOS_INVALIDARGUMENTS   There was an issue with the arguments.
 */
static int _aosSysShellcmdMemdumpCallback(BaseSequentialStream* stream, int argc, const char* argv[])
{
  aosDbgCheck(stream != NULL);

  extern uint8_t __flash0_base__, __flash0_end__;
  extern uint8_t __flash1_base__, __flash1_end__;
  extern uint8_t __flash2_base__, __flash2_end__;
  extern uint8_t __flash3_base__, __flash3_end__;
  extern uint8_t __flash4_base__, __flash4_end__;
  extern uint8_t __flash5_base__, __flash5_end__;
  extern uint8_t __flash6_base__, __flash6_end__;
  extern uint8_t __flash7_base__, __flash7_end__;

  extern uint8_t __ram0_base__, __ram0_end__;
  extern uint8_t __ram1_base__, __ram1_end__;
  extern uint8_t __ram2_base__, __ram2_end__;
  extern uint8_t __ram3_base__, __ram3_end__;
  extern uint8_t __ram4_base__, __ram4_end__;
  extern uint8_t __ram5_base__, __ram5_end__;
  extern uint8_t __ram6_base__, __ram6_end__;
  extern uint8_t __ram7_base__, __ram7_end__;

  extern uint8_t __text_base__, __text_end__;
  extern uint8_t __rodata_base__, __rodata_end__;
  extern uint8_t __data_base__, __data_end__;
  extern uint8_t __bss_base__, __bss_end__;

  if (argc >= 2) {
    if (strcmp(argv[1], "flash0") == 0) {
      _aosSysShellcmdMemdumpCallback_printDump(stream, &__flash0_base__, &__flash0_end__);
      return AOS_OK;
    }
    if (strcmp(argv[1], "flash1") == 0) {
      _aosSysShellcmdMemdumpCallback_printDump(stream, &__flash1_base__, &__flash1_end__);
      return AOS_OK;
    }
    if (strcmp(argv[1], "flash2") == 0) {
      _aosSysShellcmdMemdumpCallback_printDump(stream, &__flash2_base__, &__flash2_end__);
      return AOS_OK;
    }
    if (strcmp(argv[1], "flash3") == 0) {
      _aosSysShellcmdMemdumpCallback_printDump(stream, &__flash3_base__, &__flash3_end__);
      return AOS_OK;
    }
    if (strcmp(argv[1], "flash4") == 0) {
      _aosSysShellcmdMemdumpCallback_printDump(stream, &__flash4_base__, &__flash4_end__);
      return AOS_OK;
    }
    if (strcmp(argv[1], "flash5") == 0) {
      _aosSysShellcmdMemdumpCallback_printDump(stream, &__flash5_base__, &__flash5_end__);
      return AOS_OK;
    }
    if (strcmp(argv[1], "flash6") == 0) {
      _aosSysShellcmdMemdumpCallback_printDump(stream, &__flash6_base__, &__flash6_end__);
      return AOS_OK;
    }
    if (strcmp(argv[1], "flash7") == 0) {
      _aosSysShellcmdMemdumpCallback_printDump(stream, &__flash7_base__, &__flash7_end__);
      return AOS_OK;
    }

    if (strcmp(argv[1], "ram0") == 0) {
      _aosSysShellcmdMemdumpCallback_printDump(stream, &__ram0_base__, &__ram0_end__);
      return AOS_OK;
    }
    if (strcmp(argv[1], "ram1") == 0) {
      _aosSysShellcmdMemdumpCallback_printDump(stream, &__ram1_base__, &__ram1_end__);
      return AOS_OK;
    }
    if (strcmp(argv[1], "ram2") == 0) {
      _aosSysShellcmdMemdumpCallback_printDump(stream, &__ram2_base__, &__ram2_end__);
      return AOS_OK;
    }
    if (strcmp(argv[1], "ram3") == 0) {
      _aosSysShellcmdMemdumpCallback_printDump(stream, &__ram3_base__, &__ram3_end__);
      return AOS_OK;
    }
    if (strcmp(argv[1], "ram4") == 0) {
      _aosSysShellcmdMemdumpCallback_printDump(stream, &__ram4_base__, &__ram4_end__);
      return AOS_OK;
    }
    if (strcmp(argv[1], "ram5") == 0) {
      _aosSysShellcmdMemdumpCallback_printDump(stream, &__ram5_base__, &__ram5_end__);
      return AOS_OK;
    }
    if (strcmp(argv[1], "ram6") == 0) {
      _aosSysShellcmdMemdumpCallback_printDump(stream, &__ram6_base__, &__ram6_end__);
      return AOS_OK;
    }
    if (strcmp(argv[1], "ram7") == 0) {
      _aosSysShellcmdMemdumpCallback_printDump(stream, &__ram7_base__, &__ram7_end__);
      return AOS_OK;
    }

    if (strcmp(argv[1], "text") == 0) {
      _aosSysShellcmdMemdumpCallback_printDump(stream, &__text_base__, &__text_end__);
      return AOS_OK;
    }
    if (strcmp(argv[1], "rodata") == 0) {
      _aosSysShellcmdMemdumpCallback_printDump(stream, &__rodata_base__, &__rodata_end__);
      return AOS_OK;
    }
    if (strcmp(argv[1], "data") == 0) {
      _aosSysShellcmdMemdumpCallback_printDump(stream, &__data_base__, &__data_end__);
      return AOS_OK;
    }
    if (strcmp(argv[1], "bss") == 0) {
      _aosSysShellcmdMemdumpCallback_printDump(stream, &__bss_base__, &__bss_end__);
      return AOS_OK;
    }

    if (strcmp(argv[1], "-t") == 0 || strcmp(argv[1], "--table") == 0) {
      chprintf(stream, "SECTION     BASE        END      SIZE (kiB)\n");
      void *base, *end;
      for (uint8_t f = 0; f < 8; ++f) {
        switch (f) {
          case 0:
            base = &__flash0_base__;
            end = &__flash0_end__;
            break;
          case 1:
            base = &__flash1_base__;
            end = &__flash1_end__;
            break;
          case 2:
            base = &__flash2_base__;
            end = &__flash2_end__;
            break;
          case 3:
            base = &__flash3_base__;
            end = &__flash3_end__;
            break;
          case 4:
            base = &__flash4_base__;
            end = &__flash4_end__;
            break;
          case 5:
            base = &__flash5_base__;
            end = &__flash5_end__;
            break;
          case 6:
            base = &__flash6_base__;
            end = &__flash6_end__;
            break;
          case 7:
            base = &__flash7_base__;
            end = &__flash7_end__;
            break;
        }
        chprintf(stream, "flash%u   0x%08X  0x%08X  %9.2f\n", f, base, end, _aosSysShellcmdMemdumpCallback_BtokiB(end - base, 2, NULL));
      }
      for (uint8_t r = 0; r < 8; ++r) {
        switch (r) {
          case 0:
            base = &__ram0_base__;
            end = &__ram0_end__;
            break;
          case 1:
            base = &__ram1_base__;
            end = &__ram1_end__;
            break;
          case 2:
            base = &__ram2_base__;
            end = &__ram2_end__;
            break;
          case 3:
            base = &__ram3_base__;
            end = &__ram3_end__;
            break;
          case 4:
            base = &__ram4_base__;
            end = &__ram4_end__;
            break;
          case 5:
            base = &__ram5_base__;
            end = &__ram5_end__;
            break;
          case 6:
            base = &__ram6_base__;
            end = &__ram6_end__;
            break;
          case 7:
            base = &__ram7_base__;
            end = &__ram7_end__;
            break;
        }
        chprintf(stream, "ram%u     0x%08X  0x%08X  %9.2f\n", r, base, end, _aosSysShellcmdMemdumpCallback_BtokiB(end - base, 2, NULL));
      }
      chprintf(stream, "text     0x%08X  0x%08X  %9.2f\n", &__text_base__, &__text_end__, _aosSysShellcmdMemdumpCallback_BtokiB(&__text_end__ - &__text_base__, 2, ceilf));
      chprintf(stream, "rodata   0x%08X  0x%08X  %9.2f\n", &__rodata_base__, &__rodata_end__,_aosSysShellcmdMemdumpCallback_BtokiB(&__rodata_end__ - &__rodata_base__, 2, ceilf));
      chprintf(stream, "data     0x%08X  0x%08X  %9.2f\n", &__data_base__, &__data_end__, _aosSysShellcmdMemdumpCallback_BtokiB(&__data_end__ - &__data_base__, 2, ceilf));
      chprintf(stream, "bss      0x%08X  0x%08X  %9.2f\n", &__bss_base__, &__bss_end__, _aosSysShellcmdMemdumpCallback_BtokiB(&__bss_end__ - &__bss_base__, 2, ceilf));
      return AOS_OK;
    }
  }

  // print help
  chprintf(stream, "Usage: %s OPTION\n", argv[0]);
  chprintf(stream, "Options:\n");
  chprintf(stream, "  --help, -h\n");
  chprintf(stream, "    Print this help text.\n");
  chprintf(stream, "  --table, -t\n");
  chprintf(stream, "    Print a table of all flash and ram sections.\n");
  chprintf(stream, "  flash{0-7}\n");
  chprintf(stream, "    Read and output data from flash memory.\n");
  chprintf(stream, "    There are up to 8 flash regions.\n");
  chprintf(stream, "  ram{0-7}\n");
  chprintf(stream, "    Read and output data from RAM.\n");
  chprintf(stream, "    There are up to 8 RAM regions.\n");
  chprintf(stream, "  text\n");
  chprintf(stream, "    Read and output data from the 'text' section (instructions).\n");
  chprintf(stream, "  rodata\n");
  chprintf(stream, "    Read and output data from the 'rodata' section (read-only data).\n");
  chprintf(stream, "  data\n");
  chprintf(stream, "    Read and output data from the 'data' section (writable, initializted data).\n");
  chprintf(stream, "  bss\n");
  chprintf(stream, "    Read and output data from the 'bss' section (uninitialized data).\n");

  return (aosShellcmdCheckHelp(argc, argv, true) == AOS_SHELL_HELPREQUEST_EXPLICIT) ? AOS_OK : AOS_INVALIDARGUMENTS;

  return AOS_OK;
}

#if (((CH_CFG_USE_REGISTRY == TRUE) || (CH_CFG_USE_THREADHIERARCHY == TRUE)) && \
     (CH_DBG_STATISTICS == TRUE)) || defined(__DOXYGEN__)

/**
 * @brief   Callback function for the module:top shell command.
 *
 * @param[in] stream    The I/O stream to use.
 * @param[in] argc      Number of arguments.
 * @param[in] argv      List of pointers to arguments.
 *
 * @return      An exit status.
 */
static int _aosSysShellcmdTopCallback(BaseSequentialStream* stream, int argc, const char* argv[])
{
  aosDbgCheck(stream != NULL);

  (void)argc;
  (void)argv;

  // local variables
  rttime_t sum = 0;
#if (CH_CFG_USE_THREADHIERARCHY == TRUE)
  size_t depth;
  size_t hierarchy_depth = 0;
  const char indent[] = "+-- ";
#endif /* (CH_CFG_USE_THREADHIERARCHY == TRUE) */
  size_t c;
  const char anonymous_name[] = "<anonymous thread>";
  const char critical_thd[] = "thread critical zones";
  const char critical_isr[] = "ISR critical zones";
  size_t namelength = 0;

  // iterate through all threads for a first time
  for (thread_t* thread = aosThdGetFirst(); thread != NULL; thread = aosThdGetNext(thread)) {
    // accumulate system load
    sum += thread->stats.cumulative;

#if (CH_CFG_USE_THREADHIERARCHY == TRUE)
    // retrieve hierarchy depth
    depth = 0;
    for (thread_t* thd = thread->parent; thd != NULL; thd = thd->parent) {
      ++depth;
    }
    if (depth > hierarchy_depth) {
      hierarchy_depth = depth;
    }
#endif /* (CH_CFG_USE_THREADHIERARCHY == TRUE) */

    // find maximum length of thread names
    if (chRegGetThreadNameX(thread)) {
      if (strlen(chRegGetThreadNameX(thread)) > namelength) {
        namelength = strlen(chRegGetThreadNameX(thread));
      }
    } else{
      if (strlen(anonymous_name) > namelength) {
        namelength = strlen(anonymous_name);
      }
    }
  }
#if (CH_CFG_USE_THREADHIERARCHY == TRUE)
  namelength += (hierarchy_depth + 1) * strlen(indent);
#endif /* (CH_CFG_USE_THREADHIERARCHY == TRUE) */
  namelength = (strlen(critical_thd) > namelength) ? strlen(critical_thd) : namelength;
  namelength = (strlen(critical_isr) > namelength) ? strlen(critical_isr) : namelength;
  ++namelength; // add additional padding
  // add critical zones to total system load
  sum += currcore->kernel_stats.m_crit_thd.cumulative;
  sum += currcore->kernel_stats.m_crit_isr.cumulative;

  // print table header
  c = chprintf(stream, "THREAD");
  while (c < namelength) {
    streamPut(stream, ' ');
    ++c;
  }
  chprintf(stream, "PRIO   CPU   STACK\n");

  // iterate through all threads for a second time
  for (thread_t* thread = aosThdGetFirst(); thread != NULL; thread = aosThdGetNext(thread)) {
    c = 0;

#if (CH_CFG_USE_THREADHIERARCHY == TRUE)
    // retrieve hierarchy level of current thread
    hierarchy_depth = 0;
    for (thread_t* thd = thread->parent; thd != NULL; thd = thd->parent) {
      ++hierarchy_depth;
    }
    // print hierarachy level indentation (check all level bottom up)
    for (depth = 0; depth < hierarchy_depth; ++depth) {
      // retrieve according parent
      thread_t* thd = thread;
      for (size_t d = 0; d < hierarchy_depth - depth; ++d) {
        thd = thd->parent;
      }
      // print one level of indentation
      if (thd->sibling) {
        c += chprintf(stream, "|   ");
      } else {
        c += chprintf(stream, "    ");
      }
    }
    c += chprintf(stream, "%s", indent);
#endif /* (CH_CFG_USE_THREADHIERARCHY == TRUE) */

    // print thread name and add padding
    c += chprintf(stream, "%s", chRegGetThreadNameX(thread) ? chRegGetThreadNameX(thread) : anonymous_name);
    while (c < namelength) {
      streamPut(stream, ' ');
      ++c;
    }

    // print thread priority
    chprintf(stream, "%4u", thread->realprio);

    // print CPU load and stack utilization
#if (AMIROOS_CFG_DBG == true) && (CH_DBG_FILL_THREADS == TRUE)
    chprintf(stream, " %5.1f%% %5.1f%% (%u/%u/%u bytes)\n",
             /* CPU load          */ (float)thread->stats.cumulative / (float)sum * 100.f,
             /* stack fill        */ (float)aosThdGetStackPeakUtilization(thread) / (float)aosThdGetStacksize(thread) * 100.f,
             /* stack utilization */ aosThdGetStackPeakUtilization(thread),
             /* Stack utilization CK*/ aosThdGetStackPeakUtilization2(thread),
             /* stack size        */ aosThdGetStacksize(thread));
#else /* (AMIROOS_CFG_DBG == true) && (CH_DBG_FILL_THREADS == TRUE) */
    chprintf(stream, "%5.1f%%    n/a\n",
             /* CPU load  */ (float)thread->stats.cumulative / (float)sum * 100.f);
#endif /* (AMIROOS_CFG_DBG == true) && (CH_DBG_FILL_THREADS == TRUE) */
  }
  // print CPU utilization of critical zones (threads)
  c = chprintf(stream, "%s", critical_thd);
  while (c < namelength) {
    streamPut(stream, ' ');
    ++c;
  }
  chprintf(stream, " n/a %5.1f%%    n/a\n",
           (float)currcore->kernel_stats.m_crit_thd.cumulative / (float)sum * 100.f);
  // print CPU utilization of critical zones (ISRs)
  c = chprintf(stream, "%s", critical_isr);
  while (c < namelength) {
    streamPut(stream, ' ');
    ++c;
  }
  chprintf(stream, " n/a %5.1f%%    n/a\n",
           (float)currcore->kernel_stats.m_crit_isr.cumulative / (float)sum * 100.f);

  // print empty line and critical zones
  chprintf(stream, "\nCRITICAL ZONES TIMING\n");
  chprintf(stream, "max expeced %u us (%u clock cycles)\n", TIME_I2US(CH_CFG_ST_TIMEDELTA), US2RTC(SystemCoreClock, TIME_I2US(CH_CFG_ST_TIMEDELTA)));
  {
    const char warnmsg[] = "  WARNING: Maximum expected jitter has been exceeded!";
    rtcnt_t cnt = currcore->kernel_stats.m_crit_thd.worst;
    chprintf(stream, "    threads %u us (%u clock cycles)%s\n",
             RTC2US(SystemCoreClock, cnt), cnt,
             (RTC2US(SystemCoreClock, cnt) > TIME_I2US(CH_CFG_ST_TIMEDELTA)) ? warnmsg: "");
    cnt = currcore->kernel_stats.m_crit_isr.worst;
    chprintf(stream, "       ISRs %u us (%u clock cycles)%s\n",
             RTC2US(SystemCoreClock, cnt), cnt,
             (RTC2US(SystemCoreClock, cnt) > TIME_I2US(CH_CFG_ST_TIMEDELTA)) ? warnmsg: "");
  }

  return 0;
}

#endif /* ((CH_CFG_USE_REGISTRY == TRUE) ||
           (CH_CFG_USE_THREADHIERARCHY == TRUE)) &&
          (CH_DBG_STATISTICS == TRUE) */

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_KERNEL == true)) || defined(__DOXYGEN__)

/**
 * @brief   Callback function for the kernel:test shell command.
 *
 * @param[in] stream    The I/O stream to use.
 * @param[in] argc      Number of arguments.
 * @param[in] argv      List of pointers to the arguments.
 *
 * @return      An exit status.
 */
static int _aosSysShellcmdKerneltestCallback(BaseSequentialStream* stream, int argc, const char* argv[])
{
  aosDbgCheck(stream != NULL);

  (void)argc;
  (void)argv;

  /* print a disclaimer regarding the credibility of the kernel tests
   * Kernel benchmarks are executed in 1 s time frames, but performance
   * calculations use systime_t values, which may be insufficient to represent
   * that time period.
   */
  if (TIME_MAX_SYSTIME < chTimeS2I(1)) {
    chprintf(stream, "DISCLAMER:\n");
    chprintf(stream, "Benchmark results are not reliable due to insufficient system timer width.\n");
  }
  msg_t retval = test_execute(stream, &rt_test_suite);
  retval |= test_execute(stream, &oslib_test_suite);

  return retval;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_KERNEL == true) */
#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */

// suppress warning in case no interrupt GPIOs are defined
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
/**
 * @brief   Generic callback function for GPIO interrupts.
 *
 * @param[in] args   Pointer to the GPIO line identifier.
 */
static void _aosSysGpioCallback(void* args)
{
  aosDbgCheck((args != NULL) && (*((ioline_t*)args) != PAL_NOLINE) && (PAL_PAD(*((ioline_t*)args)) < sizeof(eventflags_t) * 8));

  chSysLockFromISR();
  chEvtBroadcastFlagsI(&aos.events.gpio, AOS_GPIOEVENT_FLAG(PAL_PAD(*((ioline_t*)args))));
  chSysUnlockFromISR();

  return;
}
#pragma GCC diagnostic pop

/**
 * @brief   Callback function for the uptime accumulation timer.
 *
 * @param[in] timer   Pointer to the calling virtual timer.
 * @param[in] par     Generic parameter.
 */
static void _aosSysUptimeCallback(virtual_timer_t* timer, void* par)
{
  (void)timer;
  (void)par;

  chSysLockFromISR();
  // read current time in system ticks
  const systime_t st = chVTGetSystemTimeX();
  // enable the timer again
  chVTSetI(&_systimer, SYSTIMER_PERIOD, &_aosSysUptimeCallback, NULL);
  // update the uptime variables
  aosTimestampAddInterval(_uptime, chTimeI2US(chTimeDiffX(_synctime, st)));
  _synctime = st;
  chSysUnlockFromISR();

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup core_system
 * @{
 */

/**
 * @brief   AMiRo-OS system initialization.
 * @note    Must be called from the system control thread (usually main thread).
 */
void aosSysInit(void)
{
  /* set control thread to maximum priority */
  chThdSetPriority(AOS_THD_CTRLPRIO);

#if (AMIROOS_CFG_SSSP_ENABLE == true)
  aosSsspInit(&_uptime);
#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */

  /* set local variables */
  chVTObjectInit(&_systimer);
#if (AMIROOS_CFG_SSSP_ENABLE == true)
  // uptime counter is started when SSSP proceeds to operation phase
  _synctime = 0;
  aosTimestampSet(_uptime, 0);
#else /* (AMIROOS_CFG_SSSP_ENABLE == true) */
  // start the uptime counter
  chSysLock();
  aosSysStartUptimeS();
  chSysUnlock();
#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */

  /* initialize aos configuration */
  aosOStreamInit(&aos.iostream);
  chEvtObjectInit(&aos.events.gpio);
  chEvtObjectInit(&aos.events.os);

#if (AMIROOS_CFG_SHELL_ENABLE == true)

  /* init shell */
  aosShellInit();
  aosShellObjectInit(&aos.shell, _shell_name, moduleShellPrompt);
  // add system commands
  aosShellAddCommand(&_shellcmd_config);
  aosShellAddCommand(&_shellcmd_info);
  aosShellAddCommand(&_shellcmd_shutdown);
  aosShellAddCommand(&_shellcmd_memdump);
#if (((CH_CFG_USE_REGISTRY == TRUE) || (CH_CFG_USE_THREADHIERARCHY == TRUE)) && \
     (CH_DBG_STATISTICS == TRUE)) || defined(__DOXYGEN__)
  aosShellAddCommand(&_shellcmd_top);
#endif /* ((CH_CFG_USE_REGISTRY == TRUE) ||
           (CH_CFG_USE_THREADHIERARCHY == TRUE)) &&
          (CH_DBG_STATISTICS == TRUE) */
#if (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_KERNEL == true)
  aosShellAddCommand(&_shellcmd_kerneltest);
#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (AMIROOS_CFG_TESTS_ENABLE_KERNEL == true) */

#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */

  return;
}

/**
 * @brief   Starts the system and all system threads.
 */
void aosSysStart(void)
{
  // print system information;
  _aosSysPrintSystemInfo((BaseSequentialStream*)&aos.iostream, '=');
  aosprintf_module("\n");
  aosflush_module();

#if (AMIROOS_CFG_SHELL_ENABLE == true)
  // setup channels
#if defined(MODULE_HAL_SERIAL)
  aosShellChannelInit(&_stdshellchannel, (BaseAsynchronousChannel*)&MODULE_HAL_SERIAL, AOS_SHELLCHANNEL_INPUT | AOS_SHELLCHANNEL_OUTPUT);
  aosShellStreamAddChannel(&aos.shell.stream, &_stdshellchannel);
#endif /* defined(MODULE_HAL_SERIAL) */
#if defined(MODULE_SHELL_SETUP_HOOK)
  MODULE_SHELL_SETUP_HOOK();
#endif /* defined(MODULE_SHELL_SETUP_HOOK) */
  // start system shell thread
  aosShellStart(&aos.shell, AMIROOS_CFG_SHELL_THREADPRIO);
#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */

  return;
}

/**
 * @brief   Start the system uptime measurement.
 * @note    Must be called from a locked context.
 */
void aosSysStartUptimeS(void)
{
  chDbgCheckClassS();

  // start the uptime aggregation counter
  _synctime = chVTGetSystemTimeX();
  chVTSetI(&_systimer, SYSTIMER_PERIOD, &_aosSysUptimeCallback, NULL);
  aosTimestampSet(_uptime, 0);

  return;
}

/**
 * @brief   Retrieves the system uptime.
 *
 * @param[out] ut   The system uptime.
 */
void aosSysGetUptimeX(aos_timestamp_t* ut)
{
  aosDbgCheck(ut != NULL);

  aosTimestampSet(*ut, aosTimestampGet(_uptime) + chTimeI2US(chTimeDiffX(_synctime, chVTGetSystemTimeX())));

  return;
}

#if (HAL_USE_RTC == TRUE) || defined(__DOXYGEN__)

/**
 * @brief   retrieves the date and time from the MCU clock.
 *
 * @param[out] dt   The date and time.
 */
void aosSysGetDateTime(struct tm* dt)
{
  aosDbgCheck(dt != NULL);

  RTCDateTime rtc;
  rtcGetTime(&MODULE_HAL_RTC, &rtc);
  rtcConvertDateTimeToStructTm(&rtc, dt, NULL);

  return;
}

/**
 * @brief   set the date and time of the MCU clock.
 *
 * @param[in] dt    The date and time to set.
 */
void aosSysSetDateTime(struct tm* dt)
{
  aosDbgCheck(dt != NULL);

  RTCDateTime rtc;
  rtcConvertStructTmToDateTime(dt, 0, &rtc);
  rtcSetTime(&MODULE_HAL_RTC, &rtc);

  return;
}

#endif /* (HAL_USE_RTC == TRUE) */

/**
 * @brief   Initializes/Acknowledges a system shutdown/restart request.
 * @note    This functions should be called from the thread with highest
 *          priority.
 *
 * @param[in] shutdown    Type of shutdown.
 */
void aosSysShutdownInit(aos_shutdown_t shutdown)
{
  // check arguments
  aosDbgCheck(shutdown != AOS_SHUTDOWN_NONE);

#if (AMIROOS_CFG_SSSP_ENABLE == true)
  // prepare SSSP for shutdown
  aosSsspShutdownInit(shutdown != AOS_SHUTDOWN_PASSIVE);
#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */

  switch (shutdown) {
    case AOS_SHUTDOWN_NONE:
      // must never occur
      aosDbgAssert(false);
      break;
#if (AMIROOS_CFG_SSSP_ENABLE == true)
    case AOS_SHUTDOWN_PASSIVE:
      chEvtBroadcastFlags(&aos.events.os, AOS_SYSTEM_EVENTFLAGS_SHUTDOWN);
      aosprintf_module("shutdown request received...\n");
      break;
#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */
#if (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_NONE)
    case AOS_SHUTDOWN_ACTIVE:
      aosprintf_module("shutdown initiated...\n");
      break;
#endif /* (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_NONE) */
    case AOS_SHUTDOWN_RESTART:
      aosprintf_module("restarting system...\n");
      break;
#if (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_AMiRoBLT)
    case AOS_SHUTDOWN_HIBERNATE:
      aosprintf_module("shutdown to hibernate mode...\n");
      break;
    case AOS_SHUTDOWN_DEEPSLEEP:
      aosprintf_module("shutdown to deepsleep mode...\n");
      break;
    case AOS_SHUTDOWN_TRANSPORTATION:
      aosprintf_module("shutdown to transportation mode...\n");
      break;
#endif /* (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_AMiRoBLT) */
  }

  return;
}

/**
 * @brief   Stops the system and all related threads (not the thread this
 *          function is called from).
 *
 * @note    Only the control thread (detected by initial thread priority) may
 *          call this function.
 */
void aosSysStop(void)
{
  // only the control thread may call this function
  aosDbgAssert(chThdGetPriorityX() == AOS_THD_CTRLPRIO);

  // local variables
#if (CH_CFG_USE_REGISTRY == TRUE) || (CH_CFG_USE_THREADHIERARCHY == TRUE)
  tprio_t prio;
  thread_t* tp;
#endif /* (CH_CFG_USE_REGISTRY == TRUE) ||
          (CH_CFG_USE_THREADHIERARCHY == TRUE) */
#if (AMIROOS_CFG_DBG == true)
  aosTimestampDecl(tprint, 0);
  aos_timestamp_t tnow;
#endif /* (AMIROOS_CFG_DBG == true) */

  // wait until the calling thread is the only remaining active thread (besides the idle thread)
  while (ch_system.threadcount > 1 + (CH_CFG_NO_IDLE_THREAD != TRUE ? 1 : 0)) {

#if (AMIROOS_CFG_DBG == true)
    aosSysGetUptime(&tnow);
    // print a message once per second
    if (aosTimestampDiff(tprint, tnow) >= MICROSECONDS_PER_SECOND) {
      aosDbgPrintf("waiting for all threads to terminate (%u remaining)...\n", ch_system.threadcount-1);
      tprint = tnow;
    }
#endif /* (AMIROOS_CFG_DBG == true) */

#if (CH_CFG_USE_REGISTRY == TRUE) || (CH_CFG_USE_THREADHIERARCHY == TRUE)
    // detect highest priority of all threads but the calling one and the idle thread
    prio = NOPRIO;
    tp = aosThdGetFirst();
    while (tp) {
#if (CH_CFG_NO_IDLE_THREAD == TRUE)
      // skip the calling thread
      if (tp != chThdGetSelfX()) {
#else /* (CH_CFG_NO_IDLE_THREAD == TRUE) */
      // skip the calling thread and the idle thread
      if (tp != chThdGetSelfX() && tp != chSysGetIdleThreadX()) {
#endif /* (CH_CFG_NO_IDLE_THREAD == TRUE) */
#if (CH_CFG_USE_MUTEXES == TRUE)
        const tprio_t tprio = tp->realprio;
#else /* (CH_CFG_USE_MUTEXES == TRUE) */
        const tprio_t tprio = tp->prio;
#endif /* (CH_CFG_USE_MUTEXES == TRUE) */
        if (tprio > prio) {
          prio = tprio;
        }
      }
      tp = aosThdGetNext(tp);
    }
    aosDbgAssert(prio <= chThdGetPriorityX());

    // lower calling thread priority as required or yield CPU to another thread with equal priority
    if (prio != NOPRIO && prio < chThdGetPriorityX()) {
      chThdSetPriority(prio);
    } else {
      chThdYield();
    }
#else /* (CH_CFG_USE_REGISTRY == TRUE) ||
         (CH_CFG_USE_THREADHIERARCHY == TRUE) */
    // yield the CPU to lower priority threads
    chThdSetPriority(AOS_THD_LOWPRIO_MIN);
    chThdYield();
#endif /* (CH_CFG_USE_REGISTRY == TRUE) ||
          (CH_CFG_USE_THREADHIERARCHY == TRUE) */
  }

  // restore priority
  chThdSetPriority(AOS_THD_CTRLPRIO);

  // print a final message
  aosDbgPrintf("AMiRo-OS signing off. Goodbye!\n");
  aosDbgFlush();

  return;
}

/**
 * @brief   Deinitialize all system variables.
 */
void aosSysDeinit(void)
{
  return;
}

#if ((AMIROOS_CFG_SSSP_ENABLE == true) &&                                       \
     (AMIROOS_CFG_SSSP_SHUTDOWN != true)) ||                                    \
    ((AMIROOS_CFG_SSSP_ENABLE != true) &&                                       \
     (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_AMiRoBLT)) ||                    \
    defined(__DOXYGEN__)

/**
 * @brief   Finally shuts down the system and calls the bootloader callback
 *          function.
 * @note    This function should be called from the thtead with highest
 *          priority.
 *
 * @param[in] shutdown    Type of shutdown.
 */
void aosSysShutdownToBootloader(aos_shutdown_t shutdown)
{
  // check arguments
  aosDbgCheck(shutdown != AOS_SHUTDOWN_NONE);

  // disable all interrupts
  irqDeinit();

#if (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_AMiRoBLT)
  // validate AMiRo-BLT
  if ((BL_CALLBACK_TABLE_ADDRESS->magicNumber == BL_MAGIC_NUMBER) &&
      (BL_CALLBACK_TABLE_ADDRESS->vBootloader.major == BL_VERSION_MAJOR) &&
      (BL_CALLBACK_TABLE_ADDRESS->vBootloader.minor >= BL_VERSION_MINOR)) {
    // call bootloader callback depending on arguments
    switch (shutdown) {
#if (AMIROOS_CFG_SSSP_ENABLE == true)
      case AOS_SHUTDOWN_PASSIVE:
        BL_CALLBACK_TABLE_ADDRESS->cbHandleShutdownRequest();
        break;
#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */
      case AOS_SHUTDOWN_HIBERNATE:
        BL_CALLBACK_TABLE_ADDRESS->cbShutdownHibernate();
        break;
      case AOS_SHUTDOWN_DEEPSLEEP:
        BL_CALLBACK_TABLE_ADDRESS->cbShutdownDeepsleep();
        break;
      case AOS_SHUTDOWN_TRANSPORTATION:
        BL_CALLBACK_TABLE_ADDRESS->cbShutdownTransportation();
        break;
      case AOS_SHUTDOWN_RESTART:
        BL_CALLBACK_TABLE_ADDRESS->cbShutdownRestart();
        break;
      // must never occur
      case AOS_SHUTDOWN_NONE:
      default:
        break;
    }
  } else {
    // fallback if AMiRo-BLT was found to be invalid
    aosprintf_module("ERROR: AMiRo-BLT incompatible or not available!\n");
    aosflush_module();
    chSysDisable();
  }
#endif /* (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_AMiRoBLT) */

  return;
}

#endif /* ((AMIROOS_CFG_SSSP_ENABLE == true) &&
           (AMIROOS_CFG_SSSP_SHUTDOWN != true)) ||
          ((AMIROOS_CFG_SSSP_ENABLE != true) &&
           (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_AMiRoBLT)) */

/**
 * @brief   Generic callback function for GPIO interrupts.
 *
 * @return  Pointer to the callback function.
 */
palcallback_t aosSysGetStdGpioCallback(void)
{
  return _aosSysGpioCallback;
}

/** @} */
