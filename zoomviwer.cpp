#include "zoomviwer.h"
#include <QDebug>

ZoomViwer::ZoomViwer(QWidget * parent) :QWidget(parent), 
m_originalHeight{ 0 }, 
m_originalWidth{ 0 }, 
m_thumbnail{ QImage() },
m_zoomRect{ QRectF() },
m_zoomFactor{1.0},
m_minZoomFactor{0.1},
m_imageRect{}
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
	m_zoomFactor = 1;
	//emit zoomRegionChanged(m_zoomRect);
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

void ZoomViwer::setZoomFactor(qreal factor)
{
	if (factor > 1.0)
		factor = 1.0;
	else if (factor < m_minZoomFactor)
		factor = m_minZoomFactor;
	m_zoomRect.center();
}

void ZoomViwer::setMinZoomFactor(qreal minFactor)
{
	if (minFactor < 0.001)
		minFactor = 0.001;
	else if (minFactor > 1.0)
		minFactor = 1.0;
	else m_minZoomFactor = minFactor;
}

ZoomViwer::~ZoomViwer()
{

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
	m_imageRect = QRectF(topLeft, QSizeF(m_thumbnail.width(), m_thumbnail.height()));
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
	if (event->button() == Qt::LeftButton) {
		QPoint mousePos = event->pos();
		qreal dx = m_zoomRect.width() / 2;
		qreal dy = m_zoomRect.height() / 2;
		QRectF validCenterRegion = m_imageRect.adjusted(dx, dy, -dx, -dy);
		if (validCenterRegion.isValid() == true) {
			if (false == validCenterRegion.contains(mousePos)) {
				qreal x = mousePos.x();
				qreal y = mousePos.y();
				qreal top = validCenterRegion.top();
				qreal left = validCenterRegion.left();
				qreal right = validCenterRegion.right();
				qreal bottom = validCenterRegion.bottom();
				if (x <= left && y <= top) {
					mousePos = QPoint(left, top);
				}
				else if (x > left &&x <= right&& y <= top) {
					mousePos = QPoint(x, top);
				}
				else if (x > right && y <= top) {
					mousePos = QPoint(right, top);
				}
				else if (x <= left && y > top&&y <= bottom) {
					mousePos = QPoint(left, y);
				}
				else if (x >= left && y > top&&y <= bottom) {
					mousePos = QPoint(right, y);
				}
				else if (x <= left && y > bottom) {
					mousePos = QPoint(left, bottom);
				}
				else if (x>left & x <= right && y>bottom) {
					mousePos = QPoint(x, bottom);
				}
				else if (x > right && y > bottom) {
					mousePos = QPoint(right, bottom);
				}
			}
			/*Translate the global coords from widget origin
			to local coords from image origin*/
			QPointF newPos = QPointF(mousePos.x(), mousePos.y())
				- m_imageRect.topLeft();
			m_zoomRect.moveCenter(newPos);
			update();
			emit zoomRegionChanged(zoomRegion());
		}
	}
	else {

	}
	
}

void ZoomViwer::wheelEvent(QWheelEvent * event)
{
	qDebug() << "Wheel Event";
	if (event->orientation() == Qt::Horizontal) {
		//Nothing
	}
	else {
		qreal maxWidth = m_imageRect.width() - m_zoomRect.left();
		qreal maxHeight = m_imageRect.height() - m_zoomRect.top(); 
		qreal newWidth;
		qreal newHeight;
		if (event->delta() > 0)
		{
			qreal newZoomFactor = m_zoomFactor + 0.05;
			newWidth = m_imageRect.width()*(newZoomFactor);
			newHeight = m_imageRect.height()*(newZoomFactor);
			if (newWidth <= maxWidth && newHeight <= maxHeight) {
				m_zoomFactor = newZoomFactor;
			}
			else {
				return;
			}
			if (m_zoomFactor > 1.0) {
				m_zoomFactor = 1.0;
			}
		}
		else {
			m_zoomFactor -= 0.05;
			newWidth = m_imageRect.width()*(m_zoomFactor);
			newHeight = m_imageRect.height()*(m_zoomFactor);
			if (m_zoomFactor < m_minZoomFactor) {
				m_zoomFactor = m_minZoomFactor;
			}
		}

		qreal dx = newWidth > maxWidth ? maxWidth : newWidth;
		qreal dy = newHeight > maxHeight ? maxHeight : newHeight;
		m_zoomRect.setWidth(dx);
		m_zoomRect.setHeight(dy);
		update();
		emit zoomRegionChanged(zoomRegion());
	}
}


