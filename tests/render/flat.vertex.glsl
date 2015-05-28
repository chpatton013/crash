#version 410

// VERTEX SHADER

uniform mat4 uModelTransform;
uniform mat4 uViewTransform;
uniform mat4 uPerspectiveTransform;

in vec3 aPosition;
in vec3 aNormal;
in vec2 aTexCoord;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;

void main() {
   mat4 mvTransform = uViewTransform * uModelTransform;
   mat4 normalMvTransform = transpose(inverse(mvTransform));

   vPosition = vec3(mvTransform * vec4(aPosition, 1.0));
   vNormal = normalize(vec3(normalMvTransform * vec4(aNormal, 0.0)));
   vTexCoord = aTexCoord;

   mat4 mvpTransform = uPerspectiveTransform * mvTransform;
   gl_Position = mvpTransform * vec4(aPosition, 1.0);
}
