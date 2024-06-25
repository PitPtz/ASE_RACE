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
 * @file    urtconf.h
 * @brief   µRT configuration file.
 * @details Contains the configuration specific µRT settings.
 * 
 * @addtogroup config
 * @{
 */


#ifndef URTCONF_H
#define URTCONF_H

/*
 * compatibility guards
 */

/**
 * @brief   Flag to detect availablility of µRT.
 */
#define _URT_CFG_

/**
 *@brief   Major version of µRT.
 */
#define URT_CFG_VERSION_MAJOR                   1

/**
 *@brief   Minor version of µRT.
 */
#define URT_CFG_VERSION_MINOR                   0

#include <stdbool.h>

/*===========================================================================*/
/**
 * @name General parameters and options.
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Width of the urt_delay_t data type.
 *
 * @details Possible values are 32 and 64 bits.
 *          By definition time is represented in microseconds.
 */
#if !defined(URT_CFG_DELAY_WIDTH)
#define URT_CFG_DELAY_WIDTH                     32
#endif

/**
 * @brief   Width of the urt_nodestage_t data type.
 *
 * @details Possible values are 8, 16, 32, and 64 bits.
 */
#if !defined(URT_CFG_NODESTAGE_WIDTH)
#define URT_CFG_NODESTAGE_WIDTH                 16
#endif

/**
 * @brief   Synchronization groups API enable flag.
 */
#if !defined(URT_CFG_SYNCGROUPS_ENABLED)
#define URT_CFG_SYNCGROUPS_ENABLED              true
#endif

/** @} */

/*===========================================================================*/
/**
 * @name General debug options.
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Debug enable flag.
 */
#if !defined(URT_CFG_DEBUG_ENABLED)
#define URT_CFG_DEBUG_ENABLED                   true
#endif

/** @} */

/*===========================================================================*/
/**
 * @name OSAL parameters and configuration.
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Hook macro to insert a custom header to urt_osal.h file.
 */
#define URT_CFG_OSAL_HEADER                     <custom_urtosal.h>

/**
 * @brief   Flag to enable timeout functionality for condition variables.
 * @details If the OS supports this feature, urtPublisherPublish() can be called
            with a timeout. Otherwise it will block indefinitely under certain
            conditions.
 */
#if !defined(URT_CFG_OSAL_CONDVAR_TIMEOUT)
#define URT_CFG_OSAL_CONDVAR_TIMEOUT            false
#endif

/** @} */

/*===========================================================================*/
/**
 * @name Publish-subscribe (pubsub) configuration.
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Enable flag for the publish-subscribe system.
 */
#if !defined(URT_CFG_PUBSUB_ENABLED)
#define URT_CFG_PUBSUB_ENABLED                  true
#endif

/**
 * @brief   Width of the urt_topicid_t data type.
 *
 * @details Possible value range is [1,64] bits.
 */
#if !defined(URT_CFG_PUBSUB_TOPICID_WIDTH)
#define URT_CFG_PUBSUB_TOPICID_WIDTH            16
#endif

/**
 * @brief   Method how messages are organized at topics.
 *
 * @details Possible values are:
 *          - URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME
 *            Messages are ordered by their timestamps.
 *            Potentially linear computational complexity, but correct order.
 *          - URT_PUBSUB_TOPIC_BUFFERORDER_PUBTIME
 *            Messages are ordered by the times they are published.
 *            Constant computational complexity, but potentially "wrong" order.
 */
#if !defined(URT_CFG_PUBSUB_TOPIC_BUFFERORDER)
#define URT_CFG_PUBSUB_TOPIC_BUFFERORDER        URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME
#endif

/**
 * @brief   Flag to enable profiling of the publish-subscribe system.
 */
#if !defined(URT_CFG_PUBSUB_PROFILING)
#define URT_CFG_PUBSUB_PROFILING                true
#endif

/**
 * @brief   Flag to enable deadline QoS for the publish-subscribe system.
 */
#if !defined(URT_CFG_PUBSUB_QOS_DEADLINECHECKS)
#define URT_CFG_PUBSUB_QOS_DEADLINECHECKS       true
#endif

/**
 * @brief   Flag to enable rate QoS for the publish-subscribe system.
 */
#if !defined(URT_CFG_PUBSUB_QOS_RATECHECKS)
#define URT_CFG_PUBSUB_QOS_RATECHECKS           true
#endif

/**
 * @brief   Flag to enable jitter QoS for the publish-subscribe system
 */
#if !defined(URT_CFG_PUBSUB_QOS_JITTERCHECKS)
#define URT_CFG_PUBSUB_QOS_JITTERCHECKS         true
#endif

/** @} */

/*===========================================================================*/
/**
 * @name Remote procedure calls (RPC) configuration.
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Enable flag for remote procedure calls.
 */
#if !defined(URT_CFG_RPC_ENABLED)
#define URT_CFG_RPC_ENABLED                     true
#endif

/**
 * @brief   Width of the urt_serviceid_t data type.
 *
 * @details Possible value range is [1,64] bits.
 */
#if !defined(URT_CFG_RPC_SERVICEID_WIDTH)
#define URT_CFG_RPC_SERVICEID_WIDTH             16
#endif

/**
 * @brief   Flag to enable profiling of remote procedure calls.
 */
#if !defined(URT_CFG_RPC_PROFILING)
#define URT_CFG_RPC_PROFILING                   true
#endif

/**
 * @brief   Flag to enable deadline QoS for remote procedure calls.
 */
#if !defined(URT_CFG_RPC_QOS_DEADLINECHECKS)
#define URT_CFG_RPC_QOS_DEADLINECHECKS          true
#endif

/**
 * @brief   Flag to enable jitter QoS for remote procedure calls.
 */
#if !defined(URT_CFG_RPC_QOS_JITTERCHECKS)
#define URT_CFG_RPC_QOS_JITTERCHECKS            true
#endif

/** @} */

/*===========================================================================*/
/**
 * @name Profiling configuration.
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Width of all profiling counters.
 *
 * @details Possible values are 8, 16, 32, and 64 bits.
 */
#if !defined(URT_CFG_PROFILING_COUNTER_WIDTH)
#define URT_CFG_PROFILING_COUNTER_WIDTH         64
#endif

/** @} */

#endif /* URTCONF_H */

/** @} */
