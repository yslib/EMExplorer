#include "volume.h"
#define GLERROR(str)									\
	{													\
		GLenum err;										\
		while ((err = glGetError()) != GL_NO_ERROR)		\
		{												\
			std::cout<<err<<" "<<str<<std::endl;		\
		}												\
	}		


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

GPUVolume::GPUVolume(const void* data, int xSize, int ySize, int zSize,const QMatrix4x4 & trans ,const VolumeFormat& fmt) 
:Volume(data, xSize, ySize, zSize) 
{
	setTransform(trans);
}


