#include <crash/render/renderable.hpp>

using namespace crash::render;

void Renderable::render(float delta_t) const {
   this->render(glm::mat4(), delta_t);
}
