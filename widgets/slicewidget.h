#ifndef SLICEVIEW_H
#define SLICEVIEW_H
#include <QGraphicsView>
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
		 * This action moves the slice when drag the slice item.
		 */
		Move = 2,
		/*
		 * This action will select marks by clicking the item or dragging a rubber.
		 */
		Selection = 4
	};

	SliceWidget(QWidget * parent = nullptr);
	void setMarks(const QList<QGraphicsItem *> & items);
public slots:

	inline void setOperation(int func);

	void setImage(const QImage & image);

	inline void setPen(const QPen & pen);

	inline void setNavigationViewEnabled(bool enabled);


	inline QPen pen()const;

	void clearSliceMarks();

	QList<QGraphicsItem*> selectedItems()const;

	int selectedItemCount()const;

	void moveSlice(const QPointF & dir);

	QSize sizeHint()const override;

protected:

	void mousePressEvent(QMouseEvent * event)Q_DECL_OVERRIDE;

	void mouseMoveEvent(QMouseEvent * event)Q_DECL_OVERRIDE;

	void mouseReleaseEvent(QMouseEvent * event)Q_DECL_OVERRIDE;

	void wheelEvent(QWheelEvent * event)Q_DECL_OVERRIDE;

	void focusInEvent(QFocusEvent* event) Q_DECL_OVERRIDE;

	void focusOutEvent(QFocusEvent* event)Q_DECL_OVERRIDE;

	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

signals:
	void sliceSelected(const QPoint & point);

	void sliceSelected();

	void markAdded(QGraphicsItem * item);

	void viewMoved(const QPointF & delta);

	void selectionChanged();

private:

	inline static void clear_slice_marks_helper_(SliceItem * slice);

	void setImageHelper(const QPoint& pos, const QImage& inImage, SliceItem *& sliceItem, QImage * outImage);

	inline  void set_mark_helper_(const QList<QGraphicsItem*>& items);

	QRect thumbnailRect(const QRectF & sliceRect,const QRectF & viewRect);

	QGraphicsItem * createMarkItem();

	static QPixmap createAnchorItemPixmap(const QString & fileName = QString());


	Q_OBJECT

	qreal m_scaleFactor;
	bool m_paintNavigationView;

	QVector<QPoint> m_paintViewPointsBuffer;
	QPointF m_prevViewPoint;

	//QColor m_color;
	QPen m_pen;
	SliceItem * m_slice;

	QImage m_image;
	QImage m_thumbnail;

	//QGraphicsItem * m_paintingItem;
	//items
	SliceItem * m_currentPaintingSlice;
	StrokeMarkItem * m_paintingItem;
	AnchorItem * m_anchorItem;
	//state variable
	bool m_paint;
	bool m_selection;
	int m_state;
};

inline void SliceWidget::setOperation(int state)
{
	Q_ASSERT_X(state == Operation::Paint || 
		state == Operation::Move || 
		state == Operation::Selection||
		state == Operation::None,
		"SliceView::setFunction", "state must be exclusive");
	m_state = state;
}
//inline void SliceView::setColor(const QColor & color) { /*m_color = color;*/ }
//inline QColor SliceView::color()const { return  QColor(); }
inline void SliceWidget::setPen(const QPen & pen){m_pen = pen;}
inline QPen SliceWidget::pen()const{return m_pen;}
inline void SliceWidget::setNavigationViewEnabled(bool enabled)
{
	m_paintNavigationView = enabled;
	update();
	updateGeometry();
}



#endif // SLICEVIEW_H
