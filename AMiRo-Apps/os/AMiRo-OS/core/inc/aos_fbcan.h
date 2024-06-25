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
 * @file    aos_fbcan.h
 * @brief   Filtered & Buffered CAN driver macros and structures.
 *
 * @defgroup core_fbcan_sdcan Serial over CAN
 * @ingroup core_fbcan
 *
 * @brief   Todo
 * @details Todo
 */

#ifndef AMIROOS_FBCAN_H
#define AMIROOS_FBCAN_H

#include <aosconf.h>

#if ((AMIROOS_CFG_FBCAN1_ENABLE == true) ||                                     \
     (AMIROOS_CFG_FBCAN2_ENABLE == true) ||                                     \
     (AMIROOS_CFG_FBCAN3_ENABLE == true)) ||                                    \
    defined(__DOXYGEN__)

#include <stdint.h>
#include <hal.h>
#include "aos_types.h"

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

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
 * @addtogroup core_fbcan
 * @{
 */

/**
 * @brief   Callback type for FBCAN receive filters.
 *
 * @param[in] frame   The received CAN frame.
 * @param[in] params  Optional custom parameters for the callback function.
 */
typedef void (*aos_fbcan_filtercb_t)(const CANRxFrame* frame, void* params);

/**
 * @brief   Filter mask type for FBCAN receive filters.
 */
typedef union aos_fbcan_rxmask {
  /**
   * @brief   Standard frame mask.
   */
  struct {
    uint32_t rsvd : 18; /**< reserved */
    uint32_t id   : 11; /**< 11 bit standard frame idetifier. */
    uint32_t ide  : 1;  /**< IDE bit. */
    uint32_t rtr  : 1;  /**< RTR bit. */
  } std;

  /**
   * @brief   Extended frame mask
   */
  struct {
    uint32_t id     : 29; /**< 29 bit extended frame identifier. */
    uint32_t ide    : 1;  /**< IDE bit. */
    uint32_t rtr    : 1;  /**< RTR bit. */
  } ext;

  /**
   * @brief   Raw data of the mask struct.
   */
  uint32_t raw;
} aos_fbcan_rxmask_t;

/**
 * @brief   FBCAN filter.
 */
typedef struct aos_fbcan_filter {
  /**
   * @brief   Filter mask.
   * @details This mask selects, which bits of the identifier are filtered as
   *          "must match" (set to 1) or "don't care" (set to 0).
   */
  aos_fbcan_rxmask_t mask;

  /**
   * @brief   Identfier filter.
   * @details Incoming CAN messages are filterd using this filter and the mask.
   */
  aos_fbcan_rxmask_t filter;

  /**
   * @brief   Callback to executed whenever a CAN message matches the filter.
   */
  aos_fbcan_filtercb_t callback;

  /**
   * @brief   optional parameters for the callback.
   */
  void* cbparams;

  /**
   * @brief   Poimter to the next filter in a singly-linked list.
   */
  struct aos_fbcan_filter* next;
} aos_fbcan_filter_t;

/**
 * @brief   Buffer element for the FBCAN transmit queue.
 * @details Buffer elements are structured as doubly-linked list.
 */
typedef struct aos_fbcan_txframe {
  /**
   * @brief   The CAN frame to be transmitted.
   */
  CANTxFrame frame;

  /**
   * @brief   Pointer to the preceeding element in the queue.
   */
  struct aos_fbcan_txframe* prev;

  /**
   * @brief   Pointer to the subsequend element in the queue.
   */
  struct aos_fbcan_txframe* next;

} aos_fbcan_txframe_t;

/**
 * @brief   Enqueue callback function type.
 *
 * @param[in] frame             Frame to enqueue.
 *                              Must not be @p NULL.
 * @param[in,out] queue_front   Pointer to the first element in the Queue.
 *                              Must not be @p NULL.
 *                              If the frame is prepended, the @p prev pointer
 *                              must be modified accordingly.
 * @param[in,out] queue_back    Pointer to the last element in the queue.
 *                              Must not be @p NULL.
 *                              If the frame is appended, the @p next pointer
 *                              must be modified accordingly.
 * @param[in] params            Optional parameters.
 */
typedef void (*aos_fbcan_enqueuecb_t)(aos_fbcan_txframe_t* frame, aos_fbcan_txframe_t* queue_front, aos_fbcan_txframe_t* queue_back, void* params);

/**
 * @brief   FBCAN structure.
 */
typedef struct {

  /**
   * @brief   Pointer to the associater CAN driver.
   */
  CANDriver* const can;

  /**
   * @brief   Pointer to the first element in a list of receive filters.
   */
  aos_fbcan_filter_t* rxfilters;

  /**
   * @brief   Transmit buffer.
   */
  struct {
    /**
     * @brief   Pointer to the first element of a list of available messages.
     */
    aos_fbcan_txframe_t* avail;

    /**
     * @brief   Size of the buffer.
     */
    size_t size;

    /**
     * @brief   Pointer to the first element in the transmission queue.
     */
    aos_fbcan_txframe_t* queue_front;

    /**
     * @brief   Pointer to the last element in the transmission queue.
     */
    aos_fbcan_txframe_t* queue_back;

    /**
     * @brief   Optional callback to enqueue frames in the transmit buffer.
     * @details If not set, messages will be queued in FIFO order.
     */
    aos_fbcan_enqueuecb_t enqueue_callback;

    /**
     * @brief   Optional parameters propagated to the ernqueue callback.
     */
    void* cbparams;
  } txbuffer;

  /**
   * @brief   Events.
   */
  struct {
    /**
     * @brief   One or more frames become available.
     * @note    After broadcasting this event it will not be broadcasted again
     *          until the received frames queue has been completely emptied. It
     *          is <b>not</b> broadcasted for each received frame. It is
     *          responsibility of the application to empty the queue by
     *          repeatedly invoking @p canReceive() when listening to this
     *          event. This behavior minimizes the interrupt served by the
     *          system because CAN traffic.
     * @note    The flags associated to the listeners will indicate which
     *          receive mailboxes become non-empty.
     */
    event_source_t rxfull;

    /**
     * @brief   One or more transmission mailbox become available.
     * @note    The flags associated to the listeners will indicate which
     *          transmit mailboxes become empty.
     * @note    The upper 16 bits are transmission error flags associated
     *          to the transmit mailboxes.
     */
    event_source_t txempty;

    /**
     * @brief   A CAN bus error happened.
     * @note    The flags associated to the listeners will indicate that
     *          receive error(s) have occurred.
     * @note    In this implementation the upper 16 bits are filled with the
     *          unprocessed content of the ESR register.
     */
    event_source_t error;

#if (CAN_USE_SLEEP_MODE == TRUE) || defined (__DOXYGEN__)

    /**
     * @brief   Entering sleep state event.
     */
    event_source_t sleep;

    /**
     * @brief   Exiting sleep state event.
     */
    event_source_t wakeup;

#endif /* (CAN_USE_SLEEP_MODE == TRUE) */

  } events;

} AosFBCANDriver;

/** @} */

#if (AMIROOS_CFG_FBCAN_SERIAL_BUFFERSIZE > 0) || defined(__DOXYGEN__)

/**
 * @ingroup core_fdcan_sdcan
 * @{
 */

/**
 * @brief Driver state machine possible states.
 */
typedef enum {
  SDCAN_UNINIT  = 0,  /**< Not initialized. */
  SDCAN_STOP    = 1,  /**< Stopped. */
  SDCAN_READY   = 2,  /**< Ready. */
} sdcan_state_t;

/**
 * @brief   Structure representing the Serial over CAN driver.
 */
typedef struct SerialCANDriver SerialCANDriver;

/**
 * @brief   Serial over CAN Driver configuration structure.
 * @details An instance of this structure must be passed to @p aosSdcanStart()
 *          in order to configure and start the driver operations.
 */
typedef struct {
  /**
   * @brief   FBCAN driver to use.
   */
  AosFBCANDriver* fbcanp;

  /**
   * @brief   CAN identifier to send and listen to.
   */
  aos_fbcan_rxmask_t canid;
} SerialCANConfig;

/** @} */

/**
 * @brief   @p SerialDriver specific data.
 */
#define _serial_can_driver_data                                                 \
  _base_asynchronous_channel_data                                               \
  /* Driver state.*/                                                            \
  sdcan_state_t state;                                                          \
  /* Input buffers queue.*/                                                     \
  input_queue_t iqueue;                                                         \
  /* Input buffer.*/                                                            \
  uint8_t ib[AMIROOS_CFG_FBCAN_SERIAL_BUFFERSIZE];                              \
  /* FBCAN receive filter */                                                    \
  aos_fbcan_filter_t filter;                                                    \
  /* End of the mandatory fields.*/                                             \
  /* Current configuration data.*/                                              \
  const SerialCANConfig *config;

/**
 * @brief   @p SerialCANDriver specific methods.
 */
#define _serial_can_driver_methods                                              \
  _base_asynchronous_channel_methods

/**
 * @ingroup core_fbcan_sdcan
 * @{
 */

/**
 * @brief   @p SerialDriver virtual methods table.
 *
 * @extends BaseAsynchronousChannelVMT
 */
struct SerialCANDriverVMT {
  _serial_can_driver_methods
};

/**
 * @brief   Serial driver over FBCAN class.
 * @details This class extends @p BaseAsynchronousChannel.
 *
 * @extends BaseAsynchronousChannel
 */
struct SerialCANDriver {
  /** @brief Virtual Methods Table.*/
  const struct SerialCANDriverVMT *vmt;
  _serial_can_driver_data
};

/** @} */

#endif /* (AMIROOS_CFG_FBCAN_SERIAL_BUFFERSIZE > 0) */

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if (AMIROOS_CFG_FBCAN1_ENABLE == true) || defined(__DOXYGEN__)
extern AosFBCANDriver FBCAND1;
#endif /* (AMIROOS_CFG_FBCAN1_ENABLE == true) */

#if (AMIROOS_CFG_FBCAN2_ENABLE == true) || defined(__DOXYGEN__)
extern AosFBCANDriver FBCAND2;
#endif /* (AMIROOS_CFG_FBCAN2_ENABLE == true) */

#if (AMIROOS_CFG_FBCAN3_ENABLE == true) || defined(__DOXYGEN__)
extern AosFBCANDriver FBCAND3;
#endif /* (AMIROOS_CFG_FBCAN3_ENABLE == true) */


#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus)*/
  void aosFBCanInit(AosFBCANDriver* fbcanp, aos_fbcan_enqueuecb_t enqueuecb, void* enqueuecbparams);
  size_t aosFBCanProvideBuffer(AosFBCANDriver* fbcanp, aos_fbcan_txframe_t* buffer, size_t size);
  CANTxFrame* aosFBCanAcquireTxFrame(AosFBCANDriver* fbcanp, sysinterval_t timeout);
  aos_status_t aosFBCanTransmit(AosFBCANDriver* fbcanp, const CANTxFrame* frame);
  aos_status_t aosFBCanTransmitTimeout(AosFBCANDriver* bcanp, const CANTxFrame* frame, sysinterval_t timeout);
  aos_status_t aosFBCanRemoveFilterI(AosFBCANDriver* fbcanp, aos_fbcan_filter_t* filter);
#if (CAN_USE_SLEEP_MODE == TRUE)
  void aosFBCanSleep(AosFBCANDriver *fbcanp);
  void aosFBCanWakeup(AosFBCANDriver *fbcanp);
#endif /* (CAN_USE_SLEEP_MODE == TRUE) */

#if (AMIROOS_CFG_FBCAN_SERIAL_BUFFERSIZE > 0) || defined(__DOXYGEN__)
  void sdcanObjectInit(SerialCANDriver* sdcanp);
  void sdcanStart(SerialCANDriver* sdcanp, const SerialCANConfig* config);
  void sdcanStop(SerialCANDriver* sdcanp);
  msg_t sdcanControl(SerialCANDriver* sdcanp, unsigned int operation, void* arg);
#endif /* (AMIROOS_CFG_FBCAN_SERIAL_BUFFERSIZE > 0) */
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus)*/

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/**
 * @addtogroup core_fbcan
 * @{
 */

/**
 * @brief   Configures and activates the CAN peripheral.
 * @note    Activating the CAN bus can be a slow operation.
 * @note    Unlike other drivers it is not possible to restart the CAN driver
 *          without first stopping it using bufferedCanStop().
 *
 * @param[in] fbcanp  Pointer to the @p AosFBCANDriver object.
 * @param[in] config  Pointer to the @p CANConfig object.
 *                    Depending on the implementation the value can be @p NULL.
 */
static inline void aosFBCanStart(AosFBCANDriver* fbcanp, const CANConfig* config)
{
  // start CAN driver
  canStart(fbcanp->can, config);

  return;
}

/**
 * @brief   Deactivates the CAN peripheral.
 *
 * @param[in] fbcanp  Pointer to the @p AosFBCANDriver object.
 */
static inline void aosFBCanStop(AosFBCANDriver* fbcanp)
{
  // stop CAN driver
  canStop(fbcanp->can);

  return;
}

/**
 * @brief   Add a receive filter.
 *
 * @param[in] fbcanp  Pointer to the @p AosFBCANDriver object.
 * @param[in] filter  Pointer to the filter to be added.
 */
static inline void aosFBCanAddFilterI(AosFBCANDriver* fbcanp, aos_fbcan_filter_t* filter)
{
  chDbgCheckClassI();

  // prepend filter to the list
  filter->next = fbcanp->rxfilters;
  fbcanp->rxfilters = filter;

  return;
}

/**
 * @brief   Add a receive filter.
 *
 * @param[in] fbcanp  Pointer to the @p AosFBCANDriver object.
 * @param[in] filter  Pointer to the filter to be added.
 */
static inline void aosFBCanAddFilter(AosFBCANDriver* fbcanp,
                                     aos_fbcan_filter_t* filter)
{
  chSysLock();
  aosFBCanAddFilterI(fbcanp, filter);
  chSysUnlock();
}

/**
 * @brief   Renove a receive filter.
 *
 * @param[ni] fbcanp  Pointer to the @p AosFBCANDriver object.
 * @param[ni] filter  Pointer to the filter to be added.
 *
 * @return  The operation result.
 * @retval AOS_SUCCESS  The filter has been removed.
 * @retval AOS_ERROR    The filter is not associated to this @p AosFBCANDriver.
 */
static inline aos_status_t aosFBCanRemoveFilter(AosFBCANDriver* fbcanp, aos_fbcan_filter_t* filter)
{
  aos_status_t status;

  chSysLock();
  status = aosFBCanRemoveFilterI(fbcanp, filter);
  chSysUnlock();

  return status;
}

/** @} */

#if (AMIROOS_CFG_FBCAN_SERIAL_BUFFERSIZE > 0) || defined(__DOXYGEN__)

/**
 * @addtogroup core_fbcan_sdcan
 * @{
 */

/**
 * @brief   Serial Driver initialization.
 */
static inline void sdcanInit(void)
{
  return;
}

/** @} */

#endif /* (AMIROOS_CFG_FBCAN_SERIAL_BUFFERSIZE > 0) */

#endif /*(AMIROOS_CFG_FBCANX_ENABLE == true) */

#endif /* AMIROOS_FBCAN_H */
