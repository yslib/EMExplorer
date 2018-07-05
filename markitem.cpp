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

StrokeMarkItem::StrokeMarkItem(const QPolygonF& path, QGraphicsItem * parent) :QGraphicsPolygonItem(parent)
{
	createPropertyInfo();
}

StrokeMarkItem::StrokeMarkItem(QGraphicsItem * parent) : QGraphicsPolygonItem(parent)
{
	createPropertyInfo();
}

void StrokeMarkItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	painter->setPen(pen());
	painter->setBrush(brush());
	painter->drawPolyline(polygon());
	if (option->state & QStyle::State_Selected)
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


void StrokeMarkItem::createPropertyInfo()
{
	const MarkPropertyInfo propertyInfos = {
		qMakePair(MarkProperty::Color,QStringLiteral("Color")),
		qMakePair(MarkProperty::CategoryColor,QStringLiteral("Category Color")),
		qMakePair(MarkProperty::Name,QStringLiteral("ID")),
		qMakePair(MarkProperty::CategoryName,QStringLiteral("Category ID")),
		qMakePair(MarkProperty::SliceIndex,QStringLiteral("Slice Index")),
		qMakePair(MarkProperty::SliceType,QStringLiteral("Slice Type")),
		qMakePair(MarkProperty::VisibleState,QStringLiteral("Visible")),
		qMakePair(MarkProperty::Length,QStringLiteral("Length")),
	};
	setData(MarkProperty::PropertyInfo, QVariant::fromValue(propertyInfos));
}

void StrokeMarkItem::updateLength()
{
	const auto  & poly = polygon();
	double length = data(MarkProperty::Length).toDouble();
	const int c = poly.count();
	if (c <= 1)
	{
		setData(MarkProperty::Length, 0);
		return;
	}
	const auto &p0 = poly[c - 2];
	const auto &p1 = poly[c - 1];
	const auto dx = p0.x() - p1.x();
	const auto dy = p0.y() - p1.y();
	length += std::sqrt(dx*dx + dy * dy);
	setData(MarkProperty::Length, length);
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
	return stream;
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
		double length;					//double
		QString name;					//QString
		int sliceIndex;					//int
		int sliceType;					//int
		bool vis;						//bool
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