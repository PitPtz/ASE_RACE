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
 * @file    urtbenchmarkconf.h
 * @brief   µRT benchmark configuration.
 *
 * @addtogroup configs_urtbenchmark_modules_nucleog071rb
 * @{
 */

#ifndef URT_BENCHMARK_CONF_H
#define URT_BENCHMARK_CONF_H


/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

// core configuration
#define URTBENCHMARK_CFG_NUMNODES                 25
#define URTBENCHMARK_CFG_REPETITIONS              100
#define URTBENCHMARK_CFG_FREQUENCY                100.0f

// publish-subscribe & RPC common configuration
#define URTBENCHMARK_CFG_NUMHOPS                  25
#define URTBENCHMARK_CFG_MAXPAYLOAD               1024
#define URTBENCHMARK_CFG_QOSSTEPS                 10
#define URTBENCHMARK_CFG_QOSTIME                  (URTBENCHMARK_CFG_QOSSTEPS * MICROSECONDS_PER_MILLISECOND)

// publish-subscribe configuration
#define URTBENCHMARK_CFG_NUMPUBLISHERS            25
#define URTBENCHMARK_CFG_NUMSUBSCRIBERS           25
#define URTBENCHMARK_CFG_NUMTOPICS                25
#define URTBENCHMARK_CFG_NUMMESSAGES              25

// RPC configuration
#define URTBENCHMARK_CFG_NUMREQUESTS              25
#define URTBENCHMARK_CFG_NUMSERVICES              25

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

#endif /* URT_BENCHMARK_CONF_H */

/** @} */
