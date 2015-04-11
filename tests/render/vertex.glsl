#version 410

// VERTEX SHADER

uniform mat4 uMvpMatrix;

in vec3 aPosition;
in vec3 aNormal;
in vec3 aTangent;
in vec3 aBitangent;
in vec2 aTexCoord;

out vec3 vPosition;
out vec3 vNormal;
out vec3 vTangent;
out vec3 vBitangent;
out vec2 vTexCoord;

void main() {
   vPosition = aPosition;
   vNormal = aNormal;
   vTangent = aTangent;
   vBitangent = aBitangent;
   vTexCoord = aTexCoord;

   gl_Position = uMvpMatrix * vec4(aPosition, 1.0);
}
