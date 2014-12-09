#pragma once

#include <boost/functional/hash.hpp>
#include <tuple>

namespace std {

template< typename... T >
struct hash< tuple< T... > > {
   std::size_t operator()(tuple< T... > const& arg) const noexcept {
      return boost::hash_value(arg);
   }
};

} // namespace std
