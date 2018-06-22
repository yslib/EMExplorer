#ifndef SLICEVIEW_H
#define SLICEVIEW_H
#include <QGraphicsView>
#include "markitem.h"

class SliceItem;

class SliceView :public QGraphicsView
{
public:
	SliceView(QWidget * parent = nullptr);
	void setMarks(const QList<QGraphicsItem *> & items);
	public slots:
	void paintEnable(bool enable) { m_paint = enable; }
	void moveEnable(bool enable) { m_moveble = enable; }
	void setImage(const QImage & image);
	void setColor(const QColor & color) { m_color = color; }
	QColor color()const { return m_color; }
	void clearSliceMarks();
protected:
	void mousePressEvent(QMouseEvent * event)Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent * event)Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent * event)Q_DECL_OVERRIDE;

	void wheelEvent(QWheelEvent * event)Q_DECL_OVERRIDE;
	void focusInEvent(QFocusEvent* event) Q_DECL_OVERRIDE;
	void focusOutEvent(QFocusEvent* event)Q_DECL_OVERRIDE;
signals:
	void sliceSelected(const QPoint & point);
	void markAdded(QGraphicsItem * item);
private:
	inline static void clear_slice_marks_helper_(SliceItem * slice);
	void set_image_helper_(const QPoint& pos, const QImage& inImage, SliceItem *& sliceItem, QImage * outImage);
	inline  void set_mark_helper_(const QList<QGraphicsItem*>& items);
	Q_OBJECT
		qreal m_scaleFactor;
	QVector<QPoint> m_paintViewPointsBuffer;
	SliceItem * m_currentPaintItem;
	bool m_paint;
	bool m_moveble;
	QPointF m_prevScenePoint;
	QColor m_color;
	SliceItem * m_slice;
	QImage  m_image;

	//QGraphicsItem * m_paintingItem;
	StrokeMarkItem * m_paintingItem;

};

#endif // SLICEVIEW_H