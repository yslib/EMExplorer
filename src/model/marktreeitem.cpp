#include "marktreeitem.h"

StrokeMarkTreeItem::
StrokeMarkTreeItem(QGraphicsItem* markItem, QAbstractItemModel* model, TreeItem* parent): TreeItem(model, parent),
                                                                                          m_markItem(nullptr),m_infoModel(nullptr) {
	m_markItem = markItem;
	m_infoModel = new MarkItemInfoModel(markItem,nullptr);
}

QVariant StrokeMarkTreeItem::data(int column, int role) const {
	if (column >= columnCount())
		return QVariant();
	if (role == Qt::DisplayRole) {
		return m_markItem->data(MarkProperty::Name);
	}
	else if (role == Qt::DecorationRole) {
		return m_markItem->data(MarkProperty::Color);
	}
	else if (role == Qt::CheckStateRole) {
		return m_markItem->data(MarkProperty::VisibleState) == true ? Qt::Checked : Qt::Unchecked;
	}
	return QVariant();
}

bool StrokeMarkTreeItem::insertColumns(int position, int columns) {
	Q_UNUSED(position);
	Q_UNUSED(columns);
	return false;
}

bool StrokeMarkTreeItem::removeColumns(int position, int columns) {
	Q_UNUSED(position);
	Q_UNUSED(columns);
	return false;
}

int StrokeMarkTreeItem::columnCount() const {
	return 2;
}

bool StrokeMarkTreeItem::setData(int column, const QVariant& value, int role) {
	if (role == Qt::EditRole) {
		m_markItem->setData(MarkProperty::Name, value);
		return true;
	}
	else if (role == Qt::DecorationRole) {
		m_markItem->setData(MarkProperty::Color, value);
		return true;
	}
	else if (role == Qt::CheckStateRole) {
		m_markItem->setData(MarkProperty::VisibleState, value == Qt::Checked ? true : false);
		m_markItem->setVisible(value == Qt::Checked ? true : false);
		return true;
	}
}

/**
 * \brief Returns the information about the tree item.
 * \return 
 */
int StrokeMarkTreeItem::type() const { return TreeItemType::Mark; }

/**
 * \brief Returns meta data in the tree item
 * \return A void * pointer. You need to cast it to a specified type before using it.
 */
void* StrokeMarkTreeItem::metaData() 
{ return static_cast<void*>(m_markItem); }

/**
 * \brief Destroys the stroke mark tree item.
 */
StrokeMarkTreeItem::~StrokeMarkTreeItem() 
{
	delete m_markItem;
	m_infoModel->deleteLater();
}



/**
 * \brief Constructs a \a MarkItemInfoModel object.
 * \param mark 
 * \param parent 
 */
MarkItemInfoModel::MarkItemInfoModel(QGraphicsItem * mark, QObject * parent):m_markItem(nullptr),QAbstractItemModel(parent)
{
	m_markItem = mark;
}

/**
 * \brief Returns the data located in \a index
 * \param index 
 * \param role 
 * \return 
 * 
 * \Note:
 *	This is a 5 by 2 table. Color, Name, SliceIndex,SliceType, Visible State, Length are included.
 */
QVariant MarkItemInfoModel::data(const QModelIndex & index, int role) const
{
	if(m_markItem == nullptr)
		return QVariant();
	if(role == Qt::DisplayRole) {
		const auto r = index.row();
		const auto c = index.column();
		const static QString tableHeaders[] = {
			QStringLiteral("Color"),
			QStringLiteral("Name") ,
			QStringLiteral("Slice Index") ,
			QStringLiteral("Slice Type") ,
			QStringLiteral("Visible"),
			QStringLiteral("Length:") };
		if(c == 0) {
			return tableHeaders[r];
		}else if(c == 1) {
			switch (r) {
			case 0:return m_markItem->data(MarkProperty::Color);
			case 1:return m_markItem->data(MarkProperty::Name);
			case 2:return m_markItem->data(MarkProperty::SliceIndex);
			case 3:return m_markItem->data(MarkProperty::SliceType);
			case 4:return m_markItem->data(MarkProperty::VisibleState);
			case 5:return m_markItem->data(MarkProperty::Length);
			default:return QVariant{};
			}
		}
	}
	return QVariant();
}

/**
 * \brief 
 * \param parent 
 * \return 
 */
int MarkItemInfoModel::columnCount(const QModelIndex & parent) const
{
	return 2;
}

/**
 * \brief 
 * \param row 
 * \param column 
 * \param parent 
 * \return 
 */
QModelIndex MarkItemInfoModel::index(int row, int column, const QModelIndex & parent) const
{
	if(parent.isValid() == false) {
		return createIndex(row, column);
	}
	return QModelIndex{};
}

/**
 * \brief 
 * \param parent 
 * \return 
 */
int MarkItemInfoModel::rowCount(const QModelIndex & parent) const
{
	return 6;
}

/**
 * \brief Return an invalid \a QModelIndex for any child
 *
 * Because the parent of any child in a table model is root. i.e An invalid model index.
 * \param child
 * \return
 */
QModelIndex MarkItemInfoModel::parent(const QModelIndex & child) const
{
	return QModelIndex{};
}

/**
 * \brief 
 * \param index 
 * \param value 
 * \param role 
 * \return 
 */
bool MarkItemInfoModel::setData(const QModelIndex& index, const QVariant& value, int role) {
	return false;
}
