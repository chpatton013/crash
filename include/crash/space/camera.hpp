#pragma once

#include <boost/optional/optional.hpp>
#include <glm/glm.hpp>
#include <crash/space/view_frustum.hpp>

namespace crash {
namespace space {

/**
 * The data type used to represent a camera in a scene.
 *
 * :member position:    The absolute global position of the Camera.
 * :member orientation: The direction the Camera is facing.
 * :member fieldOfView: The vertical field of view, in radians.
 * :member aspectRatio: The ratio between the horizontal and vertical fields of
 *                      view.
 * :member nearPlane:   The distance to the Camera's near plane.
 * :member farPlane:    The distance to the Camera's far plane.
 */
class Camera {
public:
   Camera(const glm::vec3& position, const glm::vec4& orientation,
    float fieldOfView, float aspectRatio, float nearPlane, float farPlane);

   ////////////////////////////////////////////////////////////////////////////
   // Data getters
   ////////////////////////////////////////////////////////////////////////////

   const glm::vec3& position() const;
   const glm::vec4& orientation() const;
   float fieldOfView() const;
   float aspectRatio() const;
   float nearPlane() const;
   float farPlane() const;

   ////////////////////////////////////////////////////////////////////////////
   // Data setters
   ////////////////////////////////////////////////////////////////////////////

   void position(const glm::vec3& position);
   void orientation(const glm::vec4& orientation);
   void fieldOfView(float fieldOfView);
   void aspectRatio(float aspectRatio);
   void nearPlane(float nearPlane);
   void farPlane(float farPlane);

   ////////////////////////////////////////////////////////////////////////////
   // Memoized getters
   ////////////////////////////////////////////////////////////////////////////

   /**
    * Calculate the transform matrix for this Camera.
    */
   const glm::mat4& getTransform();

   /**
    * Calculate the perspective matrix for this Camera.
    */
   const glm::mat4& getPerspective();

   /**
    * Calculate the planes of the viewing frustum for this Camera.
    * Ordered:
    *    0 - left
    *    1 - right
    *    2 - top
    *    3 - bottom
    *    4 - near
    *    5 - far
    */
   const ViewFrustum& getViewFrustum();

   ////////////////////////////////////////////////////////////////////////////
   // Other methods
   ////////////////////////////////////////////////////////////////////////////

   /**
    * Determine if a point is within the view frustum of this Camera.
    *
    * :param point:  The point to assess.
    */
   bool isPointVisible(const glm::vec3& point);

private:
   // Data members
   glm::vec3 _position;
   glm::vec4 _orientation;
   float _fieldOfView;
   float _aspectRatio;
   float _nearPlane;
   float _farPlane;

   // Memoization generators
   void invalidate();
   void generateTransform();
   void generatePerspective();
   void generateViewFrustum();

   // Memoization members
   boost::optional< glm::mat4 > _transform;
   boost::optional< glm::mat4 > _perspective;
   boost::optional< ViewFrustum > _viewFrustum;
};

} // namespace space
} // namespace crash
