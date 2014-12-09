#include <crash/space/collision.hpp>

using namespace crash::space;

Collision::Collision(Boundable* first, Boundable* second) :
   _first(first), _second(second)
{}

/* virtual */ Collision::~Collision() {}

Boundable* Collision::first() const { return this->_first; }
Boundable* Collision::second() const { return this->_second; }

bool Collision::operator<(const Collision& other) const {
   return (this->_first == other._first) ? (this->_second < other._second) :
    (this->_first < other._first);
}

bool Collision::operator==(const Collision& other) const {
   return this->_first == other._first && this->_second == other._second;
}

/* static */ Collision Collision::factory(Boundable* first, Boundable* second) {
   if (first < second) {
      return Collision(first, second);
   } else {
      return Collision(second, first);
   }
}
