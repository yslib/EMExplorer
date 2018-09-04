#include "zoomviwer.h"
#include <QDebug>

ZoomView::ZoomView(QWidget * parent) :QWidget(parent), 
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


/// \brief This function set the image to be displayed in the zoomviewer
/// \brief 
/// \param image image to be displayed
/// \param region the region of the image to be zoomed
void ZoomView::setImage(const QImage & image, const QRect & region)
{
	m_originalWidth = image.width();
	m_originalHeight = image.height();
	m_thumbnail = image.scaled(size(),Qt::KeepAspectRatio);

	//the topLeft
	m_zoomRect = QRect(QPoint(0.0, 0.0), m_thumbnail.size());
	m_zoomFactor = 1.0;

    if(region != QRect()){
        m_zoomRect = _regionToRect(region);
        qreal k1 = static_cast<qreal>(region.width())/static_cast<qreal>(m_originalWidth);
        qreal k2 = static_cast<qreal>(region.height())/static_cast<qreal>(m_originalHeight);
        m_zoomFactor = k1>k2?k2:k1;
    }
    //qDebug()<<"m_zoomRect in setImage(const QIMage &,const QRect &):"<<m_zoomRect;
	//qDebug() << "m_zoomFactor in setImage(const QImage&,const QRect &):" << m_zoomFactor;
//	emit zoomFactorChanged(m_zoomFactor);
	//qDebug() << "m_zoomRegion in setImage(const QImage &,const QRect &):" << zoomRegion();
    emit zoomRegionChanged(zoomRegion());
	update();
	updateGeometry();
}

void ZoomView::clearImage()
{
	m_zoomRect = QRectF();
	m_originalHeight = 0;
	m_originalWidth = 0;
	m_thumbnail = QImage();
	m_zoomFactor = 1.0;
    //emit zoomFactorChanged(m_zoomFactor);
	emit zoomRegionChanged(zoomRegion());
	update();
	updateGeometry();
}

QRectF ZoomView::zoomRegion() const
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

void ZoomView::setZoomRegion(const QRect &region)
{
    QRectF rect = _regionToRect(region);
    m_zoomRect = rect;
    emit zoomRegionChanged(zoomRegion());
    update();
    updateGeometry();
}

QPointF ZoomView::zoomPosition() const
{
	QRectF rect = zoomRegion();
	return QPointF(rect.x() + rect.width() / 2, rect.y() + rect.height() / 2);
}

//void ZoomViwer::setZoomFactor(qreal factor)
//{
//	if (factor > 1.0)
//		factor = 1.0;
//	else if (factor < m_minZoomFactor)
//        factor = m_minZoomFactor;
//    m_zoomFactor = factor;
//    emit zoomFactorChanged(m_zoomFactor);

//    /*set zoom rect*/
//    _setZoomRect(factor);
//    emit zoomRegionChanged(zoomRegion());
//    update();
//    updateGeometry();
//}

void ZoomView::setMinZoomFactor(qreal minFactor)
{
	if (minFactor < 0.001)
		minFactor = 0.001;
	else if (minFactor > 1.0)
		minFactor = 1.0;
    m_minZoomFactor = minFactor;
    /*If min zoom factor greater than current factor,
     * update current factor to min zoom factor and repaint*/
    if(m_minZoomFactor >m_zoomFactor){
        m_zoomFactor = m_minZoomFactor;
        //emit zoomFactorChanged(m_zoomFactor);
        _setZoomRect(m_zoomFactor);
        emit zoomRegionChanged(zoomRegion());
        update();
        updateGeometry();
    }
}

ZoomView::~ZoomView()
{

}

void ZoomView::paintEvent(QPaintEvent *event)
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

    //Drawing the rectangle:m_zoomRect
	backgroundPainter.setPen(QColor(255,0,0));
	backgroundPainter.drawRect(QRectF(QPointF(topLeft+m_zoomRect.topLeft()),QSizeF(m_zoomRect.size())));

	backgroundPainter.end();
	QPainter widgetPainter(this);
	widgetPainter.drawImage(0, 0, background);
}

void ZoomView::mousePressEvent(QMouseEvent *event)
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
				else if (x>left && x <= right && y>bottom) {
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

void ZoomView::wheelEvent(QWheelEvent * event)
{
	//qDebug() << "Wheel Event";
	if (event->orientation() == Qt::Horizontal) {
		//Nothing
	}
	else {
        /*Change the zoom factor*/
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
        //emit zoomFactorChanged(m_zoomFactor);
		emit zoomRegionChanged(zoomRegion());
    }
}

void ZoomView::_setZoomRect(qreal factor, const QPointF & leftTopPos)
{
    if(leftTopPos != QPointF()){
        m_zoomRect.setTopLeft(leftTopPos);
    }
    qreal width = m_thumbnail.width();
    qreal height = m_thumbnail.height();
    m_zoomRect.setWidth(width*factor);
    m_zoomRect.setHeight(height*factor);
}

QRectF ZoomView::_regionToRect(const QRect &region)
{
    //Check the region whether satisfies the zoom factor
    //and make a valid region
    //TODO:

    qreal kWidth = (region.width())/static_cast<qreal>(m_originalWidth);
    qreal kHeight = (region.height())/static_cast<qreal>(m_originalHeight);
    qreal kLeft = (region.left())/static_cast<qreal>(m_originalWidth);
    qreal kTop = (region.top())/static_cast<qreal>(m_originalHeight);
    if(kWidth < m_minZoomFactor || kHeight <m_minZoomFactor){
        kWidth = m_minZoomFactor;
        kHeight = m_minZoomFactor;
    }
	return QRectF(
        QPointF(kLeft*m_imageRect.width(), kTop*m_imageRect.height()),
        QSizeF(kWidth*m_imageRect.width(), kHeight*m_imageRect.height())
	);
}


