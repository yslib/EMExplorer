#ifndef SLICEVOLUME_H
#define SLICEVOLUME_H

#include "volume.h"
#include "3drender/shader/shaderdatainterface.h"
#include "3drender/shader/raycastingshader.h"
#include "algorithm/gradientcalculator.h"
#include "widgets/renderwidget.h"

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>

// Forward Declaration
class AbstractSliceDataModel;
class RenderWidget;
enum class RenderType;

class SliceVolume :public QObject, public GPUVolume, public ShaderDataInterface
{
public:
	enum RenderType {
		DVR,
		Slice,
		Modulo
	};

private:

	Q_OBJECT
	
	QOpenGLBuffer							m_positionVBO;
	QOpenGLBuffer							m_positionEBO;
	QOpenGLVertexArrayObject				m_positionVAO;

	/*
	 * Prefer raw pointer rather than smart pointer to manage following qt OpenGL helper classes.
	 * Because these class need to explicitly initialize/destroy so as to deferred instance.
	 */
	PositionShader							*m_positionShader;
	SliceShader								*m_sliceShader;
	ShaderProgram							*m_currentShader;
	
	QOpenGLFramebufferObject				*m_fbo;
	//QOpenGLTexture							*m_gradientTexture;
	QOpenGLTexture							*m_volumeTexture;

	QOpenGLBuffer							m_axisAlignedSliceVBO;
	QOpenGLVertexArrayObject				m_axisAlignedSliceVAO;
	QOpenGLVertexArrayObject				m_rayCastingTextureVAO;
	QOpenGLBuffer							m_rayCastingTextureVBO;

	//GradientCalculator						m_gradCalc;
	QHash<RenderType, ShaderProgram*>		m_shaders;



	int										m_topSlice;
	int										m_rightSlice;
	int										m_frontSlice;
	QMatrix4x4								m_normalizeTransform;

	double									m_A, m_B, m_C, m_D;


	bool									m_initialized;
	RenderType								m_renderType;

	bool									m_frontSliceVisible;
	bool									m_rightSliceVisible;
	bool									m_topSliceVisible;
	void loadDataAndGradientToTexture();
	//const AbstractSliceDataModel*			m_dataModel;
	RenderWidget							*m_renderer;

public://ShaderDataInterface
	unsigned int volumeTexId() const override;
	QVector3D voxelSize() const override;
	unsigned int startPosTexIdx() const override;
	unsigned int endPosTexIdx() const override;
	unsigned int gradientTexId() const override;
	float rayStep() const override;
	unsigned int transferFunctionsTexId() const override;
	QVector3D cameraPos() const override;
	QVector3D cameraTowards() const override;
	QVector3D cameraUp() const override;
	QVector3D cameraRight() const override;
	QMatrix4x4 viewMatrix() const override;
	QMatrix4x4 worldMatrix() const override;
	QMatrix4x4 othoMatrix() const override;
	QMatrix4x4 perspMatrix() const override;
	QVector3D lightDirection() const override;
	float ambient() const override;
	float diffuse() const override;
	float shininess() const override;
	float specular() const override;
	QVector3D volumeBound() const override;
	QSize windowSize() const override;
public:


	SliceVolume(const void * data,int x,int y,int z, const QMatrix4x4 & trans,
		const VolumeFormat & fmt = VolumeFormat(),
		RenderWidget * renderer = nullptr);

	void setRenderWidget(RenderWidget * widget);
	bool initializeGLResources() override;
	void destroyGLResources() override;
	bool render()override;

	//void sliceMode(bool enable);
	//bool isSliceMode()const { return m_sliceMode; }

	void setRenderType(RenderType type) 
	{
		m_renderType = type; 
	}
	RenderType renderType()const { return m_renderType; }


	void setSliceSphereCoord(const QVector3D & coord);

	void setFrontSliceVisible(bool check) { m_frontSliceVisible = check; }
	void setRightSliceVisible(bool check) { m_rightSliceVisible = check; }
	void setTopSliceVisible(bool check)	  { m_topSliceVisible = check; }

	~SliceVolume();

private slots:
	void setFramebufferSize(int w, int h);

private:
	static QVector<QVector3D> sliceCoord(double A,double B,double C,double D);
	static bool isInRange(double v) { return v >= 0 && v <= 1; }
	static int getSign(double v) { if (v >= 0)return 1;  if (v < 0)return -1; }
	static double clamp(double v, double a, double b) { if (v < a)return a; if (v > b)return b; return v; }
};

//inline void SliceVolume::sliceMode(bool enable) { m_sliceMode = enable; }

inline void SliceVolume::setSliceSphereCoord(const QVector3D & coord)
{
	const auto r = coord.x();
	const auto theta = coord.y();
	const auto phi = coord.z();
	const auto sinPhi = std::sin(qDegreesToRadians(phi));
	const auto x = r * sinPhi * std::sin(qDegreesToRadians(theta)) + 0.5;
	const auto y = r* sinPhi * std::cos(qDegreesToRadians(theta))+0.5;
	const auto z = r * std::cos(qDegreesToRadians(phi))+0.5;
	m_A = x-0.5;
	m_B = y -0.5;
	m_C = z -0.5;
	m_D = -x * (m_A) - y * ( m_B) - z * (m_C);
}


#endif // SLICEVOLUME_H