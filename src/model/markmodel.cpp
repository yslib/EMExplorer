#include "markmodel.h"
#include "widgets/sliceeditorwidget.h"
#include "globals.h"
#include "abstract/abstractslicedatamodel.h"

#include "markitem.h"

#include "model/roottreeitem.h"
#include "model/categorytreeitem.h"
#include "model/marktreeitem.h"

#include "algorithm/triangulate.h"

#include <QStyledItemDelegate>
#include <QScopedPointer>
#include <QAbstractItemView>
#include <QPainter>
#include <QDebug>



/**
 * \brief This is a helper function to get the internal data structure from \a index
 * 
 * 
 * \param index \a Either an invalid index or a valid index with 
 * internal pointer refer to root tree item gives the pointer refer to root item
 * \return Returns a non-null pointer anyway
 */
TreeItem* MarkModel::getItemHelper(const QModelIndex& index) const
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
 * \brief 
 * \param root 
 * \param display 
 * \return 
 * \deprecate
 */
//QModelIndex MarkModel::modelIndexHelper(const QModelIndex& root, const QString& display)const
//{
//	int c = rowCount(root);
//	for (int i = 0; i < c; i++)
//	{
//		QModelIndex id = index(i, 0, root);
//		auto item = static_cast<TreeItem*>(id.internalPointer());
//		auto d = item->data(0);
//		QString value;
//		switch (item->type())
//		{
//		case TreeItemType::Category:
//			Q_ASSERT_X(d.canConvert<__Internal_Categroy_Type_>(),
//				"category_index_helper_", "convert failure");
//			value = d.value<__Internal_Categroy_Type_>()->name();
//			break;
//		case TreeItemType::Mark:
//			value = d.value<__Internal_Mark_Type_>()->data(MarkProperty::CategoryName).toString();
//			break;
//		case TreeItemType::Root:
//		default:
//			break;
//		}
//		if (value == display)
//			return id;
//		else
//			modelIndexHelper(id, display);
//	}
//	return QModelIndex();
//}

/**
*	\brief 
*	\param category 
*	\return return a \a QModelIndex represents the \a category
*	
*	\internal 
*	\note This can be implement by a hash table, which is more efficient.
*/
QModelIndex MarkModel::categoryIndexHelper(const QString& category)const
{

	auto c = rowCount();	//children number of root. It's category				
	for (int i = 0; i < c; i++)
	{
		auto id = index(i, 0);
		const auto item = static_cast<TreeItem*>(id.internalPointer());

		if(item != nullptr && item->type() != TreeItemType::Category)
			continue;

		//auto d = item->data(0).value<__Internal_Categroy_Type_>();
		const auto var = item->data(0, Qt::DisplayRole);
		Q_ASSERT_X(var.canConvert<QString>(), "MarkModel::categoryIndexHelper", "convert failed");
		if (var == category)
		{
			return id;
		}
	}
	return QModelIndex();
}


/**
 * \brief 
 * \param category 
 * \param color 
 * \return 
 */
QModelIndex MarkModel::categoryAddHelper(const QString& category, const QColor & color)
{
	const auto c = rowCount();
	const auto success = insertRows(c, 1, QModelIndex());
	if (success == false)

		return QModelIndex();

	const auto newIndex = MarkModel::index(c, 0, QModelIndex());

	const auto p = new CategoryTreeItem(CategoryItem(category,color), newIndex,m_rootItem);

	MarkModel::setData(newIndex, QVariant::fromValue(static_cast<void*>(p)),TreeItemRole);

	//m_rootItem->appendChild(p);

	//endInsertRows();
	//setDirty();
	return createIndex(c, 0, p);
}

/**
 * \overload
 * 
 * \brief 
 * \param info 
 * \return 
 */

QModelIndex MarkModel::categoryAddHelper(const CategoryInfo& info) 
{
	return categoryAddHelper(info.name, info.color);
}
QModelIndex MarkModel::instanceAddHelper(const QString & category,const QGraphicsItem * item)
{
	auto cIndex = categoryIndexHelper(category);
	Q_ASSERT_X(cIndex.isValid(), "MarkModel::instanceAddHelper", "index is invalid");

	const auto nChild = rowCount(cIndex);
	const auto itemRect = item->boundingRect().toRect();

	QModelIndex best;
	double maxArea = 0;
	for(auto i = 0;i<nChild;i++) {
		const auto iIndex = index(i, 0, cIndex);
		const auto item = getItemHelper(iIndex);
		if(item->type() == TreeItemType::Instance) {
			const auto instanceItem = static_cast<InstanceTreeItem*>(item);
			const auto rect = instanceItem->boundingBox();

			const auto intersected = rect.intersected(itemRect);
			const auto area =  intersected.width()*intersected.height();
			if(maxArea < area) {
				best = iIndex;
				maxArea = area;
			}
		}
	}
	if(best.isValid() == false) {
		return QModelIndex();
	}
	return best;
}
/**
 * \brief 
 * \param mark 
 */
void MarkModel::addMarkInSliceHelper(QGraphicsItem * mark)
{
	int index = mark->data(MarkProperty::SliceIndex).toInt();
	MarkSliceList * markList;
	switch (static_cast<SliceType>(mark->data(MarkProperty::SliceType).toInt()))
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
void MarkModel::removeMarkInSliceHelper(QGraphicsItem * mark)
{
	int index = mark->data(MarkProperty::SliceIndex).toInt();
	auto type = static_cast<SliceType>(mark->data(MarkProperty::SliceType).value<int>());
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
void MarkModel::updateMarkVisibleHelper(QGraphicsItem * mark)
{
	//Q_ASSERT_X(m_view,"MarkModel::updateMarkVisible_Helper","null pointer");
	if (m_view == nullptr)
		return;
	int index = -1;
	//switch (static_cast<SliceType>(mark->data(MarkProperty::SliceType).toInt()))
	//{
	//case SliceType::Top:
	//	index = m_view->topSliceIndex();
	//		break;
	//case SliceType::Right:
	//	index = m_view->rightSliceIndex();
	//		break;
	//case SliceType::Front:
	//	index = m_view->frontSliceIndex();
	//		break;
	//}
	index = m_view->currentSliceIndex(static_cast<SliceType>(mark->data(MarkProperty::SliceType).toInt()));
	if (index != mark->data(MarkProperty::SliceIndex).toInt())
		return;
	const auto visible = mark->data(MarkProperty::VisibleState).toBool();
	mark->setVisible(visible);

	setDirty();
}

/**
 * \brief 
 * \param cate 
 * \return 
 */
bool MarkModel::updateMeshMarkHelper(const QString& cate)
{
	QList<StrokeMarkItem*> meshMark;
	auto oldMark = MarkModel::marks(cate);
	for (const auto om : oldMark) {
		if (om->type() == ItemTypes::StrokeMark) {					// Manually RTTI
			meshMark << static_cast<StrokeMarkItem*>(om);
		}
	}
	auto item = getItemHelper(categoryIndexHelper(cate));


	if (item->columnCount() <= 1)
	{
		bool success = item->insertColumns(item->columnCount(), 1);			//Insert one more column
		if (success == false)
			return false;
	}

	auto meshes = refactorMarks(meshMark);

	QVector<QSharedPointer<Triangulate>> tris;
	for(const auto m:meshes) {
		tris.push_back(QSharedPointer<Triangulate>(new Triangulate(m)));
		tris.back()->triangulate();
	}
	//auto tri = new Triangulate(meshMark);
	//if (tri == nullptr)
	//	return false;
	//bool success = tri->triangulate();

	item->setData(1,QVariant::fromValue(tris));
	return true;
}
void MarkModel::detachFromView()
{
	disconnect(this, &MarkModel::modified, m_view, &SliceEditorWidget::markModified);
	disconnect(this, &MarkModel::saved, m_view, &SliceEditorWidget::markSaved);
	m_view = nullptr;
	m_dataModel = nullptr;
}
void MarkModel::retrieveDataFromTreeItemHelper(TreeItem* root, TreeItemType type, int column, QVector<QVariant>& data, int role)
{
	if (root == nullptr)
		return;
	if (root->type() == type) {
		if(role == MetaDataRole) {
			data << QVariant::fromValue(root->metaData());
		}else {
			data << root->data(column, role);
		}
	}
	for (int i = 0; i < root->childCount(); i++) {
		retrieveDataFromTreeItemHelper(root->child(i), type, column, data, role);
	}
		
}

QList<QGraphicsItem*> MarkModel::marks() const
{
	QVector<QVariant> data;
	retrieveDataFromTreeItemHelper(m_rootItem, TreeItemType::Mark, 0, data,MetaDataRole);
	QList<QGraphicsItem*> items;
	foreach(const auto var, data)
	{
		Q_ASSERT_X(var.canConvert<void*>(),
			"MarkModel::marks", "convert failed");
		items << static_cast<QGraphicsItem*>(var.value<void*>());
	}
	return items;
}

QStringList MarkModel::categoryText() const
{
	QVector<QVariant> data;
	QStringList list;
	retrieveDataFromTreeItemHelper(m_rootItem, TreeItemType::Category, 0, data,Qt::DisplayRole);
	foreach(const auto & var, data) {
		Q_ASSERT_X(var.canConvert<QString>(), "MarkModel::categoryText", "convert falied");
		list << var.toString();
	}
		
	return list;
}

//QList<QSharedPointer<CategoryItem>> MarkModel::categoryItems() const
//{
//	QList<QSharedPointer<CategoryItem>> list;
//	QVector<QVariant> data;
//	retrieveDataFromTreeItemHelper(m_rootItem, TreeItemType::Category, 0, data,);
//	foreach(const auto & var,data)
//		list<<var.value<__Internal_Categroy_Type_>();
//	return list;
//}

//QSharedPointer<CategoryItem> MarkModel::categoryItem(const QString & cate) const
//{
//	const auto i = categoryIndexHelper(cate);
//	auto item = static_cast<TreeItem*>(i.internalPointer());
//	if(item == nullptr)
//		return QSharedPointer<CategoryItem>();
//	Q_ASSERT_X(item->data(0).canConvert<QSharedPointer<CategoryItem>>(), "MarkModel::categoryItem", "convert failed");
//	return item->data(0).value<QSharedPointer<CategoryItem>>();
//}

/**
 * \internal
 * \brief This function is for testing now, Because it will perform a 
 *				time-consuming update process every time when it is called.
 * \param cate 
 * \return 
 */
QVector<QSharedPointer<Triangulate>> MarkModel::markMesh(const QString& cate)
{
	updateMeshMarkHelper(cate);			// update every time
	//Maybe there should be a dirty bit to indicate whether the mesh should be updated. 
	const auto item = getItemHelper(categoryIndexHelper(cate));
	if (item == nullptr)
		return QVector<QSharedPointer<Triangulate>>();
	const auto & var = item->data(1);			//Mesh should be stored at column 1 of the category node
	if(var.canConvert<QVector<QSharedPointer<Triangulate>>>() == true) {
		//Mesh has been existed
		//Should be updated?
		return var.value<QVector<QSharedPointer<Triangulate>>>();
	}
	return QVector<QSharedPointer<Triangulate>>();
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
	std::sort(marks.begin(), marks.end(), [](const QGraphicsItem * it1,const QGraphicsItem * it2)->bool 
	{
		Q_ASSERT_X(it1->data(MarkProperty::SliceIndex).canConvert<int>(),"MarkModel::refactorMarks","it1 failed");
		Q_ASSERT_X(it2->data(MarkProperty::SliceIndex).canConvert<int>(),"MarkModel::refactorMarks","it2 failed");
		return it1->data(MarkProperty::SliceIndex).value<int>() < it2->data(MarkProperty::SliceIndex).value<int>();
	});

	/*
	 * After sorting by slice index. We need to add each mark item to corresponding mesh according to 
	 * the maximum intersected area between bounding box of the mark item and the newest representative 
	 * bounding box of the mesh.
	 */

	QVector<QList<StrokeMarkItem*>> meshes;
	QVector<QRectF> bounds;
	for(const auto item :marks) {
		auto meshIndex = -1;
		auto maxIntersectedArea = 0.0;
		const auto r = item->boundingRect();		// Rectangle of current mark
		for(auto i=0;i<bounds.size();i++) {
			if(bounds[i].intersects(r) == true) {
				const auto intersectedRect = bounds[i].intersected(r);
				const auto intersectedArea = intersectedRect.width()*intersectedRect.height();
				if(maxIntersectedArea < intersectedArea) {
					maxIntersectedArea = intersectedArea;
					meshIndex = i;
				}
			}
		}
		if(meshIndex != -1) {			// Add into a existed mesh
			bounds[meshIndex] = r;		// Update Rectangle
			meshes[meshIndex].push_back(item);
			
		}else {							// Create a new mesh
			bounds.push_back(r);
			meshes.push_back(QList<StrokeMarkItem*>{item});
		}
	}
	return meshes;
}

/**
 * \internal 
 * \brief This is a private constructor
 * 
 * A mark model can only be constructed by SliceEditorWidget
 * \param dataModel Slice data need to be marked
 * \param view 
 * \param root 
 * \param parent 
 * \sa SliceEditorWidget
 */
MarkModel::MarkModel(AbstractSliceDataModel* dataModel,
	SliceEditorWidget * view,
	//TreeItem * root,
	QObject * parent) :
	QAbstractItemModel(parent),
	m_rootItem(nullptr),					///TODO::This 
	m_dataModel(dataModel),
	m_view(view),
	m_dirty(false),
	m_identity(dataModel),
	m_selectionModel(new QItemSelectionModel(this, this))
{
	m_rootItem = new RootTreeItem(QModelIndex(),nullptr);
	m_rootItem->setPersistentModelIndex(createIndex(0, 0, m_rootItem));
	initSliceMarkContainerHelper();
}


/**
 * \internal 
 * \fn MarkModel::MarkModel(const QString & fileName)
 * \brief This is a private constructor
 * \param fileName Mark file name need to be opened
 */
MarkModel::MarkModel(const QString & fileName):
m_rootItem(nullptr),
m_dataModel(nullptr),
m_view(nullptr),
m_dirty(false),
m_selectionModel(new QItemSelectionModel(this, this))
{
	Q_ASSERT(false);

	//QFile file(fileName);
	//file.open(QIODevice::ReadOnly);
	//if (file.isOpen() == false)
	//	return;
	//QDataStream in(&file);
	//in.setVersion(QDataStream::Qt_5_9);
	//int magicNumber;
	//in >> magicNumber;
	//if (magicNumber != MagicNumber)
	//	return;
	//qRegisterMetaTypeStreamOperators<__Internal_Mark_Type_>("__Internal_Mark_Type_");
	//qRegisterMetaTypeStreamOperators<QGraphicsItem*>("CategoryItem*");
	//qRegisterMetaTypeStreamOperators<__Internal_Categroy_Type_>("QSharedPointer<CategoryItem>");
	//in >> m_identity;
	//in >> m_rootItem;
	////construct sliceMarks from the tree
	//initSliceMarkContainerHelper();

	//auto items = marks();
	//foreach(auto item,items)
	//{
	//	int value = item->data(MarkProperty::SliceType).value<int>();
	//	int index = item->data(MarkProperty::SliceIndex).value<int>();
	//	item->setFlags(QGraphicsItem::ItemIsSelectable);
	//	switch(static_cast<SliceType>(value))
	//	{
	//	case SliceType::Top:
	//		m_topSliceVisibleMarks[index].append(item);
	//		break;
	//	case SliceType::Right:
	//		m_rightSliceVisibleMarks[index].append(item);
	//		break;
	//	case SliceType::Front:
	//		m_frontSliceVisibleMarks[index].append(item);
	//		break;
	//	}
	//}
}
/**
 * \brief This is destructor of MarkModel
 * 
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
 * \brief Add marks \a marks by an existing \a category
 * \param text 
 * \param marks A QList contains a series of QGraphicsItem*
 * \return return true if adding marks is successful or return false
 */
bool MarkModel::addMarks(const QString & text, const QList<QGraphicsItem*> & marks)
{
	auto i = categoryIndexHelper(text);
	Q_ASSERT_X(i.isValid(), "MarkModel::addMarks", "index is invalid");

	//if (i.isValid() == false)
	//{
	//	return false;
	//}

	const auto r = rowCount(i);
	const auto c = marks.size();

	// Insert rows
	insertRows(r, c, i);

	// Get index of new inserted rows
	QVector<QModelIndex> newIndices;
	for(int k=0;k<c;k++) 
		newIndices << MarkModel::index(k + r, 0,i);

	// Set data
	for(int k=0;k < c;k++) {
		const auto p = new StrokeMarkTreeItem(marks[k], newIndices[k], nullptr);
		setData(newIndices[k], QVariant::fromValue(static_cast<void*>(p)), TreeItemRole);
	}



	return true;
}

/**
 * \brief Add a category
 * 
 * \param info Include all information needed by creating a category
 * \return return \a true if adding is successful or return \a false
 */
bool MarkModel::addCategory(const CategoryInfo & info)
{
	auto i = categoryIndexHelper(info.name);
	if (i.isValid() == false)
	{
		i = categoryAddHelper(info);
		setDirty();
		return true;
	}
	return false;
}

/**
 * \brief Retrieve marks by \a category
 * \param text 
 * \return return a QList contains the marks covered by \a category
 */
QList<QGraphicsItem*> MarkModel::marks(const QString & text)const
{
	auto id = categoryIndexHelper(text);
	int r = rowCount(id);
	auto item = getItemHelper(id);
	Q_ASSERT_X(item != m_rootItem,
		"MarkModel::addMark", "insert error");
	Q_ASSERT_X(item->type() == TreeItemType::Category,			//There should be TreeItemType::Category ???
		"MarkModel::marks", "type error");
	QList<QGraphicsItem*> res;
	for (int i = 0; i < r; i++)
	{
		Q_ASSERT_X(item->child(i)->type() == TreeItemType::Mark,
			"MarkModel::marks", "convert failed");
		const auto d = item->child(i)->metaData();
		res.append(static_cast<QGraphicsItem*>(d));
	}
	return res;
}

/**
 * \brief Remove a specified mark \a mark
 * \param mark A specified mark needs to be removed
 * \return return \a true if deleting is success or return \a false
 */
bool MarkModel::removeMark(QGraphicsItem * mark)
{
	const auto category = mark->data(MarkProperty::CategoryName).toString();
	auto id = categoryIndexHelper(category);
	int r = rowCount(id);
	auto item = getItemHelper(id);

	Q_ASSERT_X(item != m_rootItem,
		"MarkModel::removeMark", "insert error");
	Q_ASSERT_X(item->type() == TreeItemType::Category,
		"MarkModel::removeMark", "type error");

	int removedId = -1;
	for (int i = 0; i < r; i++)
	{
		Q_ASSERT_X(item->child(i)->type() == TreeItemType::Mark,
			"MarkModel::removeMark", "convert failure");
		auto d = item->child(i)->metaData();
		Q_ASSERT_X(d,
			"MarkModel::removeMarks", "null pointer");
		if (d == mark)
		{
			removedId = i;
			break;
		}
	}
	if (removedId == -1)
		return false;

	beginRemoveRows(id, removedId, removedId);
	item->removeChildren(removedId, 1);
	//remove from slice
	removeMarkInSliceHelper(mark);

	endRemoveRows();
	setDirty();
	return true;
}

/**
 * \brief 
 * \param marks 
 * \return return the numbers of deleted marks
 */
int MarkModel::removeMarks(const QList<QGraphicsItem*>& marks)
{
	int success = 0;
	auto func = std::bind(&MarkModel::removeMark, this,std::placeholders::_1);
	for (auto item : marks)
		if (func(item))
			success++;
	return success;
}


/**
 * \brief Save current marks contained in the mark model
 * \param fileName 
 * \param format 
 * \return return \a true if saving successes or return \a false
 */
bool MarkModel::save(const QString& fileName, MarkModel::MarkFormat format)
{
	if(format == MarkFormat::Binary)
	{
		//QFile file(fileName);
		//file.open(QIODevice::WriteOnly);
		//if (file.isOpen() == false)
		//	return false;
		//qRegisterMetaTypeStreamOperators<QGraphicsItem*>("QGraphicsItem*");
		//qRegisterMetaTypeStreamOperators<__Internal_Mark_Type_>("QSharedPointer<QGraphicsItem>");
		//qRegisterMetaTypeStreamOperators<__Internal_Categroy_Type_>("QSharedPointer<CategoryItem>");
		//QDataStream stream(&file);
		//stream.setVersion(QDataStream::Qt_5_9);
		//stream << static_cast<qint32>(MagicNumber);
		//stream << m_identity;
		//stream << m_rootItem;
		//resetDirty();
		//return true;
		return false;

	}else if(format == MarkFormat::Raw)
	{
		//resetDirt();
		QImage origin = m_dataModel->originalTopSlice(0);
		QImage slice(origin.size(), QImage::Format_Grayscale8);
		const int width = slice.width(), height = slice.height(), depth = m_dataModel->rightSlice(0).width();
		QScopedPointer<char> buffer(new char[width * height*depth ]);
		
		int sliceCount = 0;
		foreach(const auto & items, m_topSliceVisibleMarks) {
			//slice.fill(Qt::black);
			slice = m_dataModel->originalTopSlice(sliceCount);
			QPainter p(&slice);
			foreach(const auto & item, items) {
				auto mark = qgraphicsitem_cast<StrokeMarkItem*>(item);
				if (mark != nullptr) {
					const auto & poly = mark->polygon();
					auto pen = mark->pen();
					pen.setColor(Qt::black);
					p.setPen(pen);
					p.drawPolyline(poly);
				}
			}
			memcpy(buffer.data()+ width * height*sliceCount, slice.bits(), width*height);
			sliceCount++;
		}
		slice.fill(Qt::black);
		QFile out(fileName);
		out.open(QIODevice::WriteOnly);
		if (out.isOpen() == false)
			return false;
		out.write((const char *)buffer.data(), width*height*depth);
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
	const auto item = getItemHelper(index);
	Q_ASSERT_X(item, "MarkModel::data", "null pointer");
	return item->data(index.column(), role);
}

/**
 * \brief 
 * \param index 
 * \return 
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
 * 
 * \param index
 * \param value 
 * \param role 
 * 
 * \return Returns \a true if this call is successful otherwise return \a false.
 * 
 * \note If \a role is Qt::CheckStateRole, the function will be called recursively to apply the same setting
 * for its children
 * 
 * \warning When \a role is TreeItemRole, the old \a TreeItem* pointer would be deleted at once.
 * \sa TreeItem, Qt::ItemDataRole, MarkModelItemRole
 */
bool MarkModel::setData(const QModelIndex & index, const QVariant & value, int role)
{

	if(role == TreeItemRole) {			
										//Insert specially. column of the index is ignored.

		const auto r = index.row();
		const auto parentModelIndex = parent(index);
		const auto item = getItemHelper(parentModelIndex);

		const auto newItem = static_cast<TreeItem*>(value.value<void*>());
		delete item->takeChild(index.row(), newItem, nullptr);

		// Update the internal pointer refer to exact data
		const auto newIndex = createIndex(index.row(), index.column(), newItem);
		qDebug() << "row:" << newIndex.row() << " " << newIndex.column() << " " << newItem;
		emit dataChanged(newIndex,newIndex, QVector<int>{role});
		return true;

	} else {							//Insert normally.
										
		const auto item = getItemHelper(index);
		Q_ASSERT_X(item, "MarkModel::data", "null pointer");
		if (item == nullptr) return false;
		item->setData(index.column(), value, role);

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

bool MarkModel::insertColumns(int column, int count, const QModelIndex & parent)
{
	const auto item = getItemHelper(parent);
	if (item == nullptr)
		return false;
	beginInsertColumns(parent, column, column + count - 1);
	const auto success =item->insertColumns(column, count);
	endInsertColumns();
	return success;
}

bool MarkModel::removeColumns(int column, int count, const QModelIndex & parent)
{
	const auto item = getItemHelper(parent);
	if (item == nullptr)
		return false;
	beginRemoveColumns(parent, column, column + count - 1);
	const auto success = item->insertColumns(column, count);
	endRemoveColumns();
	return success;
}

/**
 * \brief 
 * \param row 
 * \param count 
 * \param parent 
 * \return 
 */
bool MarkModel::insertRows(int row, int count, const QModelIndex & parent)
{
	const auto item = getItemHelper(parent);
	beginInsertRows(parent, row, count + row - 1);

	QVector<TreeItem*> children;
	for(auto i = 0;i<count;i++) 
	{
		children << new EmptyTreeItem(QPersistentModelIndex(QModelIndex()), item);
	}
	const auto success = item->insertChildren(row,children);
	endInsertRows();
	setDirty();
	return success;
}

bool MarkModel::removeRows(int row, int count, const QModelIndex & parent)
{
	const auto item = getItemHelper(parent);
	beginRemoveRows(parent, row, row + count - 1);
	const auto success = item->removeChildren(row, count);
	endRemoveRows();
	return success;
}

QVariant MarkModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		if (section == 0)
			return QStringLiteral("Mark");
		if (section == 1)
			return QStringLiteral("Desc");
	}
	return QVariant();
}

QModelIndex MarkModel::index(int row, int column, const QModelIndex & parent) const
{
	const auto parentItem = getItemHelper(parent);
	const auto childItem = parentItem->child(row);
	//Add QModelIndex to TreeItem * here
	return createIndex(row, column, childItem);
}

QModelIndex MarkModel::parent(const QModelIndex & index) const
{
	//Index points to a root item
	TreeItem * item = getItemHelper(index);

	if (index.isValid() == false || item == m_rootItem)return QModelIndex();


	TreeItem * parentItem = item->parentItem();

	//If index points to a child item of root item
	if (parentItem == m_rootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int MarkModel::rowCount(const QModelIndex & parent) const
{
	//Only a item with 0 column number has children
	if (parent.column() > 0)
		return 0;
	TreeItem * item = getItemHelper(parent);
	return item->childCount();
}

int MarkModel::columnCount(const QModelIndex & parent) const
{
	if (parent.isValid() == false) {
		return m_rootItem->columnCount();
	}
	const auto item = static_cast<TreeItem*>(parent.internalPointer());
	return item->columnCount();
}

QDataStream & operator<<(QDataStream & stream, const CategoryItem & item)
{
	stream << item.m_info.name << item.m_info.color << item.m_count << item.m_visible;
	return stream;
}

QDataStream & operator>>(QDataStream & stream, CategoryItem & item)
{

	stream >> item.m_info.name >> item.m_info.color >> item.m_count >> item.m_visible;
	Q_ASSERT_X(stream.status() != QDataStream::ReadPastEnd,
		"Category::operator<<", "corrupt data");
	return stream;
}


QDataStream& operator<<(QDataStream& stream, const QSharedPointer<CategoryItem>& item)
{
	stream << item->m_info.name << item->m_info.color << item->m_count << item->m_visible;
	return stream;
}

QDataStream & operator>>(QDataStream & stream, QSharedPointer<CategoryItem>& item)
{
	item.reset(new CategoryItem());
	stream >> item->m_info.name >> item->m_info.color >> item->m_count >> item->m_visible;
	Q_ASSERT_X(stream.status() != QDataStream::ReadPastEnd,
		"Category::operator<<", "corrupt data");
	return stream;
}

