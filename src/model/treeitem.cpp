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

TreeItem::~TreeItem()
{
	qDeleteAll(m_children);
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
* \brief If the pointer is nullptr, the function will apply constrcution to the 
* \brief pointer-reference and if the pointer is not empty,the function will
* \brief reconstruct it to satisfied with the binary file underlying the stream
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
