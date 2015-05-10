#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <crash/render/animation.hpp>
#include <crash/render/util.hpp>

using namespace crash::render;

/* static */ const unsigned int Animation::DEFAULT_TICKS_PER_SECOND = 24;

Animation::Animation(const aiAnimation* animation) :
   animation(animation)
{}

float Animation::getDuration() const {
   return this->animation->mDuration;
}

float Animation::getTicksPerSecond() const {
   if (this->animation->mTicksPerSecond == 0.0f) {
      return Animation::DEFAULT_TICKS_PER_SECOND;
   } else {
      return this->animation->mTicksPerSecond;
   }
}

float Animation::getTick(float delta_t) const {
   return delta_t * this->getTicksPerSecond();
}

boost::optional< glm::mat4 > Animation::getNodeTransform(const aiNode* node,
 float delta_t) const {
   float tick = this->getTick(delta_t);
   if (tick > this->animation->mDuration) {
      return boost::none;
   }

   auto channelOpt = this->findNodeChannel(node);
   if (!channelOpt) {
      return boost::none;
   }

   auto channel = channelOpt.get();
   glm::vec3 translate = this->interpolateVectorKey(channel->mPositionKeys,
    channel->mNumPositionKeys, tick);
   glm::quat rotate = this->interpolateQuatKey(channel->mRotationKeys,
    channel->mNumRotationKeys, tick);
   glm::vec3 scale = this->interpolateVectorKey(channel->mScalingKeys,
    channel->mNumScalingKeys, tick);

   return glm::scale(scale) * glm::mat4_cast(rotate) *
    glm::translate(translate);
}

boost::optional< const aiNodeAnim* > Animation::findNodeChannel(
 const aiNode* node) const {
   for (unsigned int i = 0; i < this->animation->mNumChannels; ++i) {
      const aiNodeAnim* channel = this->animation->mChannels[i];
      if (channel->mNodeName == node->mName) {
         return channel;
      }
   }

   return boost::none;
}

glm::vec3 Animation::interpolateVectorKey(
 const aiVectorKey* keys, unsigned int numKeys, float tick) const {
   unsigned int a;
   unsigned int b;
   for (a = 0, b = 1; b < numKeys; ++a, ++b) {
      if (tick < keys[b].mTime) {
         break;
      }
   }

   const aiVectorKey* keyA = keys + a;
   glm::vec3 vecA = vec3AiToGlm(keyA->mValue);

   if (b >= numKeys) {
      return vecA;
   }

   const aiVectorKey* keyB = keys + b;
   glm::vec3 vecB = vec3AiToGlm(keyB->mValue);

   float tween = (tick - keyA->mTime) / (keyB->mTime - keyA->mTime);
   return (vecA * (1 - tween)) + (vecB * tween);
}

glm::quat Animation::interpolateQuatKey(
 const aiQuatKey* keys, unsigned int numKeys, float tick) const {
   unsigned int a;
   unsigned int b;
   for (a = 0, b = 1; b < numKeys; ++a, ++b) {
      if (tick < keys[b].mTime) {
         break;
      }
   }

   const aiQuatKey* keyA = keys + a;
   glm::quat quatA = quatAiToGlm(keyA->mValue);

   if (b >= numKeys) {
      return quatA;
   }

   const aiQuatKey* keyB = keys + b;
   glm::quat quatB = quatAiToGlm(keyB->mValue);

   float tween = (tick - keyA->mTime) / (keyB->mTime - keyA->mTime);
   return glm::slerp(quatA, quatB, tween);
}
