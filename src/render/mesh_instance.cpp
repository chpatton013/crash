#include <iostream>
#include <crash/render/mesh_instance.hpp>
#include <crash/render/util.hpp>

using namespace crash::common;
using namespace crash::render;

AnimationProgress::AnimationProgress() :
   active(false), progress(0.0f)
{}

void AnimationProgress::start() {
   this->active = true;
   this->progress = 0.0f;
}

void AnimationProgress::stop() {
   this->active = false;
}

MeshInstance::MeshInstance(const MeshInstance& instance) :
   _mesh(instance._mesh), _color(instance._color), _program(instance._program),
    _animationProgress(instance._animationProgress)
{}

MeshInstance::MeshInstance(const Mesh& mesh, const ColorUnit& color,
 const std::shared_ptr< ShaderProgram >& program) :
   _mesh(mesh), _color(color), _program(program), _animationProgress()
{
   this->_animationProgress.resize(this->_mesh.getAnimations().size());
}

/* virtual */ MeshInstance::~MeshInstance() {}

////////////////////////////////////////////////////////////////////////////////
// Data access.
////////////////////////////////////////////////////////////////////////////////

const Mesh& MeshInstance::getMesh() const {
   return this->_mesh;
}

const ColorUnit& MeshInstance::getColor() const {
   return this->_color;
}

void MeshInstance::setColor(const ColorUnit& color) {
   this->_color = color;
}


const std::shared_ptr< ShaderProgram >& MeshInstance::getShaderProgram() const {
   return this->_program;
}

void MeshInstance::setShaderProgram(
 const std::shared_ptr< ShaderProgram >& program) {
   this->_program = program;
}

const AnimationProgressSet& MeshInstance::getAnimationProgress() const {
   return this->_animationProgress;
}

////////////////////////////////////////////////////////////////////////////////
// Rendering.
////////////////////////////////////////////////////////////////////////////////

void MeshInstance::startAnimation(unsigned int index) {
   this->_animationProgress[index].start();
}

void MeshInstance::stopAnimation(unsigned int index) {
   this->_animationProgress[index].stop();
}

void MeshInstance::progressAnimations(float delta_t) {
   for (AnimationProgress& a : this->_animationProgress) {
      if (a.active) {
         a.progress += delta_t;
      }
   }
}

void MeshInstance::render(const glm::mat4& modelTransform,
 float delta_t) const {
   this->_mesh.render(*this->_program, this->_color, modelTransform,
    this->getNodeTransforms(delta_t));
}

NodeTransformMap MeshInstance::getNodeTransforms(float delta_t) const {
   NodeTransformMap nodeTransforms;
   this->getNodeTransformsHelper(
    this->_mesh.getScene()->mRootNode, delta_t, glm::mat4(), nodeTransforms);
   return nodeTransforms;
}

void MeshInstance::getNodeTransformsHelper(const aiNode* node, float delta_t,
 const glm::mat4& transform, NodeTransformMap& nodeTransforms) const {
   glm::mat4 localTransform = this->getNodeTransform(node, delta_t);
   glm::mat4 completeTransform = transform * localTransform;

   nodeTransforms.insert(std::make_pair(node, completeTransform));

   for (unsigned int i = 0; i < node->mNumChildren; ++i) {
      this->getNodeTransformsHelper(
       node->mChildren[i], delta_t, completeTransform, nodeTransforms);
   }
}

glm::mat4 MeshInstance::getNodeTransform(
 const aiNode* node, float delta_t) const {
   auto animations = this->_mesh.getAnimations();

   std::vector< glm::mat4 > transforms;
   transforms.reserve(this->_animationProgress.size());

   for (unsigned int i = 0; i < this->_animationProgress.size(); ++i) {
      const AnimationProgress& animationProgress = this->_animationProgress[i];
      if (!animationProgress.active) {
         continue;
      }

      auto transformOpt = animations[i].getNodeTransform(
       node, animationProgress.progress + delta_t);
      if (transformOpt) {
         transforms.push_back(transformOpt.get());
      }
   }

   // For now we'll ignore multi-animation blending.
   if (transforms.size() > 0) {
      return transforms[0];
   }

   return mat4AiToGlm(node->mTransformation);
}
