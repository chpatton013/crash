#version 410

// FRAGMENT SHADER

const uint MAX_NUM_LIGHTS = 8;

uniform uint uLightCount;
uniform vec3 uLightPosition[MAX_NUM_LIGHTS];
uniform vec4 uLightDiffuse[MAX_NUM_LIGHTS];

uniform vec4 uAmbientColor;
uniform vec4 uDiffuseColor;

uniform vec4 uAmbientBaseColor;
uniform vec4 uDiffuseBaseColor;

uniform bool uHasAmbientTexture;
uniform bool uHasDiffuseTexture;

uniform sampler2D uAmbientTexture;
uniform sampler2D uDiffuseTexture;

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;

out vec4 oColor;

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

vec4 getColorForLight(vec3 P, vec3 N, vec4 kD, int i) {
   vec3 lP = uLightPosition[i];
   vec4 lD = uLightDiffuse[i];

   vec3 L = normalize(lP - P);
   float nDotL = max(dot(N, L), 0.0);

   return clamp(kD * nDotL * lD, 0.0, 1.0);
}

void main() {
   vec4 ambientColor = getAmbientColor();
   vec4 diffuseColor = getDiffuseColor();

   oColor = ambientColor;
   for (int i = 0; i < uLightCount; ++i) {
      oColor += getColorForLight(vPosition, vNormal, diffuseColor, i);
   }
   oColor = clamp(oColor, 0.0, 1.0);
}
