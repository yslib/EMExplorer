#include "markitem.h"
#include <QStyleOptionGraphicsItem>
#include <QPainter>

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

StrokeMarkItem::StrokeMarkItem(const QPolygonF& path, QGraphicsItem * parent):QGraphicsPolygonItem(parent)
{
}

StrokeMarkItem::StrokeMarkItem(QGraphicsItem * parent):QGraphicsPolygonItem(parent)
{
}

void StrokeMarkItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	painter->setPen(pen());
	painter-> setBrush(brush());
	painter->drawPolyline(polygon());
}

void StrokeMarkItem::appendPoint(const QPointF& p)
{
	auto poly = polygon();
	poly.append(p);
	setPolygon(poly);
	update(boundingRect());
}


