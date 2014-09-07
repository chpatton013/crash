#pragma once

#include <tuple>
#include <glm/glm.hpp>

namespace crash {
namespace util {

/**
 * Create a consistently ordered pair from the given parameters, regardless
 * of the order they are passed.
 *
 * :param a:   The first element to include in the pair.
 * :param b:   The second element to include in the pair.
 * :return: The generated pair.
 */
template < class T >
std::tuple< T, T > unique_pair(T a, T b);

/**
 * Linearizes a vector-index from a 3-dimensional vector space.
 *
 * :param index:  The vector index to linearize.
 * :param bounds: The extents of the vector space.
 * :return: The linearized index in the vector space.
 */
int linearize_index(const glm::ivec3& index, const glm::ivec3& bounds);

/**
 * Vectorizes a linear-index from a 3-dimensional vector space.
 *
 * :param index:  The linear index to vectorize.
 * :param bounds: The extents of the vector space.
 * :return: The vectorized index in the vector space.
 */
glm::ivec3 vectorize_index(int index, const glm::ivec3& bounds);

/**
 * Print the given float to stdout.
 *
 * :param f:   The float to print.
 */
void print(float f);

/**
 * Print the given vector to stdout.
 *
 * :param v:   The vector to print.
 */
void print(const glm::vec3& v);

/**
 * Print the given vector to stdout.
 *
 * :param v:   The vector to print.
 */
void print(const glm::vec4& v);

/**
 * Print the given matrix to stdout.
 *
 * :param m:   The matrix to print.
 */
void print(const glm::mat4& m);

} // namespace util
} // namespace crash

#include <crash/util/util.inl>
