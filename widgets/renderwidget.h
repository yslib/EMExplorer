#ifndef VOLUMEWIDGET_H
#define VOLUMEWIDGET_H

#include <QOpenGLWidget>
#include <QList>
#include <QScopedPointer>

#include "3drender/geometry/camera.h"
#include "3drender/shader/shaderprogram.h"
#include "algorithm/gradientcalculator.h"
#include "3drender/geometry/mesh.h"

#include <QOpenGLTexture>


class MarkModel;
class AbstractSliceDataModel;
class ShaderProgram;
class QOpenGLShaderProgram;
class SliceVolume;
class QMenu;

#include "renderoptionwidget.h"


//#define TESTCUBE

class RenderWidget:public QOpenGLWidget,
				   protected QOpenGLFunctions_3_1
{
	Q_OBJECT
public:
					RenderWidget(AbstractSliceDataModel * dataModel, MarkModel * markModel, RenderParameterWidget * widget,QWidget * parent = nullptr);
	void			setDataModel(AbstractSliceDataModel * model);
	void			setMarkModel(MarkModel * model);
	FocusCamera     camera()const { return m_camera; }

	QSize			minimumSizeHint() const Q_DECL_OVERRIDE;
	QSize			sizeHint() const Q_DECL_OVERRIDE;
	void			addContextAction(QAction* action);
					~RenderWidget();
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
	void		    windowResized(int w, int h);

public slots:
	void			updateTransferFunction(const float* func, bool updated);
	void			updateMarkMesh();
	void			setTopSlice(int value)	 { m_topSlice = value; update(); }
	void			setRightSlice(int value) { m_rightSlice = value; update(); }
	void			setFrontSlice(int value) { m_frontSlice = value; update(); }
private slots:
	void updateMarkMeshVisibility(const QModelIndex& begin,const QModelIndex& end,const QVector<int> & roles);

private:
	using ShaderHash = QHash<QString, QSharedPointer<ShaderProgram>>;
	void			updateVolumeData();
	void			updateMarkData();
	void			initializeShaders();
	void			initiliazeTextures();
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

	QPoint									m_lastPos;
	QVector3D								m_voxelSize;
	QVector3D								m_volumeBound;
	float									m_rayStep;

	// Textures
	QOpenGLTexture							m_tfTexture;
	int										m_topSlice;
	int										m_rightSlice;
	int										m_frontSlice;
	QMenu									*m_contextMenu;
	//Mark Mesh

	QList<QSharedPointer<TriangleMesh>>     m_markMeshes;

	QScopedPointer<SliceVolume>				m_volume;

	friend class RenderParameterWidget;
	friend class SliceVolume;
};


#endif // VOLUMEWIDGET_H
