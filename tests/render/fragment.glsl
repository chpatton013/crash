#version 410

// FRAGMENT SHADER

uniform vec3 uLightPosition[8];
uniform vec4 uLightDiffuse[8];
uniform vec4 uLightSpecular[8];

uniform vec3 uCameraPosition;

uniform vec4 uAmbientColor;
uniform vec4 uDiffuseColor;
uniform vec4 uSpecularColor;
uniform float uShininessValue;
uniform bool uHasDiffuseTexture;
uniform sampler2D uDiffuseTexture;

in vec3 vPosition;
in vec3 vNormal;
in vec3 vTangent;
in vec3 vBitangent;
in vec2 vTexCoord;

out vec4 oColor;

vec3 getPosition() {
   return vPosition;
}

vec3 getNormal() {
   return vNormal;
}

vec4 getAmbientColor() {
   return uAmbientColor;
}

vec4 getDiffuseColor() {
   if (uHasDiffuseTexture) {
      return texture(uDiffuseTexture, vTexCoord);
   } else {
      return uDiffuseColor;
   }
}

vec4 getSpecularColor() {
   return uSpecularColor;
}

float getShininessValue() {
   return uShininessValue;
}

vec4 getColorForLight(vec3 P, vec3 N, vec4 kD, vec4 kS, float n, int i) {
   vec3 lP = uLightPosition[i];
   vec4 lD = uLightDiffuse[i];
   vec4 lS = uLightSpecular[i];

   vec3 L = normalize(lP - P);
   vec3 R = normalize(-reflect(L, N));
   vec3 V = normalize(P - uCameraPosition);

   float nDotL = max(dot(N, L), 0.0);
   float rDotV = max(dot(R, V), 0.0);

   vec4 diffuse = kD * nDotL * lD;
   vec4 specular = kS * pow(rDotV, n) * lS;

   return clamp(diffuse + specular, 0.0, 1.0);
}

void main() {
   vec3 position = getPosition();
   vec3 normal = getNormal();
   vec4 ambientColor = getAmbientColor();
   vec4 diffuseColor = getDiffuseColor();
   vec4 specularColor = getSpecularColor();
   float shininessValue = getShininessValue();

   oColor = ambientColor;
   for (int i = 0; i < 8; ++i) {
      oColor += getColorForLight(position, normal, diffuseColor, specularColor,
       shininessValue, i);
   }
   oColor = clamp(oColor, 0.0, 1.0);
}
