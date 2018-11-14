#ifndef ABSTRACTMARKITEM_H
#define ABSTRACTMARKITEM_H
#include "globals.h"
#include <QAbstractGraphicsShapeItem>
#include <QPersistentModelIndex>
#include <QPen>
#include <QLinkedList>
#include <functional>



class StrokeMarkItem :public QGraphicsPolygonItem {
	std::function<QVariant(StrokeMarkItem* mark,QGraphicsItem::GraphicsItemChange, const QVariant &)> m_itemChangeHandler;
	QPersistentModelIndex m_modelIndex;

	struct StrokeMarkItemPrivate
	{
        StrokeMarkItemPrivate():m_visibleState(true),isFilled(false){}
		QString m_name;
		SliceType m_sliceType;
		//double m_length;
		int m_index;
		bool m_visibleState;
        bool isFilled;
		friend QDataStream & operator<<(QDataStream & stream,const StrokeMarkItemPrivate * info) 
		{
			stream << info->m_name << static_cast<quint32>(info->m_sliceType) << info->m_index << info->m_visibleState;
			return stream;
		}

		friend QDataStream & operator>>(QDataStream & stream, StrokeMarkItemPrivate * info){
			Q_ASSERT(info);
			quint32 type;
			stream >> info->m_name >> type >> info->m_index >> info->m_visibleState;
			info->m_sliceType = static_cast<SliceType>(type);
			return stream;
		}

	}*m_markInfo;


	bool m_erase;
	QLinkedList<QPolygonF> m_segments;


public:

	enum {Type = StrokeMark};
	explicit StrokeMarkItem(const QPolygonF& path, QGraphicsItem * parent=nullptr);
	explicit StrokeMarkItem(QGraphicsItem * parent = nullptr);
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) Q_DECL_OVERRIDE;
	void appendPoint(const QPointF & p);
	int type() const override { return Type; }
	void setItemChangeHandler(const std::function<QVariant(StrokeMarkItem* mark, QGraphicsItem::GraphicsItemChange, const QVariant&)>& handler);
	QPersistentModelIndex modelIndex()const { return m_modelIndex; }

	QString	name()const { return m_markInfo->m_name; }
	void setName(const QString & name) { m_markInfo->m_name = name; }
	SliceType sliceType()const { return m_markInfo->m_sliceType; }
	void setSliceType(SliceType type) { m_markInfo->m_sliceType = type; }
	double length()const { return polygon().length(); }
	int sliceIndex()const { return m_markInfo->m_index; }
	void setSliceIndex(int index) { m_markInfo->m_index = index; }

	bool visibleState()const { return m_markInfo->m_visibleState; }
	void setVisibleState(bool visible) { m_markInfo->m_visibleState = visible; }

    bool isFilled()const{return m_markInfo->isFilled;}
    void setFilled(bool fill){m_markInfo->isFilled = fill;update();}

	void beginErase();
	void erase(const QPointF & center,double radius);
	QVector<StrokeMarkItem*> endErase(bool residue = false, bool* empty = nullptr);
	
	bool valid()const { return !m_erase; }


	~StrokeMarkItem();

	friend QDataStream & operator<<(QDataStream & stream, const StrokeMarkItem * item);
	friend QDataStream & operator>>(QDataStream & stream, StrokeMarkItem *& item);
	
private:
	void createPropertyInfo();
	//void updateLength();
	QVector<QLinkedList<QPolygonF>::iterator> findPolygon(const QPointF& center, double radius);
	static bool intersectsWithCircle(const QPointF & center, double radius, const QPointF& point);
	static bool rectIntersectWithCircle(const QPointF & center, double radius, const QRectF & rect);
	static QRectF unionWith(const QRectF & rect, const QPointF & p);
	static QLinkedList<QPolygonF> dividePolygon(const QPolygonF& poly, const QPointF& center, double radius);
	
protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
	friend class StrokeMarkTreeItem;		///< Member m_modelIndex
};



#endif // ABSTRACTMARKITEM_H
