#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <crash/render/mesh.hpp>
#include <crash/render/shader_program.hpp>

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

class MeshInstance {
public:
   /////////////////////////////////////////////////////////////////////////////
   // Constructors.
   /////////////////////////////////////////////////////////////////////////////

   MeshInstance(const MeshInstance& instance);
   MeshInstance(const Mesh& mesh, const ColorUnit& color,
    const std::shared_ptr< ShaderProgram >& program);
   virtual ~MeshInstance();

   /////////////////////////////////////////////////////////////////////////////
   // Data access.
   /////////////////////////////////////////////////////////////////////////////

   const Mesh& getMesh() const;

   const ColorUnit& getColor() const;
   void setColor(const ColorUnit& color);

   const std::shared_ptr< ShaderProgram >& getShaderProgram() const;
   void setShaderProgram(const std::shared_ptr< ShaderProgram >& program);

   const AnimationProgressSet& getAnimationProgress() const;

   /////////////////////////////////////////////////////////////////////////////
   // Rendering.
   /////////////////////////////////////////////////////////////////////////////

   void startAnimation(unsigned int index);
   void stopAnimation(unsigned int index);
   void progressAnimations(float delta_t);

   void render(const glm::mat4& modelTransform, float delta_t) const;

private:
   NodeTransformMap getNodeTransforms(float delta_t) const;

   void getNodeTransformsHelper(const aiNode* node, float delta_t,
    const glm::mat4& transform, NodeTransformMap& nodeTransforms) const;

   glm::mat4 getNodeTransform(const aiNode* node, float delta_t) const;

   const Mesh& _mesh;
   ColorUnit _color;
   std::shared_ptr< ShaderProgram > _program;
   AnimationProgressSet _animationProgress;
};

} // namespace render
} // namespace crash
