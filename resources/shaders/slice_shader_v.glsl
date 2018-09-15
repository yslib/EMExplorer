#version 330
layout(location = 0) in vec4 vertex;
out vec3 pos;
uniform mat4 projMatrix;
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
void main() {
   pos = vertex.xyz;
   gl_Position = projMatrix * viewMatrix* worldMatrix * vertex;
};