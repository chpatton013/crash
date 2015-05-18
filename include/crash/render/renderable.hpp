#pragma once

#include <glm/glm.hpp>

namespace crash {
namespace render {

struct Renderable {
   virtual void render(float delta_t) const;
   virtual void render(const glm::mat4& transform, float delta_t) const = 0;
};

} // namespace render
} // namespace crash
