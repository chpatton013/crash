#pragma once

#include <glm/glm.hpp>

namespace crash {
namespace math {

class Transformable {
public:
   virtual ~Transformable() {};

   virtual const glm::vec3& getPosition() const = 0;
   virtual const glm::vec4& getOrientation() const = 0;
   virtual const glm::vec3& getSize() const = 0;

   virtual void setPosition(const glm::vec3& position) = 0;
   virtual void setOrientation(const glm::vec4& orientation) = 0;
   virtual void setSize(const glm::vec3& size) = 0;

   virtual const glm::mat4& getTransform() = 0;
};

} // namespace math
} // namespace crash
