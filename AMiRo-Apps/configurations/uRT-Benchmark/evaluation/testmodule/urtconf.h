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
 * @file    urtconf.h
 * @brief   µRT configuration file for the NUCLEO-L476RG module.
 * @details Contains the module specific µRT settings.
 *
 * @addtogroup configs_urtbenchmark_modules_testmodule
 * @{
 */

#ifndef URTCONF_H
#define URTCONF_H

/*
 * compatibility guards
 */
#define _URT_CFG_
#define URT_CFG_VERSION_MAJOR                   1
#define URT_CFG_VERSION_MINOR                   2

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
#error URT_CFG_SYNCGROUPS_ENABLED not defined
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
#error URT_CFG_DEBUG_ENABLED not defined
#endif

/** @} */

/*===========================================================================*/
/**
 * @name OSAL parameters and configuration.
 * @{
 */
/*===========================================================================*/

/*
 * URT_CFG_OSAL_HEADER is defined globally in middleware.mk file.
 */

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
#error URT_CFG_PUBSUB_ENABLED not defined
#endif

/**
 * @brief   Width of the urt_topicid_t data type.
 *
 * @details Possible values are 8, 16, 32, and 64 bits.
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
#error URT_CFG_PUBSUB_PROFILING not defined
#endif

/**
 * @brief   Flag to enable deadline QoS for the publish-subscribe system.
 */
#if !defined(URT_CFG_PUBSUB_QOS_DEADLINECHECKS)
#error URT_CFG_PUBSUB_QOS_DEADLINECHECKS not defined
#endif

/**
 * @brief   Flag to enable rate QoS for the publish-subscribe system.
 */
#if !defined(URT_CFG_PUBSUB_QOS_RATECHECKS)
#error URT_CFG_PUBSUB_QOS_RATECHECKS not defined
#endif

/**
 * @brief   Flag to enable jitter QoS for the publish-subscribe system
 */
#if !defined(URT_CFG_PUBSUB_QOS_JITTERCHECKS)
#error URT_CFG_PUBSUB_QOS_JITTERCHECKS not defined
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
#error URT_CFG_RPC_ENABLED not defined
#endif

/**
 * @brief   Width of the urt_serviceid_t data type.
 *
 * @details Possible values are 8, 16, 32, and 64 bits.
 */
#if !defined(URT_CFG_RPC_SERVICEID_WIDTH)
#define URT_CFG_RPC_SERVICEID_WIDTH             16
#endif

/**
 * @brief   Flag to enable profiling of remote procedure calls.
 */
#if !defined(URT_CFG_RPC_PROFILING)
#error URT_CFG_RPC_PROFILING not defined
#endif

/**
 * @brief   Flag to enable deadline QoS for remote procedure calls.
 */
#if !defined(URT_CFG_RPC_QOS_DEADLINECHECKS)
#error URT_CFG_RPC_QOS_DEADLINECHECKS not defined
#endif

/**
 * @brief   Flag to enable jitter QoS for remote procedure calls.
 */
#if !defined(URT_CFG_RPC_QOS_JITTERCHECKS)
#error URT_CFG_RPC_QOS_JITTERCHECKS not defined
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
