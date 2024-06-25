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
 * @file    aos_iostream.c
 * @brief   Stream and channel code.
 * @details Implementation of the aos_ostrem and aos_iochannel.
 */

#include <amiroos.h>

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

/*
 * forward declarations
 */
static size_t _aosIOChannelWrite(void *instance, const uint8_t *bp, size_t n);
static size_t _aosIOChannelRead(void *instance, uint8_t *bp, size_t n);
static msg_t _aosIOChannelPut(void *instance, uint8_t b);
static msg_t _aosIOChannelGet(void *instance);
static msg_t _aosIOChannelPutt(void *instance, uint8_t b, sysinterval_t time);
static msg_t _aosIOChannelGett(void *instance, sysinterval_t time);
static size_t _aosIOChannelWritet(void *instance, const uint8_t *bp, size_t n, sysinterval_t time);
static size_t _aosIOChannelReadt(void *instance, uint8_t *bp, size_t n, sysinterval_t time);
static msg_t _aosIOChannelCtl(void *instance, unsigned int operation, void *arg);
static size_t _aosIOStreamWrite(void *instance, const uint8_t *bp, size_t n);
static msg_t _aosIOStreamPut(void *instance, uint8_t b);
static msg_t _aosIOStreamPutt(void *instance, uint8_t b, sysinterval_t time);
static size_t _aosIOStreamWritet(void *instance, const uint8_t *bp, size_t n, sysinterval_t time);
static msg_t _aosIOStreamCtl(void *instance, unsigned int operation, void *arg);

/**
 * @brief   Virtual methods table for all AosIOChannel objects.
 *
 * @ingroup core_iostream_iochannel
 */
static const struct AosIOChannelVMT _channelvmt = {
  (size_t) 0,
  _aosIOChannelWrite,
  _aosIOChannelRead,
  _aosIOChannelPut,
  _aosIOChannelGet,
  _aosIOChannelPutt,
  _aosIOChannelGett,
  _aosIOChannelWritet,
  _aosIOChannelReadt,
  _aosIOChannelCtl,
};

/**
 * @brief   Virtual methods table for all AosOStream objects.
 *
 * @ingroup core_iostream_ostream
 */
static const struct AosOStreamVMT _streamvmt = {
  (size_t) 0,
  _aosIOStreamWrite,
  NULL,
  _aosIOStreamPut,
  NULL,
  _aosIOStreamPutt,
  NULL,
  _aosIOStreamWritet,
  NULL,
  _aosIOStreamCtl,
};

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * @brief   Implementation of the BaseAsynchronousChannel write() method
 *          (inherited from BaseSequentialStream).
 */
static size_t _aosIOChannelWrite(void *instance, const uint8_t *bp, size_t n)
{
  if (((AosIOChannel*)instance)->flags & AOS_IOCHANNEL_OUTPUT) {
    return streamWrite(((AosIOChannel*)instance)->asyncchannel, bp, n);
  } else {
    return 0;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronousChannel read() method
 *          (inherited from BaseSequentialStream).
 */
static size_t _aosIOChannelRead(void *instance, uint8_t *bp, size_t n)
{
  n = streamRead(((AosIOChannel*)instance)->asyncchannel, bp, n);
  return (((AosIOChannel*)instance)->flags & AOS_IOCHANNEL_INPUT) ? n : 0;
}

/**
 * @brief   Implementation of the BaseAsynchronousChannel put() method
 *          (inherited from BaseSequentialStream).
 */
static msg_t _aosIOChannelPut(void *instance, uint8_t b)
{
  if (((AosIOChannel*)instance)->flags & AOS_IOCHANNEL_OUTPUT) {
    return streamPut(((AosIOChannel*)instance)->asyncchannel, b);
  } else {
    return MSG_RESET;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronousChannel get() method
 *          (inherited from BaseSequentialStream).
 */
static msg_t _aosIOChannelGet(void *instance)
{
  const msg_t m = streamGet(((AosIOChannel*)instance)->asyncchannel);
  return (((AosIOChannel*)instance)->flags & AOS_IOCHANNEL_INPUT) ? m : MSG_RESET;
}

/**
 * @brief   Implementation of the BaseAsynchronousChannel putt() method.
 */
static msg_t _aosIOChannelPutt(void *instance, uint8_t b, sysinterval_t time)
{
  if (((AosIOChannel*)instance)->flags & AOS_IOCHANNEL_OUTPUT) {
    return chnPutTimeout(((AosIOChannel*)instance)->asyncchannel, b, time);
  } else {
    return MSG_RESET;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronousChannel gett() method.
 */
static msg_t _aosIOChannelGett(void *instance, sysinterval_t time)
{
  const msg_t m = chnGetTimeout(((AosIOChannel*)instance)->asyncchannel, time);
  return (((AosIOChannel*)instance)->flags & AOS_IOCHANNEL_INPUT) ? m : MSG_RESET;
}

/**
 * @brief   Implementation of the BaseAsynchronousChannel writet() method.
 */
static size_t _aosIOChannelWritet(void *instance, const uint8_t *bp, size_t n, sysinterval_t time)
{
  if (((AosIOChannel*)instance)->flags & AOS_IOCHANNEL_OUTPUT) {
    return chnWriteTimeout(((AosIOChannel*)instance)->asyncchannel, bp, n, time);
  } else {
    return 0;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronousChannel readt() method.
 */
static size_t _aosIOChannelReadt(void *instance, uint8_t *bp, size_t n, sysinterval_t time)
{
  n = chnReadTimeout(((AosIOChannel*)instance)->asyncchannel, bp, n, time);
  return (((AosIOChannel*)instance)->flags & AOS_IOCHANNEL_INPUT) ? n : 0;
}

/**
 * @brief   Implementation of the BaseAsynchronousChannel ctl() method.
 */
static msg_t _aosIOChannelCtl(void *instance, unsigned int operation, void *arg)
{
  if (((AosIOChannel*)instance)->flags & (AOS_IOCHANNEL_OUTPUT | AOS_IOCHANNEL_INPUT)) {
    return chnControl(((AosIOChannel*)instance)->asyncchannel, operation, arg);
  } else {
    return MSG_OK;
  }
}

/**
 * @brief   Implementation of the BaseChannel write() method.
 */
static size_t _aosIOStreamWrite(void *instance, const uint8_t *bp, size_t n)
{
  aosDbgCheck(instance != NULL);

  // local variables
  AosIOChannel* channel = ((AosOStream*)instance)->channel;
  size_t bytes;
  size_t maxbytes = 0;

  // iterate through the list of channels
  while (channel != NULL) {
    bytes = streamWrite(channel, bp, n);
    maxbytes = (bytes > maxbytes) ? bytes : maxbytes;
    channel = channel->next;
  }

  return maxbytes;
}

/**
 * @brief   Implementation of the BaseChannel put() method.
 */
static msg_t _aosIOStreamPut(void *instance, uint8_t b)
{
  aosDbgCheck(instance != NULL);

  // local variables
  AosIOChannel* channel = ((AosOStream*)instance)->channel;
  msg_t ret = MSG_OK;

  // iterate through the list of channels
  while (channel != NULL) {
    msg_t ret_ = streamPut(channel, b);
    ret = (ret_ < ret) ? ret_ : ret;
    channel = channel->next;
  }

  return ret;
}

/**
 * @brief   Implementation of the BaseChannel putt() method.
 */
static msg_t _aosIOStreamPutt(void *instance, uint8_t b, sysinterval_t time)
{
  aosDbgCheck(instance != NULL);

  // local variables
  systime_t currt = chVTGetSystemTime();
  const systime_t callt = currt;
  const systime_t timeoutt = chTimeAddX(currt, time);
  AosIOChannel* channel = ((AosOStream*)instance)->channel;
  msg_t ret = MSG_OK;

  // iterate through list of channels
  while (channel != NULL && chTimeIsInRangeX(currt, callt, timeoutt)) {
    msg_t ret_ = chnPutTimeout(channel, b, chTimeDiffX(currt, timeoutt));
    ret = (ret_ < ret) ? ret_ : ret;
    channel = channel->next;
    currt = chVTGetSystemTime();
  }

  return (channel == NULL) ? ret : MSG_TIMEOUT;
}

/**
 * @brief   Implementation of the BaseChannel writet() method.
 */
static size_t _aosIOStreamWritet(void *instance, const uint8_t *bp, size_t n, sysinterval_t time)
{
  aosDbgCheck(instance != NULL);

  // local variables
  systime_t currt = chVTGetSystemTime();
  const systime_t callt = currt;
  const systime_t timeoutt = chTimeAddX(currt, time);
  AosIOChannel* channel = ((AosOStream*)instance)->channel;
  msg_t ret = MSG_OK;

  // iterate through list of channels
  while (channel != NULL && chTimeIsInRangeX(currt, callt, timeoutt)) {
    msg_t ret_ = chnWriteTimeout(channel, bp, n, chTimeDiffX(currt, timeoutt));
    ret = (ret_ < ret) ? ret_ : ret;
    channel = channel->next;
    currt = chVTGetSystemTime();
  }

  return (channel == NULL) ? ret : MSG_TIMEOUT;
}

/**
 * @brief   Implementation of the BaseChannel ctl() method.
 */
static msg_t _aosIOStreamCtl(void *instance, unsigned int operation, void *arg)
{
  aosDbgCheck(instance != NULL);

  // local variables
  AosIOChannel* channel = ((AosOStream*)instance)->channel;
  msg_t ret = MSG_OK;

  // iterate through list of channels
  while (channel != NULL) {
    msg_t ret_ = chnControl(channel, operation, arg);
    ret = (ret_ < ret) ? ret_ : ret;
    channel = channel->next;
  }

  return ret;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

/**
 * @addtogroup core_iostream_ostream
 * @{
 */

/**
 * @brief   Initializes an aos_iostream_t object.
 *
 * @param[in] stream    Th aos_iostream_t object to initialize.
 */
void aosOStreamInit(AosOStream* stream)
{
  aosDbgCheck(stream != NULL);

  stream->vmt = &_streamvmt;
  stream->channel = NULL;

  return;
}

/**
 * @brief   Initializes an aos_iostream_channel_t object.
 *
 * @param[in] channel       The aos_iostream_channel_t to initialize.
 * @param[in] asyncchannel  The BaseAsynchronousChannel object to associate with
 *                          the channel.
 * @param[in] flags         Flags to set to the AosIOChannel.
 *                          The AOS_IOCHANNEL_ATTACHED must not be set.
 */
void aosIOChannelInit(AosIOChannel *channel, BaseAsynchronousChannel *asyncchannel, aos_iochannelflags_t flags)
{
  aosDbgCheck(channel != NULL);
  aosDbgCheck(asyncchannel != NULL);
  aosDbgCheck((flags & AOS_IOCHANNEL_ATTACHED) == 0);

  channel->vmt = &_channelvmt;
  channel->asyncchannel = asyncchannel;
  channel->next = NULL;
  channel->flags = flags;

  return;
}

/**
 * @brief   Adds a channel to a stream.
 *
 * @param[in] stream    The stream to be expanded.
 * @param[in] channel   The channel to be added.
 */
void aosOStreamAddChannel(AosOStream *stream, AosIOChannel* channel)
{
  aosDbgCheck(stream != NULL);
  aosDbgCheck(channel != NULL && channel->asyncchannel != NULL && channel->next == NULL && (channel->flags & AOS_IOCHANNEL_ATTACHED) == 0);

  // prepend the new channel
  chSysLock();
  channel->flags |= AOS_IOCHANNEL_ATTACHED;
  channel->next = stream->channel;
  stream->channel = channel;
  chSysUnlock();

  return;
}

/**
 * @brief   Removes a channel from a stream.
 *
 * @param[in] stream    The stream to remove the channel from.
 * @param[in] channel   The channel to remove from the stream.
 *
 * @return  The operation result.
 * @retval AOS_SUCCESS  The channel was removed from the stream.
 * @retval AOS_ERROR    The channes is not associated to the stream.
 */
aos_status_t aosOStreamRemoveChannel(AosOStream* stream, AosIOChannel* channel)
{
  aosDbgCheck(stream != NULL);
  aosDbgCheck(channel != NULL);

  // local variables
  AosIOChannel* prev = NULL;
  AosIOChannel* curr = stream->channel;

  // iterate through the list and search for the specified channel
  while (curr != NULL) {
    // if the channel was found, remove it
    if (curr == channel) {
      chSysLock();
      // special case: the first channel matches (prev is NULL)
      if (prev == NULL) {
        stream->channel = curr->next;
      } else {
        prev->next = curr->next;
      }
      curr->next = NULL;
      curr->flags &= ~AOS_IOCHANNEL_ATTACHED;
      chSysUnlock();
      return AOS_SUCCESS;
    }
    // iterate to the next channel
    else {
      prev = curr;
      curr = curr->next;
    }
  }

  // if the channel was not found, return an error
  return AOS_ERROR;
}

/** @} */
