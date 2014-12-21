#version 400

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 texCoord;

struct lights{
  vec3 direction;
  vec3 colour;
};

uniform lights light1;
uniform lights light2;

out vec3 normal;
out vec3 eyeDir;
out vec3 lightDir;

uniform vec3 lightDirection;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
uniform mat4 modelViewProjectionMatrix;

void main (){
  lightDir =normalize(lightDirection);
  eyeDir = normalize(normalMatrix * vertexPosition);
  normal = normalize(normalMatrix * vertexNormal);
  //normal = normalize(vertexNormal);
  gl_Position = modelViewProjectionMatrix * vec4(vertexPosition, 1.0);
}
