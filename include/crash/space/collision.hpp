#pragma once

namespace crash {
namespace space {

class BoundingBox;

class Collision {
public:
   virtual ~Collision();

   BoundingBox* getFirst() const;
   BoundingBox* getSecond() const;

   bool operator<(const Collision& other) const;

   static Collision factory(BoundingBox* first, BoundingBox* second);

private:
   Collision(BoundingBox* first, BoundingBox* second);

   BoundingBox* _first;
   BoundingBox* _second;
};

} // namespace space
} // namespace crash
