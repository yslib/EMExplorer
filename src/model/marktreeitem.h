#ifndef MARKTREEITEM_H
#define MARKTREEITEM_H

#include "treeitem.h"

class QAbstractItemView;

class MarkItemInfoModel :public QAbstractItemModel
{
	StrokeMarkItem *& m_markItem;
	QVector<QString> propertyNames;
public:
	explicit MarkItemInfoModel(StrokeMarkItem * & mark,QObject * parent);
	QVariant data(const QModelIndex& index, int role) const override;
	int columnCount(const QModelIndex& parent) const override;
	QModelIndex index(int row, int column, const QModelIndex& parent) const override;
	int rowCount(const QModelIndex& parent) const override;
	QModelIndex parent(const QModelIndex& child) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;
};

class StrokeMarkTreeItem : public TreeItem {

	StrokeMarkItem * m_markItem = nullptr;
	QAbstractItemModel * m_infoModel = nullptr;
	QAbstractItemView * m_infoView = nullptr;
protected:
	void modelIndexChanged(const QPersistentModelIndex& index) override 
	{
		if(m_markItem != nullptr)
		{
			m_markItem->m_modelIndex = index;
		}
	}
public:
	StrokeMarkTreeItem(StrokeMarkItem* markItem,const QPersistentModelIndex & pIndex, TreeItem* parent);

	QVariant data(int column, int role) const override;

	int columnCount() const override;

	bool setData(int column, const QVariant& value, int role) override;

	int type() const override;

	void * metaData() override;

	void setModelView(QAbstractItemView*view)override;

    StrokeMarkItem * markItem() {return m_markItem;}

    ~StrokeMarkTreeItem() override;

	friend QDataStream& operator<<(QDataStream& stream, const StrokeMarkTreeItem * item);

	friend QDataStream & operator>>(QDataStream & stream,StrokeMarkTreeItem *& item);

};

#endif // MARKTREEITEM_H
