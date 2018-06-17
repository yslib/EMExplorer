#include <QWheelEvent>
#include <QDebug>

#include "sliceview.h"
#include "globals.h"
#include "sliceitem.h"
#include "markitem.h"

SliceView::SliceView(QWidget *parent) :QGraphicsView(parent),
m_scaleFactor(0.5),
m_currentPaintItem(nullptr),
m_paint(false),
m_moveble(true),
m_color(Qt::black),
m_slice(nullptr),
m_paintingItem(nullptr)
{
	//setScene(new SliceScene(this));
	setScene(new QGraphicsScene(this));

	scale(m_scaleFactor, m_scaleFactor);

}

void SliceView::setMarks(const QList<QGraphicsItem*>& items)
{
	set_mark_helper_(items);
}
void SliceView::wheelEvent(QWheelEvent *event) {
	double numDegrees = -event->delta() / 8.0;
	double numSteps = numDegrees / 15.0;
	double factor = std::pow(1.125, numSteps);
	scale(factor, factor);
}
void SliceView::focusInEvent(QFocusEvent* event)
{
	Q_UNUSED(event);
	this->setStyleSheet("border:2px solid red");
}

void SliceView::focusOutEvent(QFocusEvent* event)
{
	Q_UNUSED(event);
	this->setStyleSheet("");
}
void SliceView::mousePressEvent(QMouseEvent *event)
{
	QPoint viewPos = event->pos();
	QPointF pos = mapToScene(viewPos);
	m_prevScenePoint = pos;
	auto items = scene()->items(pos);
	for (const auto & item : items) {
		SliceItem * slice = qgraphicsitem_cast<SliceItem*>(item);
		if (slice == m_slice)
		{
			QPoint itemPoint = slice->mapFromScene(pos).toPoint();
			emit sliceSelected(itemPoint);
			if (m_paint == true)
			{
				m_currentPaintItem = slice;
				m_paintingItem = new StrokeMarkItem(m_currentPaintItem);
				m_paintingItem->setPen(QPen(QBrush(m_color), 5, Qt::SolidLine));
				m_paintingItem->appendPoint(itemPoint);
			}
			m_paintViewPointsBuffer.clear();
			m_paintViewPointsBuffer << viewPos;
			return;
		}
	}
	m_currentPaintItem = nullptr;

	QGraphicsView::mousePressEvent(event);
}

void SliceView::mouseMoveEvent(QMouseEvent *event)
{
	if (m_paint == true)
	{
		if (m_currentPaintItem != nullptr)
		{
			QPoint viewPos = event->pos();
			m_paintingItem->appendPoint(m_currentPaintItem->mapFromScene(mapToScene(viewPos)));
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

void SliceView::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_paint == true)
	{
		if (m_currentPaintItem != nullptr)
		{
			m_paintingItem->appendPoint(m_currentPaintItem->mapFromScene(mapToScene(event->pos())));
			
			if (m_currentPaintItem == m_slice)
				emit markAdded(m_paintingItem);

			//if (m_paintViewPointsBuffer.empty() == false)
			//{
			//	//draw a polygon and add to scene as the child of current paint item
			//	QPolygon poly(m_paintViewPointsBuffer);
			//	QPolygonF polyF = mapToScene(poly);
			//	polyF = m_currentPaintItem->mapFromScene(polyF);
			//	auto polyItem = new QGraphicsPolygonItem(polyF, m_currentPaintItem);
			//	QBrush aBrush(m_color);
			//	QPen aPen(aBrush, 5, Qt::SolidLine);
			//	polyItem->setPen(aPen);
			//	polyItem->setZValue(100);
			//	//emit
			//	if (m_currentPaintItem == m_slice)
			//		emit markAdded(polyItem);
			//	return;
			//}
		}
	}
	QGraphicsView::mouseReleaseEvent(event);
}
void SliceView::set_image_helper_(const QPoint& pos, const QImage& inImage, SliceItem*& sliceItem, QImage * outImage)
{
	if (sliceItem == nullptr)
	{
		sliceItem = new SliceItem(QPixmap::fromImage(inImage));
		(sliceItem)->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
		sliceItem->setPos(pos);
		scene()->addItem(sliceItem);
	}
	else
	{
		sliceItem->setPixmap(QPixmap::fromImage(inImage));
	}
	QSize size = inImage.size();
	*outImage = inImage;
}
inline
void SliceView::set_mark_helper_(
	const QList<QGraphicsItem*>& items)
{
	foreach(QGraphicsItem * item, items)
		item->setVisible(item->data(MarkProperty::VisibleState).toBool());
}
inline
void SliceView::clear_slice_marks_helper_(SliceItem * slice)
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
void SliceView::setImage(const QImage& image)
{
	//set_image_helper(image);
	QSize size = image.size();
	QPoint pos = QPoint(-size.width() / 2, -size.height() / 2);
	set_image_helper_(pos, image, m_slice, &m_image);
}

void SliceView::clearSliceMarks()
{
	clear_slice_marks_helper_(m_slice);
}

