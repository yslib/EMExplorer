#include "imageviewer.h"
#include "titledsliderwithspinbox.h"
#include <QToolBar>
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
#include <cassert>
#define saaa(x)  asdfsadf$a;


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

ImageView::ImageView(QWidget *parent) :QWidget(parent), m_topSlice(nullptr), m_rightSlice(nullptr), m_frontSlice(nullptr)
{
	//layout
	m_layout = new QGridLayout(this);

	//QGraphicsView
	m_view = new GraphicsView(this);
	m_layout->addWidget(m_view, 1, 0);


	//m_view->setScene(m_scene);

	//sliders
	m_topSlider = new TitledSliderWithSpinBox(this, tr("Z:"));
	m_rightSlider = new TitledSliderWithSpinBox(this, tr("X:"));
	m_frontSlider = new TitledSliderWithSpinBox(this, tr("Y:"));

	//actions
	m_topSlicePlayAction = new QAction(tr("Play"), this);
	m_topSlicePlayAction->setCheckable(true);
	m_rightSlicePlayAction = new QAction(tr("Play"), this);
	m_rightSlicePlayAction->setCheckable(true);
	m_frontSlicePlayAction = new QAction(tr("Play"), this);
	m_frontSlicePlayAction->setCheckable(true);
    m_markAction = new QAction(tr("Mark"), this);
	m_markAction->setCheckable(true);


	//tool bar
	m_toolBar = new QToolBar(this);

	m_layout->addWidget(m_toolBar, 0, 0);

	m_toolBar->addWidget(m_topSlider);
	m_toolBar->addAction(m_topSlicePlayAction);
	m_toolBar->addWidget(m_rightSlider);
	m_toolBar->addAction(m_rightSlicePlayAction);
	m_toolBar->addWidget(m_frontSlider);
	m_toolBar->addAction(m_frontSlicePlayAction);
	m_toolBar->addAction(m_markAction);


	//
	connect(m_topSlider, SIGNAL(valueChanged(int)), this, SIGNAL(ZSliderChanged(int)));
	connect(m_rightSlider, SIGNAL(valueChanged(int)), this, SIGNAL(YSliderChanged(int)));
	connect(m_frontSlider, SIGNAL(valueChanged(int)), this, SIGNAL(XSliderChanged(int)));

	connect(m_view, SIGNAL(zSliceSelected(const QPoint &)), this, SIGNAL(zSliceSelected(const QPoint &)));
	connect(m_view, SIGNAL(ySliceSelected(const QPoint &)), this, SIGNAL(ySliceSelected(const QPoint &)));
	connect(m_view, SIGNAL(xSliceSelected(const QPoint &)), this, SIGNAL(xSliceSelected(const QPoint &)));

	//action
	connect(m_markAction, SIGNAL(triggered(bool)), m_view, SLOT(paintEnable(bool)));
	connect(m_topSlicePlayAction, SIGNAL(triggered(bool)), this, SLOT(onTopSliceTimer(bool)));
	connect(m_rightSlicePlayAction, SIGNAL(triggered(bool)), this, SLOT(onRightSliceTimer(bool)));
	connect(m_frontSlicePlayAction, SIGNAL(triggered(bool)), this, SLOT(onFrontSliceTimer(bool)));

	setLayout(m_layout);
}

void ImageView::setTopSliceCount(int value)
{
	m_topSlider->setMaximum(value);
}
void ImageView::setRightSliceCount(int value)
{
	m_rightSlider->setMaximum(value);
}
void ImageView::setFrontSliceCount(int value)
{
	m_frontSlider->setMaximum(value);
}
void ImageView::setTopImage(const QImage &image)
{
	m_view->setTopImage(image);
}

void ImageView::setRightImage(const QImage &image)
{
	m_view->setRightImage(image);
}

void ImageView::setFrontImage(const QImage &image)
{
	m_view->setFrontImage(image);
}

int ImageView::getZSliceValue() const
{
	return m_topSlider->value();
}

int ImageView::getYSliceValue() const
{
	return m_rightSlider->value();
}

int ImageView::getXSliceValue() const
{
	return m_frontSlider->value();
}

void ImageView::onTopSliceTimer(bool enable)
{
	if(enable)
	{
		m_topTimerId = startTimer(10);
		m_topSlicePlayDirection = Direction::Forward;
	}else
	{
		killTimer(m_topTimerId);
	}
}

void ImageView::onRightSliceTimer(bool enable)
{
	if(enable)
	{
		m_rightTimerId = startTimer(10);
		m_rightSlicePlayDirection = Direction::Forward;
	}else
	{
		killTimer(m_rightTimerId);
	}
}

void ImageView::onFrontSliceTimer(bool enable)
{
	if(enable)
	{
		m_frontTimerId = startTimer(10);
		m_frontSlicePlayDirection = Direction::Forward;
	}else
	{
		killTimer(m_frontTimerId);
	}
}

void ImageView::timerEvent(QTimerEvent* event)
{
	int timeId = event->timerId();
	if(timeId == m_topTimerId)
	{
		int maxSlice = m_topSlider->maximum();
		int cur = m_topSlider->value();
		if(m_topSlicePlayDirection == Direction::Forward)
		{
			cur++;
		}else
		{
			cur--;
		}
		if(cur >= maxSlice)
		{
			m_topSlicePlayDirection = Direction::Backward;
		}else if(cur <=0)
		{
			m_topSlicePlayDirection = Direction::Forward;
		}
		m_topSlider->setValue(cur);
	}else if(timeId == m_rightTimerId)
	{
		int maxSlice = m_rightSlider->maximum();
		int cur = m_rightSlider->value();
		if (m_rightSlicePlayDirection == Direction::Forward)
		{
			cur++;
		}
		else
		{
			cur--;
		}
		if (cur >= maxSlice)
		{
			m_rightSlicePlayDirection = Direction::Backward;
		}
		else if (cur <= 0)
		{
			m_rightSlicePlayDirection = Direction::Forward;
		}
		m_rightSlider->setValue(cur);

	}else if(timeId == m_frontTimerId)
	{
		int maxSlice = m_frontSlider->maximum();
		int cur = m_frontSlider->value();
		if (m_frontSlicePlayDirection == Direction::Forward)
		{
			cur++;
		}
		else
		{
			cur--;
		}
		if (cur >= maxSlice)
		{
			m_frontSlicePlayDirection = Direction::Backward;
		}
		else if (cur <= 0)
		{
			m_frontSlicePlayDirection = Direction::Forward;
		}
		m_frontSlider->setValue(cur);
	}
}

GraphicsView::GraphicsView(QWidget *parent) :QGraphicsView(parent),
m_scaleFactor(0.5),
m_currentPaintItem(nullptr),
m_paint(false),
m_moveble(true),
m_topSlice(nullptr),
m_rightSlice(nullptr),
m_frontSlice(nullptr)
{
	m_scene = new GraphicsScene(this);
	setScene(m_scene);
	scale(m_scaleFactor, m_scaleFactor);
}

void GraphicsView::wheelEvent(QWheelEvent *event) {
	double numDegrees = -event->delta() / 8.0;
	double numSteps = numDegrees / 15.0;
	double factor = std::pow(1.125, numSteps);
	scale(factor, factor);
	//int delta = event->delta();
	//if (delta > 0) {
	//	scale(1.1, 1.1);
	//}
	//else {
	//	scale(0.9, 0.9);
	//}
	//qDebug() << "wheelEvent in View";
}
void GraphicsView::mousePressEvent(QMouseEvent *event)
{
	QPoint viewPos = event->pos();
	QPointF pos = mapToScene(viewPos);
	m_prevScenePoint = pos;
	auto items = scene()->items(pos);
	for (const auto & item : items) {
		SliceItem * slice = qgraphicsitem_cast<SliceItem*>(item);
		QPoint itemPoint = slice->mapFromScene(pos).toPoint();
		if(slice ==m_topSlice)
		{
			qDebug() << "emit z slice point" << itemPoint;
			emit zSliceSelected(itemPoint);
		}else if(slice == m_rightSlice)
		{
			emit ySliceSelected(itemPoint);
		}else if(slice == m_frontSlice)
		{
			emit xSliceSelected(itemPoint);
		}
		//find first slice item on which to draw
		if (slice != nullptr)
		{
			if (m_paint == true)
				m_currentPaintItem = slice;
			m_paintViewPointsBuffer.clear();
			m_paintViewPointsBuffer << viewPos;
			return;
		}
	}
	m_currentPaintItem = nullptr;

	QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	if (m_paint == true)
	{
		if (m_currentPaintItem != nullptr)
		{
			QPoint viewPos = event->pos();
			m_paintViewPointsBuffer << viewPos;
			return;
		}
	}
	else
	{
		QPointF currentScenePoint = mapToScene(event->pos());
		QPointF delta = currentScenePoint - m_prevScenePoint;
		m_prevScenePoint = currentScenePoint;
		auto items = scene()->items();
		for (const auto & item : items)
		{
			SliceItem * sliceItem = qgraphicsitem_cast<SliceItem*>(item);
			if (sliceItem != nullptr)
			{
				sliceItem->setPos(sliceItem->pos() + delta);
			}
		}
	}
	QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_paint == true)
	{
		if (m_currentPaintItem != nullptr)
		{
			if (m_paintViewPointsBuffer.empty() == false)
			{
				//draw a polygon and add to scene as the child of current paint item
				QPolygon poly(m_paintViewPointsBuffer);
				QPolygonF polyF = mapToScene(poly);

				polyF = m_currentPaintItem->mapFromScene(polyF);
				QGraphicsPolygonItem * polyItem = new QGraphicsPolygonItem(polyF, m_currentPaintItem);
				QBrush aBrush(QColor(12, 0, 0));
				QPen aPen(aBrush, 5, Qt::SolidLine);
				polyItem->setPen(aPen);
				polyItem->setZValue(100);
				return;
			}
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

StrokeMarkItem::StrokeMarkItem(QGraphicsItem * parent) :QGraphicsItem(parent)
{

}
void StrokeMarkItem::addPoint(const QPointF& p)
{
	prepareGeometryChange();
	m_boundingRect = unionWith(m_boundingRect, p);
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
	QPointF newTopLeft = QPointF(std::min(p.x(), topLeft.x()), std::min(p.y(), topLeft.y()));
	QPointF newBottomRight = QPointF(std::max(p.x(), bottomRight.x()), std::max(p.y(), bottomRight.y()));
	return QRectF(newTopLeft, newBottomRight);
}

void StrokeMarkItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{

	QGraphicsItem::mousePressEvent(event);
}

void StrokeMarkItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{

	QGraphicsItem::mouseMoveEvent(event);
}

void StrokeMarkItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{

	QGraphicsItem::mouseReleaseEvent(event);
}

void StrokeMarkItem::wheelEvent(QGraphicsSceneWheelEvent * event)
{

	QGraphicsItem::wheelEvent(event);
}
void SliceItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{

	QGraphicsPixmapItem::mousePressEvent(event);
}

void SliceItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{

	QGraphicsPixmapItem::mouseMoveEvent(event);
}

void SliceItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{

	QGraphicsPixmapItem::mouseReleaseEvent(event);
}

void SliceItem::wheelEvent(QGraphicsSceneWheelEvent* event)
{

	QGraphicsPixmapItem::wheelEvent(event);
}

void GraphicsView::setTopImage(const QImage & image)
{
	if (m_topSlice == nullptr)
	{
		m_topSlice = new SliceItem(QPixmap::fromImage(image));
		m_topSlice->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
		m_scene->addItem(m_topSlice);
	}
	else
	{
		m_topSlice->setPixmap(QPixmap::fromImage(image));
	}
	QSize size = image.size();
	m_topSlice->setPos(-size.width() / 2, -size.height() / 2);
}

void GraphicsView::setRightImage(const QImage & image)
{
	if (m_rightSlice == nullptr)
	{
		m_rightSlice = new SliceItem(QPixmap::fromImage(image));
		m_scene->addItem(m_rightSlice);
		m_rightSlice->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
	}
	else
	{
		m_rightSlice->setPixmap(QPixmap::fromImage(image));
	}
	assert(m_topSlice != nullptr);
	QSize size = m_topSlice->pixmap().size();
	m_rightSlice->setPos(m_topSlice->pos() + QPointF(size.width() + 40, 0));

}

void GraphicsView::setFrontImage(const QImage & image)
{
	if (m_frontSlice == nullptr)
	{
		m_frontSlice = new SliceItem(QPixmap::fromImage(image));
		m_scene->addItem(m_frontSlice);
		m_frontSlice->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
	}
	else
	{
		m_frontSlice->setPixmap(QPixmap::fromImage(image));
	}
	assert(m_topSlice != nullptr);
	QSize size = m_topSlice->pixmap().size();
	m_frontSlice->setPos(m_topSlice->pos() + QPointF(0, size.height() + 40));
}
