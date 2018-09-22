#ifndef VOLUMEWIDGET_H
#define VOLUMEWIDGET_H

#include <QOpenGLWidget>
#include <QList>
#include <QScopedPointer>
#include <QOpenGLFunctions_3_3_Core>

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
				   protected QOpenGLFunctions_3_3_Core
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
	void			mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
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
	void updateMark();
private:
	void									updateVolumeData();
	void									updateMarkData();


	static QColor							idToColor(int id);
	static int								colorToId(const QColor & color);
	int										selectMesh(int x,int y);		//(x,y) coordinates on screen


	void									cleanup();

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
	QList<QColor>							m_markColor;
	int										m_selectedId;
	
	QOpenGLShaderProgram					m_meshShader;

	QScopedPointer<SliceVolume>				m_volume;


	QOpenGLShaderProgram					m_selectShader;
	QScopedPointer<QOpenGLFramebufferObject>m_pickFBO;
	bool									m_selectMode;
	bool									m_startSelect;

	friend class RenderParameterWidget;
	friend class TriangleMesh;
	friend class SliceVolume;
};

inline QColor RenderWidget::idToColor(int id) 
{
	return { (id & 0x000000FF) >> 0, (id & 0x0000FF00) >> 8, (id & 0x00FF0000) >> 16 };		//255^3 objects
}

inline int RenderWidget::colorToId(const QColor& color) 
{
	return color.red() + color.green() * 255 + color.blue() * 255 * 255;
}




#endif // VOLUMEWIDGET_H
