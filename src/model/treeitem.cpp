#include "treeitem.h"
#include <QGraphicsItem>

QDataStream& operator<<(QDataStream& stream, const TreeItemType& type)
{
	stream << static_cast<qint32>(type);
	return stream;
}

QDataStream& operator>>(QDataStream& stream, TreeItemType& type)
{
	qint32 t;
	stream >> t;
	type = static_cast<TreeItemType>(t);
	return stream;
}

void TreeItem::updateChildQPersistentModelIndex(TreeItem*item,int row) {
	Q_ASSERT_X(item, "TreeItem::updateChildQPersistentModelIndex", "null pointer");
	const auto m = m_persistentModelIndex.model();
	if(m == nullptr) {
		item->m_persistentModelIndex = QModelIndex();
		qDebug() << "QPersistentModelIndex created invalid";
	}else {
		
		item->m_persistentModelIndex = m->index(row,0,m_persistentModelIndex);
		qDebug() << item->persistentModelIndex().isValid();
	}
}

TreeItem::~TreeItem()
{
	qDeleteAll(m_children);
}

const QAbstractItemModel* TreeItem::itemModel() const {
	if(m_persistentModelIndex.isValid() == false) 
	{
		return nullptr;
	}
	return m_persistentModelIndex.model();
}

/**
 * \brief 
 * \param child 
 */
void TreeItem::appendChild(TreeItem* child) {
	Q_ASSERT_X(child, "TreeItem::appendChild", "null pointer");

	child->setParentItem(this);
	m_children.append(child);
	updateChildQPersistentModelIndex(child, m_children.size() - 1);
}


/**
 * \brief This function replace the old child node with a new one.
 * 
 * \param row The row number of the old child node in the \a TreeItem
 * \param child The new child node that will replace the old one.
 * \param takeSuccess success flag pointer. If it's not a null pointer, the reference is set as \a true 
 * when substitution is successful otherwise it is set as \a false.
 * 
 * \return Return tje old child node pointer
 * 
 * \note If \a row is larger than the child number, nothing will be done. \a takeSuccess will be set as \a false
 * if it's not a \a nullptr. The ownership of the old child pointer is returned to the caller and its parent 
 * item is set as \a nullptr.
 */
TreeItem * TreeItem::takeChild(int row, TreeItem * child, bool * takeSuccess)noexcept
{
	Q_ASSERT_X(child, "TreeItem::appendChild", "null pointer");

	if (row >= m_children.size()) {
		if (takeSuccess != nullptr)
			*takeSuccess = false;
		return nullptr;
	}

	const auto c = m_children[row];
	if(c != nullptr)
		c->setParentItem(nullptr);
	child->setParentItem(this);
	m_children[row] = child;

	updateChildQPersistentModelIndex(child, row);

	if (takeSuccess != nullptr)
		*takeSuccess = true;
	return c;
}

int TreeItem::row() const {
	if (m_parent != nullptr)
		return m_parent->m_children.indexOf(const_cast<TreeItem*>(this));
	return 0;
}

bool TreeItem::insertChildren(int position, const QVector<TreeItem*>& children) {
	if (position < 0 || position > m_children.size())
		return false;
	for (auto item : children) {
		if(item != nullptr)
		item->setParentItem(this);
	}
		
	//std::copy(children.begin(), children.end(), m_children.begin() + position);
	for (auto row = 0; row < children.size(); row++) {
		Q_ASSERT_X(children[row],"TreeItem::insertChildren","null pointer");
		m_children.insert(position, children[row]);
	}
	return true;
}

bool TreeItem::removeChildren(int position, int count) noexcept {
	if (position < 0 || position >= m_children.size())
		return false;
	for (auto i = 0; i < count; i++)
		delete m_children.takeAt(position);
	return true;
}

/**
* \brief Note:The stream operator would serialized the 
* \brief TreeItem to a binary file underly the stream	recursively.
* \param Reference of QDataStream
* \param Pointer to TreeItem
* \return Reference of QDataStream
*/

QDataStream & operator<<(QDataStream & stream, const TreeItem * item)
{
	if (item == nullptr)
		return stream;
	//stream << item->m_type;
	//stream << item->m_data;
	stream << item->m_children;
	return stream;
}

/**
* \brief Note:The stream operator would modified the item pointer whenever possible.
* 
*  If the pointer is nullptr, the function will apply constrcution to the 
*  pointer-reference and if the pointer is not empty,the function will
*  reconstruct it to satisfied with the binary file underlying the stream

* \param Reference of QDataStream
* \param Pointer to the reference of TreeItem
* \return Reference of QDataStream
*/

QDataStream & operator>>(QDataStream & stream, TreeItem *& item)
{
	//Decode the node type
	TreeItemType type;
	stream >> type;
	if(stream.status() == QDataStream::ReadPastEnd)	//Recursion terminate.
		return stream;
	QVector<QVariant> data;
	stream >> data;
	//item = new TreeItem(data,type, nullptr);
	stream >> item->m_children;

	foreach(auto it,item->m_children)
		it->setParentItem(item);		//set parent for the children
	return stream;
}
