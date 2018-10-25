#ifndef ROOTTREEITEM_H
#define ROOTTREEITEM_H

#include "model/treeitem.h"

/**
 * \brief This class represents the root node in the mark tree view
 */
class RootTreeItem : public TreeItem {

public:
	RootTreeItem(QAbstractItemModel * model) :TreeItem(model,nullptr) {}
	QVariant data(int column, int role) const override { return QVariant(); }
	int columnCount() const override { return 1; }
	bool insertColumns(int position, int columns) override{Q_UNUSED(position);Q_UNUSED(columns);return false;}
	bool removeColumns(int position, int columns) override{Q_UNUSED(position);Q_UNUSED(columns);return false;}
	bool setData(int column, const QVariant& value, int role) override{return false;}
	int type() const override { return TreeItemType::Root; }
	void * metaData()override { return nullptr; }
	QAbstractItemModel * infoModel() const override { return nullptr; }
};

#endif // ROOTTREEITEM_H