#ifndef VOLUMEWIDGET_H
#define VOLUMEWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_1>
#include <QOpenGLFunctions_3_3_Compatibility>

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QScopedPointer>
#include <QOpenGLFramebufferObject>

#include "volume/camera.h"
#include "volume/Shaders/shaderdatainterface.h"
#include "volume/Shaders/shaderprogram.h"
#include <QMutex>
#include <memory>
#include <QOpenGLTexture>

class MarkModel;
class AbstractSliceDataModel;
class ShaderProgram;
class QOpenGLShaderProgram;





struct LightingParameters
{
	float ambient;
	float specular;
	float diffuse;
	float shininess;
	QVector3D lightDirection;
	LightingParameters():
	ambient(1.0)
	,specular(0.75)
	,diffuse(0.5)
	,shininess(40.00)
	,lightDirection(0,-1,0)
	{}
};

struct MeshRenderingParamters
{
	bool lineMode;
	bool antiAliasing;
};
class GradientCalculator :public QObject
{
	Q_OBJECT
	std::unique_ptr<unsigned char[]> m_gradient;
	const AbstractSliceDataModel * m_sliceModel;
	const MarkModel * m_mark;
	QMutex m_mutex;
	bool m_ready;
public:
	GradientCalculator(const AbstractSliceDataModel * slice, const MarkModel * mark, QObject * parent = nullptr);
	void setDataModel(const AbstractSliceDataModel * slice);
	void setMarkModel(const MarkModel * mark);
	bool ready()const;
	bool hasData()const;
	unsigned char * data()const;
private:
	void init();
	QVector3D triCubicIntpGrad(const unsigned char * pData, double px, double py, double pz);
	inline double value(const unsigned char* pData, double x, double y, double z) const;
	inline static double cubicIntpGrad(double v0, double v1, double v2, double v3, double mu);
	inline static double cubicIntpValue(double v0, double v1, double v2, double v3, double mu);
public slots:
	void calcGradent();
signals:
	void finished();
};

//#define TESTCUBE

class VolumeWidget:public QOpenGLWidget,
				   public ShaderDataInterface,
				   protected QOpenGLFunctions_3_1
{
	Q_OBJECT
public:
					VolumeWidget(AbstractSliceDataModel * dataModel, MarkModel * markModel, QWidget * parent = nullptr);
	void			setDataModel(AbstractSliceDataModel * model);
	void			setMarkModel(MarkModel * model);
	QSize			minimumSizeHint() const Q_DECL_OVERRIDE;
	QSize			sizeHint() const Q_DECL_OVERRIDE;
					~VolumeWidget();
public://ShaderDataInterface
	unsigned int	getVolumeTexIdx()const override								{return m_volumeTexture.textureId();}
	//unsigned int	getMagnitudeTexIdx()override						{ return -1; }
	// voxel size
	QVector3D		getVoxelSize()const override								{ return m_voxelSize; }
	// ray casting start and end position texture idx
	unsigned int	getStartPosTexIdx()const override							{return m_fbo->textures()[0];}
	unsigned int	getEndPosTexIdx()const override								{return m_fbo->textures()[1];}
	// ray casting step
	float			getRayStep()const override									{ return m_rayStep; }
	// transfer function idx
	unsigned int	getTF1DIdx()const override									{return m_tfTexture.textureId();}
	// camera parameters
	QVector3D		getCameraEye()const override								{ return m_camera.position(); }
	QVector3D		getCameraTowards()const override							{ return m_camera.front(); }
	QVector3D		getCameraUp()const override									{ return m_camera.up(); }
	QVector3D		getCameraRight()const override								{ return m_camera.right(); }
	// matrix
	QMatrix4x4		getViewMatrix()const override								{ return m_camera.view(); }
	QMatrix4x4		getModelMatrix()const override								{ return m_world; }
	QMatrix4x4		getOthoMatrix()const override								{ return m_otho; }
	QMatrix4x4		getPerspMatrix()const override								{ return m_proj; }
	// lighting parameters
	QVector3D		getLightDirection()const override							{ return m_lightParameters.lightDirection; }
	float			getAmbient()const override									{ return m_lightParameters.ambient; }
	float			getDiffuse()const override									{ return m_lightParameters.diffuse; }
	float			getShininess()const override								{ return m_lightParameters.shininess; }
	float			getSpecular()const override									{ return m_lightParameters.specular; }
	// mouse position
	//unsigned int	getMouseX()override									{ return -1; }
	//unsigned int	getMouseY()override									{ return -1; }
	// volume boundary
	QVector3D		getVolumeBound()const override									{ return m_volumeBound; }
	QSize			windowSize()const override										{ return size(); }
	
protected:
	void			initializeGL() Q_DECL_OVERRIDE;
	void			resizeGL(int w, int h) Q_DECL_OVERRIDE;
	void			paintGL() Q_DECL_OVERRIDE;
	void			mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void			mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
signals:
	void			markModelChanged();
	void			dataModelChanged();
	void			requireTransferFunction();
public slots:
	void updateTransferFunction(const float* func, bool updated);
private:
	using ShaderHash = QHash<QString, QSharedPointer<ShaderProgram>>;
	void			updateVolumeData();
	void			updateMarkData();
	void			initializeShaders();
	void			initiliazeTextures();
	void			loadDataToTextures();
	void			cubeInitilized();

	void			cleanup();

	MarkModel								*m_markModel;
	AbstractSliceDataModel					*m_dataModel;

	QMatrix4x4								m_proj;
	QMatrix4x4								m_otho;
	QMatrix4x4								m_world;
	FocusCamera								m_camera;		//view matrix in this
	QVector3D								m_scale;
	QVector3D								m_trans;
	QPoint									m_lastPos;
	QVector3D								m_voxelSize;
	QVector3D								m_volumeBound;
	float									m_rayStep;
	LightingParameters						m_lightParameters;
	// FrameBuffer Object
	QScopedPointer<QOpenGLFramebufferObject>m_fbo;
	// Ray casting
	QSharedPointer<ShaderProgram>			m_currentShader;
	QOpenGLVertexArrayObject				m_rayCastingTextureVAO;
	QOpenGLBuffer							m_rayCastingTextureVBO;
	// Front and back face
	QScopedPointer<ShaderProgram>			m_positionShader;
	QOpenGLBuffer							m_positionVBO;
	QOpenGLBuffer							m_positionEBO;
	QOpenGLVertexArrayObject				m_positionVAO;
	// Textures
	QOpenGLTexture							m_tfTexture;
	QOpenGLTexture							m_volumeTexture;
	// Gradient
	GradientCalculator						m_gradCalc;

	// Cube
	QScopedPointer<QOpenGLShaderProgram>    m_cubeShader;
	QOpenGLVertexArrayObject				m_cubeVAO;
	QOpenGLBuffer							m_cubeVBO;
};


#endif // VOLUMEWIDGET_H