#version 410

// FRAGMENT SHADER

uniform vec3 uLightPosition[8];
uniform vec4 uLightDiffuse[8];
uniform vec4 uLightSpecular[8];

uniform vec3 uCameraPosition;

uniform vec4 uDefaultAmbientColor = vec4(vec3(0.4), 1.0);
uniform vec4 uDefaultDiffuseColor = vec4(vec3(0.7), 1.0);
uniform vec4 uDefaultSpecularColor = vec4(vec3(0.9), 1.0);
uniform float uDefaultSpecularReflectivity = 250.0;

in vec3 vPosition;
in vec3 vNormal;
in vec3 vTangent;
in vec3 vBitangent;
in vec2 vTexCoord;

out vec4 oColor;

void main() {
   vec4 ambientColor;
   vec4 diffuseColor;
   vec4 specularColor;
   float reflectivity;

   ambientColor = uDefaultAmbientColor;
   diffuseColor = uDefaultDiffuseColor;
   specularColor = uDefaultSpecularColor;
   reflectivity = uDefaultSpecularReflectivity;

   oColor = ambientColor;
   for (int i = 0; i < 2; ++i) {
      vec3 L = normalize(uLightPosition[i] - vPosition);
      vec3 R = normalize(-reflect(L, vNormal));
      vec3 V = normalize(vPosition - uCameraPosition);

      float nDotL = max(dot(vNormal, L), 0.0);
      float rDotV = max(dot(R, V), 0.0);

      vec4 diffuse = diffuseColor * nDotL;
      vec4 specular = specularColor * pow(rDotV, reflectivity);

      diffuse.x *= uLightDiffuse[i].x;
      diffuse.y *= uLightDiffuse[i].y;
      diffuse.z *= uLightDiffuse[i].z;
      diffuse.w *= uLightDiffuse[i].w;

      specular.x *= uLightSpecular[i].x;
      specular.y *= uLightSpecular[i].y;
      specular.z *= uLightSpecular[i].z;
      specular.w *= uLightSpecular[i].w;

      diffuse = clamp(diffuse, 0.0, 1.0);
      specular = clamp(specular, 0.0, 1.0);

      oColor += clamp(diffuse + specular, 0.0, 1.0);
   }
}
