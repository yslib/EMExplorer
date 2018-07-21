#ifndef VOLUMEWIDGET_H
#define VOLUMEWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QScopedPointer>

#include "volume/camera.h"

class MarkModel;
class AbstractSliceDataModel;
class QOpenGLShaderProgram;


struct VolumeRenderingParameters
{
	float ambient;
	float specular;
	float diffuse;
	float shininess;
};

struct MeshRenderingParamters
{
	bool lineMode;
	bool antiAliasing;
};

class VolumeWidget:public QOpenGLWidget,protected QOpenGLFunctions
{
	Q_OBJECT
public:
    VolumeWidget(AbstractSliceDataModel * dataModel, MarkModel * markModel, QWidget * parent = nullptr);
	void setDataModel(AbstractSliceDataModel * model);
	void setMarkModel(MarkModel * model);
	QSize minimumSizeHint() const Q_DECL_OVERRIDE;
	QSize sizeHint() const Q_DECL_OVERRIDE;
	~VolumeWidget();
protected:
	void initializeGL() Q_DECL_OVERRIDE;
	void resizeGL(int w, int h) Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
signals:
	void markModelChanged();
	void dataModelChanged();
private:
	MarkModel * m_markModel;
	AbstractSliceDataModel * m_dataModel;
	QMatrix4x4 m_proj;
	QMatrix4x4 m_world;
	FocusCamera m_camera;
	QPoint m_lastPos;
	int m_projMatLoc;
	int m_modelViewMatLoc;
	int m_normalMatrixLoc;
	int m_lightPosLoc;
	QScopedPointer<QOpenGLShaderProgram> m_program;
	QOpenGLBuffer m_cubeVBO;
	QOpenGLVertexArrayObject m_cubeVAO;
	void drawCube();
	void updateVolumeData();
	void updateMarkData();
};

#endif // VOLUMEWIDGET_H