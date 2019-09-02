#ifndef ROOTTREEITEM_H
#define ROOTTREEITEM_H

#include "model/treeitem.h"

/**
 * \brief This class represents the root node in the mark tree view
 * 
 * The class is trivial for necessity.
 */
class RootTreeItem : public TreeItem {
	void setModelIndexRecursively(const QModelIndex & parent) 
	{
		_internal_setModelIndexRecursively(this, parent);
	}
	static void _internal_setModelIndexRecursively(TreeItem * item, const QModelIndex & parent) {
		item->m_persistentModelIndex = parent;
        for (auto i = 0; i < item->childCount(); i++) {
			auto ch = item->child(i);
			const auto index = parent.model()->index(i, 0, parent);
			ch->setModelIndex(index);
			_internal_setModelIndexRecursively(ch, index);
		}
	}

public:
	RootTreeItem(const QPersistentModelIndex & pIndex,TreeItem * parent) :TreeItem(pIndex,parent) {}
	int columnCount() const override { return 2; }
	int type() const override { return TreeItemType::Root; }

	friend class MarkModel;

	friend QDataStream & operator<<(QDataStream & stream, const RootTreeItem * item);
	friend QDataStream & operator>>(QDataStream & stream, RootTreeItem *& item);
};

#endif // ROOTTREEITEM_H
