#version 400

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;

uniform mat4 modelViewProjectionMatrix;
uniform mat3 normalMatrix;

out vec3 normal;
out vec3 eyeDir;

void main(){
  normal = normalize(normalMatrix * vertexNormal);
  eyeDir =normalize(normalMatrix * vertexPosition);
  gl_Position = modelViewProjectionMatrix * vec4(vertexPosition, 1.0);
}
