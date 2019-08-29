#version 330
layout(location = 0)in vec3 vertex;
layout(location = 1)in vec3 normal;

out vec3 outNormal;
out vec3 fragPos;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

void main(){
	outNormal = normalMatrix*normal;	
	fragPos = vec3(modelMatrix*vec4(vertex,1.0));		// view space
	gl_Position = projMatrix*viewMatrix*modelMatrix*vec4(vertex,1.0);
}