#pragma once

#include <limits>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace crash {
namespace math {

template < typename T >
bool approxEqual(const T& a, const T& b) {
   static const T epsilon = std::numeric_limits< T >::epsilon();

   return std::abs(a - b) < epsilon;
}

template<>
/* inline */ bool approxEqual< glm::vec3 >(const glm::vec3& a, const glm::vec3& b) {
   static const float epsilon = std::numeric_limits< float >::epsilon();

   return glm::epsilonEqual(a, b, epsilon).x;
}

template<>
/* inline */ bool approxEqual< glm::vec4 >(const glm::vec4& a, const glm::vec4& b) {
   static const float epsilon = std::numeric_limits< float >::epsilon();

   return glm::epsilonEqual(a, b, epsilon).x;
}

template<>
/* inline */ bool approxEqual< glm::mat4 >(const glm::mat4& a, const glm::mat4& b) {
   const float* av = glm::value_ptr(a);
   const float* bv = glm::value_ptr(b);

   for (int ndx = 0; ndx < 16; ++ndx) {
      if (!approxEqual(av[ndx], bv[ndx])) {
         return false;
      }
   }

   return true;
}

template < class T >
bool approxLessThan(T a, T b) {
   static const T epsilon = std::numeric_limits< T >::epsilon();

   return (a - b) < epsilon;
}

template < class T >
bool approxGreaterThan(T a, T b) {
   static const T epsilon = std::numeric_limits< T >::epsilon();

   return (a - b) > epsilon;
}

template < class T >
T average(T a, T b) {
   return a + (b - a) * 0.5f;
}

} // namespace math
} // namespace crash
