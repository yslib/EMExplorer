#include "zoomviwer.h"
#include <QDebug>

ZoomViwer::ZoomViwer(QWidget * parent) :QWidget(parent), m_originalHeight{ 0 }, m_originalWidth{ 0 }, m_thumbnail(QImage()), m_zoomRect{ QRectF() }
{
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	setMinimumSize(WIDTH, HEIGHT);
	resize(WIDTH, HEIGHT);
}

void ZoomViwer::setImage(const QImage & image)
{
	m_originalWidth = image.width();
	m_originalHeight = image.height();
	m_thumbnail = image.scaled(size(),Qt::KeepAspectRatio);
	m_zoomRect = QRectF(QPointF(0.0, 0.0), m_thumbnail.size());
	update();
	updateGeometry();
}

void ZoomViwer::clearImage()
{
	m_zoomRect = QRectF();
	m_originalHeight = 0;
	m_originalWidth = 0;
	m_thumbnail = QImage();
	update();
	updateGeometry();
}

QRectF ZoomViwer::zoomRegion() const
{
	if (m_thumbnail.width() == 0 || m_thumbnail.height() == 0)
		return QRectF();
	qreal width = m_thumbnail.width();
	qreal height = m_thumbnail.height();
	qreal topX = (m_zoomRect.x() / width)*m_originalWidth;
	qreal topY = (m_zoomRect.y() / height)*m_originalHeight;
	qreal w = (m_zoomRect.width()/width)*m_originalWidth;
	qreal h = (m_zoomRect.height()/height)*m_originalHeight;
	return QRectF(QPointF(topX, topY), QSizeF(w, h));
}

QPointF ZoomViwer::zoomPosition() const
{
	QRectF rect = zoomRegion();
	return QPointF(rect.x() + rect.width() / 2, rect.y() + rect.height() / 2);
}

void ZoomViwer::paintEvent(QPaintEvent *event)
{
	
	QImage background(size(), QImage::Format_ARGB32_Premultiplied);
	QPainter backgroundPainter(&background);
	backgroundPainter.initFrom(this);
	backgroundPainter.setRenderHint(QPainter::Antialiasing, true);
	backgroundPainter.fillRect(rect(), QBrush(Qt::white));

	//Draw the thumbnail in the center of the background
	QSizeF bgSize = size();
	qreal topX = bgSize.width() / 2 - m_thumbnail.width() / 2;
	qreal topY = bgSize.height() / 2 - m_thumbnail.height() / 2;
	QPointF topLeft(topX, topY);
	backgroundPainter.drawImage(topLeft, m_thumbnail);

	//Drawing the rectangle
	backgroundPainter.setPen(QColor(255,0,0));
	backgroundPainter.drawRect(QRectF(QPointF(topLeft+m_zoomRect.topLeft()),QSizeF(m_zoomRect.size())));

	backgroundPainter.end();
	QPainter widgetPainter(this);
	widgetPainter.drawImage(0, 0, background);
}

void ZoomViwer::mousePressEvent(QMouseEvent *event)
{
	qDebug() << "Mouse Press Event";
	
}

void ZoomViwer::wheelEvent(QWheelEvent * event)
{
	qDebug() << "Wheel Event";
	if (event->orientation() == Qt::Horizontal) {
		//Nothing
	}
	else {
		qreal zoomFactor = 0.9;
		if (event->delta() > 0)
			zoomFactor = 1 / zoomFactor;
		m_zoomRect.setWidth(m_zoomRect.width()*zoomFactor);
		m_zoomRect.setHeight(m_zoomRect.height()*zoomFactor);
		update();
		emit zoomRegionChanged(zoomRegion());
	}
}

