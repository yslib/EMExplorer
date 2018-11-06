#include "markitem.h"
#include "model/treeitem.h"

#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QDebug>
#include <cmath>

//StrokeMarkItem::StrokeMarkItem(QGraphicsItem * parent, int index, const QString & name, const QColor & color, SliceType type, bool visible) : QGraphicsItem(parent), AbstractMarkItem(name, 0.0, color, type, index, visible)
//{
//}
//
//
//QRectF StrokeMarkItem::unionWith(const QRectF & rect, const QPointF & p)
//{
//	const QPointF &topLeft = rect.topLeft();
//	const QPointF &bottomRight = rect.bottomRight();
//	QPointF newTopLeft = QPointF(std::min(p.x(), topLeft.x()), std::min(p.y(), topLeft.y()));
//	QPointF newBottomRight = QPointF(std::max(p.x(), bottomRight.x()), std::max(p.y(), bottomRight.y()));
//	return QRectF(newTopLeft, newBottomRight);
//}
//
//void StrokeMarkItem::addPoint(const QPointF& p)
//{
//	prepareGeometryChange();
//	m_boundingRect = unionWith(m_boundingRect, p);
//	m_points << p;
//	update();
//}
//
//void StrokeMarkItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
//{
//	Q_UNUSED(painter);
//	Q_UNUSED(option);
//	Q_UNUSED(widget);
//}
//
//
//void StrokeMarkItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
//{
//
//	QGraphicsItem::mousePressEvent(event);
//}
//
//void StrokeMarkItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
//{
//
//	QGraphicsItem::mouseMoveEvent(event);
//}
//
//void StrokeMarkItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
//{
//
//	QGraphicsItem::mouseReleaseEvent(event);
//}
//
//void StrokeMarkItem::wheelEvent(QGraphicsSceneWheelEvent * event)
//{
//
//	QGraphicsItem::wheelEvent(event);
//}

/*!
\internal

Highlights \a item as selected.

NOTE: This function is a duplicate of qt_graphicsItem_highlightSelected() in 
	qgraphicsitem.cpp!
*/

static void drawHighlightSelected(
	QGraphicsItem *item, QPainter *painter, const QStyleOptionGraphicsItem *option)
{
	const QRectF murect = painter->transform().mapRect(QRectF(0, 0, 1, 1));
	if (qFuzzyIsNull(qMax(murect.width(), murect.height())))
		return;

	const QRectF mbrect = painter->transform().mapRect(item->boundingRect());
	if (qMin(mbrect.width(), mbrect.height()) < qreal(1.0))
		return;

	qreal itemPenWidth;
	switch (item->type()) {
	case QGraphicsEllipseItem::Type:
		itemPenWidth = static_cast<QGraphicsEllipseItem *>(item)->pen().widthF();
		break;
	case QGraphicsPathItem::Type:
		itemPenWidth = static_cast<QGraphicsPathItem *>(item)->pen().widthF();
		break;
	case QGraphicsPolygonItem::Type:
		itemPenWidth = static_cast<QGraphicsPolygonItem *>(item)->pen().widthF();
		break;
	case QGraphicsRectItem::Type:
		itemPenWidth = static_cast<QGraphicsRectItem *>(item)->pen().widthF();
		break;
	case QGraphicsSimpleTextItem::Type:
		itemPenWidth = static_cast<QGraphicsSimpleTextItem *>(item)->pen().widthF();
		break;
	case QGraphicsLineItem::Type:
		itemPenWidth = static_cast<QGraphicsLineItem *>(item)->pen().widthF();
		break;
	default:
		itemPenWidth = 1.0;
	}
	const qreal pad = itemPenWidth / 2;
	const qreal penWidth = 0; // cosmetic pen
	const QColor fgcolor = option->palette.windowText().color();
	const QColor bgcolor( // ensure good contrast against fgcolor
		fgcolor.red()   > 127 ? 0 : 255,
		fgcolor.green() > 127 ? 0 : 255,
		fgcolor.blue()  > 127 ? 0 : 255);

	painter->setPen(QPen(bgcolor, penWidth, Qt::SolidLine));
	painter->setBrush(Qt::NoBrush);
	painter->drawRect(item->boundingRect().adjusted(pad, pad, -pad, -pad));

	painter->setPen(QPen(option->palette.windowText(), 0, Qt::DashLine));
	painter->setBrush(Qt::NoBrush);
	painter->drawRect(item->boundingRect().adjusted(pad, pad, -pad, -pad));
}

StrokeMarkItem::StrokeMarkItem(const QPolygonF& path, QGraphicsItem * parent) :
QGraphicsPolygonItem(parent),
m_markInfo(new StrokeMarkItemPrivate)
{
	setPolygon(path);
	updateLength();
}

StrokeMarkItem::StrokeMarkItem(QGraphicsItem * parent) : 
QGraphicsPolygonItem(parent),
m_markInfo(new StrokeMarkItemPrivate)
{
	updateLength();
}

void StrokeMarkItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setPen(pen());
    painter->setBrush(brush());
    if(m_markInfo->isFilled == false){      //None filled
        painter->drawPolyline(polygon());
    }else{
        painter->setBrush(QBrush(pen().color(),Qt::SolidPattern));
        painter->drawPolygon(polygon()); // filled

    }


	if ( option != nullptr && option->state & QStyle::State_Selected)
		drawHighlightSelected(this, painter, option);
}

void StrokeMarkItem::appendPoint(const QPointF& p)
{
	auto poly = polygon();
	poly.append(p);
	setPolygon(poly);
	update(boundingRect());
	updateLength();
}


/**
 * \brief This function is used to set a handler to handle some state change of the item from external
 * 
 * Because QGraphicsItem and its subclasses don't inherit from QObject, they can't emit any signals to 
 * notify any other object. On a count of this, a call back function need to be employed manually.
 * \param handler This is a type of \a std::function<>
 */
void StrokeMarkItem::setItemChangeHandler(
	const std::function<QVariant(StrokeMarkItem* mark, QGraphicsItem::GraphicsItemChange, const QVariant&)>& handler) {
	m_itemChangeHandler = handler;
}

StrokeMarkItem::~StrokeMarkItem()
{
	delete m_markInfo;
	m_markInfo = nullptr;
}

void StrokeMarkItem::createPropertyInfo()
{
	/*const MarkPropertyInfo propertyInfos = {
		qMakePair(MarkProperty::Color,QStringLiteral("Color")),
		qMakePair(MarkProperty::CategoryColor,QStringLiteral("Category Color")),
		qMakePair(MarkProperty::Name,QStringLiteral("ID")),
		qMakePair(MarkProperty::CategoryName,QStringLiteral("Category ID")),
		qMakePair(MarkProperty::SliceIndex,QStringLiteral("Slice Index")),
		qMakePair(MarkProperty::SliceType,QStringLiteral("Slice Type")),
		qMakePair(MarkProperty::VisibleState,QStringLiteral("Visible")),
		qMakePair(MarkProperty::Length,QStringLiteral("Length")),
	};
	setData(MarkProperty::PropertyInfo, QVariant::fromValue(propertyInfos));*/
}

void StrokeMarkItem::updateLength()
{
	const auto  & poly = polygon();
	const int c = poly.count();
	if (c <= 1)
	{
		m_markInfo->m_length = 0;
		return;
	}
	const auto &p0 = poly[c - 2];
	const auto &p1 = poly[c - 1];
	const auto dx = p0.x() - p1.x();
	const auto dy = p0.y() - p1.y();
	m_markInfo->m_length += std::sqrt(dx*dx + dy * dy);
}

/**
 * \brief This is a item state change handler
 * 
 * This handler can receive many state change of the item. You can hand these change in this function
 * 
 * \param change 
 * \param value 
 * \return 
 * 
 * \warning Some member functions can not be called in this handler or it will cause recursively calls
 */
QVariant StrokeMarkItem::itemChange(GraphicsItemChange change, const QVariant& value) {
	if(m_itemChangeHandler != nullptr)
		return m_itemChangeHandler(this,change,value);
	return QGraphicsPolygonItem::itemChange(change, value);
}

QDataStream & operator<<(QDataStream & stream, const StrokeMarkItem * item)
{
	if (item == nullptr || item->type() != ItemTypes::StrokeMark)
		return stream;
	if (item->type() == ItemTypes::StrokeMark)
	{
		stream << static_cast<quint32>(item->type())
			<< item->pen()
			<< item->polygon()
			<< item->m_markInfo;
	}
	return stream;
}

QDataStream & operator>>(QDataStream & stream, StrokeMarkItem *& item)
{
	qint32 type;
	stream >> type;
	Q_ASSERT(stream.status() != QDataStream::ReadPastEnd);
	Q_ASSERT(type == ItemTypes::StrokeMark);
	if(type == ItemTypes::StrokeMark)
	{
		auto newMark = new StrokeMarkItem(nullptr);

		QPen pen;
		QPolygonF poly;

		stream >> pen >> poly >> newMark->m_markInfo;
;
		newMark->setPolygon(poly);
		newMark->setPen(pen);

		item = newMark;
	}
	return stream;
}
