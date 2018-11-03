#include "marktreeitem.h"
#include "markmodel.h"
#include <QAbstractItemView>
#include <QGraphicsScene>

StrokeMarkTreeItem::
StrokeMarkTreeItem(StrokeMarkItem* markItem, const QPersistentModelIndex & pIndex, TreeItem* parent) : TreeItem(pIndex, parent),
m_markItem(nullptr), 
m_infoModel(nullptr) {
	m_markItem = markItem;

	//if (m_markItem != nullptr) {
	//	// Add state change handler
	//	m_markItem->setItemChangeHandler([this](StrokeMarkItem * mark,QGraphicsItem::GraphicsItemChange change, const QVariant & value)->QVariant {
	//		if (this->persistentModelIndex().isValid() == false)
	//		{
	//			qWarning("QPersistentModelIndex is invalid");
	//		}
	//		else if (change == QGraphicsItem::GraphicsItemChange::ItemSelectedChange) {
	//			if(value.toBool() == false)			//The item is presently selected. Do nothing
	//			{
	//				return value;
	//			}
	//			const auto model = persistentModelIndex().model();
	//			//This is a bad design. But there is a no better remedy so far.
	//			const auto markModel = static_cast<const MarkModel*>(model);
	//			if (markModel != nullptr) {
	//				const auto selectionModel = markModel->selectionModelOfThisModel();
	//				
	//				mark->scene()->clearSelection();
	//				selectionModel->clearSelection();
	//				selectionModel->setCurrentIndex(persistentModelIndex(), QItemSelectionModel::Current);
	//				selectionModel->setCurrentIndex(persistentModelIndex(), QItemSelectionModel::Select);
	//			}
	//		}
	//		return value;
	//	});
	//}

	//QVariant::Color;

	m_markItem->m_modelIndex = persistentModelIndex();

	m_infoModel = new MarkItemInfoModel(markItem, nullptr);

	//Q_ASSERT(parentItem());

	m_markItem->setName(QStringLiteral("# %1 #").number(1));
}

QVariant StrokeMarkTreeItem::data(int column, int role) const {

	if (column >= columnCount())
		return QVariant();
	if(column == 0) 
	{
		if (role == Qt::DisplayRole)
		{
			return m_markItem->name();
		}
		if (role == Qt::DecorationRole)
		{
			return m_markItem->pen().color();
		}
		if (role == Qt::CheckStateRole)
		{
			return m_markItem->visibleState() ? Qt::Checked : Qt::Unchecked;
		}
	}
	if(column == 1) {
		if(role == Qt::DisplayRole) 
		{
			return m_markItem->sliceIndex();
		}
	}

	return QVariant{};
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

int StrokeMarkTreeItem::columnCount() const 
{
	return 2;
}

bool StrokeMarkTreeItem::setData(int column, const QVariant& value, int role) {

	if (role == Qt::EditRole) 
	{
		m_markItem->setName(value.toString());
		return true;
	}
	if (role == Qt::CheckStateRole) 
	{
		m_markItem->setVisibleState(value == Qt::Checked);
		return true;
	}
	return false;
}

/**
 * \brief Returns the information about the tree item.
 * \return
 */
int StrokeMarkTreeItem::type() const 
{
	return TreeItemType::Mark;
}

/**
 * \brief Returns meta data in the tree item
 * \return A void * pointer. You need to cast it to a specified type before using it.
 */
void* StrokeMarkTreeItem::metaData()
{
	return static_cast<void*>(m_markItem);
}

/**
 * \brief Destroys the stroke mark tree item.
 */
StrokeMarkTreeItem::~StrokeMarkTreeItem()
{
	delete m_markItem;
	m_infoModel->deleteLater();
}

QDataStream& operator<<(QDataStream& stream, const StrokeMarkTreeItem * item) 
{
	Q_ASSERT(item);
	stream << item->m_markItem;
	return stream;
}

QDataStream & operator>>(QDataStream & stream, StrokeMarkTreeItem *& item) 
{
	StrokeMarkItem * mark;
	stream >> mark;
	const auto newItem = new StrokeMarkTreeItem(mark, QModelIndex{}, nullptr);
	item = newItem;
	return stream;
}




/**
 * \brief Constructs a \a MarkItemInfoModel object.
 * \param mark
 * \param parent
 */
MarkItemInfoModel::MarkItemInfoModel(StrokeMarkItem * mark, QObject * parent) :m_markItem(nullptr), QAbstractItemModel(parent)
{
	m_markItem = mark;

	propertyNames << QStringLiteral("Name") 
	<< QStringLiteral("Slice Index") 
	<< QStringLiteral("Slice Type") 
	<< QStringLiteral("Visible") 
	<< QStringLiteral("Color") 
	<< QStringLiteral("Length:");

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
	if (m_markItem == nullptr)
		return QVariant{};
	const auto r = index.row();
	const auto c = index.column();
	if ( role == Qt::EditRole || role == Qt::DisplayRole) 
	{
		if (c == 0) 
		{
			return propertyNames[r];
		}
		if (c == 1) 
		{
			switch (r) 
			{
				case 0:return m_markItem->name();
				case 1:return m_markItem->sliceIndex();
				case 2:return static_cast<int>(m_markItem->sliceType());
				case 3:return m_markItem->visibleState();
				case 4:return m_markItem->pen().color();	// Color
				case 5:return m_markItem->length();
				default:return QVariant{};
			}
		}
	}
	if(role == Qt::BackgroundColorRole) {
		if(r == 4 && c == 1) 
		{
			return m_markItem->pen().color();
		}
	}
	return QVariant{};
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
	if (!parent.isValid()) 
	{
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
	return propertyNames.size();
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
bool MarkItemInfoModel::setData(const QModelIndex& index, const QVariant& value, int role)
{

	Q_ASSERT(m_markItem);

	if (!index.isValid())
		return false;
	const auto r = index.row();
	const auto c = index.column();
	if(role == Qt::EditRole)		// Edits name of the mark
	{
		qDebug() << "???";
		if( c == 1) {
			if(r == 0) {			// Name
				m_markItem->setName(value.toString());
				emit dataChanged(index, index, { role });
				return true;
			}
			if(r == 3) {
				m_markItem->setVisibleState(value.toBool());
				emit dataChanged(index, index, { role });
				return true;
			}
			if(r == 4) {
				auto p = m_markItem->pen();
				p.setColor(qvariant_cast<QColor>(value));
				m_markItem->setPen(p);
				emit dataChanged(index, index, { role });
				return true;
			}

		}
	}
	return false;
}

/**
 * \brief Reimplemented from QAbstractItemModel::headerData(int section, Qt::Orientation orientation, int role)
 *
 * Returns the data for the given role and section in the header with the specified orientation.
 * For horizontal headers, the section number corresponds to the column number. Similarly,
 *  for vertical headers, the section number corresponds to the row number.
 * \param section
 * \param orientation
 * \param role
 * \return
 */
QVariant MarkItemInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole) 
	{
		if (orientation == Qt::Horizontal) 
		{
			switch(section) 
			{
				case 0:return QStringLiteral("Property");
				case 1:return QStringLiteral("Value");
				default:return QVariant{};
			}

		}
	}
	return QVariant{};
}

Qt::ItemFlags MarkItemInfoModel::flags(const QModelIndex& index) const 
{
	if (!index.isValid())
		return Qt::NoItemFlags;
	const auto r = index.row();
	const auto c = index.column();
	if(c == 1)
	{
		switch(r) 
		{
			case 0:case 3:case 4:									// Name Visibility Color					
			return Qt::ItemIsEnabled | Qt::ItemIsEditable;			
			default:return Qt::ItemIsEnabled;							// Others
		}
	}
	if(c == 0) 
	{
		return Qt::ItemIsEnabled;
	}
	return QAbstractItemModel::flags(index);
}
