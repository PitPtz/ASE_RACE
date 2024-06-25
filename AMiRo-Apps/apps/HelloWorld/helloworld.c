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
 * @file    helloworld.c
 * @brief   A simple HelloWorld application.
 */

#include <helloworld.h>
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

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_helloworld
 * @{
 */

#if ((URT_CFG_PUBSUB_ENABLED == true) || (URT_CFG_RPC_ENABLED == true)) || defined(__DOXYGEN__)

/**
 * @brief   Callback function to calculate usefulness (SRT only).
 *
 * @param[in] dt      Latency to calculate usefulness for.
 * @param[in] params  Additional parameter (not used).
 *
 * @return  A usefulness value in range [0,1].
 */
float helloworldSrtUsefulness(urt_delay_t dt, void* params)
{
  (void)params;

  return 1.0f / ( ((float)dt / MICROSECONDS_PER_MILLISECOND) + 1.0f );
}

#endif /* (URT_CFG_PUBSUB_ENABLED == true) || (URT_CFG_RPC_ENABLED == true) */

/** @} */
