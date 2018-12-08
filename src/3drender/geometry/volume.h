#ifndef VOLUME_H
#define VOLUME_H

#include <memory>
#include <fstream>
//#include "base/transformation.h"
#include "mathematics/geometry.h"
#include "mathematics/transformation.h"
#include <vector>


struct VolumeFormat;


class RenderableObject {
	ysl::Transform m_trans;
public:
	virtual bool render() = 0;

	void setTransform(const ysl::Transform& trans);

	ysl::Transform transform() const;

	virtual ~RenderableObject() = 0;
};

class GPURenderObject:public RenderableObject 
{
public:
	virtual bool initializeGLResources() = 0;
	virtual void destoryGLResources() = 0;
};


// Enum Type
enum VoxelType { UInt8, Float32 };
enum VoxelFormat { Grayscale, RGB, RGBA };
struct VolumeFormat 
{
	VoxelFormat fmt;
	VoxelType type;
	VolumeFormat() :fmt(Grayscale), type(UInt8) {}
};
class VolumeDataReader
{
	std::ifstream m_fileHandle;
	std::unique_ptr<char[]> m_buffer;
	std::vector<void> m_pool;
public:
	VolumeDataReader(const std::string & fileName,const VolumeFormat & fmt);

	VolumeDataReader(const VolumeDataReader & reader) = delete;

	VolumeDataReader & operator=(const VolumeDataReader & reader) = delete;

	VolumeDataReader(VolumeDataReader && reader)noexcept
	{
		m_fileHandle = std::move(reader.m_fileHandle);
		m_buffer = std::move(reader.m_buffer);
	}
	VolumeDataReader & operator=( VolumeDataReader && reader)noexcept
	{
		m_fileHandle = std::move(reader.m_fileHandle);
		m_buffer = std::move(reader.m_buffer);
		return *this;
	}

	void * wholeData();
	ysl::Vector3i getDimension()const;
	VolumeFormat dataFormat()const;
	void * subData(const ysl::Point3i & min, const ysl::Point3i & max);

	double sampleAt(const ysl::Point3f & pos) const;
};


// Volume Class

template<typename T>
const T & trippleMax(const T& t1, const T &t2, const T & t3) { return std::max(t1, std::max(t2, t3)); }
template<typename T>
const T & trippleMin(const T& t1, const T &t2, const T & t3) { return std::min(t1, std::min(t2, t3)); }

class Volume
{
	VolumeFormat m_fmt;
	std::unique_ptr<unsigned char[]> m_data;
	std::unique_ptr<double[]> m_isoStat;
	int m_xSize, m_ySize, m_zSize;
	double m_maxIsoValue;
public:
	Volume(const void * data, size_t xSize, size_t ySize, size_t zSize, const VolumeFormat & fmt = VolumeFormat());
	int xLength()const;
	int yLength()const;
	int zLength()const;
	/**
	 * \brief  Returns a histogram of this volume data. The histogram is made of 256 bins.
	 */
	double * isoStat()const { return m_isoStat.get(); }
	double maxIsoValue()const { return m_maxIsoValue; }
	const void * data()const;
	const VolumeFormat & format()const;
	void blend(int xpos, int ypos, int zpos,void * data, size_t xlen, size_t ylen, size_t zlen,VolumeFormat sourceVolumeFormat);
	virtual ~Volume();
private:
	void calcIsoStat();
};

inline int Volume::xLength() const { return m_xSize; }
inline int Volume::yLength() const { return m_ySize; }
inline int Volume::zLength() const { return m_zSize; }
inline const void * Volume::data() const { return m_data.get(); }
inline const VolumeFormat & Volume::format()const { return m_fmt; }
inline Volume::~Volume() {}



class GPUVolume :public Volume
{
	ysl::Transform m_trans;
public:
	GPUVolume() :Volume(nullptr, 0, 0, 0) {}

	GPUVolume(const void * data, int xSize, int ySize, int zSize,const ysl::Transform & trans,const VolumeFormat & fmt = VolumeFormat());

	virtual bool initializeGLResources() = 0;
	virtual void destroyGLResources() = 0;
	void setTransform(const ysl::Transform& trans);
	ysl::Transform transform() const;
	virtual bool render()=0;
	virtual ~GPUVolume(){}
};

inline void GPUVolume::setTransform(const ysl::Transform& trans) {m_trans = trans;}
inline ysl::Transform GPUVolume::transform() const { return  m_trans; }
#endif // VOLUME_H