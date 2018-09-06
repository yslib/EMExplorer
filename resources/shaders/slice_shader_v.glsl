#version 330
layout(location = 0) in vec4 vertex;
layout(location = 1) in vec2 tex;
out vec2 texCoord;
uniform mat4 projMatrix;
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
void main() {
   texCoord = tex;
   gl_Position = projMatrix * viewMatrix* worldMatrix * vertex;
};