#include <iostream>
#include <crash/render/mesh_instance.hpp>
#include <crash/render/util.hpp>

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
   _mesh(instance._mesh), _transformer(instance._transformer),
    _animationProgress(instance._animationProgress)
{}

MeshInstance::MeshInstance(const Mesh& mesh,
 const space::Transformer& transformer) :
   _mesh(mesh), _transformer(transformer), _animationProgress()
{
   this->_animationProgress.resize(this->_mesh.getAnimations().size());
}

/* virtual */ MeshInstance::~MeshInstance() {}

////////////////////////////////////////////////////////////////////////////////
// Movable interface.
////////////////////////////////////////////////////////////////////////////////

glm::vec3 MeshInstance::getPosition() const {
   return this->_transformer.getPosition();
}

glm::quat MeshInstance::getOrientation() const {
   return this->_transformer.getOrientation();
}

glm::vec3 MeshInstance::getSize() const {
   return this->_transformer.getSize();
}

glm::vec3 MeshInstance::getTranslationalVelocity() const {
   return this->_transformer.getTranslationalVelocity();
}

glm::quat MeshInstance::getRotationalVelocity() const {
   return this->_transformer.getRotationalVelocity();
}

glm::vec3 MeshInstance::getScaleVelocity() const {
   return this->_transformer.getScaleVelocity();
}

void MeshInstance::setPosition(const glm::vec3& position) {
   this->setPosition(position);
}

void MeshInstance::setOrientation(const glm::quat& orientation) {
   this->setOrientation(orientation);
}

void MeshInstance::setSize(const glm::vec3& size) {
   this->setSize(size);
}

void MeshInstance::setTranslationalVelocity(const glm::vec3& translationalVelocity) {
   this->setTranslationalVelocity(translationalVelocity);
}

void MeshInstance::setRotationalVelocity(const glm::quat& rotationalVelocity) {
   this->setRotationalVelocity(rotationalVelocity);
}

void MeshInstance::setScaleVelocity(const glm::vec3& scaleVelocity) {
   this->setScaleVelocity(scaleVelocity);
}

////////////////////////////////////////////////////////////////////////////////
// Rendering.
////////////////////////////////////////////////////////////////////////////////

const Mesh& MeshInstance::getMesh() const {
   return this->_mesh;
}

const AnimationProgressSet& MeshInstance::getAnimationProgress() const {
   return this->_animationProgress;
}

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

void MeshInstance::render(const ShaderProgram& program,
 const UniformVariable& vars, const glm::mat4& parentTransform,
 float delta_t) const {
   glm::mat4 localTransform = this->getTransform(delta_t);
   glm::mat4 modelTransform = parentTransform * localTransform;

   this->_mesh.render(program, vars, modelTransform,
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
