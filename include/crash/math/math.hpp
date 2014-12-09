#pragma once

#include <glm/glm.hpp>

namespace crash {
namespace math {

/**
 * Generate a rotation vector from a given axis and rotation angle.
 *
 * :param axis:   The axis of rotation.
 * :param angle:  The amount of rotation, in radians.
 * :return:       The generated rotation vector.
 */
glm::vec4 rotation(const glm::vec3& axis, float angle);

/**
 * Generate a rotation vector from a given axis and rotation angle.
 *
 * :param axis:   The axis of rotation.
 * :param angle:  The amount of rotation, in radians.
 * :return:       The generated rotation vector.
 */
glm::vec4 rotation(const glm::vec4& axis, float angle);

/**
 * Generate the transformation matrix for the given position, orientation,
 * and size of an object.
 *
 * :param position:     The absolute global position vector of the object.
 * :param orientation:  The absolute rotation vector of the object.
 * :param size:         The absolute size vector of the object.
 * :return:             The generated transform matrix.
 */
glm::mat4 transform(const glm::vec3& position, const glm::vec4& orientation,
 const glm::vec3& size);

/**
 * Generate the transformation matrix for the given position and orientation
 * of an object.
 *
 * :param position:     The absolute global position vector of the object.
 * :param orientation:  The absolute rotation vector of the object.
 * :return:             The generated transform matrix.
 */
glm::mat4 transform(const glm::vec3& position, const glm::vec4& orientation);

/**
 * Compute the average between two numeric parameters.
 *
 * :param a:   The first number to use in the average calculation.
 * :param b:   The second number to use in the average calculation.
 * :return:    The average of the passed parameters.
 */
template < class T >
T average(T a, T b);

/**
 * Determine if two arguments have approximately the same value.
 *
 * :param a:   The first argument to compare.
 * :param b:   The second argument to compare.
 */
template < typename T >
bool approxEqual(const T& a, const T& b);
template <>
inline bool approxEqual<glm::vec3>(const glm::vec3& a, const glm::vec3& b);
template <>
inline bool approxEqual<glm::vec4>(const glm::vec4& a, const glm::vec4& b);
template <>
inline bool approxEqual<glm::mat4>(const glm::mat4& a, const glm::mat4& b);

/**
 * Determine if the first argument is approximately less than the second.
 *
 * :param a:   The first argument to compare.
 * :param b:   The second argument to compare.
 */
template < class T >
bool approxLessThan(T a, T b);

/**
 * Determine if the first argument is approximately greater than the second.
 *
 * :param a:   The first argument to compare.
 * :param b:   The second argument to compare.
 */
template < class T >
bool approxGreaterThan(T a, T b);

/**
 * The constant value of pi as a float.
 */
extern const float pi;

/**
 * A vector marking the origin of 3-space.
 */
extern const glm::vec3 origin;

/**
 * A vector marking the primary x-axis.
 */
extern const glm::vec3 xAxis;

/**
 * A vector marking the primary y-axis.
 */
extern const glm::vec3 yAxis;

/**
 * A vector marking the primary z-axis.
 */
extern const glm::vec3 zAxis;

} // namespace math
} // namespace crash

#include <crash/math/math.inl>
