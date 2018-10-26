#include "instanceitem.h"


InstanceTreeItem::InstanceTreeItem(const QString & text, const QPersistentModelIndex& pModelIndex, TreeItem* parent) :
	TreeItem(pModelIndex, parent),
m_text(text),
m_checkState(0)
{

}


/**
 * \brief 
 * \param column 
 * \param role 
 * \return 
 */
QVariant InstanceTreeItem::data(int column, int role) const
{
	if(role == Qt::DisplayRole) {
		if(column == 0) {
			return m_text;
		}
	}else if(role == Qt::CheckStateRole) {
		if(column == 0) {
			return m_checkState == 1?Qt::Checked:Qt::Unchecked;
		}
	}
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
	if (role == Qt::EditRole) {
		if (column == 0) {
			m_text = value.toString();
			return true;
		}
	}
	else if (role == Qt::CheckStateRole) {
		if (column == 0) {
			m_checkState = (value == Qt::Checked ? 1 : 0);
			return true;
		}
	}
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



InstanceTreeItemInfoModel::InstanceTreeItemInfoModel(QObject * parent) :QAbstractItemModel(parent)
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
