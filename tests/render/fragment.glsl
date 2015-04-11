#version 410

// FRAGMENT SHADER

struct Light {
   vec3 position;
   vec4 diffuse;
   vec4 specular;
};

uniform vec3 uCameraPosition;

uniform Light uLights[2];

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
      vec3 L = normalize(uLights[i].position - vPosition);
      vec3 R = normalize(-reflect(L, vNormal));
      vec3 V = normalize(vPosition - uCameraPosition);

      float nDotL = max(dot(vNormal, L), 0.0);
      float rDotV = max(dot(R, V), 0.0);

      vec4 diffuse = diffuseColor * nDotL;
      vec4 specular = specularColor * pow(rDotV, reflectivity);

      diffuse.x *= uLights[i].diffuse.x;
      diffuse.y *= uLights[i].diffuse.y;
      diffuse.z *= uLights[i].diffuse.z;
      diffuse.w *= uLights[i].diffuse.w;

      specular.x *= uLights[i].specular.x;
      specular.y *= uLights[i].specular.y;
      specular.z *= uLights[i].specular.z;
      specular.w *= uLights[i].specular.w;

      diffuse = clamp(diffuse, 0.0, 1.0);
      specular = clamp(specular, 0.0, 1.0);

      oColor += clamp(diffuse + specular, 0.0, 1.0);
   }
}
