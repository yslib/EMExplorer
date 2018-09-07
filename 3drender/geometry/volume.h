#ifndef VOLUME_H
#define VOLUME_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions_3_1>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFramebufferObject>

#include <memory>
#include <QScopedPointer>
#include "3drender/shader/raycastingshader.h"
#include <QOpenGLTexture>
#include "algorithm/gradientcalculator.h"


struct VolumeFormat;
class ShaderProgram;


enum VoxelType { UInt8, Float32 };
enum VoxelFormat { Grayscale, RGB, RGBA };

struct VolumeFormat {
	VoxelFormat fmt;
	VoxelType type;
	VolumeFormat() :fmt(Grayscale), type(UInt8) {}
};

;

class Volume
{
	VolumeFormat m_fmt;
	std::unique_ptr<void> m_data;
	int m_xSize, m_ySize, m_zSize;
public:
	Volume(const void * data, int xSize, int ySize, int zSize, const VolumeFormat & fmt = VolumeFormat());
	int xLength()const;
	int yLength()const;
	int zLength()const;
	const void * data()const;
	const VolumeFormat & format()const;
	virtual ~Volume();
};

inline int Volume::xLength() const { return m_xSize; }
inline int Volume::yLength() const { return m_ySize; }
inline int Volume::zLength() const { return m_zSize; }
inline const void * Volume::data() const { return m_data.get(); }
inline const VolumeFormat & Volume::format()const { return m_fmt; }
inline Volume::~Volume() {}




class GPUVolume :public Volume
{
public:
	GPUVolume(const void * data, int xSize, int ySize, int zSize, const VolumeFormat & fmt = VolumeFormat());
	virtual bool initializeGLResources() = 0;
	virtual void destoryGLResources();
	virtual bool render();
	virtual ~GPUVolume(){}
};


class SliceVolume:public GPUVolume,protected QOpenGLFunctions_3_1
{

	QScopedPointer<PositionShader>			m_positionShader;
	QOpenGLBuffer							m_positionVBO;
	QOpenGLBuffer							m_positionEBO;
	QOpenGLVertexArrayObject				m_positionVAO;
	QScopedPointer<QOpenGLFramebufferObject>m_fbo;

	QSharedPointer<RayCastingShader>		m_currentShader;
	QOpenGLVertexArrayObject				m_rayCastingTextureVAO;
	QOpenGLBuffer							m_rayCastingTextureVBO;

	QScopedPointer<SliceShader>			    m_sliceShader;
	QOpenGLBuffer							m_topSliceVBO;
	QOpenGLVertexArrayObject				m_topSliceVAO;
	QOpenGLBuffer							m_rightSliceVBO;
	QOpenGLVertexArrayObject				m_rightSliceVAO;
	QOpenGLBuffer							m_frontSliceVBO;
	QOpenGLVertexArrayObject				m_frontSliceVAO;

	QOpenGLTexture							m_gradientTexture;
	QOpenGLTexture							m_volumeTexture;

	GradientCalculator						m_gradCalc;

	int										m_topSlice;
	int										m_rightSlice;
	int										m_frontSlice;
	void loadVolumeData();

public:
	SliceVolume(const void * data, int xSize, int ySize, int zSize, const VolumeFormat & fmt = VolumeFormat());
	bool initializeGLResources() override;
	void destoryGLResources() override;
	bool render()override;

};


#endif // VOLUME_H