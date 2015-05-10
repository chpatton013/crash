#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <crash/space/movable.hpp>
#include <crash/render/mesh.hpp>

namespace crash {
namespace render {

struct AnimationProgress {
   AnimationProgress();
   void start();
   void stop();

   bool active;
   float progress;
};

typedef std::vector< AnimationProgress > AnimationProgressSet;

class MeshInstance : public space::Movable {
public:
   /////////////////////////////////////////////////////////////////////////////
   // Constructors.
   /////////////////////////////////////////////////////////////////////////////

   MeshInstance(const MeshInstance& instance);
   MeshInstance(const Mesh& mesh, const space::Transformer& transformer);
   virtual ~MeshInstance();

   /////////////////////////////////////////////////////////////////////////////
   // Movable interface.
   /////////////////////////////////////////////////////////////////////////////

   glm::vec3 getPosition() const;
   glm::quat getOrientation() const;
   glm::vec3 getSize() const;
   glm::vec3 getTranslationalVelocity() const;
   glm::quat getRotationalVelocity() const;
   glm::vec3 getScaleVelocity() const;

   void setPosition(const glm::vec3& position);
   void setOrientation(const glm::quat& orientation);
   void setSize(const glm::vec3& size);
   void setTranslationalVelocity(const glm::vec3& translationalVelocity);
   void setRotationalVelocity(const glm::quat& rotationalVelocity);
   void setScaleVelocity(const glm::vec3& scaleVelocity);

   /////////////////////////////////////////////////////////////////////////////
   // Rendering.
   /////////////////////////////////////////////////////////////////////////////

   const Mesh& getMesh() const;

   const AnimationProgressSet& getAnimationProgress() const;
   void startAnimation(unsigned int index);
   void stopAnimation(unsigned int index);
   void progressAnimations(float delta_t);

   void render(const ShaderProgram& program, const UniformVariable& vars,
    const glm::mat4& parentTransform, float delta_t) const;

private:
   NodeTransformMap getNodeTransforms(float delta_t) const;

   void getNodeTransformsHelper(const aiNode* node, float delta_t,
    const glm::mat4& transform, NodeTransformMap& nodeTransforms) const;

   glm::mat4 getNodeTransform(const aiNode* node, float delta_t) const;

   const Mesh& _mesh;
   space::Transformer _transformer;
   AnimationProgressSet _animationProgress;
};

} // namespace render
} // namespace crash
