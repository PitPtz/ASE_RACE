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
 * @file    urt_message.h
 * @brief   Message structure and interfaces.
 *
 * @defgroup pubsub_message Message
 * @ingroup pubsub
 * @brief   Messages act like buffers to pass information from publishers to
 *          subscribers.
 * @details Any information (i.e. payload data but also meta information) is
 *          transmitted using messages.
 *          Those buffer objects can be associated to a topic to increaser its
 *          queue depth as required.
 *
 * @addtogroup pubsub_message
 * @{
 */

#ifndef URT_MESSAGE_H
#define URT_MESSAGE_H

#include <urt.h>

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#if (URT_CFG_PUBSUB_ENABLED == true) || defined(__DOXYGEN__)
/**
 * @brief Value to indicate invalid (= yet unused) messages.
 */
#define URT_MESSAGEID_INVALID                   (urt_messageid_t)0
#endif /* URT_CFG_PUBSUB_ENABLED == true */

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

#if (URT_CFG_PUBSUB_PROFILING == true) || defined(__DOXYGEN__)
/**
 * @brief   Message profiling data structure
 */
typedef struct urt_message_profilingdata
{
  /**
   * @brief   Counter how often a message has been used to publish data.
   */
  urt_profilingcounter_t utilizations;

  /**
   * @brief   Counter of any consumers left, which have not processed the message yet.
   * @note    This is required to track the number of discarded messages at topic.
   */
  size_t numConsumersLeft;

} urt_message_profilingdata_t;
#endif /* (URT_CFG_UBSUB_PROFILING == true) */

/**
 * @brief   Type to uniquely identify messages.
 */
typedef size_t urt_messageid_t;

/**
 * @brief   Message structure to be passed by the publish-subscribe system.
 */
typedef struct urt_message
{
  /**
   * @brief   Pointer to the next message in a singly-linked, circular list.
   */
  struct urt_message* next;

  /**
   * @brief   Identifier to identify messages.
   * @details Messages identifiers are not globally unique but within a topic.
   *          The identifier is updated every time it is used to publish new data.
   *          A value of URT_MESSAGEID_INVALID indicates that the message has not been used yet.
   */
  urt_messageid_t id;

  /**
   * @brief   Origin time of the information passed by the message.
   * @details This time represents the age of the information (e.g. sensor data).
   *          All real-time constraints are checked against this time.
   */
  urt_osTime_t time;

  /**
   * @brief   Pointer to an optional payload of the message.
   */
  void* payload;

  /**
   * @brief   Size of the current payload in bytes.
   */
  size_t size;

  /**
   * @brief   Counter of HRT consumers left, which yet have to process the message.
   */
  size_t numHrtConsumersLeft;

#if (URT_CFG_PUBSUB_PROFILING == true) || defined(__DOXYGEN__)
  /**
   * @brief   Profiling data.
   */
  urt_message_profilingdata_t profiling;
#endif /* (URT_CFG_UBSUB_PROFILING == true) */

} urt_message_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void urtMessageInit(urt_message_t* message, void* payload);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
#endif /* URT_MESSAGE_H */

/** @} */
