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
 * @file    urt_message.c
 * @brief   Message code.
 */

#include <urt.h>

#if (URT_CFG_PUBSUB_ENABLED == true || defined(__DOXYGEN__))

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED VARIABLES                                                         */
/******************************************************************************/

/******************************************************************************/
/* LOCAL TYPES                                                                */
/******************************************************************************/

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

/**
 * @addtogroup pubsub_message
 * @{
 */

/**
 * @brief   Initalize the messages.
 *
 * @param[in] message   Message to initalize.
 *                      Must not be NULL.
 * @param[in] payload   May be NULL for messages without payload.
 */
void urtMessageInit(urt_message_t* message, void* payload)
{
  urtDebugAssert(message != NULL);

  // initialize message data
  message->next = NULL;
  message->id = URT_MESSAGEID_INVALID;
  urtTimeSet(&message->time, 0);
  message->payload = payload;
  message->size = 0;
  message->numHrtConsumersLeft = 0;
#if (URT_CFG_PUBSUB_PROFILING  == true)
  message->profiling.utilizations = 0;
  message->profiling.numConsumersLeft = 0;
#endif /* (URT_CFG_PUBSUB_PROFILING == true) */

  return;
}

/** @} */

#endif /* (URT_CFG_PUBSUB_ENABLED == true) */
