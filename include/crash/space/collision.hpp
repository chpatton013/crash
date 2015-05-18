#pragma once

namespace crash {
namespace space {

struct Boundable;

class Collision {
public:
   virtual ~Collision();

   Boundable* getFirst() const;
   Boundable* getSecond() const;

   bool operator<(const Collision& other) const;

   static Collision factory(Boundable* first, Boundable* second);

private:
   Collision(Boundable* first, Boundable* second);

   Boundable* _first;
   Boundable* _second;
};

} // namespace space
} // namespace crash
