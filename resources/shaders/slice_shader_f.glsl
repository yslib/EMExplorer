#version 330
out vec4 fragColor;
uniform sampler3D volume;
in vec3 pos;
void main() {
	float scalar = texture(volume, pos).r;
	fragColor = vec4(scalar,scalar,scalar, 1.0);
}