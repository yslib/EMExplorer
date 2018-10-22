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
//#include "renderoptionwidget.h"

#include <QOpenGLTexture>



class MarkModel;
class AbstractSliceDataModel;
class ShaderProgram;
class QOpenGLShaderProgram;
class SliceVolume;
class QMenu;
class GPUVolume;
class QOpenGLFramebufferObject;


enum RenderMode {
	SliceTexture = 1,
	LineMesh = 2,
	FillMesh = 4,
	DVR = 8
};

struct RenderOptions {
	float ambient;
	float specular;
	float diffuse;
	float shininess;
	float xSpacing;
	float ySpacing;
	float zSpacing;
	QVector3D lightDirection;
	RenderMode mode;
	QVector3D sliceNormal;
	RenderOptions() :
		ambient(1.0)
		, specular(0.75)
		, diffuse(0.5)
		, shininess(40.00)
		, lightDirection(0, -1, 0)
		, xSpacing(1.0)
		, ySpacing(1.0)
		, zSpacing(1.0)
		, mode(RenderMode::DVR)
		, sliceNormal(0, 0, 0)
	{
	}
};

class RenderWidgetPrivate {
	Q_DECLARE_PUBLIC(RenderWidget);
	RenderWidget * const q_ptr;
public:
	RenderWidgetPrivate(RenderWidget * outer) :q_ptr(outer),
		topSliceIndex(0)
		, rightSliceIndex(0)
		, frontSliceIndex(0)
		, selectedObjectId(-1)
		, enablePickingMode(true)
		, enableStartPicking(false)
		, options(new RenderOptions){
	}
	int topSliceIndex;
	int rightSliceIndex;
	int frontSliceIndex;
	int selectedObjectId;
	bool enablePickingMode;
	bool enableStartPicking;
	QSharedPointer<RenderOptions> options;
	QPoint lastMousePos;
	QMatrix4x4 volumeNormalTransform;

};
//class RenderWidgetPrivate;


class RenderWidget :public QOpenGLWidget,
	protected QOpenGLFunctions_3_3_Core
{
	Q_OBJECT
public:
	RenderWidget(AbstractSliceDataModel * dataModel, MarkModel * markModel, QWidget * parent = nullptr);
	void			setDataModel(AbstractSliceDataModel * model);
	void			setMarkModel(MarkModel * model);
	AbstractSliceDataModel*			dataModel()const { return m_dataModel; }
	FocusCamera     camera()const { return m_camera; }
	QSharedPointer<RenderOptions> options()const;
	GPUVolume*	    volume()const;

	QSize			minimumSizeHint() const Q_DECL_OVERRIDE;
	QSize			sizeHint() const Q_DECL_OVERRIDE;
	~RenderWidget();
protected:
	void			initializeGL() Q_DECL_OVERRIDE;
	void			resizeGL(int w, int h) Q_DECL_OVERRIDE;
	void			paintGL() Q_DECL_OVERRIDE;
	void			mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void			mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void			mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
signals:
	void			markModelChanged();
	void			dataModelChanged();
	void			requireTransferFunction();
	void		    windowResized(int w, int h);
public slots:
	void			updateTransferFunction(const float * func);
	void			setTopSlice(int value) { Q_D(RenderWidget); d->topSliceIndex = value; update(); }
	void			setRightSlice(int value) { Q_D(RenderWidget); d->rightSliceIndex = value; update(); }
	void			setFrontSlice(int value) { Q_D(RenderWidget); d->frontSliceIndex = value; update(); }
	void			setTopSliceVisible(bool check);
	void			setRightSliceVisible(bool check);
	void			setFrontSliceVisible(bool check);
private slots:
	void			updateMark();
private:
	RenderWidgetPrivate* const d_ptr;
	Q_DECLARE_PRIVATE(RenderWidget);

	void									updateVolumeData();
	//void									updateMarkData();
	static QColor							idToColor(int id);
	static int								colorToId(const QColor & color);
	int										selectMesh(int x,int y);		//(x,y) coordinates on screen
	void									cleanup();

	MarkModel								*m_markModel;
	AbstractSliceDataModel					*m_dataModel;
	//RenderParameterWidget					*m_parameterWidget;

	QMatrix4x4								m_proj;
	QMatrix4x4								m_otho;
	FocusCamera								m_camera;		//view matrix in this

	QVector3D								m_voxelSize;
	QVector3D								m_volumeBound;
	float									m_rayStep;
	// Textures
	QOpenGLTexture							*m_tfTexture;
	//QMenu									*m_contextMenu;
	//Mark Mesh

	QList<QSharedPointer<TriangleMesh>>     m_markMeshes;
	QList<QColor>							m_markColor;
	QScopedPointer<SliceVolume>				m_volume;


	QOpenGLShaderProgram					*m_meshShader;
	QOpenGLShaderProgram					*m_selectShader;
	QOpenGLFramebufferObject				*m_pickFBO;

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
inline QSharedPointer<RenderOptions> RenderWidget::options()const 
{
	//Q_D(RenderWidget);
	return d_ptr->options;
}
#endif // VOLUMEWIDGET_H
