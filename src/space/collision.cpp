#include <crash/space/collision.hpp>

using namespace crash::space;

Collision::Collision(BoundingBox* first, BoundingBox* second) :
   _first(first), _second(second)
{}

/* virtual */ Collision::~Collision() {}

BoundingBox* Collision::getFirst() const {
   return this->_first;
}

BoundingBox* Collision::getSecond() const {
   return this->_second;
}

bool Collision::operator<(const Collision& other) const {
   return (this->_first == other._first) ? (this->_second < other._second) :
    (this->_first < other._first);
}

/* static */ Collision Collision::factory(BoundingBox* first,
 BoundingBox* second) {
   if (first < second) {
      return Collision(first, second);
   } else {
      return Collision(second, first);
   }
}
