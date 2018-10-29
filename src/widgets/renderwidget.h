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
#include <QTreeView>


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
	QMatrix4x4 volumeNormalTransform;

};


class ItemQueryId {
	QHash<QPersistentModelIndex, int>		m_index2Id;
	QHash<int, QPersistentModelIndex>       m_id2Index;
public:
	ItemQueryId(){}
	void addQueryPair(const QPersistentModelIndex & index, int id) {
		m_index2Id[index] = id;
		m_id2Index[id] = index;
	}
	void removeQueryPair(int id) {
		const auto itr = m_id2Index.constFind(id);
		if (itr == m_id2Index.constEnd())
			return;
		m_index2Id.remove(*itr);
		m_id2Index.remove(id);
	}
	void removeQueryPair(const QPersistentModelIndex & index) {
		const auto itr = m_index2Id.constFind(index);
		if (itr == m_index2Id.constEnd())
			return;
		m_id2Index.remove(*itr);
		m_index2Id.remove(index);
	}
	const QPersistentModelIndex & toIndex(int id)const {
		const auto itr = m_id2Index.constFind(id);
		if(itr != m_id2Index.constEnd()) {
			return *itr;
		}
		return QPersistentModelIndex();

	}
	int toId(const QPersistentModelIndex & index)const {
		const auto itr = m_index2Id.constFind(index);
		if (itr != m_index2Id.constEnd()) {
			return *itr;
		}
		return -1;
	}
	void clear() {
		m_id2Index.clear();
		m_index2Id.clear();
	}
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
	void			markModelDataChanged(const QModelIndex & begin, const QModelIndex & end, const QVector<int>& role);
	void			markModelOfSelectionModelCurrentChanged(const QModelIndex & current, const QModelIndex & previous);
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

	QList<MeshIntegration>					m_integration;
	ItemQueryId								m_query;

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
