#pragma once

#include <glm/glm.hpp>

namespace crash {
namespace math {

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

} // namespace math
} // namespace crash

#include <crash/math/arithmetic.inl>
