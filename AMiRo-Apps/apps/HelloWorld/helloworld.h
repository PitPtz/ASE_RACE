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
 * @file    helloworld.h
 * @brief   A simple HelloWorld application.
 *
 * @defgroup apps_helloworld Hello World
 * @ingroup apps
 * @brief   todo
 * @details todo
 *
 * @addtogroup apps_helloworld
 * @{
 */

#ifndef HELLOWORLD_H
#define HELLOWORLD_H

#include <urt.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @brief   Event flag to identify trigger events related to basic message output.
 */
#define HELLOWORLD_TRIGGERFLAG_MESSAGE          (urt_osEventFlags_t)(1 << 0)

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)

/**
 * @brief   Event flag to identify trigger events related to publish-subscribe.
 */
#define HELLOWORLD_TRIGGERFLAG_PUBSUB           (urt_osEventFlags_t)(1 << 1)

#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

#if (URT_CFG_RPC_ENABLED == true) || defined(__DOXYGEN__)

/**
 * @brief   Event flag to identify trigger events related to remote procedure calls.
 */
#define HELLOWORLD_TRIGGERFLAG_RPC              (urt_osEventFlags_t)(1 << 2)

#endif /* (URT_CFG_RPC_ENABLED == true) */

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

#if ((URT_CFG_PUBSUB_ENABLED == true) || (URT_CFG_RPC_ENABLED == true)) || defined(__DOXYGEN__)
  float helloworldSrtUsefulness(urt_delay_t dt, void* params);
#endif /* (URT_CFG_PUBSUB_ENABLED == true) || (URT_CFG_RPC_ENABLED == true) */

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/******************************************************************************/
/* SUBSYSTEMS                                                                 */
/******************************************************************************/

#include <helloworld_master.h>
#include <helloworld_slave.h>

#endif /* HELLOWORLD_H */

/** @} */
