#include "markitem.h"
#include "treeitem.h"
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QDebug>

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
		stream << (qint32)mark->type()
			<< mark->data(MarkProperty::CategoryColor).value<QColor>()
			<< mark->data(MarkProperty::CategoryName).value<QString>()
			<< mark->data(MarkProperty::Color).value<QColor>()
			<< mark->data(MarkProperty::Length).value<double>()
			<< mark->data(MarkProperty::Name).value<QString>()
			<< mark->data(MarkProperty::SliceIndex).value<int>()
			<< mark->data(MarkProperty::SliceType).value<int>()
			<< mark->data(MarkProperty::VisibleState).value<bool>()
			<< mark->polygon()
			<< mark->pen();
	}

}

QDataStream & operator>>(QDataStream & stream, QGraphicsItem *& item)
{
	qint32 type;
	stream >> type;
	Q_ASSERT_X(stream.status() != QDataStream::ReadPastEnd, 
		"QDataStream & operator>>(QDataStream & stream, QGraphicsItem *& item)", "corrupt data");
	Q_ASSERT_X(type == ItemTypes::StrokeMark, 
		"QDataStream & operator>>(QDataStream & stream, QGraphicsItem *& item)", "corrupt data");
	if(type == ItemTypes::StrokeMark)			//There may be a error
	{
		QColor categoryColor;			//QColor
		QString categoryName;			//QString
		QColor color;					//QColor
		double length;				//double
		QString name;					//QString
		int sliceIndex;			//int
		int sliceType;				//int
		bool vis;					//bool
		QPolygonF poly;					//QPolygon
		QPen pen;
		stream >> categoryColor
			>> categoryName
			>> color
			>> length
			>> name
			>> sliceIndex
			>> sliceType
			>> vis
			>> poly
			>> pen;
		Q_ASSERT_X(stream.status() != QDataStream::ReadPastEnd,
			"QDataStream & operator>>(QDataStream & stream, QGraphicsItem *& item)", "corrupt data");
		auto mark = new StrokeMarkItem;
		mark->setData(MarkProperty::CategoryColor, QVariant::fromValue<QColor>(categoryColor));
		mark->setData(MarkProperty::CategoryName, QVariant::fromValue<QString>(categoryName));
		mark->setData(MarkProperty::Color, QVariant::fromValue<QColor>(color));
		mark->setData(MarkProperty::Length, QVariant::fromValue<double>(length));
		mark->setData(MarkProperty::Name, QVariant::fromValue<QString>(name));
		mark->setData(MarkProperty::SliceIndex, QVariant::fromValue<int>(sliceIndex));
		mark->setData(MarkProperty::SliceType, QVariant::fromValue<int>(sliceType));
		mark->setData(MarkProperty::VisibleState, QVariant::fromValue<bool>(vis));
		mark->setPolygon(poly);
		mark->setPen(pen);
		item = mark;
	}
	return stream;
}
