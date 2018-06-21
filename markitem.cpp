#include "markitem.h"
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include "treeitem.h"

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

StrokeMarkItem::StrokeMarkItem(const QPolygonF& path, QGraphicsItem * parent) :QGraphicsPolygonItem(parent)
{
}

StrokeMarkItem::StrokeMarkItem(QGraphicsItem * parent) : QGraphicsPolygonItem(parent)
{
}

void StrokeMarkItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	painter->setPen(pen());
	painter->setBrush(brush());
	painter->drawPolyline(polygon());
}

void StrokeMarkItem::appendPoint(const QPointF& p)
{
	auto poly = polygon();
	poly.append(p);
	setPolygon(poly);
	update(boundingRect());
}
QDataStream & operator<<(QDataStream & stream, const QGraphicsItem * item)
{
	if (item == nullptr || item->type() != ItemTypes::StrokeMark)
		return stream;
	if (item->type() == ItemTypes::StrokeMark)
	{
		auto mark = static_cast<const StrokeMarkItem*>(item);
		stream << mark->type()
			<< mark->data(MarkProperty::CategoryColor)
			<< mark->data(MarkProperty::CategoryName)
			<< mark->data(MarkProperty::Color)
			<< mark->data(MarkProperty::Length)
			<< mark->data(MarkProperty::Name)
			<< mark->data(MarkProperty::SliceIndex)
			<< mark->data(MarkProperty::SliceType)
			<< mark->data(MarkProperty::VisibleState)
			<< mark->polygon();
	}

}

QDataStream & operator>>(QDataStream & stream, QGraphicsItem *& item)
{
	int type;
	stream >> type;
	Q_ASSERT_X(stream.status() != QDataStream::ReadPastEnd, 
		"QDataStream & operator>>(QDataStream & stream, QGraphicsItem *& item)", "corrupt data");
	Q_ASSERT_X(type != ItemTypes::StrokeMark, 
		"QDataStream & operator>>(QDataStream & stream, QGraphicsItem *& item)", "corrupt data");
	if(type == ItemTypes::StrokeMark)			//There may be a error
	{
		QVariant categoryColor;			//QColor
		QVariant categoryName;			//QString
		QVariant color;					//QColor
		QVariant length;				//double
		QVariant name;					//QString
		QVariant sliceIndex;			//int
		QVariant sliceType;				//int
		QVariant vis;					//bool
		QPolygonF poly;					//QPolygonF
		stream >> categoryColor 
		>> categoryName
		>> color 
		>> length 
		>> name 
		>> sliceIndex
		>> sliceType 
		>> vis 
		>> poly;
		Q_ASSERT_X(stream.status() != QDataStream::ReadPastEnd,
			"QDataStream & operator>>(QDataStream & stream, QGraphicsItem *& item)", "corrupt data");
		auto mark = new StrokeMarkItem;
		mark->setData(MarkProperty::CategoryColor, categoryColor);
		mark->setData(MarkProperty::CategoryName, categoryName);
		mark->setData(MarkProperty::Color, color);
		mark->setData(MarkProperty::Length, length);
		mark->setData(MarkProperty::Name, name);
		mark->setData(MarkProperty::SliceIndex, sliceIndex);
		mark->setData(MarkProperty::SliceType, sliceType);
		mark->setData(MarkProperty::VisibleState, vis);
		mark->setPolygon(poly);
		item = mark;
	}
	return stream;
}
