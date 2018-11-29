#ifndef VOLUME_H
#define VOLUME_H

#include <memory>
#include <QMatrix4x4>


struct VolumeFormat;


class RenderableObject {
	QMatrix4x4 m_trans;
public:
	virtual bool render() = 0;
	void setTransform(const QMatrix4x4 & trans);
	QMatrix4x4 transform()const;
	virtual ~RenderableObject() = 0;
};

class GPURenderObject:public RenderableObject {
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

	double * isoStat()const { return m_isoStat.get(); }
	double maxIsoValue()const { return m_maxIsoValue; }

	const void * data()const;
	const VolumeFormat & format()const;
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
	QMatrix4x4 m_trans;
public:
	GPUVolume() :Volume(nullptr, 0, 0, 0) { m_trans.setToIdentity(); }

	GPUVolume(const void * data, int xSize, int ySize, int zSize,const QMatrix4x4 &trans,const VolumeFormat & fmt = VolumeFormat());

	virtual bool initializeGLResources() = 0;
	virtual void destroyGLResources() = 0;
	void setTransform(const QMatrix4x4 & trans);
	QMatrix4x4 transform()const;
	virtual bool render()=0;
	virtual ~GPUVolume(){}
};

inline void GPUVolume::setTransform(const QMatrix4x4& trans) {m_trans = trans;}
inline QMatrix4x4 GPUVolume::transform() const { return  m_trans; }
#endif // VOLUME_H