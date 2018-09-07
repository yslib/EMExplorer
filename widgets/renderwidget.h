#ifndef VOLUMEWIDGET_H
#define VOLUMEWIDGET_H

#include <QOpenGLWidget>
#include <QList>
#include <QScopedPointer>
#include <QOpenGLFramebufferObject>

#include "3drender/geometry/camera.h"
#include "3drender/shader/shaderdatainterface.h"
#include "3drender/shader/shaderprogram.h"
#include "algorithm/gradientcalculator.h"
#include "3drender/geometry/mesh.h"

#include <QOpenGLTexture>
#include "3drender/shader/raycastingshader.h"


class MarkModel;
class AbstractSliceDataModel;
class ShaderProgram;
class QOpenGLShaderProgram;
class QMenu;

#include "renderoptionwidget.h"


//#define TESTCUBE

class RenderWidget:public QOpenGLWidget,
				   public ShaderDataInterface,
				   protected QOpenGLFunctions_3_1
{
	Q_OBJECT
public:
					RenderWidget(AbstractSliceDataModel * dataModel, MarkModel * markModel, RenderParameterWidget * widget,QWidget * parent = nullptr);
	void			setDataModel(AbstractSliceDataModel * model);
	void			setMarkModel(MarkModel * model);

	QSize			minimumSizeHint() const Q_DECL_OVERRIDE;
	QSize			sizeHint() const Q_DECL_OVERRIDE;
	void			addContextAction(QAction* action);
					~RenderWidget();
public://ShaderDataInterface
	unsigned int	volumeTexId()const override								{ return m_volumeTexture.textureId();}
	QVector3D		voxelSize()const override								{ return m_voxelSize; }
	unsigned int	startPosTexIdx()const override							{ return m_fbo->textures()[0];}
	unsigned int	endPosTexIdx()const override							{ return m_fbo->textures()[1];}
	unsigned int    gradientTexId() const override							{ return m_gradientTexture.textureId(); }
	float			rayStep()const override									{ return m_rayStep; }
	unsigned int	transferFunctionsTexId()const override					{ return m_tfTexture.textureId();}
	QVector3D		cameraPos()const override								{ return m_camera.position(); }
	QVector3D		cameraTowards()const override							{ return m_camera.front(); }
	QVector3D		cameraUp()const override							    { return m_camera.up(); }
	QVector3D		cameraRight()const override								{ return m_camera.right(); }
	QMatrix4x4		viewMatrix()const override								{ return m_camera.view(); }
	QMatrix4x4		worldMatrix()const override								{ return m_world; }
	QMatrix4x4		othoMatrix()const override								{ return m_otho; }
	QMatrix4x4		perspMatrix()const override								{ return m_proj; }
	QVector3D		lightDirection()const override							{ return m_parameterWidget->options()->lightDirection; }
	float			ambient()const override									{ return m_parameterWidget->options()->ambient; }
	float			diffuse()const override									{ return m_parameterWidget->options()->diffuse; }
	float			shininess()const override								{ return m_parameterWidget->options()->shininess; }
	float			specular()const override								{ return m_parameterWidget->options()->specular; }
	QVector3D		volumeBound()const override								{ return m_volumeBound; }
	QSize			windowSize()const override								{ return size(); }
	
protected:
	void			initializeGL() Q_DECL_OVERRIDE;
	void			resizeGL(int w, int h) Q_DECL_OVERRIDE;
	void			paintGL() Q_DECL_OVERRIDE;
	void			mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void			mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void			contextMenuEvent(QContextMenuEvent * event)Q_DECL_OVERRIDE;
signals:
	void			markModelChanged();
	void			dataModelChanged();
	void			requireTransferFunction();

public slots:
	void updateTransferFunction(const float* func, bool updated);
	void updateMarkMesh();
	void setTopSlice(int value) { m_topSlice = value; update(); }
	void setRightSlice(int value) { m_rightSlice = value; update(); }
	void setFrontSlice(int value) { m_frontSlice = value; update(); }


private:
	using ShaderHash = QHash<QString, QSharedPointer<ShaderProgram>>;
	void			updateVolumeData();
	void			updateMarkData();
	void			initializeShaders();
	void			initiliazeTextures();
	void			loadDataToTextures();
	void			cubeInitilized();
	void			contextMenuAddedHelper(QWidget * widget);
	void			cleanup();

	MarkModel								*m_markModel;
	AbstractSliceDataModel					*m_dataModel;
	RenderParameterWidget					*m_parameterWidget;

	RenderOptions*							m_options;


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

	//3d Slice
	QScopedPointer<SliceShader>			    m_sliceShader;
	QOpenGLBuffer							m_topSliceVBO;
	QOpenGLVertexArrayObject				m_topSliceVAO;
	QOpenGLBuffer							m_rightSliceVBO;
	QOpenGLVertexArrayObject				m_rightSliceVAO;
	QOpenGLBuffer							m_frontSliceVBO;
	QOpenGLVertexArrayObject				m_frontSliceVAO;


	// Textures
	QOpenGLTexture							m_tfTexture;
	QOpenGLTexture							m_gradientTexture;
	QOpenGLTexture							m_volumeTexture;
	int										m_topSlice;
	int										m_rightSlice;
	int										m_frontSlice;
	// Gradient
	GradientCalculator						m_gradCalc;
	// Cube
	QScopedPointer<QOpenGLShaderProgram>    m_cubeShader;
	//QOpenGLVertexArrayObject				m_cubeVAO;
	//QOpenGLBuffer							m_cubeVBO;
	QScopedPointer<TriangleMesh>			m_cube;
	//Context Menu
	QMenu									*m_contextMenu;

	//Mark Mesh
	QList<QSharedPointer<TriangleMesh>>     m_markMeshes;


	friend class RenderParameterWidget;
};


#endif // VOLUMEWIDGET_H
