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
 * @file    LightRing_leddata.h
 * @brief   Data type describing the payload of all LEDs.
 *
 * @defgroup msgtypes_led LED
 * @ingroup	msgtypes
 * @brief   todo
 * @details todo
 *
 * @addtogroup msgtypes_led
 * @{
 */

#ifndef LIGHTRING_LEDDATA_H
#define LIGHTRING_LEDDATA_H
#include <stdint.h>
/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

static const uint8_t LIGHT_COLORS[56][3] = { {  0,   0,   0},
                                             {255,   0,   0},
                                             {  0, 255,   0},
                                             {  0,   0, 255},
                                             {255, 255,   0},
                                             {  0, 255, 255},
                                             {255,   0, 255},
                                             {127,   0, 255},
                                             {255, 127,   0},
                                             // Green -> Yellow -> Red gradient count: 47
                                             {  0, 255, 0},
                                             { 11, 255, 0},
                                             { 22, 255, 0},
                                             { 33, 255, 0},
                                             { 44, 255, 0},
                                             { 55, 255, 0},
                                             { 67, 255, 0},
                                             { 78, 255, 0},
                                             { 89, 255, 0},
                                             {100, 255, 0},
                                             {111, 255, 0},
                                             {122, 255, 0},
                                             {133, 255, 0},
                                             {144, 255, 0},
                                             {155, 255, 0},
                                             {166, 255, 0},
                                             {177, 255, 0},
                                             {188, 255, 0},
                                             {200, 255, 0},
                                             {211, 255, 0},
                                             {222, 255, 0},
                                             {233, 255, 0},
                                             {244, 255, 0},
                                             {255, 255, 0},
                                             {255, 244, 0},
                                             {255, 233, 0},
                                             {255, 222, 0},
                                             {255, 211, 0},
                                             {255, 200, 0},
                                             {255, 188, 0},
                                             {255, 177, 0},
                                             {255, 166, 0},
                                             {255, 155, 0},
                                             {255, 144, 0},
                                             {255, 133, 0},
                                             {255, 122, 0},
                                             {255, 111, 0},
                                             {255, 100, 0},
                                             {255,  89, 0},
                                             {255,  78, 0},
                                             {255,  67, 0},
                                             {255,  55, 0},
                                             {255,  44, 0},
                                             {255,  33, 0},
                                             {255,  22, 0},
                                             {255,  11, 0},
                                             {255,   0, 0}};

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

typedef enum {
  OFF,
  RED,
  GREEN,
  BLUE,
  YELLOW,
  TURQUOISE,
  MAGENTA,
  PURPLE,
  ORANGE
} color_t;

/**
 * @brief   Structure to represent the light data
 */
typedef struct light_led_data {
    color_t color[24]; // 24 lEDS because of the BlackMiRo
}light_led_data_t;

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* EXTERN DECLARATIONS                                                        */
/******************************************************************************/

/******************************************************************************/
/* INLINE FUNCTIONS                                                           */
/******************************************************************************/

/******************************************************************************/
/* SUBSYSTEMS                                                                 */
/******************************************************************************/

#endif /* LIGHTRING_LEDDATA_H */

/** @} */
