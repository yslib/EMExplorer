#include "categorytreeitem.h"


CategoryTreeItemInfoModel::CategoryTreeItemInfoModel(QObject* parent):QAbstractItemModel(parent) {
	m_table.resize(rowCount(QModelIndex()));
	for(auto i=0;i<m_table.size();i++) {
		m_table.resize(columnCount(QModelIndex()));
	}
}
QVariant CategoryTreeItemInfoModel::data(const QModelIndex& index, int role) const 
{
	if (index.isValid() == false)
		return QVariant();
	const auto r = index.row();
	const auto c = index.column();
	Q_ASSERT_X(r == m_table.size(), "", "");
	Q_ASSERT_X(m_table.size() != 0 && m_table[0].size() == c, "", "");
	return m_table[r][c];
}

int CategoryTreeItemInfoModel::columnCount(const QModelIndex & parent) const
{
	if(parent.isValid() == false)
	{
		return 2;
	}
	return 0;
}

QModelIndex CategoryTreeItemInfoModel::index(int row, int column, const QModelIndex & parent) const
{
	if (parent.isValid() == false)		// Root Index
		return createIndex(row, column, nullptr);
	else
		return QModelIndex();
}

int CategoryTreeItemInfoModel::rowCount(const QModelIndex & parent) const
{
	if(parent.isValid() == false)		//Root Index
	{
		return 5;
	}
	return 0;
}

/**
 * \brief Return an invalid \a QModelIndex for any child
 * 
 * Because the parent of any cell in a table model is root. i.e An invalid model index.  
 * \param child 
 * \return 
 */
QModelIndex CategoryTreeItemInfoModel::parent(const QModelIndex & child) const
{
	Q_UNUSED(child);
	return QModelIndex{};
}

Qt::ItemFlags CategoryTreeItemInfoModel::flags(const QModelIndex& index) const 
{
	return QAbstractItemModel::flags(index);
}

/**
 * \brief Constructs a CategoryTreeItem
 * \param categoryItem The information necessarily needed by a \a CategroyTreeItem
 * \param parent Pointer references a parent TreeItem
 */
CategoryTreeItem::CategoryTreeItem(const CategoryItem& categoryItem,const QPersistentModelIndex & pIndex,TreeItem* parent): TreeItem(pIndex,parent),
                                                                                        m_categoryItem(categoryItem) {

}


/**
 * \brief Reimplemented from TreeItem::data(int column,int role)
 * \param column 
 * \param role 
 * \return 
 */
QVariant CategoryTreeItem::data(int column, int role) const {
	if (column >= columnCount())
		return QVariant{};
	if (role == Qt::DisplayRole) {
		return m_categoryItem.name();
	}
	else if (role == Qt::DecorationRole) {
		return m_categoryItem.color();
	}
	else if (role == Qt::CheckStateRole) {
		return m_categoryItem.visible()?Qt::Checked:Qt::Unchecked;
	}
	return QVariant();
}

/**
 * \brief Reimplemented from TreeItem::columnCount()
 * \return 
 */
int CategoryTreeItem::columnCount() const { return 1; }

/**
 * \brief Reimplemented from TreeItem::setData(int column,const QVariant& value,int role)
 * \param column 
 * \param value 
 * \param role 
 * \return 
 */
bool CategoryTreeItem::setData(int column, const QVariant& value, int role) {
	if (column >= columnCount())
		return false;
	if (role == Qt::EditRole) {
		
		m_categoryItem.setName(value.toString());
		return true;
	}
	else if (role == Qt::DecorationRole) {
		m_categoryItem.setColor(value.value<QColor>());
		return true;
	}
	else if (role == Qt::CheckStateRole) {
		m_categoryItem.setVisible(value == Qt::Checked ? true:false);
		return true;
	}
	return false;
}

/**
 * \brief Reimplemented from TreeItem::insertColumns(int position,int columns)
 * \param position 
 * \param columns 
 * \return 
 */
bool CategoryTreeItem::insertColumns(int position, int columns) {
	Q_UNUSED(position);
	Q_UNUSED(columns);
	return false;
}

/**
 * \brief Reimplemented from TreeItem::insertColumns(int position,int columns)
 * \param position 
 * \param columns 
 * \return 
 */
bool CategoryTreeItem::removeColumns(int position, int columns) {
	Q_UNUSED(position);
	Q_UNUSED(columns);
	return false;
}

/**
 * \brief Reimplemented from TreeItem::type()const
 * \return 
 */
int CategoryTreeItem::type() const { return TreeItemType::Category; }

/**
 * \brief 
 * \return Reimplemented from TreeItem::metaData()
 */
void* CategoryTreeItem::metaData() { return static_cast<void*>(&m_categoryItem); }

void CategoryTreeItem::setModelView(QAbstractItemView* view)
{
	if (m_infoView != view)
	{
		if (m_infoView)
			m_infoView->setModel(nullptr);
		m_infoView = view;
		if (m_infoView)
			m_infoView->setModel(nullptr);
	}
}

CategoryTreeItem::~CategoryTreeItem()
{
	if (m_infoView)
		m_infoView->setModel(nullptr);
}

QDataStream & operator<<(QDataStream & stream, const CategoryItem & item)
{
	stream << item.m_info.name << item.m_info.color << item.m_visible;
	return stream;
}

QDataStream & operator>>(QDataStream & stream, CategoryItem & item)
{
	stream >> item.m_info.name >> item.m_info.color >> item.m_visible;
	Q_ASSERT_X(stream.status() != QDataStream::ReadPastEnd,
		"Category::operator<<", "corrupt data");
	return stream;
}

QDataStream& operator<<(QDataStream& stream, const CategoryTreeItem * item) 
{
	Q_ASSERT(item);
	stream << item->m_categoryItem;
	return stream;
}

QDataStream & operator>>(QDataStream & stream, CategoryTreeItem *& item) 
{
	const auto newItem = new CategoryTreeItem(CategoryItem(), QModelIndex(), nullptr);
	stream >> (newItem->m_categoryItem);
	item = newItem;
	return stream;
}




