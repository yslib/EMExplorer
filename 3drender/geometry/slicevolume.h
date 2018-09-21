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


class SliceVolume :public QObject, public GPUVolume, public ShaderDataInterface
{
	Q_OBJECT
	QScopedPointer<PositionShader>			m_positionShader;
	QOpenGLBuffer							m_positionVBO;
	QOpenGLBuffer							m_positionEBO;
	QOpenGLVertexArrayObject				m_positionVAO;
	QScopedPointer<QOpenGLFramebufferObject>m_fbo;
	QSharedPointer<RayCastingShader>		m_currentShader;
	QOpenGLVertexArrayObject				m_rayCastingTextureVAO;
	QOpenGLBuffer							m_rayCastingTextureVBO;
	QScopedPointer<SliceShader>			    m_sliceShader;
	QOpenGLBuffer							m_axisAlignedSliceVBO;
	QOpenGLVertexArrayObject				m_axisAlignedSliceVAO;
	QOpenGLTexture							m_gradientTexture;
	QOpenGLTexture							m_volumeTexture;
	GradientCalculator						m_gradCalc;
	int										m_topSlice;
	int										m_rightSlice;
	int										m_frontSlice;
	RenderWidget							*m_renderer;
	bool									m_sliceMode;
	void loadDataAndGradientToTexture();
	const AbstractSliceDataModel*					m_dataModel;

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
	SliceVolume(const AbstractSliceDataModel * data, const QMatrix4x4 & trans,
		const VolumeFormat & fmt = VolumeFormat(),
		RenderWidget * renderer = nullptr);

	void setRenderWidget(RenderWidget * widget);
	bool initializeGLResources() override;
	void destroyGLResources() override;

	bool render()override;
	void sliceMode(bool enable);
	private slots:
	void windowSizeChanged(int w, int h);
};

inline void SliceVolume::sliceMode(bool enable) { m_sliceMode = enable; }



#endif // SLICEVOLUME_H