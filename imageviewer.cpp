#include "imageviewer.h"
#include <QLabel>
#include <QLayout>
#include <QWheelEvent>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPolygon>

bool ImageViewer::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == m_topImageLabel) {
		if (event->type() == QEvent::MouseButtonPress) {
			m_topImageEnablePaint = true;
			auto e = static_cast<QMouseEvent*>(event);
			m_prevPaint = e->pos();
			qDebug() << m_prevPaint;
			return true;
		}
		else if (event->type() == QEvent::MouseMove) {
			if (m_topImageEnablePaint == true) {
				auto e = static_cast<QMouseEvent*>(event);
				//update();
				QPoint p = e->pos();
				paintLine(m_prevPaint, p, m_topImageLabel);
				m_prevPaint = p;
			}
			return true;
		}
		else if (event->type() == QEvent::MouseButtonRelease) {
			m_topImageEnablePaint = false;
			return true;
		}
	}
	else if (obj == m_rightImageLabel) {
		if (event->type() == QEvent::MouseButtonPress) {
			m_rightImageEnablePaint = true;
			auto e = static_cast<QMouseEvent*>(event);
			m_prevPaint = e->pos();
			update();
			return true;
		}
		else if (event->type() == QEvent::MouseMove) {

			return true;
		}
		else if (event->type() == QEvent::MouseButtonRelease) {
			m_rightImageEnablePaint = false;
			return true;
		}


	}
	else if (obj == m_frontImageLabel) {
		if (event->type() == QEvent::MouseButtonPress) {
			m_frontImageEnablePaint = true;
			auto e = static_cast<QMouseEvent*>(event);
			m_prevPaint = e->pos();
			update();
			return true;
		}
		else if (event->type() == QEvent::MouseMove) {

			return true;
		}
		else if (event->type() == QEvent::MouseButtonRelease) {
			m_frontImageLabel = false;
			return true;
		}

	}
	return QScrollArea::eventFilter(obj, event);
}

void ImageViewer::wheelEvent(QWheelEvent *event)
{
	if (event->type() == QEvent::Wheel) {
		QWheelEvent *e = static_cast<QWheelEvent*>(event);
		if (e->orientation() == Qt::Horizontal) {

		}
		else {
			if (m_loaded == true) {
				if (e->delta() > 0) {
					zoom(m_factor + 0.25);
				}
				else {
					zoom(m_factor - 0.25);
				}

			}
		}
	}
}

void ImageViewer::paintEvent(QPaintEvent *event)
{

}

ImageViewer::ImageViewer(QWidget *parent) : QScrollArea(parent), m_factor(1.0), m_loaded(false)
{

	m_displayWidget = new QWidget(this);
	m_displayWidget->resize(Width, Height);
	//m_displayWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	m_layout = new QGridLayout(this);
	m_topImageLabel = new QLabel(this);
	m_rightImageLabel = new QLabel(this);
	m_frontImageLabel = new QLabel(this);

	installEventFilter(m_topImageLabel);
	installEventFilter(m_rightImageLabel);
	installEventFilter(m_frontImageLabel);
	//installEventFilter(this);
	//setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
	m_topImageLabel->installEventFilter(this);

	//m_topImageLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
	m_topImageLabel->setScaledContents(true);
	m_rightImageLabel->setScaledContents(true);
	m_frontImageLabel->setScaledContents(true);

	m_displayWidget->setLayout(m_layout);
	m_layout->addWidget(m_topImageLabel, 0, 0);
	m_layout->addWidget(m_rightImageLabel, 0, 1);
	m_layout->addWidget(m_frontImageLabel, 1, 0);

	//m_layout->setSizeConstraint(QLayout::SetFixedSize);
	setVisible(false);
	setBackgroundRole(QPalette::Dark);
	setFocusPolicy(Qt::ClickFocus);
}

void ImageViewer::setTopImage(const QImage &image)
{
	qDebug() << "Set Image";
	m_topImageLabel->setPixmap(QPixmap::fromImage(image));
	//m_topImageLabel->resize(image.size());
	m_topImage = image;
	setVisible(true);
	m_loaded = true;
	updateStretchFactor();
}

void ImageViewer::setRightImage(const QImage &image)
{
	qDebug() << "Set Right Image";
	m_rightImageLabel->setPixmap(QPixmap::fromImage(image));
	m_loaded = true;
	setVisible(true);
	m_rightImage = image;
	updateStretchFactor();
}

void ImageViewer::setFrontImage(const QImage &image)
{
	qDebug() << "Set Front Image";
	m_frontImageLabel->setPixmap(QPixmap::fromImage(image));
	m_loaded = true;
	setVisible(true);
	m_topImage = image;
	updateStretchFactor();
}
void ImageViewer::zoom(qreal factor)
{
	if (factor > 2.00)
		factor = 2.00;
	else if (factor < 0.25)
		factor = 0.25;
	m_factor = factor;
	//qDebug()<<"after adjusting:"<<m_factor;
	//m_imageLabel->resize(factor*m_imageLabel->pixmap()->size());
	m_displayWidget->resize(factor*QSize(Width, Height));
	//qDebug()<<m_topImageLabel->size();
}

void ImageViewer::updateStretchFactor()
{
	//    QSizePolicy topImagePolicy = m_topImageLabel->sizePolicy();
	//    QSizePolicy rightImagePolicy = m_rightImageLabel->sizePolicy();
	//    QSizePolicy frontImagePolicy = m_frontImageLabel->sizePolicy();

	QSize topImageSize;
	if (m_topImageLabel->pixmap() != nullptr) {
		topImageSize = m_topImageLabel->pixmap()->size();
		//   qDebug()<<"aaa";
	}
	QSize rightImageSize;
	if (m_rightImageLabel->pixmap() != nullptr) {
		rightImageSize = m_rightImageLabel->pixmap()->size();
		// qDebug()<<"asdf";
	}
	QSize frontImageSize;
	if (m_frontImageLabel->pixmap() != nullptr) {
		frontImageSize = m_frontImageLabel->pixmap()->size();
		//  qDebug()<<"asdfsadf";
	}

	m_layout->setColumnStretch(0, topImageSize.width());
	m_layout->setColumnStretch(1, rightImageSize.width());
	m_layout->setRowStretch(0, topImageSize.height());
	m_layout->setRowStretch(1, frontImageSize.height());
	//qDebug()<<topImageSize<<" "<<rightImageSize<<" "<<frontImageSize;
	//topImagePolicy.setHorizontalStretch(topImageSize.width());
	//rightImagePolicy.setHorizontalStretch(rightImageSize.width());
	//topImagePolicy.setVerticalStretch(topImageSize.height());
	//frontImagePolicy.setVerticalStretch(frontImageSize.height());
	// m_topImageLabel->setSizePolicy(topImagePolicy);
	// m_rightImageLabel->setSizePolicy(rightImagePolicy);
	// m_frontImageLabel->setSizePolicy(frontImagePolicy);
}

void ImageViewer::paintLine(const QPoint &begin, const QPoint &end, QPaintDevice *dev)
{
	m_painter.begin(dev);
	m_painter.drawLine(begin, end);
	m_painter.end();
}
//bool ImageViewer::event(QEvent *event)
//{
//    qDebug()<<"in event";

//     return QObject::event(event);
//}

ImageView::ImageView(QWidget *parent) :QWidget(parent)
{
	m_view = new GraphicsView(this);
	m_scene = new GraphicsScene(this);
	m_scene->setSceneRect(0, 0, 500, 500);
	m_view->setScene(m_scene);
	m_view->scale(0.1, 0.1);
	m_layout = new QGridLayout(this);
	m_layout->addWidget(m_view);
	setLayout(m_layout);
}

void ImageView::setTopImage(const QImage &image)
{
	m_slice = new SliceItem(QPixmap::fromImage(image));
	m_scene->addItem(m_slice);
	QSize size = image.size();
	m_slice->setPos(-size.width() / 2, -size.height() / 2);
}

void ImageView::setRightImage(const QImage &image)
{

}

void ImageView::setFrontImage(const QImage &image)
{

}
GraphicsView::GraphicsView(QWidget *parent) :QGraphicsView(parent),
m_scaleFactor(0.5),
m_currentPaintItem(nullptr)
{
	scale(m_scaleFactor, m_scaleFactor);
}

void GraphicsView::wheelEvent(QWheelEvent *event) {
	int delta = event->delta();
	if (delta > 0) {
		scale(1.1, 1.1);
	}
	else {
		scale(0.9, 0.9);
	}
	qDebug() << "wheelEvent in View";
	QGraphicsView::wheelEvent(event);

}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
	QPoint viewPos = event->pos();
	QPointF pos = mapToScene(viewPos);
	auto items = scene()->items(pos);
	for (const auto & item : items) {
		SliceItem * slice = qgraphicsitem_cast<SliceItem*>(item);
		//find first slice item on which to draw
		if (slice != nullptr)
		{
			qDebug() << "first slice item on which to draw";
			m_currentPaintItem = slice;
			m_points.clear();
			m_points << viewPos;
			return;
		}
	}
	m_currentPaintItem = nullptr;
	QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	if (m_currentPaintItem != nullptr)
	{
		QPoint viewPos = event->pos();
		m_points << viewPos;
		qDebug() << "add pos of view coords";
		return;
	}
	QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	if(m_currentPaintItem != nullptr)
	{
		if(m_points.empty() == false)
		{
			//draw a polygon and add to scene as the child of current paint item
			QPolygon poly(m_points);
			QPolygonF polyF = mapToScene(poly);
			polyF = m_currentPaintItem->mapFromScene(polyF);
			QGraphicsPolygonItem * polyItem = new QGraphicsPolygonItem(polyF, m_currentPaintItem);
			QBrush aBrush(QColor(12,0,0));
			QPen aPen(aBrush,5,Qt::SolidLine);
			polyItem->setPen(aPen);
			polyItem->setZValue(100);
			qDebug() << "add a polygon on slice item";
			return;
		}
	}
	QGraphicsView::mouseReleaseEvent(event);
}

GraphicsScene::GraphicsScene(QObject *parent) :QGraphicsScene(parent)
{

}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	qDebug() << "mousePressEvent in scene";
	QGraphicsScene::mousePressEvent(event);
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
	qDebug() << "mouseMoveEvent in scene";
	QGraphicsScene::mouseMoveEvent(event);
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
	qDebug() << "mouseReleaseEvent in scene";
	QGraphicsScene::mouseReleaseEvent(event);
}

void GraphicsScene::wheelEvent(QGraphicsSceneWheelEvent * event)
{
	qDebug() << "wheelEvent in scene";
	QGraphicsScene::wheelEvent(event);
}

StrokeMarkItem::StrokeMarkItem(QGraphicsItem * parent):QGraphicsItem(parent)
{

}
void StrokeMarkItem::addPoint(const QPointF& p)
{
	prepareGeometryChange();
	m_boundingRect = unionWith(m_boundingRect,p);
	m_points << p;
	update();
}

void StrokeMarkItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{

}

QRectF StrokeMarkItem::unionWith(const QRectF & rect, const QPointF & p)
{
	const QPointF &topLeft = rect.topLeft();
	const QPointF &bottomRight = rect.bottomRight();
	QPointF newTopLeft= QPointF(std::min(p.x(),topLeft.x()), std::min(p.y(),topLeft.y()));
	QPointF newBottomRight = QPointF(std::max(p.x(), bottomRight.x()), std::max(p.y(),bottomRight.y()));
	return QRectF(newTopLeft, newBottomRight);
}

void StrokeMarkItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	qDebug() << "mousePressEvent in item";
	QGraphicsItem::mousePressEvent(event);
}

void StrokeMarkItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
	qDebug() << "mouseMoveEvent in item";
	QGraphicsItem::mouseMoveEvent(event);
}

void StrokeMarkItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
	qDebug() << "mouseReleaseEvent in item";
	QGraphicsItem::mouseReleaseEvent(event);
}

void StrokeMarkItem::wheelEvent(QGraphicsSceneWheelEvent * event)
{
	qDebug() << "wheelEvent in item";
	QGraphicsItem::wheelEvent(event);
}
void SliceItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	qDebug() << "mousePressEvent in item";
	QGraphicsPixmapItem::mousePressEvent(event);
}

void SliceItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	qDebug() << "mouseMoveEvent in item";
	QGraphicsPixmapItem::mouseMoveEvent(event);
}

void SliceItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	qDebug() << "mouseReleaseEvent in item";
	QGraphicsPixmapItem::mouseReleaseEvent(event);
}

void SliceItem::wheelEvent(QGraphicsSceneWheelEvent* event)
{
	qDebug() << "wheelEvent in item";
	QGraphicsPixmapItem::wheelEvent(event);
}
