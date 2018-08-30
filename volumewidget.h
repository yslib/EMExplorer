#ifndef VOLUMEWIDGET_H
#define VOLUMEWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_1>

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



//#define USE_QT_FRAMEBUFFEROBJECT


//#define USE_QT_TEXTUREOBJECT

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
public:
	GradientCalculator(const AbstractSliceDataModel * slice, const MarkModel * mark, QObject * parent = nullptr);
	void setDataModel(const AbstractSliceDataModel * slice);
	void setMarkModel(const MarkModel * mark);
	bool ready()const;
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
	unsigned int	getVolumeTexIdx()const override
	{
		//qDebug() << "volume tex id:" << m_volumeTexture->textureId();
#ifdef USE_QT_TEXTUREOBJECT
		return m_volumeTexture->textureId();
#else
		return m_volumeTextureId;
#endif
	}
	//unsigned int	getMagnitudeTexIdx()override						{ return -1; }
	// voxel size
	QVector3D		getVoxelSize()const override								{ return m_voxelSize; }
	// ray casting start and end position texture idx
	unsigned int	getStartPosTexIdx()const override
	{
#ifdef USE_QT_FRAMEBUFFEROBJECT
		const auto & v = m_fbo->textures();
		return v[0];
#else
		return m_startFaceTextureId;
#endif
	}
	unsigned int	getEndPosTexIdx()const override
	{
#ifdef USE_QT_FRAMEBUFFEROBJECT
		const auto & v = m_fbo->textures();
		return v[1];
#else
		return m_endFaceTextureId;
#endif
	}
	// ray casting step
	float			getRayStep()const override									{ return m_rayStep; }
	// transfer function idx
	unsigned int	getTF1DIdx()const override
	{
		qDebug() <<"TF tex id:"<< m_tfTexture->textureId();
		return m_tfTexture->textureId();
	}
	// camera parameters
	QVector3D		getCameraEye()const override								{ return m_camera.position(); }
	QVector3D		getCameraTowards()const override							{ return m_camera.front(); }
	QVector3D		getCameraUp()const override									{ return m_camera.up(); }
	QVector3D		getCameraRight()const override								{ return m_camera.right(); }
	// matrix
	QMatrix4x4		getViewMatrix()const override									{ return m_camera.view(); }
	QMatrix4x4		getModelMatrix()const override								{ return m_world; }
	QMatrix4x4		getOthoMatrix()const override									{ return m_otho; }
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
	QVector3D		getVolumeBound()override									{ return m_volumeBound; }
	
protected:
	void			initializeGL() Q_DECL_OVERRIDE;
	void			resizeGL(int w, int h) Q_DECL_OVERRIDE;
	void			paintGL() Q_DECL_OVERRIDE;
	void			mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void			mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
signals:
	void			markModelChanged();
	void			dataModelChanged();
public slots:
	void updateTransferFunction(const float* func);
private:
	using ShaderHash = QHash<QString, QSharedPointer<ShaderProgram>>;
	void			updateVolumeData();
	void			updateMarkData();
	void			initializeShaders();
	void			initiliazeTextures();
	void			loadDataToTextures();

	void			cleanup();

	MarkModel								*m_markModel;
	AbstractSliceDataModel					*m_dataModel;

	QMatrix4x4								m_proj;
	QMatrix4x4								m_otho;
	QMatrix4x4								m_world;
	FocusCamera								m_camera;		//view matrix in this
	LightingParameters						m_lightParameters;
#ifdef USE_QT_FRAMEBUFFEROBJECT
	QScopedPointer<QOpenGLFramebufferObject>m_fbo;
#else
	//QScopedPointer<FramebufferObject>		m_FBO;
	unsigned int							m_framebuffer;
	unsigned int							m_startFaceTextureId;
	unsigned int							m_endFaceTextureId;
	unsigned int							m_depthTextureId;
#endif
	//ShaderHash								m_shaders;
	QSharedPointer<ShaderProgram>			m_currentShader;
	QOpenGLVertexArrayObject				m_rayCastingTextureVAO;
	QOpenGLBuffer							m_rayCastingTextureVBO;

	QScopedPointer<ShaderProgram>			m_positionShader;
	QOpenGLBuffer							m_positionVBO;
	QOpenGLBuffer							m_positionEBO;
	QOpenGLVertexArrayObject				m_positionVAO;
	
	QVector3D								m_voxelSize;
	QVector3D								m_volumeBound;
	float									m_rayStep;
	//unsigned int							m_tfTextureId;
	QScopedPointer<QOpenGLTexture>			m_tfTexture;
#ifdef USE_QT_TEXTUREOBJECT
	QScopedPointer<QOpenGLTexture>			m_volumeTexture;
#else
	unsigned int							m_volumeTextureId;
#endif
	//unsigned int							m_gradientTextureId;
	int										m_projMatLoc;
	int										m_modelViewMatLoc;
	int										m_normalMatrixLoc;
	int										m_lightPosLoc;
	QPoint									m_lastPos;
	GradientCalculator						m_gradCalc;

	//for test cube

};


#endif // VOLUMEWIDGET_H