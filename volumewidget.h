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
	QVector3D ligthDirection;
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
	unsigned int	getVolumeTexIdx()const override 							{ return m_volumeTextureId; }
	//unsigned int	getMagnitudeTexIdx()override						{ return -1; }
	// voxel size
	QVector3D		getVoxelSize()const override								{ return m_voxelSize; }
	// ray casting start and end position texture idx
	unsigned int	getStartPosTexIdx()const override							{ return m_startFaceTextureId; }
	unsigned int	getEndPosTexIdx()const override							{ return m_endFaceTextureId; }
	// ray casting step
	float			getRayStep()const override								{ return m_rayStep; }
	// transfer function idx
	unsigned int	getTF1DIdx()const override								{ return m_tfTextureId; }
	// camera parameters
	QVector3D		getCameraEye()const override								{ return m_camera.position(); }
	QVector3D		getCameraTowards()const override							{ return m_camera.front(); }
	QVector3D		getCameraUp()const override								{ return m_camera.up(); }
	QVector3D		getCameraRight()const override							{ return m_camera.right(); }
	// lighting parameters
	QVector3D		getLightDirection()const override							{ return m_lightParameters.ligthDirection; }
	float			getAmbient()const override								{ return m_lightParameters.ambient; }
	float			getDiffuse()const override								{ return m_lightParameters.diffuse; }
	float			getShininess()const override								{ return m_lightParameters.shininess; }
	float			getSpecular()const override								{ return m_lightParameters.specular; }
	// mouse position
	//unsigned int	getMouseX()override									{ return -1; }
	//unsigned int	getMouseY()override									{ return -1; }
	// volume boundary
	QVector3D		getVolumeBound()override							{ return m_volumeBound; }
	
protected:
	void			initializeGL() Q_DECL_OVERRIDE;
	void			resizeGL(int w, int h) Q_DECL_OVERRIDE;
	void			paintGL() Q_DECL_OVERRIDE;
	void			mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void			mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
signals:
	void			markModelChanged();
	void			dataModelChanged();
private slots:
	void updateTransferFunction(const float* func);
private:
	using ShaderHash = QHash<QString, QSharedPointer<ShaderProgram>>;
	void			drawCube();
	void			updateVolumeData();
	void			updateMarkData();
	void			initializeShaders();
	void			initiliazeTextures();
	void			loadDataToTextures();

	void			cleanup();

	MarkModel								*m_markModel;
	AbstractSliceDataModel					*m_dataModel;

	QMatrix4x4								m_proj;
	QMatrix4x4								m_world;
	FocusCamera								m_camera;
	LightingParameters						m_lightParameters;

	QScopedPointer<QOpenGLFramebufferObject>m_fbo;


	ShaderHash								m_shaders;
	QSharedPointer<ShaderProgram>			m_currentShader;

	QScopedPointer<QOpenGLShaderProgram>	m_positionShader;
	QOpenGLBuffer							m_positionVBO;
	QOpenGLBuffer							m_positionEBO;
	QOpenGLVertexArrayObject				m_positionVAO;
	

	QVector3D								m_voxelSize;
	QVector3D								m_volumeBound;
	float									m_rayStep;

	unsigned int							m_tfTextureId;
	//QOpenGLTexture						m_tfTexture;
	QScopedPointer<QOpenGLTexture>			m_tfTexture;
	unsigned int							m_volumeTextureId;
	QScopedPointer<QOpenGLTexture>			m_volumeTexture;
	unsigned int							m_gradientTextureId;
	QScopedPointer<QOpenGLTexture>			m_gradientTeture;


	QOpenGLTexture							*m_testTexture;

	unsigned int							m_startFaceTextureId;
	unsigned int							m_endFaceTextureId;

	int										m_projMatLoc;
	int										m_modelViewMatLoc;
	int										m_normalMatrixLoc;
	int										m_lightPosLoc;

	QPoint									m_lastPos;
	GradientCalculator						m_gradCalc;

	QScopedPointer<QOpenGLShaderProgram>	m_program;
	QOpenGLBuffer							m_cubeVBO;
	QOpenGLVertexArrayObject				m_cubeVAO;
	

};


#endif // VOLUMEWIDGET_H