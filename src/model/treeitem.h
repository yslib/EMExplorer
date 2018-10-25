#ifndef TREEITEM_H
#define TREEITEM_H
#include <QVector>
#include <QVariant>
#include <QDataStream>
#include <QDebug>
//#include "categorytreeitem.h"
#include "markitem.h"
#include <QAbstractItemModel>

enum  TreeItemType
{
	Root,
	Category,
	Mark
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
	QAbstractItemModel * m_model;
public:
	explicit TreeItem(QAbstractItemModel * model,PTR_TYPE(TreeItem)parent = nullptr) :
	m_parent(nullptr),
	m_model(nullptr)
	{
		m_parent = parent;
		m_model = model;
	}
	virtual ~TreeItem();

	void setItemModel(QAbstractItemModel * model) {m_model = model;}
	QAbstractItemModel* itemModel()const { return m_model; }
	void appendChild(TreeItem * child) { child->setParentItem(this); m_children.append(child); }
	void setParentItem(TreeItem * parent) { m_parent = parent; }
	TreeItem* parentItem()const { return m_parent; };
	TreeItem* child(int row)const { return m_children.value(row); }

	virtual QAbstractItemModel* infoModel()const = 0;

	/**
	* \brief Get the index of the node
	* 
	* \return return the index in its parent's children
	*/
	int row() const;

	int childCount()const { return m_children.size(); }

	virtual int columnCount()const = 0;

	virtual QVariant data(int column = 0, int role = Qt::DisplayRole)const = 0;

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


	bool insertChildren(int position, const QList<TreeItem*>& children);


	virtual bool insertColumns(int position, int columns) = 0;

	bool removeChildren(int position, int count) noexcept;

	virtual bool removeColumns(int position, int columns) = 0;

	/**
	* \brief To make implementation of the model easier, we return true
	* \brief to indicate whether the data was set successfully, or false if an invalid column
	* \param column
	* \param value
	* \return
	*/

	virtual bool setData(int column, const QVariant & value, int role = Qt::DisplayRole) = 0;

	virtual int type()const = 0;
	virtual void * metaData() = 0;

	friend QDataStream & operator<<(QDataStream & stream, const TreeItem * item);
	friend QDataStream & operator>>(QDataStream & stream, TreeItem *& item);

};





#endif // TREEITEM_H