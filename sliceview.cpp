#include <QWheelEvent>
#include <QDebug>

#include "sliceview.h"
#include "globals.h"
#include "sliceitem.h"
#include "markitem.h"

SliceView::SliceView(QWidget *parent) :QGraphicsView(parent),
m_scaleFactor(0.5),
m_currentPaintingSlice(nullptr),
m_paint(false),
m_selection(false),
m_pen(QPen(Qt::black,5,Qt::SolidLine)),
m_slice(nullptr),
m_paintingItem(nullptr),
m_state(0),
m_anchorItem(nullptr)
{
	setScene(new QGraphicsScene(this));
	scale(m_scaleFactor, m_scaleFactor);
	connect(scene(), &QGraphicsScene::selectionChanged, this, &SliceView::selectionChanged);
	m_anchorItem = new QGraphicsPixmapItem(createAnchorItemPixmap());
	m_anchorItem->setVisible(false);
	setStyleSheet(QStringLiteral("border:0px solid white"));
}

void SliceView::setMarks(const QList<QGraphicsItem*>& items)
{
	set_mark_helper_(items);
}
void SliceView::wheelEvent(QWheelEvent *event) {
	//double numDegrees = -event->delta() / 8.0;
	//double numSteps = numDegrees / 15.0;
	//double factor = std::pow(1.125, numSteps);
	//scale(factor, factor);
	//QGraphicsView::wheelEvent(event);
	event->accept();
}
void SliceView::focusInEvent(QFocusEvent* event)
{
	Q_UNUSED(event);
	setStyleSheet(QStringLiteral("border:2px solid red"));
}

void SliceView::focusOutEvent(QFocusEvent* event)
{
	Q_UNUSED(event);
	setStyleSheet(QStringLiteral("border:0px solid white"));
}


void SliceView::mousePressEvent(QMouseEvent *event)
{
	if (m_state == None)
		return;
	QPoint viewPos = event->pos();
	QPointF pos = mapToScene(viewPos);
	m_prevScenePoint = pos;
	auto items = scene()->items(pos);
	Qt::MouseButton button = event->button();

	if(button == Qt::LeftButton)			//locate the anchor for left button click
	{
		m_anchorItem->setPos(m_slice->mapFromScene(pos).toPoint());
		m_anchorItem->setVisible(true);
	}

	for (const auto & item : items) {
		if (m_state == OperationState::Paint
			&&button == Qt::LeftButton)
		{
			SliceItem * slice = qgraphicsitem_cast<SliceItem*>(item);
			if (slice == m_slice)
			{
				QPoint itemPoint = slice->mapFromScene(pos).toPoint();
				emit sliceSelected(itemPoint);

				m_currentPaintingSlice = slice;
				m_paintingItem = new StrokeMarkItem(m_currentPaintingSlice);
				m_paintingItem->setFlags(QGraphicsItem::ItemIsSelectable);
				m_paintingItem->setPen(m_pen);
				m_paintingItem->appendPoint(itemPoint);

				//m_paintViewPointsBuffer.clear();
				//m_paintViewPointsBuffer << viewPos;
				event->accept();
				return;	//In painting state, We only find the click position on slice and do nothing else
			}
		}
		else if (m_state == OperationState::Selection
			&& button == Qt::LeftButton)
		{
			SliceItem * slice = qgraphicsitem_cast<SliceItem*>(item);
			if (slice == m_slice /*or slice != nullptr*/)
			{
				emit sliceSelected(slice->mapFromScene(pos).toPoint());
			}
			//In the state,We could select only one mark and apply default mouse 
			//click event on base class for the rubber mode
			StrokeMarkItem * mark = qgraphicsitem_cast<StrokeMarkItem*>(item);
			if (mark != nullptr&&mark->isVisible())			//select a visible mark
			{
				///TODO::Do something for the selected visible marks
				//select one 
				//qDebug() << "Select";
				//mark->setSelected(true);
				return QGraphicsView::mousePressEvent(event);
			}

		}
		else if (m_state == OperationState::Move && button == Qt::LeftButton)
		{
			event->accept();
			return;
		}
	}
	QGraphicsView::mousePressEvent(event);
}

void SliceView::mouseMoveEvent(QMouseEvent *event)
{
	if (m_state == None)
		return;
	if (m_state == OperationState::Paint)		// on drawing a mark
	{
		if (m_currentPaintingSlice != nullptr)
		{
			QPoint viewPos = event->pos();
			m_paintingItem->appendPoint(m_currentPaintingSlice->mapFromScene(mapToScene(viewPos)));
			m_paintViewPointsBuffer << viewPos;
			return;
		}
	}
	else if (m_state == OperationState::Move)	//move the slice
	{
		QPointF currentScenePoint = mapToScene(event->pos());
		QPointF delta = currentScenePoint - m_prevScenePoint;
		m_prevScenePoint = currentScenePoint;
		auto items = scene()->items();
		for (const auto & item : items)
		{
			SliceItem * sliceItem = qgraphicsitem_cast<SliceItem*>(item);
			if (sliceItem == m_slice)
			{
				sliceItem->setPos(sliceItem->pos() + delta);
				emit sliceMoved(delta);
				event->accept();
				return;
			}
		}
		return QGraphicsView::mouseMoveEvent(event);
	}
	else if (m_state == OperationState::Selection)
	{
		QGraphicsView::mouseMoveEvent(event);
	}
	QGraphicsView::mouseMoveEvent(event);
}

void SliceView::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_state == None)
		return;
	Qt::MouseButton button = event->button();
	if (button == Qt::LeftButton && m_state == OperationState::Paint)			//create a mark
	{
		Q_ASSERT_X(m_currentPaintingSlice, 
			"SliceView::mouseReleaseEvent", "null pointer");
		m_paintingItem->appendPoint(m_currentPaintingSlice->mapFromScene(mapToScene(event->pos())));
		if (m_currentPaintingSlice == m_slice)
			emit markAdded(m_paintingItem);
		m_currentPaintingSlice = nullptr;
		return;
	}else if(button == Qt::LeftButton && m_state == OperationState::Move)
	{
		QGraphicsView::mouseReleaseEvent(event);
	}else if(button == Qt::LeftButton && m_state == OperationState::Selection)
	{
		QGraphicsView::mouseReleaseEvent(event);
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
		m_anchorItem->setParentItem(sliceItem);
		scene()->addItem(sliceItem);
		//scene()->setSceneRect(0,0,5000,5000);
		//qDebug() << scene()->sceneRect();
	}
	else
	{
		sliceItem->setPixmap(QPixmap::fromImage(inImage));
	}
	QSize size = inImage.size();
	*outImage = inImage;
}
QGraphicsItem * SliceView::createMarkItem()
{
	return nullptr;
}

QPixmap SliceView::createAnchorItemPixmap(const QString & fileName)
{
	int length = 12;
	QRect target(0, 0, length, length);

	QPixmap pixmap(target.size());
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	
	if(fileName.isEmpty() == false)
	{
		QPixmap image(fileName);
		painter.drawPixmap(target, image, image.rect());
	}else
	{
		painter.setPen(QPen(Qt::yellow, 2, Qt::SolidLine));
		painter.drawLine(0, length / 2, length, length / 2);
		painter.drawLine(length / 2, 0, length / 2, length);
	}
	return pixmap;
}

inline
void SliceView::set_mark_helper_(
	const QList<QGraphicsItem*>& items)
{
	foreach(QGraphicsItem * item, items)
	{
		item->setVisible(item->data(MarkProperty::VisibleState).toBool());
		item->setParentItem(m_slice);
	}

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

QList<QGraphicsItem*> SliceView::selectedItems() const
{
	return scene()->selectedItems();
}

int SliceView::selectedItemCount() const
{
	return scene()->selectedItems().size();
}

void SliceView::moveSlice(const QPointF& dir)
{
	m_slice->moveBy(dir.x(), dir.y());
}

QSize SliceView::sizeHint() const
{
	return m_image.size()*0.45;
}

//
//QSize SliceView::sizeHint() const
//{
//	return QAbstractScrollArea::sizeHint();
//}

