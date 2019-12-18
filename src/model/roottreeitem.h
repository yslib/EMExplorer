#ifndef ROOTTREEITEM_H
#define ROOTTREEITEM_H

#include "model/treeitem.h"

/**
 * \brief This class represents the root node in the mark tree view
 * 
 * The class is trivial for necessity.
 */
class RootTreeItem : public TreeItem {
	//void setPersistentModelIndex(const QPersistentModelIndex & index) {setModelIndex(index);}

	void setModelIndexRecursively(const QModelIndex & parent) 
	{
		_internal_setModelIndexRecursively(this, parent);
	}
	static void _internal_setModelIndexRecursively(TreeItem * item, const QModelIndex & parent) {

		item->m_persistentModelIndex = parent;
		const auto nChild = item->childCount();
		for (auto i = 0; i < nChild; i++) {
			auto ch = item->child(i);
			const auto index = parent.model()->index(i, 0, parent);
			ch->setModelIndex(index);
			_internal_setModelIndexRecursively(ch, index);
		}
	}

public:
	RootTreeItem(const QPersistentModelIndex & pIndex,TreeItem * parent) :TreeItem(pIndex,parent) {}
	QVariant data(int column, int role) const override { return QVariant(); }
	int columnCount() const override { return 2; }
	bool insertColumns(int position, int columns) override{Q_UNUSED(position);Q_UNUSED(columns);return false;}
	bool removeColumns(int position, int columns) override{Q_UNUSED(position);Q_UNUSED(columns);return false;}
	bool setData(int column, const QVariant& value, int role) override{return false;}
	int type() const override { return TreeItemType::Root; }
	void * metaData()override { return nullptr; }
	void setModelView(QAbstractItemView* view) override{}

	friend class MarkModel;

	friend QDataStream & operator<<(QDataStream & stream, const RootTreeItem * item);
	friend QDataStream & operator>>(QDataStream & stream, RootTreeItem *& item);
};


#endif // ROOTTREEITEM_H