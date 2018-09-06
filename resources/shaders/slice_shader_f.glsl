#version 330
in vec2 texCoord;
out vec4 fragColor;
uniform sampler3D volume;
uniform int sliceType;
uniform int sliceIndex;
uniform int sliceCount;
void main() {
	vec3 samplerPoint;
	float coord = float(sliceIndex) /float(sliceCount);
	if (sliceType == 0) {
		samplerPoint = vec3(texCoord.x, texCoord.y, coord);
	}else if (sliceType == 1) {
		samplerPoint = vec3(coord, texCoord.x, texCoord.y);
	}else if (sliceType == 2) {
		samplerPoint = vec3(texCoord.x, coord, texCoord.y);
	}else{
		fragColor = vec4(0.5, 0.7, 0.3, 1.0);
		return;
	}
	float scalar = texture(volume, samplerPoint).r;
	fragColor = vec4(scalar,scalar,scalar, 1.0);
}