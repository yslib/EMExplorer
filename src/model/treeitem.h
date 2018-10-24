#ifndef TREEITEM_H
#define TREEITEM_H
#include <QVector>
#include <QVariant>
#include <QDataStream>
#include "categoryitem.h"
#include "markitem.h"

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


public:
	explicit TreeItem(PTR_TYPE(TreeItem)parent = nullptr) :m_parent(parent){}
	virtual ~TreeItem();

	void appendChild(TreeItem * child) { child->setParentItem(this); m_children.append(child); }
	void setParentItem(TreeItem * parent) { m_parent = parent; }
	TreeItem* parentItem()const { return m_parent; };
	TreeItem* child(int row)const { return m_children.value(row); }

	/**
	* \brief Get the index of the node
	* 
	* \return return the index in its parent's children
	*/
	int row() const {
		if (m_parent != nullptr)
			return m_parent->m_children.indexOf(const_cast<TreeItem*>(this));
		return 0;
	}

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


	bool insertChildren(int position, const QList<TreeItem*> & children)
	{
		if (position < 0 || position > m_children.size())
			return false;
		for (auto item : children)
			item->setParentItem(this);
		//std::copy(children.begin(), children.end(), m_children.begin() + position);
		for(auto row = 0; row<children.size(); row++)
		{
			m_children.insert(position, children[row]);
		}
		return true;
	}



	virtual bool insertColumns(int position, int columns) = 0;

	bool removeChildren(int position, int count)noexcept
	{
		if (position < 0 || position >= m_children.size())
			return false;
		for (auto i = 0; i < count; i++)
			delete m_children.takeAt(position);
		return true;
	}

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


class RootTreeItem: public TreeItem {

public:
	RootTreeItem():TreeItem(nullptr){}
	QVariant data(int column, int role) const override {return QVariant();}
	int columnCount() const override { return 1; }
	bool insertColumns(int position, int columns) override 
	{
		Q_UNUSED(position);
		Q_UNUSED(columns); 
		return false; 
	}
	bool removeColumns(int position, int columns) override 
	{
		Q_UNUSED(position);
		Q_UNUSED(columns);
		return false; 
	}
	bool setData(int column, const QVariant& value, int role) override { return false; }
	int type() const override { return TreeItemType::Root; }
	void * metaData()override { return nullptr; }
};


class CategoryTreeItem:public TreeItem {
	CategoryItem m_categoryItem;
public:
	CategoryTreeItem(const CategoryItem & categoryItem,TreeItem * parent):TreeItem(parent),m_categoryItem(categoryItem) {
	}
	QVariant data(int column, int role) const override {
		if (column >= 1)
			return false;
		if(role == Qt::DisplayRole) {
			return m_categoryItem.name();
		}else if(role == Qt::DecorationRole) {
			return m_categoryItem.color();
		}
	}

	int columnCount() const override {return 1;}
	bool setData(int column, const QVariant& value, int role) override {
		if (column >= columnCount())
			return false;
		if(role == Qt::DisplayRole) {
			m_categoryItem.setName(value.toString());
			return true;
		}
		return false;
	}
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
	int type() const override { return TreeItemType::Category; }
	void * metaData() override { return static_cast<void*>(&m_categoryItem);}

};


class StrokeMarkTreeItem:public TreeItem {
	
	QGraphicsItem * m_markItem;

public:
	StrokeMarkTreeItem(QGraphicsItem * markItem,TreeItem * parent):TreeItem(parent),m_markItem(nullptr)
	{
		m_markItem = markItem;
	}
	QVariant data(int column, int role) const override {
		if (column >= columnCount())
			return QVariant();
		if(role == Qt::DisplayRole && column == 1) 
		{
			return m_markItem->data(MarkProperty::Name);
		}
		return QVariant();
	}
	bool insertColumns(int position, int columns) override {
		Q_UNUSED(position);
		Q_UNUSED(columns);
		return false;
	}
	bool removeColumns(int position, int columns) override 
	{
		Q_UNUSED(position);
		Q_UNUSED(columns);
		return false;
	}
	int columnCount() const override 
	{
		return 2;
	}
	bool setData(int column, const QVariant& value, int role) override {
		if (column >= columnCount())
			return false;
	}
	int type() const override { return TreeItemType::Mark; }

	void * metaData() override { return static_cast<void*>(m_markItem); }
	
	~StrokeMarkTreeItem() 
	{
		delete m_markItem;
	}

};

#endif // TREEITEM_H