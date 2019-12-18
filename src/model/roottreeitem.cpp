#include "roottreeitem.h"
QDataStream & operator<<(QDataStream & stream, const RootTreeItem * item)
{
	return stream;		// Do nothing
}
QDataStream & operator>>(QDataStream & stream, RootTreeItem *& item)
{
	RootTreeItem * newItem = new RootTreeItem(QModelIndex{}, nullptr);
	item = newItem;
	return stream;		// Do nothing
}