#include "instanceitem.h"

#include "algorithm/triangulate.h"


/**
 * \internal
 * \brief This is a internal helper function
 *
 * This function is used to sort marks by their position and divide them into several groups
 * \param marks Mass marks
 * \return Mark groups returned by the functions
 */
QVector<QList<StrokeMarkItem*>> InstanceTreeItem::refactorMarks(QList<StrokeMarkItem*>& marks)
{
	/* TODO::
	 * QList<QGraphicsItem*> marks has not been sorted so far. Maybe it can be sorted
	 * when item is inserted at once so as to get a better performance here.
	 */

	std::sort(marks.begin(), marks.end(), [](const StrokeMarkItem * it1, const StrokeMarkItem * it2)->bool
	{
		return it1->sliceIndex() << it2->sliceIndex();
	});

	/*
	 * After sorting by slice index. We need to add each mark item to corresponding mesh according to
	 * the maximum intersected area between bounding box of the mark item and the newest representative
	 * bounding box of the mesh.
	 */

	QVector<QList<StrokeMarkItem*>> meshes;
	QVector<QRectF> bounds;
	for (const auto item : marks) {
		auto meshIndex = -1;
		auto maxIntersectedArea = 0.0;
		const auto r = item->boundingRect();		// Rectangle of current mark
		for (auto i = 0; i < bounds.size(); i++) {
			if (bounds[i].intersects(r) == true) {
				const auto intersectedRect = bounds[i].intersected(r);
				const auto intersectedArea = intersectedRect.width()*intersectedRect.height();
				if (maxIntersectedArea < intersectedArea) {
					maxIntersectedArea = intersectedArea;
					meshIndex = i;
				}
			}
		}
		if (meshIndex != -1) {			// Add into a existed mesh
			bounds[meshIndex] = r;		// Update Rectangle
			meshes[meshIndex].push_back(item);

		}
		else {							// Create a new mesh
			bounds.push_back(r);
			meshes.push_back(QList<StrokeMarkItem*>{item});
		}
	}
	return meshes;
}


InstanceTreeItem::InstanceTreeItem(InstanceMetaData * metaData, const QPersistentModelIndex& pModelIndex, TreeItem* parent) :
	TreeItem(pModelIndex, parent),
	m_infoModel(nullptr),
	m_metaData(metaData)
{
	m_infoModel = new InstanceTreeItemInfoModel(m_metaData,this, nullptr);
}


/**
 * \brief
 * \param column
 * \param role
 * \return
 */
QVariant InstanceTreeItem::data(int column, int role) const
{
	if (m_metaData == nullptr)
		return QVariant{};
	if (role == Qt::DisplayRole)
	{
		if (column == 0) {
			return m_metaData->name();
		}
	}
	else if (role == Qt::CheckStateRole) {
		if (column == 0) {
			return m_metaData->visibleState() ? Qt::Checked : Qt::Unchecked;
		}
	}
	return QVariant{};
}

/**
 * \brief
 * \param column
 * \param value
 * \param role
 * \return
 */
bool InstanceTreeItem::setData(int column, const QVariant& value, int role) {
	if (m_metaData == nullptr)
		return false;
	if (role == Qt::EditRole) {
		if (column == 0) {
			m_metaData->setName(value.toString());
			return true;
		}
	}
	else if (role == Qt::CheckStateRole) {
		if (column == 0) {
			m_metaData->setVisibleState(value == Qt::Checked);
			return true;
		}
	}
	return false;
}

/**
 * \brief
 * \return
 */
int InstanceTreeItem::columnCount() const 
{
	return 2;
}

/**
 * \brief
 * \return
 */
int InstanceTreeItem::type() const 
{
	return TreeItemType::Instance;
}

/**
 * \brief Reimplemented from TreeItem::insertColums(int position, int columns)
 *
 * \param position
 * \param columns
 * \return
 */
bool InstanceTreeItem::insertColumns(int position, int columns) 
{
	return false;
}

/**
 * \brief
 * \param position
 * \param columns
 * \return
 */
bool InstanceTreeItem::removeColumns(int position, int columns)
{
	return false;
}

/**
 * \brief Retruns the meta data in the item
 *
 * \return Returns nullptr
 */
void* InstanceTreeItem::metaData() { return m_metaData; }

void InstanceTreeItem::setCurrentSelected(bool selected)
{

}

void InstanceTreeItem::setInfoView(QAbstractItemView* view)
{
	if (m_infoView != view)
	{
		if (m_infoView)
			m_infoView->setModel(nullptr);
		m_infoView = view;
		if (m_infoView)
			m_infoView->setModel(m_infoModel);
	}
}


QSharedPointer<Triangulate> InstanceTreeItem::mesh() const 
{
	QList<StrokeMarkItem*> marks;
	const auto nChild = childCount();
	for (auto i = 0; i < nChild; i++) {
		const auto item = child(i);
		if (item->type() == TreeItemType::Mark) {
			marks << static_cast<StrokeMarkItem*>(item->metaData());
		}
	}
	auto tri = QSharedPointer<Triangulate>(new Triangulate(marks));
	tri->triangulate();
	return tri;
}

InstanceTreeItem::~InstanceTreeItem() 
{
	delete m_metaData;
	m_metaData = nullptr;
	if(m_infoView) m_infoView->setModel(nullptr);
	m_infoModel->deleteLater();
	m_infoModel = nullptr;
}

QDataStream & operator<<(QDataStream & stream, const InstanceMetaData * metaData)
{
	Q_ASSERT(metaData);
	stream << metaData->m_name << metaData->m_region << metaData->m_visibleState;
	return stream;
}

QDataStream & operator>>(QDataStream & stream, InstanceMetaData *& metaData)
{
	metaData = new InstanceMetaData;
	stream >> metaData->m_name >> metaData->m_region >> metaData->m_visibleState;
	return stream;
}

QDataStream & operator<<(QDataStream & stream, const InstanceTreeItem * item)
{
	Q_ASSERT(item);
	stream << item->m_metaData;
	return stream;
}
QDataStream & operator>>(QDataStream & stream, InstanceTreeItem *& item) 
{
	InstanceMetaData * metaData = nullptr;
	stream >> metaData;		// allocate in it
	const auto newItem = new InstanceTreeItem(metaData, QModelIndex{}, nullptr);
	item = newItem;
	return stream;
}

InstanceTreeItemInfoModel::InstanceTreeItemInfoModel(InstanceMetaData * metaData, InstanceTreeItem * item, QObject * parent) :QAbstractItemModel(parent)
{
	m_treeItem = item;
	m_metaData = metaData;

	m_propertyNames << QStringLiteral("Name")
		<< QStringLiteral("Region")
		<< QStringLiteral("Visibility")
		<< QStringLiteral("Mark Count");
}

QVariant InstanceTreeItemInfoModel::data(const QModelIndex & index, int role) const
{
	Q_ASSERT(m_treeItem);
	Q_ASSERT(m_metaData);
	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		const auto r = index.row();
		const auto c = index.column();
		if (c == 0) {
			return m_propertyNames.value(r);
		}
		if (c == 1) {
			switch (r) 
			{
				case 0:return m_metaData->name();
				case 1:return m_metaData->region();
				case 2:return m_metaData->visibleState();
				case 3:return m_treeItem->childCount();
				default: return QVariant{};
			}
		}
	}
	return QVariant();
}

int InstanceTreeItemInfoModel::columnCount(const QModelIndex& parent) const 
{
	return 2;
}

QModelIndex InstanceTreeItemInfoModel::index(int row, int column, const QModelIndex & parent) const
{
	if (!parent.isValid())
	{
		return createIndex(row, column);
	}
	return QModelIndex{};
}

int InstanceTreeItemInfoModel::rowCount(const QModelIndex& parent) const 
{
	return m_propertyNames.size();
}

QModelIndex InstanceTreeItemInfoModel::parent(const QModelIndex & child) const
{
	return QModelIndex{};
}

bool InstanceTreeItemInfoModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	Q_ASSERT(m_treeItem);
	Q_ASSERT(m_metaData);
	if (role == Qt::EditRole) {
		const auto r = index.row();
		const auto c = index.column();
		if (c == 1) {
			switch (r)
			{
			case 0: {m_metaData->setName(value.toString()); return true; }
			case 2: {m_metaData->setVisibleState(value.toBool()); return true; }
			default: return false;
			}
		}
	}
	return false;
}

QVariant InstanceTreeItemInfoModel::headerData(int section, Qt::Orientation orientation, int role) const 
{
	if(orientation == Qt::Horizontal) 
	{
		if(role == Qt::DisplayRole) {
			if(section == 0) {
				return QStringLiteral("Property Name");
			}
			if (section == 1) {
				return QStringLiteral("Value");
			}
		}
	}
	return QVariant{};
}

Qt::ItemFlags InstanceTreeItemInfoModel::flags(const QModelIndex& index) const 
{
	return QAbstractItemModel::flags(index);
}
