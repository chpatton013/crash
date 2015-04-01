#version 410

// FRAGMENT SHADER

uniform vec3 uCameraPosition;

uniform vec3 uLightPosition;
uniform float uLightIntensity;

uniform vec4 uDefaultAmbientColor = vec4(vec3(0.4), 1.0);
uniform vec4 uDefaultDiffuseColor = vec4(vec3(0.7), 1.0);
uniform vec4 uDefaultSpecularColor = vec4(vec3(0.9), 1.0);
uniform float uDefaultSpecularReflectivity = 250.0;

// uniform bool uHasAmbientTexture;
// uniform bool uHasDiffuseTexture;
// uniform bool uHasSpecularTexture;
// uniform sampler2D uAmbientSampler;
// uniform sampler2D uDiffuseSampler;
// uniform sampler2D uSpecularSampler;

in vec3 vPosition;
in vec3 vNormal;
in vec3 vTangent;
in vec3 vBitangent;
in vec3 vTexCoord;

out vec4 oColor;

void main() {
   vec4 ambientColor;
   vec4 diffuseColor;
   vec4 specularColor;
   float reflectivity;

   // if (uHasAmbientTexture) {
   //    ambientColor = texture3D(uAmbientSampler, vTexCoord);
   // } else {
   //    ambientColor = uDefaultAmbientColor;
   // }

   // if (uHasDiffuseTexture) {
   //    diffuseColor = texture3D(uDiffuseSampler, vTexCoord);
   // } else {
   //    diffuseColor = uDefaultDiffuseColor;
   // }

   // if (uHasSpecularTexture) {
   //    specularColor = texture3D(uSpecularSampler, vTexCoord);
   //    specularReflectivity = uDefaultSpecularReflectivity;
   // } else {
   //    specularColor = uDefaultSpecularColor;
   //    reflectivity = uDefaultSpecularReflectivity;
   // }

   ambientColor = uDefaultAmbientColor;
   diffuseColor = uDefaultDiffuseColor;
   specularColor = uDefaultSpecularColor;
   reflectivity = uDefaultSpecularReflectivity;

   vec3 L = normalize(uLightPosition - vPosition);
   float nDotL = dot(vNormal, L);

   vec3 V = normalize(vPosition - uCameraPosition);
   vec3 R = normalize((2 * nDotL * vNormal) - L);
   float rDotV = dot(R, V);

   vec4 ambient = ambientColor;
   vec4 diffuse = diffuseColor * nDotL;
   vec4 specular = specularColor * pow(rDotV, reflectivity);

   oColor = clamp(ambient + uLightIntensity * (diffuse + specular), 0.0, 1.0);
}
