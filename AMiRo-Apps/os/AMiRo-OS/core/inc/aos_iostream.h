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
 * @file    aos_iostream.h
 * @brief   Stream and channel macros and structures.
 *
 * @defgroup core_iostream_iochannel Channel
 * @ingroup core_iostream
 * @brief   Todo
 * @details Todo
 *
 * @defgroup core_iostream_ostream Output Stream
 * @ingroup core_iostream
 * @brief   Todo
 * @details Todo
 */

#ifndef AMIROOS_IOSTREAM_H
#define AMIROOS_IOSTREAM_H

#include <stdint.h>
#include <hal.h>
#include "aos_types.h"

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/**
 * @addtogroup core_iostream_iochannel
 * @{
 */

/**
 * @brief   Channel flag to indicate whether the channel is attached to a
 *          stream.
 */
#define AOS_IOCHANNEL_ATTACHED                  ((aos_iochannelflags_t)(1 << 0))

/**
 * @brief   Channel flag to indicate whether the channel is set as input.
 */
#define AOS_IOCHANNEL_INPUT                     ((aos_iochannelflags_t)(1 << 1))

/**
 * @brief   Channel flag to indicate whether the channel is set as output.
 */
#define AOS_IOCHANNEL_OUTPUT                    ((aos_iochannelflags_t)(1 << 2))

/** @} */

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
 * @addtogroup core_iostream_iochannel
 * @{
 */

/**
 * @brief   AosIOChannel flags type.
 */
typedef uint8_t aos_iochannelflags_t;

/** @} */

/**
 * @brief   AosIOChannel specific methods.
 */
#define _aos_iochannel_methods                                                  \
  _base_asynchronous_channel_methods

/**
 * @brief   AosIOChannel specific data.
 */
#define _aos_iochannel_data                                                     \
  /* pointer to a BaseAsynchronousChannel object */                             \
  BaseAsynchronousChannel* asyncchannel;                                        \
  /* pointer to the next channel in a AosOStream */                             \
  struct aos_iochannel* next;                                                   \
  /* flags related to the channel */                                            \
  aos_iochannelflags_t flags;

/**
 * @addtogroup core_iostream_iochannel
 * @{
 */

/**
 * @extends BaseAsynchronousChannelVMT
 *
 * @brief   AosIOChannel virtual methods table.
 */
struct AosIOChannelVMT {
  _aos_iochannel_methods
};

/**
 * @extends BaseAsynchronousChannel
 *
 * @brief   I/O Channel class.
 * @details This class implements an asynchronous I/O channel.
 */
typedef struct aos_iochannel {
  /** @brief Virtual Methods Table. */
  const struct AosIOChannelVMT* vmt;
  _aos_iochannel_data
} AosIOChannel;

/** @} */

/**
 * @brief   AosOStream methods.
 */
#define _aos_ostream_methods                                                    \
  _base_channel_methods

/**
 * @brief   AosOStream data.
 */
#define _aos_ostream_data                                                       \
  _base_channel_data                                                            \
  /* Pointer to the first channel in a list. */                                 \
  AosIOChannel* channel;

/**
 * @addtogroup core_iostream_ostream
 * @{
 */

/**
 * @extends BaseChannel
 *
 * @brief   AosOStream virtual methods table.
 */
struct AosOStreamVMT {
  _aos_ostream_methods
};

/**
 * @extends BaseChannel
 *
 * @brief   Output Stream class.
 * @details This class implements a base channel, but only supports output.
 */
typedef struct aos_ostream {
  const struct AosOStreamVMT* vmt;  /**< Pointer to a virtual methods table. */
  _aos_ostream_data
} AosOStream;

/** @} */

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void aosOStreamInit(AosOStream* stream);
  void aosIOChannelInit(AosIOChannel* channel, BaseAsynchronousChannel* asyncchannel, aos_iochannelflags_t flags);
  void aosOStreamAddChannel(AosOStream* stream, AosIOChannel* channel);
  aos_status_t aosOStreamRemoveChannel(AosOStream* stream, AosIOChannel* channel);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/**
 * @addtogroup core_iostream_ostream
 * @{
 */

/**
 * @brief   Flushs a stream output.
 *
 * @param[in] stream    The stream to flush.
 *
 * @return  The operation result.
 * @retval AOS_OK     The stream was successfully flushed.
 * @retval AOS_ERROR  Flushing the streamn failed.
 */
static inline aos_status_t aosOStreamFlush(AosOStream *stream)
{
  return (chnControl((BaseChannel*)stream, CHN_CTL_TX_WAIT, NULL) == MSG_OK) ? AOS_OK : AOS_ERROR;
}

/** @} */

/**
 * @addtogroup core_iostream_iochannel
 * @{
 */

/**
 * @brief   Enable input for a AosIOChannel.
 *
 * @param[in] channel   The AosIOChannel to enable as input.
 */
static inline void aosIOChannelInputEnable(AosIOChannel* channel)
{
  channel->flags |= AOS_IOCHANNEL_INPUT;
  return;
}

/**
 * @brief   Disable input for a AosIOChannel.
 *
 * @param[in] channel   The AosIOChannel to disable as input.
 */
static inline void aosIOChannelInputDisable(AosIOChannel* channel)
{
  channel->flags &= ~AOS_IOCHANNEL_INPUT;
  return;
}

/**
 * @brief   Enable output for a AosIOChannel.
 *
 * @param[in] channel   The AosIOChannel to enable as output.
 */
static inline void aosIOChannelOutputEnable(AosIOChannel* channel)
{
  channel->flags |= AOS_IOCHANNEL_OUTPUT;
  return;
}

/**
 * @brief   Disable output for a AosIOChannel.
 *
 * @param[in] channel   The AosIOChannel to disable as output.
 */
static inline void aosIOChannelOutputDisable(AosIOChannel* channel)
{
  channel->flags &= ~AOS_IOCHANNEL_OUTPUT;
  return;
}

/** @} */

#endif /* AMIROOS_IOSTREAM_H */
