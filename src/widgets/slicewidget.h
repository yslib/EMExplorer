#ifndef SLICEVIEW_H
#define SLICEVIEW_H
#include <QGraphicsView>
#include <QSet>
#include "model/markitem.h"

class SliceItem;

using AnchorItem = QGraphicsPixmapItem;


struct SliceWidgetState {
	SliceItem * sliceItem;
	AnchorItem * anchorItem;
	StrokeMarkItem * paintingItem;
	QVector<QPoint> m_paintViewPointsBuffer;
	QPointF prevViewPoint;
	bool navigationView;
	bool painting;
	bool selection;
	int opState;
};

struct candidatePoint
{
	int x, y, cost;
	bool operator < (const candidatePoint& a) const
	{
		return cost < a.cost;
	}
	bool operator > (const candidatePoint& a) const
	{
		return cost > a.cost;
	}
};

class SliceWidgetPrivate {
	public:
};

class SliceWidget :public QGraphicsView
{
public:
	enum Operation
	{
		/*
		 * This action only sets the anchor when click the slice item in the view.
		 */
		None = 0,
		/*
		 * This action marks on the slice item.
		 */
		Paint = 1,
		/*
		 * This action will select marks by clicking the item or dragging a rubber.
		 */
		Selection = 2,

		/*
		 *This action allow to erase mark partially
		 */
		Erase = 3
	};

	SliceWidget(QWidget * parent = nullptr);

	void setMarks(const QList<StrokeMarkItem *> & items);
public slots:

	inline void setOperation(int state);

	void setImage(const QImage & image);

	inline void setGradientMap(QVector<QVector<int>> GradientMap);

	inline void setIntelligentScissorsState(bool is_enable_intelligentScissors);

	inline void setPen(const QPen & pen);

	void setDefaultZoom();

	inline void setNavigationViewEnabled(bool enabled);

	bool navigationViewEnabled()const { return m_paintNavigationView; };

	inline QPen pen()const;

	void clearSliceMarks();

	QList<StrokeMarkItem*> selectedItems() const;

	int selectedItemCount()const;

	void moveSlice(const QPointF & dir);

	QSize sizeHint()const override;


protected:
	void mousePressEvent(QMouseEvent * event)Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent * event)Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent * event)Q_DECL_OVERRIDE;
	void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

	void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
	void wheelEvent(QWheelEvent * event)Q_DECL_OVERRIDE;
	void focusInEvent(QFocusEvent* event) Q_DECL_OVERRIDE;
	void focusOutEvent(QFocusEvent* event)Q_DECL_OVERRIDE;
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

signals:

	/**
	 * \brief The signals is emitted when the slice is selected
	 * \param point The position selected on the slice
	 */
	void sliceSelected(const QPoint & point);

	/**
	 * \brief The signals is emitted when the slice is selected
	 */
	void sliceSelected();

	/**
	 * \brief The signals is emitted when the new mark item is added
	 * 
	 * The exact time this signals emits is when the last point 
	 * append at the item in \a mouseReleaseEvent()
	 * \param item The new added item
	 */
	void markAdded(StrokeMarkItem * item);


	/**
	 * \brief This signals is emitted when the slice is dragged by mouse
	 * \param delta The position difference between movement.
	 */
	void viewMoved(const QPointF & delta);

	/**
	 * \brief This signals is emitted when the number of selected mark items changed.
	 */
	void selectionChanged();

private:

	inline static void clearSliceMarksHelper(SliceItem * slice);

	inline  void setMarkHelper(const QList<StrokeMarkItem*>& items);

	QRect thumbnailRect(const QRectF & sliceRect,const QRectF & viewRect) const;

	QGraphicsItem * createMarkItem();

	static QPixmap createAnchorItemPixmap(const QString & fileName = QString());

	QVector<QPoint> getShortestPath(QPoint s_point, QPoint e_point);

	QPoint snapPoint(QPoint clickPoint, int range = 7);

	Q_OBJECT

	enum {
		ThumbnailLength = 200
	};

	//qreal m_scaleFactor;
	bool m_paintNavigationView;
	QPointF m_prevViewPoint;
	QPen m_pen;
	SliceItem * m_slice;
	QImage m_thumbnail;
	//items
	SliceItem * m_currentPaintingSlice;
	StrokeMarkItem * m_paintingItem;
	AnchorItem * m_anchorItem;
	QSet<StrokeMarkItem *> m_erasingMarks;
	//state variable
	bool m_paint;
	bool m_selection;
	int m_state;
	//Cost Map  only consider gradient value
	QVector<QVector<int>> GradientMap;
	QVector<QPoint> m_paintViewPointsBuffer; //selected points
	QVector<QPoint> auxiliaryLinePath;  // temp shorest path

	QVector<QVector<QPoint>> path;

	bool is_draw_new_mark;// 开始绘制新的mark
	bool enable_intelligent_scissor; //是否开启智能剪刀
	bool pause_intelligent_scissor;//在绘制过程中暂时关闭智能剪刀改用手动绘制
	StrokeMarkItem* m_resultItem; //temp line

	StrokeMarkItem* tempAuxiliaryLine; //temp line

};

inline void SliceWidget::setOperation(int state)
{
	Q_ASSERT_X(state == Operation::Paint || 
		state == Operation::Selection||
		state == Operation::None ||
		state == Operation::Erase,
		"SliceView::setFunction", "state must be exclusive");
	m_state = state;

	if (state == Operation::Paint) { setMouseTracking(true); update(); }
	else { setMouseTracking(false); }
}
inline void SliceWidget::setGradientMap(QVector<QVector<int>> GradientMap) { this->GradientMap = GradientMap; }
inline void SliceWidget::setIntelligentScissorsState(bool is_enable_intelligentScissors) { enable_intelligent_scissor = is_enable_intelligentScissors; }
inline void SliceWidget::setPen(const QPen & pen){m_pen = pen;}
inline QPen SliceWidget::pen()const{return m_pen;}
inline void SliceWidget::setNavigationViewEnabled(bool enabled)
{
	m_paintNavigationView = enabled;
	update();
	updateGeometry();
}



#endif // SLICEVIEW_H
