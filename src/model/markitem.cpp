#include "markitem.h"
#include "model/treeitem.h"

#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QDebug>
#include <cmath>
#include <QVector2D>


/*!
\internal

Highlights \a item as selected.

NOTE: This function is a duplicate of qt_graphicsItem_highlightSelected() in 
	qgraphicsitem.cpp!
*/

//#define DEBUG_MARK_ERASE

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
m_markInfo(new StrokeMarkItemPrivate),
m_erase(false)
{
	setPolygon(path);
	//updateLength();
}

StrokeMarkItem::StrokeMarkItem(QGraphicsItem * parent) : 
QGraphicsPolygonItem(parent),
m_markInfo(new StrokeMarkItemPrivate),
m_erase(false)
{
    //updateLength();
}

StrokeMarkItem::StrokeMarkItem(const StrokeMarkItem &obj)
{
    //NEVER USE!
    m_itemChangeHandler = obj.m_itemChangeHandler;
    m_modelIndex = obj.modelIndex();
    m_erase = obj.m_erase;
    m_segments = obj.m_segments;

}

void StrokeMarkItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setPen(pen());
    painter->setBrush(brush());
	if(!m_erase) {
		if (m_markInfo->isFilled == false) {      //None filled
			painter->drawPolyline(polygon());
		}
		else {
			painter->setBrush(QBrush(pen().color(), Qt::SolidPattern));
			painter->drawPolygon(polygon()); // filled

		}
		if (option != nullptr && option->state & QStyle::State_Selected)
			drawHighlightSelected(this, painter, option);
	}else {	
		/*Being erased, we draw the segments only*/
		if(!m_markInfo->isFilled) {
			for(const auto & poly:m_segments) {
				painter->drawPolyline(poly);
			}
		}else {
			painter->setBrush(QBrush(pen().color(), Qt::SolidPattern));
			for(const auto & poly:m_segments) {
				painter->drawPolygon(poly,Qt::OddEvenFill);
			}
		}
	}

}

void StrokeMarkItem::appendPoint(const QPointF& p)
{
	auto poly = polygon();
	poly.append(p);
	setPolygon(poly);
	update(boundingRect());
	//updateLength();
}


/**
 * \brief This function is used to set a handler to handle some state change of the item from external
 * 
 * Because QGraphicsItem and its subclasses don't inherit from QObject, they can't emit any signals to 
 * notify any other object. On a count of this, a call back function need to be employed manually.
 * \param handler This is a type of \a std::function<>
 */
void StrokeMarkItem::setItemChangeHandler(
    const std::function<QVariant(StrokeMarkItem* mark, QGraphicsItem::GraphicsItemChange, const QVariant&)>& handler)
{
	m_itemChangeHandler = handler;
}

void StrokeMarkItem::beginErase() 
{
	m_erase = true;
	m_segments.clear();
	const auto p = polygon();
	m_segments << p;
}

void StrokeMarkItem::erase(const QPointF & center, double radius)
{
	if (!m_erase) 
	{
		qWarning("Erase state is invalid");
		return;
	}
		
	const auto its = findPolygon(center, radius);
	QLinkedList<QPolygonF> newPolygons;

	for(const auto &it:its) 
	{
		newPolygons += dividePolygon(*it, center, radius);		// splice
		m_segments.erase(it);
	}

	m_segments += newPolygons;			// splice
	update();
}

QVector<StrokeMarkItem*> StrokeMarkItem::endErase(bool residue, bool* empty) 
{
	m_erase = false;
	QVector < StrokeMarkItem * > residues;
	if(m_segments.isEmpty()) {
		if (empty) *empty = true;
		return residues;
	}

	setPolygon(m_segments.first());
	update(boundingRect());
	//updateLength();
	update();

	/*If the size is 0, the following loop will not hit the end condition*/
	if (m_segments.size() <= 1) {
		if (empty)
			*empty = false;
		return residues;
	}
		

	if(residue) {
		
		for(auto it = (++m_segments.begin());it !=m_segments.end();++it) {
			auto m = new StrokeMarkItem(*it, parentItem());
			m->setFilled(this->isFilled());
			m->setVisibleState(this->visibleState());
			m->setName(this->name() + "*");
			m->setSliceType(this->sliceType());
			m->setSliceIndex(this->sliceIndex());
			m->setPen(m->pen());
			residues << m;
		}
	}

	if (empty) *empty = false;
	return residues;
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

//void StrokeMarkItem::updateLength()
//{
//	const auto  & poly = polygon();
//	const int c = poly.count();
//	if (c <= 1)
//	{
//		m_markInfo->m_length = 0;
//		return;
//	}
//	const auto &p0 = poly[c - 2];
//	const auto &p1 = poly[c - 1];
//	const auto dx = p0.x() - p1.x();
//	const auto dy = p0.y() - p1.y();
//
//	m_markInfo->m_length += std::sqrt(dx*dx + dy * dy);
//
//
//	//return polygon().length();
//}

QVector<QLinkedList<QPolygonF>::iterator> StrokeMarkItem::findPolygon(const QPointF& center, double radius)
{
	QVector<QLinkedList<QPolygonF>::iterator> iters;
	for(auto it = m_segments.begin();it != m_segments.end();++it) {
		
		if (rectIntersectWithCircle(center, radius, it->boundingRect())) {
			iters << it;
#ifdef DEBUG_MARK_ERASE
			qDebug() << "Erase range:" << center << "," << radius << " Mark BoundingRect:" << it->boundingRect();
#endif
		}
			
	}
	return iters;
}


bool StrokeMarkItem::intersectsWithCircle(const QPointF& center, double radius, const QPointF& point) 
{
	const auto v = center - point;
	return (v.x()*v.x() + v.y()*v.y()) <= radius * radius;
}

bool StrokeMarkItem::rectIntersectWithCircle(const QPointF& center, double radius, const QRectF& rect) {
	QPointF p;
	const auto cRect = rect.center();
	if(center.x() < cRect.x() && center.y() <=cRect.y()) 
	{
		//p = { 2*cRect.x() -  center.x() , center.y() };
		p = { 2 * cRect.x() - center.x(),2 * cRect.y() - center.y() };
	}
	else if(center.x() >= cRect.x() && center.y() < cRect.y() ) 
	{
		p = { center.x(),2*cRect.y() - center.y()};
	}
	else if(center.x() < cRect.x() && center.y() >= cRect.y()) 
	{
		//p = { 2 * cRect.x() - center.x(),2 * cRect.y() - center.y() };
		p = { 2 * cRect.x() - center.x() , center.y() };
	}
	QVector2D v(p - rect.bottomRight());
	v.setX(std::max(v.x(), 0.f));
	v.setY(std::max(v.y(), 0.f));
	return v.lengthSquared() < radius*radius;
}


QRectF StrokeMarkItem::unionWith(const QRectF & rect, const QPointF & p)
{
	const auto &topLeft = rect.topLeft();
	const auto &bottomRight = rect.bottomRight();
	const auto  newTopLeft = QPointF(std::min(p.x(), topLeft.x()), std::min(p.y(), topLeft.y()));
	const auto newBottomRight = QPointF(std::max(p.x(), bottomRight.x()), std::max(p.y(), bottomRight.y()));
	return {newTopLeft, newBottomRight};
}


QLinkedList<QPolygonF> StrokeMarkItem::dividePolygon(const QPolygonF& poly, const QPointF& center, double radius)
{
	QLinkedList<QPolygonF> polygons;
	if (poly.size() == 0)
		return polygons;

	enum {In,Out};
	auto state = Out;
	int clipBegin;

	QVector<QPair<int, int>> ends;

	for(auto i = 0 ;i<poly.size();i++) 
	{
		const auto p = poly[i];
		if(intersectsWithCircle(center,radius,p)) {
			if(state == Out) {
				state = In;
				clipBegin = i;
			}
		}else {
			if(state == In) {
				state = Out;
				ends << qMakePair(clipBegin, i);
			}
		}

	}

	// Corner case
	if(state == In) 
	{
		ends << qMakePair(clipBegin, poly.size());
	}

	int i = 0;

	// Reconstructs polygons with points except for range in ends

	for(const auto &p:ends) {
		QPolygonF s;
		for(;i<p.first;i++)  
			s << poly[i];
		i = p.second;
		if(s.size() > 1) {
			polygons << s;
		}
#ifdef DEBUG_MARK_ERASE
		if (s.size() == 1) {
			qDebug() << "1-point polygon";
		}
#endif
	}
	//const auto endSegment = ends.end();
	QPolygonF s;
	for(;i<poly.size();i++) 
	{
		s << poly[i];
	}
	if(s.size() > 1) {

		// Polygon that only have 1 point will be discarded
		polygons << s;
	}

#ifdef DEBUG_MARK_ERASE
	if (s.size() == 1) {
		qDebug() << "1-point polygon";
	}
#endif

	return polygons;
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
    if (item != nullptr && item->type() == ItemTypes::StrokeMark)
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
