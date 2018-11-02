#ifndef INSTANCEITEM_H
#define INSTANCEITEM_H

#include "model/treeitem.h"
#include "algorithm/triangulate.h"

#include <QDataStream>

class InstanceMetaData 
{
	QString m_name;
	QRectF  m_region;
	bool m_visibleState;
public:
	InstanceMetaData():m_visibleState(true){}
	QString name()const { return m_name; }
	void setName(const QString & name) { m_name = name; }
	bool visibleState()const { return m_visibleState; }
	void setVisibleState(bool visible) { m_visibleState = visible;}
	QRectF region()const { return m_region; }
	void setRegion(const QRectF & rect) { m_region = rect; }

	friend QDataStream & operator<<(QDataStream & stream, const InstanceMetaData * metaData);

	friend QDataStream & operator>>(QDataStream & stream, InstanceMetaData *& metaData);


};



class InstanceTreeItem;

class InstanceTreeItemInfoModel :public QAbstractItemModel
{
	InstanceTreeItem * m_treeItem;
	QVector<QString> m_propertyNames;
	InstanceMetaData * m_metaData;
public:
	InstanceTreeItemInfoModel(InstanceMetaData * metaData,InstanceTreeItem * item, QObject * parent = nullptr);
	QVariant data(const QModelIndex& index, int role) const override;
	int columnCount(const QModelIndex& parent) const override;
	QModelIndex index(int row, int column, const QModelIndex& parent) const override;
	int rowCount(const QModelIndex& parent) const override;
	QModelIndex parent(const QModelIndex& child) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;
};

class InstanceTreeItem:public TreeItem
{
	//QString m_text;
	//QRect m_range;
	//quint8 m_checkState;
	InstanceTreeItemInfoModel * m_infoModel;
	static QVector<QList<StrokeMarkItem*>> refactorMarks(QList<StrokeMarkItem*> &marks);

	InstanceMetaData* m_metaData;
public:

	// Inherit from TreeItem
    InstanceTreeItem(InstanceMetaData * metaData, const QPersistentModelIndex & pModelIndex,TreeItem * parent);
	QVariant data(int column, int role) const override;
	bool setData(int column, const QVariant& value, int role) override;
	int columnCount() const override;
	int type() const override;
	bool insertColumns(int position, int columns) override;
	bool removeColumns(int position, int columns) override;
	void * metaData() override;
	QAbstractItemModel * infoModel() const override { return m_infoModel; }

	QRectF boundingBox() const { return m_metaData->region(); }
	void setBoundingBox(const QRectF& rect) { m_metaData->setRegion(rect); }
	bool visible()const { return m_metaData->visibleState(); }

	QSharedPointer<Triangulate> mesh()const;

	~InstanceTreeItem();

	friend QDataStream & operator<<(QDataStream & stream, const InstanceTreeItem * item);
	friend QDataStream & operator>>(QDataStream & stream, InstanceTreeItem *& item);

};





#endif // INSTANCEITEM_H