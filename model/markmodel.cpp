#include "markmodel.h"
#include "widgets/sliceeditorwidget.h"
#include "globals.h"
#include "abstract/abstractslicedatamodel.h"
#include "markitem.h"
#include "model/treeitem.h"
#include <QStyledItemDelegate>
#include <QScopedPointer>
#include <QPainter>
#include "categoryitem.h"
#include "algorithm/triangulate.h"

/*
*New data Model
*/


TreeItem* MarkModel::getItemHelper(const QModelIndex& index) const
{
	if (index.isValid())
	{
		TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
		if (item)return item;
	}
	return m_rootItem;
}
QModelIndex MarkModel::modelIndexHelper(const QModelIndex& root, const QString& display)const
{
	int c = rowCount(root);
	for (int i = 0; i < c; i++)
	{
		QModelIndex id = index(i, 0, root);
		auto item = static_cast<TreeItem*>(id.internalPointer());
		auto d = item->data(0);
		QString value;
		switch (item->type())
		{
		case TreeItemType::Category:
			Q_ASSERT_X(d.canConvert<__Internal_Categroy_Type_>(),
				"category_index_helper_", "convert failure");
			value = d.value<__Internal_Categroy_Type_>()->name();
			break;
		case TreeItemType::Mark:
			value = d.value<__Internal_Mark_Type_>()->data(MarkProperty::CategoryName).toString();
			break;
		case TreeItemType::Root:
		default:
			break;
		}
		if (value == display)
			return id;
		else
			modelIndexHelper(id, display);
	}
	return QModelIndex();
}
QModelIndex MarkModel::categoryIndexHelper(const QString& category)const
{
	/*
	 * This can be implement by a hash table, which is more efficient.
	 */

	int c = rowCount();	//children number of root. It's category				
	for (int i = 0; i < c; i++)
	{
		auto id = index(i, 0);
		auto item = static_cast<TreeItem*>(id.internalPointer());
		Q_ASSERT_X(item->data(0).canConvert<__Internal_Categroy_Type_>(),
			"category_index_helper_", "convert failure");
		auto d = item->data(0).value<__Internal_Categroy_Type_>();
		if (d->name() == category)
		{
			return id;
		}
	}
	return QModelIndex();
}
QModelIndex MarkModel::categoryAddHelper(const QString& category)
{
	int c = rowCount();
	beginInsertRows(QModelIndex(), c, c);
	QVector<QVariant> d{ QVariant::fromValue(__Internal_Categroy_Type_{new CategoryItem(category)}) };
	auto p = new TreeItem(d, TreeItemType::Category, m_rootItem);
	m_rootItem->appendChild(p);
	endInsertRows();
	setDirty();
	return createIndex(c, 0, p);
}

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
void MarkModel::updateMarkVisibleHelper(MarkModel::__Internal_Mark_Type_& mark)
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
	bool visible = mark->data(MarkProperty::VisibleState).toBool();
	mark->setVisible(visible);
	setDirty();
}
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

	auto tri = new Triangulate(meshMark);
	if (tri == nullptr)
		return false;

	bool success = tri->triangulate();
	if (item->columnCount() <= 1)
	{
		bool success = item->insertColumns(item->columnCount(), 1);			//Insert one more column
		if (success == false)
			return false;
	}

	item->setData(1, QVariant::fromValue(QSharedPointer<Triangulate>(tri)));
	return success;
}
void MarkModel::detachFromView()
{
	disconnect(this, &MarkModel::modified, m_view, &SliceEditorWidget::markModified);
	disconnect(this, &MarkModel::saved, m_view, &SliceEditorWidget::markSaved);
	m_view = nullptr;
	m_dataModel = nullptr;
}
void MarkModel::retrieveDataFromTreeItemHelper(const TreeItem * root, TreeItemType type, int column, QVector<QVariant>& data)
{
	if (root == nullptr)
		return;
	if (root->type() == type)
		data << root->data(column);
	for (int i = 0; i < root->childCount(); i++)
		retrieveDataFromTreeItemHelper(root->child(i), type, column, data);
}

QList<QGraphicsItem*> MarkModel::marks() const
{
	QVector<QVariant> data;
	retrieveDataFromTreeItemHelper(m_rootItem, TreeItemType::Mark, 0, data);
	QList<QGraphicsItem*> items;
	foreach(const auto & var, data)
	{
		Q_ASSERT_X(var.canConvert<__Internal_Mark_Type_>(), 
			"MarkModel::marks", "convert failed");
		items << var.value<__Internal_Mark_Type_>().data();
	}
	return items;
}

QStringList MarkModel::categoryText() const
{
	QVector<QVariant> data;
	QStringList list;
	retrieveDataFromTreeItemHelper(m_rootItem, TreeItemType::Category, 0, data);
	foreach(const auto & var, data)
		list << var.value<__Internal_Categroy_Type_>()->name();
	return list;
}

QList<QSharedPointer<CategoryItem>> MarkModel::categoryItems() const
{
	QList<QSharedPointer<CategoryItem>> list;
	QVector<QVariant> data;
	retrieveDataFromTreeItemHelper(m_rootItem, TreeItemType::Category, 0, data);
	foreach(const auto & var,data)
		list<<var.value<__Internal_Categroy_Type_>();
	return list;
}

QSharedPointer<CategoryItem> MarkModel::categoryItem(const QString & cate) const
{
	const auto i = categoryIndexHelper(cate);
	auto item = static_cast<TreeItem*>(i.internalPointer());
	if(item == nullptr)
		return QSharedPointer<CategoryItem>();
	Q_ASSERT_X(item->data(0).canConvert<QSharedPointer<CategoryItem>>(), "MarkModel::categoryItem", "convert failed");
	return item->data(0).value<QSharedPointer<CategoryItem>>();
}


/*
 * 
 */
/**
 * \brief Note: This function is for testing now, Because it will perform a 
 *				time-consuming update process every time when it is called.
 * \param cate 
 * \return 
 */
const Triangulate * MarkModel::markMesh(const QString & cate)
{
	updateMeshMarkHelper(cate);			// update every time
	//Maybe there should be a dirty bit to indicate whether the mesh should be updated. 
	const auto item = getItemHelper(categoryIndexHelper(cate));
	if (item == nullptr)
		return nullptr;
	const auto & var = item->data(1);			//Mesh should be stored at column 1 of the category node
	if(var.canConvert<QSharedPointer<Triangulate>>() == true) {
		//Mesh has been existed
		//Should be updated?
		return var.value<QSharedPointer<Triangulate>>().data();
	}
	return nullptr;
}

void MarkModel::initSliceMarkContainerHelper()
{
	Q_ASSERT_X(m_identity.isValid() == true, 
		"MarkModel::initSliceMarkContainer_Helper", "IdentityTester is not valid");
	m_topSliceVisibleMarks.resize(m_identity.topSliceCount());
	m_rightSliceVisibleMarks.resize(m_identity.rightSliceCount());
	m_frontSliceVisibleMarks.resize(m_identity.frontSliceCount());
}

MarkModel::MarkModel(AbstractSliceDataModel* dataModel,
	SliceEditorWidget * view,
	TreeItem * root,
	QObject * parent) :
	QAbstractItemModel(parent),
	m_rootItem(root),
	m_dataModel(dataModel),
	m_view(view),
	m_dirty(false),
	m_identity(dataModel)
{
	initSliceMarkContainerHelper();
}

MarkModel::MarkModel(const QString & fileName):
m_rootItem(nullptr),
m_dataModel(nullptr),
m_view(nullptr),
m_dirty(false)
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
	qRegisterMetaTypeStreamOperators<__Internal_Mark_Type_>("__Internal_Mark_Type_");
	qRegisterMetaTypeStreamOperators<QGraphicsItem*>("CategoryItem*");
	qRegisterMetaTypeStreamOperators<__Internal_Categroy_Type_>("QSharedPointer<CategoryItem>");
	in >> m_identity;
	in >> m_rootItem;
	//construct sliceMarks from the tree
	initSliceMarkContainerHelper();

	auto items = marks();
	foreach(auto item,items)
	{
		int value = item->data(MarkProperty::SliceType).value<int>();
		int index = item->data(MarkProperty::SliceIndex).value<int>();
		item->setFlags(QGraphicsItem::ItemIsSelectable);
		switch(static_cast<SliceType>(value))
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
MarkModel::~MarkModel()
{
	if (m_rootItem != nullptr)
	{
		delete m_rootItem;
		m_rootItem = nullptr;
	}
}


void MarkModel::addMarks(const QString & category, const QList<QGraphicsItem*>& marks)
{
	auto i = categoryIndexHelper(category);
	if (i.isValid() == false)
	{
		i = categoryAddHelper(category);
	}
	int r = rowCount(i);
	int c = marks.size();
	beginInsertRows(i, r, r + c - 1);
	auto item = getItemHelper(i);
	Q_ASSERT_X(item != m_rootItem,
		"MarkModel::addMark", "insert error");


	int n = 0;
	QList<TreeItem*> list;
	for (auto m : marks)
	{


		QVector<QVariant> d;
		m->setData(MarkProperty::Name,category + QString("#%1").arg(r + n++));
		addMarkInSliceHelper(m);
		d.append(QVariant::fromValue(__Internal_Mark_Type_(m)));	
		list.append(new TreeItem(d, TreeItemType::Mark, nullptr));
	}
	item->insertChildren(r, list);		//insert marks at the end

	endInsertRows();
	setDirty();

}

QList<QGraphicsItem*> MarkModel::marks(const QString & category)const
{
	auto id = categoryIndexHelper(category);
	int r = rowCount(id);
	auto item = getItemHelper(id);
	Q_ASSERT_X(item != m_rootItem,
		"MarkModel::addMark", "insert error");
	Q_ASSERT_X(item->type() == TreeItemType::Category,			//There should be TreeItemType::Category ???
		"MarkModel::marks", "type error");
	QList<QGraphicsItem*> res;
	for (int i = 0; i < r; i++)
	{
		Q_ASSERT_X(item->child(i)->data(0).canConvert<__Internal_Mark_Type_>(),
			"MarkModel::marks", "convert failed");
		auto d = item->child(i)->data(0).value<__Internal_Mark_Type_>();
		res.append(d.data());
	}
	return res;
}

bool MarkModel::removeMark(QGraphicsItem* mark)
{
	QString category = mark->data(MarkProperty::CategoryName).toString();
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
		Q_ASSERT_X(item->child(i)->data(0).canConvert<__Internal_Mark_Type_>(),
			"MarkModel::removeMark", "convert failure");
		auto d = item->child(i)->data(0).value<__Internal_Mark_Type_>();
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

int MarkModel::removeMarks(const QList<QGraphicsItem*>& marks)
{
	int success = 0;
	auto func = std::bind(&MarkModel::removeMark, this,std::placeholders::_1);
	for (auto item : marks)
		if (func(item))
			success++;
	return success;
}
bool MarkModel::save(const QString& fileName, MarkModel::MarkFormat format)
{
	if(format == MarkFormat::Binary)
	{
		QFile file(fileName);
		file.open(QIODevice::WriteOnly);
		if (file.isOpen() == false)
			return false;
		qRegisterMetaTypeStreamOperators<QGraphicsItem*>("QGraphicsItem*");
		qRegisterMetaTypeStreamOperators<__Internal_Mark_Type_>("QSharedPointer<QGraphicsItem>");
		qRegisterMetaTypeStreamOperators<__Internal_Categroy_Type_>("QSharedPointer<CategoryItem>");
		QDataStream stream(&file);
		stream.setVersion(QDataStream::Qt_5_9);
		stream << static_cast<qint32>(MagicNumber);
		stream << m_identity;
		stream << m_rootItem;
		resetDirty();
		return true;

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

QVariant MarkModel::data(const QModelIndex & index, int role) const
{
	if (index.isValid() == false)
		return QVariant();
	if (role == Qt::DisplayRole||role == Qt::ToolTipRole)
	{
		TreeItem * item = static_cast<TreeItem*>(index.internalPointer());
		QVariant d = item->data(index.column());
		switch (item->type())
		{
		case TreeItemType::Root:
			return QVariant();
		case TreeItemType::Category:
			if (index.column() == 0)
			{
				Q_ASSERT_X(d.canConvert<__Internal_Categroy_Type_>(),
					"MarkModel::data", "convert failure");
				return d.value<__Internal_Categroy_Type_>()->name();
			}else if(index.column() == 1)
			{
				//display total count

			}
			return QVariant();
		case TreeItemType::Mark:

			if(index.column() == 0)
			{
				Q_ASSERT_X(d.canConvert<__Internal_Mark_Type_>(),
					"MarkModel::data", "convert failure");
				auto mark = d.value<__Internal_Mark_Type_>();

				return QVariant::fromValue(mark->data(MarkProperty::Name).toString());
			}
			else if(index.column() == 1)
			{
				//display slice index
				auto d = item->data(0);
				auto mark = d.value<__Internal_Mark_Type_>();
				QString sliceType;
				switch(static_cast<SliceType>(mark->data(MarkProperty::SliceType).toInt()))
				{
				case SliceType::Top:
					sliceType = QStringLiteral("Top:");
					break;
				case SliceType::Right:
					sliceType = QStringLiteral("Right:");
					break;
				case SliceType::Front:
					sliceType = QStringLiteral("Front:");
					break;
				}
				//get slicetype and index
				return sliceType+QString::number(mark->data(MarkProperty::SliceIndex).toInt());
			}
			return QVariant();
		}
	}
	if (role == Qt::CheckStateRole&&index.column() == 0)			//So far, we only consider the first column
	{
		TreeItem * item = static_cast<TreeItem*>(index.internalPointer());
		QVariant d = item->data(index.column());
		switch (item->type())
		{
		case TreeItemType::Root:
			return QVariant();
		case TreeItemType::Category:
			Q_ASSERT_X(d.canConvert<__Internal_Categroy_Type_>(),
				"MarkModel::data", "convert failure");
			return d.value<__Internal_Categroy_Type_>()->visible() ? Qt::Checked : Qt::Unchecked;
		case TreeItemType::Mark:
			Q_ASSERT_X(d.canConvert<__Internal_Mark_Type_>(),
				"MarkModel::data", "convert failure");
			auto mark = d.value<__Internal_Mark_Type_>();
			//get visible stats
			return mark->data(MarkProperty::VisibleState).toBool() ? Qt::Checked : Qt::Unchecked;
		}
	}
	if(role == Qt::DecorationRole && index.column() == 0)
	{
		TreeItem * item = static_cast<TreeItem*>(index.internalPointer());
		QVariant d = item->data(index.column());
		switch (item->type())
		{
		case TreeItemType::Root:
			return QVariant();
		case TreeItemType::Category:
			Q_ASSERT_X(d.canConvert<__Internal_Categroy_Type_>(),
				"MarkModel::data", "convert failure");
			return d.value<__Internal_Categroy_Type_>()->color();
		case TreeItemType::Mark:
			Q_ASSERT_X(d.canConvert<__Internal_Mark_Type_>(),
				"MarkModel::data", "convert failure");
			auto mark = d.value<__Internal_Mark_Type_>();
			//get color
			return mark->data(MarkProperty::Color);
		}
	}
	return QVariant();
}

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

bool MarkModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if (role == Qt::CheckStateRole && index.column() == 0)
	{
		const auto item = static_cast<TreeItem*>(index.internalPointer());
		QVariant d = item->data(index.column());
		switch (item->type())
		{
		case TreeItemType::Root:
			return false;
		case TreeItemType::Category:
		{
			Q_ASSERT_X(d.canConvert<__Internal_Categroy_Type_>(),
				"MarkModel::setData", "convert failure");
			d.value<__Internal_Categroy_Type_>()->setVisible(value == Qt::Checked);
			emit dataChanged(index, index, QVector<int>{Qt::CheckStateRole});
			//update child state recursively
			int c = rowCount(index);
			for (int i = 0; i < c; i++)
				setData(MarkModel::index(i, 0, index), value, Qt::CheckStateRole);
			return true;
		}
		case TreeItemType::Mark:
			Q_ASSERT_X(d.canConvert<__Internal_Mark_Type_>(),
				"MarkModel::setData", "convert failure");
			auto mark = d.value<__Internal_Mark_Type_>();
			const auto vis = (value == Qt::Checked);
			mark->setData(MarkProperty::VisibleState, vis);
			updateMarkVisibleHelper(mark);		//set dirty
			emit dataChanged(index, index, QVector<int>{Qt::CheckStateRole});
			return true;
		}
	}
	return false;
}

bool MarkModel::insertColumns(int column, int count, const QModelIndex & parent)
{
	beginInsertColumns(parent, column, column + count - 1);
	//insert same columns at same position from the top of the tree to down recursively
	bool success = m_rootItem->insertColumns(column, count);
	endInsertColumns();
	return success;
}

bool MarkModel::removeColumns(int column, int count, const QModelIndex & parent)
{
	beginRemoveColumns(parent, column, column + count - 1);
	bool success = m_rootItem->removeColumns(column, count);
	endRemoveColumns();

	if (m_rootItem->columnCount() == 0)
		removeRows(0, rowCount());
	return success;
}

bool MarkModel::insertRows(int row, int count, const QModelIndex & parent)
{
	TreeItem * item = getItemHelper(parent);
	beginInsertRows(parent, row, count + row - 1);
	//the number of inserted column is the same as the root, i.e 2
	TreeItemType type;
	switch (item->type())
	{
	case TreeItemType::Root:
		type = TreeItemType::Category;
		break;
	case TreeItemType::Category:
		type = TreeItemType::Mark;
		break;
	case TreeItemType::Mark:
		return false;
	default:
		return false;
	}
	bool success = item->insertChildren(row, count, columnCount(), type);
	endInsertRows();
	return success;
}

bool MarkModel::removeRows(int row, int count, const QModelIndex & parent)
{
	TreeItem * item = getItemHelper(parent);
	bool success = true;
	beginRemoveRows(parent, row, row + count - 1);
	success = item->removeChildren(row, count);
	endRemoveRows();
	return success;
}

QVariant MarkModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return m_rootItem->data(section);
	return QVariant();
}

QModelIndex MarkModel::index(int row, int column, const QModelIndex & parent) const
{
	if (parent.isValid() == true && parent.column() != 0)
		return QModelIndex();
	TreeItem * parentItem = getItemHelper(parent);
	TreeItem * childItem = parentItem->child(row);
	if (childItem == nullptr)
		return QModelIndex();
	else
		return createIndex(row, column, childItem);
}

QModelIndex MarkModel::parent(const QModelIndex & index) const
{
	//Index points to a root item
	if (index.isValid() == false)return QModelIndex();

	TreeItem * item = getItemHelper(index);
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
	if (parent.isValid() == false)
		return m_rootItem->columnCount();
	return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
}

QDataStream & operator<<(QDataStream & stream, const CategoryItem & item)
{
	stream << item.m_name << item.m_color << item.m_count << item.m_visible;
	return stream;
}

QDataStream & operator>>(QDataStream & stream, CategoryItem & item)
{

	stream >> item.m_name >> item.m_color >> item.m_count >> item.m_visible;
	Q_ASSERT_X(stream.status() != QDataStream::ReadPastEnd,
		"Category::operator<<", "corrupt data");
	return stream;
}


QDataStream& operator<<(QDataStream& stream, const QSharedPointer<CategoryItem>& item)
{
	stream << item->m_name << item->m_color << item->m_count << item->m_visible;
	return stream;
}

QDataStream & operator>>(QDataStream & stream, QSharedPointer<CategoryItem>& item)
{
	item.reset(new CategoryItem());
	stream >> item->m_name >> item->m_color >> item->m_count >> item->m_visible;
	Q_ASSERT_X(stream.status() != QDataStream::ReadPastEnd,
		"Category::operator<<", "corrupt data");
	return stream;
}

