#pragma once

#include <glm/glm.hpp>

namespace crash {
namespace math {

/**
 * The constant value of pi as a float.
 */
extern const float pi;

/**
 * A vector marking the origin of the coordinate system.
 */
extern const glm::vec3 ORIGIN;

/**
 * A vector marking the primary x-axis.
 */
extern const glm::vec3 X_AXIS;

/**
 * A vector marking the primary y-axis.
 */
extern const glm::vec3 Y_AXIS;

/**
 * A vector marking the primary z-axis.
 */
extern const glm::vec3 Z_AXIS;

/**
 * A vector marking the global up direction.
 */
extern const glm::vec3 UP;

/**
 * A vector marking the global forward direction.
 */
extern const glm::vec3 FORWARD;

/**
 * A vector marking the global right direction.
 */
extern const glm::vec3 RIGHT;

/**
 * A quaternion representing no rotation;
 */
extern const glm::quat NO_ROTATION;

} // namespace math
} // namespace crash
