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
 * @file    urt_confoptions.h
 * @brief   Header containing several non-trivial configuration options.
 *
 * @defgroup config_options Options
 * @ingroup config
 * @brief   Selectable options for several feature flags.
 * @details Some feature flags are neither boolean, nor numerical, but are to
 *          be selected of several options. All available options fur these
 *          flags are specified here.
 *
 * @addtogroup config_options
 * @{
 */

#ifndef URT_CONFOPTIONS_H
#define URT_CONFOPTIONS_H

#include <stdbool.h>

/*============================================================================*/
/* CONSTANTS                                                                  */
/*============================================================================*/

/**
 * @brief   Compile setting configuring topics to order messages by their
 *          timestamps.
 *
 * @note    Option for the URT_CFG_PUBSUB_TOPIC_BUFFERORDER setting.
 *
 * @details Potentially linear computational complexity, but correct order.
 *
 */
#define URT_PUBSUB_TOPIC_BUFFERORDER_MSGTIME    1

/**
 * @brief   Compile setting configuring topics to order messages by the times
 *          they are published.
 *
 * @note    Option for the URT_CFG_PUBSUB_TOPIC_BUFFERORDER setting.
 *
 * @details Constant computational complexity, but potentially "wrong" order.
 */
#define URT_PUBSUB_TOPIC_BUFFERORDER_PUBTIME    2

/*============================================================================*/
/* SETTINGS                                                                   */
/*============================================================================*/

/*============================================================================*/
/* CHECKS                                                                     */
/*============================================================================*/

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

#endif /* URT_CONFOPTIONS_H */

/** @} */
