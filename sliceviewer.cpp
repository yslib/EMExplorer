#include "sliceviewer.h"

SliceViewer::SliceViewer(QWidget * parent /* = nullptr */) :QWidget(parent), m_paint{ false },m_image{QImage()}
{
	resize(WIDTH, HEIGHT);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	setMinimumSize(WIDTH, HEIGHT);
}
SliceViewer::SliceViewer(QWidget * parent, const QImage & image) :SliceViewer{ parent } 
{
	setImage(image);
}
void SliceViewer::setImage(const QImage & image) {
	m_image = image;
	update();
	updateGeometry();
}
void SliceViewer::paintEvent(QPaintEvent *event) {
	QPainter painter(this);
	painter.drawImage(0, 0, m_image.scaled(size()));
	painter.end();
}
