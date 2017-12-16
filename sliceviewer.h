#pragma once
#ifndef SLICEVIEWER_H_
#define SLICEVIEWER_H_
#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QMouseEvent>
#include <QPicture>
#include <QDebug>
#include <QVector>

class SliceViewer :public QWidget
{
	Q_OBJECT
public:
	enum class Shape{Line,Ellipse};
	enum class PaintState{LineState,EllipseState,All};
public:
	SliceViewer(QWidget * parent = nullptr);
	SliceViewer(QWidget * parent, const QImage & image,const QRect & rect = QRect());
    void setImage(const QImage & image,const QRect & region = QRect());
	void addMark(const QPicture & mark);
    void setMarks(const QVector<QPicture> & marks);

	void originalSize(bool original = false) { m_originalSize = original; }
	void setGrayscaleStrechingLowerBound(int value);
	void setGrayscaleStrechingUpperBound(int value);
	void setMarkColor(const QColor & color) { m_pen.setColor(color); }
	QColor getMarkColor()const { return m_pen.color(); }
	void setPenWidth(int w) { m_pen.setWidth(w); }
	void setShape(Shape shape) { m_shape = shape; }
	Shape getShape()const { return m_shape; }
	public slots:
	void paintEnable(bool enable) { m_paintEnable = enable; }
private:
	QPoint _thisCoordToImageCoord(const QPoint & p);
	void _drawLineOnThis(const QPoint &p1, const QPoint &p2);
	void _drawEllipseOnThis(const QPoint & center,int rx,int ry);
protected:
	void paintEvent(QPaintEvent *event)override;
	void mousePressEvent(QMouseEvent *event)override;
	void mouseReleaseEvent(QMouseEvent *event)override;
	void mouseMoveEvent(QMouseEvent *event)override;
signals:
	void onDrawing(const QPoint & point);
	void drawingFinished(const QPicture & points);
private:
	const int WIDTH = 500;
	const int HEIGHT = 500;
	bool m_originalSize;
	bool m_paintEnable;
	bool m_painting;

	Shape m_shape;
	PaintState m_paintState;
	QPoint m_linePoint1, m_linePoint2;
	QPoint m_ellipseCenter;
	int m_rx, m_ry;

	QVector<QPoint> m_points;
	QPoint m_prevPoint;
	QPoint m_firstPoint;
	QPoint m_lastPoint;

	QPen m_pen;
	QPixmap m_canvas;
	QRect m_imageRect;
    QVector<QPicture> m_marks;
};
#endif // !SLICEVIEWER_H_
