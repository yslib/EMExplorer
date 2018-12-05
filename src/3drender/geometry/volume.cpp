#include "volume.h"

#include <cstring>              // included for std::memcpy
#define GLERROR(str)									\
	{													\
		GLenum err;										\
		while ((err = glGetError()) != GL_NO_ERROR)		\
		{												\
			std::cout<<err<<" "<<str<<std::endl;		\
		}												\
	}		


Volume::Volume(const void * data, size_t xSize, size_t ySize, size_t zSize, const VolumeFormat& fmt) :
	m_xSize(xSize)
	, m_ySize(ySize)
	, m_zSize(zSize)
	, m_fmt(fmt)
	, m_data(nullptr)
	, m_isoStat(nullptr)
{
	auto voxelChannel = 0;
	switch (m_fmt.fmt)
	{
	case VoxelFormat::Grayscale:voxelChannel = 1; break;
	case VoxelFormat::RGB:voxelChannel = 3; break;
	case VoxelFormat::RGBA:voxelChannel = 4; break;
	}
	size_t bytes = 0;

	switch (m_fmt.type)
	{
	case VoxelType::UInt8:
		m_data.reset(reinterpret_cast<unsigned char*>(new unsigned char[xSize*ySize*zSize*voxelChannel]));
		m_isoStat.reset(new double[256]);
		bytes = xSize * ySize*zSize * sizeof(unsigned char)*voxelChannel;
		break;
	case VoxelType::Float32:
		m_data.reset(reinterpret_cast<unsigned char*>(new float[xSize*ySize*zSize*voxelChannel]));
		m_isoStat.reset(new double[256]);
		bytes = xSize * ySize * zSize * sizeof(float)*voxelChannel;
		break;
	}

	if (m_data != nullptr)
	{
		std::memcpy(m_data.get(), data, bytes);
	}

	if (m_isoStat != nullptr)
		calcIsoStat();

}

/**
 * \brief  Blends the \a data with current data of the value
 *
 * \note Only \a sourceVoxelType with \a VoxelType::UInt8 and \a sourceVoxelFormat with VoxelFormat::Grayscale are accepted so far.
 */
void Volume::blend(
	int xpos,
	int ypos,
	int zpos,
	void * data,
	size_t xlen,
	size_t ylen,
	size_t zlen,
	VolumeFormat sourceVolumeFormat)
{
	const size_t width = m_xSize;
	const size_t height = m_ySize;
	const size_t slice = m_zSize;

	Q_ASSERT_X(xlen <= width, "Volume::blend", "out of range");
	Q_ASSERT_X(ylen <= height, "Volume::blend", "out of range");
	Q_ASSERT_X(zlen <= slice, "Volume::blend", "out of range");

	if (sourceVolumeFormat .fmt!= VoxelFormat::Grayscale || sourceVolumeFormat.type != VoxelType::UInt8)
		return;

	const auto p = static_cast<unsigned char*>(data);

#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (auto z = 0; z < zlen; z++) {
		for (auto y = 0; y < ylen; y++) {
			for (auto x = 0; x < xlen; x++) {
				const auto index = (x + xpos) + (y + ypos)*width + (z + zpos)*width * height;
				const auto sourceIndex = x + y * xlen + z * xlen * ylen;
				const auto d = p[sourceIndex];
				if(d) 
				{
					m_data[index] = d;
				}
			}
		}
	}

}

void Volume::calcIsoStat() {
	memset(m_isoStat.get(), 0, sizeof(double) * 256);

	//for(int i = 0; i < m_zSize; ++i) {
	//	for(int j = 0; j < m_ySize; ++j) {
	//		for(int k = 0; k < m_xSize; ++k) {
	//			int index = i * m_xSize * m_ySize + j * m_xSize + k;
	//			int value = m_data.get()[index];
	//			isoStat[value] += 1.0;
	//		}
	//	}
	//}
	// We don't support histogram for float type volume data
	if (m_fmt.type == VoxelType::Float32)
		return;

#pragma omp parallel for
	for (int i = 0; i < m_zSize; ++i) {
		const auto zNext = (i < m_zSize - 1) ? m_xSize * m_ySize : 0;
		for (int j = 0; j < m_ySize; ++j) {
			const auto yNext = (j < m_ySize - 1) ? m_xSize : 0;
			for (int k = 0; k < m_xSize; ++k) {
				const auto index = i * m_xSize * m_ySize + j * m_xSize + k;
				const int xNext = (k < m_xSize - 1) ? 1 : 0;
				const int f000 = m_data.get()[index];
				const int f100 = m_data.get()[index + xNext];
				const int f010 = m_data.get()[index + yNext];
				const int f001 = m_data.get()[index + zNext];
				const int f101 = m_data.get()[index + xNext + zNext];
				const int f011 = m_data.get()[index + yNext + zNext];
				const int f110 = m_data.get()[index + xNext + yNext];
				const int f111 = m_data.get()[index + xNext + yNext + zNext];
				const int minValue = trippleMin(f000, f100, trippleMin(f010, f001, trippleMin(f101, f011, trippleMin(f110, f111, 255))));
				const int maxValue = trippleMax(f000, f100, trippleMax(f010, f001, trippleMax(f101, f011, trippleMax(f110, f111, 0))));
				for (auto m = minValue; m <= maxValue; ++m) {
					m_isoStat[m] += 1.0;
				}
			}
		}
	}
	//qDebug() << "??";
	m_maxIsoValue = m_isoStat[1];
	for (int i = 2; i < 256; ++i)
		m_maxIsoValue = std::max(m_maxIsoValue, m_isoStat[i]);
}

GPUVolume::GPUVolume(const void * data, int xSize, int ySize, int zSize, const ysl::Transform & trans, const VolumeFormat& fmt)
	:Volume(data, xSize, ySize, zSize, fmt)
{
	setTransform(trans);
}


