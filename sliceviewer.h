#pragma once
#ifndef SLICEVIEWER_H_
#define SLICEVIEWER_H_
#include <QWidget>
#include <QPainter>
#include <QColor>

class SliceViewer :public QWidget
{
	Q_OBJECT
public:
	SliceViewer(QWidget * parent = nullptr);
	SliceViewer(QWidget * parent, const QImage & image);
	void setImage(const QImage & image);
	void paintBegin() { m_paint = true; }
	void paintEnd() { m_paint = false; }
	void setGrayscaleStrechingLowerBound(int value);
	void setGrayscaleStrechingUpperBound(int value);
protected:
	void paintEvent(QPaintEvent *event)override;
private:
	const int WIDTH = 500;
	const int HEIGHT = 500;

	bool m_paint;
	QImage m_image;
	QImage m_imageCopy;
};
#endif // !SLICEVIEWER_H_
