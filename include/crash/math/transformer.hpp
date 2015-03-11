#pragma once

#include <glm/glm.hpp>
#include <boost/optional.hpp>

namespace crash {
namespace math {

class Transformer {
public:
   Transformer(const Transformer& transformer);
   Transformer(const glm::vec3& position, const glm::vec4& orientation,
    const glm::vec3& size);

   const glm::vec3& position() const;
   const glm::vec4& orientation() const;
   const glm::vec3& size() const;

   void position(const glm::vec3& position);
   void orientation(const glm::vec4& orientation);
   void size(const glm::vec3& size);

   void invalidate();

   const glm::mat4& getTransform();

private:
   glm::vec3 _position;
   glm::vec4 _orientation;
   glm::vec3 _size;

   void generateTransform();

   boost::optional< glm::mat4 > _transform;
};

} // namespace math
} // namespace crash
