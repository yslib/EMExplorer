#include <QWheelEvent>
#include <QDebug>

#include "slicewidget.h"
#include "globals.h"
#include "model/sliceitem.h"
#include "model/markitem.h"

SliceWidget::SliceWidget(QWidget *parent) :QGraphicsView(parent),
m_scaleFactor(0.5),
m_currentPaintingSlice(nullptr),
m_paint(false),
m_selection(false),
m_pen(QPen(Qt::black, 5, Qt::SolidLine)),
m_slice(nullptr),
m_paintingItem(nullptr),
m_state(Operation::None),
m_anchorItem(nullptr),
m_paintNavigationView(false)
{

	setScene(new QGraphicsScene(this));

	scale(m_scaleFactor, m_scaleFactor);

	connect(scene(), &QGraphicsScene::selectionChanged, this, &SliceWidget::selectionChanged);

	setTransformationAnchor(QGraphicsView::NoAnchor);

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	setDragMode(QGraphicsView::RubberBandDrag);

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	const auto pixel = createAnchorItemPixmap();
	m_anchorItem = new QGraphicsPixmapItem(pixel);
	//const auto p = pixel.size();
	//const QPointF point(p.width()/2.0,p.height()/2.0);
	//m_anchorItem->setTransformOriginPoint(point);
	
	m_anchorItem->setVisible(false);

	setStyleSheet(QStringLiteral("border:0px solid white"));
}

void SliceWidget::setMarks(const QList<StrokeMarkItem*>& items)
{
	setMarkHelper(items);
}
void SliceWidget::wheelEvent(QWheelEvent *event) {
	event->accept();
}
void SliceWidget::focusInEvent(QFocusEvent* event)
{
	Q_UNUSED(event);
	setStyleSheet(QStringLiteral("border:1px solid red"));
	emit sliceSelected();
}

void SliceWidget::focusOutEvent(QFocusEvent* event)
{
	Q_UNUSED(event);
	setStyleSheet(QStringLiteral("border:0px solid white"));
}

void SliceWidget::paintEvent(QPaintEvent* event)
{
	QGraphicsView::paintEvent(event);

	if (m_paintNavigationView == false)
		return;

	const auto & sliceRectInScene = m_slice->mapRectToScene(m_slice->boundingRect());
	const auto & viewRectInScene = mapToScene(rect()).boundingRect();
	if (viewRectInScene.contains(sliceRectInScene))
		return;
	//const auto & scRect = sceneRect();
	const auto & scRect = m_slice->mapRectToScene(m_slice->boundingRect());
	QImage thumbnail(QSize(200, 200), QImage::Format_RGB32);
	QPainter p0(&thumbnail);
	render(&p0, thumbnail.rect(), mapFromScene(scRect).boundingRect());		//rendering the scene image
	p0.end();
	const auto & navigationRect = thumbnail.rect();
	const auto f1 = navigationRect.width() / scRect.width(), f2 = navigationRect.height() / scRect.height();
	QPainter p(&thumbnail);

	p.setPen(QPen(Qt::red, 2));			// TODO:: There may be a bug

	p.drawRect(QRect(
		f1 * (viewRectInScene.x() - scRect.x()),			//transform from view rectangle to thumbnail rectangle
		f2 * (viewRectInScene.y() - scRect.y()),
		f1 * viewRectInScene.width(),
		f2 * viewRectInScene.height()));			//draw the zoom rectangle onto the thumbnail

	p.drawRect(thumbnail.rect());
	p.end();
	QPainter p2(this->viewport());//draw the zoom image
	const auto s = size();
	p2.drawPixmap(thumbnailRect(sliceRectInScene, viewRectInScene), QPixmap::fromImage(thumbnail));
	p2.end();
}

void SliceWidget::mousePressEvent(QMouseEvent *event)
{
	//if (event->isAccepted())
	//	return;

	const auto button = event->button();
	const auto viewPos = event->pos();
	const auto scenePos = mapToScene(viewPos);

	m_prevViewPoint = viewPos;

	auto items = scene()->items(scenePos);

	if (button == Qt::RightButton) {
		event->accept();
		return;
	}

	for (const auto & item : items) {
		auto * itm = qgraphicsitem_cast<SliceItem*>(item);
		if (itm == m_slice) {	 // Operations must perform on slice item.

			if (button == Qt::RightButton) {			// ignore right button event
				event->accept();
				return;
			}

			const auto itemPoint = m_slice->mapFromScene(scenePos);

			emit sliceSelected(itemPoint.toPoint());

			if (m_state == Operation::Paint) {

				m_currentPaintingSlice = m_slice;

				m_paintingItem = new StrokeMarkItem(m_currentPaintingSlice);

				m_paintingItem->setFlags(QGraphicsItem::ItemIsSelectable);
				m_paintingItem->setPen(m_pen);
				m_paintingItem->appendPoint(itemPoint);

				event->accept();
				return;
			}
			else if (m_state == Operation::Selection) {

				// Selecting items automatically by calling default event handler of the QGraphicsView
				return QGraphicsView::mousePressEvent(event);
			}
			else if (m_state == Operation::None) {
				// Set Anchor
				const auto siz = m_anchorItem->pixmap().size();
				const auto originItemPoint = QPointF(siz.width()/2.0,siz.height()/2.0);
				m_anchorItem->setPos(itemPoint-originItemPoint);
				m_anchorItem->setVisible(true);
				event->accept();
				return;
			}

		}
	}
	event->accept();
	return;
}

void SliceWidget::mouseMoveEvent(QMouseEvent *event)
{
	// Note that the returned value for event->button() is always Qt::NoButton for mouse move events.
	const auto viewPos = event->pos();
	//const auto scenePos = mapToScene(viewPos);

	if (event->buttons() == Qt::RightButton) {		// Move
		const auto delta = viewPos - m_prevViewPoint;
		m_prevViewPoint = viewPos;
		translate(delta.x(), delta.y());
		emit viewMoved(delta);
		event->accept();
		return;
	}
	else if (m_state == Operation::Paint)		// Drawing a mark
	{
		if (m_currentPaintingSlice != nullptr)
		{
			m_paintingItem->appendPoint(m_currentPaintingSlice->mapFromScene(mapToScene(viewPos)));
			m_paintViewPointsBuffer << viewPos;
			event->accept();
			return;
		}
	}
	else if (m_state == Operation::Selection)
	{
		return QGraphicsView::mouseMoveEvent(event);
	}

	QGraphicsView::mouseMoveEvent(event);
}

void SliceWidget::mouseReleaseEvent(QMouseEvent *event)
{

	if (m_state == None)
		return;

	if (event->button() == Qt::RightButton) {
		event->accept();
		return;
	}

	Qt::MouseButton button = event->button();
	if (m_state == Operation::Paint)			//create a mark
	{
		if (m_currentPaintingSlice == nullptr) {
			event->accept();
			return;
		}
		m_paintingItem->appendPoint(m_currentPaintingSlice->mapFromScene(mapToScene(event->pos())));
		if (m_currentPaintingSlice == m_slice)
			emit markAdded(m_paintingItem);
		m_currentPaintingSlice = nullptr;

		event->accept();
		return;
	}

	QGraphicsView::mouseReleaseEvent(event);
}



void SliceWidget::setImageHelper(const QPoint& pos, const QImage& inImage, SliceItem*& sliceItem, QImage * outImage)
{
	if (sliceItem == nullptr)
	{
		sliceItem = new SliceItem(QPixmap::fromImage(inImage));

		(sliceItem)->setFlag(QGraphicsItem::ItemClipsChildrenToShape);

		sliceItem->setPos(pos);
		m_anchorItem->setParentItem(sliceItem);
		scene()->addItem(sliceItem);
		/**
		 *We need to give a exactly scene rect according to the image size for efficiency rendering.
		 *We assume that the size of rect of the scene is two times larger than the size of image.
		 */
		auto rect = inImage.rect();
		//auto rectInView = mapFromScene(rect).boundingRect();
		translate(rect.width() / 2, rect.height() / 2);

		rect.adjust(-rect.width(), -rect.height(), 0, 0);

		scene()->setSceneRect(QRectF(-10000,-10000,20000,20000));
	}
	else
	{
		sliceItem->setPixmap(QPixmap::fromImage(inImage));
	}
	QSize size = inImage.size();
	*outImage = inImage;
}

QRect SliceWidget::thumbnailRect(const QRectF & sliceRect, const QRectF & viewRect)
{
	const int w = 0.2*width(), h = 0.2*height();
	const int W = width(), H = height();
	//if(sliceRect.contains(viewRect))
	//{
	//	return QRect(0,0,w,h);
	//}else if(sliceRect.contains(viewRect.topLeft()))
	//{
	//	return QRect(W-w,H-h,w,h);
	//}else if(sliceRect.contains(viewRect.bottomLeft()))
	//{
	//	return QRect(W-w,0,w,h);
	//}else if(sliceRect.contains(viewRect.topRight()))
	//{
	//	
	//}else if(sliceRect.contains(viewRect.bottomRight()))
	//{
	//	return QRect(0, 0, w, h);
	//}
	//return QRect(0,0,w,h);
	return QRect(0, H - h, w, h);
}

QGraphicsItem * SliceWidget::createMarkItem()
{
	return nullptr;
}

QPixmap SliceWidget::createAnchorItemPixmap(const QString & fileName)
{
	const auto length = 12;
	QRect target(0, 0, length, length);

	QPixmap pixmap(target.size());
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);

	if (fileName.isEmpty() == false)
	{
		QPixmap image(fileName);
		painter.drawPixmap(target, image, image.rect());
	}
	else
	{
		painter.setPen(QPen(Qt::yellow, 2, Qt::SolidLine));
		painter.drawLine(0, length / 2, length, length / 2);
		painter.drawLine(length / 2, 0, length / 2, length);
	}
	return pixmap;
}

inline
void SliceWidget::setMarkHelper(
	const QList<StrokeMarkItem*>& items)
{
	foreach(QGraphicsItem * item, items)
	{
		item->setVisible(item->data(MarkProperty::VisibleState).toBool());
		item->setParentItem(m_slice);
	}

}


inline
void SliceWidget::clearSliceMarksHelper(SliceItem * slice)
{
	if (slice == nullptr)
	{
		qWarning("Top slice is empty.");
		return;
	}
	auto children = slice->childItems();
	foreach(QGraphicsItem * item, children)
	{
		item->setVisible(false);
	}
}
void SliceWidget::setImage(const QImage& image)
{
	//set_image_helper(image);
	const auto size = image.size();
	const auto pos = QPoint(-size.width()/2, -size.height() / 2);
	setImageHelper(pos, image, m_slice, &m_image);
}

void SliceWidget::clearSliceMarks()
{
	clearSliceMarksHelper(m_slice);
}

QList<StrokeMarkItem*> SliceWidget::selectedItems() const
{
	Q_ASSERT_X(scene(), "SliceWidget::selectedItems", "null pointer");
	const auto items = scene()->selectedItems();
	QList<StrokeMarkItem*> marks;
	for (const auto item : items)
		marks << qgraphicsitem_cast<StrokeMarkItem*>(item);
	return marks;
}

int SliceWidget::selectedItemCount() const
{
	Q_ASSERT_X(scene(), "SliceWidget::selectedItems", "null pointer");
	return scene()->selectedItems().size();
}

void SliceWidget::moveSlice(const QPointF& dir)
{
	Q_ASSERT_X(m_slice, "SliceWidget::moveSlice", "null pointer");
	m_slice->moveBy(dir.x(), dir.y());
}

QSize SliceWidget::sizeHint() const
{
	//const auto pSize = parentWidget()->size();

	const auto maxLength = std::max(m_image.width(), m_image.height());

	if (maxLength < 800)
		return m_image.size();
	return m_image.size().scaled(800, 800, Qt::KeepAspectRatio);

}
