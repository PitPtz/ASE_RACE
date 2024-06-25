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
 * @file    aos_confcheck.h
 * @brief   Header that checks whether all necessary configurations are correct.
 */

#ifndef AMIROOS_CONFCHECK_H
#define AMIROOS_CONFCHECK_H

#include <aosconf.h>
#include <hal.h>
#include "bootloader/aos_bootloader.h"

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/* kernel parameters and options */

# if !defined(AMIROOS_CFG_DBG)
#   error "AMIROOS_CFG_DBG not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_DBG) */

# if !defined(AMIROOS_CFG_TESTS_ENABLE)
#   error "AMIROOS_CFG_TESTS_ENABLE not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_TESTS_ENABLE) */
# if !defined(AMIROOS_CFG_TESTS_ENABLE_KERNEL)
#   error "AMIROOS_CFG_TESTS_ENABLE_KERNEL not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_TESTS_ENABLE_KERNEL) */
# if !defined(AMIROOS_CFG_TESTS_ENABLE_PERIPHERY)
#   error "AMIROOS_CFG_TESTS_ENABLE_PERIPHERY not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_TESTS_ENABLE_PERIPHERY) */

# if !defined(AMIROOS_CFG_PROFILE)
#   error "AMIROOS_CFG_PROFILE not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_PROFILE) */

# if !defined(AMIROOS_CFG_MAIN_LOOP_TIMEOUT)
#   error "AMIROOS_CFG_MAIN_LOOP_TIMEOUT not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_MAIN_LOOP_TIMEOUT) */

/* bootloader parameters and options */

# if !defined(AMIROOS_CFG_BOOTLOADER)
#   error "AMIROOS_CFG_BOOTLOADER not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_BOOTLOADER)*/

/* Filtered & Buffered CAN (FBCAN) parameters and options */

# if !defined(AMIROOS_CFG_FBCAN1_ENABLE)
#   error "AMIROOS_CFG_FBCAN1_ENABLE not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_FBCAN1_ENABLE) */

# if !defined(AMIROOS_CFG_FBCAN2_ENABLE)
#   error "AMIROOS_CFG_FBCAN2_ENABLE not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_FBCAN2_ENABLE) */

# if !defined(AMIROOS_CFG_FBCAN3_ENABLE)
#   error "AMIROOS_CFG_FBCAN3_ENABLE not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_FBCAN3_ENABLE) */

#if (AMIROOS_CFG_FBCAN1_ENABLE == true) ||                                      \
    (AMIROOS_CFG_FBCAN2_ENABLE == true) ||                                      \
    (AMIROOS_CFG_FBCAN3_ENABLE == true)

# if (CAN_ENFORCE_USE_CALLBACKS == FALSE)
#   error "CAN_ENFORCE_USE_CALLBACKS must be TRUE when FBCAN is enbaled"
# endif /* (CAN_ENFORCE_USE_CALLBACKS == FALSE) */

# if !defined(AMIROOS_CFG_FBCAN_SERIAL_BUFFERSIZE)
#   error "AMIROOS_CFG_FBCAN_SERIAL_BUFFERSIZE not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_FBCAN_SERIAL_BUFFERSIZE) */

#endif /* (AMIROOS_CFG_FBCANX_ENABLE == true) */

/* SSSP parameters and options */

# if !defined(AMIROOS_CFG_SSSP_ENABLE)
#   error "AMIROOS_CFG_SSSP_ENABLE not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_SSSP_ENABLE) */

#if (AMIROOS_CFG_SSSP_ENABLE == true)

# if !defined(AMIROOS_CFG_SSSP_STARTUP)
#   error "AMIROOS_CFG_SSSP_STARTUP not defined in aosconf.h"
# else /* !defined(AMIROOS_CFG_SSSP_STARTUP) */
#   if (AMIROOS_CFG_SSSP_STARTUP == true)
#     if (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_AMiRoBLT)
#       error "AMIROOS_CFG_SSSP_STARTUP is enabled in aosconf.h, but SSSP startup phase is already handled by AMiRo-BLT."
#     endif /* (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_AMiRoBLT) */
#   else /* (AMIROOS_CFG_SSSP_STARTUP == true) */
#     if (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_NONE)
#       error "AMIROOS_CFG_SSSP_STARTUP is disabled in aosconf.h, but no bootloader is defined to handle SSSP startup phase instead."
#     endif /* (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_NONE) */
#   endif /* (AMIROOS_CFG_SSSP_STARTUP == true) */
# endif /* !defined(AMIROOS_CFG_SSSP_STARTUP) */

# if !defined(AMIROOS_CFG_SSSP_SHUTDOWN)
#   error "AMIROOS_CFG_SSSP_SHUTDOWN not defined in aosconf.h"
# else /* !defined(AMIROOS_CFG_SSSP_SHUTDOWN) */
#   if (AMIROOS_CFG_SSSP_SHUTDOWN != true)
#     if (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_NONE)
#       error "AMIROOS_CFG_SSSP_SHUTDOWN is disabled in aosconf.h, but no bootloader is defined to handle SSSP shutdown phase instead."
#     endif /* (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_NONE) */
#   endif /* (AMIROOS_CFG_SSSP_SHUTDOWN != true) */
# endif /* !defined(AMIROOS_CFG_SSSP_SHUTDOWN) */

# if !defined(AMIROOS_CFG_SSSP_MSI)
#   error "AMIROOS_CFG_SSSP_MSI not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_SSSP_MSI) */

# if !defined(AMIROOS_CFG_SSSP_MODULEIDWIDTH)
#   error "AMIROOS_CFG_SSSP_MODULEIDWIDTH not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_SSSP_MODULEIDWIDTH) */

# if !defined(AMIROOS_CFG_SSSP_MASTER)
#   error "AMIROOS_CFG_SSSP_MASTER not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_SSSP_MASTER) */

# if !defined(AMIROOS_CFG_SSSP_STACK_START)
#   error "AMIROOS_CFG_SSSP_STACK_START not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_SSSP_STACK_START) */

# if !defined(AMIROOS_CFG_SSSP_STACK_END)
#   error "AMIROOS_CFG_SSSP_STACK_END not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_SSSP_STACK_END) */

# if (AMIROOS_CFG_SSSP_STACK_START == true) &&                                  \
     (AMIROOS_CFG_SSSP_STACK_END == true)
#   warning "AMIROOS_CFG_SSSP_STACK_START and AMIROOS_CFG_SSSP_STACK_END both enabled in aosconf.h"
#   if (AMIROOS_CFG_SSSP_MASTER != true)
#     error "AMIROOS_CFG_SSSP_MASTER must be enabled in this case"
#   endif /* (AMIROOS_CFG_SSSP_MASTER != true) */
# endif /* (AMIROOS_CFG_SSSP_STACK_START == true) &&
           (AMIROOS_CFG_SSSP_STACK_END == true) */

# if !defined(AMIROOS_CFG_SSSP_SIGNALDELAY)
#   error "AMIROOS_CFG_SSSP_SIGNALDELAY not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_SSSP_SIGNALDELAY) */

# if !defined(AMIROOS_CFG_SSSP_SYSSYNCPERIOD)
#   error "AMIROOS_CFG_SSSP_SYSSYNCPERIOD not defined in aosconf.h"
# elif AMIROOS_CFG_SSSP_SYSSYNCPERIOD < 0
#    error "AMIROOS_CFG_SSSP_SYSSYNCPERIOD must be greater or equal 0"
# endif /* !defined(AMIROOS_CFG_SSSP_SYSSYNCPERIOD) */

#else /* (AMIROOS_CFG_SSSP_ENABLE == true) */

# if (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_AMiRoBLT)
#   warning "SSSP is disabled in aosconf.h, but AMiRo-BLT implements it nevertheless."
# endif /* (AMIROOS_CFG_BOOTLOADER == AOS_BOOTLOADER_AMiRoBLT) */

#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */

/* system shell options */

# if !defined(AMIROOS_CFG_SHELL_ENABLE)
#   error "AMIROOS_CFG_SHELL_ENABLE not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_SHELL_ENABLE) */

#if (AMIROOS_CFG_SHELL_ENABLE == true)

# if !defined(AMIROOS_CFG_SHELL_STACKSIZE)
#   error "AMIROOS_CFG_SHELL_STACKSIZE not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_SHELL_STACKSIZE) */

# if !defined(AMIROOS_CFG_SHELL_THREADPRIO)
#   error "AMIROOS_CFG_SHELL_THREADPRIO not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_SHELL_THREADPRIO) */

# if !defined(AMIROOS_CFG_SHELL_LINEWIDTH)
#   error "AMIROOS_CFG_SHELL_LINEWIDTH not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_SHELL_LINEWIDTH) */

# if !defined(AMIROOS_CFG_SHELL_MAXARGS)
#   error "AMIROOS_CFG_SHELL_MAXARGS not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_SHELL_MAXARGS) */

# if !defined(AMIROOS_CFG_SHELL_HISTLENGTH)
#   error "AMIROOS_CFG_SHELL_HISTLENGTH not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_SHELL_HISTLENGTH) */

# if !defined(AMIROOS_CFG_SHELL_REMOTE_ENABLE)
#   error "AMIROOS_CFG_SHELL_REMOTE_ENABLE not defined in aosconf.h"
# endif /* !defined(AMIROOS_CFG_SHELL_REMOTE_ENABLE) */

# else /* (AMIROOS_CFG_SHELL_ENABLE == true) */

# if (AMIROOS_CFG_TESTS_ENABLE == true)
#   error "Enabling AMIROOS_CFG_TESTS_ENABLE in aosconf.h requires AMIROOS_CFG_SHELL_ENABLE to be enabled as well."
# endif /* (AMIROOS_CFG_TESTS_ENABLE == true) */

#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROOS_CONFCHECK_H */
