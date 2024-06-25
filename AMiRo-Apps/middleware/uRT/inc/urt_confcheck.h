/*
µRT is a lightweight publish-subscribe & RPC middleware for real-time
applications. It was developed as part of the software habitat for the
Autonomous Mini Robot (AMiRo) but can be used for other purposes as well.

Copyright (C) 2018..2022  Thomas Schöpping et al.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    urt_confcheck.h
 * @brief   Header that checks whether all necessary configurations are correct.
 *
 * @defgroup config Configuration
 * @brief   µRT is configured via several feature flags and settings.
 * @details Flags can be used to deactivate entire subsystems (e.g. publish-
 *          subscribe or RPC), but also to configure those subsystems in detail.
 *          Feature flags and settings are evaluated at build time to allow for
 *          high level of optimization by compiler and linker.
 *
 * @addtogroup config
 * @{
 */

#ifndef URT_CONFCHECK_H
#define URT_CONFCHECK_H

#include <urt.h>

/*============================================================================*/
/* CONSTANTS                                                                  */
/*============================================================================*/

/*============================================================================*/
/* SETTINGS                                                                   */
/*============================================================================*/

/*============================================================================*/
/* CHECKS                                                                     */
/*============================================================================*/

/* general parameters and options */

#if !defined(URT_CFG_DELAY_WIDTH)
# error "URT_CFG_DELAY_WIDTH not defined in urtconf.h"
#endif

#if !defined(URT_CFG_NODESTAGE_WIDTH)
# error "URT_CFG_NODESTAGE_WIDTH not defined in urtconf.h"
#endif

#if !defined(URT_CFG_DEBUG_ENABLED)
# error "URT_CFG_DEBUG_ENABLED not defined in urtconf.h"
#endif

#if !defined(URT_CFG_OSAL_CONDVAR_TIMEOUT)
# error "URT_CFG_OSAL_CONDVAR_TIMEOUT not defined in urtconf.h"
#endif

#if !defined(URT_CFG_SYNCGROUPS_ENABLED)
# error "URT_CFG_SYNCGROUPS_ENABLED not defined in urtconf.h"
#endif

/* publish-subscribe parameters and options */

#if !defined(URT_CFG_PUBSUB_ENABLED)
# error "URT_CFG_PUBSUB_ENABLED not defined in urtconf.h"
#endif

#if (URT_CFG_PUBSUB_ENABLED == true)

# if !defined(URT_CFG_PUBSUB_TOPICID_WIDTH)
#   error "URT_CFG_PUBSUB_TOPICID_WIDTH not defined in urtconf.h"
# endif

# if !defined(URT_CFG_PUBSUB_TOPIC_BUFFERORDER)
#   error "URT_CFG_PUBSUB_TOPIC_BUFFERORDER not defined in urtconf.h"
# endif

# if !defined(URT_CFG_PUBSUB_PROFILING)
#   error "URT_CFG_PUBSUB_PROFILING not defined in urtconf.h"
# endif

# if !defined(URT_CFG_PUBSUB_QOS_DEADLINECHECKS)
#   error "URT_CFG_PUBSUB_QOS_DEADLINECHECKS not defined in urtconf.h"
# endif

# if !defined(URT_CFG_PUBSUB_QOS_RATECHECKS)
#   error "URT_CFG_PUBSUB_QOS_RATECHECKS not defined in urtconf.h"
# endif

# if !defined(URT_CFG_PUBSUB_QOS_JITTERCHECKS)
#   error "URT_CFG_PUBSUB_QOS_JITTERCHECKS not defined in urtconf.h"
# endif

#endif

/* remote procedure call parameters and options */

#if !defined(URT_CFG_RPC_ENABLED)
# error "URT_CFG_RPC_ENABLED not defined in urtconf.h"
#endif

#if (URT_CFG_RPC_ENABLED == true)

# if !defined(URT_CFG_RPC_SERVICEID_WIDTH)
#   error "URT_CFG_RPC_SERVICEID_WIDTH not defined in urtconf.h"
# endif

# if !defined(URT_CFG_RPC_PROFILING)
#   error "URT_CFG_RPC_PROFILING not defined in urtconf.h"
# endif

# if !defined(URT_CFG_RPC_QOS_DEADLINECHECKS)
#   error "URT_CFG_RPC_QOS_DEADLINECHECKS not defined in urtconf.h"
# endif

# if !defined(URT_CFG_RPC_QOS_JITTERCHECKS)
#   error "URT_CFG_RPC_QOS_JITTERCHECKS not defined in urtconf.h"
# endif

#endif /* URT_CFG_RPC_ENABLED == true */

/* profiling options */

#if ((URT_CFG_PUBSUB_ENABLED == true) && (URT_CFG_PUBSUB_PROFILING == true)) || \
    ((URT_CFG_RPC_ENABLED == true) && (URT_CFG_RPC_PROFILING == true))

# if !defined(URT_CFG_PROFILING_COUNTER_WIDTH)
#   error "URT_CFG_PROFILING_COUNTER_WIDTH not defined in urtconf.h"
# endif

#endif /* ((URT_CFG_PUBSUB_ENABLED == true) && (URT_CFG_PUBSUB_PROFILING == true)) || ((URT_CFG_RPC_ENABLED == true) && (URT_CFG_RPC_PROFILING == true)) */

/*============================================================================*/
/* DATA STRUCTURES AND TYPES                                                  */
/*============================================================================*/

/*============================================================================*/
/* MACROS                                                                     */
/*============================================================================*/

/*============================================================================*/
/* EXTERN DECLARATIONS                                                        */
/*============================================================================*/

/*============================================================================*/
/* INLINE FUNCTIONS                                                           */
/*============================================================================*/

#endif /* URT_CONFCHECK_H */

/** @} */
