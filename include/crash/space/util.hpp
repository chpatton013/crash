#pragma once

#include <glm/glm.hpp>

namespace crash {
namespace space {

/**
 * Linearizes a vector-index from a 2-dimensional vector space.
 *
 * :param index:  The vector index to linearize.
 * :param bounds: The extents of the vector space.
 * :return: The linearized index in the vector space.
 */
int linearize_index(const glm::ivec2& index, const glm::ivec2& bounds);

/**
 * Linearizes a vector-index from a 3-dimensional vector space.
 *
 * :param index:  The vector index to linearize.
 * :param bounds: The extents of the vector space.
 * :return: The linearized index in the vector space.
 */
int linearize_index(const glm::ivec3& index, const glm::ivec3& bounds);

/**
 * Vectorizes a linear-index from a 2-dimensional vector space.
 *
 * :param index:  The linear index to vectorize.
 * :param bounds: The extents of the vector space.
 * :return: The vectorized index in the vector space.
 */
glm::ivec2 vectorize_index(int index, const glm::ivec2& bounds);

/**
 * Vectorizes a linear-index from a 3-dimensional vector space.
 *
 * :param index:  The linear index to vectorize.
 * :param bounds: The extents of the vector space.
 * :return: The vectorized index in the vector space.
 */
glm::ivec3 vectorize_index(int index, const glm::ivec3& bounds);

} // namespace space
} // namespace crash
