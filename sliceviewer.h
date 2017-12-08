#pragma once
#ifndef SLICEVIEWER_H_
#define SLICEVIEWER_H_
#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QMouseEvent>
#include <QPicture>
#include <QDebug>

class SliceViewer :public QWidget
{
	Q_OBJECT
public:
	SliceViewer(QWidget * parent = nullptr);
	SliceViewer(QWidget * parent, const QImage & image,const QRect & rect = QRect());
	void setImage(const QImage & image,const QRect & rect = QRect());
	void setMark(const QPicture & mark);
	

	void originalSize(bool original = false) { m_originalSize = original; }
	void setGrayscaleStrechingLowerBound(int value);
	void setGrayscaleStrechingUpperBound(int value);
	void setMarkColor(const QColor & color) { m_color = color; }
	QColor getMarkColor()const { return m_color; }
	public slots:
	void paintEnable(bool enable) { m_paintEnable = enable; }
protected:
	void paintEvent(QPaintEvent *event)override;
	void mousePressEvent(QMouseEvent *event)override;
	void mouseReleaseEvent(QMouseEvent *event)override;
	void mouseMoveEvent(QMouseEvent *event)override;
signals:
	void onDrawing(const QPoint & point);
private:
	const int WIDTH = 500;
	const int HEIGHT = 500;
	bool m_originalSize;
	bool m_paintEnable;
	bool m_painting;
	QColor m_color;
	QImage m_image;
	QPicture m_mark;
	QRect m_imageRect;
};
#endif // !SLICEVIEWER_H_
