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
 * @file    aos_time.h
 * @brief   Time related constants and data types.
 *
 * @addtogroup core_time
 * @{
 */

#ifndef AMIROOS_TIME_H
#define AMIROOS_TIME_H

#include <stdint.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

#define MICROSECONDS_PER_MICROSECOND  ((uint8_t)  (1))  /**< Number of microseconds per microsecond. */
#define MILLISECONDS_PER_MILLISECOND  ((uint8_t)  (1))  /**< Number of milliseconds per millisecond. */
#define SECONDS_PER_SECOND            ((uint8_t)  (1))  /**< Number of seconds per second. */
#define MINUTES_PER_MINUTE            ((uint8_t)  (1))  /**< Number of minutes per minute. */
#define HOURS_PER_HOUR                ((uint8_t)  (1))  /**< Number of hours per hour. */
#define DAYS_PER_DAY                  ((uint8_t)  (1))  /**< Number of days per day. */
#define WEEKS_PER_WEEK                ((uint8_t)  (1))  /**< Number of weeks per week. */
#define MONTHS_PER_MONTH              ((uint8_t)  (1))  /**< Number of months per month. */
#define YEARS_PER_YEAR                ((uint8_t)  (1))  /**< Number of years per year. */
#define DECADES_PER_DECADE            ((uint8_t)  (1))  /**< Number of decades per decade. */
#define CENTURIES_PER_CENTURY         ((uint8_t)  (1))  /**< Number of centuries per century. */
#define MILLENIA_PER_MILLENIUM        ((uint8_t)  (1))  /**< Number of millenia per millenium. */

#define MICROSECONDS_PER_MILLISECOND  ((uint16_t) (1000))     /**< Number of microseconds per millisecond. */
#define MILLISECONDS_PER_SECOND       ((uint16_t) (1000))     /**< Number of milliseconds per second. */
#define SECONDS_PER_MINUTE            ((uint8_t)  (60))       /**< Number of seconds per minute. */
#define MINUTES_PER_HOUR              ((uint8_t)  (60))       /**< Number of minutes per hour. */
#define HOURS_PER_DAY                 ((uint8_t)  (24))       /**< Number of hours per day. */
#define DAYS_PER_WEEK                 ((uint8_t)  (7))        /**< Number of days per week. */
#define DAYS_PER_YEAR                 ((float)    (365.25f))  /**< Number of days per year. */
#define MONTHS_PER_YEAR               ((uint8_t)  (12))       /**< Number of months per year. */
#define YEARS_PER_DECADE              ((uint8_t)  (10))       /**< Number of years per decade. */
#define DECADES_PER_CENTURY           ((uint8_t)  (10))       /**< Number of decades per century. */
#define CENTURIES_PER_MILLENIUM       ((uint8_t)  (10))       /**< Number of centuries per millenium. */

#define MICROSECONDS_PER_SECOND       ((uint32_t) ((uint32_t)MICROSECONDS_PER_MILLISECOND * (uint32_t)MILLISECONDS_PER_SECOND)) /**< Number of microseconds per second. */
#define MILLISECONDS_PER_MINUTE       ((uint16_t) ((uint16_t)MILLISECONDS_PER_SECOND * (uint16_t)SECONDS_PER_MINUTE))           /**< Number of milliseconds per minute. */
#define SECONDS_PER_HOUR              ((uint16_t) ((uint16_t)SECONDS_PER_MINUTE * (uint16_t)MINUTES_PER_HOUR))                  /**< Number of seconds per hour. */
#define MINUTES_PER_DAY               ((uint16_t) ((uint16_t)MINUTES_PER_HOUR * (uint16_t)HOURS_PER_DAY))                       /**< Number of minutes per day. */
#define HOURS_PER_WEEK                ((uint8_t)  (HOURS_PER_DAY * DAYS_PER_WEEK))                                              /**< Number of hours per week. */
#define HOURS_PER_YEAR                ((uint16_t) ((float)HOURS_PER_DAY * DAYS_PER_YEAR))                                       /**< Number of hours per year. */
#define DAYS_PER_MONTH                ((float)    (DAYS_PER_YEAR / (float)MONTHS_PER_YEAR))                                     /**< Number of days per month. */
#define DAYS_PER_DECADE               ((float)    (DAYS_PER_YEAR * (float)YEARS_PER_DECADE))                                    /**< Number of days per decade. */
#define WEEKS_PER_YEAR                ((float)    (DAYS_PER_YEAR / (float)DAYS_PER_WEEK))                                       /**< Number of weeks per year. */
#define MONTHS_PER_DECADE             ((uint8_t)  (MONTHS_PER_YEAR * YEARS_PER_DECADE))                                         /**< Number of months per decade. */
#define YEARS_PER_CENTURY             ((uint8_t)  (YEARS_PER_DECADE * DECADES_PER_CENTURY))                                     /**< Number of years per century. */
#define DECADES_PER_MILLENIUM         ((uint8_t)  (DECADES_PER_CENTURY * CENTURIES_PER_MILLENIUM))                              /**< Number of decades per millenium. */

#define MICROSECONDS_PER_MINUTE       ((uint32_t) ((uint32_t)MICROSECONDS_PER_MILLISECOND * (uint32_t)MILLISECONDS_PER_MINUTE)) /**< Number of microseconds per minute. */
#define MILLISECONDS_PER_HOUR         ((uint32_t) ((uint32_t)MILLISECONDS_PER_SECOND * (uint32_t)SECONDS_PER_HOUR))             /**< Number of milliseconds per hour. */
#define SECONDS_PER_DAY               ((uint32_t) ((uint32_t)SECONDS_PER_MINUTE * (uint32_t)MINUTES_PER_DAY))                   /**< Number of seconds per day. */
#define MINUTES_PER_WEEK              ((uint16_t) ((uint16_t)MINUTES_PER_HOUR * (uint16_t)HOURS_PER_WEEK))                      /**< Number of minutes per week. */
#define HOURS_PER_MONTH               ((float)    ((float)HOURS_PER_DAY * DAYS_PER_MONTH))                                      /**< Number of hours per month. */
#define HOURS_PER_DECADE              ((uint32_t) ((float)HOURS_PER_DAY * DAYS_PER_DECADE))                                     /**< Number of hours per decade. */
#define MINUTES_PER_YEAR              ((uint32_t) ((uint32_t)MINUTES_PER_HOUR * (uint32_t)HOURS_PER_YEAR))                      /**< Number of minutes per year. */
#define DAYS_PER_CENTURY              ((uint16_t) (DAYS_PER_YEAR * (float)YEARS_PER_CENTURY))                                   /**< Number of days per century. */
#define WEEKS_PER_MONTH               ((float)    (DAYS_PER_MONTH / (float)DAYS_PER_WEEK))                                      /**< Number of weeks per month. */
#define WEEKS_PER_DECADE              ((float)    (DAYS_PER_DECADE / (float)DAYS_PER_WEEK))                                     /**< Number of weeks per decade. */
#define MONTHS_PER_CENTURY            ((uint16_t) ((uint16_t)MONTHS_PER_YEAR * (uint16_t)YEARS_PER_CENTURY))                    /**< Number of months per century. */
#define YEARS_PER_MILLENIUM           ((uint16_t) ((uint16_t)YEARS_PER_DECADE * (uint16_t)DECADES_PER_MILLENIUM))               /**< Number of years per millenium. */

#define MICROSECONDS_PER_HOUR         ((uint32_t) ((uint32_t)MICROSECONDS_PER_MILLISECOND * MILLISECONDS_PER_HOUR)) /**< Number of microseconds per hour. */
#define MILLISECONDS_PER_DAY          ((uint32_t) ((uint32_t)MILLISECONDS_PER_SECOND * SECONDS_PER_DAY))            /**< Number of milliseconds per day. */
#define SECONDS_PER_WEEK              ((uint32_t) (SECONDS_PER_MINUTE * (uint32_t)MINUTES_PER_WEEK))                /**< Number of seconds per week. */
#define SECONDS_PER_YEAR              ((uint32_t) (SECONDS_PER_MINUTE * MINUTES_PER_YEAR))                          /**< Number of seconds per year. */
#define MINUTES_PER_MONTH             ((uint16_t) ((float)MINUTES_PER_HOUR * HOURS_PER_MONTH))                      /**< Number of minutes per month. */
#define MINUTES_PER_DECADE            ((uint32_t) ((uint32_t)MINUTES_PER_HOUR * HOURS_PER_DECADE))                  /**< Number of minutes per decade. */
#define HOURS_PER_CENTURY             ((uint32_t) ((uint32_t)HOURS_PER_DAY * (uint32_t)DAYS_PER_CENTURY))           /**< Number of hours per century. */
#define DAYS_PER_MILLENIUM            ((uint32_t) (DAYS_PER_YEAR * (float)YEARS_PER_MILLENIUM))                     /**< Number of days per millenium. */
#define WEEKS_PER_CENTURY             ((float)    ((float)DAYS_PER_CENTURY / (float)DAYS_PER_WEEK))                 /**< Number of weeks per century. */
#define MONTHS_PER_MILLENIUM          ((uint16_t) ((uint16_t)MONTHS_PER_YEAR * YEARS_PER_MILLENIUM))                /**< Number of months per millenium. */

#define MICROSECONDS_PER_DAY          ((uint64_t) ((uint64_t)MICROSECONDS_PER_MILLISECOND * (uint64_t)MILLISECONDS_PER_DAY))  /**< Number of microseconds per day. */
#define MILLISECONDS_PER_WEEK         ((uint32_t) ((uint32_t)MILLISECONDS_PER_SECOND * SECONDS_PER_WEEK))                     /**< Number of milliseconds per week. */
#define MILLISECONDS_PER_YEAR         ((uint64_t) ((uint64_t)MILLISECONDS_PER_SECOND * (uint64_t)SECONDS_PER_YEAR))           /**< Number of milliseconds per year. */
#define SECONDS_PER_MONTH             ((uint32_t) ((uint32_t)SECONDS_PER_MINUTE * (uint32_t)MINUTES_PER_MONTH))               /**< Number of seconds per month. */
#define SECONDS_PER_DECADE            ((uint32_t) ((uint32_t)SECONDS_PER_MINUTE * MINUTES_PER_DECADE))                        /**< Number of seconds per decade. */
#define MINUTES_PER_CENTURY           ((uint32_t) ((uint32_t)MINUTES_PER_HOUR * HOURS_PER_CENTURY))                           /**< Number of minutes per century. */
#define HOURS_PER_MILLENIUM           ((uint32_t) ((uint32_t)HOURS_PER_DAY * DAYS_PER_MILLENIUM))                             /**< Number of hours per millenium. */
#define WEEKS_PER_MILLENIUM           ((float)    ((float)DAYS_PER_MILLENIUM / (float)DAYS_PER_WEEK))                         /**< Number of weeks per millenium. */

#define MICROSECONDS_PER_WEEK         ((uint64_t) ((uint64_t)MICROSECONDS_PER_MILLISECOND * (uint64_t)MILLISECONDS_PER_WEEK)) /**< Number of microseconds per week. */
#define MICROSECONDS_PER_YEAR         ((uint64_t) ((uint64_t)MICROSECONDS_PER_MILLISECOND * MILLISECONDS_PER_YEAR))           /**< Number of microseconds per year. */
#define MILLISECONDS_PER_MONTH        ((uint32_t) ((uint32_t)MILLISECONDS_PER_SECOND * SECONDS_PER_MONTH))                    /**< Number of milliseconds per month. */
#define MILLISECONDS_PER_DECADE       ((uint64_t) ((uint64_t)MILLISECONDS_PER_SECOND * (uint64_t)SECONDS_PER_DECADE))         /**< Number of milliseconds per decade. */
#define SECONDS_PER_CENTURY           ((uint32_t) (SECONDS_PER_MINUTE * MINUTES_PER_CENTURY))                                 /**< Number of seconds per century. */
#define MINUTES_PER_MILLENIUM         ((uint32_t) ((uint32_t)MINUTES_PER_HOUR * HOURS_PER_MILLENIUM))                         /**< Number of minutes per millenium. */

#define MICROSECONDS_PER_MONTH        ((uint64_t) ((uint64_t)MICROSECONDS_PER_MILLISECOND * (uint64_t)MILLISECONDS_PER_MONTH))  /**< Number of microseconds per month. */
#define MICROSECONDS_PER_DECADE       ((uint64_t) ((uint64_t)MICROSECONDS_PER_MILLISECOND * MILLISECONDS_PER_DECADE))           /**< Number of microseconds per decade. */
#define MILLISECONDS_PER_CENTURY      ((uint64_t) ((uint64_t)MILLISECONDS_PER_SECOND * (uint64_t)SECONDS_PER_CENTURY))          /**< Number of milliseconds per century. */
#define SECONDS_PER_MILLENIUM         ((uint64_t) ((uint64_t)SECONDS_PER_MINUTE * (uint64_t)MINUTES_PER_MILLENIUM))             /**< Number of seconds per millenium. */

#define MICROSECONDS_PER_CENTURY      ((uint64_t) ((uint64_t)MICROSECONDS_PER_MILLISECOND * MILLISECONDS_PER_CENTURY))  /**< Number of microseconds per century. */
#define MILLISECONDS_PER_MILLENIUM    ((uint64_t) ((uint64_t)MILLISECONDS_PER_SECOND * SECONDS_PER_MILLENIUM))          /**< Number of milliseconds per millenium. */

#define MICROSECONDS_PER_MILLENIUM    ((uint64_t) ((uint64_t)MICROSECONDS_PER_MILLISECOND * MILLISECONDS_PER_MILLENIUM))  /**< Number of microseconds per millenium. */

#define MILLISECONDS_PER_MICROSECOND  ((float)MILLISECONDS_PER_MILLISECOND / (float)MICROSECONDS_PER_MILLISECOND) /**< Fraction of a millisecond per microsecond. */

#define SECONDS_PER_MICROSECOND       ((float)SECONDS_PER_SECOND / (float)MICROSECONDS_PER_SECOND)  /**< Fraction of a second per microsecond. */
#define SECONDS_PER_MILLISECOND       ((float)SECONDS_PER_SECOND / (float)MILLISECONDS_PER_SECOND)  /**< Fraction of a second per millisecond. */

#define MINUTES_PER_MICROSECOND       ((float)MINUTES_PER_MINUTE / (float)MICROSECONDS_PER_MINUTE)  /**< Fraction of a minute per microsecond. */
#define MINUTES_PER_MILLISECOND       ((float)MINUTES_PER_MINUTE / (float)MILLISECONDS_PER_MINUTE)  /**< Fraction of a minute per millisecond. */
#define MINUTES_PER_SECOND            ((float)MINUTES_PER_MINUTE / (float)SECONDS_PER_MINUTE)       /**< Fraction of a minute per second. */

#define HOURS_PER_MICROSECOND         ((float)HOURS_PER_HOUR / (float)MICROSECONDS_PER_HOUR)  /**< Fraction of an hour per microsecond. */
#define HOURS_PER_MILLISECOND         ((float)HOURS_PER_HOUR / (float)MILLISECONDS_PER_HOUR)  /**< Fraction of an hour per millisecond. */
#define HOURS_PER_SECOND              ((float)HOURS_PER_HOUR / (float)SECONDS_PER_HOUR)       /**< Fraction of an hour per second. */
#define HOURS_PER_MINUTE              ((float)HOURS_PER_HOUR / (float)MINUTES_PER_HOUR)       /**< Fraction of an hour per minute. */

#define DAYS_PER_MICROSECOND          ((float)DAYS_PER_DAY / (float)MICROSECONDS_PER_DAY) /**< Fraction of a day per microsecond. */
#define DAYS_PER_MILLISECOND          ((float)DAYS_PER_DAY / (float)MILLISECONDS_PER_DAY) /**< Fraction of a day per millisecond. */
#define DAYS_PER_SECOND               ((float)DAYS_PER_DAY / (float)SECONDS_PER_DAY)      /**< Fraction of a day per second. */
#define DAYS_PER_MINUTE               ((float)DAYS_PER_DAY / (float)MINUTES_PER_DAY)      /**< Fraction of a day per minute. */
#define DAYS_PER_HOUR                 ((float)DAYS_PER_DAY / (float)HOURS_PER_DAY)        /**< Fraction of a day per hour. */

#define WEEKS_PER_MICROSECOND         ((float)WEEKS_PER_WEEK / (float)MICROSECONDS_PER_WEEK)  /**< Fraction of a week per microsecond. */
#define WEEKS_PER_MILLISECOND         ((float)WEEKS_PER_WEEK / (float)MILLISECONDS_PER_WEEK)  /**< Fraction of a week per millisecond. */
#define WEEKS_PER_SECOND              ((float)WEEKS_PER_WEEK / (float)SECONDS_PER_WEEK)       /**< Fraction of a week per second. */
#define WEEKS_PER_MINUTE              ((float)WEEKS_PER_WEEK / (float)MINUTES_PER_WEEK)       /**< Fraction of a week per minute. */
#define WEEKS_PER_HOUR                ((float)WEEKS_PER_WEEK / (float)HOURS_PER_WEEK)         /**< Fraction of a week per hour. */
#define WEEKS_PER_DAY                 ((float)WEEKS_PER_WEEK / (float)DAYS_PER_WEEK)          /**< Fraction of a week per day. */

#define MONTHS_PER_MICROSECOND        ((float)MONTHS_PER_MONTH / (float)MICROSECONDS_PER_MONTH) /**< Fraction of a month per microsecond. */
#define MONTHS_PER_MILLISECOND        ((float)MONTHS_PER_MONTH / (float)MILLISECONDS_PER_MONTH) /**< Fraction of a month per millisecond. */
#define MONTHS_PER_SECOND             ((float)MONTHS_PER_MONTH / (float)SECONDS_PER_MONTH)      /**< Fraction of a month per second. */
#define MONTHS_PER_MINUTE             ((float)MONTHS_PER_MONTH / (float)MINUTES_PER_MONTH)      /**< Fraction of a month per minute. */
#define MONTHS_PER_HOUR               ((float)MONTHS_PER_MONTH / (float)HOURS_PER_MONTH)        /**< Fraction of a month per hour. */
#define MONTHS_PER_DAY                ((float)MONTHS_PER_MONTH / (float)DAYS_PER_MONTH)         /**< Fraction of a month per day. */
#define MONTHS_PER_WEEK               ((float)MONTHS_PER_MONTH / (float)WEEKS_PER_MONTH)        /**< Fraction of a month per week. */

#define YEARS_PER_MICROSECOND         ((float)YEARS_PER_YEAR / (float)MICROSECONDS_PER_YEAR)  /**< Fraction of a year per microsecond. */
#define YEARS_PER_MILLISECOND         ((float)YEARS_PER_YEAR / (float)MILLISECONDS_PER_YEAR)  /**< Fraction of a year per millisecond. */
#define YEARS_PER_SECOND              ((float)YEARS_PER_YEAR / (float)SECONDS_PER_YEAR)       /**< Fraction of a year per second. */
#define YEARS_PER_MINUTE              ((float)YEARS_PER_YEAR / (float)MINUTES_PER_YEAR)       /**< Fraction of a year per minute. */
#define YEARS_PER_HOUR                ((float)YEARS_PER_YEAR / (float)HOURS_PER_YEAR)         /**< Fraction of a year per hour. */
#define YEARS_PER_DAY                 ((float)YEARS_PER_YEAR / (float)DAYS_PER_YEAR)          /**< Fraction of a year per day. */
#define YEARS_PER_WEEK                ((float)YEARS_PER_YEAR / (float)WEEKS_PER_YEAR)         /**< Fraction of a year per week. */
#define YEARS_PER_MONTH               ((float)YEARS_PER_YEAR / (float)MONTHS_PER_YEAR)        /**< Fraction of a year per month. */

#define DECADES_PER_MICROSECOND       ((float)DECADES_PER_DECADE / (float)MICROSECONDS_PER_DECADE)  /**< Fraction of a decade per microsecond. */
#define DECADES_PER_MILLISECOND       ((float)DECADES_PER_DECADE / (float)MILLISECONDS_PER_DECADE)  /**< Fraction of a decade per millisecond. */
#define DECADES_PER_SECOND            ((float)DECADES_PER_DECADE / (float)SECONDS_PER_DECADE)       /**< Fraction of a decade per second. */
#define DECADES_PER_MINUTE            ((float)DECADES_PER_DECADE / (float)MINUTES_PER_DECADE)       /**< Fraction of a decade per minute. */
#define DECADES_PER_HOUR              ((float)DECADES_PER_DECADE / (float)HOURS_PER_DECADE)         /**< Fraction of a decade per hour. */
#define DECADES_PER_DAY               ((float)DECADES_PER_DECADE / (float)DAYS_PER_DECADE)          /**< Fraction of a decade per day. */
#define DECADES_PER_WEEK              ((float)DECADES_PER_DECADE / (float)WEEKS_PER_DECADE)         /**< Fraction of a decade per week. */
#define DECADES_PER_MONTH             ((float)DECADES_PER_DECADE / (float)MONTHS_PER_DECADE)        /**< Fraction of a decade per month. */
#define DECADES_PER_YEAR              ((float)DECADES_PER_DECADE / (float)YEARS_PER_DECADE)         /**< Fraction of a decade per year. */

#define CENTURIES_PER_MICROSECOND     ((float)CENTURIES_PER_CENTURY / (float)MICROSECONDS_PER_CENTURY)  /**< Fraction of a century per microsecond. */
#define CENTURIES_PER_MILLISECOND     ((float)CENTURIES_PER_CENTURY / (float)MILLISECONDS_PER_CENTURY)  /**< Fraction of a century per millisecond. */
#define CENTURIES_PER_SECOND          ((float)CENTURIES_PER_CENTURY / (float)SECONDS_PER_CENTURY)       /**< Fraction of a century per second. */
#define CENTURIES_PER_MINUTE          ((float)CENTURIES_PER_CENTURY / (float)MINUTES_PER_CENTURY)       /**< Fraction of a century per minute. */
#define CENTURIES_PER_HOUR            ((float)CENTURIES_PER_CENTURY / (float)HOURS_PER_CENTURY)         /**< Fraction of a century per hour. */
#define CENTURIES_PER_DAY             ((float)CENTURIES_PER_CENTURY / (float)DAYS_PER_CENTURY)          /**< Fraction of a century per day. */
#define CENTURIES_PER_WEEK            ((float)CENTURIES_PER_CENTURY / (float)WEEKS_PER_CENTURY)         /**< Fraction of a century per week. */
#define CENTURIES_PER_MONTH           ((float)CENTURIES_PER_CENTURY / (float)MONTHS_PER_CENTURY)        /**< Fraction of a century per month. */
#define CENTURIES_PER_YEAR            ((float)CENTURIES_PER_CENTURY / (float)YEARS_PER_CENTURY)         /**< Fraction of a century per year. */
#define CENTURIES_PER_DECADE          ((float)CENTURIES_PER_CENTURY / (float)DECADES_PER_CENTURY)       /**< Fraction of a century per decade. */

#define MILLENIA_PER_MICROSECOND      ((float)MILLENIA_PER_MILLENIUM / (float)MICROSECONDS_PER_MILLENIUM) /**< Fraction of a millenium per microsecond. */
#define MILLENIA_PER_MILLISECOND      ((float)MILLENIA_PER_MILLENIUM / (float)MILLISECONDS_PER_MILLENIUM) /**< Fraction of a millenium per millisecond. */
#define MILLENIA_PER_SECOND           ((float)MILLENIA_PER_MILLENIUM / (float)SECONDS_PER_MILLENIUM)      /**< Fraction of a millenium per second. */
#define MILLENIA_PER_MINUTE           ((float)MILLENIA_PER_MILLENIUM / (float)MINUTES_PER_MILLENIUM)      /**< Fraction of a millenium per minute. */
#define MILLENIA_PER_HOUR             ((float)MILLENIA_PER_MILLENIUM / (float)HOURS_PER_MILLENIUM)        /**< Fraction of a millenium per hour. */
#define MILLENIA_PER_DAY              ((float)MILLENIA_PER_MILLENIUM / (float)DAYS_PER_MILLENIUM)         /**< Fraction of a millenium per day. */
#define MILLENIA_PER_WEEK             ((float)MILLENIA_PER_MILLENIUM / (float)WEEKS_PER_MILLENIUM)        /**< Fraction of a millenium per week. */
#define MILLENIA_PER_MONTH            ((float)MILLENIA_PER_MILLENIUM / (float)MONTHS_PER_MILLENIUM)       /**< Fraction of a millenium per month. */
#define MILLENIA_PER_YEAR             ((float)MILLENIA_PER_MILLENIUM / (float)YEARS_PER_MILLENIUM)        /**< Fraction of a millenium per year. */
#define MILLENIA_PER_DECADE           ((float)MILLENIA_PER_MILLENIUM / (float)DECADES_PER_MILLENIUM)      /**< Fraction of a millenium per decade. */
#define MILLENIA_PER_CENTURY          ((float)MILLENIA_PER_MILLENIUM / (float)CENTURIES_PER_MILLENIUM)    /**< Fraction of a millenium per century. */

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
 * @brief   Generic time to represent long time frames at high precision.
 * @note    By definition the temporal resolution is 1us.
 */
typedef struct aos_timestamp {
  /**
   * @brief   Internal time representation.
   */
  uint64_t time;
} aos_timestamp_t;

/**
 * @brief   generic time to represent medium length time frames at high
 *          precision.
 * @note    By definition the temporal resolution is 1us.
 */
typedef uint32_t aos_interval_t;

/**
 * @brief   Generic time to represent long time frames at high precision.
 * @note    By definition the temporal resolution is 1us.
 */
typedef uint64_t aos_longinterval_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/**
 * @brief   Declare a timestamp with an initial value.
 *
 * @param[in, out] var  Name of the instance to declare.
 * @param[in]      val  Initial value to set.
 */
#define aosTimestampDecl(var, val)    aos_timestamp_t var = {.time = val}

/**
 * @brief   Set a timestamp value.
 *
 * @param[in, out] t    Timestamp to be modified.
 * @param[in]      val  Value to set.
 */
#define aosTimestampSet(t, val)       ((t).time = (val))

/**
 * @brief   Get the value of a timestamp.
 *
 * @param[in] t   Timestamp to retrieve the value from.
 *
 * @return  Copy of the internal timestamp value.
 */
#define aosTimestampGet(t)            (t).time

/**
 * @brief   Add a timestamp to another timestamp.
 *
 * @param[in, out] lhs  Timestamp to be modified.
 * @param[in]      rhs  Timestamp to be added.
 */
#define aosTimestampAdd(lhs, rhs)     ((lhs).time += (rhs).time)

/**
 * @brief   Add an interval to a timestamp.
 *
 * @param[in, out] lhs  Timestamp to be modified.
 * @param[in]      rhs  Interval value to be added.
 */
#define aosTimestampAddInterval(lhs, rhs) ((lhs).time += (rhs))

/**
 * @brief   Add a long interval to a timestamp.
 *
 * @param[in, out] lhs  Timestamp to be modified.
 * @param[in]      rhs  Long interval value to be added.
 */
#define aosTimestampAddLongInterval(lhs, rhs) ((lhs).time += (rhs))

/**
 * @brief   Calculate the difference between two timestamps.
 * @details In case the value of later timestamp is lower than the value of the
 *          earlier timestamp, a difference of 0 is returned.
 *
 * @param[in] from  Earlier timestamp.
 * @param[in] to    Later timestamp.
 *
 * @return  Temporal difference between timestamps.
 */
#define aosTimestampDiff(from, to)    (((to).time > (from).time) ? ((to).time - (from).time) : 0)

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
  uint8_t aosTimeDayOfWeekFromDate(const uint16_t day, const uint8_t month, const uint16_t year);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* AMIROOS_TIME_H */

/** @} */
