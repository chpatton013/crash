#pragma once

#include <assimp/scene.h>
#include <boost/optional.hpp>
#include <glm/glm.hpp>

namespace crash {
namespace render {

class Animation {
public:
   Animation(const aiAnimation* animation);

   float getDuration() const;
   float getTick(float delta_t) const;
   boost::optional< glm::mat4 > getNodeTransform(const aiNode* node,
    float delta_t) const;

private:
   boost::optional< const aiNodeAnim* > findNodeChannel(
    const aiNode* node) const;
   glm::vec3 interpolateVectorKey(const aiVectorKey* keys,
    unsigned int numKeys, float tick) const;
   glm::quat interpolateQuatKey(const aiQuatKey* keys,
    unsigned int numKeys, float tick) const;

   const aiAnimation* animation;
};

} // namespace render
} // namespace crash
