#version 400

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 texCoord;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
out vec3 Normal;
out vec4 eyeDirection;
out vec3 position;
out vec3 lightDir;

void main(){
  Normal = vertexNormal;
  eyeDirection =vec4(vertexPosition, 1.0);
  position = vec3(modelViewMatrix * vec4(vertexPosition, 1.0));
  gl_Position = modelViewProjectionMatrix * vec4(vertexPosition, 1.0);
}
