#include "treeitem.h"
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
	stream << item->m_type;
	//int nData = item->m_data.size();
	//stream << nData;
	//for(int i=0;i<nData;i++)
	//	stream << item->m_data[i];

	//int nChild = item->m_children.size();
	//stream << nChild;
	//for (int i = 0; i < nChild; i++)
	//	stream << item->m_children[i];
	stream << item->m_data;
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

	//Decode the data
	//int nData;
	//stream >> nData;
	//Q_ASSERT_X(stream.status() != QDataStream::ReadPastEnd,
	//	"TreeItem::operator>>", "Corrupt Data");
	//QVector<QVariant> data(nData);
	//item->m_data.resize(nData);
	//for (int i = 0; i < nData; i++)
	//{
	//	Q_ASSERT_X(stream.status() != QDataStream::ReadPastEnd,
	//		"TreeItem::operator>>", "Corrupt Data");
	//	stream >> data[i];
	//}

	QVector<QVariant> data;
	stream >> data;
	item = new TreeItem(data,type, nullptr);
	stream >> item->m_children;

	//Decode the children node
	//int nChild;
	//stream >> nChild;
	//Q_ASSERT_X(stream.status() != QDataStream::ReadPastEnd,
	//	"TreeItem::operator>>", "Corrupt Data");
	//item->m_children.resize(nChild);
	//for (int i = 0; i < nChild; i++)
	//{
	//	Q_ASSERT_X(stream.status() != QDataStream::ReadPastEnd,
	//		"TreeItem::operator>>", "Corrupt Data");
	//	stream >> item->m_children[i];			//recursion
	//}
	//for (int i = 0; i < nChild; i++)		//set parent for the children
	//	item->m_children[i]->m_parent = item;
	return stream;
}
