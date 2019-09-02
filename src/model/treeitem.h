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
    TreeItem* takeChild(int row, TreeItem * child = nullptr)noexcept;
    int childCount()const { return m_children.size(); }
    bool insertChildren(int position, const QVector<TreeItem*>& children);
    bool removeChildren(int position, int count) noexcept;
    int row() const;

    void setParentItem(TreeItem * parent) { m_parent = parent; }
    TreeItem* parentItem()const { return m_parent; }


    const QPersistentModelIndex & persistentModelIndex()const { return m_persistentModelIndex; }
    const QAbstractItemModel * itemModel() const;

    virtual int columnCount()const{ return 1; }
    virtual QVariant data(int column = 0, int role = Qt::DisplayRole)const { return QVariant{}; }
    virtual bool setData(int column, const QVariant & value, int role = Qt::DisplayRole) {return false;}
    virtual int type()const { return TreeItemType::Empty; }
    virtual void * metaData(){ return nullptr; }
    virtual void setModelView(QAbstractItemView * /*view*/){}


	friend QDataStream & operator<<(QDataStream & stream, const TreeItem * item);
	friend QDataStream & operator>>(QDataStream & stream, TreeItem *& item);

	friend class MarkModel;
	friend class RootTreeItem;
};

#endif // TREEITEM_H
