#pragma once

#include <boost/optional.hpp>
#include <glm/glm.hpp>

namespace crash {
namespace math {

class Transformer {
public:
   ////////////////////////////////////////////////////////////////////////////
   // Constructors.
   ////////////////////////////////////////////////////////////////////////////

   Transformer(const Transformer& transformer);
   Transformer(const glm::vec3& position, const glm::vec4& orientation,
    const glm::vec3& size);

   ////////////////////////////////////////////////////////////////////////////
   // Data access.
   ////////////////////////////////////////////////////////////////////////////

   const glm::vec3& getPosition() const;
   const glm::vec4& getOrientation() const;
   const glm::vec3& getSize() const;

   void setPosition(const glm::vec3& position);
   void setOrientation(const glm::vec4& orientation);
   void setSize(const glm::vec3& size);

   ////////////////////////////////////////////////////////////////////////////
   // Memoization.
   ////////////////////////////////////////////////////////////////////////////

   void invalidate();

   glm::mat4 getTranslateTransform();
   glm::mat4 getRotateTransform();
   glm::mat4 getScaleTransform();
   const glm::mat4& getPositionInvariantTransform();
   const glm::mat4& getOrientationInvariantTransform();
   const glm::mat4& getSizeInvariantTransform();
   const glm::mat4& getTransform();

   void applyTranslation(const glm::vec3& translation);
   void applyRotation(const glm::vec4& rotation);
   void applyScale(const glm::vec3& scale);

private:
   void calculatePositionInvariantTransform();
   void calculateOrientationInvariantTransform();
   void calculateSizeInvariantTransform();
   void calculateTransform();

   glm::vec3 _position;
   glm::vec4 _orientation;
   glm::vec3 _size;

   boost::optional< glm::mat4 > _positionInvariantTransform;
   boost::optional< glm::mat4 > _orientationInvariantTransform;
   boost::optional< glm::mat4 > _sizeInvariantTransform;
   boost::optional< glm::mat4 > _transform;
};

} // namespace math
} // namespace crash
