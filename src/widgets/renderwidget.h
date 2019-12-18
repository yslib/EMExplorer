#ifndef VOLUMEWIDGET_H
#define VOLUMEWIDGET_H

#include <QOpenGLWidget>
#include <QList>
#include <QScopedPointer>

#include "3drender/geometry/camera.h"
#include "3drender/shader/shaderprogram.h"
#include "3drender/geometry/mesh.h"
#include "algorithm/bimap.h"

#include <QOpenGLTexture>
#include "model/treeitem.h"


class MarkModel;
class AbstractSliceDataModel;
class ShaderProgram;
class QOpenGLShaderProgram;
class SliceVolume;
class QMenu;
class GPUVolume;
class QOpenGLFramebufferObject;


enum RenderMode 
{
	SliceTexture = 1,
	LineMesh = 2,
	FillMesh = 4,
	DVR = 8,
	Modulo = 16
};

struct RenderOptions {
	float ambient;
	float specular;
	float diffuse;
	float shininess;
	float xSpacing;
	float ySpacing;
	float zSpacing;
	ysl::Vector3f lightDirection;
	RenderMode mode;
	ysl::Vector3f sliceNormal;
	RenderOptions():
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

struct MeshIntegration 
{
	QSharedPointer<TriangleMesh> mesh;
	QColor color;
	bool visible;
	MeshIntegration(const QSharedPointer<TriangleMesh> & amesh,const QColor & acolor,bool avisible):mesh(amesh),color(acolor),visible(avisible){}
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
	ysl::Transform volumeNormalTransform;
};

class RenderWidget :public QOpenGLWidget,
	protected QOpenGLFunctions_3_3_Core
{
	Q_OBJECT
public:
	RenderWidget(AbstractSliceDataModel * dataModel, MarkModel * markModel, QWidget * parent = nullptr);
	void			setDataModel(AbstractSliceDataModel * model);
	void			setMarkModel(MarkModel * model);
	void			saveMesh(const QString& dir);
	AbstractSliceDataModel*			dataModel()const { return m_dataModel; }
	//FocusCameraOld  cameraOld()const { return m_camera; }
	FocusCamera	camera()const { return m_cameraEx; }
	QSharedPointer<RenderOptions> options()const;
	GPUVolume*	    volume()const;
	QSize			minimumSizeHint() const Q_DECL_OVERRIDE;
	QSize			sizeHint() const Q_DECL_OVERRIDE;
					~RenderWidget()override;
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
	void			_slot_markModelDataChanged(const QModelIndex & begin, const QModelIndex & end, const QVector<int>& role);
	void			onCurrentMeshChanged(int current, int previous);
	void			oExternalCurrentMarkChanged(const QModelIndex & current, const QModelIndex & previous);
	void			onExternalSelectedMarksChanged(const QItemSelection & selected, const QItemSelection & deselected);

private:
	//void			drawCoordinate(QPainter* painter);
	void			drawThreeAxis();

	ysl::Transform worldMatrix() const;

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

	ysl::Transform							m_proj;
	ysl::Transform							m_otho;

	//FocusCameraOld							m_camera;		//view matrix in this
	FocusCamera							m_cameraEx;

	QVector3D								m_voxelSize;
	QVector3D								m_volumeBound;
	float									m_rayStep;
	// Textures
	QOpenGLTexture							*m_tfTexture;
	//QMenu									*m_contextMenu;
	//Mark Mesh

	QList<MeshIntegration>					m_integration;
	BiMap<QPersistentModelIndex,int>						m_query;
	QScopedPointer<SliceVolume>				m_volume;


	QOpenGLShaderProgram					*m_meshShader;
	QOpenGLShaderProgram					*m_selectShader;
	QOpenGLFramebufferObject				*m_pickFBO;

	QOpenGLShaderProgram					*m_trivialShader;
	// Boundingbox
	QOpenGLBuffer							 m_boundingBoxVBO;
	QOpenGLVertexArrayObject				 m_boundingBoxVAO;

	// axis
	QOpenGLBuffer							 m_axisVBO;
	QOpenGLVertexArrayObject				 m_axisVAO;

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
/**
 * \brief Returns the options for rendering.
 * 
 * \sa RenderOptions
 */
inline QSharedPointer<RenderOptions> RenderWidget::options()const 
{
	//Q_D(RenderWidget);
	return d_ptr->options;
}


#endif // VOLUMEWIDGET_H
