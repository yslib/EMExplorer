#ifndef VOLUMEWIDGET_H
#define VOLUMEWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include "volume/camera.h"


class MarkModel;
class AbstractSliceDataModel;
class QOpenGLShaderProgram;

class VolumeWidget:public QOpenGLWidget,protected QOpenGLFunctions
{
public:
    VolumeWidget(AbstractSliceDataModel * dataModel, MarkModel * markModel, QWidget * parent = nullptr);
	void setDataModel(AbstractSliceDataModel * model);
	void setMarkModel(MarkModel * model);
	QSize minimumSizeHint() const Q_DECL_OVERRIDE;
	QSize sizeHint() const Q_DECL_OVERRIDE;
protected:
	void initializeGL() Q_DECL_OVERRIDE;
	void resizeGL(int w, int h) Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

private:
	MarkModel * m_markModel;
	AbstractSliceDataModel * m_dataModel;

	QMatrix4x4 m_proj;
	QMatrix4x4 m_world;
	//Camera m_camera;
	FocusCamera m_camera;
	QPoint m_lastPos;

	int m_projMatLoc;
	int m_modelViewMatLoc;
	int m_normalMatrixLoc;
	int m_lightPosLoc;

	QOpenGLShaderProgram * m_program;
	QOpenGLBuffer m_cubeVBO;
	QOpenGLVertexArrayObject m_cubeVAO;

	void drawCube();
	void updateVolumeData();
	void updateMarkData();
};

#endif // VOLUMEWIDGET_H