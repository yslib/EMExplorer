#include "markmodel.h"

MarksModel::MarksModel(const QString& data, QObject* parent)
{
}

MarksModel::~MarksModel()
{
}

QVariant MarksModel::data(const QModelIndex& index, int role) const
{
	return QVariant();
}

QVariant MarksModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}

QModelIndex MarksModel::index(int row, int column, const QModelIndex& parent) const
{
	return QModelIndex();
}

QModelIndex MarksModel::parent(const QModelIndex& index) const
{
	return QModelIndex();
}

int MarksModel::rowCount(const QModelIndex& parent) const
{
	return -1;
}

int MarksModel::columnCount(const QModelIndex& parent) const
{
	return -1;
}

Qt::ItemFlags MarksModel::flags(const QModelIndex& index) const
{
	return 0;
}

bool MarksModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	return false;
}

bool MarksModel::insertColumns(int column, int count, const QModelIndex& parent)
{
	return false;
}

bool MarksModel::removeColumns(int column, int count, const QModelIndex& parent)
{
	return false;
}

bool MarksModel::insertRows(int row, int count, const QModelIndex& parent)
{
	return false;
}

bool MarksModel::removeRows(int row, int count, const QModelIndex& parent)
{
	return false;
}

void MarksModel::addMark(const QString& category, QGraphicsItem* mark)
{
	m_items[category].push_back(mark);
}

void MarksModel::addMarks(const QString& category, const QList<QGraphicsItem*>& marks)
{
	auto & list = m_items[category];
	for(auto item:marks)
	{
		list.append(item);
	}
}
QList<QGraphicsItem*> MarksModel::marks(const QString& category)
{
	auto itr = m_items.find(category);
	if(itr == m_items.constEnd())
		return QList<QGraphicsItem*>();
	return *itr;
}

bool MarksModel::removeMark(const QString & category,QGraphicsItem * item)
{
	auto itr = m_items.find(category);
	if (itr == m_items.end())
		return false;
	return itr->removeOne(item);
}

int MarksModel::removeMarks(const QString & category, const QList<QGraphicsItem*>& marks)
{	
	auto itr = m_items.find(category);
	if (itr == m_items.end())
		return 0;
	if (marks.isEmpty() == true)
	{
		if (itr->isEmpty() == true)
			return 0;
		int count = itr->size();
		m_items.erase(itr);
		return count;
	}
	int count = 0;
	for(auto item:marks)
	{
		if (itr->removeOne(item))
			count++;
	}
	return count;
}
