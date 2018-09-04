#include "profileview.h"
#include <QBoxLayout>
#include <QListWidget>
#include <QTableView>
#include <QDebug>
#include <QHeaderView>


ProfileView::ProfileView(QWidget* parent): QTableView(parent)
{
	horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	setShowGrid(false);
}

QAbstractItemModel * ProfileView::takeModel(QAbstractItemModel* m)
{
	auto t = model();
    setModel(m);
	return t;
}

//void ProfileView::setModel(QAbstractItemModel * model)
//{
//	m_tableView->setModel(model);
//}

//QAbstractItemModel* ProfileView::model() const
//{
//	return m_tableView->model();
//}

MRCInfoTableModel::MRCInfoTableModel(int row,int column,QObject* parent):
QAbstractTableModel(parent),
m_rowCount(row),
m_columnCount(column)
{
	m_data.resize(row);
	for (int i = 0; i < row; i++)
		m_data[i].resize(column);
}

int MRCInfoTableModel::rowCount(const QModelIndex& parent) const
{
	return m_rowCount;
}

int MRCInfoTableModel::columnCount(const QModelIndex& parent) const
{
	return m_columnCount;
}

QVariant MRCInfoTableModel::data(const QModelIndex& index, int role) const
{
	if (index.isValid() == false)
		return QVariant();
	if(role == Qt::DisplayRole)
	{
		return m_data[index.row()][index.column()];
	}
	return QVariant();
}

bool MRCInfoTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (role != Qt::DisplayRole)
		return true;

	int row = index.row();
	int column = index.column();
	if (row < 0 || row >= m_rowCount || column < 0 || column >= m_columnCount)
		return false;
	m_data[row][column] = value;
    return true;
}

QVariant MRCInfoTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Horizontal){
        if(section == 0){
            return QVariant::fromValue(QString("Property"));
        }else if(section == 1){
            return QVariant::fromValue(QString("Value"));
        }

    }else if(orientation == Qt::Vertical){
        return QVariant::fromValue(QString::number(section));
    }
	return QVariant();
}

