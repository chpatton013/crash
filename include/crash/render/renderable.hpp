#pragma once

#include <glm/glm.hpp>

namespace crash {
namespace render {

class MeshInstance;

struct Renderable {
   virtual MeshInstance* getMeshInstance() = 0;
   virtual void render(float delta_t);
   virtual void render(const glm::mat4& transform, float delta_t);
};

} // namespace render
} // namespace crash
