#pragma once

#include <boost/functional/hash.hpp>
#include <tuple>

#define AUTO auto&
#define CAUTO const AUTO

namespace std {

template<typename... T>
struct hash< tuple< T... > > {
   std::size_t operator()(tuple< T... > const& arg) const noexcept {
      return boost::hash_value(arg);
   }
};

}
