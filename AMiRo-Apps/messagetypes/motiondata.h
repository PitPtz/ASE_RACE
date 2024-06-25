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
 * @file    motiondata.h
 * @brief   Data type describing arbitrary DOF (degrees of freedom) motion.
 *
 * @defgroup msgtypes_motion Motion
 * @ingroup	msgtypes
 * @brief   todo
 * @details todo
 *
 * @addtogroup msgtypes_motion
 * @{
 */

#ifndef MOTIONDATA_H
#define MOTIONDATA_H

#include <stdint.h>

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

/******************************************************************************/
/* SETTINGS                                                                   */
/******************************************************************************/

#if !defined(MOTIONDATA_AXES) || defined(__DOXYGEN__)
/**
 * @brief Number of axes that are represented by motion data.
 * @note  Must be greater than 0.
 */
#define MOTIONDATA_AXES                           3
#endif

/******************************************************************************/
/* CHECKS                                                                     */
/******************************************************************************/

#if (MOTIONDATA_AXES <= 0)
# error "MOTIONDATA_AXES must be defined to a value greater than 0."
#endif

/******************************************************************************/
/* DATA STRUCTURES AND TYPES                                                  */
/******************************************************************************/

/* translation ****************************************************************/

/**
 * @brief   Translation along Euler axes using µ-representation.
 *
 * @struct translation_euler_u
 */
typedef struct translation_euler_u {
  /**
   * @brief   Translation along each Euler axis.
   * @details By definition, values represent µm/s.
   */
  int32_t axes[MOTIONDATA_AXES];
} translation_euler_u;

/**
 * @brief   Translation along Euler axes using SI-representation.
 *
 * @struct translation_euler_si
 */
typedef struct translation_euler_si {
  /**
   * @brief   Translation along each Euler axis.
   * @details By definition, values represent m/s.
   */
  float axes[MOTIONDATA_AXES];
} translation_euler_si;

/**
 * @brief   Translation in polar coordinates using µ-representation.
 *
 * @struct translation_polar_u
 */
typedef struct translation_polar_u {
  /**
   * @brief   Radial distance.
   * @details By definition, the value represents µm/s.
   */
  uint32_t radial_distance;

#if (MOTIONDATA_AXES > 1) || defined (__DOXYGEN__)
  /**
   * @brief   Angles, defining the direction.
   * @details In 3D space, the first angle typically describes inclination the
   *          the second angle destribes azimuth. However, differnt concentions
   *          may be used as well.
   *          By definition values represent µrad/s.
   */
  uint32_t angles[MOTIONDATA_AXES - 1];
#endif
} translation_polar_u;

/**
 * @brief   Translation in polar coordinates using SI-representation.
 *
 * @struct translation_polar_si
 */
typedef struct translation_polar_si {
  /**
   * @brief   Radial distance.
   * @details By definition, the value represents m/s.
   */
  float radial_distance;

#if (MOTIONDATA_AXES > 1) || defined (__DOXYGEN__)
  /**
   * @brief   Angles, defining the direction.
   * @details In 3D space, the first angle typically describes inclination the
   *          the second angle destribes azimuth. However, differnt concentions
   *          may be used as well.
   *          By definition values represent rad/s.
   */
  float angles[MOTIONDATA_AXES - 1];
#endif
} translation_polar_si;

/* rotation *******************************************************************/

/**
 * @brief   Rotation about a vector using µ-representation.
 *
 * @struct rotation_vector_u
 */
typedef struct rotation_vector_u {
  /**
   * @brief   Unit vector describing the rotation axis.
   * @details Each integer value maps linearly to range [0,1].
   */
  uint32_t vector[MOTIONDATA_AXES];

  /**
   * @brief   Angle to rotate about.
   * @details By definition, the value represents µrad/s.
   */
  int32_t angle;
} rotation_vector_u;

/**
 * @brief   Rotation about a vector using SI-representation.
 *
 * @struct rotation_vector_si
 */
typedef struct rotation_vector_si {
  /**
   * @brief   Unit vector describing the rotation axis.
   */
  float vector[MOTIONDATA_AXES];

  /**
   * @brief   Angle to rotate about.
   * @details By definition, the value represents rad/s.
   */
  float angle;
} rotation_vector_si;

/**
 * @brief   Rotation about a vector using compact SI-representation.
 *
 * @struct rotation_vector_csi
 */
typedef struct rotation_vector_csi {
  /**
   * @brief   Unit vector describing the rotation axis and rotation angle (in
   *          rad/s) by its length.
   */
  float vector[MOTIONDATA_AXES];
} rotation_vector_csi;

/**
 * @brief   Euler rotation using µ-representation.
 *
 * @struct rotation_euler_u
 */
typedef struct rotation_euler_u {
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
     * @details By definition, the value represents µrad/s.
     */
    int32_t angle;
  } rotation[MOTIONDATA_AXES];
} rotation_euler_u;

/**
 * @brief   Euler rotation using SI-representation.
 *
 * @struct rotation_euler_si
 */
typedef struct rotation_euler_si {
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
     * @details By definition, the value represents rad/s.
     */
    float angle;
  } rotation[MOTIONDATA_AXES];
} rotation_euler_si;

/**
 * @brief   Quaternion rotation using integer representation.
 *
 * @struct rotation_quaternion_i
 */
typedef struct rotation_quaternion_i {
  /**
   * @brief   Vector part of the unit quaternion.
   * @details Each integer value maps linearly to range [-1,1].
   */
  int32_t vector[MOTIONDATA_AXES];

  /**
   * @brief   Scalar part of the unit quaternion.
   * @details The value maps linearly to range [-1,1].
   */
  int32_t scalar;
} rotation_quaternion_i;

/**
 * @brief   Quaternion rotation using floating point representation.
 *
 * @struct rotation_quaternion_f
 */
typedef struct rotation_quaternion_f {
  /**
   * @brief   Vector part of the unit quaternion.
   */
  float vector[MOTIONDATA_AXES];

  /**
   * @brief   Scalar part of the unit quaternion.
   */
  float scalar;
} rotation_quaternion_f;

/**
 * @brief   Quaternion rotation using compact floating point representation.
 *
 * @struct rotation_quaternion_cf
 */
typedef struct rotation_quaternion_cf {
  /**
   * @brief   Vector part of the quaternion.
   * @details The scalar part is encoded by the length of the vector part.
   */
  float vector[MOTIONDATA_AXES];
} rotation_quaternion_cf;

/* motion *********************************************************************/

/**
 * @brief   Motion via Euler translation and vector rotation using
 *          µ-representation.
 *
 * @struct motion_ev_u
 */
typedef struct motion_ev_u
{
  /**
   * @brief   Translation along Euler axes.
   */
  translation_euler_u translation;

  /**
   * @brief   Rotation about a vector.
   */
  rotation_vector_u rotation;
} motion_ev_u;

/**
 * @brief   Motion via Euler translation and vector rotation using
 *          SI-representation.
 *
 * @struct motion_ev_si
 */
typedef struct motion_ev_si
{
  /**
   * @brief   Translation along Euler axes.
   */
  translation_euler_si translation;

  /**
   * @brief   Rotation about a vector.
   */
  rotation_vector_si rotation;
} motion_ev_si;

/**
 * @brief   Motion via Euler translation and vector rotation using
 *          compact SI-representation.
 *
 * @struct motion_ev_csi
 */
typedef struct motion_ev_csi
{
  /**
   * @brief   Translation along Euler axes.
   */
  translation_euler_si translation;

  /**
   * @brief   Rotation about a vector.
   */
  rotation_vector_csi rotation;
} motion_ev_csi;

/**
 * @brief   Motion via Euler translation and Euler rotation using
 *          µ-representation.
 *
 * @struct motion_ee_u
 */
typedef struct motion_ee_u
{
  /**
   * @brief   Translation along Euler axes.
   */
  translation_euler_u translation;

  /**
   * @brief   Euler rotation.
   */
  rotation_euler_u rotation;
} motion_ee_u;

/**
 * @brief   Motion via Euler translation and Euler rotation using
 *          SI-representation.
 *
 * @struct motion_ee_si
 */
typedef struct motion_ee_si
{
  /**
   * @brief   Translation along Euler axes.
   */
  translation_euler_si translation;

  /**
   * @brief   Euler rotation.
   */
  rotation_euler_si rotation;
} motion_ee_si;

/**
 * @brief   Motion via Euler translation and quaternion rotation using
 *          µ-/integer representation.
 *
 * @struct motion_eq_u
 */
typedef struct motion_eq_u
{
  /**
   * @brief   Translation along Euler axes.
   */
  translation_euler_u translation;

  /**
   * @brief   Quaternion rotation.
   */
  rotation_quaternion_i rotation;
} motion_eq_u;

/**
 * @brief   Motion via Euler translation and quaternion rotation using
 *          SI-/floating point representation.
 *
 * @struct motion_eq_si
 */
typedef struct motion_eq_si
{
  /**
   * @brief   Translation along Euler axes.
   */
  translation_euler_si translation;

  /**
   * @brief   Quaternion rotation.
   */
  rotation_quaternion_f rotation;
} motion_eq_si;

/**
 * @brief   Motion via Euler translation and quaternion rotation using
 *          SI-/compact floating point representation.
 *
 * @struct motion_eq_csi
 */
typedef struct motion_eq_csi
{
  /**
   * @brief   Translation along Euler axes.
   */
  translation_euler_si translation;

  /**
   * @brief   Quaternion rotation.
   */
  rotation_quaternion_cf rotation;
} motion_eq_csi;

/**
 * @brief   Motion via polar translation and vector rotation using
 *          µ-representation.
 *
 * @struct motion_pv_u
 */
typedef struct motion_pv_u
{
  /**
   * @brief   Translation in polar coordinates.
   */
  translation_polar_u translation;

  /**
   * @brief   Rotation about a vector.
   */
  rotation_vector_u rotation;
} motion_pv_u;

/**
 * @brief   Motion via polar translation and vector rotation using
 *          SI-representation.
 *
 * @struct motion_pv_si
 */
typedef struct motion_pv_si
{
  /**
   * @brief   Translation in polar coordinates.
   */
  translation_polar_si translation;

  /**
   * @brief   Rotation about a vector.
   */
  rotation_vector_si rotation;
} motion_pv_si;

/**
 * @brief   Motion via polar translation and vector rotation using
 *          compact SI-representation.
 *
 * @struct motion_pv_csi
 */
typedef struct motion_pv_csi
{
  /**
   * @brief   Translation in polar coordinates.
   */
  translation_polar_si translation;

  /**
   * @brief   Rotation about a vector.
   */
  rotation_vector_csi rotation;
} motion_pv_csi;

/**
 * @brief   Motion via polar translation and Euler rotation using
 *          µ-representation.
 *
 * @struct motion_pe_u
 */
typedef struct motion_pe_u
{
  /**
   * @brief   Translation in polar coordinates.
   */
  translation_polar_u translation;

  /**
   * @brief   Euler rotation.
   */
  rotation_euler_u rotation;
} motion_pe_u;

/**
 * @brief   Motion via polar translation and Euler rotation using
 *          SI-representation.
 *
 * @struct motion_pe_si
 */
typedef struct motion_pe_si
{
  /**
   * @brief   Translation in polar coordinates.
   */
  translation_polar_si translation;

  /**
   * @brief   Euler rotation.
   */
  rotation_euler_si rotation;
} motion_pe_si;

/**
 * @brief   Motion via polar translation and quaternion rotation using
 *          µ-/integer representation.
 *
 * @struct motion_pq_u
 */
typedef struct motion_pq_u
{
  /**
   * @brief   Translation in polar coordinates.
   */
  translation_polar_u translation;

  /**
   * @brief   Quaternion rotation.
   */
  rotation_quaternion_i rotation;
} motion_pq_u;

/**
 * @brief   Motion via polar translation and quaternion rotation using
 *          SI-/floating point representation.
 *
 * @struct motion_pq_si
 */
typedef struct motion_pq_si
{
  /**
   * @brief   Translation in polar coordinates.
   */
  translation_polar_si translation;

  /**
   * @brief   Quaternion rotation.
   */
  rotation_quaternion_f rotation;
} motion_pq_si;

/**
 * @brief   Motion via polar translation and quaternion rotation using
 *          SI-/compact floating point representation.
 *
 * @struct motion_pq_csi
 */
typedef struct motion_pq_csi
{
  /**
   * @brief   Translation in polar coordinates.
   */
  translation_polar_si translation;

  /**
   * @brief   Quaternion rotation.
   */
  rotation_quaternion_cf rotation;
} motion_pq_csi;

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

#endif /* MOTIONDATA_H */

/** @} */
