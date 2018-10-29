#ifndef MARKTREEITEM_H
#define MARKTREEITEM_H

#include "treeitem.h"

class MarkItemInfoModel :public QAbstractItemModel
{
	StrokeMarkItem * m_markItem;

public:
	explicit MarkItemInfoModel(StrokeMarkItem * mark,QObject * parent);

	QVariant data(const QModelIndex& index, int role) const override;
	int columnCount(const QModelIndex& parent) const override;
	QModelIndex index(int row, int column, const QModelIndex& parent) const override;
	int rowCount(const QModelIndex& parent) const override;
	QModelIndex parent(const QModelIndex& child) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
};



class StrokeMarkTreeItem :public TreeItem {

	StrokeMarkItem* m_markItem;
	QAbstractItemModel * m_infoModel;
public:
	StrokeMarkTreeItem(StrokeMarkItem* markItem,const QPersistentModelIndex & pIndex, TreeItem* parent);

	QVariant data(int column, int role) const override;

	bool insertColumns(int position, int columns) override;

	bool removeColumns(int position, int columns) override;

	int columnCount() const override;

	bool setData(int column, const QVariant& value, int role) override;

	int type() const override;

	void* metaData() override;

	QAbstractItemModel * infoModel() const override { return m_infoModel; }

	~StrokeMarkTreeItem();

};

#endif // MARKTREEITEM_H