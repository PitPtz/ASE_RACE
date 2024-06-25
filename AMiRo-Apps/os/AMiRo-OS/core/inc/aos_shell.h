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
 * @file    aos_shell.h
 * @brief   Shell macros and structures.
 *
 * @defgroup core_shell_shellchannel Shell Channel
 * @ingroup core_shell
 * @brief   Todo
 * @details Todo
 *
 * @defgroup core_shell_shellstream Shell Stream
 * @ingroup core_shell
 * @brief   Todo
 * @details Todo
 *
 * @defgroup core_shell_shellremote Remote
 * @ingroup core_shell
 * @brief   Todo
 * @details Todo
 */

#ifndef AMIROOS_SHELL_H
#define AMIROOS_SHELL_H

#include <aosconf.h>

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)

#include <stdint.h>
#include <hal.h>
#include "aos_timer.h"
#include "aos_types.h"

#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) || defined(__DOXYGEN__)
/**
 * @page shellremoteprotocol Remote Access Control Protocol
 * @ingroup core_shell_shellremote
 *
 * The remote access control protocol is a very light-weight protocol for
 * simple, cooperative remote shell access and comprises commands:
 * - discover request: ['ENQ' '?']\n
 *   The emitting shell requests all shells connected to a channel to name their
 *   individual identifiers.
 * - discover response: ['ENQ' '!' identifier]\n
 *   Answering a discover request, a shell broadcasts its identifier via the
 *   channel on which the request was received.
 * - client request: ['ENQ' '#']\n
 *   The emitting shell requests the client on the channel to name its
 *   individual identifier.
 * - client response: ['ENQ' '~' identifier]\n
 *   Answering a client request, the shell broadcasts its identifier via the
 *   channel on which the request was received.
 * - connect: ['ENQ' '@' identifier]\n
 *   A shell requests a connection to another remote shell. As soon as that
 *   server acknowledges, the requesting shell becomes either the client (if it
 *   was neither server nor client before) or disconnected. The requesting shell
 *   furthermore has to cancel the request on all other channels (see below). If
 *   the target shell does not reply in time, the requesting shell must send a
 *   cancelation message (see below). Any shell can request a remote connection
 *   to any other shell at any time.
 * - acknowledge: ['ENQ' '+']\n
 *   A shell that has been requested as server for a remote connection
 *   acknowledges this request. It immediately becomes the server and the
 *   requesting shell becomes the client.
 * - cancel: ['ENQ' '-']\n
 *   All shells currently active on the channel (server or client) must close
 *   this connection immediately. Cancellation messages are transmitted whenever
 *   some error occurred or a connection is disconnected by the user.
 * - disable: ['ENQ' '\']\n
 *   The channel on which this message was received must be closed and disabled
 *   for further remote communication. Disable messages are transmitted if a
 *   shell detects a duplicate identifier.
 *
 * The protocol as described above allows to discover all shells on a channel,
 * e.g. in order to list all available shells for the user. Channels in this
 * case are assumed to be independant communication interfaces to which a shell
 * is connected to for serial user I/O (e.g. UART, SPI, CAN). There must be no
 * more than a single remote connection per channel.
 *
 * @par Security
 * Note that this protocol is indeed very simple and not secured against
 * malicious interventions in any form. As soon as a remote connection has been
 * established, the actual data exchanged between shells can be encrypted,
 * though (not implemented).
 *
 * @par Examples
 * Let A, B, and C be shells; let X and Y be channels.
 * - list remotes\n
 *   1. A emits a discover request via X and Y and sets an internal timeout.
 *   2. B responds quickly, so A lists its identifier to the user.
 *   3. The timeout expires and A returns to normal operation.
 *   4. C responds late, so its message has no effect.
 * - connection request timeout\n
 *   1. A requests a remote connection to B and sets an internal timeout.
 *   2. B does not respond in time, so A cancels the request by emmiting a
 *      cancelation message.
 *   3. Due to internal delays, B has sent an acknowledgement (but too late)
 *      before it detects the cancellation message. While it had established the
 *      connection internaly for a brief moment, it has to undo this state as
 *      soon as the cancelation message has been processed.
 * - establishing a connection\n
 *   1. A requests a remote connection to B and sets an internal timeout.
 *   2. B acknowledges quickly, so both, A and B, update their internal values.
 *      A also resets the timeout. The connection has been established with A
 *      being the client and B the server.
 * - traversal along channels\n
 *   1. The shells A and B are connected via channel X, B and C are connected
 *      via channel Y. The user is currently on A but wants to connect to C.
 *   2. A first remote connection needs to be etsablished on channel X to the
 *      "proxy" shell B.
 *   3. From B the user can traverse further via channel Y to shell C.
 * - interrupting an established or broken connection\n
 *   1. A remote connection has already been established between B and C, but
 *      for some reason the connection stalled.
 *   2. A new remote connection is requested from A to C.
 *   3. C acknowledges the request and cancels the previous connection to B.
 *   4. B takes note of the new connection as well and cancels the previos
 *      connection to C as well.
 */
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/* AosShellChannel ************************************************************/
/**
 * @addtogroup core_shell_shellchannel
 * @{
 */

/**
 * @brief   Shell I/O channel flag whether the channel is enabled as input.
 */
#define AOS_SHELLCHANNEL_INPUT                  ((aos_shellchannelflags_t)(1 << 0))

/**
 * @brief   Shell I/O channel flag whether the channel is enabled as output.
 */
#define AOS_SHELLCHANNEL_OUTPUT                 ((aos_shellchannelflags_t)(1 << 1))

/**
 * @brief   Shell I/O channel flag whether the channel may be used fopr remote
 *          communication.
 * @details Channels flagged as remote will implicitely have input enabled.
 * @note    This flag has no effect if AMIROOS_CFG_SHELL_REMOTE_ENABLE is not
 *          true.
 */
#define AOS_SHELLCHANNEL_REMOTE                 ((aos_shellchannelflags_t)(1 << 8))

/**
 * @brief   Shall I/O channel flags mask.
 * @note    Other bits of the flags member may be used internally.
 */
#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) || defined(__DOXYGEN__)
# define AOS_SHELLCHANNEL_FLAGSMASK            (AOS_SHELLCHANNEL_INPUT   |      \
                                                AOS_SHELLCHANNEL_OUTPUT  |      \
                                                AOS_SHELLCHANNEL_REMOTE)

#else /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */
# define AOS_SHELLCHANNEL_FLAGSMASK            (AOS_SHELLCHANNEL_INPUT   |      \
                                                AOS_SHELLCHANNEL_OUTPUT)
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */

/** @} */

/* shell events ***************************************************************/
/**
 * @addtogroup core_shell
 * @{
 */

/**
 * @brief   Mask of events, which must not be used within shell commands.
 */
#define AOS_SHELL_EVENTSMASK_PROHIBITED         ((eventmask_t)0xFF)

/**
 * @brief   Mask of events, which may be used within shell commands.
 */
#define AOS_SHELL_EVENTSMASK_COMMAND            (~AOS_SHELL_EVENTSMASK_PROHIBITED)

/**
 * @brief   Event mask to be set on OS related events.
 */
#define AOS_SHELL_EVENT_OS                      EVENT_MASK(0)

#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) || defined(__DOXYGEN__)
/**
 * @brief   Event mask to be set on a remote timer event.
 *
 * @ingroup core_shell_shellremote
 */
#define AOS_SHELL_EVENT_REMOTETIMER             EVENT_MASK(1)
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */

/**
 * @brief   Event mask to be set on a input event.
 */
#define AOS_SHELL_EVENT_INPUT                   EVENT_MASK(2)

/** @} */

/* shell event flags **********************************************************/
/**
 * @addtogroup core_shell
 * @{
 */

/**
 * @brief   Shell event flag that is emitted when the thread starts.
 */
#define AOS_SHELL_EVTFLAG_START                 ((eventflags_t)(1 << 0))

/**
 * @brief   Shell event flag that is emitted when a command is executed.
 */
#define AOS_SHELL_EVTFLAG_EXECUTE               ((eventflags_t)(1 << 1))

/**
 * @brief   Shell event flag that is emitted when a command execution finished.
 */
#define AOS_SHELL_EVTFLAG_DONE                  ((eventflags_t)(1 << 2))

/**
 * @brief   Shell event flag that is emitted when the shread stops.
 */
#define AOS_SHELL_EVTFLAG_EXIT                  ((eventflags_t)(1 << 3))

/**
 * @brief   Shell event flag that is emitted when an I/O error occurred.
 */
#define AOS_SHELL_EVTFLAG_IOERROR               ((eventflags_t)(1 << 4))

/**
 * @brief   Shell event flag that is emitted when a remote connection to a
 *          server has been established.
 */
#define AOS_SHELL_EVTFLAG_REMOTECLIENT          ((eventflags_t)(1 << 4))

/**
 * @brief   Shell event flag that is emitted when a remote connection to a
 *          client has been established.
 */
#define AOS_SHELL_EVTFLAG_REMOTESERVER          ((eventflags_t)(1 << 5))

/**
 * @brief   Shell event flag that is emitted when a remote connection has been
 *          closed.
 */
#define AOS_SHELL_EVTFLAG_REMOTEDISCONNECTED    ((eventflags_t)(1 << 6))

/**
 * @brief   Shell event flag that is emitted when a remote channel has been
 *          disabled.
 */
#define AOS_SHELL_EVTFLAG_REMOTEDISABLED        ((eventflags_t)(1 << 7))

/** @} */

/* remote *********************************************************************/
/**
 * @addtogroup core_shell_shellremote
 * @{
 */

#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) || defined(__DOXYGEN__)

/**
 * @brief   Number of bytes for a remote shell identifier type.
 * @details Allowed values are 1, 2, 4 and 8.
 */
#if !defined(AOS_SHELL_REMOTEIDWIDTH) || defined(__DOXYGEN__)
# define AOS_SHELL_REMOTEIDWIDTH                4
#endif /* !defined(AOS_SHELL_REMOTEIDWIDTH) */

/**
 * @brief   Lenth of the longest expected remote access control message.
 */
#define AOS_SHELL_RMSG_MAXLENGTH                (AOS_SHELL_REMOTEIDWIDTH + 2)

#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */

/** @} */

/* misc ***********************************************************************/
/**
 * @addtogroup core_shell
 * @{
 */

/**
 * @brief   Length of the longest expected escape sequences.
 */
#define AOS_SHELL_ESCSEQ_MAXLENGTH              6

/**
 * @brief   Size of the sequence buffer.
 * @details The size is defined as the maximum of the multiple use cases for
 *          this buffer plus a trailing NUL byte.
 */
#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE != true) || (AOS_SHELL_ESCSEQ_MAXLENGTH > AOS_SHELL_RMSG_MAXLENGTH) || defined(__DOXYGEN__)
# define AOS_SHELL_SEQUENCEBUFFER_SIZE          (AOS_SHELL_ESCSEQ_MAXLENGTH + 1)
#else /* (AOS_SHELL_ESCSEQ_MAXLENGTH > AOS_SHELL_RMSG_MAXLENGTH) */
# define AOS_SHELL_SEQUENCEBUFFER_SIZE          (AOS_SHELL_RMSG_MAXLENGTH + 1)
#endif /* (AOS_SHELL_ESCSEQ_MAXLENGTH > AOS_SHELL_RMSG_MAXLENGTH) */

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

#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) || defined(__DOXYGEN__)

/**
 * @brief   Remote identifier type.
 * @details A identifier value of 0 is considered invalid.
 *
 * @ingroup core_shell_shellremote
 */
#if (AOS_SHELL_REMOTEIDWIDTH == 1)
typedef uint8_t aos_shellremoteid_t;
#elif (AOS_SHELL_REMOTEIDWIDTH == 2)
typedef uint16_t aos_shellremoteid_t;
#elif (AOS_SHELL_REMOTEIDWIDTH == 4) || defined(__DOXYGEN__)
typedef uint32_t aos_shellremoteid_t;
#elif (AOS_SHELL_REMOTEIDWIDTH == 8)
typedef uint64_t aos_shellremoteid_t;
#else
#error "AOS_SHELL_REMOTEIDWIDTH set to invalid value"
#endif

#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */

/**
 * @brief   AosShellChannel flags type.
 *
 * @ingroup core_shell_shellchannel
 */
#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) || defined(__DOXYGEN__)
typedef uint16_t aos_shellchannelflags_t;
#else /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */
typedef uint8_t aos_shellchannelflags_t;
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */

/**
 * @brief   AosShellChannel specific methods.
 */
#define _aos_shell_channel_methods                                              \
  _base_asynchronous_channel_methods

/**
 * @brief   AosShellChannel specific data.
 */
#define _aos_shell_channel_data                                                 \
  /* pointer to the next channel in a AosShellStream */                         \
  struct aos_shellchannel* next;                                                \
  /* pointer to a BaseAsynchronousChannel object */                             \
  BaseAsynchronousChannel* asyncchannel;                                        \
  /* event listener for the associated BaseAsynchronousChannel */               \
  event_listener_t listener;                                                    \
  /* flags related to the channel */                                            \
  aos_shellchannelflags_t flags;

/**
 * @brief   AosShellChannel virtual methods table.
 *
 * @extends BaseAsynchronousChannelVMT
 * @ingroup core_shell_shellchannel
 */
struct AosShellChannelVMT {
  _aos_shell_channel_methods
};

/**
 * @brief   Shell channel class.
 * @details This class implements an asynchronous I/O channel.
 *
 * @extends BaseAsynchronousChannel
 * @ingroup core_shell_shellchannel
 */
typedef struct aos_shellchannel {
  /** @brief Virtual Methods Table. */
  const struct AosShellChannelVMT* vmt;
  _aos_shell_channel_data
} AosShellChannel;

/**
 * @brief   AosShellStream methods.
 */
#define _aos_shellstream_methods                                                \
  _base_channel_methods

/**
 * @brief   AosShellStream data.
 */
#define _aos_shellstream_data                                                   \
  _base_channel_data                                                            \
  /* Pointer to the first channel in a list. */                                 \
  AosShellChannel* channel;

/**
 * @brief   AosShellStream virtual methods table.
 *
 * @extends BaseChannelVMT
 * @ingroup core_shell_shellstream
 */
struct AosShellStreamVMT {
  _aos_shellstream_methods
};

/**
 * @brief   AosShellStream class.
 * @details This class implements an base sequential stream.
 * @todo    So far only output but no input is supported.
 *
 * @extends BaseChannel
 * @ingroup core_shell_shellstream
 */
typedef struct aosshellstream {
  const struct AosShellStreamVMT* vmt;  /**< Pointer to a virtual methods table. */
  _aos_shellstream_data
} AosShellStream;

/**
 * @addtogroup core_shell
 * @{
 */

/**
 * @brief   Shell command calback type.
 *
 * @param[in] stream  Stream to print to.
 * @param[in] argc    Number of arguments.
 * @param[in] argv    List of arguments.
 *
 * @return  Status of the command.
 * @retval 0  Execution successful.
 */
typedef int (*aos_shellcmdcb_t)(BaseSequentialStream* stream, int argc, const char* argv[]);

/**
 * @brief   Shell command structure.
 */
typedef struct aos_shellcommand {
  /**
   * @brief   Command name.
   */
  const char* name;

  /**
   * @brief   Callback function.
   */
  aos_shellcmdcb_t callback;

  /**
   * @brief   Pointer to next command in a singly linked list.
   */
  struct aos_shellcommand* next;

} aos_shellcommand_t;

/**
 * @brief   Shell help request status.
 */
typedef enum aos_shellhelprequest {
  AOS_SHELL_HELPREQUEST_NONE      = 0,  /**< No help requested. */
  AOS_SHELL_HELPREQUEST_EXPLICIT  = 1,  /**< Help requested explicitely. */
  AOS_SHELL_HELPREQUEST_IMPLICIT  = 2,  /**< Help requested implicitely. */
} aos_shellhelprequest_t;

/**
 * @brief   Shell state.
 */
typedef enum aos_shellstate {
  AOS_SHELL_STATE_UNINT       = 0,  /**< Shell yet uninitialized. */
  AOS_SHELL_STATE_INIT        = 1,  /**< Shell has been initialized. */
  AOS_SHELL_STATE_RUNNING     = 2,  /**< Shell is running. */
  AOS_SHELL_STATE_STOPPING    = 3,  /**< Shell is is about to terminate. */
  AOS_SHELL_STATE_TERMINATED  = 4,  /**< Shell has terminated. */
} aos_shellstate_t;

/**
 * @brief   Shell configuration type.
 */
typedef uint8_t aos_shellconfig_t;

/**
 * @brief   Enumerator to encode shell actions.
 */
typedef enum aos_shellaction {
  AOS_SHELL_ACTION_UNSPECIFIED,             /**< Unspecified/invalid action. */
  AOS_SHELL_ACTION_NONE,                    /**< No action at all. */
  AOS_SHELL_ACTION_READCHAR,                /**< Read a printable character. */
  AOS_SHELL_ACTION_AUTOCOMPLETE,            /**< Automatically comlete input by using available command. */
  AOS_SHELL_ACTION_SUGGEST,                 /**< Suggest matching available commands. */
  AOS_SHELL_ACTION_EXECUTE,                 /**< Execute input. */
  AOS_SHELL_ACTION_DELETEBACKWARD,          /**< Delete a single character backwards. */
  AOS_SHELL_ACTION_DELETEFORWARD,           /**< Delete a single character forwards. */
  AOS_SHELL_ACTION_CLEAR,                   /**< Clear the input. */
  AOS_SHELL_ACTION_RECALLPREVIOUS,          /**< Recall the previous (older) entry in the history. */
  AOS_SHELL_ACTION_RECALLNEXT,              /**< Recall the next (more recent) entry in the history. */
  AOS_SHELL_ACTION_RECALLOLDEST,            /**< Recall the oldest entry in the history. */
  AOS_SHELL_ACTION_RECALLCURRENT,           /**< Recall the current input. */
  AOS_SHELL_ACTION_CURSORLEFT,              /**< Move cursor one character to the left. */
  AOS_SHELL_ACTION_CURSORRIGHT,             /**< Move cursor one character to the right. */
  AOS_SHELL_ACTION_CURSORWORDLEFT,          /**< Move cursor one word to the left. */
  AOS_SHELL_ACTION_CURSORWORDRIGHT,         /**< Move cursor one word to the right. */
  AOS_SHELL_ACTION_CURSORTOEND,             /**< Move cursor to the very right. */
  AOS_SHELL_ACTION_CURSORTOSTART,           /**< Move cursor to the very left. */
  AOS_SHELL_ACTION_RESET,                   /**< Reset the current input. */
  AOS_SHELL_ACTION_INSERTTOGGLE,            /**< Toggle insertion mode. */
  AOS_SHELL_ACTION_PRINTUNKNOWNSEQUENCE,    /**< Print an unknown sequence. */
#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) || defined(__DOXYGEN__)
  AOS_SHELL_ACTION_REMOTE_PASSTHROUGH,      /**< Just pass I/O through. */
  AOS_SHELL_ACTION_REMOTE_DISCOVERRESPOND,  /**< Respond to a remote discover request. */
  AOS_SHELL_ACTION_REMOTE_CLIENTRESPOND,    /**< Respond to a remote client request. */
  AOS_SHELL_ACTION_REMOTE_LIST,             /**< List a remote shell that answered to a ping. */
  AOS_SHELL_ACTION_REMOTE_ACKNOWLEDGE,      /**< Acknowledge and establish a requested connection. */
  AOS_SHELL_ACTION_REMOTE_ESTABLISH,        /**< Establish a requested connection. */
  AOS_SHELL_ACTION_REMOTE_CLOSE,            /**< Close a remote connection. */
  AOS_SHELL_ACTION_REMOTE_CANCEL_ACTIVE,    /**< Actively cancel the current remote connection. */
  AOS_SHELL_ACTION_REMOTE_CANCEL_PASSIVE,   /**< Passively cancel the current remote connection. */
  AOS_SHELL_ACTION_REMOTE_DISABLE_ACTIVE,   /**< Actively disable a remote channel. */
  AOS_SHELL_ACTION_REMOTE_DISABLE_PASSIVE,  /**< Passively disable a remote channel. */
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */
} aos_shellaction_t;

/**
 * @brief   Shell structure.
 */
typedef struct aos_shell {

  /**
   * @brief   Thread working area of the shell.
   * @details Since this is the first member of the shell structure, builtin
   *          commands can retrieve the calling shell via the pointer of the
   *          currently executed thread.
   */
  THD_WORKING_AREA(thread_wa, AMIROOS_CFG_SHELL_STACKSIZE);

  /**
   * @brief   Name of the shell and the associated thread.
   */
  const char* name;

  /**
   * @brief   String to printed as prompt.
   */
  const char* prompt;

  /**
   * @brief   Execution status of the shell.
   */
  aos_shellstate_t state;

  /**
   * @brief   Configuration flags.
   */
  aos_shellconfig_t config;

  /**
   * @brief   Event system related data.
   */
  struct {
    /**
     * @brief   Event source.
     */
    event_source_t source;

    /**
     * @brief   Listener for OS related events.
     */
    event_listener_t osListener;
  } events;

  /**
     * @brief   I/O stream.
     */
  AosShellStream stream;

#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) || defined(__DOXYGEN__)
  /**
   * @brief   Remote access control related data.
   */
  struct {
    /**
     * @brief   Shell remote identifier.
     */
    aos_shellremoteid_t id;

    /**
     * @brief   Timer used for remote acces control timeouts.
     */
    aos_timer_t timer;

    /**
     * @brief   Event source for timer events.
     */
    event_source_t evtSource;

    /**
     * @brief   Event listener for timer events.
     */
    event_listener_t evtListener;
  } remote;
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */

  /**
   * @brief   Input & history buffer.
   * @details This buffer is interpreted as two dimensional array.
   *          It contains @p nentries elements of @p linewidth size each.
   */
  char buffer[(AMIROOS_CFG_SHELL_HISTLENGTH+1) * AMIROOS_CFG_SHELL_LINEWIDTH];

  /**
   * @brief   Operation data of the shell.
   */
  struct {
    /**
     * @brief   The currently evaluated channel.
     */
    AosShellChannel* channel;

    /**
     * @brief   Data related to the current input.
     */
    struct {
      /**
       * @brief   Length of the input.
       */
      size_t length;

      /**
       * @brief   Current position of the cursor in the input line.
       */
      size_t cursorpos;

      /**
       * @brief   Buffer to store special sequences.
       */
      char seqbuf[AOS_SHELL_SEQUENCEBUFFER_SIZE];

      /**
       * @brief   Length of the string in seqbuf.
       * @details The length of the string in seqbuf must not be determined
       *          using strlen() function, since NUL bytes may be valid.
       */
      size_t seqlen;

      /**
       * @brief   Buffer to store argument pointer.
       */
      char* argbuf[AMIROOS_CFG_SHELL_MAXARGS];
    } input;

    /**
     * @brief   Data related to the entry or history buffer.
     */
    struct {
      /**
       * @brief   Current entry to be filled and executed.
       * @details This is an absolute index in the history buffer.
       *          It indicates, which entry in the cirular-like history buffer
       *          is set next.
       */
      size_t current;

      /**
       * @brief   Selected entry in the 'history' as preview.
       * @details This is relative index in the history buffer wrt. 'current'.
       *          A value of 0 indicates, that the line is cleared as a preview.
       *          A value of 1 indicates, that the current entry is selected.
       *          A value of t>1 indicates, that the entry t-1 in the past is
       *          selected.
       *          The value must never be greater than the number of entries
       *          available, of course.
       */
      size_t selected;

      /**
       * @brief   Selected entry in the 'history' that has been edited by the
       *          user.
       * @details This is a relative index in the history buffer wrt. 'current'.
       *          A value of 0 indicates, that there was no modification by the
       *          user yet (i.e. charcters, deletions or autofill).
       *          A value of 1 indicates, that the current entry was edited.
       *          A value of t>1 indicates, that the history entry t-1 was
       *          recalled and then edited.
       */
      size_t edited;
    } history;

    /**
     * @brief   The last action executed by the shell.
     */
    aos_shellaction_t lastaction;

#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) || defined(__DOXYGEN__)
    /**
     * @brief   Data related to remote access control.
     */
    struct {
      /**
       * @brief   Identifier of the requested remote shell.
       */
      aos_shellremoteid_t requested;

      /**
       * @brief   The channel to be used for remote access or NULL if no remote
       *          connection has been established.
       * @details If set, this shell is client on that channel.
       */
      AosShellChannel* channel;
    } remote;
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */

    /**
     * @brief   Execution status of the most recens command.
     */
    struct {
      /**
       * @brief   Pointer to the command that was executed.
       */
      const aos_shellcommand_t* command;

      /**
       * @brief   Return value of the executed command.
       */
      int retval;
    } execstatus;

    /**
     * @brief Status flags to indicate ongoing actions.
     */
    uint8_t flags;
  } operation;
} aos_shell_t;

/** @} */

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/**
 * @addtogroup core_shell
 * @{
 */

/**
 * @brief   Initializes a shell command object.
 *
 * @param[in] var       Name of the object variable to be initialized.
 * @param[in] name      Shell command name.
 * @param[in] callback  Pointer to the callback function.
 */
#define AOS_SHELL_COMMAND(var, name, callback)                                  \
  aos_shellcommand_t var = {                                                    \
    /* name     */ name,                                                        \
    /* callback */ callback,                                                    \
    /* next     */ NULL,                                                        \
  }

/**
 * @brief   Event register function to be used from within shell commands.
 *
 * @param[in] source    Pointer to event source to register to.
 * @param[in] listener  Pointer to event listener to register to the source.
 * @param[in] id        Event idenetifier to be registered.
 */
#define aosShellEventRegister(source, listener, id) {                           \
  aosDbgCheck((EVENT_MASK(id) & AOS_SHELL_EVENTSMASK_PROHIBITED) == 0);         \
  chEvtRegister(source, listener, id);                                          \
}

/**
 * @brief   Event register function to be used from within shell commands.
 *
 * @param[in] source    Pointer to event source to register to.
 * @param[in] listener  Pointer to event listener to register to the source.
 * @param[in] mask      Event mask to be registered.
 */
#define aosShellEventRegisterMask(source, listener, mask) {                     \
  aosDbgCheck((mask & AOS_SHELL_EVENTSMASK_PROHIBITED) == 0);                   \
  chEvtRegisterMask(source, listener, mask);                                    \
}

/**
 * @brief   Event register function to be used from within shell commands.
 *
 * @param[in] source    Pointer to event source to register to.
 * @param[in] listener  Pointer to event listener to register to the source.
 * @param[in] mask      Event mask to be registered.
 * @param[in] wflags    Event flags to filter events.
 */
#define aosShellEventRegisterMaskWithFlags(source, listener, mask, wflags) {    \
  aosDbgCheck((mask & AOS_SHELL_EVENTSMASK_PROHIBITED) == 0);                   \
  chEvtRegisterMaskWithFlags(source, listener, mask, 0);                        \
}

/**
 * @brief   Event unregister function to be used from within shell commands.
 *
 * @param[in] source    Pointer to event source to unregister from.
 * @param[in] listener  Pointer to event listener to unregister.
 */
#define aosShellEventUnregister(source, listener) {                             \
  chEvtUnregister(source, listener);                                            \
}

/**
 * @brief   Retrieve the AosShellStream of a shell.
 *
 * @param[in] shell   Pointer to the shell to retrieve the stream from.
 *
 * @return  Pointer to the AosShellStream.
 */
#define aosShellGetStream(shell)                (&(shell)->stream)

/** @} */

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  void aosShellInit(void);
  void aosShellObjectInit(aos_shell_t* shell, const char* name, const char* prompt);
  void aosShellStart(aos_shell_t* shell, tprio_t prio);
  aos_status_t aosShellAddCommand(aos_shellcommand_t* cmd);
  aos_status_t aosShellRemoveCommand(const char* cmd, aos_shellcommand_t** removed);
  void aosShellChannelInit(AosShellChannel* channel, BaseAsynchronousChannel* asyncchannel, aos_shellchannelflags_t flags);
  void aosShellStreamAddChannel(AosShellStream* stream, AosShellChannel* channel);
  aos_shellhelprequest_t aosShellcmdCheckHelp(int argc, const char* argv[], bool abbrev);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/**
 * @brief   Flushs the stream output.
 *
 * @ingroup core_shell_shellstream
 *
 * @param[in] stream  The stream to flush.
 *
 * @return  The operation result.
 * @retval AOS_OK     The stream was successfully flushed.
 * @retval AOS_ERROR  Flushing the stream failed.
 */
static inline aos_status_t aosShellStreamFlush(AosShellStream* stream)
{
  return (chnControl((BaseChannel*)stream, CHN_CTL_TX_WAIT, NULL) == MSG_OK) ? AOS_OK : AOS_ERROR;
}

#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */

#endif /* AMIROOS_SHELL_H */
