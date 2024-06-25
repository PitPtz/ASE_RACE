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
 * @file    aos_time.c
 * @brief   Time related function implementations.
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

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup core_thread
 * @{
 */

/**
 * @brief   Calculates the day of week from a specified date.
 *
 * @param[in] day     The day of the date.
 * @param[in] month   The month of the daste.
 * @param[in] year    The year of the date.
 *
 * @return    The day of week depending on the date represented in ISO format
 *            (1 = Monday to 7 = Sunday).
 */
uint8_t aosTimeDayOfWeekFromDate(const uint16_t day, const uint8_t month, const uint16_t year)
{
  aosDbgCheck(day >= 1 && day <= 31);
  aosDbgCheck(month >= 1 && month <= 12);

  /*
   * Implementation of Zeller's congruence for Gregorian calender.
   * See https://en.wikipedia.org/wiki/Zeller%27s_congruence for details.
   */
  const uint32_t q = day;
  const uint32_t m = (month >= 3) ? month : month + 12;
  const uint32_t K = year % 100;
  const uint32_t J = year / 100;

  const uint32_t h = (q + ((13 * (m + 1)) / 5) + K + (K / 4) + (J / 4) - (2 * J)) % 7;

  return ((h + 5) % 7) + 1;
}

/** @} */
