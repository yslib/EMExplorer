#ifndef TREEITEM_H
#define TREEITEM_H

#include <QDebug>
//#include "categorytreeitem.h"
#include "markitem.h"
#include <QAbstractItemModel>
//#include "categorytreeitem.h"

enum  TreeItemType
{
	Root,
	Category,
	Mark,
	Empty
};
QDataStream & operator<<(QDataStream & stream, const TreeItemType &type);
QDataStream & operator>>(QDataStream & stream, TreeItemType &type);

#define RAW_POINTER_TYPE

#define PTR_TYPE(TYPE) __PTR_TYPE_(TYPE)
#define INTERNAL_PTR(x) __INTERNAL_PTR_(x)

#ifdef RAW_POINTER_TYPE
#define __TREE_NODE_POINTER_TYPE_RAW_PTR_
#elif defined(STD_UNIQUE_POINTER_TYPE)
#define __TREE_NODE_POINTER_TYPE_STD_UNIQUE_PTR_
#elif defined(STD_SHARED_POINTER_TYPE)
#define __TREE_NODE_POINTER_TYPE_STD_SHARED_PTR_
#elif defined(QT_SCOPED_POINTER_TYPE)
#define __TREE_NODE_POINTER_TYPE_QT_SCOPED_PTR_
#elif defined(QT_SHARED_POINTER_TYPE)
#define __TREE_NODE_POINTER_TYPE_QT_SHARED_PTR_
#endif


#ifdef __TREE_NODE_POINTER_TYPE_RAW_PTR_
	#define __PTR_TYPE_(TYPE) TYPE*
	#define __INTERNAL_PTR_(x) (x)
#elif defined(__TREE_NODE_POINTER_TYPE_STD_UNIQUE_PTR_)
	#define __PTR_TYPE_(TYPE) std::unique_ptr<TYPE>
	#define __INTERNAL_PTR_(x) (x.get())
#elif defined (__TREE_NODE_POINTER_TYPE_QT_SCOPED_PTR_)
	#define __PTR_TYPE_(TYPE) std::shared_ptr<TYPE>
	#define __INTERNAL_PTR_(x) (x.get())
#elif defined(__TREE_NODE_POINTER_TYPE_STD_SHARED_PTR_)
	#define __PTR_TYPE_(TYPE) QSharedPointer<TYPE>
	#define __INTERNAL_PTR_(x) (x.get())
#elif defined(__TREE_NODE_POINTER_TYPE_QT_SHARED_PTR_)
	#define __PTR_TYPE_(TYPE) QScopedPointer<TYPE>
	#define __INTERNAL_PTR_(x) (x.get())
#endif



class TreeItem
{
	PTR_TYPE(TreeItem) m_parent;
	QVector<PTR_TYPE(TreeItem)> m_children;
	//QAbstractItemModel * m_model;

	QPersistentModelIndex m_persistentModelIndex;

	void updateChildQPersistentModelIndex(TreeItem * item,int row);

public:
	explicit TreeItem(const QPersistentModelIndex & pIndex,PTR_TYPE(TreeItem)parent = nullptr) :
	m_parent(nullptr)
	{
		m_parent = parent;
		m_persistentModelIndex = pIndex;
	}
	virtual ~TreeItem();

	const QAbstractItemModel* itemModel() const;

	/**
	 * \brief Returns the model index refers to the item in the model
	 * 
	 * A QPersistentModelIndex is ensured that you can access the item referred by the it at any time
	 * as long as the item can be accessed by the model.
	 * \return 
	 * 
	 * \sa QPersistentModelIndex
	 */
	const QPersistentModelIndex & persistentModelIndex()const { return m_persistentModelIndex; }

	void setPersistentModelIndex(const QPersistentModelIndex & pIndex) { m_persistentModelIndex = pIndex; }

	void appendChild(TreeItem* child);

	void setParentItem(TreeItem * parent) { m_parent = parent; }

	TreeItem* parentItem()const { return m_parent; };

	TreeItem* child(int row)const { return m_children.value(row); }

	TreeItem* takeChild(int row, TreeItem * child = nullptr,bool * takeSuccess = nullptr)noexcept;

	int childCount()const { return m_children.size(); }

	bool insertChildren(int position, const QVector<TreeItem*>& children);

	bool removeChildren(int position, int count) noexcept;

	int row() const;

	virtual QAbstractItemModel* infoModel()const = 0;

	virtual int columnCount()const = 0;

	virtual QVariant data(int column = 0, int role = Qt::DisplayRole)const = 0;

	virtual bool insertColumns(int position, int columns) = 0;

	virtual bool removeColumns(int position, int columns) = 0;


	virtual bool setData(int column, const QVariant & value, int role = Qt::DisplayRole) = 0;

	virtual int type()const = 0;

	virtual void * metaData() = 0;

	//void setCommonData(const QVariant & value) { m_commonData = value; }
	/**
	*	All above methods are necessary for a read-only TreeView.
	*	Following methods are required for a editable TreeView.
	*/
	//bool insertChildren(int position, int count)
	//{
	//	///TODO:: Is this check necessary? 
	//	if (position < 0 || position > m_children.size())
	//		return false;
	//	for (auto row = 0; row < count; row++)
	//	{
	//		//QVector<QHash<int,QVariant>> data(columns);
	//		auto * item = new TreeItem(type, this);
	//		

	//		item->m_data.resize(columns);			//Same as parent's


	//		m_children.insert(position, item);
	//	}
	//	return true;
	//}
	friend QDataStream & operator<<(QDataStream & stream, const TreeItem * item);
	friend QDataStream & operator>>(QDataStream & stream, TreeItem *& item);

};




/**
 * \brief This instance of the class represents a empty node in mark tree view
 */
class EmptyTreeItem:public TreeItem {
public:
	EmptyTreeItem(const QPersistentModelIndex & pModelIndex, TreeItem * parent):TreeItem(pModelIndex,parent){}
	QVariant data(int column, int role) const override { return QVariant{}; }
	bool setData(int column, const QVariant& value, int role) override { return false; }
	int columnCount() const override { return 1; }
	int type() const override { return TreeItemType::Empty; };
	bool insertColumns(int position, int columns) override {
		Q_UNUSED(position);
		Q_UNUSED(columns);
		return false;
	}
	bool removeColumns(int position, int columns) override {
		Q_UNUSED(position);
		Q_UNUSED(columns);
		return false;
	}

	void * metaData() override { return nullptr; }

	QAbstractItemModel * infoModel() const override { return nullptr; }
};


#endif // TREEITEM_H