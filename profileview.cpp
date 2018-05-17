#include "profileview.h"


TableModel::TableModel(QObject* parent)
{

}

int TableModel::rowCount(const QModelIndex& parent) const
{
	return 0;
}

int TableModel::columnCount(const QModelIndex& parent) const
{
	return 0;
}

QVariant TableModel::data(const QModelIndex& index, int role) const
{
	return QVariant();
}