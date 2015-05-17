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

uniform vec4 uAmbientBaseColor;
uniform vec4 uDiffuseBaseColor;
uniform vec4 uSpecularBaseColor;
uniform float uShininessBaseValue;

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
   vec4 ambient;
   if (uHasAmbientTexture) {
      ambient = texture(uAmbientTexture, vTexCoord);
   } else {
      ambient = uAmbientColor;
   }

   return ambient * uAmbientBaseColor;
}

vec4 getDiffuseColor() {
   vec4 diffuse;
   if (uHasDiffuseTexture) {
      diffuse = texture(uDiffuseTexture, vTexCoord);
   } else {
      diffuse = uDiffuseColor;
   }

   return diffuse * uDiffuseBaseColor;
}

vec4 getSpecularColor() {
   vec4 specular;
   if (uHasSpecularTexture) {
      specular = texture(uSpecularTexture, vTexCoord);
   } else {
      specular = uSpecularColor;
   }

   return specular * uSpecularBaseColor;
}

float getShininessValue() {
   float shininess;
   if (uHasShininessTexture) {
      shininess = texture(uShininessTexture, vTexCoord).w;
   } else {
      shininess = uShininessValue;
   }

   return shininess * uShininessBaseValue;
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
