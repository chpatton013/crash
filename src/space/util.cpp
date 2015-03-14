#include <cstdlib>
#include <crash/space/util.hpp>

using namespace crash::space;

int crash::space::linearize_index(const glm::ivec2& index,
 const glm::ivec2& bounds) {
   return (index.y * bounds.x) + index.x;
}

int crash::space::linearize_index(const glm::ivec3& index,
 const glm::ivec3& bounds) {
   return (index.z * bounds.x * bounds.y) + (index.y * bounds.x) + index.x;
}

glm::ivec2 crash::space::vectorize_index(int index, const glm::ivec2& bounds) {
   // x: index % bounds.x
   // y: index / bounds.x
   std::div_t q = std::div(index, bounds.x);
   return glm::ivec2(q.rem, q.quot);
}

glm::ivec3 crash::space::vectorize_index(int index, const glm::ivec3& bounds) {
   // x: index % bounds.x
   // y: (index / bounds.x) % bounds.y
   // z: (index / bounds.x) / bounds.y
   std::div_t x = std::div(index, bounds.x);
   std::div_t yz = std::div(x.quot, bounds.y);
   return glm::ivec3(x.rem, yz.rem, yz.quot);
}
