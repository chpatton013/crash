#pragma once

#include <tuple>
#include <crash/math/math.hpp>

namespace crash {
namespace util {

template < class T >
std::tuple< T, T > unique_pair(T a, T b) {
   if (a < b) {
      return std::tuple< T, T >(a, b);
   } else {
      return std::tuple< T, T >(b, a);
   }
}

} // namespace util
} // namespace crash
