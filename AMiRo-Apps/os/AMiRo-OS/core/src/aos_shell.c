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
 * @file    aos_shell.c
 * @brief   Shell code.
 * @details Shell code as well as shell related channels and streams.
 */

#include <amiroos.h>

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/* AosShellChannel ************************************************************/

/**
 * @brief   Shell I/O channel flag whether the channel is attached to a list.
 */
#define SHELLCHANNEL_ATTACHED                   ((aos_shellchannelflags_t)(1 << 2))

/**
 * @brief   Shell I/O channel flag whether input on the channel has been
 *          suspended.
 * @details Input is not read from buffer, thus can be retrieved later.
 *          Does not affect output.
 */
#define SHELLCHANNEL_INPUT_SUSPENDED            ((aos_shellchannelflags_t)(1 << 3))

/**
 * @brief   Shell I/O channel flag wether input is discarded.
 * @details When discarding input, it is read from the buffer but dropped
 */
#define SHELLCHANNEL_INPUT_DISCARD              ((aos_shellchannelflags_t)(1 << 4))

/**
 * @brief   Shell I/O flag whether output setting is overidden (enabled).
 * @details Output is written to the channel even if it is not flagged as
 *          output.
 */
#define SHELLCHANNEL_OUTPUT_OVERRIDE            ((aos_shellchannelflags_t)(1 << 5))

/**
 * @brief   Shell I/O channel flag wether output is discarded.
 * @details Output is not written to the channel even if it is flagged as
 *          output.
 */
#define SHELLCHANNEL_OUTPUT_DISCARD             ((aos_shellchannelflags_t)(1 << 6))

#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) || defined(__DOXYGEN__)

/**
 * @brief   Shell I/O channel flag whether a remote connection has been
 *          established to a client.
 */
#define SHELLCHANNEL_REMOTE_ESTABLISHED         ((aos_shellchannelflags_t)(1 << 11))

#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */

/* shell configuration flags **************************************************/

/**
 * @brief   Shell input configuration for replacing content by user input.
 */
#define SHELL_CONFIG_INPUT_OVERWRITE            (1 << 0)

/**
 * @brief   Shell prompt configuration print a minimalistic prompt.
 */
#define SHELL_CONFIG_PROMPT_MINIMAL             (1 << 1)

/**
 * @brief   Shell prompt configuration to additionally print the system uptime
 *          with the prompt.
 */
#define SHELL_CONFIG_PROMPT_UPTIME              (1 << 2)

/**
 * @brief   Shell prompt configuration to additionally print the date and time
 *          with the prompt.
 */
#define SHELL_CONFIG_PROMPT_DATETIME            (2 << 2)

/**
 * @brief   Shell input is case sensitive.
 */
#define SHELL_CONFIG_MATCHCASE                  (1 << 4)

/**
 * @brief   Shell default configuration.
 */
#define SHELL_CONFIG_DEFAULT                    0

/* shell operation flags ******************************************************/

/**
 * @brief   Shell is currently interpreting an escape sequence.
 */
#define SHELL_OPFLAGS_ESCSEQUENCE               (1 << 0)

/**
 * @brief   Shell is currently executing a command.
 */
#define SHELL_OPFLAGS_EXECUTING                 (1 << 1)

#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) || defined(__DOXYGEN__)

/**
 * @brief   Shell is currently interpreting a remote access control message.
 */
#define SHELL_OPFLAGS_RMSGSEQUENCE              (1 << 4)

/**
 * @brief   Mask of the remote action value.
 */
#define SHELL_OPFLAGS_REMOTEACTION_MASK         (7 << 5)

/**
 * @brief   No action is currently performed.
 */
#define SHELL_OPFLAGS_REMOTEACTION_NONE         (0 << 5)

/**
 * @brief   Shell has sent a discover request and is waiting for responses.
 */
#define SHELL_OPFLAGS_REMOTEACTION_DISCOVER     (1 << 5)

/**
 * @brief   Shell has sent a client request and is waiting for responses.
 */
#define SHELL_OPFLAGS_REMOTEACTION_LISTCLIENTS  (2 << 5)

/**
 * @brief   Shell has sent a connect request and is waiting for acknowledgement.
 */
#define SHELL_OPFLAGS_REMOTEACTION_CONNECT      (3 << 5)

/**
 * @brief   Shell is waiting for response from certain remote shell to
 *          disconnect the according channel.
 */
#define SHELL_OPFLAGS_REMOTEACTION_DISCONNECT   (4 << 5)

#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */

/* character definitions ******************************************************/

/**
 * @brief   The character the input buffer is initialized with.
 * @note    Must not be ASCII_NUL.
 */
#define INBUF_INIT_CHAR                         ASCII_DEL

/**
 * @brief   ASCII character: Null
 * @note    '\\0'
 */
#define ASCII_NUL                               0x00

/**
 * @brief   ASCII character: Start of Heading
 */
#define ASCII_SOH                               0x01

/**
 * @brief   ASCII character: Start of Text
 */
#define ASCII_STX                               0x02

/**
 * @brief   ASCII character: End of Text
 */
#define ASCII_ETX                               0x03

/**
 * @brief   ASCII character: End of Transmission
 */
#define ASCII_EOT                               0x04

/**
 * @brief   ASCII character: Enquiry
 */
#define ASCII_ENQ                               0x05

/**
 * @brief   ASCII character: Acknowledgement
 */
#define ASCII_ACK                               0x06

/**
 * @brief   ASCII character: Bell
 * @note    '\\a'
 */
#define ASCII_BEL                               0x07

/**
 * @brief   ASCII character: Backspace
 * @note    '\\b'
 */
#define ASCII_BS                                0x08

/**
 * @brief   ASCII character: Horizontal Tab
 * @note    '\\t'
 */
#define ASCII_HT                                0x09

/**
 * @brief   ASCII character: Line Feed
 * @note    '\\n'
 */
#define ASCII_LF                                0x0A

/**
 * @brief   ASCII character: Vertical Tab
 * @note    '\\v'
 */
#define ASCII_VT                                0x0B

/**
 * @brief   ASCII character: Form Feed
 * @note    '\\f'
 */
#define ASCII_FF                                0x0C

/**
 * @brief   ASCII character: Carriage Return
 * @note    '\\r'
 */
#define ASCII_CR                                0x0D

/**
 * @brief   ASCII character: Shift Out
 */
#define ASCII_SO                                0x0E

/**
 * @brief   ASCII character: Shift In
 */
#define ASCII_SI                                0x0F

/**
 * @brief   ASCII character: Data Link Escape
 */
#define ASCII_DLE                               0x10

/**
 * @brief   ASCII character: Device Control 1
 */
#define ASCII_DC1                               0x11

/**
 * @brief   ASCII character: Device Control 2
 */
#define ASCII_DC2                               0x12

/**
 * @brief   ASCII character: Device Control 3
 */
#define ASCII_DC3                               0x13

/**
 * @brief   ASCII character: Device Control 4
 */
#define ASCII_DC4                               0x14

/**
 * @brief   ASCII character: Negative Acknowledgement
 */
#define ASCII_NAK                               0x15

/**
 * @brief   ASCII character: Synchronous Idle
 */
#define ASCII_SYN                               0x16

/**
 * @brief   ASCII character: End of Transmission Block
 */
#define ASCII_ETB                               0x17

/**
 * @brief   ASCII character: Cancel
 */
#define ASCII_CAN                               0x18

/**
 * @brief   ASCII character: End of Medium
 */
#define ASCII_EM                                0x19

/**
 * @brief   ASCII character: Substitute
 */
#define ASCII_SUB                               0x1A

/**
 * @brief   ASCII character: Escape
 */
#define ASCII_ESC                               0x1B

/**
 * @brief   ASCII character: File Separator
 */
#define ASCII_FS                                0x1C

/**
 * @brief   ASCII character: Group Separator
 */
#define ASCII_GS                                0x1D

/**
 * @brief   ASCII character: Record Separator
 */
#define ASCII_RS                                0x1E

/**
 * @brief   ASCII character: Unit Separator
 */
#define ASCII_US                                0x1F

/**
 * @brief   ASCII character: Delete
 */
#define ASCII_DEL                               0x7F

/* escape key sequences *******************************************************/

/**
 * @brief   String that defines the INSERT key as specified by VT100.
 * @details ESC [ 2 ~
 */
#define KEYSTRING_INSERT                        "\x1B\x5B\x32\x7E"

/**
 * @brief   String that defines the DEL key as specified by VT100.
 * @details ESC [ 3 ~
 */
#define KEYSTRING_DELETE                        "\x1B\x5B\x33\x7E"

/**
 * @brief   String that defines the HOME key as specified by VT100.
 * @details ESC [ H
 */
#define KEYSTRING_HOME                          "\x1B\x5B\x48"

/**
 * @brief   String that defines the END key as specified by VT100.
 * @details ESC [ F
 */
#define KEYSTRING_END                           "\x1B\x5B\x46"

/**
 * @brief   String that defines the PGUP key as specified by VT100.
 * @details ESC [ 5 ~
 */
#define KEYSTRING_PAGEUP                        "\x1B\x5B\x35\x7E"

/**
 * @brief   String that defines the PGUP key as specified by VT100.
 * @details ESC [ 6 ~
 */
#define KEYSTRING_PAGEDOWN                      "\x1B\x5B\x36\x7E"

/**
 * @brief   String that defines the 'arrow down' key as specified by VT100.
 * @details ESC [ A
 */
#define KEYSTRING_ARROWUP                       "\x1B\x5B\x41"

/**
 * @brief   String that defines the 'arrow up' key as specified by VT100.
 * @details ESC [ B
 */
#define KEYSTRING_ARROWDOWN                     "\x1B\x5B\x42"

/**
 * @brief   String that defines the 'arrow left' key as specified by VT100.
 * @details ESC [ D
 */
#define KEYSTRING_ARROWLEFT                     "\x1B\x5B\x44"

/**
 * @brief   String that defines the 'arrow right' key as specified by VT100.
 * @details ESC [ C
 */
#define KEYSTRING_ARROWRIGHT                    "\x1B\x5B\x43"

/**
 * @brief   String that defines the CRTL + 'arrow up' key combination as
 *          specified by VT100.
 * @details ESC [ 1 ; 5 A
 */
#define KEYSTRING_CTRL_ARROWUP                  "\x1B\x5B\x31\x3B\x35\x41"

/**
 * @brief   String that defines the CRTL + 'arrow down' key combination as
 *          specified by VT100.
 * @details ESC [ 1 ; 5 B
 */
#define KEYSTRING_CTRL_ARROWDOWN                "\x1B\x5B\x31\x3B\x35\x42"

/**
 * @brief   String that defines the CRTL + 'arrow left' key combination as
 *          specified by VT100.
 * @details ESC [ 1 ; 5 D
 */
#define KEYSTRING_CTRL_ARROWLEFT                "\x1B\x5B\x31\x3B\x35\x44"

/**
 * @brief   String that defines the CRTL + 'arrow right' key combination as
 *          specified by VT100.
 * @details ESC [ 1 ; 5 C
 */
#define KEYSTRING_CTRL_ARROWRIGHT               "\x1B\x5B\x31\x3B\x35\x43"

/* remote access control ******************************************************/

#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) || defined(__DOXYGEN__)

/**
 * @brief   Special remote identifier for invalid/not available remote.
 */
#define REMOTEID_INVALID                        (aos_shellremoteid_t)0

/**
 * @brief   Byte to identify remote access control messages.
 */
#define REMOTEMSG_IDENTIFIER                    ASCII_ENQ

/**
 * @brief   Byte to identify remote access control discover request messages.
 */
#define REMOTEMSG_IDENTIFIER_DISCOVERREQUEST    '?'

/**
 * @brief   Byte to identify remote access control discover response messages.
 */
#define REMOTEMSG_IDENTIFIER_DISCOVERRESPONSE   '!'

/**
 * @brief   Byte to identify remote access control clients request messages.
 */
#define REMOTEMSG_IDENTIFIER_CLIENTSREQUEST     '#'

/**
 * @brief   Byte to identify remote access control client response messages.
 */
#define REMOTEMSG_IDENTIFIER_CLIENTRESPONSE     '~'

/**
 * @brief   Byte to identify remote access control connect messages.
 */
#define REMOTEMSG_IDENTIFIER_CONNECT            '@'

/**
 * @brief   Byte to identify remote access control acknowledgement messages.
 */
#define REMOTEMSG_IDENTIFIER_ACKNOWLEDGE        '+'

/**
 * @brief   Byte to identify remote access control cancelation messages.
 */
#define REMOTEMSG_IDENTIFIER_CANCEL             '-'

/**
 * @brief   Byte to identify remote access control disable messages.
 */
#define REMOTEMSG_IDENTIFIER_DISABLE            '\\'

#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */

/* input & history buffer *****************************************************/

/**
 * @brief   Number of entries in the history buffer.
 */
#define BUFFER_ENTRIES                          (AMIROOS_CFG_SHELL_HISTLENGTH + 1)

/******************************************************************************/
/* EXPORTED VARIABLES                                                         */
/******************************************************************************/

/******************************************************************************/
/* LOCAL TYPES                                                                */
/******************************************************************************/

/**
 * @brief   Enumerator of special keyboard keys.
 */
typedef enum special_key {
  KEY_UNKNOWN,          /**< any/unknow key */
  KEY_AMBIGUOUS,        /**< key is ambiguous */
  KEY_TAB,              /**< tabulator key */
  KEY_BACKSPACE,        /**< backspace key */
  KEY_INSERT,           /**< insert key */
  KEY_DELETE,           /**< delete key */
  KEY_ESCAPE,           /**< escape key */
  KEY_HOME,             /**< home key */
  KEY_END,              /**< end key */
  KEY_PAGEUP,           /**< page up key */
  KEY_PAGEDOWN,         /**< page down key */
  KEY_ARROWUP,          /**< arrow up key */
  KEY_ARROWDOWN,        /**< arrow down key */
  KEY_ARROWLEFT,        /**< arrow left key */
  KEY_ARROWRIGHT,       /**< arrow right key */
  KEY_CTRL_ARROWUP,     /**< CTRL + arrow up key */
  KEY_CTRL_ARROWDOWN,   /**< CTRL + arrow down key */
  KEY_CTRL_ARROWLEFT,   /**< CTRL + arrow left key */
  KEY_CTRL_ARROWRIGHT,  /**< CTRL + arrow right key */
  KEY_CTRL_C,           /**< CTRL + C key */
} special_key_t;

/**
 * @brief   Enumerator for case (in)sensitive character matching.
 */
typedef enum charmatch {
  CHAR_MATCH_NOT    = 0,  /**< Characters do not match at all. */
  CHAR_MATCH_NCASE  = 1,  /**< Characters would match case insensitive. */
  CHAR_MATCH_CASE   = 2,  /**< Characters do match with case. */
} charmatch_t;

#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) || defined(__DOXYGEN__)

/**
 * @brief   Remote access control message structure.
 */
typedef struct remote_message {
  /**
   * @brief   Identifier that was transmitted with the message.
   */
  aos_shellremoteid_t id;

  /**
   * @brief   Enumeration of remote access control message types.
   */
  enum {
    RMSG_UNKNOWN,           /**< Unknown/invalid message. */
    RMSG_AMBIGUOUS,         /**< Ambiguous message. */
    RMSG_DISCOVERREQUEST,   /**< Discover request message. */
    RMSG_DISCOVERRESPONSE,  /**< Discover response message. */
    RMSG_CLIENTSREQUEST,    /**< Client request message. */
    RMSG_CLIENTRESPONSE,    /**< Client response message. */
    RMSG_CONNECT,           /**< Connect message. */
    RMSG_ACKNOWLEDGE,       /**< Acknowledgement message. */
    RMSG_CANCEL,            /**< Cancelation message. */
    RMSG_DISABLE,           /**< Disable message. */
  } type;
} remote_message_t;

#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/

/* AosShellChannel ************************************************************/

/* forward declarations */
static size_t _aosShellChannelWrite(void *instance, const uint8_t *bp, size_t n);
static size_t _aosShellChannelRead(void *instance, uint8_t *bp, size_t n);
static msg_t _aosShellChannelPut(void *instance, uint8_t b);
static msg_t _aosShellChannelGet(void *instance);
static msg_t _aosShellChannelPutt(void *instance, uint8_t b, sysinterval_t time);
static msg_t _aosShellChannelGett(void *instance, sysinterval_t time);
static size_t _aosShellChannelWritet(void *instance, const uint8_t *bp, size_t n, sysinterval_t time);
static size_t _aosShellChannelReadt(void *instance, uint8_t *bp, size_t n, sysinterval_t time);
static msg_t _aosShellChannelCtl(void *instance, unsigned int operation, void *arg);

/**
 * @brief   Virtual method table for AosShellChannel class objects.
 */
static const struct AosShellChannelVMT _channelvmt = {
  (size_t) 0,
  _aosShellChannelWrite,
  _aosShellChannelRead,
  _aosShellChannelPut,
  _aosShellChannelGet,
  _aosShellChannelPutt,
  _aosShellChannelGett,
  _aosShellChannelWritet,
  _aosShellChannelReadt,
  _aosShellChannelCtl,
};

/* AosShellStream *************************************************************/

/* forward declarations */
static size_t _aosShellStreamWrite(void *instance, const uint8_t *bp, size_t n);
static msg_t _aosShellStreamPut(void *instance, uint8_t b);
static msg_t _aosShellStreamPutt(void *instance, uint8_t b, sysinterval_t time);
static size_t _aosShellStreamWritet(void *instance, const uint8_t *bp, size_t n, sysinterval_t time);
static msg_t _aosShellStreamCtl(void *instance, unsigned int operation, void *arg);

/**
 * @brief   Virtual method table for AosShellStream class objects.
 */
static const struct AosShellStreamVMT _streamvmt = {
  (size_t) 0,
  _aosShellStreamWrite,
  NULL,
  _aosShellStreamPut,
  NULL,
  _aosShellStreamPutt,
  NULL,
  _aosShellStreamWritet,
  NULL,
  _aosShellStreamCtl,
};

/* commands *******************************************************************/

/* forward declarations */
static int _aosShellCommandInfoCallback(BaseSequentialStream* stream, int argc, const char* argv[]);
static int _aosShellCommandHelpCallback(BaseSequentialStream* stream, int argc, const char* argv[]);
static int _aosShellCommandConfigCallback(BaseSequentialStream* stream, int argc, const char* argv[]);
#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) || defined(__DOXYGEN__)
static int _aosShellCommandRemoteCallback(BaseSequentialStream* stream, int argc, const char* argv[]);
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */

/**
 * @brief   Pointer to the first element of the singly linked list of commands.
 * @details Commands are ordered alphabetically in the list.
 */
static aos_shellcommand_t* _commands = NULL;

/**
 * @brief   Mutex lock for exclusive access to the common command list.
 */
static mutex_t _commands_mtx;

/**
 * @brief   Shell command to print some information about the shell.
 */
static AOS_SHELL_COMMAND(_command_info, "shell:info", _aosShellCommandInfoCallback);

/**
 * @brief   Shell command to print help how to use the shell.
 */
static AOS_SHELL_COMMAND(_command_help, "shell:help", _aosShellCommandHelpCallback);

/**
 * @brief   Shell command to configure the shell.
 */
static AOS_SHELL_COMMAND(_command_config, "shell:config", _aosShellCommandConfigCallback);

#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) || defined(__DOXYGEN__)
/**
 * @brief   Shell command for remote access control.
 */
static AOS_SHELL_COMMAND(_command_remote, "shell:remote", _aosShellCommandRemoteCallback);
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/**
 * forward declarations
 * @cond
 */
static size_t _aosShellChannelWrite(void *instance, const uint8_t *bp, size_t n);
static size_t _aosShellChannelRead(void *instance, uint8_t *bp, size_t n);
static msg_t _aosShellChannelPut(void *instance, uint8_t b);
static msg_t _aosShellChannelGet(void *instance);
static msg_t _aosShellChannelPutt(void *instance, uint8_t b, sysinterval_t time);
static msg_t _aosShellChannelGett(void *instance, sysinterval_t time);
static size_t _aosShellChannelWritet(void *instance, const uint8_t *bp, size_t n, sysinterval_t time);
static size_t _aosShellChannelReadt(void *instance, uint8_t *bp, size_t n, sysinterval_t time);
static msg_t _aosShellChannelCtl(void *instance, unsigned int operation, void *arg);
void _aosShellChannelSuspend(AosShellChannel* channel);
void _aosShellChannelResume(AosShellChannel* channel);
static size_t _aosShellStreamWrite(void *instance, const uint8_t *bp, size_t n);
static msg_t _aosShellStreamPut(void *instance, uint8_t b);
static msg_t _aosShellStreamPutt(void *instance, uint8_t b, sysinterval_t time);
static size_t _aosShellStreamWritet(void *instance, const uint8_t *bp, size_t n, sysinterval_t time);
static msg_t _aosShellStreamCtl(void *instance, unsigned int operation, void *arg);
void _aosShellStreamInit(AosShellStream* stream);
void _aosShellStreamSuspendAllChannelsBut(AosShellStream* stream, AosShellChannel* except);
void _aosShellStreamResumeSuspendedChannels(AosShellStream* stream);
size_t _aosShellCommandsCount(void);
static int _aosShellCommandInfoCallback(BaseSequentialStream* stream, int argc, const char* argv[]);
static int _aosShellCommandHelpCallback(BaseSequentialStream* stream, int argc, const char* argv[]);
static int _aosShellCommandConfigCallback(BaseSequentialStream* stream, int argc, const char* argv[]);
#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true)
static int _aosShellCommandRemoteCallback(BaseSequentialStream* stream, int argc, const char* argv[]);
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */
char* _aosShellGetAbsoluteEntry(aos_shell_t* shell, size_t entry);
size_t _aosShellHistoryOffset2EntryIndex(const aos_shell_t* shell, size_t offset);
char* _aosShellGetRelativeEntry(aos_shell_t* shell, size_t offset);
char* _aosShellGetCurrentEntry(aos_shell_t* shell);
const char* _aosShellGetSelectedEntry(aos_shell_t* shell);
char* _aosShellGetVisualisedEntry(aos_shell_t* shell);
size_t _aosShellParseArguments(aos_shell_t* shell, char* str);
charmatch_t _aosShellCharcmp(char lhs, char rhs);
char _aosShellMapAscii2Custom(char c);
int _aosShellStrccmp(const char *str1, const char *str2, bool cs, size_t* n, charmatch_t* m);
special_key_t _aosShellInterpreteEscapeSequence(const char seq[], size_t len);
#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true)
void _aosShellRemoteSerializeId(uint8_t* buf, aos_shellremoteid_t id);
aos_shellremoteid_t _aosShellRemoteSeserialseId(const uint8_t* buf);
int _aosShellRemotePrintId(BaseSequentialStream* stream, aos_shellremoteid_t id);
aos_shellremoteid_t _aosShellRemoteParseId(const char* str);
remote_message_t _aosShellRemoteInterpreteMessage(const char seq[], size_t len);
void _aosShellRemoteSetupMessage(char* buf, remote_message_t msg);
aos_shellremoteid_t _aosShellRemoteHashString(const char str[], aos_shellremoteid_t h);
static void _aosShellRemoteTimerCallback(virtual_timer_t* timer, void* params);
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */
void _aosShellPrintPrompt(aos_shell_t* shell);
int _aosShellMoveCursor(aos_shell_t* shell, const char* line, size_t from, size_t to);
size_t _aosShellPrintString(aos_shell_t* shell, const char* line, size_t from, size_t to);
int _aosShellPrintChar(aos_shell_t* shell, char c);
void _aosShellOverwriteOutput(aos_shell_t* shell, const char* line);
size_t _aosShellRestoreWhitespace(char* string, size_t length);
char* _aosShellPrepare4Modification(aos_shell_t* shell);
aos_shellaction_t _aosShellSelectAction(aos_shell_t* shell, char c, special_key_t key);
THD_FUNCTION(_aosShellThreadFunction, shell);
aos_shell_t* _aosShellRetrieveFromThread(thread_t* thd);
/** @endcond */

/* AosShellChannel ************************************************************/

/**
 * @brief   Implementation of the BaseAsynchronous write() method (inherited
 *          from BaseSequentialStream).
 */
static size_t _aosShellChannelWrite(void *instance, const uint8_t *bp, size_t n)
{
  // write only if the channel is flagged as active output
  return ((((AosShellChannel*)instance)->flags & (AOS_SHELLCHANNEL_OUTPUT | SHELLCHANNEL_OUTPUT_OVERRIDE)) &&
           !(((AosShellChannel*)instance)->flags & SHELLCHANNEL_OUTPUT_DISCARD)) ?
        streamWrite(((AosShellChannel*)instance)->asyncchannel, bp, n) :
        0;
}

/**
 * @brief   Implementation of the BaseAsynchronous read() method (inherited from
 *          BaseSequentialStream).
 */
static size_t _aosShellChannelRead(void *instance, uint8_t *bp, size_t n)
{
  // read only if the channel is flagged as active input
  if (((AosShellChannel*)instance)->flags & AOS_SHELLCHANNEL_INPUT &&
      !(((AosShellChannel*)instance)->flags & SHELLCHANNEL_INPUT_SUSPENDED)) {
    n = streamRead(((AosShellChannel*)instance)->asyncchannel, bp, n);
    // return 0 if channel is flagged to discard input
    return (((AosShellChannel*)instance)->flags & SHELLCHANNEL_INPUT_DISCARD) ? 0 : n;
  } else {
    return 0;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronous put() method (inherited from
 *          BaseSequentialStream).
 */
static msg_t _aosShellChannelPut(void *instance, uint8_t b)
{
  // write only if the channel is flagged as active output
  return ((((AosShellChannel*)instance)->flags & (AOS_SHELLCHANNEL_OUTPUT | SHELLCHANNEL_OUTPUT_OVERRIDE)) &&
           !(((AosShellChannel*)instance)->flags & SHELLCHANNEL_OUTPUT_DISCARD)) ?
        streamPut(((AosShellChannel*)instance)->asyncchannel, b) :
        MSG_RESET;
}

/**
 * @brief   Implementation of the BaseAsynchronous get() method (inherited from
 *          BaseSequentialStream).
 */
static msg_t _aosShellChannelGet(void *instance)
{
  // read only if the channel is flagged as active input
  if (((AosShellChannel*)instance)->flags & AOS_SHELLCHANNEL_INPUT &&
      !(((AosShellChannel*)instance)->flags & SHELLCHANNEL_INPUT_SUSPENDED)) {
    const msg_t msg = streamGet(((AosShellChannel*)instance)->asyncchannel);
    // return MSG_RESET if channel is flagged to discard input
    return (((AosShellChannel*)instance)->flags & SHELLCHANNEL_INPUT_DISCARD) ? MSG_RESET : msg;
  } else {
    return MSG_RESET;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronous putt() method.
 */
static msg_t _aosShellChannelPutt(void *instance, uint8_t b, sysinterval_t time)
{
  // write only if the channel is flagged as active output
  return ((((AosShellChannel*)instance)->flags & (AOS_SHELLCHANNEL_OUTPUT | SHELLCHANNEL_OUTPUT_OVERRIDE)) &&
           !(((AosShellChannel*)instance)->flags & SHELLCHANNEL_OUTPUT_DISCARD)) ?
        chnPutTimeout(((AosShellChannel*)instance)->asyncchannel, b, time) :
        MSG_RESET;
}

/**
 * @brief   Implementation of the BaseAsynchronous gett() method.
 */
static msg_t _aosShellChannelGett(void *instance, sysinterval_t time)
{
  // read only if the channel is flagged as active input
  if (((AosShellChannel*)instance)->flags & AOS_SHELLCHANNEL_INPUT &&
      !(((AosShellChannel*)instance)->flags & SHELLCHANNEL_INPUT_SUSPENDED)) {
    const msg_t msg = chnGetTimeout(((AosShellChannel*)instance)->asyncchannel, time);
    // return MSG_RESET if channel is flagged to discard input
    return (((AosShellChannel*)instance)->flags & SHELLCHANNEL_INPUT_DISCARD) ? MSG_RESET : msg;
  } else {
    return MSG_RESET;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronous writet() method.
 */
static size_t _aosShellChannelWritet(void *instance, const uint8_t *bp, size_t n, sysinterval_t time)
{
  // write only if the channel is flagged as active output
  return ((((AosShellChannel*)instance)->flags & (AOS_SHELLCHANNEL_OUTPUT | SHELLCHANNEL_OUTPUT_OVERRIDE)) &&
           !(((AosShellChannel*)instance)->flags & SHELLCHANNEL_OUTPUT_DISCARD)) ?
        chnWriteTimeout(((AosShellChannel*)instance)->asyncchannel, bp, n, time) :
        0;
}

/**
 * @brief   Implementation of the BaseAsynchronous readt() method.
 */
static size_t _aosShellChannelReadt(void *instance, uint8_t *bp, size_t n, sysinterval_t time)
{
  // read only if the channel is flagged as active input
  if (((AosShellChannel*)instance)->flags & AOS_SHELLCHANNEL_INPUT &&
      !(((AosShellChannel*)instance)->flags & SHELLCHANNEL_INPUT_SUSPENDED)) {
    n = chnReadTimeout(((AosShellChannel*)instance)->asyncchannel, bp, n, time);
    // return 0 if channel is flagged to discard input
    return (((AosShellChannel*)instance)->flags & SHELLCHANNEL_INPUT_DISCARD) ? 0 : n;
  } else {
    return 0;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronousChannel ctl() method.
 */
static msg_t _aosShellChannelCtl(void *instance, unsigned int operation, void *arg)
{
  // control only if the channel is flaggs as input or output
  return (((AosShellChannel*)instance)->flags & (AOS_SHELLCHANNEL_INPUT | AOS_SHELLCHANNEL_OUTPUT)) ?
        chnControl(((AosShellChannel*)instance)->asyncchannel, operation, arg) :
        MSG_RESET;
}

/**
 * @brief   Suspend a channel.
 *
 * @param[in] channel   Channel to suspend.
 */
void _aosShellChannelSuspend(AosShellChannel* channel)
{
  aosDbgCheck(channel != NULL);

  chSysLock();
  channel->listener.wflags &= ~CHN_INPUT_AVAILABLE;
  channel->flags |= SHELLCHANNEL_INPUT_SUSPENDED;
  chSysUnlock();

  return;
}

/**
 * @brief   Resume a suspended channel.
 *
 * @param[in] channel   Channel to resume.
 */
void _aosShellChannelResume(AosShellChannel* channel)
{
  aosDbgCheck(channel != NULL);

  chSysLock();
  channel->flags &= ~SHELLCHANNEL_INPUT_SUSPENDED;
  channel->listener.wflags |= CHN_INPUT_AVAILABLE;
  chSysUnlock();

  return;
}

/* AosShellStream *************************************************************/

/**
 * @brief   Implementation of the BaseChannel write() method.
 */
static size_t _aosShellStreamWrite(void *instance, const uint8_t *bp, size_t n)
{
  aosDbgCheck(instance != NULL);

  // local variables
  size_t maxbytes = 0;

  // iterate through the list of channels
  for (AosShellChannel* channel = ((AosShellStream*)instance)->channel; channel != NULL; channel = channel->next) {
    const size_t bytes = streamWrite(channel, bp, n);
    maxbytes = (bytes > maxbytes) ? bytes : maxbytes;
  }

  return maxbytes;
}

/**
 * @brief   Implementation of the BaseChannel put() method.
 */
static msg_t _aosShellStreamPut(void *instance, uint8_t b)
{
  aosDbgCheck(instance != NULL);

  // local variables
  msg_t ret = MSG_OK;

  // iterate through the list of channels
  for (AosShellChannel* channel = ((AosShellStream*)instance)->channel; channel != NULL; channel = channel->next) {
    const msg_t ret_ = streamPut(channel, b);
    ret = (ret_ < ret) ? ret_ : ret;
  }

  return ret;
}

/**
 * @brief   Implementation of the BaseChannel putt() method.
 */
static msg_t _aosShellStreamPutt(void *instance, uint8_t b, sysinterval_t time)
{
  aosDbgCheck(instance != NULL);

  // local variables
  systime_t currt = chVTGetSystemTime();
  const systime_t callt = currt;
  const systime_t timeoutt = chTimeAddX(currt, time);
  AosShellChannel* channel = ((AosShellStream*)instance)->channel;
  msg_t ret = MSG_OK;

  // iterate through list of channels
  for (; channel != NULL && chTimeIsInRangeX(currt, callt, timeoutt); channel = channel->next) {
    const msg_t ret_ = chnPutTimeout(channel, b, chTimeDiffX(currt, timeoutt));
    ret = (ret_ < ret) ? ret_ : ret;
    currt = chVTGetSystemTime();
  }

  return (channel == NULL) ? ret : MSG_TIMEOUT;
}

/**
 * @brief   Implementation of the BaseChannel wrtitet() method.
 */
static size_t _aosShellStreamWritet(void *instance, const uint8_t *bp, size_t n, sysinterval_t time)
{
  aosDbgCheck(instance != NULL);

  // local variables
  systime_t currt = chVTGetSystemTime();
  const systime_t callt = currt;
  const systime_t timeoutt = chTimeAddX(currt, time);
  AosShellChannel* channel = ((AosShellStream*)instance)->channel;
  msg_t ret = MSG_OK;

  // iterate through list of channels
  for (; channel != NULL && chTimeIsInRangeX(currt, callt, timeoutt); channel = channel->next) {
    const msg_t ret_ = chnWriteTimeout(channel, bp, n, chTimeDiffX(currt, timeoutt));
    ret = (ret_ < ret) ? ret_ : ret;
    currt = chVTGetSystemTime();
  }

  return (channel == NULL) ? ret : MSG_TIMEOUT;
}

/**
 * @brief   Implementation of the BaseChannel ctl() method.
 */
static msg_t _aosShellStreamCtl(void *instance, unsigned int operation, void *arg)
{
  aosDbgCheck(instance != NULL);

  // local variables
  msg_t ret = MSG_OK;

  // iterate through list of channels
  for (AosShellChannel* channel = ((AosShellStream*)instance)->channel; channel != NULL; channel = channel->next) {
    const msg_t ret_ = chnControl(channel, operation, arg);
    ret = (ret_ < ret) ? ret_ : ret;
  }

  return ret;
}

/**
 * @brief   Initialize an ShellStream object.
 *
 * @param[in] stream  The AosShellStrem to initialize.
 */
void _aosShellStreamInit(AosShellStream* stream)
{
  aosDbgCheck(stream != NULL);

  stream->vmt = &_streamvmt;
  stream->channel = NULL;

  return;
}

/**
 * @brief   Suspend input from all channels of a stream, except for one.
 *
 * @param[in] stream  Stream to suspend all channels of.
 * @param[in] except  Channel not to be suspended (may be NULL).
 */
void _aosShellStreamSuspendAllChannelsBut(AosShellStream* stream, AosShellChannel* except)
{
  aosDbgCheck(stream != NULL);

  for (AosShellChannel* c = stream->channel; c != NULL; c = c->next) {
    if (c != except) {
      _aosShellChannelSuspend(c);
    }
  }

  return;
}

/**
 * @brief   Resume all suspended channels of a stream.
 *
 * @param[in] stream  Stream to release all channels of.
 */
void _aosShellStreamResumeSuspendedChannels(AosShellStream* stream)
{
  aosDbgCheck(stream != NULL);

  // iterate through all channels
  for (AosShellChannel* c = stream->channel; c != NULL; c = c->next) {
    if (c->flags & SHELLCHANNEL_INPUT_SUSPENDED) {
      _aosShellChannelResume(c);
    }
  }

  return;
}

/* commands *******************************************************************/

/**
 * @brief   Count the number of commands assigned to the shell.
 *
 * @return  The number of commands associated to the shell.
 */
size_t _aosShellCommandsCount(void)
{
  // local variables
  size_t count = 0;

  chMtxLock(&_commands_mtx);
  for (aos_shellcommand_t* cmd = _commands; cmd != NULL; cmd = cmd->next) {
    ++count;
  }
  chMtxUnlock(&_commands_mtx);

  return count;
}

/**
 * @brief   Builtin information command callback function.
 *
 * @param[in] stream    Stream to print to.
 * @param[in] argc      Number of arguments.
 * @param[in] argv      List of arguments.
 * @param[in] params    Pointer to a shell object.
 *
 * @return  The operation result.
 */
static int _aosShellCommandInfoCallback(BaseSequentialStream* stream, int argc, const char* argv[])
{
  aosDbgCheck(stream != NULL);

  (void)argc;
  (void)argv;

  // local variables
  aos_shell_t* const shell = _aosShellRetrieveFromThread(chThdGetSelfX());

  if (shell->name != NULL) {
    chprintf(stream, "                  name: %s\n", shell->name);
  }
#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true)
  chprintf(stream, "     remote identifier: ");
  _aosShellRemotePrintId(stream, shell->remote.id);
  chprintf(stream, "\n");
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */
  chprintf(stream, "    commands available: %u\n", _aosShellCommandsCount());
  chprintf(stream, "           input width: %u characters\n", AMIROOS_CFG_SHELL_LINEWIDTH);
  chprintf(stream, "     maximum arguments: %u\n", AMIROOS_CFG_SHELL_MAXARGS);
  chprintf(stream, "          history size: %u\n", BUFFER_ENTRIES);
#if (AMIROOS_CFG_DBG == true)
  chprintf(stream, "            stack size: %u bytes\n", aosThdGetStacksize(chThdGetSelfX()));
#if (CH_DBG_FILL_THREADS == TRUE)
  {
    const size_t utilization = aosThdGetStackPeakUtilization(chThdGetSelfX());
    chprintf(stream, "stack peak utilization: %u bytes (%.2f%%)\n", utilization, (float)utilization / (float)(aosThdGetStacksize(chThdGetSelfX())) * 100.0f);

    chprintf(stream, "stack peak utilization: %u bytes (%.2f%%) (%u)\n", utilization, (float)utilization / (float)(aosThdGetStacksize(chThdGetSelfX())) * 100.0f, aosThdGetStackPeakUtilization2(chThdGetSelfX()));
  }
#endif /* (CH_DBG_FILL_THREADS == TRUE) */
#endif /* (AMIROOS_CFG_DBG == true) */

  return 0;
}

static int _aosShellCommandHelpCallback(BaseSequentialStream* stream, int argc, const char* argv[])
{
  aosDbgCheck(stream != NULL);

  (void)argc;
  (void)argv;

  // just print a help text
  chprintf(stream, "How to use the AMiRo-OS shell:\n");
  streamPut(stream, '\n');
  chprintf(stream, "BACKSPACE/DELETE\n");
  chprintf(stream, "  delete left/right\n");
  chprintf(stream, "INSERT\n");
  chprintf(stream, "  toggle insert/overwrite mode\n");
  chprintf(stream, "ENTER\n");
  chprintf(stream, "  execute command\n");
  chprintf(stream, "ARROW LEFT/RIGHT\n");
  chprintf(stream, "  move cursor one character left/right\n");
  chprintf(stream, "CTRL + ARROW LEFT/RIGHT\n");
  chprintf(stream, "  move cursor one word left/right\n");
  chprintf(stream, "HOME/END\n");
  chprintf(stream, "  move cursor to the very left/right\n");
  chprintf(stream, "ARROW UP\n");
  chprintf(stream, "  recall previous input from history\n");
  chprintf(stream, "ARROW DOWN\n");
  chprintf(stream, "  recall subsequent input from history or clear the current input\n");
  chprintf(stream, "CTRL + ARROW UP\n");
  chprintf(stream, "  recall oldest input from history\n");
  chprintf(stream, "CTRL + ARROW DOWN\n");
  chprintf(stream, "  recall or clear the current input\n");
  chprintf(stream, "PAGE UP/DOWN\n");
  chprintf(stream, "  identical to CRTL + ARROW UP/DOWN\n");
  chprintf(stream, "TAB\n");
  chprintf(stream, "  1st time: auto-complete current input\n");
  chprintf(stream, "  consecutively: suggest matching commands\n");
  streamPut(stream, '\n');

  return 0;
}

/**
 * @brief   Builtin configuration command callback function.
 *
 * @param[in] stream    Stream to print to.
 * @param[in] argc      Number of arguments.
 * @param[in] argv      List of arguments.
 *
 * @return  The operation result.
 * @retval AOS_OK                 Configuration successful.
 * @retval AOS_INVALIDARGUMENTS   User passed invalid arguments.
 */
static int _aosShellCommandConfigCallback(BaseSequentialStream* stream, int argc, const char* argv[])
{
  aosDbgCheck(stream != NULL);

  // local variables
  aos_shell_t* const shell = _aosShellRetrieveFromThread(chThdGetSelfX());
  const char* unknown_option = NULL;

  // if there are additional arguments
  if (argc > 1) {
    // if the user wants to configure the prompt
    if (strcmp(argv[1], "-p") == 0 || strcmp(argv[1], "--prompt") == 0) {
      // there must be a further argument
      if (argc > 2) {
        // handle the option
        if (strcmp(argv[2], "text") == 0) {
          shell->config &= ~SHELL_CONFIG_PROMPT_MINIMAL;
          return AOS_OK;
        }
        else if (strcmp(argv[2], "minimal") == 0) {
          shell->config |= SHELL_CONFIG_PROMPT_MINIMAL;
          return AOS_OK;
        }
        else if (strcmp(argv[2], "notime") == 0) {
          shell->config &= ~(SHELL_CONFIG_PROMPT_UPTIME | SHELL_CONFIG_PROMPT_DATETIME);
          return AOS_OK;
        }
        else if (strcmp(argv[2], "uptime") == 0) {
          shell->config &= ~SHELL_CONFIG_PROMPT_DATETIME;
          shell->config |= SHELL_CONFIG_PROMPT_UPTIME;
          return AOS_OK;
        }
        else if (strcmp(argv[2], "date&time") == 0) {
          shell->config &= ~SHELL_CONFIG_PROMPT_UPTIME;
          shell->config |= SHELL_CONFIG_PROMPT_DATETIME;
          return AOS_OK;
        }
        else {
          unknown_option = argv[2];
        }
      }
    }
    // if the user wants to configure string matching
    else if (strcmp(argv[1], "-m") == 0 || strcmp(argv[1], "--match") == 0) {
      // if there is a further argument
      if (argc > 2) {
        if (strcmp(argv[2], "casesensitive") == 0 || strcmp(argv[2], "cs") == 0) {
          shell->config |= SHELL_CONFIG_MATCHCASE;
          return AOS_OK;
        }
        else if (strcmp(argv[2], "caseinsensitive") == 0 || strcmp(argv[2], "ci") == 0) {
          shell->config &= ~SHELL_CONFIG_MATCHCASE;
          return AOS_OK;
        }
        else {
          unknown_option = argv[2];
        }
      }
      // if there is no further argument
      else {
        // print current case matching configuration.
        chprintf(stream, "currently %s\n", (shell->config & SHELL_CONFIG_MATCHCASE) ? "casesensitive" : "caseinsesitive");
        return AOS_OK;
      }
    }
    else if (aosShellcmdCheckHelp(argc, argv, true) != AOS_SHELL_HELPREQUEST_EXPLICIT) {
      unknown_option = argv[1];
    }
  }

  // if an unknown option was detected
  if (unknown_option != NULL) {
    // error
    chprintf(stream, "unknown option '%s'\n", unknown_option);
  }

  // print help
  chprintf(stream, "Usage: %s OPTION [VAL]\n", argv[0]);
  chprintf(stream, "Options:\n");
  chprintf(stream, "  --help, -h\n");
  chprintf(stream, "    Print this help text.\n");
  chprintf(stream, "  --prompt, -p [text|minimal|uptime|date&time|notime]\n");
  chprintf(stream, "    Configures the prompt.\n");
  chprintf(stream, "  --match, -m [casesensitive|cs|caseinsenitive|ci]\n");
  chprintf(stream, "    Configures string matching.\n");

  return (aosShellcmdCheckHelp(argc, argv, true) == AOS_SHELL_HELPREQUEST_EXPLICIT) ? AOS_OK : AOS_INVALIDARGUMENTS;
}

#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) || defined(__DOXYGEN__)

/**
 * @brief   Builtin shell command for remote access handling.
 * @note    For details on the protocol, please refer to the comment block in
 *          the @p readChannel() method.
 *
 * @param[in] stream  Stream to print to.
 * @param[in] argc    Number of arguments.
 * @param[in] argv    Argument list.
 *
 * @return  Operation result.
 */
static int _aosShellCommandRemoteCallback(BaseSequentialStream* stream, int argc, const char* argv[])
{
  aosDbgCheck(stream != NULL);

  // local variables
  aos_shell_t* const shell = _aosShellRetrieveFromThread(chThdGetSelfX());
  float timeout = 0.1;

  aosDbgAssertMsg((shell->operation.flags & SHELL_OPFLAGS_REMOTEACTION_MASK) == SHELL_OPFLAGS_REMOTEACTION_NONE, "remote busy");

  // print the help text
  if (aosShellcmdCheckHelp(argc, argv, true) != AOS_SHELL_HELPREQUEST_NONE) {
    chprintf(stream,"Usage: %s OPTION [ID] [TIMEOUT=1]\n", argv[0]);
    chprintf(stream, "Options:\n");
    chprintf(stream, "  --help, -h\n");
    chprintf(stream, "    Print this help text.\n");
    chprintf(stream, "  --available, -a\n");
    chprintf(stream, "    Lists all available remote shells to connect to.\n");
    chprintf(stream, "  --list, -l\n");
    chprintf(stream, "    Lists all remote shells which are currently client or server.\n");
    chprintf(stream, "  --connect, -c ID\n");
    chprintf(stream, "    Tries to connect to the remote shell 'ID'.\n");
    chprintf(stream, "  --disconnect, -d {ID,*}\n");
    chprintf(stream, "    Disconnect channel with client 'ID' or all channels (ID='*').\n");
    chprintf(stream, "TIMEOUT defaults to 0.1 seconds.\n");

    return (aosShellcmdCheckHelp(argc, argv, true) == AOS_SHELL_HELPREQUEST_EXPLICIT) ? AOS_OK : AOS_INVALIDARGUMENTS;
  }

  // discover
  else if (strcmp(argv[1], "-a") == 0 || strcmp(argv[1], "--available") == 0) {
    // check for optional timeout argument
    if (argc > 2 && (fpclassify(atof(argv[2])) == FP_NORMAL || fpclassify(atof(argv[2])) == FP_ZERO) && atof(argv[2]) >= 0.0f) {
      timeout = atof(argv[2]);
    }
    // iterate through all remote channels
    const char msg[] = {REMOTEMSG_IDENTIFIER, REMOTEMSG_IDENTIFIER_DISCOVERREQUEST};
    for (AosShellChannel* c = shell->stream.channel; c != NULL; c = c->next) {
      if (c->flags & AOS_SHELLCHANNEL_REMOTE) {
        // temporarily enforce output and send request
        c->flags |= SHELLCHANNEL_OUTPUT_OVERRIDE;
        chnWrite(c, (uint8_t*)msg, sizeof(msg));
        // disable output for all non-remote channels
        if (!(c->flags & SHELLCHANNEL_REMOTE_ESTABLISHED)) {
          c->flags &= ~SHELLCHANNEL_OUTPUT_OVERRIDE;
        }
      }
    }
    // setup timeout timer
    aosTimerSetLongInterval(&shell->remote.timer, timeout * MICROSECONDS_PER_SECOND, _aosShellRemoteTimerCallback, &shell->remote.evtSource);
    // set operation flags
    shell->operation.flags |= SHELL_OPFLAGS_REMOTEACTION_DISCOVER;

    return AOS_OK;
  }

  // list clients
  else if (strcmp(argv[1], "-l") == 0 || strcmp(argv[1], "--list") == 0) {
    // check for optional timeout argument
    if (argc > 2 && (fpclassify(atof(argv[2])) == FP_NORMAL || fpclassify(atof(argv[2])) == FP_ZERO) && atof(argv[2]) >= 0.0f) {
      timeout = atof(argv[2]);
    }
    // iterate through all remote channels
    const char msg[] = {REMOTEMSG_IDENTIFIER, REMOTEMSG_IDENTIFIER_CLIENTSREQUEST};
    for (AosShellChannel* c = shell->stream.channel; c != NULL; c = c->next) {
      // if this shell is server on the channel
      if (c->flags & SHELLCHANNEL_REMOTE_ESTABLISHED && c != shell->operation.remote.channel) {
        chnWrite(c, (uint8_t*)msg, sizeof(msg));
      }
    }
    // setup timeout timer
    aosTimerSetLongInterval(&shell->remote.timer, timeout * MICROSECONDS_PER_SECOND, _aosShellRemoteTimerCallback, &shell->remote.evtSource);
    // set operation flags
    shell->operation.flags |= SHELL_OPFLAGS_REMOTEACTION_LISTCLIENTS;

    return AOS_OK;
  }

  // connect
  else if (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "--connect") == 0) {
    // check for ID argument
    if (argc < 3) {
      chprintf(stream, "no ID specified\n");
      return AOS_INVALIDARGUMENTS;
    }
    // check for optional timeout argument
    if (argc > 3 && (fpclassify(atof(argv[3])) == FP_NORMAL || fpclassify(atof(argv[3])) == FP_ZERO) && atof(argv[3]) >= 0.0f) {
      timeout = atof(argv[3]);
    }
    // try to parse identifier argument
    shell->operation.remote.requested = _aosShellRemoteParseId(argv[2]);
    if (shell->operation.remote.requested == REMOTEID_INVALID || shell->operation.remote.requested == shell->remote.id) {
      chprintf(stream, "ID invalid\n");
      shell->operation.remote.requested = REMOTEID_INVALID;
      return AOS_INVALIDARGUMENTS;
    } else {
      chprintf(stream, "connecting to ");
      _aosShellRemotePrintId(stream, shell->operation.remote.requested);
      chprintf(stream, "...");
    }
    // setup and transmit message
    char str[AOS_SHELL_RMSG_MAXLENGTH];
    _aosShellRemoteSetupMessage(str, (remote_message_t){shell->operation.remote.requested, RMSG_CONNECT});
    // iterate through all remote channels
    for (AosShellChannel* c = shell->stream.channel; c != NULL; c = c->next) {
      if (c->flags & AOS_SHELLCHANNEL_REMOTE) {
        // temporarily enforce output and send request
        c->flags |= SHELLCHANNEL_OUTPUT_OVERRIDE;
        chnWrite(c, (uint8_t*)str, AOS_SHELL_RMSG_MAXLENGTH);
        // disable output for all non-remote channels
        if (!(c->flags & SHELLCHANNEL_REMOTE_ESTABLISHED)) {
          c->flags &= ~SHELLCHANNEL_OUTPUT_OVERRIDE;
        }
      }
    }
    // setup timeout timer
    aosTimerSetLongInterval(&shell->remote.timer, timeout * MICROSECONDS_PER_SECOND, _aosShellRemoteTimerCallback, &shell->remote.evtSource);
    // set operation flags
    shell->operation.flags |= SHELL_OPFLAGS_REMOTEACTION_CONNECT;

    return AOS_OK;
  }

  // disconnect
  else if (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--disconnect") == 0) {
    // check for ID argument
    if (argc < 3) {
      chprintf(stream, "no ID specified\n");
      return AOS_INVALIDARGUMENTS;
    }
    // check for optional timeout argument
    if (argc > 3 && (fpclassify(atof(argv[3])) == FP_NORMAL || fpclassify(atof(argv[3])) == FP_ZERO) && atof(argv[3]) >= 0.0f) {
      timeout = atof(argv[3]);
    }
    // if user wants to disconnect all channels
    if (strcmp(argv[2], "*") == 0) {
      // reset server channel
      shell->operation.remote.channel = NULL;
      // cancel all remote connections
      const char msg[] = {REMOTEMSG_IDENTIFIER, REMOTEMSG_IDENTIFIER_CANCEL};
      for (AosShellChannel* c = shell->stream.channel; c != NULL; c = c->next) {
        if (c->flags & SHELLCHANNEL_REMOTE_ESTABLISHED) {
          // emit cancelation message and reset flags
          chnWrite(c, (uint8_t*)msg, sizeof(msg));
          c->flags &= ~(SHELLCHANNEL_REMOTE_ESTABLISHED | SHELLCHANNEL_OUTPUT_OVERRIDE);
        }
      }
    }
    // if the user wants to disconnect a specific client
    else {
      // try to parse identifier argument
      shell->operation.remote.requested = _aosShellRemoteParseId(argv[2]);
      if (shell->operation.remote.requested == REMOTEID_INVALID || shell->operation.remote.requested == shell->remote.id) {
        chprintf(stream, "ID invalid\n");
        shell->operation.remote.requested = REMOTEID_INVALID;
        return AOS_INVALIDARGUMENTS;
      } else {
        chprintf(stream, "searching for ");
        _aosShellRemotePrintId(stream, shell->operation.remote.requested);
        chprintf(stream, "...");
      }
      // search for the specified client
      const char msg[] = {REMOTEMSG_IDENTIFIER, REMOTEMSG_IDENTIFIER_CLIENTSREQUEST};
      for (AosShellChannel* c = shell->stream.channel; c != NULL; c = c->next) {
        // if this shell is server on the channel
        if (c->flags & AOS_SHELLCHANNEL_REMOTE && c != shell->operation.remote.channel) {
          // do not enforce output, so communications in which this shell is not involved are maintained
          chnWrite(c, (uint8_t*)msg, sizeof(msg));
        }
      }
      // setup timeout timer
      aosTimerSetLongInterval(&shell->remote.timer, timeout * MICROSECONDS_PER_SECOND, _aosShellRemoteTimerCallback, &shell->remote.evtSource);
      // set operation flags
      shell->operation.flags |= SHELL_OPFLAGS_REMOTEACTION_DISCONNECT;
    }
    return AOS_OK;
  }

  // invalid
  else {
    chprintf(stream, "unknown argument '%s'\n", argv[1]);
    return AOS_INVALIDARGUMENTS;
  }
}

#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */

/* input & history ************************************************************/

/**
 * @brief   Retrieve a pointer to the string buffer of a specified entry in the
 *          input buffer.
 *
 * @param[in] shell   Pointer to a shell object.
 * @param[in] entry   Entry to be retrieved.
 *
 * @return  Pointer to the entry in the input buffer.
 */
char* _aosShellGetAbsoluteEntry(aos_shell_t* shell, size_t entry)
{
  aosDbgCheck(shell != NULL);
  aosDbgCheck(entry < BUFFER_ENTRIES);

  return &(shell->buffer[entry * AMIROOS_CFG_SHELL_LINEWIDTH * sizeof(char)]);
}

/**
 * @brief   Calculate absolute entry from history offset.
 *
 * @param[in] shell   Pointer to a shell object.
 * @param[in] offset  Relative offset of the entry to be retrieved.
 *
 * @return  Absolute index of the historic entry.
 */
size_t _aosShellHistoryOffset2EntryIndex(const aos_shell_t* shell, size_t offset)
{
  aosDbgCheck(shell != NULL);
  aosDbgCheck(offset < BUFFER_ENTRIES);

  return ((shell->operation.history.current + BUFFER_ENTRIES - offset) % BUFFER_ENTRIES);
}

/**
 * @brief   Retrieve a pointer to the string buffer of a historic entry in the
 *          input buffer.
 *
 * @param[in] shell   Pointer to a shell object.
 * @param[in] offset  Relative offset of the entry to be retrieved.
 *
 * @return  Pointer to the entry in the input buffer.
 */
char* _aosShellGetRelativeEntry(aos_shell_t* shell, size_t offset)
{
  aosDbgCheck(shell != NULL);
  aosDbgCheck(offset < BUFFER_ENTRIES);

  return _aosShellGetAbsoluteEntry(shell, _aosShellHistoryOffset2EntryIndex(shell, offset));
}

/**
 * @brief   Retrieve a pointer to the current entry string in the input buffer.
 *
 * @param[in] shell   Pointer to a shell object.
 *
 * @return  Pointer to the string of the current entry in the input buffer.
 */
char* _aosShellGetCurrentEntry(aos_shell_t* shell)
{
  aosDbgCheck(shell != NULL);

  return _aosShellGetAbsoluteEntry(shell, shell->operation.history.current);
}

/**
 * @brief   Retrieve a pointer to the currently selected entry.
 *
 * @param[in] shell   Pointer to a shell object.
 *
 * @return  Pointer to the currently selected entry or NULL if no entry is
 *          selected (cleared preview).
 */
const char* _aosShellGetSelectedEntry(aos_shell_t* shell)
{
  aosDbgCheck(shell != NULL);

  if (shell->operation.history.selected > 0) {
    return _aosShellGetRelativeEntry(shell, shell->operation.history.selected - 1);
  } else {
    return NULL;
  }
}

/**
 * @brief   Retrieve the currently visualized entry.
 *
 * @param[in] shell   Pointer to a shell object.
 *
 * @return  Pointer to the currently visualized entry or NULL if the input has
 *          been cleared (cleared preview) or there was no input yet.
 */
char* _aosShellGetVisualisedEntry(aos_shell_t* shell)
{
  aosDbgCheck(shell != NULL);

  if (shell->operation.history.selected == 0) {
    // cleared preview, nothing visualized
    return NULL;
  } else {
    char* line;
    if (shell->operation.history.selected == 1 || shell->operation.history.selected == shell->operation.history.edited) {
      // the current or a modified entry is selected
      line = _aosShellGetCurrentEntry(shell);
    } else {
      // a historic, unmodified entry is selected
      line = _aosShellGetRelativeEntry(shell, shell->operation.history.selected - 1);
    }
    // return NULL if the entry is unutilized yet (invalid entry = hidden)
    return (line && line[0] != INBUF_INIT_CHAR) ? line : NULL;
  }
}

/**
 * @brief   Parses the content of the given string to separate arguments.
 *
 * @param[in]   shell   Pointer to the shell object.
 * @param[in]   str     String to be parsed.
 *
 * @return            Number of arguments found.
 */
size_t _aosShellParseArguments(aos_shell_t* shell, char* str)
{
  aosDbgCheck(shell != NULL);
  aosDbgCheck(str != NULL);

  /*
   * States for a very small FSM.
   */
  typedef enum {
    START,
    SPACE,
    TEXT,
    END,
  } state_t;

  // local variables
  state_t state = START;
  size_t nargs = 0;

  // iterate through the line
  for (size_t c = 0; c < AMIROOS_CFG_SHELL_LINEWIDTH; ++c) {
    // terminate at first NUL byte
    if (str[c] == ASCII_NUL) {
      state = END;
      break;
    }
    // spaces become NUL bytes
    else if (str[c] == ' ') {
      str[c] = ASCII_NUL;
      state = SPACE;
    }
    // handle non-NUL bytes
    else {
      switch (state) {
        case START:
        case SPACE:
          // ignore too many arguments
          if (nargs < AMIROOS_CFG_SHELL_MAXARGS) {
            shell->operation.input.argbuf[nargs] = &str[c];
          }
          ++nargs;
          break;
        case TEXT:
        case END:
          break;
      }
      state = TEXT;
    }
  }

  // set all remaining argument pointers to NULL
  for (size_t a = nargs; a < AMIROOS_CFG_SHELL_MAXARGS; ++a) {
    shell->operation.input.argbuf[a] = NULL;
  }

  return nargs;
}

/* char & string handling *****************************************************/

/**
 * @brief   Compare two characters.
 *
 * @param[in] lhs       First character to compare.
 * @param[in] rhs       Second character to compare.
 *
 * @return              How well the characters match.
 */
charmatch_t _aosShellCharcmp(char lhs, char rhs)
{
  // if lhs is a upper case letter and rhs is a lower case letter
  if (lhs >= 'A' && lhs <= 'Z' && rhs >= 'a' && rhs <= 'z') {
    return (lhs == (rhs - 'a' + 'A')) ? CHAR_MATCH_NCASE : CHAR_MATCH_NOT;
  }
  // if lhs is a lower case letter and rhs is a upper case letter
  else if (lhs >= 'a' && lhs <= 'z' && rhs >= 'A' && rhs <= 'Z') {
    return ((lhs - 'a' + 'A') == rhs) ? CHAR_MATCH_NCASE : CHAR_MATCH_NOT;
  }
  // default
  else {
    return (lhs == rhs) ? CHAR_MATCH_CASE : CHAR_MATCH_NOT;
  }
}

/**
 * @brief   Maps an character from ASCII to a modified custom encoding.
 * @details The custom character encoding is very similar to ASCII and has the
 *          following structure:
 *          0x00=NULL ... 0x40='@' (identically to ASCII)
 *          0x4A='a'; 0x4B='A'; 0x4C='b'; 0x4D='B' ... 0x73='z'; 0x74='Z' (custom letter order)
 *          0x75='[' ... 0x7A='`' (0x5B..0x60 is ASCII)
 *          0x7B='{' ... 0x7F=DEL (identically to ASCII)
 *
 * @param[in] c   Character to map to the custom encoding.
 *
 * @return    The customly encoded character.
 */
char _aosShellMapAscii2Custom(char c)
{
  if (c >= 'A' && c <= 'Z') {
    return ((c - 'A') * 2) + 'A' + 1;
  } else if (c > 'Z' && c < 'a') {
    return c + ('z' - 'a') + 1;
  } else if (c >= 'a' && c <= 'z') {
    return ((c - 'a') * 2) + 'A';
  } else {
    return c;
  }
}

/**
 * @brief   Compares two strings wrt. letter case.
 * @details Comparisson uses a custom character encoding or mapping.
 *          See @p _aosShellMapAscii2Custom() for details.
 *
 * @param[in] str1    First string to compare.
 * @param[in] str2    Second string to compare.
 * @param[in] cs      Flag indicating whether comparison shall be case
 *                    sensitive.
 * @param[in,out] n   Maximum number of character to compare (in) and number of
 *                    matching characters (out). If a null pointer is specified,
 *                    this parameter is ignored. If the value pointed to is
 *                    zero, comarison will not be limited.
 * @param[out] m      Optional indicator whether there was at least one case
 *                    mismatch.
 *
 * @return      Integer value indicating the relationship between the strings.
 * @retval <0   The first character that does not match has a lower value in
 *              str1 than in str2.
 * @retval  0   The contents of both strings are equal.
 * @retval >0   The first character that does not match has a greater value in
 *              str1 than in str2.
 */
int _aosShellStrccmp(const char *str1, const char *str2, bool cs, size_t* n, charmatch_t* m)
{
  aosDbgCheck(str1 != NULL);
  aosDbgCheck(str2 != NULL);

  // initialize variables
  if (m) {
    *m = CHAR_MATCH_NOT;
  }
  size_t i = 0;

  // iterate through the strings
  while ((n == NULL) || (*n == 0) || (*n > 0 && i < *n)) {
    // break on NUL
    if (str1[i] == ASCII_NUL || str2[i] == ASCII_NUL) {
      if (n) {
        *n = i;
      }
      break;
    }
    // compare character
    const charmatch_t match = _aosShellCharcmp(str1[i], str2[i]);
    if ((match == CHAR_MATCH_CASE) || (!cs && match == CHAR_MATCH_NCASE)) {
      if (m != NULL && *m != CHAR_MATCH_NCASE) {
        *m = match;
      }
      ++i;
    } else {
      if (n) {
        *n = i;
      }
      break;
    }
  }

  return _aosShellMapAscii2Custom(str1[i]) - _aosShellMapAscii2Custom(str2[i]);
}

/* escape sequence ************************************************************/

/**
 * @brief   Interprete a escape sequence
 * @details This function interpretes escape sequences (starting with ASCII
 *          "Escape" character 0x1B) according to the VT100 / VT52 ANSI escape
 *          sequence definitions.
 * @note    Only the most important escape sequences are implemented yet.
 *
 * @param[in] seq   Character sequence to interprete.
 * @param[in] len   Length of the sequence string.
 *                  If 0, it is determined from the string.
 *
 * @return          A @p special_key value.
 */
special_key_t _aosShellInterpreteEscapeSequence(const char seq[], size_t len)
{
  // local variables
  size_t strl = 0;
  bool ambiguous = false;
  if (len == 0) {
    len = strlen(seq);
  }

  // TAB
  /* not supported yet; use ASCII_HT instead */

  // BACKSPACE
  /* not supported yet; use ASCII_BS instead */

  // ESCAPE
  /* not supported yet; use ASCII_ESC instead */

  // CTRL + C
  /* not defined yet; use ASCII_ETX instead */

  // INSERT
  if (strncmp(seq, KEYSTRING_INSERT, len) == 0) {
    strl = strlen(KEYSTRING_INSERT);
    if (len == strl) {
      return KEY_INSERT;
    } else if (len < strl) {
      ambiguous = true;
    }
  }

  // DELETE
  if (strncmp(seq, KEYSTRING_DELETE, len) == 0) {
    strl = strlen(KEYSTRING_DELETE);
    if (len == strl) {
      return KEY_DELETE;
    } else if (len < strl) {
      ambiguous = true;
    }
  }

  // HOME
  if (strncmp(seq, KEYSTRING_HOME, len) == 0) {
    strl = strlen(KEYSTRING_HOME);
    if (len == strl) {
      return KEY_HOME;
    } else if (len < strl) {
      ambiguous = true;
    }
  }

  // END
  if (strncmp(seq, KEYSTRING_END, len) == 0) {
    strl = strlen(KEYSTRING_END);
    if (len == strl) {
      return KEY_END;
    } else if (len < strl) {
      ambiguous = true;
    }
  }

  // PAGE UP
  if (strncmp(seq, KEYSTRING_PAGEUP, len) == 0) {
    strl = strlen(KEYSTRING_PAGEUP);
    if (len == strl) {
      return KEY_PAGEUP;
    } else if (len < strl) {
      ambiguous = true;
    }
  }

  // PAGE DOWN
  if (strncmp(seq, KEYSTRING_PAGEDOWN, len) == 0) {
    strl = strlen(KEYSTRING_PAGEDOWN);
    if (len == strl) {
      return KEY_PAGEDOWN;
    } else if (len < strl) {
      ambiguous = true;
    }
  }

  // ARROW UP
  if (strncmp(seq, KEYSTRING_ARROWUP, len) == 0) {
    strl = strlen(KEYSTRING_ARROWUP);
    if (len == strl) {
      return KEY_ARROWUP;
    } else if (len < strl) {
      ambiguous = true;
    }
  }

  // ARROW DOWN
  if (strncmp(seq, KEYSTRING_ARROWDOWN, len) == 0) {
    strl = strlen(KEYSTRING_ARROWDOWN);
    if (len == strl) {
      return KEY_ARROWDOWN;
    } else if (len < strl) {
      ambiguous = true;
    }
  }

  // ARROW LEFT
  if (strncmp(seq, KEYSTRING_ARROWLEFT, len) == 0) {
    strl = strlen(KEYSTRING_ARROWLEFT);
    if (len == strl) {
      return KEY_ARROWLEFT;
    } else if (len < strl) {
      ambiguous = true;
    }
  }

  // ARROW RIGHT
  if (strncmp(seq, KEYSTRING_ARROWRIGHT, len) == 0) {
    strl = strlen(KEYSTRING_ARROWRIGHT);
    if (len == strl) {
      return KEY_ARROWRIGHT;
    } else if (len < strl) {
      ambiguous = true;
    }
  }

  // CTRL + ARROW UP
  if (strncmp(seq, KEYSTRING_CTRL_ARROWUP, len) == 0) {
    strl = strlen(KEYSTRING_CTRL_ARROWUP);
    if (len == strl) {
      return KEY_CTRL_ARROWUP;
    } else if (len < strl) {
      ambiguous = true;
    }
  }

  // CTRL + ARROW DOWN
  if (strncmp(seq, KEYSTRING_CTRL_ARROWDOWN, len) == 0) {
    strl = strlen(KEYSTRING_CTRL_ARROWDOWN);
    if (len == strl) {
      return KEY_CTRL_ARROWDOWN;
    } else if (len < strl) {
      ambiguous = true;
    }
  }

  // CTRL + ARROW LEFT
  if (strncmp(seq, KEYSTRING_CTRL_ARROWLEFT, len) == 0) {
    strl = strlen(KEYSTRING_CTRL_ARROWLEFT);
    if (len == strl) {
      return KEY_CTRL_ARROWLEFT;
    } else if (len < strl) {
      ambiguous = true;
    }
  }

  // CTRL + ARROW RIGHT
  if (strncmp(seq, KEYSTRING_CTRL_ARROWRIGHT, len) == 0) {
    strl = strlen(KEYSTRING_CTRL_ARROWRIGHT);
    if (len == strl) {
      return KEY_CTRL_ARROWRIGHT;
    } else if (len < strl) {
      ambiguous = true;
    }
  }

  return ambiguous ? KEY_AMBIGUOUS : KEY_UNKNOWN;
}

/* remote *********************************************************************/
#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) || defined(__DOXYGEN__)

/**
 * @brief   Serialize a remote identifier.
 *
 * @param[out] buf  Buffer to write the serialized identifier to.
 * @param[in]  id   Identifier to serialize.
 */
void _aosShellRemoteSerializeId(uint8_t* buf, aos_shellremoteid_t id)
{
  aosDbgCheck(buf != NULL);

  // serialize in little-endian format
  for (size_t byte = 0; byte < AOS_SHELL_REMOTEIDWIDTH; ++byte) {
    buf[byte] = id & 0xFF;
    id = id >> 8;
  }

  return;
}

/**
 * @brief   Deserialize a remote identifier from a buffer.
 *
 * @param[in] buf   Buffer containing a serialized identifier.
 *
 * @return  The deserialized identifier.
 */
aos_shellremoteid_t _aosShellRemoteDeserialseId(const uint8_t* buf)
{
  aosDbgCheck(buf != NULL);

  // deserialize from little-endian format
  aos_shellremoteid_t id = 0;
  for (size_t byte = 0; byte < AOS_SHELL_REMOTEIDWIDTH; ++byte) {
    id += ((aos_shellremoteid_t)buf[byte]) << (byte * 8);
  }

  return id;
}

/**
 * @brief   Print a remote identifier to a stream.
 *
 * @param[in] stream  Stream to print to.
 * @param[in] id      Identifier toi print.
 *
 * @return  Number of characters printed.
 */
int _aosShellRemotePrintId(BaseSequentialStream* stream, aos_shellremoteid_t id)
{
  aosDbgCheck(stream != NULL);

  // local variables
  int c = 0;

  // print the identifier
  for (size_t byte = 0; byte < AOS_SHELL_REMOTEIDWIDTH; ++byte) {
    c += chprintf(stream, "%02X", (id >> (AOS_SHELL_REMOTEIDWIDTH - (byte+1)) * 8) & 0xFF);
    if (byte + 1 < AOS_SHELL_REMOTEIDWIDTH) {
      streamPut(stream, ':');
      ++c;
    }
  }

  return c;
}

/**
 * @brief   Parse a string and extract a remote identifier.
 * @details The identifier string can be formatted in the following ways:
 *          - decimal number, e.g.
 *            - "0123456789"
 *          - hexadecimal number, e.g.
 *            - "0x123456789ABCDEF"
 *            - "0x123456789abcdef"
 *            - "123456789ABCDEF"
 *            - "0x123456789abcdef"
 *          - point separated (byte-wise decimals)
 *            - "1.2.3.4"
 *            - "1.1" -> 1.0.0.1
 *            - "1.1.1" -> 1.1.0.1
 *          - colon separated (byte-wise hexadecimals)
 *            - "1:2:3:4"
 *            - "1:1" -> 01:00:00:01
 *            - "1:1:1" -> 01:01:00:01
 *          Note that in case of single-byte identifiers, the separated
 *          formattings are not supported, obviously.
 *
 * @param[in] str   String to parse.
 *
 * @return  The extracted remote identifier or @p REMOTEID_INVALID if extraction
 *          failed.
 */
aos_shellremoteid_t _aosShellRemoteParseId(const char* str)
{
  // local variables
  const size_t len = strlen(str);
  uint8_t cnt = 0;
  enum {
    UNKNOWN,
    INVALID,
    DECIMAL,
    HEXADECIMAL,
    BYTEWISE_DECIMAL,
    BYTEWISE_HEXADECIMAL,
  } format = UNKNOWN;
  aos_shellremoteid_t id = 0;

  // retrieve format
  for (size_t c = 0; c < len && format != INVALID; ++c) {
    if (isdigit((int)str[c])) {
      switch (format) {
        case UNKNOWN:
          format = DECIMAL;
          break;
        default:
          break;
      }
    }
    else if (isxdigit((int)str[c])) {
      switch (format) {
        case UNKNOWN:
        case DECIMAL:
          format = HEXADECIMAL;
          break;
        case BYTEWISE_DECIMAL:
          format = INVALID;
          break;
        default:
          break;
      }
    }
    else if ((str[c] == 'x' || str[c] == 'X') && c > 0 && str[c-1] == '0') {
      switch (format) {
        case UNKNOWN:
        case DECIMAL:
          format = HEXADECIMAL;
          break;
        case BYTEWISE_DECIMAL:
          format = INVALID;
          break;
        default:
          break;
      }
    }
    else if (str[c] == '.') {
      ++cnt;
      switch (format) {
        case DECIMAL:
          format = (cnt < AOS_SHELL_REMOTEIDWIDTH) ? BYTEWISE_DECIMAL : INVALID;
          break;
        case UNKNOWN:
        case HEXADECIMAL:
        case BYTEWISE_HEXADECIMAL:
          format = INVALID;
          break;
        default:
          break;
      }
    }
    else if (str[c] == ':') {
      ++cnt;
      switch (format) {
        case DECIMAL:
        case HEXADECIMAL:
          format = (cnt < AOS_SHELL_REMOTEIDWIDTH) ? BYTEWISE_HEXADECIMAL : INVALID;
          break;
        case UNKNOWN:
        case BYTEWISE_DECIMAL:
          format = INVALID;
          break;
        default:
          break;
      }
    }
    else {
      format = INVALID;
    }
  }

  // interprete string
  switch (format) {
    case UNKNOWN:
    case INVALID:
    {
      id = REMOTEID_INVALID;
      break;
    }
    case DECIMAL:
    {
      id = strtoull(str, NULL, 10);
      break;
    }
    case HEXADECIMAL:
    {
      id = strtoull(str, NULL, 16);
      break;
    }
    case BYTEWISE_DECIMAL:
    {
      char* val_end;
      for (size_t byte = 0; byte <= cnt; ++byte) {
        const unsigned long long val = strtoull(str, &val_end, 10);
        if (val > 255) {
          id = REMOTEID_INVALID;
          break;
        } else {
          id += val << (((byte == cnt) ? 0 : AOS_SHELL_REMOTEIDWIDTH - byte - 1) * 8);
        }
        str = val_end+1;
      }
      break;
    }
    case BYTEWISE_HEXADECIMAL:
    {
      char* val_end;
      for (size_t byte = 0; byte <= cnt; ++byte) {
        const unsigned long long val = strtoull(str, &val_end, 16);
        if (val > 0xFF) {
          id = REMOTEID_INVALID;
          break;
        } else {
          id += val << (((byte == cnt) ? 0 : AOS_SHELL_REMOTEIDWIDTH - byte - 1) * 8);
        }
        str = val_end+1;
      }
      break;
    }
  }

  return id;
}

/**
 * @brief   Interprete a remote access control message.
 *
 * @param[in] seq   The message string to interprete.
 * @param[in] len   Length of the message string.
 *                  If 0, it is determined from the string.
 *
 * @return  The interpreted message.
 */
remote_message_t _aosShellRemoteInterpreteMessage(const char seq[], size_t len)
{
  // local variables
  remote_message_t msg = {
    /* id   */ REMOTEID_INVALID,
    /* type */ RMSG_UNKNOWN,
  };

  // optionally detect length
  if (len == 0) {
    len = strlen(seq);
  }

  // interprete control bytes
  if (len >= 1 && seq[0] == REMOTEMSG_IDENTIFIER) {
    if (len == 1) {
      msg.type = RMSG_AMBIGUOUS;
    } else {
      switch (seq[1]) {
        case REMOTEMSG_IDENTIFIER_DISCOVERREQUEST:
          msg.type = RMSG_DISCOVERREQUEST;
          break;
        case REMOTEMSG_IDENTIFIER_DISCOVERRESPONSE:
          msg.type = RMSG_DISCOVERRESPONSE;
          break;
        case REMOTEMSG_IDENTIFIER_CLIENTSREQUEST:
          msg.type = RMSG_CLIENTSREQUEST;
          break;
        case REMOTEMSG_IDENTIFIER_CLIENTRESPONSE:
          msg.type = RMSG_CLIENTRESPONSE;
          break;
        case REMOTEMSG_IDENTIFIER_CONNECT:
          msg.type = RMSG_CONNECT;
          break;
        case REMOTEMSG_IDENTIFIER_ACKNOWLEDGE:
          msg.type = RMSG_ACKNOWLEDGE;
          break;
        case REMOTEMSG_IDENTIFIER_CANCEL:
          msg.type = RMSG_CANCEL;
          break;
        case REMOTEMSG_IDENTIFIER_DISABLE:
          msg.type = RMSG_DISABLE;
          break;
        default:
          msg.type = RMSG_UNKNOWN;
          break;
      }
    }
  }

  // extract identifier
  switch (msg.type) {
    case RMSG_DISCOVERRESPONSE:
    case RMSG_CLIENTRESPONSE:
    case RMSG_CONNECT:
    case RMSG_ACKNOWLEDGE:
      if (len == 2 + AOS_SHELL_REMOTEIDWIDTH) {
        msg.id = _aosShellRemoteDeserialseId((uint8_t*)&seq[2]);
      } else {
        msg.type = RMSG_AMBIGUOUS;
      }
      break;
    case RMSG_UNKNOWN:
    case RMSG_AMBIGUOUS:
    case RMSG_DISCOVERREQUEST:
    case RMSG_CLIENTSREQUEST:
    case RMSG_CANCEL:
    case RMSG_DISABLE:
      break;
  }

  return msg;
}

/**
 * @brief   Setup a remote access control message.
 * @details The resulting string is not terminated by a trailing NUL byte.
 *
 * @param[out] buf  Buffer to write the message to.
 * @param[in]  msg  Message to setup.
 */
void _aosShellRemoteSetupMessage(char* buf, remote_message_t msg)
{
  aosDbgCheck(buf != NULL);
  aosDbgCheck(msg.type != RMSG_UNKNOWN && msg.type != RMSG_AMBIGUOUS);

  // set control bytes and identifier
  buf[0] = REMOTEMSG_IDENTIFIER;
  switch (msg.type) {
    case RMSG_DISCOVERREQUEST:
      buf[1] = REMOTEMSG_IDENTIFIER_DISCOVERREQUEST;
      break;
    case RMSG_DISCOVERRESPONSE:
      buf[1] = REMOTEMSG_IDENTIFIER_DISCOVERRESPONSE;
      break;
    case RMSG_CLIENTSREQUEST:
      buf[1] = REMOTEMSG_IDENTIFIER_CLIENTSREQUEST;
      break;
    case RMSG_CLIENTRESPONSE:
      buf[1] = REMOTEMSG_IDENTIFIER_CLIENTRESPONSE;
      break;
    case RMSG_CONNECT:
      buf[1] = REMOTEMSG_IDENTIFIER_CONNECT;
      break;
    case RMSG_ACKNOWLEDGE:
      buf[1] = REMOTEMSG_IDENTIFIER_ACKNOWLEDGE;
      break;
    case RMSG_CANCEL:
      buf[1] = REMOTEMSG_IDENTIFIER_CANCEL;
      break;
    case RMSG_DISABLE:
      buf[1] = REMOTEMSG_IDENTIFIER_DISABLE;
      break;
    case RMSG_UNKNOWN:
    case RMSG_AMBIGUOUS:
      /* must not occur; checked above */
      break;
  }

  // set identifier
  switch (msg.type) {
    case RMSG_DISCOVERRESPONSE:
    case RMSG_CLIENTRESPONSE:
    case RMSG_CONNECT:
    case RMSG_ACKNOWLEDGE:
      aosDbgAssertMsg(msg.id != REMOTEID_INVALID, "invalid remote ID");
      _aosShellRemoteSerializeId((uint8_t*)&buf[2], msg.id);
      break;
    case RMSG_DISCOVERREQUEST:
    case RMSG_CLIENTSREQUEST:
    case RMSG_CANCEL:
    case RMSG_DISABLE:
      break;
    case RMSG_UNKNOWN:
    case RMSG_AMBIGUOUS:
      /* must not occur; checked above */
      break;
  }

  return;
}

/**
 * @brief   Hash a string of variable length.
 * @details For details about the hashing algorithm see
 *          https://en.wikipedia.org/wiki/Universal_hashing#Hashing_strings
 *
 * @param[in] str   NULL terminated string to hash.
 * @param[in] h     Start value for hash calculation.
 *
 * @return  Hash of the string.
 */
aos_shellremoteid_t _aosShellRemoteHashString(const char str[], aos_shellremoteid_t h)
{
  // local variables
  const aos_shellremoteid_t a = 5;  // an 'arbitrary' constant
  const size_t len = strlen(str);

  // calculate hash
  for (size_t i = 0; i < len; ++i) {
    // update hash
    h = (h * a) + str[i % len];
    // keep i in bounds
    if (i > 2 * len) {
      i -= len;
    }
  }

  return h;
}

/**
 * @brief   Remote access control timer callback.
 *
 * @param[in] timer     Pointer to the calling virtual timer.
 * @param[in] params    Pointer to an event object object.
 */
static void _aosShellRemoteTimerCallback(virtual_timer_t* timer, void* params)
{
  (void)timer;

  // fire event
  chEvtBroadcastI((event_source_t*)params);

  return;
}

#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */

/* output *********************************************************************/

/**
 * @brief   Print the shell prompt
 * @details Depending on the configuration flags, the system uptime is printed
 *          before the prompt string.
 *
 * @param[in] shell   Pointer to the shell object.
 */
void _aosShellPrintPrompt(aos_shell_t* shell)
{
  aosDbgCheck(shell != NULL);

  // print some time informattion before prompt if configured
  if (shell->config & (SHELL_CONFIG_PROMPT_UPTIME | SHELL_CONFIG_PROMPT_DATETIME)) {
    // printf the system uptime
    if ((shell->config & (SHELL_CONFIG_PROMPT_UPTIME | SHELL_CONFIG_PROMPT_DATETIME)) == SHELL_CONFIG_PROMPT_UPTIME) {
      // get current system uptime
      aos_timestamp_t uptime;
      aosSysGetUptime(&uptime);

      chprintf((BaseSequentialStream*)&shell->stream, "[%01u:%02u:%02u:%02u:%03u:%03u] ",
               (uint32_t)(aosTimestampGet(uptime) / MICROSECONDS_PER_DAY),
               (uint8_t)(aosTimestampGet(uptime) % MICROSECONDS_PER_DAY / MICROSECONDS_PER_HOUR),
               (uint8_t)(aosTimestampGet(uptime) % MICROSECONDS_PER_HOUR / MICROSECONDS_PER_MINUTE),
               (uint8_t)(aosTimestampGet(uptime) % MICROSECONDS_PER_MINUTE / MICROSECONDS_PER_SECOND),
               (uint16_t)(aosTimestampGet(uptime) % MICROSECONDS_PER_SECOND / MICROSECONDS_PER_MILLISECOND),
               (uint16_t)(aosTimestampGet(uptime) % MICROSECONDS_PER_MILLISECOND / MICROSECONDS_PER_MICROSECOND));
    }
#if (HAL_USE_RTC == TRUE)
    else if ((shell->config & (SHELL_CONFIG_PROMPT_UPTIME | SHELL_CONFIG_PROMPT_DATETIME)) == SHELL_CONFIG_PROMPT_DATETIME) {
      // get current RTC time
      struct tm dt;
      aosSysGetDateTime(&dt);
      chprintf((BaseSequentialStream*)&shell->stream, "[%02u-%02u-%04u|%02u:%02u:%02u] ",
               dt.tm_mday,
               dt.tm_mon + 1,
               dt.tm_year + 1900,
               dt.tm_hour,
               dt.tm_min,
               dt.tm_sec);
    }
#endif /* (HAL_USE_RTC == TRUE) */
    else {
      aosDbgAssert(false);
    }
  }

  // flush output and print the actual prompt string
  chnControl(&shell->stream, CHN_CTL_TX_WAIT, NULL);
  if (shell->config & SHELL_CONFIG_PROMPT_MINIMAL) {
    chprintf((BaseSequentialStream*)&shell->stream, "%>$ ");
  } else {
    chprintf((BaseSequentialStream*)&shell->stream, "%s$ ", (shell->prompt) ? shell->prompt : BOARD_NAME);
  }

  return;
}

/**
 * @brief   Move the cursor in the terminal.
 *
 * @param[in] shell   Pointer to the shell object.
 * @param[in] line    Pointer to the current content of the line.
 * @param[in] from    Starting position of the cursor.
 * @param[in] to      Target position to move the cursor to.
 *
 * @return            The number of positions moved.
 */
int _aosShellMoveCursor(aos_shell_t* shell, const char* line, size_t from, size_t to)
{
  aosDbgCheck(shell != NULL);
  aosDbgCheck(line !=  NULL || from >= to);
  aosDbgCheck(from <= AMIROOS_CFG_SHELL_LINEWIDTH);
  aosDbgCheck(to <= AMIROOS_CFG_SHELL_LINEWIDTH);

  // local variables
  size_t pos = from;

  // move cursor left by printing backspaces
  while (pos > to) {
    streamPut(&shell->stream, ASCII_BS);
    --pos;
  }

  // move cursor right by printing line content
  while (pos < to) {
    streamPut(&shell->stream, (uint8_t)line[pos]);
    ++pos;
  }

  return (int)pos - (int)from;
}

/**
 * @brief   Print content of a given string to the shell output stream.
 *
 * @param[in] shell   Pointer to the shell object.
 * @param[in] line    Pointer to the line to be printed.
 * @param[in] from    First position to start printing from.
 * @param[in] to      Position after the last character to print.
 *
 * @return            Number of characters printed.
 */
size_t _aosShellPrintString(aos_shell_t* shell, const char* line, size_t from, size_t to)
{
  aosDbgCheck(shell != NULL);
  aosDbgCheck(line != NULL || from >= to);
  aosDbgCheck(from < AMIROOS_CFG_SHELL_LINEWIDTH);
  aosDbgCheck(to <= AMIROOS_CFG_SHELL_LINEWIDTH);

  // local variables
  size_t cnt;

  for (cnt = 0; from + cnt < to; ++cnt) {
    streamPut(&shell->stream, (uint8_t)line[from + cnt]);
  }

  return cnt;
}

/**
 * @brief   Print a single character to the input buffer and to the output
 *          stream.
 *
 * @param[in] shell   Pointer to a shell object.
 * @param[in] c       Character to print.
 *
 * @return  Number of successfully handled characters.
 *          The return value can be interpreted as boolean (1 = sucess;
 *          0 = error).
 */
int _aosShellPrintChar(aos_shell_t* shell, char c)
{
  aosDbgCheck(shell != NULL);

  // check whether input line is already full
  if (shell->operation.input.length + 1 >= AMIROOS_CFG_SHELL_LINEWIDTH) {
    return 0;
  }

  // retrieve entry in the input buffer
  char* line = _aosShellGetCurrentEntry(shell);

  // overwrite content
  if (shell->config & SHELL_CONFIG_INPUT_OVERWRITE) {
    line[shell->operation.input.cursorpos] = c;
    ++shell->operation.input.cursorpos;
    shell->operation.input.length = (shell->operation.input.cursorpos > shell->operation.input.length) ? shell->operation.input.cursorpos : shell->operation.input.length;
    streamPut(&shell->stream, (uint8_t)c);
    return 1;
  }
  // insert character
  else {
    memmove(&line[shell->operation.input.cursorpos + 1], &line[shell->operation.input.cursorpos], shell->operation.input.length - shell->operation.input.cursorpos);
    line[shell->operation.input.cursorpos] = c;
    ++shell->operation.input.length;
    _aosShellPrintString(shell, line, shell->operation.input.cursorpos, shell->operation.input.length);
    ++shell->operation.input.cursorpos;
    _aosShellMoveCursor(shell, line, shell->operation.input.length, shell->operation.input.cursorpos);
    return 1;
  }
}

/**
 * @brief   Overwrite the current output with a given line.
 * @details If the current output is longer than the string, the additional
 *          characters are cleared.
 *
 * @param[in] shell   Pointer to a shell object.
 * @param[in] line    The line to be printed.
 */
void _aosShellOverwriteOutput(aos_shell_t* shell, const char* line)
{
  aosDbgCheck(shell != NULL);
  aosDbgCheck(line != NULL && line[0] != INBUF_INIT_CHAR);

  // local variables
  const size_t oldlength = shell->operation.input.length;

  // print line (overwrite current output)
  _aosShellMoveCursor(shell, line, shell->operation.input.cursorpos, 0);
  shell->operation.input.length = strlen(line);
  _aosShellPrintString(shell, line, 0, shell->operation.input.length);

  // clear any remaining symbols
  if (oldlength > shell->operation.input.length) {
    for (shell->operation.input.cursorpos = shell->operation.input.length; shell->operation.input.cursorpos < oldlength; ++shell->operation.input.cursorpos) {
      streamPut(&shell->stream, ' ');
    }
    _aosShellMoveCursor(shell, line, oldlength, shell->operation.input.length);
  }

  shell->operation.input.cursorpos = shell->operation.input.length;

  return;
}

/**
 * @brief   Alters all intermediate NUL bytes in a string to spaces.
 *
 * @param[in] string  The string to be handled.
 * @param[in] length  Length of the string.
 *
 * @return  Detected Length of the actual content of the string.
 */
size_t _aosShellRestoreWhitespace(char* string, size_t length)
{
  aosDbgCheck(string != NULL || length == 0);

  // local variables
  size_t c = length;

  // seach for first non-NUL byte from the back
  while (c > 0) {
    --c;
    if (string[c] != ASCII_NUL) {
      // store the detected length of the content
      length = ++c;
      break;
    }
  }

  // iterate further and replace all encountered NUL bytes by spaces
  while (c > 0) {
    --c;
    if (string[c] == ASCII_NUL) {
      string[c] = ' ';
    }
  }

  return length;
}

/**
 * @brief   Performs required actions before an imminent modification (character
 *          input, deletion or autofill).
 * @details This function checks the current status and clears or copies entries
 *          in the input buffer as required. Status information (operation data)
 *          is altered accordingly as well.
 *
 * @param[in] shell   Pointer to a shell object.
 *
 * @return    Pointer to the current entry in the input buffer.
 */
char* _aosShellPrepare4Modification(aos_shell_t* shell)
{
  aosDbgCheck(shell != NULL);

  char* const line = _aosShellGetCurrentEntry(shell);

  // cleared preview
  if (shell->operation.history.selected == 0) {
    // reset the current entry if required
    if (shell->operation.history.edited != 0) {
      memset(line, ASCII_NUL, AMIROOS_CFG_SHELL_LINEWIDTH * sizeof(char));
    }
    // set the current entry as the selected one and mark it as edited
    shell->operation.history.selected = 1;
    shell->operation.history.edited = 1;
  }
  // current entry
  else if (shell->operation.history.selected == 1) {
    // mark current entry as edited
    shell->operation.history.edited = 1;
  }
  // preview of historic entry
  else if (shell->operation.history.selected > 1) {
    // copy the selected entry before modification if required
    if (shell->operation.history.selected!= shell->operation.history.edited) {
      memcpy(line, _aosShellGetSelectedEntry(shell), AMIROOS_CFG_SHELL_LINEWIDTH * sizeof(char));
    }
    // mark the selected entry as edited
    shell->operation.history.edited = shell->operation.history.selected;
  }

  return line;
}


/* main shell logic ***********************************************************/

/**
 * @brief   Select action depending on input and current status.
 * @details Only one of the arguments @p c and @p key must be valid.
 *          If @p c is not NUL, @p key must be 'KEY_UNKNOWN'.
 *          If @p key is not 'KEY_UNKNOWN', @p c must be NUL.
 *
 * @param[in] shell   Shell object to select for.
 * @param[in] c       ASCII character.
 * @param[in] key     Key press derived from escape sequence.
 *
 * @return  The selected action to execute.
 */
aos_shellaction_t _aosShellSelectAction(aos_shell_t* shell, char c, special_key_t key)
{
  aosDbgCheck(shell != NULL);
  aosDbgCheck(c == ASCII_NUL || key == KEY_UNKNOWN);

  // printable character
  if (isprint((int)c)) {
    return AOS_SHELL_ACTION_READCHAR;
  }
  // tab key or character
  else if (c == ASCII_HT || key == KEY_TAB) {
    /*
     * pressing tab once applies auto fill
     * pressing tab a second time (or more) prints suggestions
     */
    if (shell->operation.lastaction == AOS_SHELL_ACTION_AUTOCOMPLETE ||
        shell->operation.lastaction == AOS_SHELL_ACTION_SUGGEST ||
        shell->operation.input.cursorpos == 0) {
      return AOS_SHELL_ACTION_SUGGEST;
    } else {
      return AOS_SHELL_ACTION_AUTOCOMPLETE;
    }
  }
  // carriage return ('\r') or line feed ('\n') character
  else if (c == ASCII_CR || c == ASCII_LF) {
    return AOS_SHELL_ACTION_EXECUTE;
  }
  // backspace key or character
  else if (c == ASCII_BS || key == KEY_BACKSPACE) {
    // ignore if cursor is at very left
    if (shell->operation.input.cursorpos > 0) {
      return AOS_SHELL_ACTION_DELETEBACKWARD;
    }
  }
  // DEL key or character
  else if (c == ASCII_DEL || key == KEY_DELETE) {
    // ignore if cursor is at very right
    if (shell->operation.input.cursorpos < shell->operation.input.length) {
      return AOS_SHELL_ACTION_DELETEFORWARD;
    }
  }
  // 'arrow up' key
  else if (key == KEY_ARROWUP) {
    // recall previous input from history only if
    // not the oldest entry is already selected and
    // the previous entry has been set.
    if (shell->operation.history.selected < BUFFER_ENTRIES &&
        (_aosShellGetRelativeEntry(shell, shell->operation.history.selected))[0] != INBUF_INIT_CHAR) {
      return AOS_SHELL_ACTION_RECALLPREVIOUS;
    }
  }
  // 'arrow down' key
  else if (key == KEY_ARROWDOWN) {
    // clear the line if
    // no historic entry is selected but the current input has been edited or
    // the most recent entry is selected, but the current one is occupied by a moodfied version of a historic entry
    if ((shell->operation.history.selected == 1 && shell->operation.history.edited == 1) ||
        (shell->operation.history.selected == 2 && shell->operation.history.edited > 1)) {
      return AOS_SHELL_ACTION_CLEAR;
    }
    // if a historic entry is selected, recall the next input from history
    else if (shell->operation.history.selected > 1) {
      return AOS_SHELL_ACTION_RECALLNEXT;
    }
  }
  // 'arrow left' key
  else if (key == KEY_ARROWLEFT) {
    // ignore if cursor is very left
    if (shell->operation.input.cursorpos > 0) {
      return AOS_SHELL_ACTION_CURSORLEFT;
    }
  }
  // 'arrow right' key
  else if (key == KEY_ARROWRIGHT) {
    // ignore if cursor is very right
    if (shell->operation.input.cursorpos < shell->operation.input.length) {
      return AOS_SHELL_ACTION_CURSORRIGHT;
    }
  }
  // CTRL + 'arrow up' key combination or 'page up' key
  else if (key == KEY_CTRL_ARROWUP || key == KEY_PAGEUP) {
    // recall oldest input from history only if
    // not the oldest entry is already selected and
    // there is at least one history entry set
    if (shell->operation.history.selected < BUFFER_ENTRIES &&
        (_aosShellGetRelativeEntry(shell, (shell->operation.history.selected > 0) ? 1 : 0))[0] != INBUF_INIT_CHAR) {
      return AOS_SHELL_ACTION_RECALLOLDEST;
    }
  }
  // CTRL + 'arrow down' key combination or 'page down' key
  else if (key == KEY_CTRL_ARROWDOWN || key == KEY_PAGEDOWN) {
    // clear the line if
    // no historic entry is selected but the current input has been edited or
    // the most recent entry is selected, but the current one is occupied by a moodfied version of a historic entry
    if ((shell->operation.history.selected == 1 && shell->operation.history.edited == 1) ||
        (shell->operation.history.selected > 1 && shell->operation.history.edited > 1)) {
      return AOS_SHELL_ACTION_CLEAR;
    }
    // if a historic entry is selected, reset to the current input
    else if (shell->operation.history.selected > 1) {
      return AOS_SHELL_ACTION_RECALLCURRENT;
    }
  }
  // CTRL + 'arrow left' key combination
  else if (key == KEY_CTRL_ARROWLEFT) {
    // ignore if cursor is very left
    if (shell->operation.input.cursorpos > 0) {
      return AOS_SHELL_ACTION_CURSORWORDLEFT;
    }
  }
  // CTRL + 'arrow right' key combination
  else if (key == KEY_CTRL_ARROWRIGHT) {
    // ignore if cursor is very right
    if (shell->operation.input.cursorpos < shell->operation.input.length) {
      return AOS_SHELL_ACTION_CURSORWORDRIGHT;
    }
  }
  // 'end' key
  else if (key == KEY_END) {
    // ignore if cursos is very right
    if (shell->operation.input.cursorpos < shell->operation.input.length) {
      return AOS_SHELL_ACTION_CURSORTOEND;
    }
  }
  // 'home' key
  else if (key == KEY_HOME) {
    // ignore if cursor is very left
    if (shell->operation.input.cursorpos > 0) {
      return AOS_SHELL_ACTION_CURSORTOSTART;
    }
  }
  // CTRL + C key combination
  else if (c == ASCII_ETX || key == KEY_CTRL_C) {
    return AOS_SHELL_ACTION_RESET;
  }
  // INS key
  else if (key == KEY_INSERT) {
    return AOS_SHELL_ACTION_INSERTTOGGLE;
  }

  // default
  return AOS_SHELL_ACTION_UNSPECIFIED;
}

/**
 * @brief   Shell thread main function.
 */
THD_FUNCTION(_aosShellThreadFunction, shellp)
{
  aosDbgCheck(shellp != NULL);

  // local variables
  aos_shell_t* const shell = shellp;

  // initialize variables and buffers
  chRegSetThreadName(shell->name);
  memset(_aosShellGetCurrentEntry(shell), ASCII_NUL, AMIROOS_CFG_SHELL_LINEWIDTH * sizeof(char));
  shell->operation.channel = NULL;
  shell->operation.input.length = 0;
  shell->operation.input.cursorpos = 0;
  memset(shell->operation.input.seqbuf, ASCII_NUL, AOS_SHELL_SEQUENCEBUFFER_SIZE * sizeof(char));
  shell->operation.input.seqlen = 0;
  for (size_t arg = 0; arg < AMIROOS_CFG_SHELL_MAXARGS; ++arg) {
    shell->operation.input.argbuf[arg] = NULL;
  }
  shell->operation.history.current = 0;
  shell->operation.history.selected = (AMIROOS_CFG_SHELL_HISTLENGTH > 0) ? 1 : 0;
  shell->operation.history.edited = 0;
  shell->operation.lastaction = AOS_SHELL_ACTION_UNSPECIFIED;
#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true)
  shell->operation.remote.requested = REMOTEID_INVALID;
  shell->operation.remote.channel = NULL;
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */
  shell->operation.execstatus.command = NULL;
  shell->operation.execstatus.retval = 0;
  shell->operation.flags = 0;

  // register OS related events
  chEvtRegisterMask(&aos.events.os, &shell->events.osListener, AOS_SHELL_EVENT_OS);
#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true)
  // register remote timeout timer event
  chEvtRegisterMask(&shell->remote.evtSource, &shell->remote.evtListener, AOS_SHELL_EVENT_REMOTETIMER);
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */
  // register events to all channels input
  for (AosShellChannel* channel = shell->stream.channel; channel != NULL; channel = channel->next) {
    chEvtRegisterMaskWithFlags(&(channel->asyncchannel->event), &(channel->listener), AOS_SHELL_EVENT_INPUT, CHN_INPUT_AVAILABLE);
  }

  // print initial message
  chprintf((BaseSequentialStream*)&shell->stream, "Welcome to AMiRo-OS shell!\n");
  chprintf((BaseSequentialStream*)&shell->stream, "Hit TAB to get started.\n");
  streamPut(&shell->stream, '\n');

  // print the prompt for the first time
  _aosShellPrintPrompt(shell);

  // fire start event
  shell->state = AOS_SHELL_STATE_RUNNING;
  chEvtBroadcastFlags(&shell->events.source, AOS_SHELL_EVTFLAG_START);

  // enter thread loop
  while (!chThdShouldTerminateX()) {
    // wait for event and handle it accordingly
    const eventmask_t eventmask = chEvtWaitOne(ALL_EVENTS);

    // handle event
    switch (eventmask) {

      // OS related events
      case AOS_SHELL_EVENT_OS:
      {
        const eventflags_t eventflags = chEvtGetAndClearFlags(&shell->events.osListener);
        // handle shutdown/restart events
        if (eventflags & AOS_SYSTEM_EVENTFLAGS_SHUTDOWN_MASK) {
          shell->state = AOS_SHELL_STATE_STOPPING;
          chThdTerminate(chThdGetSelfX());
        } else {
          // print an error message
          chprintf((BaseSequentialStream*)&shell->stream, "\nERROR: unknown OS event received (0x%08X)\n", eventflags);
        }
        break;
      }

#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true)
      case AOS_SHELL_EVENT_REMOTETIMER:
      {
        switch (shell->operation.flags & SHELL_OPFLAGS_REMOTEACTION_MASK) {
          case SHELL_OPFLAGS_REMOTEACTION_DISCOVER:
          case SHELL_OPFLAGS_REMOTEACTION_LISTCLIENTS:
            break;
          case SHELL_OPFLAGS_REMOTEACTION_CONNECT:
          case SHELL_OPFLAGS_REMOTEACTION_DISCONNECT:
            chprintf((BaseSequentialStream*)&shell->stream, "\tfail\n");
            shell->operation.remote.requested = REMOTEID_INVALID;
            break;
          default:
            aosDbgAssertMsg(false, "unexpected timeout event");
            break;
        }
        // update state and broadcast event
        shell->operation.flags &= ~(SHELL_OPFLAGS_EXECUTING | SHELL_OPFLAGS_REMOTEACTION_MASK);
        chEvtBroadcastFlags(&shell->events.source, AOS_SHELL_EVTFLAG_DONE);
        // print prompt
        _aosShellPrintPrompt(shell);
        break;
      }
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */

      // input events
      case AOS_SHELL_EVENT_INPUT:
      {
        // reset the channel only if the shell does not listen to a certain channel
        if (shell->operation.channel == NULL) {
          shell->operation.channel = shell->stream.channel;
        }

        while (shell->operation.channel != NULL) {
          const eventflags_t eventflags = chEvtGetAndClearFlags(&shell->operation.channel->listener);

          // if there is new input
          if (eventflags & CHN_INPUT_AVAILABLE) {
            // read one character at a time from the channel
            char c;
            while (chnReadTimeout(shell->operation.channel, (uint8_t*)&c, 1, TIME_IMMEDIATE)) {
              // temporary variables
              special_key_t key = KEY_UNKNOWN;
#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true)
              remote_message_t rmsg = {REMOTEID_INVALID, RMSG_UNKNOWN};
              aos_shellaction_t action = (shell->operation.remote.channel != NULL) ?
                    AOS_SHELL_ACTION_REMOTE_PASSTHROUGH :
                    (shell->operation.channel->flags & AOS_SHELLCHANNEL_REMOTE &&
                     !(shell->operation.channel->flags & SHELLCHANNEL_REMOTE_ESTABLISHED)) ?
                      AOS_SHELL_ACTION_NONE :
                      AOS_SHELL_ACTION_UNSPECIFIED;
              eventflags_t emitflags = 0;
#else /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */
              aos_shellaction_t action = AOS_SHELL_ACTION_UNSPECIFIED;
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */

#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true)
              if (shell->operation.channel->flags & AOS_SHELLCHANNEL_REMOTE) {
                // set default action
                if (shell->operation.channel->flags & SHELLCHANNEL_REMOTE_ESTABLISHED) {
                  action = (shell->operation.channel == shell->operation.remote.channel) ? AOS_SHELL_ACTION_REMOTE_PASSTHROUGH : AOS_SHELL_ACTION_UNSPECIFIED;
                } else {
                  action = AOS_SHELL_ACTION_NONE;
                }

                // detect remote access control messages
                if (c == REMOTEMSG_IDENTIFIER && shell->operation.input.seqlen == 0) {
                  // start interpreting a remote message sequence
                  shell->operation.flags |= SHELL_OPFLAGS_RMSGSEQUENCE;
                  // suspend input from all other channels
                  _aosShellStreamSuspendAllChannelsBut(&shell->stream, shell->operation.channel);
                }
                // if a remote message is being interpreted
                if (shell->operation.flags & SHELL_OPFLAGS_RMSGSEQUENCE) {
                  // append and 'consume' character
                  shell->operation.input.seqbuf[shell->operation.input.seqlen] = c;
                  c = ASCII_NUL;
                  ++shell->operation.input.seqlen;
                  // try to interprete sequence
                  rmsg = _aosShellRemoteInterpreteMessage(shell->operation.input.seqbuf, shell->operation.input.seqlen);
                  switch (rmsg.type) {
                    case RMSG_UNKNOWN:
                      action = AOS_SHELL_ACTION_PRINTUNKNOWNSEQUENCE;
                      break;
                    case RMSG_AMBIGUOUS:
                      // read next byte to resolve ambiguity
                      continue;
                    case RMSG_DISCOVERREQUEST:
                      // respond
                      action = AOS_SHELL_ACTION_REMOTE_DISCOVERRESPOND;
                      break;
                    case RMSG_DISCOVERRESPONSE:
                      // if a duplicate identifier is detected
                      if (rmsg.id == shell->remote.id) {
                        action = AOS_SHELL_ACTION_REMOTE_DISABLE_ACTIVE;
                      }
                      // if this shell is waiting for discover responses
                      else if ((shell->operation.flags & SHELL_OPFLAGS_REMOTEACTION_MASK) == SHELL_OPFLAGS_REMOTEACTION_DISCOVER) {
                        action = AOS_SHELL_ACTION_REMOTE_LIST;
                      }
                      break;
                    case RMSG_CLIENTSREQUEST:
                      // if this shell is client on the channel
                      if (shell->operation.remote.channel == shell->operation.channel) {
                        action = AOS_SHELL_ACTION_REMOTE_CLIENTRESPOND;
                      }
                      break;
                    case RMSG_CLIENTRESPONSE:
                      // if a duplicate identifier is detected
                      if (rmsg.id == shell->remote.id) {
                        action = AOS_SHELL_ACTION_REMOTE_DISABLE_ACTIVE;
                      }
                      // if this shell is listing all clients
                      else if ((shell->operation.flags & SHELL_OPFLAGS_REMOTEACTION_MASK) == SHELL_OPFLAGS_REMOTEACTION_LISTCLIENTS) {
                        action = AOS_SHELL_ACTION_REMOTE_LIST;
                      }
                      // if this shell is looking for this client to disconnect
                      else if ((shell->operation.flags & SHELL_OPFLAGS_REMOTEACTION_MASK) == SHELL_OPFLAGS_REMOTEACTION_DISCONNECT &&
                               rmsg.id == shell->operation.remote.requested) {
                        action = AOS_SHELL_ACTION_REMOTE_CANCEL_ACTIVE;
                      }
                      break;
                    case RMSG_CONNECT:
                      // if this shell did not expect such a message
                      if ((shell->operation.flags & SHELL_OPFLAGS_REMOTEACTION_MASK) == SHELL_OPFLAGS_REMOTEACTION_CONNECT) {
                        action = AOS_SHELL_ACTION_REMOTE_CANCEL_ACTIVE;
                      }
                      // if this shell was targeted
                      else if (rmsg.id == shell->remote.id) {
                        action = AOS_SHELL_ACTION_REMOTE_ACKNOWLEDGE;
                      }
                      // if this shell was server
                      else if (shell->operation.channel->flags & SHELLCHANNEL_REMOTE_ESTABLISHED &&
                          shell->operation.channel != shell->operation.remote.channel) {
                        action = AOS_SHELL_ACTION_REMOTE_CLOSE;
                      }
                      break;
                    case RMSG_ACKNOWLEDGE:
                      // if a duplicate identifier is detected
                      if (rmsg.id == shell->remote.id) {
                        action = AOS_SHELL_ACTION_REMOTE_DISABLE_ACTIVE;
                      }
                      // if this shell requested the connection
                      else if ((shell->operation.flags & SHELL_OPFLAGS_REMOTEACTION_MASK) == SHELL_OPFLAGS_REMOTEACTION_CONNECT) {
                        // if the identifier matches the requested one
                        if (rmsg.id == shell->operation.remote.requested) {
                          // if this shell was server on the channel
                          if ((shell->operation.channel->flags & SHELLCHANNEL_REMOTE_ESTABLISHED) &&
                              shell->operation.remote.channel != shell->operation.channel) {
                            action = AOS_SHELL_ACTION_REMOTE_CLOSE;
                          } else {
                            action = AOS_SHELL_ACTION_REMOTE_ESTABLISH;
                          }
                        } else {
                          action = AOS_SHELL_ACTION_REMOTE_CANCEL_ACTIVE;
                        }
                      }
                      break;
                    case RMSG_CANCEL:
                      action = AOS_SHELL_ACTION_REMOTE_CANCEL_PASSIVE;
                      break;
                    case RMSG_DISABLE:
                      action = AOS_SHELL_ACTION_REMOTE_DISABLE_PASSIVE;
                      break;
                  }
                  /* sequence is never ambiguous at this point */
                  // release all suspended channels and move on
                  _aosShellStreamResumeSuspendedChannels(&shell->stream);
                  shell->operation.flags &= ~SHELL_OPFLAGS_RMSGSEQUENCE;
                  // reset the sequence buffer
                  if (action != AOS_SHELL_ACTION_PRINTUNKNOWNSEQUENCE) {
                    memset(shell->operation.input.seqbuf, ASCII_NUL, AOS_SHELL_SEQUENCEBUFFER_SIZE * sizeof(char));
                    shell->operation.input.seqlen = 0;
                  }
                }
              }
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */

              // detect, interprete and act wrt. escape sequences
              if (action == AOS_SHELL_ACTION_UNSPECIFIED) {
                // detect escape sequence
                if (c == ASCII_ESC && shell->operation.input.seqlen == 0) {
                  // start interpreting a remote message sequence
                  shell->operation.flags |= SHELL_OPFLAGS_ESCSEQUENCE;
                  // suspend input from all other channels
                  _aosShellStreamSuspendAllChannelsBut(&shell->stream, shell->operation.channel);
                }
                // if an escape sequence is being interpreted
                if (shell->operation.flags & SHELL_OPFLAGS_ESCSEQUENCE) {
                  // append and 'consume' character
                  shell->operation.input.seqbuf[shell->operation.input.seqlen] = c;
                  c = ASCII_NUL;
                  ++shell->operation.input.seqlen;
                  // try to interprete sequence
                  key = _aosShellInterpreteEscapeSequence(shell->operation.input.seqbuf, shell->operation.input.seqlen);
                  switch (key) {
                    case KEY_UNKNOWN:
                      action = AOS_SHELL_ACTION_PRINTUNKNOWNSEQUENCE;
                      break;
                    case KEY_AMBIGUOUS:
                      // read next byte to resolve ambiguity
                      continue;
                    default:
                      break;
                  }
                  /* sequence is never ambiguous at this point */
                  // release all suspended channels and move on
                  _aosShellStreamResumeSuspendedChannels(&shell->stream);
                  shell->operation.flags &= ~SHELL_OPFLAGS_ESCSEQUENCE;
                  // reset the sequence buffer
                  if (action != AOS_SHELL_ACTION_PRINTUNKNOWNSEQUENCE) {
                    memset(shell->operation.input.seqbuf, ASCII_NUL, AOS_SHELL_SEQUENCEBUFFER_SIZE * sizeof(char));
                    shell->operation.input.seqlen = 0;
                  }
                }
              }

              // if no action has been selected yet
              if (action == AOS_SHELL_ACTION_UNSPECIFIED) {
                // select action by input and current status
                action = _aosShellSelectAction(shell, c, key);
                if (action == AOS_SHELL_ACTION_UNSPECIFIED) {
                  action = AOS_SHELL_ACTION_NONE;
                }
              }

              // execute action
              switch (action) {
                case AOS_SHELL_ACTION_UNSPECIFIED:
                {
                  // this must never happen
                  aosDbgAssertMsg(false, "invalid action");
                  break;
                }

                case AOS_SHELL_ACTION_NONE:
                {
                  // do nothing (ignore input) and read next byte
                  break;
                }

                case AOS_SHELL_ACTION_READCHAR:
                {
                  char* line = _aosShellPrepare4Modification(shell);
                  if (_aosShellPrintChar(shell, c) == 0) {
                    // line is full
                    _aosShellMoveCursor(shell, line, shell->operation.input.cursorpos, shell->operation.input.length);
                    chprintf((BaseSequentialStream*)&shell->stream, "\n\tmaximum line width reached\n");
                    _aosShellPrintPrompt(shell);
                    _aosShellPrintString(shell, line, 0, shell->operation.input.length);
                    _aosShellMoveCursor(shell, line, shell->operation.input.length, shell->operation.input.cursorpos);
                  }
                  break;
                }

                case AOS_SHELL_ACTION_AUTOCOMPLETE:
                {
                  // local variables
                  char* line = _aosShellGetVisualisedEntry(shell);
                  const char* fill = line;
                  size_t cmatch = shell->operation.input.cursorpos;
                  charmatch_t matchlevel = CHAR_MATCH_NOT;
                  size_t n;

                  // only execute autofill if the line is valid
                  if (line) {
                    // iterate through command list
                    chMtxLock(&_commands_mtx);
                    for (aos_shellcommand_t* cmd = _commands; cmd != NULL; cmd = cmd->next) {
                      // compare current match with command
                      n = cmatch;
                      charmatch_t mlvl = CHAR_MATCH_NOT;
                      _aosShellStrccmp(fill, cmd->name, shell->config & SHELL_CONFIG_MATCHCASE, (n == 0) ? NULL : &n, &mlvl);
                      const int cmp = (n < cmatch) ?
                            ((int)n - (int)cmatch) :
                            (cmd->name[n] != ASCII_NUL) ?
                              (int)strlen(cmd->name) - (int)n :
                              0;
                      // if an exact match was found
                      if ((size_t)((int)cmatch + cmp) == shell->operation.input.cursorpos) {
                        cmatch = shell->operation.input.cursorpos;
                        fill = cmd->name;
                        // break the loop only if there are no case mismatches with the input
                        n = shell->operation.input.cursorpos;
                        _aosShellStrccmp(fill, line, false, &n, &mlvl);
                        if (mlvl == CHAR_MATCH_CASE) {
                          break;
                        }
                      }
                      // if a not exact match was found
                      else if ((size_t)((int)cmatch + cmp) > shell->operation.input.cursorpos) {
                        // if this is the first one
                        if (fill == line) {
                          cmatch = (size_t)((int)cmatch + cmp);
                          fill = cmd->name;
                        }
                        // if this is a worse one
                        else if ((cmp < 0) || (cmp == 0 && mlvl == CHAR_MATCH_CASE)) {
                          cmatch = (size_t)((int)cmatch + cmp);
                        }
                      }
                      // non matching commands are ignored
                      else {}
                    }
                    chMtxUnlock(&_commands_mtx);

                    // evaluate if there are case mismatches
                    n = cmatch;
                    _aosShellStrccmp(line, fill, shell->config & SHELL_CONFIG_MATCHCASE, &n, &matchlevel);
                    // print the auto fill if any
                    if ((cmatch > shell->operation.input.cursorpos) ||
                        (cmatch == shell->operation.input.cursorpos && matchlevel == CHAR_MATCH_NCASE && strlen(fill) == shell->operation.input.cursorpos)) {
                      line = _aosShellPrepare4Modification(shell);
                      // move the cursor to the right as long as already available input matches
                      while (shell->operation.input.cursorpos < shell->operation.input.length) {
                        const charmatch_t cm = _aosShellCharcmp(fill[shell->operation.input.cursorpos], line[shell->operation.input.cursorpos]);
                        if (cm == CHAR_MATCH_CASE || (cm == CHAR_MATCH_NCASE && shell->config & SHELL_CONFIG_MATCHCASE)) {
                          shell->operation.input.cursorpos += _aosShellMoveCursor(shell, line, shell->operation.input.cursorpos, shell->operation.input.cursorpos+1);
                        } else {
                          break;
                        }
                      }
                      // limit auto fill so it will not overflow the line width
                      if (shell->operation.input.length + (cmatch - shell->operation.input.cursorpos) > AMIROOS_CFG_SHELL_LINEWIDTH) {
                        cmatch = AMIROOS_CFG_SHELL_LINEWIDTH - shell->operation.input.length + shell->operation.input.cursorpos;
                      }
                      // move trailing memory further in the line
                      memmove(&line[cmatch], &line[shell->operation.input.cursorpos], (shell->operation.input.length - shell->operation.input.cursorpos) * sizeof(char));
                      shell->operation.input.length += cmatch - shell->operation.input.cursorpos;
                      // if there was no incorrect case when matching
                      if (matchlevel == CHAR_MATCH_CASE) {
                        // insert fill command name to line
                        memcpy(&line[shell->operation.input.cursorpos], &fill[shell->operation.input.cursorpos], (cmatch - shell->operation.input.cursorpos) * sizeof(char));
                        // print the output
                        _aosShellPrintString(shell, line, shell->operation.input.cursorpos, shell->operation.input.length);
                      } else {
                        // overwrite line with fill command name
                        memcpy(line, fill, cmatch * sizeof(char));
                        // reprint the whole line
                        _aosShellMoveCursor(shell, line, shell->operation.input.cursorpos, 0);
                        _aosShellPrintString(shell, line, 0, shell->operation.input.length);
                      }
                      // move cursor to the end of the matching sequence
                      shell->operation.input.cursorpos = cmatch;
                      _aosShellMoveCursor(shell, line, shell->operation.input.length, shell->operation.input.cursorpos);
                    }
                  }
                  break;
                }

                case AOS_SHELL_ACTION_SUGGEST:
                {
                  // local variables
                  const char* const line = _aosShellGetVisualisedEntry(shell);
                  unsigned int matches = 0;

                  // iterate through command list
                  chMtxLock(&_commands_mtx);
                  for (aos_shellcommand_t* cmd = _commands; cmd != NULL; cmd = cmd->next) {
                    // compare line content with command, except if cursorpos is 0
                    size_t i = shell->operation.input.cursorpos;
                    if (shell->operation.input.cursorpos > 0) {
                      _aosShellStrccmp(line, cmd->name, shell->config & SHELL_CONFIG_MATCHCASE, &i, NULL);
                    }
                    const int cmp = (i < shell->operation.input.cursorpos) ?
                          ((int)i - (int)shell->operation.input.cursorpos) :
                          (cmd->name[i] != ASCII_NUL) ?
                            (int)strlen(cmd->name) - (int)i :
                            0;
                    // if a match was found
                    if (cmp > 0) {
                      // if this is the first one
                      if (matches == 0) {
                        _aosShellMoveCursor(shell, line, shell->operation.input.cursorpos, shell->operation.input.length);
                        streamPut(&shell->stream, ASCII_LF);
                      }
                      // print the command
                      chprintf((BaseSequentialStream*)&shell->stream, "\t%s\n", cmd->name);
                      ++matches;
                    }
                  }
                  chMtxUnlock(&_commands_mtx);

                  // reprint the prompt and line if any matches have been found
                  if (matches > 0) {
                    _aosShellPrintPrompt(shell);
                    _aosShellPrintString(shell, line, 0, shell->operation.input.length);
                    _aosShellMoveCursor(shell, line, shell->operation.input.length, shell->operation.input.cursorpos);
                  }
                  break;
                }

                case AOS_SHELL_ACTION_EXECUTE:
                {
                  streamPut(&shell->stream, ASCII_LF);
                  const aos_shellcommand_t* cmd = NULL;
                  // get the current entry
                  char* const line = _aosShellGetVisualisedEntry(shell);
                  // skip, if there is nothing to be executed
                  if (line) {
                    // parse arguments
#if (AMIROOS_CFG_SHELL_HISTLENGTH > 0)
                    _aosShellPrepare4Modification(shell);
#endif /* (AMIROOS_CFG_SHELL_HISTLENGTH > 0) */
                    const size_t nargs = _aosShellParseArguments((aos_shell_t*)shell, line);
                    // check number of arguments found
                    if (nargs > AMIROOS_CFG_SHELL_MAXARGS) {
                      chprintf((BaseSequentialStream*)&shell->stream, "\ttoo many arguments\n");
                    } else if (nargs > 0) {
                      // search command list for arg[0] and execute callback
                      chMtxLock(&_commands_mtx);
                      cmd = _commands;
                      while (cmd != NULL) {
                        // if the requested command has been found
                        if (strcmp(shell->operation.input.argbuf[0], cmd->name) == 0) {
                          // break the loop
                          break;
                        } else {
                          // keep searching
                          cmd = cmd->next;
                        }
                      } /* end of while */
                      chMtxUnlock(&_commands_mtx);

                      // if a matching commands was found
                      if (cmd) {
                        // execute command
                        shell->operation.execstatus.command = cmd;
                        shell->operation.flags |= SHELL_OPFLAGS_EXECUTING;
                        chEvtBroadcastFlags(&shell->events.source, AOS_SHELL_EVTFLAG_EXECUTE);
                        shell->operation.execstatus.retval = cmd->callback((BaseSequentialStream*)&shell->stream, (int)nargs, (const char**)shell->operation.input.argbuf);
                      }
                      // if no matching command was found, print a message
                      else {
                        chprintf((BaseSequentialStream*)&shell->stream, "\tcommand '%s' not found\n", shell->operation.input.argbuf[0]);
                      }
                    }

                    // restore spaces in the current entry, which have been modified to NUL bytes due to argument parsing
                    _aosShellRestoreWhitespace(line, shell->operation.input.length);

                    // update internal variables
                    shell->operation.input.length = 0;
                    shell->operation.input.cursorpos = 0;
                    shell->operation.history.edited = 0;
#if (AMIROOS_CFG_SHELL_HISTLENGTH > 0)
                    // iterate in the history only if
                    // there was some valid input and
                    // the user did not execute the exact previous command again
                    if (nargs > 0 && strcmp(line, _aosShellGetRelativeEntry((aos_shell_t*)shell, 1)) != 0) {
                      shell->operation.history.current = (shell->operation.history.current + 1) % BUFFER_ENTRIES;
                    }
                    // clear and select next (now current) entry
                    memset(_aosShellGetCurrentEntry(shell), ASCII_NUL, AMIROOS_CFG_SHELL_LINEWIDTH * sizeof(char));
                    shell->operation.history.selected = 1;
#else /* (AMIROOS_CFG_SHELL_HISTLENGTH > 0) */
                    // do not clear/reset the entry, but start off with a cleared preview
                    shell->operation.history.selected = 0;
#endif /* (AMIROOS_CFG_SHELL_HISTLENGTH > 0) */

#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true)
                    // check for ongoing remote access control commands
                    if (shell->operation.flags & SHELL_OPFLAGS_REMOTEACTION_MASK) {
                      break;
                    }
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */

                    // if a command was actually executed
                    if (cmd) {
                      // update state and broadcast event
                      shell->operation.flags &= ~SHELL_OPFLAGS_EXECUTING;
                      chEvtBroadcastFlags(&shell->events.source, AOS_SHELL_EVTFLAG_DONE);

                      // notify user if execution of the command was not successful
                      if (shell->operation.execstatus.retval != 0) {
                        chprintf((BaseSequentialStream*)&shell->stream, "command returned exit status %d\n", shell->operation.execstatus.retval);
                      }
                    }
                  }

                  // print a new prompt
                  if (!chThdShouldTerminateX()) {
                    _aosShellPrintPrompt((aos_shell_t*)shell);
                  } else {
                    shell->state = AOS_SHELL_STATE_STOPPING;
                  }
                  break;
                }

                case AOS_SHELL_ACTION_DELETEBACKWARD:
                {
                  char* line = _aosShellPrepare4Modification(shell);
                  --shell->operation.input.cursorpos;
                  memmove(&line[shell->operation.input.cursorpos], &line[shell->operation.input.cursorpos + 1], (shell->operation.input.length - shell->operation.input.cursorpos) * sizeof(char));
                  --shell->operation.input.length;
                  line[shell->operation.input.length] = ASCII_NUL;
                  _aosShellMoveCursor(shell, line, shell->operation.input.cursorpos + 1, shell->operation.input.cursorpos);
                  _aosShellPrintString(shell, line, shell->operation.input.cursorpos, shell->operation.input.length);
                  streamPut(&shell->stream, ' ');
                  _aosShellMoveCursor(shell, line, shell->operation.input.length + 1, shell->operation.input.cursorpos);
                  break;
                }

                case AOS_SHELL_ACTION_DELETEFORWARD:
                {
                  char* line = _aosShellPrepare4Modification(shell);
                  --shell->operation.input.length;
                  memmove(&line[shell->operation.input.cursorpos], &line[shell->operation.input.cursorpos + 1], (shell->operation.input.length - shell->operation.input.cursorpos) * sizeof(char));
                  _aosShellPrintString(shell, line, shell->operation.input.cursorpos, shell->operation.input.length);
                  streamPut(&shell->stream, ' ');
                  _aosShellMoveCursor(shell, line, shell->operation.input.length + 1, shell->operation.input.cursorpos);
                  break;
                }

                case AOS_SHELL_ACTION_CLEAR:
                {
                  // clear visualization
                  _aosShellMoveCursor(shell, NULL, shell->operation.input.cursorpos, 0);
                  for (size_t cpos = 0; cpos < shell->operation.input.length; ++cpos) {
                    streamPut(&shell->stream, ' ');
                  }
                  _aosShellMoveCursor(shell, NULL, shell->operation.input.length, 0);

                  // update metadata
                  shell->operation.input.cursorpos = 0;
                  shell->operation.input.length = 0;
                  shell->operation.history.selected = 0;

                  break;
                }

                case AOS_SHELL_ACTION_RECALLPREVIOUS:
                {
                  // if the input was cleared but the current entry is occupied by a modified copy of a history entry, skip the current entry
                  if (BUFFER_ENTRIES > 1 && shell->operation.history.selected == 0 && shell->operation.history.edited > 1) {
                    shell->operation.history.selected = 2;
                  } else {
                    ++shell->operation.history.selected;
                  }
                  // only print if the selected entry is valid
                  const char* const line = _aosShellGetVisualisedEntry(shell);
                  if (line) {
                    _aosShellOverwriteOutput(shell, line);
                  }
                  break;
                }

                case AOS_SHELL_ACTION_RECALLNEXT:
                {
                  --shell->operation.history.selected;
                  _aosShellOverwriteOutput(shell, _aosShellGetVisualisedEntry(shell));
                  break;
                }

                case AOS_SHELL_ACTION_RECALLOLDEST:
                {
                  // find oldest valid entry
                  shell->operation.history.selected = BUFFER_ENTRIES;
                  while (_aosShellGetSelectedEntry(shell)[0] == INBUF_INIT_CHAR) {
                    --shell->operation.history.selected;
                  }
                  // only print if the selected entry is valid
                  const char* const line = _aosShellGetVisualisedEntry(shell);
                  if (line) {
                    _aosShellOverwriteOutput(shell, line);
                  }
                  break;
                }

                case AOS_SHELL_ACTION_RECALLCURRENT:
                {
                  shell->operation.history.selected = 1;
                  _aosShellOverwriteOutput(shell, _aosShellGetVisualisedEntry(shell));
                  break;
                }

                case AOS_SHELL_ACTION_CURSORLEFT:
                {
                  _aosShellMoveCursor(shell, NULL, shell->operation.input.cursorpos, shell->operation.input.cursorpos - 1);
                  --shell->operation.input.cursorpos;
                  break;
                }

                case AOS_SHELL_ACTION_CURSORRIGHT:
                {
                  _aosShellMoveCursor(shell, _aosShellGetVisualisedEntry(shell), shell->operation.input.cursorpos, shell->operation.input.cursorpos + 1);
                  ++shell->operation.input.cursorpos;
                  break;
                }

                case AOS_SHELL_ACTION_CURSORWORDLEFT:
                {
                  // local variables
                  const char* line = _aosShellGetVisualisedEntry(shell);
                  size_t cpos = shell->operation.input.cursorpos;

                  // skip spaces
                  while (cpos > 0) {
                    --cpos;
                    if (line[cpos] != ' ') {
                      break;
                    }
                  }

                  // search for beginning of the word
                  while (cpos > 0) {
                    --cpos;
                    if (line[cpos] == ' ') {
                      // go back to first character of the word
                      ++cpos;
                      break;
                    }
                  }

                  // move the cursor and set metadata
                  _aosShellMoveCursor(shell, line, shell->operation.input.cursorpos, cpos);
                  shell->operation.input.cursorpos = cpos;

                  break;
                }

                case AOS_SHELL_ACTION_CURSORWORDRIGHT:
                {
                  // local variables
                  const char* line = _aosShellGetVisualisedEntry(shell);
                  size_t cpos = shell->operation.input.cursorpos;

                  // skip spaces
                  while (cpos < shell->operation.input.length && line[cpos] == ' ') {
                    ++cpos;
                  }

                  // search for end of the word
                  while (cpos < shell->operation.input.length && line[cpos] != ' ') {
                    ++cpos;
                  }

                  // move cursor and set metadata
                  _aosShellMoveCursor(shell, line, shell->operation.input.cursorpos, cpos);
                  shell->operation.input.cursorpos = cpos;

                  break;
                }

                case AOS_SHELL_ACTION_CURSORTOEND:
                {
                  _aosShellMoveCursor(shell, _aosShellGetVisualisedEntry(shell), shell->operation.input.cursorpos, shell->operation.input.length);
                  shell->operation.input.cursorpos = shell->operation.input.length;
                  break;
                }

                case AOS_SHELL_ACTION_CURSORTOSTART:
                {
                  _aosShellMoveCursor(shell, _aosShellGetVisualisedEntry(shell), shell->operation.input.cursorpos, 0);
                  shell->operation.input.cursorpos = 0;
                  break;
                }

                case AOS_SHELL_ACTION_RESET:
                {
                  // print reset indicator
                  chprintf((BaseSequentialStream*)&shell->stream, "^C");
                  if (!(shell->config & SHELL_CONFIG_INPUT_OVERWRITE)) {
                    _aosShellPrintString(shell, _aosShellGetVisualisedEntry(shell), shell->operation.input.cursorpos, shell->operation.input.length);
                  }
                  chprintf((BaseSequentialStream*)&shell->stream, "\n");

                  // reset buffers and metadata
                  if (shell->operation.history.edited != 0) {
                    memset(_aosShellGetCurrentEntry(shell), ASCII_NUL, AMIROOS_CFG_SHELL_LINEWIDTH * sizeof(char));
                  }
                  shell->operation.input.length = 0;
                  shell->operation.input.cursorpos = 0;
#if (AMIROOS_CFG_SHELL_HISTLENGTH > 0)
                  shell->operation.history.selected = 1;
#else /* (AMIROOS_CFG_SHELL_HISTLENGTH > 0) */
                  shell->operation.history.selected = 0;
#endif /* (AMIROOS_CFG_SHELL_HISTLENGTH > 0) */
                  shell->operation.history.edited = 0;

                  // print a new prompt
                  _aosShellPrintPrompt(shell);

                  break;
                }

                case AOS_SHELL_ACTION_INSERTTOGGLE:
                {
                  if (shell->config & SHELL_CONFIG_INPUT_OVERWRITE) {
                    shell->config &= ~SHELL_CONFIG_INPUT_OVERWRITE;
                  } else {
                    shell->config |= SHELL_CONFIG_INPUT_OVERWRITE;
                  }
                  break;
                }

                case AOS_SHELL_ACTION_PRINTUNKNOWNSEQUENCE:
                {
                  const char* line = _aosShellGetVisualisedEntry(shell);
                  // insert a new line
                  _aosShellMoveCursor(shell, line, shell->operation.input.cursorpos, shell->operation.input.length);
                  streamPut(&shell->stream, ASCII_LF);
                  // print message
                  streamPut(&shell->stream, '?');
                  for (size_t seqc = 0; seqc < shell->operation.input.seqlen; ++seqc) {
                    chprintf((BaseSequentialStream*)&shell->stream, " 0x%02X", shell->operation.input.seqbuf[seqc]);
                  }
                  streamPut(&shell->stream, ASCII_LF);
                  // reset sequence buffer
                  memset(shell->operation.input.seqbuf, ASCII_NUL, AOS_SHELL_SEQUENCEBUFFER_SIZE * sizeof(char));
                  shell->operation.input.seqlen = 0;
                  // reprint prompt and line
                  _aosShellPrintPrompt(shell);
                  _aosShellPrintString(shell, line, 0, shell->operation.input.length);
                  _aosShellMoveCursor(shell, line, shell->operation.input.length, shell->operation.input.cursorpos);
                  break;
                }

#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true)

                case AOS_SHELL_ACTION_REMOTE_PASSTHROUGH:
                {
                  // if the current input is from the server
                  if (shell->operation.channel == shell->operation.remote.channel) {
                    // pass character to all channels but this one
                    shell->operation.remote.channel->flags |= SHELLCHANNEL_OUTPUT_DISCARD;
                    streamPut(&shell->stream, c);
                    shell->operation.remote.channel->flags &= ~SHELLCHANNEL_OUTPUT_DISCARD;
                  } else {
                    // pass character only to the remote server
                    streamPut(shell->operation.remote.channel, c);
                  }
                  break;
                }

                case AOS_SHELL_ACTION_REMOTE_DISCOVERRESPOND:
                {
                  // setup message
                  char str[2 + AOS_SHELL_REMOTEIDWIDTH];
                  _aosShellRemoteSetupMessage(str, (remote_message_t){shell->remote.id, RMSG_DISCOVERRESPONSE});
                  // transmit response
                  shell->operation.channel->flags |= SHELLCHANNEL_OUTPUT_OVERRIDE;
                  chnWrite(shell->operation.channel, (uint8_t*)str, AOS_SHELL_RMSG_MAXLENGTH);
                  // disable output for all non-remote channels
                  if (!(shell->operation.channel->flags & SHELLCHANNEL_REMOTE_ESTABLISHED)) {
                    shell->operation.channel->flags &= ~SHELLCHANNEL_OUTPUT_OVERRIDE;
                  }
                  break;
                }

                case AOS_SHELL_ACTION_REMOTE_CLIENTRESPOND:
                {
                  // setup message
                  char str[2 + AOS_SHELL_REMOTEIDWIDTH];
                  _aosShellRemoteSetupMessage(str, (remote_message_t){shell->remote.id, RMSG_CLIENTRESPONSE});
                  // transmit response
                  chnWrite(shell->operation.channel, (uint8_t*)str, AOS_SHELL_RMSG_MAXLENGTH);
                  break;
                }

                case AOS_SHELL_ACTION_REMOTE_LIST:
                {
                  // check identifier
                  if (rmsg.id != REMOTEID_INVALID) {
                    // print received identifier
                    chprintf((BaseSequentialStream*)&shell->stream, "- ");
                    _aosShellRemotePrintId((BaseSequentialStream*)&shell->stream, rmsg.id);
                    chprintf((BaseSequentialStream*)&shell->stream, "\n");
                  } else {
                    chprintf((BaseSequentialStream*)&shell->stream, "! <invalid identifier>\n");
                  }
                  break;
                }

                case AOS_SHELL_ACTION_REMOTE_ACKNOWLEDGE:
                {
                  // set channel as remote client
                  shell->operation.channel->flags |= (SHELLCHANNEL_REMOTE_ESTABLISHED | SHELLCHANNEL_OUTPUT_OVERRIDE);
                  // acknowledge request
                  {
                    char msg[2 + AOS_SHELL_REMOTEIDWIDTH];
                    _aosShellRemoteSetupMessage(msg, (remote_message_t){shell->remote.id, RMSG_ACKNOWLEDGE});
                    streamWrite(shell->operation.channel, (uint8_t*)msg, AOS_SHELL_RMSG_MAXLENGTH);
                  }
                  // temporarily disable output for all other channels
                  for (AosShellChannel* c = shell->stream.channel; c != NULL; c = c->next) {
                    if (c != shell->operation.channel) {
                      c->flags |= SHELLCHANNEL_OUTPUT_DISCARD;
                    }
                  }
                  // print welcome message to current (remote) channel
                  chprintf((BaseSequentialStream*)&shell->stream, "\tok\nWelcome to shell ");
                  _aosShellRemotePrintId((BaseSequentialStream*)&shell->stream, shell->remote.id);
                  chprintf((BaseSequentialStream*)&shell->stream, "!\n");
                  // if this shell was client on the current channel before
                  if (shell->operation.remote.channel == shell->operation.channel) {
                    // reset remote channel
                    shell->operation.remote.channel = NULL;
                    // disable output on current (remote) channel but enable it on all non-remote channels
                    for (AosShellChannel* c = shell->stream.channel; c != NULL; c = c->next) {
                      if (c == shell->operation.channel) {
                        c->flags |= SHELLCHANNEL_OUTPUT_DISCARD;
                      } else if (!(c->flags & SHELLCHANNEL_REMOTE_ESTABLISHED)) {
                        c->flags &= ~SHELLCHANNEL_OUTPUT_DISCARD;
                      }
                    }
                    // print welcome message to all non-remote channels
                    chprintf((BaseSequentialStream*)&shell->stream, "\tok\nWelcome back to local shell!\n");
                    // re-enable output for current (remote channel)
                    shell->operation.channel->flags &= ~SHELLCHANNEL_OUTPUT_DISCARD;
                  }
                  // broadcast event
                  chEvtBroadcastFlags(&shell->events.source, AOS_SHELL_EVTFLAG_REMOTESERVER);
                  // if no command is being executed right now
                  if (!(shell->operation.flags & SHELL_OPFLAGS_EXECUTING)) {
                    // print the prompt and current input
                    _aosShellPrintPrompt(shell);
                    char* const line = _aosShellGetVisualisedEntry(shell);
                    if (line) {
                      _aosShellPrintString(shell, line, 0, shell->operation.input.length);
                      _aosShellMoveCursor(shell, line, shell->operation.input.length, shell->operation.input.cursorpos);
                    }
                  }
                  // re-enable output for all channels
                  for (AosShellChannel* c = shell->stream.channel; c != NULL; c = c->next) {
                    c->flags &= ~SHELLCHANNEL_OUTPUT_DISCARD;
                  }
                  break;
                }

                case AOS_SHELL_ACTION_REMOTE_ESTABLISH:
                {
                  // supress message if this channel was client before
                  if (shell->operation.channel->flags & SHELLCHANNEL_REMOTE_ESTABLISHED) {
                    shell->operation.channel->flags |= SHELLCHANNEL_OUTPUT_DISCARD;
                  }
                  shell->operation.channel->flags &= ~SHELLCHANNEL_OUTPUT_DISCARD;
                  // set channel as remote server
                  shell->operation.channel->flags |= (SHELLCHANNEL_REMOTE_ESTABLISHED | SHELLCHANNEL_OUTPUT_OVERRIDE);
                  shell->operation.remote.channel = shell->operation.channel;
                  // reset temporary id value
                  shell->operation.remote.requested = REMOTEID_INVALID;
                  // update state and broadcast event
                  shell->operation.flags &= ~(SHELL_OPFLAGS_EXECUTING | SHELL_OPFLAGS_REMOTEACTION_MASK);
                  chEvtBroadcastFlags(&shell->events.source, AOS_SHELL_EVTFLAG_DONE | AOS_SHELL_EVTFLAG_REMOTECLIENT);
                  // reset timer and discard pending timeout event
                  aosTimerReset(&shell->remote.timer);
                  chEvtWaitAnyTimeout(AOS_SHELL_EVENT_REMOTETIMER, TIME_IMMEDIATE);
                  break;
                }

                case AOS_SHELL_ACTION_REMOTE_CLOSE:
                {
                  // reset remote flags of current channel
                  shell->operation.channel->flags &= ~(SHELLCHANNEL_REMOTE_ESTABLISHED | SHELLCHANNEL_OUTPUT_OVERRIDE);
                  // reset temporary id value
                  shell->operation.remote.requested = REMOTEID_INVALID;
                  // print message
                  chprintf((BaseSequentialStream*)&shell->stream, "\tok\nWelcome back to local shell!\n");
                  // update state and broadcast event
                  shell->operation.flags &= ~(SHELL_OPFLAGS_EXECUTING | SHELL_OPFLAGS_REMOTEACTION_MASK);
                  chEvtBroadcastFlags(&shell->events.source, AOS_SHELL_EVTFLAG_DONE | AOS_SHELL_EVTFLAG_REMOTEDISCONNECTED);
                  // reset timer and discard pending timeout event
                  aosTimerReset(&shell->remote.timer);
                  chEvtWaitAnyTimeout(AOS_SHELL_EVENT_REMOTETIMER, TIME_IMMEDIATE);
                  // print prompt
                  _aosShellPrintPrompt(shell);
                  break;
                }

                case AOS_SHELL_ACTION_REMOTE_CANCEL_ACTIVE:
                {
                  // if the user triggered this cancelation
                  if ((shell->operation.flags & SHELL_OPFLAGS_REMOTEACTION_MASK) == SHELL_OPFLAGS_REMOTEACTION_DISCONNECT) {
                    // print a message
                    chprintf((BaseSequentialStream*)&shell->stream, "\tdisconnecting\n");
                  }
                  // emit cancelation message on current channel
                  const char msg[] = {REMOTEMSG_IDENTIFIER, REMOTEMSG_IDENTIFIER_CANCEL};
                  chnWrite(shell->operation.channel, (uint8_t*)msg, sizeof(msg));
                  // reset temporary id value
                  shell->operation.remote.requested = REMOTEID_INVALID;
                  // if a command is being executed right now
                  if (shell->operation.flags & SHELL_OPFLAGS_EXECUTING) {
                    // update state and set broadcast flags
                    shell->operation.flags &= ~(SHELL_OPFLAGS_EXECUTING | SHELL_OPFLAGS_REMOTEACTION_MASK);
                    emitflags |= AOS_SHELL_EVTFLAG_DONE;
                    // reset timer and discard pending timeout event
                    aosTimerReset(&shell->remote.timer);
                    chEvtWaitAnyTimeout(AOS_SHELL_EVENT_REMOTETIMER, TIME_IMMEDIATE);
                  }
                  __attribute__((fallthrough));
                }
                case AOS_SHELL_ACTION_REMOTE_CANCEL_PASSIVE:
                {
                  // reset remote data
                  const bool established = shell->operation.channel->flags & SHELLCHANNEL_REMOTE_ESTABLISHED;
                  shell->operation.channel->flags &= ~SHELLCHANNEL_REMOTE_ESTABLISHED;
                  if (shell->operation.channel == shell->operation.remote.channel) {
                    shell->operation.remote.channel = NULL;
                  }
                  // broadcast event
                  chEvtBroadcastFlags(&shell->events.source, emitflags | AOS_SHELL_EVTFLAG_REMOTEDISCONNECTED);
                  // reset output
                  shell->operation.channel->flags &= ~SHELLCHANNEL_OUTPUT_OVERRIDE;
                  // if this shell was server or client and no command is being executed right now
                  if (established && !(shell->operation.flags & SHELL_OPFLAGS_EXECUTING)) {
                    // print the prompt and current input
                    _aosShellPrintPrompt(shell);
                    char* const line = _aosShellGetVisualisedEntry(shell);
                    if (line) {
                      _aosShellPrintString(shell, line, 0, shell->operation.input.length);
                      _aosShellMoveCursor(shell, line, shell->operation.input.length, shell->operation.input.cursorpos);
                    }
                  }
                  break;
                }

                case AOS_SHELL_ACTION_REMOTE_DISABLE_ACTIVE:
                {
                  // emit disable message in current channel
                  const char msg[] = {REMOTEMSG_IDENTIFIER, REMOTEMSG_IDENTIFIER_DISABLE};
                  chnWrite(shell->operation.channel, (uint8_t*)msg, sizeof(msg));
                  // if a command is being executed right now
                  if (shell->operation.flags & SHELL_OPFLAGS_EXECUTING) {
                    // update state and set broadcast flags
                    shell->operation.flags &= ~SHELL_OPFLAGS_EXECUTING;
                    emitflags |= AOS_SHELL_EVTFLAG_DONE;
                    // reset timer and discard pending timeout event
                    aosTimerReset(&shell->remote.timer);
                    chEvtWaitAnyTimeout(AOS_SHELL_EVENT_REMOTETIMER, TIME_IMMEDIATE);
                  }
                  __attribute__((fallthrough));
                }
                case AOS_SHELL_ACTION_REMOTE_DISABLE_PASSIVE:
                {
                  // reset remote flags and disable channel
                  shell->operation.channel->flags &= ~(AOS_SHELLCHANNEL_REMOTE | SHELLCHANNEL_REMOTE_ESTABLISHED | SHELLCHANNEL_OUTPUT_OVERRIDE);
                  if (shell->operation.channel == shell->operation.remote.channel) {
                    shell->operation.remote.channel = NULL;
                  }
                  // broadcast event
                  chEvtBroadcastFlags(&shell->events.source, emitflags | AOS_SHELL_EVTFLAG_REMOTEDISABLED);
                  // print error message
                  chprintf((BaseSequentialStream*)&shell->stream, "\nWARNING: remote channel disabled\n");
                  // if no command is being executed right now
                  if (!(shell->operation.flags & SHELL_OPFLAGS_EXECUTING)) {
                    // print the prompt and current input
                    _aosShellPrintPrompt(shell);
                    char* const line = _aosShellGetVisualisedEntry(shell);
                    if (line) {
                      _aosShellPrintString(shell, line, 0, shell->operation.input.length);
                      _aosShellMoveCursor(shell, line, shell->operation.input.length, shell->operation.input.cursorpos);
                    }
                  }
                  break;
                }

#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */
              } /* end of switch(action) */

              // update operation data
              shell->operation.lastaction = (action != AOS_SHELL_ACTION_NONE) ? action : shell->operation.lastaction;
            } /* end of while chnReadTimeout() */
          }

          // update channel
#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE != true)
          if (shell->operation.flags & SHELL_OPFLAGS_ESCSEQUENCE) {
#else /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE != true) */
          if (shell->operation.flags & (SHELL_OPFLAGS_ESCSEQUENCE | SHELL_OPFLAGS_RMSGSEQUENCE)) {
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE != true) */
            // do not change the channel at all but break the loop
            break;
          } else if (shell->operation.channel == NULL) {
            // restart from the beginning
            shell->operation.channel = shell->stream.channel;
          } else {
            // iterate to next channel
            shell->operation.channel = shell->operation.channel->next;
          }
        } /* end of while(shell->operation.channel != NULL) */
        break;
      }

      // other events
      default:
      {
        // print an error message
        chprintf((BaseSequentialStream*)&shell->stream, "\nSHELL: ERROR: unknown event received (0x%08X)\n", eventmask);
        break;
      }

    } /* end of switch(evtmask) */

  } /* end of while(!chThdShouldTerminateX()) */

  // deinitialize
  chEvtUnregister(&aos.events.os, &shell->events.osListener);
#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true)
  aosTimerReset(&shell->remote.timer);
  chEvtUnregister(&shell->remote.evtSource, &shell->remote.evtListener);
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */
  for (AosShellChannel* channel = shell->stream.channel; channel != NULL; channel = channel->next) {
    chEvtUnregister(&(channel->asyncchannel->event), &(channel->listener));
  }

  // fire event and exit the thread
  chSysLock();
  shell->state = AOS_SHELL_STATE_TERMINATED;
  chEvtBroadcastFlagsI(&shell->events.source, AOS_SHELL_EVTFLAG_EXIT);
  chThdExitS(MSG_OK);
  // no chSysUnlock() required since the thread has been terminated an all waiting threads have been woken up
}

/* misc ***********************************************************************/

/**
 * @brief   Retrieve a shell object pointer from a thread.
 * @note    This function assumes, that the thread actually holds a shell.
 *
 * @param[in] thd   Pointer to a thread.
 *
 * @return  Pointer to the shell object.
 */
aos_shell_t* _aosShellRetrieveFromThread(thread_t* thd)
{
  return (void*)thd + MEM_ALIGN_NEXT(sizeof(thread_t), PORT_STACK_ALIGN) - THD_WORKING_AREA_SIZE(AMIROOS_CFG_SHELL_STACKSIZE);
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

/**
 * @addtogroup core_shell
 * @{
 */

/**
 * @brief   Initialize global shell data structures.
 */
void aosShellInit(void)
{
  // initialize command list mutex lock
  chMtxObjectInit(&_commands_mtx);

  // add builtin commands
  aosShellAddCommand(&_command_config);
  aosShellAddCommand(&_command_info);
  aosShellAddCommand(&_command_help);
#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true)
  aosShellAddCommand(&_command_remote);
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */

  return;
}

/**
 * @brief   Initializes a shell object with the specified parameters.
 *
 * @param[in,out] shell           Pointer to the shell object to be initialized.
 * @param[in]     name            Name of the shell thread (may be NULL).
 * @param[in]     prompt          Prompt line to print (NULL = use default
 *                                prompt).
 */
void aosShellObjectInit(aos_shell_t* shell, const char* name, const char* prompt)
{
  aosDbgCheck(shell != NULL);

  // initialize parameters
  shell->name = name;
  shell->prompt = prompt;
  shell->state = AOS_SHELL_STATE_INIT;
  shell->config = SHELL_CONFIG_DEFAULT;
  chEvtObjectInit(&shell->events.source);
  // no need to initialize shell->events.osListener
  _aosShellStreamInit(&shell->stream);
#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true)
#if (AMIROOS_CFG_SSSP_ENABLE == true)
  shell->remote.id = _aosShellRemoteHashString(BOARD_NAME, aos.sssp.moduleId);
#else /* (AMIROOS_CFG_SSSP_ENABLE == true) */
  shell->remote.id = _aosShellRemoteHashString(BOARD_NAME, 0);
#endif /* (AMIROOS_CFG_SSSP_ENABLE == true) */
  if (shell->name != NULL) {
    shell->remote.id = _aosShellRemoteHashString(shell->name, shell->remote.id);
  }
  aosDbgAssert(shell->remote.id != REMOTEID_INVALID);
  aosTimerInit(&shell->remote.timer);
  chEvtObjectInit(&shell->remote.evtSource);
  // no need to initialize shell->remote.timerEvtListener
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */
  memset(shell->buffer, INBUF_INIT_CHAR, (AMIROOS_CFG_SHELL_HISTLENGTH+1) * AMIROOS_CFG_SHELL_LINEWIDTH * sizeof(char));

  return;
}

/**
 * @brief   Start the shell thread.
 *
 * @param[in] shell   Pointer to the shell to start.
 * @param[in] prio    Priority of the shell thread.
 */
void aosShellStart(aos_shell_t* shell, tprio_t prio)
{
  aosDbgCheck(shell != NULL);

  chThdCreateStatic(shell->thread_wa, sizeof(shell->thread_wa), prio, _aosShellThreadFunction, shell);

  return;
}

/**
 * @brief   Inserts a command to the shells list of commands.
 *
 * @param[in] cmd     Pointer to the command to add.
 *
 * @return            A status value.
 * @retval AOS_SUCCESS  The command was added successfully.
 * @retval AOS_ERROR    Another command with identical name already exists.
 */
aos_status_t aosShellAddCommand(aos_shellcommand_t *cmd)
{
  aosDbgCheck(cmd != NULL);
  aosDbgCheck(cmd->name != NULL &&
              strlen(cmd->name) > 0 &&
              strchr(cmd->name, ' ') == NULL &&
              strchr(cmd->name, ASCII_HT) == NULL);
  aosDbgCheck(cmd->callback != NULL);
  aosDbgCheck(cmd->next == NULL);

  // local variables
  aos_shellcommand_t* prev = NULL;

  chMtxLock(&_commands_mtx);

  // insert the command to the list wrt lexographical order (exception: lower case characters preceed upper their uppercase counterparts)
  for (aos_shellcommand_t* curr = _commands; curr != NULL; curr = curr->next) {
    // iterate through the list as long as the command names are 'smaller'
    const int cmp = _aosShellStrccmp(curr->name, cmd->name, true, NULL, NULL);
    if (cmp < 0) {
      prev = curr;
      continue;
    }
    // insert the command as soon as a 'larger' name was found
    else if (cmp > 0) {
      cmd->next = curr;
      // special case: the first command is larger
      if (prev == NULL) {
        _commands = cmd;
      } else {
        prev->next = cmd;
      }
      chMtxUnlock(&_commands_mtx);
      return AOS_SUCCESS;
    }
    // error if the command name already exists
    else /* if (cmp == 0) */ {
      chMtxUnlock(&_commands_mtx);
      return AOS_ERROR;
    }

  }
  // the end of the list has been reached

  // append the command
  if (prev) {
    prev->next = cmd;
  } else {
    _commands = cmd;
  }

  chMtxUnlock(&_commands_mtx);

  return AOS_SUCCESS;
}

/**
 * @brief   Removes a command from the shells list of commands.
 *
 * @param[in] cmd       Name of the command to removde.
 * @param[out] removed  Optional pointer to the command that was removed.
 *
 * @return              A status value.
 * @retval AOS_SUCCESS  The command was removed successfully.
 * @retval AOS_ERROR    The command name was not found.
 */
aos_status_t aosShellRemoveCommand(const char* cmd, aos_shellcommand_t** removed)
{
  aosDbgCheck(cmd != NULL && strlen(cmd) > 0);

  // local variables
  aos_shellcommand_t* prev = NULL;

  // iterate through the list and seach for the specified command name
  chMtxLock(&_commands_mtx);
  for (aos_shellcommand_t* curr = _commands; curr != NULL; curr = curr->next) {
    const int cmp = strcmp(curr->name, cmd);
    // iterate through the list as long as the command names are 'smaller'
    if (cmp < 0) {
      prev = curr;
      continue;
    }
    // remove the command when found
    else if (cmp == 0) {
      // special case: the first command matches
      if (prev == NULL) {
        _commands = curr->next;
      } else {
        prev->next = curr->next;
      }
      curr->next = NULL;
      // set the optional output argument
      if (removed != NULL) {
        *removed = curr;
      }
      chMtxUnlock(&_commands_mtx);
      return AOS_SUCCESS;
    }
    // break the loop if the command names are 'larger'
    else /* if (cmpval > 0) */ {
      break;
    }
  }
  chMtxUnlock(&_commands_mtx);

  // if the command was not found, return an error
  return AOS_ERROR;
}

/** @} */

/**
 * @brief   Initialize an AosShellChannel object with the specified parameters.
 *
 * @ingroup core_shell_shellchannel
 *
 * @param[in] channel       The AosShellChannel to initialize.
 * @param[in] asyncchannel  An BaseAsynchronousChannel this AosShellChannel is
 *                          associated with.
 * @param[in] flags         Flags to set to the AosShellChannel.
 */
void aosShellChannelInit(AosShellChannel* channel, BaseAsynchronousChannel* asyncchannel, aos_shellchannelflags_t flags)
{
  aosDbgCheck(channel != NULL);
  aosDbgCheck(asyncchannel != NULL);
  aosDbgCheck((flags & ~AOS_SHELLCHANNEL_FLAGSMASK) == 0);

  channel->vmt = &_channelvmt;
  channel->next = NULL;
  channel->asyncchannel = asyncchannel;
#if (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true)
  if (flags & AOS_SHELLCHANNEL_REMOTE) {
    flags |= AOS_SHELLCHANNEL_INPUT;
  }
#endif /* (AMIROOS_CFG_SHELL_REMOTE_ENABLE == true) */
  channel->flags = flags;

  return;
}

/**
 * @brief   Add a channel to a AosShellStream.
 *
 * @ingroup core_shell_shellstream
 *
 * @param[in] stream    The AosShellStream to extend.
 * @param[in] channel   The channel to be added to the stream.
 */
void aosShellStreamAddChannel(AosShellStream* stream, AosShellChannel* channel)
{
  aosDbgCheck(stream != NULL);
  aosDbgCheck(channel != NULL &&
      channel->asyncchannel != NULL &&
      channel->next == NULL &&
      (channel->flags & ~AOS_SHELLCHANNEL_FLAGSMASK) == 0);

  // prepend the new channel
  channel->next = stream->channel;
  channel->flags |= SHELLCHANNEL_ATTACHED;
  stream->channel = channel;

  return;
}

/**
 * @brief   Check whether the user requested help by typing '--help' or '-h'.
 * @note    This function is not inline in order to reduce image size.
 *
 * @ingroup core_shell
 *
 * @param[in] argc    Number of arguments passed to the shell command.
 * @param[in] argv    List of argument strings.
 * @param[in] abbrev  Flag whether short option '-h' is allowed.
 *
 * @return  Indicator whether the user requested help.
 * @retval AOS_SHELL_HELPREQUEST_NONE       User did not request help.
 * @retval AOS_SHELL_HELPREQUEST_EXPLICIT   User requested help explicitely by typing '--help' or '-h'.
 * @retval AOS_SHELL_HELPREQUEST_IMPLICIT   User requested help implicitely by calling the command without arguments.
 */
aos_shellhelprequest_t aosShellcmdCheckHelp(int argc, const char* argv[], bool abbrev)
{
  if (argc < 2) {
    return AOS_SHELL_HELPREQUEST_IMPLICIT;
  } else if ((abbrev && strcmp(argv[1], "-h") == 0) || strcmp(argv[1], "--help") == 0) {
    return AOS_SHELL_HELPREQUEST_EXPLICIT;
  } else {
    return AOS_SHELL_HELPREQUEST_NONE;
  }
}

#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */
