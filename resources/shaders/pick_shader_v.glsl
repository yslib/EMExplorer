#version 330
layout(location = 0)in vec3 vertex;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 modelMatrix;

void main(){
	gl_Position = projMatrix*viewMatrix*modelMatrix*vec4(vertex,1.0);
}