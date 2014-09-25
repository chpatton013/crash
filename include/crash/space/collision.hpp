#pragma once

#include <boost/functional/hash.hpp>

namespace crash {
namespace space {

class Boundable;

class Collision {
public:
   virtual ~Collision();

   Boundable* first() const;
   Boundable* second() const;

   bool operator<(const Collision& other) const;
   bool operator==(const Collision& other) const;

   static Collision factory(Boundable* first, Boundable* second);

private:
   Collision(Boundable* first, Boundable* second);

   Boundable* _first;
   Boundable* _second;
};

} // namespace space
} // namespace crash

namespace std {

template<>
struct hash< crash::space::Collision > {
   std::size_t operator()(crash::space::Collision const& arg) const noexcept {
      std::size_t seed = 0;
      boost::hash_combine(seed, arg.first());
      boost::hash_combine(seed, arg.second());
      return seed;
   }
};

} // namespace std
