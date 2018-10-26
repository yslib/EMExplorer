#include "instanceitem.h"


InstanceTreeItem::InstanceTreeItem(const QPersistentModelIndex& pModelIndex, TreeItem* parent) :
	TreeItem(pModelIndex, parent) {

}


QVariant InstanceTreeItem::data(int column, int role) const
{
	return QVariant();
}

/**
 * \brief 
 * \param column 
 * \param value 
 * \param role 
 * \return 
 */
bool InstanceTreeItem::setData(int column, const QVariant& value, int role) {
	return false;
}

/**
 * \brief 
 * \return 
 */
int InstanceTreeItem::columnCount() const {
	return 0;
}

/**
 * \brief 
 * \return 
 */
int InstanceTreeItem::type() const {
	return 0;
}

/**
 * \brief 
 * \param position 
 * \param columns 
 * \return 
 */
bool InstanceTreeItem::insertColumns(int position, int columns) {
	return false;
}

/**
 * \brief 
 * \param position 
 * \param columns 
 * \return 
 */
bool InstanceTreeItem::removeColumns(int position, int columns) {
	return false;
}

/**
 * \brief 
 * \return 
 */
void* InstanceTreeItem::metaData() { return nullptr; }

InstanceTreeItemInfoModel::InstanceTreeItemInfoModel(QObject * parent)
{

}

QVariant InstanceTreeItemInfoModel::data(const QModelIndex & index, int role) const
{
	return QVariant();
}

int InstanceTreeItemInfoModel::columnCount(const QModelIndex& parent) const { return 2; }

QModelIndex InstanceTreeItemInfoModel::index(int row, int column, const QModelIndex & parent) const
{
	return QModelIndex();
}

int InstanceTreeItemInfoModel::rowCount(const QModelIndex& parent) const { return 1; }

QModelIndex InstanceTreeItemInfoModel::parent(const QModelIndex & child) const
{
	return QModelIndex();
}

bool InstanceTreeItemInfoModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	return false;
}
