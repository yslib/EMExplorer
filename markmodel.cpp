#include "markmodel.h"

#include "imageviewer.h"

MarksModel::MarksModel(QObject* parent):QAbstractItemModel(parent)
{

}

MarksModel::~MarksModel()
{

}

QVariant MarksModel::data(const QModelIndex& index, int role) const
{
	if (index.isValid() == false)
		return QVariant();
	if (role == Qt::DisplayRole) {
		quintptr iid = index.internalId();
		if (iid == FirstLevel) {
			//Fist Level
			return QVariant::fromValue(m_class[index.row()]);
		}
		else if (iid == SecondLevel) {
			//Second Level
			auto ptr = static_cast<AbstractMarkItem*>(index.internalPointer());
			if(ptr != nullptr)
				return QVariant::fromValue(ptr->name());
			return QVariant();
		}
		return QVariant();

	}
	else if (role == Qt::BackgroundRole) {
		return QVariant();
	}
	return QVariant();
}

QVariant MarksModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal) {
		if (role == Qt::DisplayRole) {
			return QVariant::fromValue(QStringLiteral("Name"));
		}
		return QVariant();
	}
	return QVariant();
}

QModelIndex MarksModel::index(int row, int column, const QModelIndex& parent) const
{
	if (parent.isValid() == false) {	//First Level
		return createIndex(row, column,FirstLevel);
	}
	quintptr iid = parent.internalId();
	if (iid == FirstLevel) {
		auto index = createIndex(row, column, m_marks[parent.row()][row]);
		return index;
	}
	return QModelIndex();
}

QModelIndex MarksModel::parent(const QModelIndex& index) const
{
	if (index.isValid() == false)
		return QModelIndex();
	auto iid = index.internalId();
	if (iid == FirstLevel)
		return QModelIndex();
	//Second Level
	return createIndex(0,0 , FirstLevel);
}

int MarksModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid() == false) {
		//First level, return the number of category of mark
		return m_class.size();
	}
	auto iid = parent.internalId();
	if (iid == FirstLevel) {
		return m_marks[parent.row()].size();
	}
	return 0;
}

int MarksModel::columnCount(const QModelIndex& parent) const
{
	return 1;
}

Qt::ItemFlags MarksModel::flags(const QModelIndex& index) const
{
	if (index.isValid() == false)
		return 0;
	return QAbstractItemModel::flags(index);
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

void MarksModel::addMark(const QString & category, AbstractMarkItem* mark)
{
	int index = m_class.indexOf(category);
	if (index == -1) {
		m_class.append(category);
		QList<AbstractMarkItem*> items = { mark };
		m_marks.append(items);
	}
	else {
		m_marks[index].append(mark);
	}
}

void MarksModel::addMarks(const QString & category, const QList<AbstractMarkItem*> & marks)
{
	int index = m_class.indexOf(category);
	if (index == -1) {
		m_class.append(category);
		m_marks.append(marks);
	}
	else {
		m_marks[index].append(marks);
	}
}
QList<AbstractMarkItem*> MarksModel::marks(const QString & category)
{
	return m_marks.value(m_class.indexOf(category));
}

bool MarksModel::removeMark(const QString & category,AbstractMarkItem * mark)
{
	return m_marks.value(m_class.indexOf(category)).removeOne(mark);
}

int MarksModel::removeMarks(const QString & category, const QList<AbstractMarkItem*> & marks /*= QList<QGraphicsItem*>()*/)
{	
	auto list = m_marks.value(m_class.indexOf(category));
	int count = 0;
	for (auto it : marks) {
		if(list.removeOne(it))
			count++;
	}
	return count;
}
