#include "markmodel.h"
#include "widgets/sliceeditorwidget.h"
#include "globals.h"
#include "abstract/abstractslicedatamodel.h"
#include "markitem.h"
#include "model/roottreeitem.h"
#include "model/categorytreeitem.h"

#include <QStyledItemDelegate>
#include <QScopedPointer>
#include <QAbstractItemView>
#include <QPainter>
#include <QDebug>
#include <cstring>


/**
 * \brief This is a function to get the internal data structure from \a index
 *
 *
 * \param index \a Either an invalid index or a valid index with
 * internal pointer refer to root tree item gives the pointer refer to root item
 * \return Returns a non-null pointer anyway
 */
TreeItem* MarkModel::treeItem(const QModelIndex& index) const
{
	if (index.isValid())
	{
		const auto item = static_cast<TreeItem*>(index.internalPointer());
		if (item)
			return item;
	}
	return m_rootItem;
}

/**
 * \brief  This is a convenience function to returns the root tree item pointer
 */
TreeItem* MarkModel::rootItem() const 
{
	return (m_rootItem);
}

/**
 * \brief
 * \param mark
 */
void MarkModel::addMarkInSliceHelper(StrokeMarkItem * mark)
{
	const int index = mark->sliceIndex();
	MarkSliceList * markList;
	switch (mark->sliceType())
	{
	case SliceType::Top:
		markList = &m_topSliceVisibleMarks;
		break;
	case SliceType::Right:
		markList = &m_rightSliceVisibleMarks;
		break;
	case SliceType::Front:
		markList = &m_frontSliceVisibleMarks;
		break;
	}
	(*markList)[index].append(mark);

	setDirty();
}

/**
 * \brief
 * \param mark
 */
void MarkModel::removeMarkInSliceHelper(StrokeMarkItem * mark)
{
	const auto index = mark->sliceIndex();
	const auto type = mark->sliceType();
	switch (type)
	{
	case SliceType::Top:
		m_topSliceVisibleMarks[index].removeOne(mark);
		break;
	case SliceType::Front:
		m_rightSliceVisibleMarks[index].removeOne(mark);
		break;
	case SliceType::Right:
		m_frontSliceVisibleMarks[index].removeOne(mark);
		break;
	}
	setDirty();
}

/**
 * \brief
 * \param mark
 */
void MarkModel::updateMarkVisibleHelper(StrokeMarkItem * mark)
{
	//Q_ASSERT_X(m_view,"MarkModel::updateMarkVisible_Helper","null pointer");
	if (m_view == nullptr)
		return;
	int index = -1;

	index = m_view->currentSliceIndex(static_cast<SliceType>(mark->sliceType()));
	if (index != mark->sliceIndex())
		return;

	const auto visible = mark->visibleState();
	mark->setVisible(visible);
	setDirty();
}
QSharedPointer<char> MarkModel::rawMarks() const
{

	const auto topSize = tester().topSliceSize();
	QImage slice(topSize, QImage::Format_Grayscale8);
	slice.fill(Qt::black);
	const size_t width = slice.width(), height = slice.height(), depth = tester().rightSliceSize().width();
	QSharedPointer<char> buffer(new char[width * height * depth], [](char * obj) {delete[] obj; });
	size_t sliceCount = 0;
	Q_ASSERT(width == slice.bytesPerLine());
	const auto visibleMarks = topVisibleMarks();
	QPainter p;
	for (const auto & items : visibleMarks) {
		p.begin(&slice);
		for (const auto & item : items) {
			if (item != nullptr) {
				item->paint(&p, nullptr, nullptr);
			}
		}
		p.end();
		//qDebug() << "Slice:" << sliceCount << " count:" << items.size();
		// Because QImage is 32bit-aligned, so we need write it for each scan line
		for (auto i = size_t(0); i < height; i++)
			std::memcpy(buffer.data() + width * height * sliceCount + i * width, slice.bits() + i * slice.bytesPerLine(), slice.bytesPerLine());
		sliceCount++;
		
		slice.fill(Qt::black);		// This will also use paint device
	}

	return buffer;
}
QSharedPointer<int> MarkModel::markMask() const
{
	const auto topSize = tester().topSliceSize();
	QImage slice(topSize, QImage::Format_ARGB32_Premultiplied);
	slice.fill(Qt::black);
	const size_t width = slice.width(), height = slice.height(), depth = tester().rightSliceSize().width();
	QSharedPointer<int> buffer(new int[width * height * depth], [](int * obj) {delete[] obj; });

	size_t sliceCount = 0;
	const auto visibleMarks = topVisibleMarks();
	QPainter p(&slice);
	for (const auto & items : visibleMarks) {
		const auto offset = buffer.data() + sliceCount * width*height;
		
		for (const auto & item : items)
			if (item != nullptr)
				item->paint(&p, nullptr, nullptr);

		for (auto h = size_t(0); h < height; h++) {
			for (auto w = size_t(0); w < width; w++) {
				const QColor color = slice.pixel(w, h);
				offset[h*width + w] = color.red() + color.green() * 255 + color.blue() * 255 * 255;		// Convert RGB Color to a integer by a one-to-one mapping
			}
		}
		sliceCount++;
		slice.fill(Qt::black);
	}
	return buffer;
}
void MarkModel::retrieveData(TreeItem * root, TreeItemType type, int column, QVector<QVariant>& data, int role)
{
	if (root == nullptr)
		return;
	if (root->type() == type) {
		if (role == MetaDataRole) {
			data << QVariant::fromValue(root->metaData());
		}
		else {
			data << root->data(column, role);
		}
	}
	for (int i = 0; i < root->childCount(); i++) {
		retrieveData(root->child(i), type, column, data, role);
	}

}

/**
 * \brief This is a helper function for get specified \a type to \a items in the parent of \parent
 *
 * It is called in recursively, which may lead a time-consuming operation.
 */
void MarkModel::retrieveTreeItem(TreeItem * parent, TreeItemType type, QList<TreeItem*>* items)
{
	Q_ASSERT(items);
	if (parent == nullptr)
	{
		return;
	}
	const auto nChild = parent->childCount();
	for (auto i = 0; i < nChild; i++) {
		auto item = parent->child(i);
		if (item->type() == type) {
			items->append(item);
		}
		retrieveTreeItem(item, type, items);
	}
}

QModelIndex MarkModel::_hlp_indexByItem(TreeItem * parent, TreeItem* item) {

	if(parent != nullptr) {
		const auto nChild = parent->childCount();
		for(int i=0;i<nChild;i++) {
			const auto child = parent->child(i);
			if(child == item) {
				return createIndex(i, 0, item);
			}else {
				return _hlp_indexByItem(child, item);
			}
		}
	}
	return QModelIndex();
}


/**
 * \interal
 * \brief This is a internal helper function
 *
 * This function is used to initialize and identify whether mark model and slice model match
 */
void MarkModel::initSliceMarkContainerHelper()
{
	Q_ASSERT_X(m_identity.isValid() == true,
		"MarkModel::initSliceMarkContainer_Helper", "IdentityTester is not valid");
	m_topSliceVisibleMarks.resize(m_identity.topSliceCount());
	m_rightSliceVisibleMarks.resize(m_identity.rightSliceCount());
	m_frontSliceVisibleMarks.resize(m_identity.frontSliceCount());
}

/**
 * \internal
 * \brief This is a internal helper function
 *
 * This function is used to sort marks by their position and divide them into several groups
 * \param marks Mass marks
 * \return Mark groups returned by the functions
 */
QVector<QList<StrokeMarkItem*>> MarkModel::refactorMarks(QList<StrokeMarkItem*> & marks)
{
	/* TODO::
	 * QList<QGraphicsItem*> marks has not been sorted so far. Maybe it can be sorted
	 * when item is inserted at once so as to get a better performance here.
	 */

	std::sort(marks.begin(), marks.end(), [](const StrokeMarkItem * it1, const StrokeMarkItem * it2)->bool
	{
		return it1->sliceIndex() < it2->sliceIndex();
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




/**
 * \brief This is a private constructor
 *
 * A mark model can only be constructed by SliceEditorWidget.
 * \sa SliceEditorWidget
 */
MarkModel::MarkModel(AbstractSliceDataModel* dataModel,
	SliceEditorWidget * view,
	//TreeItem * root,
	QObject * parent) :
	QAbstractItemModel(parent),
	//m_dataModel(dataModel),
	m_rootItem(nullptr),
	m_view(view),
	m_dirty(false),
	m_selectionModel(new QItemSelectionModel(this, this)),
	m_identity(dataModel)
{
	m_rootItem = new RootTreeItem(QModelIndex(), nullptr);
	m_rootItem->updateModelIndex(createIndex(0, 0, m_rootItem));
	initSliceMarkContainerHelper();
}



/**
 * \brief Creates a mark model from a file.
 * 
 */
MarkModel::MarkModel(const QString & fileName) :
	m_rootItem(nullptr),
	//m_dataModel(nullptr),
	m_view(nullptr),
	m_dirty(false),
	m_selectionModel(new QItemSelectionModel(this, this))
{

	QFile file(fileName);
	file.open(QIODevice::ReadOnly);
	if (file.isOpen() == false)
		return;
	QDataStream in(&file);
	in.setVersion(QDataStream::Qt_5_9);
	int magicNumber;
	in >> magicNumber;
	if (magicNumber != MagicNumber)
		return;

	/////
	in >> m_identity;
	TreeItem * root;
	in >> root;
	if(root->type() != TreeItemType::Root) {
		Q_ASSERT_X(false, "MarkModel::MarkModel(const QString & fileName)", "Not a root tree item");
		return;
	}
	m_rootItem = static_cast<RootTreeItem*>(root);
	m_rootItem->setModelIndexRecursively(createIndex(0,0,m_rootItem));
	////

	//construct sliceMarks from the tree
	initSliceMarkContainerHelper();

	auto items = QList<StrokeMarkItem*>();		//
	QVector<QVariant> data;
	retrieveData(m_rootItem, TreeItemType::Mark, 0, data, MetaDataRole);
	foreach(const auto var, data)
	{
		Q_ASSERT_X(var.canConvert<void*>(),
			"MarkModel::marks", "convert failed");
		items << static_cast<StrokeMarkItem*>(var.value<void*>());
	}


	foreach(auto item,items)
	{
		SliceType type = item->sliceType();
		const auto index = item->sliceIndex();

		item->setFlags(QGraphicsItem::ItemIsSelectable);

		switch(type)
		{
		case SliceType::Top:
			m_topSliceVisibleMarks[index].append(item);
			break;
		case SliceType::Right:
			m_rightSliceVisibleMarks[index].append(item);
			break;
		case SliceType::Front:
			m_frontSliceVisibleMarks[index].append(item);
			break;
		}
	}
}
/**
 * \brief Destroyes the mark model
 *
 */
MarkModel::~MarkModel()
{
	if (m_rootItem != nullptr)
	{
		delete m_rootItem;
		m_rootItem = nullptr;
	}
	//if(m_selectionModel != nullptr) {		//It's trivial.
	//	m_selectionModel->deleteLater();
	//}
}

/**
 * \brief  Returns all tree items according to a given parent index \a parent and a \a type
 */
QList<TreeItem*> MarkModel::treeItems(const QModelIndex & parent, int type)
{
	QList<TreeItem*> items;
	const auto item = treeItem(parent);
	retrieveTreeItem(item, (TreeItemType)type, &items);
	return items;
}

/**
 * \brief Return the index represents the underlying data \a item
 * 
 * \param item 
 * \return Returns a valid \a QModelIndex if the \a item exsits in the tree model otherwise return
 * an invalid QModelIndex
 * 
 * \note An invalid QModelIndex indicates that there is no an \a item in the tree model
 * rather than it represents a root item
 */
QModelIndex MarkModel::indexByItem(TreeItem * item)
{
	return _hlp_indexByItem(m_rootItem, item);
}

/**
 * \brief This is a convenience function for inserting an tree item into the tree model by the pointer itself \a item
 * and its parent index \a parent
 * 
 * \param item The pointer need to be inserted
 * \param parent The parent index of the pointer 
 * \return Returns \a true if it is inserted successfully otherwise returns \a false
 */
bool MarkModel::insertTreeItem(TreeItem * item, const QModelIndex & parent)
{
	const auto nChild = rowCount(parent);
	const auto success = insertRows(nChild, 1, parent);
	if(success) {
		const auto newIndex = index(nChild, 0, parent);
		setData(newIndex, QVariant::fromValue<void*>(static_cast<void*>(item)), TreeItemRole);
	}
	return success;
}

/**
 * \brief This is a convenience function for inserting a bundle of tree items into the tree model by the pointers themselves \a items
 * and their parent index \a parent
 *
 * \param items The pointers needed to be inserted
 * \param parent The parent index of the pointers
 * \return Returns \a true if they are inserted successfully otherwise returns \a false
 */
bool MarkModel::insertTreeItems(const QList<TreeItem*>& items, const QModelIndex & parent)
{
	const auto nChild = rowCount(parent), nItems = items.size();
	const auto success = insertRows(nChild, nItems, parent);
	if (success) {
		for(int i=0;i<nItems;i++) {
			const auto newIndex = index(nChild+i, 0, parent);
			setData(newIndex, QVariant::fromValue<void*>(static_cast<void*>(items[i])), TreeItemRole);
		}
	}
	return success;
}

/**
 * \brief This is a convenience function for removing a item in the the tree model by its pointer \item and its parent index \parent
 *
 * \param item The pointer need to be removed
 * \param parent The parent index of the pointer \a item belongs to
 * \return Returns \a true if it is deleted successfully otherwise returns \a false
 */
bool MarkModel::removeTreeItem(TreeItem * item)
{
	const auto index = indexByItem(item);
	const auto parent = MarkModel::parent(index);
	return removeRow(index.row(), parent);
}

/**
 * \brief This is a convenience function for removed a bundle of tree items into the tree model by the pointers themselves \a items
 *
 * \param items The pointers needed to be inserted
 * \return Returns \a true if they are removed successfully otherwise returns \a false
 */
bool MarkModel::removeTreeItems(const QList<TreeItem*>& items)
{
	for (auto item : items)
		removeTreeItem(item);
	return true;
}

/**
 * \brief Save current marks contained in the mark model
 * \param fileName
 * \param format
 * \return return \a true if saving successes or return \a false
 */
bool MarkModel::save(const QString& fileName, MarkModel::MarkFormat format)
{
	if (format == MarkFormat::Binary)
	{
		QFile file(fileName);
		file.open(QIODevice::WriteOnly);
		if (!file.isOpen())
			return false;

		QDataStream stream(&file);
		stream.setVersion(QDataStream::Qt_5_8);
		stream << static_cast<qint32>(MagicNumber);			// Magic Number first
		stream << m_identity;								// Identity Tester
		stream << static_cast<TreeItem*>(m_rootItem);		// Root Item
		resetDirty();
		return true;
	}
	else if (format == MarkFormat::Raw)
	{
		//resetDirt();
		//QImage origin = m_dataModel->originalTopSlice(0);
		const auto topSize = tester().topSliceSize();
		//QImage slice(topSize, QImage::Format_Grayscale8);
		//slice.fill(Qt::black);
		const size_t width = topSize.width(), height = topSize.height(), depth = tester().rightSliceSize().width();
		//QSharedPointer<char> buffer(new char[width * height * depth], [](char * obj) {delete[] obj; });
		//size_t sliceCount = 0;
		//Q_ASSERT(width == slice.bytesPerLine());
		//const auto visibleMarks = topVisibleMarks();
		//for(const auto & items: visibleMarks) {
		//	QPainter p(&slice);
		//	for(const auto & item: items) {
		//		if (item != nullptr) {
		//			item->paint(&p, nullptr, nullptr);
		//		}
		//	}
		//	// Because QImage is 32bit-aligned, so we need write it for each scan line
		//	for(auto i = size_t(0) ;i < height; i++) 
		//		std::memcpy(buffer.data() + width * height * sliceCount + i*width, slice.bits()+i*slice.bytesPerLine(), slice.bytesPerLine());
		//	sliceCount++;
		//	slice.fill(Qt::black);
		//}
		const auto buffer = rawMarks();
		QFile out(fileName);
		out.open(QIODevice::WriteOnly);
		if (!out.isOpen())
			return false;
		out.write(static_cast<const char *>(buffer.data()), width*height*depth*sizeof(char));
		resetDirty();
		return true;

	}else if(MarkFormat::Mask == format) {
		//const auto topSize = tester().topSliceSize();
		//QImage slice(topSize, QImage::Format_ARGB32_Premultiplied);
		const auto topSize = tester().topSliceSize();
		//slice.fill(Qt::black);
		//const size_t width = slice.width(), height = slice.height(), depth = tester().rightSliceSize().width();
		//QSharedPointer<int> buffer(new int[width * height * depth],[](int * obj) {delete[] obj;});
		const size_t width = topSize.width(), height = topSize.height(), depth = tester().rightSliceSize().width();
		//size_t sliceCount = 0;
		//const auto visibleMarks = topVisibleMarks();
		//for (const auto & items : visibleMarks) {
		//	const auto offset = buffer.data() + sliceCount * width*height;
		//	QPainter p(&slice);
		//	for (const auto & item : items) 
		//		if (item != nullptr) 
		//			item->paint(&p, nullptr, nullptr);
		//		
		//	for (auto h = size_t(0); h < height; h++) {
		//		for (auto w = size_t(0); w < width; w++) {
		//			const QColor color = slice.pixel(w, h);
		//			offset[h*width + w] = color.red() + color.green() * 255 + color.blue() * 255 * 255;		// Convert RGB Color to a integer by a one-to-one mapping
		//		}
		//	}
		//	sliceCount++;
		//	slice.fill(Qt::black);
		//}
		const auto buffer = markMask();
		QFile out(fileName);
		out.open(QIODevice::WriteOnly);
		if (!out.isOpen())
			return false;
		out.write(reinterpret_cast<const char *>(buffer.data()), width*height*depth*sizeof(int));
		resetDirty();
		return true;
	}

	return false;
}

/**
 * \brief Reimplemented from QAbstractItemModel::data(const QModelIndex & index,int role)
 *
 * Available \a role includes all enums in \a Qt::ItemDataRole
 * \param index
 * \param role
 * \return
 */

QVariant MarkModel::data(const QModelIndex & index, int role) const
{
	if (index.isValid() == false)
		return QVariant();
	const auto item = treeItem(index);
	Q_ASSERT_X(item, "MarkModel::data", "null pointer");
	return item->data(index.column(), role);
}

/**
 * \brief Reimplemeted from QAbstractItemModel::flags(const QModelIndex & index)const
 */
Qt::ItemFlags MarkModel::flags(const QModelIndex & index) const
{
	if (index.isValid() == false)		//root
		return 0;
	if (index.column() == 0)			//only the first column can be selected
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEditable;
	if (index.column() == 1)
		return Qt::ItemIsEnabled;
	return QAbstractItemModel::flags(index);
	//return 0;
}

/**
 * \brief Reimplemented from QAbstractItemModel::setData(const QModelIndex & index, const QVariant & value, int role)
 *
 *
 * Available \a role includes all enums in \a Qt::ItemDataRole.
 *
 * There are some customized roles besides above.
 * \a MetaDataRole means that this function call will set \a value as the meta data field of \a TreeItem*, which
 * is the exact data stored in the \a TreeItem.
 * \a TreeItemRole means that this function call will set \a value as the specific instance of \a TreeItem in
 * the node represented by \a index. \a Under this situation, the type of internal data in \a value should be \a void*
 * and the value of \a column() of \a index is ignored
 *
 * \return Returns \a true if this call is successful otherwise return \a false.
 *
 * \note If \a role is Qt::CheckStateRole, the function will be called recursively to apply the same setting
 * for its children
 *
 * \warning When \a role is TreeItemRole, the old \a TreeItem* pointer would be deleted at once.
 * \sa TreeItem, Qt::ItemDataRole, MarkModelItemRole
 * 
 * \internal
 */
bool MarkModel::setData(const QModelIndex & index, const QVariant & value, int role)
{

	if (role == TreeItemRole) {
		//Insert specially. column of the index is ignored.

		const auto r = index.row();
		const auto parentModelIndex = parent(index);
		const auto item = treeItem(parentModelIndex);

		const auto newItem = static_cast<TreeItem*>(value.value<void*>());

		if(newItem->type() == TreeItemType::Mark)		// If it is a mark, add to slice mark cache.
		{
			addMarkInSliceHelper(static_cast<StrokeMarkItem*>(newItem->metaData()));
		}
		delete item->takeChild(index.row(), newItem, nullptr);
		// Update the internal pointer refer to underlying data
		const auto newIndex = createIndex(index.row(), index.column(), newItem);
		newItem->updateModelIndex(newIndex);
		emit dataChanged(newIndex, newIndex, QVector<int>{role});
		return true;
	}
	else {							//Insert normally.

		const auto item = treeItem(index);
		Q_ASSERT_X(item, "MarkModel::data", "null pointer");
		if (item == nullptr) return false;
		item->setData(index.column(), value, role);

		if(item->type() == TreeItemType::Mark) 
		{
			updateMarkVisibleHelper(static_cast<StrokeMarkItem*>(item->metaData()));
		}

		if (role == Qt::CheckStateRole) {	// The modification on CheckStateRole will be applied recursively.
			const auto c = rowCount(index);
			for (int i = 0; i < c; i++) {
				setData(MarkModel::index(i, 0, index), value, Qt::CheckStateRole);
			}
		}

		emit dataChanged(index, index, QVector<int>{role});
		return true;
	}

}

/**
 * \brief  insert \a count columns at the position \a column of \a parent
 * 
 * Reimplemented from QAbstractItemModel::insertColumns(int column, int count, const QModelIndex & parent)
 */
bool MarkModel::insertColumns(int column, int count, const QModelIndex & parent)
{
	const auto item = treeItem(parent);
	if (item == nullptr)
		return false;
	beginInsertColumns(parent, column, column + count - 1);
	const auto success = item->insertColumns(column, count);
	endInsertColumns();
	return success;
}

/**
 * \brief Reimplemented from QAbstractItemModel::removeColumns(int column, int count, const QModelIndex & parent))
 */
bool MarkModel::removeColumns(int column, int count, const QModelIndex & parent)
{
	const auto item = treeItem(parent);
	if (item == nullptr)
		return false;
	beginRemoveColumns(parent, column, column + count - 1);
	const auto success = item->insertColumns(column, count);
	endRemoveColumns();
	return success;
}

/**
 * \brie Reimplemented from QAbstractItemModel::insertRows(int row, int count, const QModelIndex & parent)
 */
bool MarkModel::insertRows(int row, int count, const QModelIndex & parent)
{
	const auto item = treeItem(parent);
	beginInsertRows(parent, row, count + row - 1);

	QVector<TreeItem*> children;
	for (auto i = 0; i < count; i++)
	{
		children << new EmptyTreeItem(QPersistentModelIndex(QModelIndex()), item);
	}
	const auto success = item->insertChildren(row, children);
	endInsertRows();
	setDirty();
	return success;
}

/**
 * \brie Reimplemented from QAbstractItemModel::removeRows(int row, int count, const QModelIndex & parent)
 */

bool MarkModel::removeRows(int row, int count, const QModelIndex & parent)
{
	const auto item = treeItem(parent);
	beginRemoveRows(parent, row, row + count - 1);
	const auto success = item->removeChildren(row, count);
	endRemoveRows();
	return success;
}

/**
 * \brief Reimplemented from QAbstractItemModel::headerData(int section, Qt::Orientation orientation, int role) const
 */
QVariant MarkModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		if (section == 0)
			return QStringLiteral("Mark");
		if (section == 1)
			return QStringLiteral("Desc");
	}
	return QVariant{};
}

/**
 * \brief Reimplemented from QAbstractItemModel::index(int row, int column, const QModelIndex & parent) const
 */
QModelIndex MarkModel::index(int row, int column, const QModelIndex & parent) const
{
	const auto parentItem = treeItem(parent);
	const auto childItem = parentItem->child(row);
	//Add QModelIndex to TreeItem * here
	return createIndex(row, column, childItem);
}

/**
 * \brief Reimplemented from QAbstractItemModel::parent(const QModelIndex & index) const
 */
QModelIndex MarkModel::parent(const QModelIndex & index) const
{
	//Index points to a root item
	const auto item = treeItem(index);

	if (index.isValid() == false || item == m_rootItem)
		return QModelIndex();

	const auto parentItem = item->parentItem();

	//If index points to a child item of root item
	if (parentItem == m_rootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

/**
 * \brief Reimplemented from QAbstractItemModel::rowCount(const QModelIndex & parent) const
 */
int MarkModel::rowCount(const QModelIndex & parent) const
{
	//Only a item with 0 column number has children
	if (parent.column() > 0)
		return 0;
	TreeItem * item = treeItem(parent);
	return item->childCount();
}

/**
 * \brief Reimplemented from QAbstractItemModel::columnCount(const QModelIndex & parent) const
 */
int MarkModel::columnCount(const QModelIndex & parent) const
{
	if (parent.isValid() == false) {
		return m_rootItem->columnCount();
	}
	const auto item = static_cast<TreeItem*>(parent.internalPointer());
	return item->columnCount();
}

