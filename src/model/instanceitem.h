#ifndef INSTANCEITEM_H
#define INSTANCEITEM_H

#include "model/treeitem.h"

class InstanceTreeItemInfoModel:public QAbstractItemModel {

public:

	InstanceTreeItemInfoModel(QObject * parent = nullptr);
	QVariant data(const QModelIndex& index, int role) const override;
	int columnCount(const QModelIndex& parent) const override;
	QModelIndex index(int row, int column, const QModelIndex& parent) const override;
	int rowCount(const QModelIndex& parent) const override;
	QModelIndex parent(const QModelIndex& child) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;
};

class InstanceTreeItem:public TreeItem
{

public:
    InstanceTreeItem(QAbstractItemModel * model,TreeItem * parent);
	QVariant data(int column, int role) const override;
	bool setData(int column, const QVariant& value, int role) override;
	int columnCount() const override;
	int type() const override;
	bool insertColumns(int position, int columns) override;
	bool removeColumns(int position, int columns) override;
	void * metaData() override;
	QAbstractItemModel * infoModel() const override { return nullptr; }
	
};

#endif // INSTANCEITEM_H