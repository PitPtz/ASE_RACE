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
 * @file    positiondata.h
 * @brief   Data type describing arbitrary DOF (degrees of freedom) positions.
 *
 * @defgroup msgtypes_position Position
 * @ingroup	msgtypes
 * @brief   todo
 * @details todo
 *
 * @addtogroup msgtypes_position
 * @{
 */

#ifndef POSITIONDATA_H
#define POSITIONDATA_H

#include <stdint.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

#if !defined(POSITIONDATA_AXES) || defined(__DOXYGEN__)
/**
 * @brief Number of axes that are represented by position data.
 * @note  Must be greater than 0.
 */
#define POSITIONDATA_AXES                         3
#endif

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

#if (POSITIONDATA_AXES <= 0)
# error "POSITIONDATA_AXES must be defined to a value greater than 0."
#endif

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

/* location *******************************************************************/

/**
 * @brief   Location as cartesian coordinates using µ-representation.
 *
 * @struct location_cartesian_u
 */
typedef struct location_cartesian_u {
  /**
   * @brief   Location in cartesian space.
   * @details By definition, values represent µm.
   */
  int32_t axes[POSITIONDATA_AXES];
} location_cartesian_u;

/**
 * @brief   Location as cartesian coordinates using SI-representation.
 *
 * @struct location_cartesian_si
 */
typedef struct location_cartesian_si {
  /**
   * @brief   Location in cartesian space.
   * @details By definition, values represent m.
   */
  float axes[POSITIONDATA_AXES];
} location_cartesian_si;

/**
 * @brief   Location as polar coordinates using µ-representation.
 *
 * @struct location_polar_u
 */
typedef struct location_polar_u {
  /**
   * @brief   Radial distance.
   * @details By definition, the value represents µm.
   */
  uint32_t radial_distance;

#if (POSITIONDATA_AXES > 1) || defined (__DOXYGEN__)
  /**
   * @brief   Angles, defining the direction.
   * @details In 3D space, the first angle typically describes inclination the
   *          the second angle destribes azimuth. However, differnt concentions
   *          may be used as well.
   *          By definition values represent µrad.
   */
  uint32_t angles[POSITIONDATA_AXES - 1];
#endif
} location_polar_u;

/**
 * @brief   Location as polar coordinates using SI-representation.
 *
 * @struct location_polar_si
 */
typedef struct location_polar_si {
  /**
   * @brief   Radial distance.
   * @details By definition, the value represents m.
   */
  float radial_distance;

#if (POSITIONDATA_AXES > 1) || defined (__DOXYGEN__)
  /**
   * @brief   Angles, defining the direction.
   * @details In 3D space, the first angle typically describes inclination the
   *          the second angle destribes azimuth. However, differnt concentions
   *          may be used as well.
   *          By definition values represent rad.
   */
  float angles[POSITIONDATA_AXES - 1];
#endif
} location_polar_si;

/* orientation ****************************************************************/

/**
 * @brief   Orientation about a vector using µ-representation.
 *
 * @struct orientation_vector_u
 */
typedef struct orientation_vector_u {
  /**
   * @brief   Unit vector describing the rotation axis.
   * @details Each integer value maps linearly to range [0,1].
   */
  uint32_t vector[POSITIONDATA_AXES];

  /**
   * @brief   Angle to rotate about.
   * @details By definition, the value represents µrad.
   */
  int32_t angle;
} orientation_vector_u;

/**
 * @brief   Orientation about a vector using SI-representation.
 *
 * @struct orientation_vector_si
 */
typedef struct orientation_vector_si {
  /**
   * @brief   Unit vector describing the rotation axis.
   */
  float vector[POSITIONDATA_AXES];

  /**
   * @brief   Angle to rotate about.
   * @details By definition, the value represents rad.
   */
  float angle;
} orientation_vector_si;

/**
 * @brief   Orientation about a vector using compact SI-representation.
 *
 * @struct orientation_vector_csi
 */
typedef struct orientation_vector_csi {
  /**
   * @brief   Unit vector describing the rotation axis and rotation angle (in
   *          rad) by its length.
   */
  float vector[POSITIONDATA_AXES];
} orientation_vector_csi;

/**
 * @brief   Orientation about a vector using compact SI-representation.
 *
 * @struct orientation_vector_csi
 */
typedef struct orientation_angle_csi {
  /**
   * @brief   Unit angle describing the rotation around position axis (in
   *          rad) by its length.
   */
  float angle;
} orientation_angle_csi;


/**
 * @brief   Orientation using Euler rotation and µ-representation.
 *
 * @struct orientation_euler_u
 */
typedef struct orientation_euler_u {
  /**
   * @brief   Array specifying subsequent rotation about euler axes.
   */
  struct {
    /**
     * @brief   Identifyier of the axis to rotate about.
     * @details Typically 1 refers to X-axis, 2 to Y-axis and 3 to Z-axis.
     *          However, different conventions may be used as well.
     */
    uint8_t axis;

    /**
     * @brief   Angle to rotate about.
     * @details By definition, the value represents µrad.
     */
    int32_t angle;
  } rotation[POSITIONDATA_AXES];
} orientation_euler_u;

/**
 * @brief   Orientation using Euler rotation and SI-representation.
 *
 * @struct orientation_euler_si
 */
typedef struct orientation_euler_si {
  /**
   * @brief   Array specifying subsequent rotation about euler axes.
   */
  struct {
    /**
     * @brief   Identifyier of the axis to rotate about.
     * @details Typically 1 refers to X-axis, 2 to Y-axis and 3 to Z-axis.
     *          However, different conventions may be used as well.
     */
    uint8_t axis;

    /**
     * @brief   Angle to rotate about.
     * @details By definition, the value represents rad.
     */
    float angle;
  } rotation[POSITIONDATA_AXES];
} orientation_euler_si;

/**
 * @brief   Orientation using Quaternion and integer representation.
 *
 * @struct orientation_quaternion_i
 */
typedef struct orientation_quaternion_i {
  /**
   * @brief   Vector part of the unit quaternion.
   * @details Each integer value maps linearly to range [-1,1].
   */
  int32_t vector[POSITIONDATA_AXES];

  /**
   * @brief   Scalar part of the unit quaternion.
   * @details The value maps linearly to range [-1,1].
   */
  int32_t scalar;
} orientation_quaternion_i;

/**
 * @brief   Orientation using Quaternion and floating point representation.
 *
 * @struct orientation_quaternion_f
 */
typedef struct orientation_quaternion_f {
  /**
   * @brief   Vector part of the unit quaternion.
   */
  float vector[POSITIONDATA_AXES];

  /**
   * @brief   Scalar part of the unit quaternion.
   */
  float scalar;
} orientation_quaternion_f;

/**
 * @brief   Orientation using Quaternion and compact floating point
 *          representation.
 *
 * @struct orientation_quaternion_cf
 */
typedef struct orientation_quaternion_cf {
  /**
   * @brief   Vector part of the quaternion.
   * @details The scalar part is encoded by the length of the vector part.
   */
  float vector[POSITIONDATA_AXES];
} orientation_quaternion_cf;

/* position *******************************************************************/

/**
 * @brief   Position in cartesian cordinates and vector orientation using
 *          µ-representation.
 *
 * @struct position_cv_u
 */
typedef struct position_cv_u
{
  /**
   * @brief   Location in cartesian coordinates.
   */
  location_cartesian_u location;

  /**
   * @brief   Orientation about a vector.
   */
  orientation_vector_u oriantation;
} position_cv_u;

/**
 * @brief   Position in cartesian cordinates and vector orientation using
 *          SI-representation.
 *
 * @struct position_cv_si
 */
typedef struct position_cv_si
{
  /**
   * @brief   Location in cartesian coordinates.
   */
  location_cartesian_si location;

  /**
   * @brief   Orientation about a vector.
   */
  orientation_vector_si oriantation;
} position_cv_si;

/**
 * @brief   Position in cartesian cordinates and vector orientation using
 *          compact SI-representation.
 *
 * @struct position_cv_csi
 */
typedef struct position_cv_csi
{
  /**
   * @brief   Location in cartesian coordinates.
   */
  location_cartesian_si location;

  /**
   * @brief   Orientation about a vector.
   */
  orientation_vector_csi oriantation;
} position_cv_csi;

/**
 * @brief   Position in cartesian cordinates and vector orientation using
 *          compact SI-representation.
 *
 * @struct position_cv_csi
 */
typedef struct position_cv_ccsi
{
  /**
   * @brief   Location in cartesian coordinates.
   */
  location_cartesian_si location;

  /**
   * @brief   Orientation angle around location axis.
   */
  orientation_angle_csi orientation;
} position_cv_ccsi;

/**
 * @brief   Position in cartesian cordinates and Euler orientation using
 *          µ-representation.
 *
 * @struct position_ce_u
 */
typedef struct position_ce_u
{
  /**
   * @brief   Location in cartesian coordinates.
   */
  location_cartesian_u location;

  /**
   * @brief   Euler orientation.
   */
  orientation_euler_u oriantation;
} position_ce_u;

/**
 * @brief   Position in cartesian cordinates and Euler orientation using
 *          SI-representation.
 *
 * @struct position_ce_si
 */
typedef struct position_ce_si
{
  /**
   * @brief   Location in cartesian coordinates.
   */
  location_cartesian_si location;

  /**
   * @brief   Euler orientation.
   */
  orientation_euler_si oriantation;
} position_ce_si;

/**
 * @brief   Position in cartesian cordinates and quaternion orientation using
 *          µ-/integer representation.
 *
 * @struct position_cq_u
 */
typedef struct position_cq_u
{
  /**
   * @brief   Location in cartesian coordinates.
   */
  location_cartesian_u location;

  /**
   * @brief   Quaternion orientation.
   */
  orientation_quaternion_i oriantation;
} position_cq_u;

/**
 * @brief   Position in cartesian cordinates and quaternion orientation using
 *          SI-/floating point representation.
 *
 * @struct position_cq_si
 */
typedef struct position_cq_si
{
  /**
   * @brief   Location in cartesian coordinates.
   */
  location_cartesian_si location;

  /**
   * @brief   Quaternion orientation.
   */
  orientation_quaternion_f oriantation;
} position_cq_si;

/**
 * @brief   Position in cartesian cordinates and quaternion orientation using
 *          SI-/compact floating point representation.
 *
 * @struct position_cq_csi
 */
typedef struct position_cq_csi
{
  /**
   * @brief   Location in cartesian coordinates.
   */
  location_cartesian_si location;

  /**
   * @brief   Quaternion orientation.
   */
  orientation_quaternion_cf oriantation;
} position_cq_csi;

/**
 * @brief   Position in polar cordinates and vector orientation using
 *          µ-representation.
 *
 * @struct position_pv_u
 */
typedef struct position_pv_u
{
  /**
   * @brief   Location in polar coordinates.
   */
  location_polar_u location;

  /**
   * @brief   Orientation about a vector.
   */
  orientation_vector_u oriantation;
} position_pv_u;

/**
 * @brief   Position in polar coordinates and vector orientation using
 *          SI-representation.
 *
 * @struct position_pv_si
 */
typedef struct position_pv_si
{
  /**
   * @brief   Location in polar coordinates.
   */
  location_polar_si location;

  /**
   * @brief   Orientation about a vector.
   */
  orientation_vector_si oriantation;
} position_pv_si;

/**
 * @brief   Position in polar coordinates and vector orientation using
 *          compact SI-representation.
 *
 * @struct position_pv_csi
 */
typedef struct position_pv_csi
{
  /**
   * @brief   Location in polar coordinates.
   */
  location_polar_si location;

  /**
   * @brief   Orientation about a vector.
   */
  orientation_vector_csi oriantation;
} position_pv_csi;

/**
 * @brief   Position in polar coordinates and Euler orientation using
 *          µ-representation.
 *
 * @struct position_pe_u
 */
typedef struct position_pe_u
{
  /**
   * @brief   Location in polar coordinates.
   */
  location_polar_u location;

  /**
   * @brief   Euler orientation.
   */
  orientation_euler_u oriantation;
} position_pe_u;

/**
 * @brief   Position in polar coordinates and Euler orientation using
 *          SI-representation.
 *
 * @struct position_pe_si
 */
typedef struct position_pe_si
{
  /**
   * @brief   Location in polar coordinates.
   */
  location_polar_si location;

  /**
   * @brief   Euler orientation.
   */
  orientation_euler_si oriantation;
} position_pe_si;

/**
 * @brief   Position in polar coordinates and quaternion orientation using
 *          µ-/integer representation.
 *
 * @struct position_pq_u
 */
typedef struct position_pq_u
{
  /**
   * @brief   Location in polar coordinates.
   */
  location_polar_u location;

  /**
   * @brief   Quaternion orientation.
   */
  orientation_quaternion_i oriantation;
} position_pq_u;

/**
 * @brief   Position in polar coordinates and quaternion orientation using
 *          SI-/floating point representation.
 *
 * @struct position_pq_si
 */
typedef struct position_pq_si
{
  /**
   * @brief   Location in polar coordinates.
   */
  location_polar_si location;

  /**
   * @brief   Quaternion orientation.
   */
  orientation_quaternion_f oriantation;
} position_pq_si;

/**
 * @brief   Position in polar coordinates and quaternion orientation using
 *          SI-/compact floating point representation.
 *
 * @struct position_pq_csi
 */
typedef struct position_pq_csi
{
  /**
   * @brief   Location in polar coordinates.
   */
  location_polar_si location;

  /**
   * @brief   Quaternion orientation.
   */
  orientation_quaternion_cf oriantation;
} position_pq_csi;

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

#endif /* POSITIONDATA_H */

/** @} */
