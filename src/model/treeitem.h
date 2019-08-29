#ifndef TREEITEM_H
#define TREEITEM_H
#include <QDebug>
#include "markitem.h"
#include <QAbstractItemModel>
#include <QAbstractItemView>

enum  TreeItemType
{
	Empty,
	Root,
	Category,
	Instance,
	Mark
};

QDataStream & operator<<(QDataStream & stream, const TreeItemType &type);
QDataStream & operator>>(QDataStream & stream, TreeItemType &type);

/**
 * \brief It is used to represent a generic item in QAbstractItemModel
 */
class TreeItem
{
	TreeItem* m_parent;
	QVector<TreeItem*> m_children;
	QPersistentModelIndex m_persistentModelIndex;
	void updateChildQPersistentModelIndex(TreeItem * item, int row);
	void setModelIndex(const QPersistentModelIndex & index){m_persistentModelIndex = index;	modelIndexChanged(index);}
protected:
	virtual void modelIndexChanged(const QPersistentModelIndex & index) {Q_UNUSED(index);}
public:
	explicit TreeItem(const QPersistentModelIndex& pIndex, TreeItem* parent = nullptr);
	virtual ~TreeItem();

	void appendChild(TreeItem* child);
    TreeItem* child(int row)const { return m_children.value(row); }
    TreeItem* takeChild(int row, TreeItem * child = nullptr,bool * takeSuccess = nullptr)noexcept;
    int childCount()const { return m_children.size(); }
    bool insertChildren(int position, const QVector<TreeItem*>& children);
    int row() const;

    void setParentItem(TreeItem * parent) { m_parent = parent; }
    TreeItem* parentItem()const { return m_parent; }


    const QPersistentModelIndex & persistentModelIndex()const { return m_persistentModelIndex; }
    const QAbstractItemModel * itemModel() const;

	virtual int columnCount()const = 0;
	virtual QVariant data(int column = 0, int role = Qt::DisplayRole)const = 0;
	virtual bool setData(int column, const QVariant & value, int role = Qt::DisplayRole) = 0;
	virtual int type()const = 0;
	virtual void * metaData() = 0;
	virtual void setModelView(QAbstractItemView * view) = 0;


	friend QDataStream & operator<<(QDataStream & stream, const TreeItem * item);
	friend QDataStream & operator>>(QDataStream & stream, TreeItem *& item);

	friend class MarkModel;
	friend class RootTreeItem;
};

/**
 * \brief This instance of the class represents a empty node in mark tree view
 */
class EmptyTreeItem:public TreeItem 
{
public:
	EmptyTreeItem(const QPersistentModelIndex & pModelIndex, TreeItem * parent):TreeItem(pModelIndex,parent){}
	QVariant data(int column, int role) const override { return QVariant{}; }
	bool setData(int column, const QVariant& value, int role) override { return false; }
	int columnCount() const override { return 1; }
	int type() const override { return TreeItemType::Empty; }
	void * metaData() override { return nullptr; }
	void setModelView(QAbstractItemView *view) override{}
};


#endif // TREEITEM_H
