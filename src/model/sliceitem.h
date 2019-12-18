#ifndef SLICEITEM_H
#define SLICEITEM_H
#include <QGraphicsPixmapItem>

#include "globals.h"
QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;
QT_END_NAMESPACE

class SliceItem :public QGraphicsPixmapItem
{
public:
	enum { Type = UserType + Slice };
	SliceItem(QGraphicsItem * parent = nullptr) :QGraphicsPixmapItem(parent) {}
	SliceItem(const QPixmap & pixmap, QGraphicsItem * parent = nullptr) :QGraphicsPixmapItem(pixmap, parent) {}
	int type() const Q_DECL_OVERRIDE { return Type; }
	virtual  ~SliceItem() = default;
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;
	void wheelEvent(QGraphicsSceneWheelEvent* event) Q_DECL_OVERRIDE;
};

#endif // SLICEITEM_H