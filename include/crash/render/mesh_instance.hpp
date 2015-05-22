#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <crash/render/mesh.hpp>
#include <crash/render/renderable.hpp>
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

class MeshInstance : public Renderable {
public:
   /////////////////////////////////////////////////////////////////////////////
   // Constructors.
   /////////////////////////////////////////////////////////////////////////////

   MeshInstance(const MeshInstance& instance);
   MeshInstance(const Mesh& mesh, const ColorUnit& color,
    const ShaderProgramPtr& program);
   virtual ~MeshInstance();

   /////////////////////////////////////////////////////////////////////////////
   // Renderable interface.
   /////////////////////////////////////////////////////////////////////////////

   MeshInstance* getMeshInstance();
   void render(float delta_t);
   void render(const glm::mat4& transform, float delta_t);

   /////////////////////////////////////////////////////////////////////////////
   // Data access.
   /////////////////////////////////////////////////////////////////////////////

   const Mesh& getMesh() const;

   const ColorUnit& getColor() const;
   void setColor(const ColorUnit& color);

   const ShaderProgramPtr& getShaderProgram() const;
   void setShaderProgram(const ShaderProgramPtr& program);

   const AnimationProgressSet& getAnimationProgress() const;

   void startAnimation(unsigned int index);
   void stopAnimation(unsigned int index);
   void progressAnimations(float delta_t);

private:
   NodeTransformMap getNodeTransforms(float delta_t) const;

   void getNodeTransformsHelper(const aiNode* node, float delta_t,
    const glm::mat4& transform, NodeTransformMap& nodeTransforms) const;

   glm::mat4 getNodeTransform(const aiNode* node, float delta_t) const;

   const Mesh& _mesh;
   ColorUnit _color;
   ShaderProgramPtr _program;
   AnimationProgressSet _animationProgress;
};

} // namespace render
} // namespace crash
