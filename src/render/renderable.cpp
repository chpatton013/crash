#include <crash/render/renderable.hpp>
#include <crash/render/mesh_instance.hpp>

using namespace crash::render;

/* virtual */ void Renderable::render(float delta_t) {
   return this->getMeshInstance()->render(delta_t);
}

/* virtual */ void Renderable::render(
 const glm::mat4& transform, float delta_t) {
   return this->getMeshInstance()->render(transform, delta_t);
}
