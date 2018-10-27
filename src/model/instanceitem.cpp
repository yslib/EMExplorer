#include "instanceitem.h"


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

	std::sort(marks.begin(), marks.end(), [](const QGraphicsItem * it1, const QGraphicsItem * it2)->bool
	{
		Q_ASSERT_X(it1->data(MarkProperty::SliceIndex).canConvert<int>(), "MarkModel::refactorMarks", "it1 failed");
		Q_ASSERT_X(it2->data(MarkProperty::SliceIndex).canConvert<int>(), "MarkModel::refactorMarks", "it2 failed");
		return it1->data(MarkProperty::SliceIndex).value<int>() < it2->data(MarkProperty::SliceIndex).value<int>();
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


InstanceTreeItem::InstanceTreeItem(const QString & text, const QPersistentModelIndex& pModelIndex, TreeItem* parent) :
	TreeItem(pModelIndex, parent),
m_text(text),
m_checkState(1)
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
	return 1;
}

/**
 * \brief 
 * \return 
 */
int InstanceTreeItem::type() const {
	return TreeItemType::Instance;
}

/**
 * \brief Reimplemented from TreeItem::insertColums(int position, int columns)
 * 
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
 * \brief Retruns the meta data in the item
 * 
 * \return Returns nullptr
 */
void* InstanceTreeItem::metaData() { return nullptr; }


QSharedPointer<Triangulate> InstanceTreeItem::mesh() const {
	QList<StrokeMarkItem*> marks;
	const auto nChild = childCount();
	for(auto i=0;i<nChild;i++) {
		const auto item = child(i);
		if(item->type() == TreeItemType::Mark) {
			marks << static_cast<StrokeMarkItem*>(item->metaData());
		}
	}
	auto tri = QSharedPointer<Triangulate>::create(new Triangulate(marks));
	return tri;
}


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
