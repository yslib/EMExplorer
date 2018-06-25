#ifndef SLICEVIEW_H
#define SLICEVIEW_H
#include <QGraphicsView>
#include "markitem.h"

class SliceItem;




class SliceView :public QGraphicsView
{
public:
	enum OperationState
	{
		None = 0,
		Paint = 1,
		Move = 2,
		Selection = 4
	};

	SliceView(QWidget * parent = nullptr);
	void setMarks(const QList<QGraphicsItem *> & items);
	public slots:
	//void paintEnable(bool enable) { m_paint = true; }
	//void moveEnable(bool enable) { m_selection = enable; }
	inline void setOperation(int func);
	void setImage(const QImage & image);
	//inline void setColor(const QColor & color);
	//inline QColor color()const;
	inline void setPen(const QPen & pen);
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
	//void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
signals:
	void sliceSelected(const QPoint & point);
	void markAdded(QGraphicsItem * item);
	void sliceMoved(const QPointF & delta);
private:
	inline static void clear_slice_marks_helper_(SliceItem * slice);
	void set_image_helper_(const QPoint& pos, const QImage& inImage, SliceItem *& sliceItem, QImage * outImage);
	inline  void set_mark_helper_(const QList<QGraphicsItem*>& items);
	QGraphicsItem * createMarkItem();

	Q_OBJECT
	qreal m_scaleFactor;
	QVector<QPoint> m_paintViewPointsBuffer;
	SliceItem * m_currentPaintingSlice;

	QPointF m_prevScenePoint;
	//QColor m_color;
	QPen m_pen;
	SliceItem * m_slice;
	QImage  m_image;

	//QGraphicsItem * m_paintingItem;
	StrokeMarkItem * m_paintingItem;

	//state variable
	bool m_paint;
	bool m_selection;

	int m_state;

};

inline void SliceView::setOperation(int state)
{
	Q_ASSERT_X(state == OperationState::Paint || state == OperationState::Move || state == OperationState::Selection||state == OperationState::None,
		"SliceView::setFunction", "state must be exclusive");
	m_state = state;
}
//inline void SliceView::setColor(const QColor & color) { /*m_color = color;*/ }
//inline QColor SliceView::color()const { return  QColor(); }
inline void SliceView::setPen(const QPen & pen){m_pen = pen;}
inline QPen SliceView::pen()const{return m_pen;}


#endif // SLICEVIEW_H