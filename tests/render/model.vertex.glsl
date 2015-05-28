#version 410

// VERTEX SHADER

const uint MAX_NUM_BONES = 100;
const uint MAX_NUM_BONES_PER_VERTEX = 4;

uniform mat4 uModelTransform;
uniform mat4 uViewTransform;
uniform mat4 uPerspectiveTransform;
uniform mat4 uBones[MAX_NUM_BONES];

in vec3 aPosition;
in vec3 aNormal;
in vec3 aTangent;
in vec3 aBitangent;
in vec2 aTexCoord;
in ivec4 aBoneIds;
in vec4 aBoneWeights;

out vec3 vPosition;
out vec3 vNormal;
out vec3 vTangent;
out vec3 vBitangent;
out vec2 vTexCoord;

void main() {
   // Transform vertex attributes into bone space for current pose.
   vec4 originalPosition = vec4(aPosition, 1.0);
   vec4 originalNormal = vec4(aNormal, 0.0);
   vec4 originalTangent = vec4(aTangent, 0.0);
   vec4 originalBitangent = vec4(aBitangent, 0.0);

   vec4 skinnedPosition = vec4(0.0);
   vec4 skinnedNormal = vec4(0.0);
   vec4 skinnedTangent = vec4(0.0);
   vec4 skinnedBitangent = vec4(0.0);

   float defaultWeight = 1.0;

   for (uint i = 0; i < MAX_NUM_BONES_PER_VERTEX; ++i) {
      mat4 boneMatrix = uBones[aBoneIds[i]];
      mat4 normalBoneMatrix = transpose(inverse(boneMatrix));
      float weight = aBoneWeights[i];
      defaultWeight -= weight;

      skinnedPosition += boneMatrix * originalPosition * weight;
      skinnedNormal += normalBoneMatrix * originalNormal * weight;
      skinnedTangent += normalBoneMatrix * originalTangent * weight;
      skinnedBitangent += normalBoneMatrix * originalBitangent * weight;
   }

   skinnedPosition += originalPosition * defaultWeight;
   skinnedNormal += originalNormal * defaultWeight;
   skinnedTangent += originalTangent * defaultWeight;
   skinnedBitangent += originalBitangent * defaultWeight;


   // Calculate reused matrices.
   mat4 mvTransform = uViewTransform * uModelTransform;
   mat4 normalMvTransform = transpose(inverse(mvTransform));


   // Set varying fragment values.
   vPosition = vec3(mvTransform * skinnedPosition);
   vNormal = normalize(vec3(normalMvTransform * skinnedNormal));
   vTangent = normalize(vec3(normalMvTransform * skinnedTangent));
   vBitangent = normalize(vec3(normalMvTransform * skinnedBitangent));
   vTexCoord = aTexCoord;


   // Move vertex to MVP space.
   mat4 mvpTransform = uPerspectiveTransform * mvTransform;
   gl_Position = mvpTransform * skinnedPosition;
}
