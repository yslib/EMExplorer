#include "volume.h"
#define GLERROR(str)									\
	{													\
		GLenum err;										\
		while ((err = glGetError()) != GL_NO_ERROR)		\
		{												\
			std::cout<<err<<" "<<str<<std::endl;		\
		}												\
	}		
//static const char *vertexShaderSource =
//"#version 150\n"
//"#extension GL_ARB_explicit_attrib_location : enable\n"
//"layout(location = 0) in vec4 vertex;\n"
//"layout(location = 1) in vec3 normal;\n"
//"layout(location = 2) in vec2 tex;"
//"out vec2 texCoord;\n"
//"out vec3 vert;\n"
//"out vec3 vertNormal;\n"
//"uniform mat4 projMatrix;\n"
//"uniform mat4 mvMatrix;\n"
//"uniform mat3 normalMatrix;\n"
//"void main() {\n"
//"   vert = vertex.xyz;\n"
//"   vertNormal = normalMatrix * normal;\n"
//"	texCoord = tex;\n"
//"   gl_Position = projMatrix * mvMatrix * vertex;\n"
//"}\n";
//
//
//static const char *fragmentShaderSource =
//"#version 150\n"
//"in highp vec3 vert;\n"
//"in highp vec3 vertNormal;\n"
//"out highp vec4 fragColor;\n"
//"in vec2 texCoord;\n"
//"uniform highp vec3 lightPos;\n"
//"uniform sampler2D tex;\n"
//"void main() {\n"
//"   highp vec3 L = normalize(lightPos - vert);\n"
//"   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
//"   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"
//"   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
//"   fragColor = vec4(1.0,0.0,0.0,1.0);\n"
//"}\n";

Volume::Volume(const void* data, int xSize, int ySize, int zSize, const VolumeFormat& fmt):
	m_xSize(xSize)
	, m_ySize(ySize)
	, m_zSize(zSize)
	, m_fmt(fmt)
	, m_data(nullptr)
{
	int voxelChannel = 0;
	switch (m_fmt.fmt) {
	case VoxelFormat::Grayscale:voxelChannel = 1; break;
	case VoxelFormat::RGB:voxelChannel = 3; break;
	case VoxelFormat::RGBA:voxelChannel = 4; break;
	}
	size_t bytes = 0;
	switch (m_fmt.type) {
	case VoxelType::UInt8:
		m_data.reset(reinterpret_cast<unsigned char*>(new unsigned char[xSize*ySize*zSize*voxelChannel]));
		bytes = xSize * ySize*zSize * sizeof(unsigned char)*voxelChannel;
		break;
	case VoxelType::Float32:
		m_data.reset(reinterpret_cast<unsigned char*>(new float[xSize*ySize*zSize*voxelChannel]));
		bytes = xSize * ySize*zSize * sizeof(float)*voxelChannel;
		break;
	}
	if (m_data != nullptr) {
		std::memcpy(m_data.get(), data, bytes);
	}
}

GPUVolume::GPUVolume(const void* data, int xSize, int ySize, int zSize, const VolumeFormat& fmt) :Volume(data, xSize, ySize, zSize) {}


