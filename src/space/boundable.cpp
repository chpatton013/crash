#include <crash/space/boundable.hpp>
#include <crash/space/bounding_box.hpp>

using namespace crash::render;
using namespace crash::space;

/* virtual */ bool Boundable::isVisible(const ViewFrustum& viewFrustum) {
   return this->getBoundingBox()->isVisible(viewFrustum);
}

/* virtual */ bool Boundable::isIntersecting(Boundable* boundable) {
   return this->getBoundingBox()->isIntersecting(boundable);
}
