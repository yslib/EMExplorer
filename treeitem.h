#ifndef TREEITEM_H
#define TREEITEM_H
#include <QVector>
#include <QVariant>
#include <QDataStream>

enum class TreeItemType
{
	Root,
	Category,
	Mark
};
QDataStream & operator<<(QDataStream & stream, const TreeItemType &type);
QDataStream & operator>>(QDataStream & stream, TreeItemType &type);

class TreeItem
{
	TreeItem * m_parent;
	QVector<TreeItem*> m_children;
	QVector<QVariant> m_data;
	TreeItemType m_type;
public:
	explicit TreeItem(const QVector<QVariant> & data, TreeItemType type, TreeItem * parent = nullptr) :m_parent(parent), m_data(data), m_type(type) {}
	~TreeItem();
	void appendChild(TreeItem * child) { child->setParentItem(this); m_children.append(child); }
	void setParentItem(TreeItem * parent) { m_parent = parent; }
	TreeItem* parentItem()const { return m_parent; };
	TreeItem* child(int row)const { return m_children.value(row); }
	/**
	* \brief This is convinence for Model to create QModelIndex in Model::parent() method
	* \return return the index of this node
	*/
	int row() const {
		if (m_parent != nullptr)
			return m_parent->m_children.indexOf(const_cast<TreeItem*>(this));
		return 0;
	}
	int childCount()const { return m_children.size(); }
	int columnCount()const { return m_data.size(); }
	QVariant data(int column = 0)const { return m_data.value(column); }

	/**
	*All above methods are necessary for a read-only TreeView.
	*Following methods are requried for a editable TreeView.
	*
	*/
	bool insertChildren(int position, int count, int columns, TreeItemType type)
	{
		///TODO:: Is this check necessary? 
		if (position < 0 || position > m_children.size())
			return false;
		for (int row = 0; row < count; row++)
		{
			QVector<QVariant> data(columns);
			TreeItem * item = new TreeItem(data, type, this);
			m_children.insert(position, item);
		}
		return true;
	}
	bool insertChildren(int position, QList<TreeItem*>& children)
	{
		if (position < 0 || position > m_children.size())
			return false;
		for (auto item : children)
			item->setParentItem(this);
		//std::copy(children.begin(), children.end(), m_children.begin() + position);
		for (int row = 0; row<children.size(); row++)
		{
			m_children.insert(position, children[row]);
		}
		return true;
	}

	bool insertColumns(int position, int columns)
	{
		if (position<0 || position > m_data.size())return false;
		//insert corresponding columns form current node.
		for (int i = 0; i < columns; i++)
			m_data.insert(position, QVariant());

		//and insert same columns at same position of its all children recursively.
		for (auto & child : m_children)
			child->insertColumns(position, columns);
		return true;
	}
	bool removeChildren(int position, int count)noexcept
	{
		if (position < 0 || position >= m_children.size())
			return false;
		for (int i = 0; i < count; i++)
			delete m_children.takeAt(position);
		return true;
	}
	bool removeColumns(int position, int columns)
	{
		if (position<0 || position > m_data.size())return false;
		//remove corresponding columns from current node.
		for (int i = 0; i < columns; i++)
			m_data.remove(position);
		//and remove same columns at same position of its all children recursively. 
		for (auto & child : m_children)
			child->removeColumns(position, columns);
		return true;
	}

	/**
	* \brief To make implementation of the model easier, we return true
	* \brief to indicate whether the data was set successfully, or false if an invalid column
	* \param column
	* \param value
	* \return
	*/


	bool setData(int column, const QVariant & value)
	{
		if (column < 0 || column >= m_data.size())return false;
		m_data[column] = value;
		return true;
	}

	TreeItemType type()const { return m_type; }

	friend QDataStream & operator<<(QDataStream & stream, const TreeItem * item);
	friend QDataStream & operator>>(QDataStream & stream, TreeItem *& item);

};
#endif // TREEITEM_H