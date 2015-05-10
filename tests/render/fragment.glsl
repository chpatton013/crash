#version 410

// FRAGMENT SHADER

const uint MAX_NUM_LIGHTS = 8;

uniform vec3 uLightPosition[MAX_NUM_LIGHTS];
uniform vec4 uLightDiffuse[MAX_NUM_LIGHTS];
uniform vec4 uLightSpecular[MAX_NUM_LIGHTS];

uniform vec3 uCameraPosition;

uniform vec4 uAmbientColor;
uniform vec4 uDiffuseColor;
uniform vec4 uSpecularColor;
uniform float uShininessValue;
uniform bool uHasDisplacementTexture;
uniform bool uHasNormalTexture;
uniform bool uHasAmbientTexture;
uniform bool uHasDiffuseTexture;
uniform bool uHasSpecularTexture;
uniform bool uHasShininessTexture;
uniform sampler2D uDisplacementTexture;
uniform sampler2D uNormalTexture;
uniform sampler2D uAmbientTexture;
uniform sampler2D uDiffuseTexture;
uniform sampler2D uSpecularTexture;
uniform sampler2D uShininessTexture;

in vec3 vPosition;
in vec3 vNormal;
in vec3 vTangent;
in vec3 vBitangent;
in vec2 vTexCoord;

out vec4 oColor;

vec3 getPosition() {
   return vPosition;
   if (uHasDisplacementTexture) {
      return vPosition + vec3(texture(uDisplacementTexture, vTexCoord));
   } else {
      return vPosition;
   }
}

vec3 getNormal() {
   if (uHasNormalTexture) {
      return vec3(texture(uNormalTexture, vTexCoord));
   } else {
      return vNormal;
   }
}

vec4 getAmbientColor() {
   if (uHasAmbientTexture) {
      return texture(uAmbientTexture, vTexCoord);
   } else {
      return uAmbientColor;
   }
}

vec4 getDiffuseColor() {
   if (uHasDiffuseTexture) {
      return texture(uDiffuseTexture, vTexCoord);
   } else {
      return uDiffuseColor;
   }
}

vec4 getSpecularColor() {
   if (uHasSpecularTexture) {
      return texture(uSpecularTexture, vTexCoord);
   } else {
      return uSpecularColor;
   }
}

float getShininessValue() {
   if (uHasShininessTexture) {
      return texture(uShininessTexture, vTexCoord).w;
   } else {
      return uShininessValue;
   }
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
   for (int i = 0; i < MAX_NUM_LIGHTS; ++i) {
      oColor += getColorForLight(position, normal, diffuseColor, specularColor,
       shininessValue, i);
   }
   oColor = clamp(oColor, 0.0, 1.0);
}
