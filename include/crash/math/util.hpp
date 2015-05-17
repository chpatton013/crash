#pragma once

#include <glm/glm.hpp>

namespace crash {
namespace math {

/**
 * Print the given float to stdout.
 *
 * :param f:   The float to print.
 */
void print(float f);

/**
 * Print the given float to stdout.
 *
 * :param f:         The float to print.
 * :param newline:   Should a trailing newline be printed.
 */
void print(float f, bool newline);

/**
 * Print the given vector to stdout.
 *
 * :param v:   The vector to print.
 */
void print(const glm::vec3& v);

/**
 * Print the given vector to stdout.
 *
 * :param v:         The vector to print.
 * :param newline:   Should a trailing newline be printed.
 */
void print(const glm::vec3& v, bool newline);

/**
 * Print the given vector to stdout.
 *
 * :param v:   The vector to print.
 */
void print(const glm::vec4& v);

/**
 * Print the given vector to stdout.
 *
 * :param v:         The vector to print.
 * :param newline:   Should a trailing newline be printed.
 */
void print(const glm::vec4& v, bool newline);

/**
 * Print the given vector to stdout.
 *
 * :param v:   The vector to print.
 */
void print(const glm::ivec3& v);

/**
 * Print the given vector to stdout.
 *
 * :param v:         The vector to print.
 * :param newline:   Should a trailing newline be printed.
 */
void print(const glm::ivec3& v, bool newline);

/**
 * Print the given vector to stdout.
 *
 * :param v:   The vector to print.
 */
void print(const glm::ivec4& v);

/**
 * Print the given vector to stdout.
 *
 * :param v:         The vector to print.
 * :param newline:   Should a trailing newline be printed.
 */
void print(const glm::ivec4& v, bool newline);

/**
 * Print the given quaternion to stdout.
 *
 * :param v:   The quaternion to print.
 */
void print(const glm::quat& q);

/**
 * Print the given quaternion to stdout.
 *
 * :param v:         The quaternion to print.
 * :param newline:   Should a trailing newline be printed.
 */
void print(const glm::quat& q, bool newline);

/**
 * Print the given matrix to stdout.
 *
 * :param m:   The matrix to print.
 */
void print(const glm::mat4& m);

} // namespace math
} // namespace crash
