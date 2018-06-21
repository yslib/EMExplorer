#include "markmodel.h"
#include "imageviewer.h"
#include "globals.h"
#include "abstractslicedatamodel.h"
#include "markitem.h"
#include "treeitem.h"

/*
*New data Model
*/


TreeItem* MarkModel::getItem_Helper(const QModelIndex& index) const
{
	if (index.isValid())
	{
		TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
		if (item)return item;
	}
	return m_rootItem;
}
QModelIndex MarkModel::modelIndex_Helper(const QModelIndex& root, const QString& display)const
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
			Q_ASSERT_X(d.canConvert<QSharedPointer<CategoryItem>>(),
				"category_index_helper_", "convert failure");
			value = d.value<QSharedPointer<CategoryItem>>()->name();
			break;
		case TreeItemType::Mark:
			value = d.value<QGraphicsItem*>()->data(MarkProperty::CategoryName).toString();
			break;
		case TreeItemType::Root:
		default:
			break;
		}
		if (value == display)
			return id;
		else
			modelIndex_Helper(id, display);
	}
	return QModelIndex();
}
QModelIndex MarkModel::categoryIndex_Helper(const QString& category)const
{
	int c = rowCount();
	for (int i = 0; i < c; i++)
	{
		auto id = index(i, 0);
		auto item = static_cast<TreeItem*>(id.internalPointer());
		Q_ASSERT_X(item->data(0).canConvert<QSharedPointer<CategoryItem>>(),
			"category_index_helper_", "convert failure");
		auto d = item->data(0).value<QSharedPointer<CategoryItem>>();
		if (d->name() == category)
		{
			return id;
		}
	}
	return QModelIndex();
}
QModelIndex MarkModel::categoryAdd_Helper(const QString& category)
{
	int c = rowCount();
	beginInsertRows(QModelIndex(), c, c);
	QVector<QVariant> d{ QVariant::fromValue(QSharedPointer<CategoryItem>{new CategoryItem(category)}) };
	auto p = new TreeItem(d, TreeItemType::Category, m_rootItem);
	m_rootItem->appendChild(p);
	endInsertRows();
	return createIndex(c, 0, p);
}

bool MarkModel::checkMatch_Helper(const AbstractSliceDataModel * dataModel)
{
	Q_ASSERT_X(m_dataModel, "MarkModel::check_match_helper_", "null pointer");
	return ((m_dataModel->frontSliceCount() == dataModel->frontSliceCount()) &&
		(m_dataModel->topSliceCount() == dataModel->topSliceCount()) &&
		(m_dataModel->rightSliceCount() == dataModel->rightSliceCount()));
	//Image size need to be considered later
}

void MarkModel::addMarkInSlice_Helper(QGraphicsItem * mark)
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
}
void MarkModel::updateMarkVisible_Helper(QGraphicsItem * mark)
{
	Q_ASSERT_X(m_view,"MarkModel::updateMarkVisible_Helper","null pointer");
	int index = -1;
	switch (static_cast<SliceType>(mark->data(MarkProperty::SliceType).toInt()))
	{
	case SliceType::Top:
		index = m_view->topSliceIndex();
			break;
	case SliceType::Right:
		index = m_view->rightSliceIndex();
			break;
	case SliceType::Front:
		index = m_view->frontSliceIndex();
			break;
	}
	if (index != mark->data(MarkProperty::SliceIndex).toInt())
		return;
	//auto item = QueryMarkItemInterface<QGraphicsItem*, PolyMarkItem*>(mark);
	bool visible = mark->data(MarkProperty::VisibleState).toBool();
	mark->setVisible(visible);
}
void MarkModel::encode_Helper(TreeItem * root, QDataStream & stream)
{
	Q_UNUSED(root);
	Q_UNUSED(stream);
}
TreeItem * MarkModel::decode_Helper(QDataStream & stream)
{
	Q_UNUSED(stream);
	return nullptr;
}

MarkModel::MarkModel(AbstractSliceDataModel* dataModel,
	ImageView * view,
	TreeItem * root,
	MarkSliceList top,
	MarkSliceList right,
	MarkSliceList front,
	QObject * parent) :
	QAbstractItemModel(parent),
	m_rootItem(root),
	m_dataModel(dataModel),
	m_view(view),
m_dirty(false),
	m_topSliceVisibleMarks(std::move(top)),
	m_rightSliceVisibleMarks(std::move(right)),
	m_frontSliceVisibleMarks(std::move(front))
{
}
bool MarkModel::open(const QString & fileName)
{

	return false;
}
MarkModel::MarkModel(const QString & fileName)
{
	///TODO::
}
MarkModel::~MarkModel()
{
	if (m_rootItem != nullptr)
	{
		delete m_rootItem;
		m_rootItem = nullptr;
	}
}

void MarkModel::addMark(const QString & category, QGraphicsItem * mark)
{
	addMarks(category, QList<QGraphicsItem*>{mark});
}
void MarkModel::addMarks(const QString & category, const QList<QGraphicsItem*>& marks)
{
	auto i = categoryIndex_Helper(category);
	if (i.isValid() == false)
	{
		i = categoryAdd_Helper(category);
	}
	int r = rowCount(i);
	int c = marks.size();
	beginInsertRows(i, r, r + c - 1);
	auto item = getItem_Helper(i);

	Q_ASSERT_X(item != m_rootItem,
		"MarkModel::addMark", "insert error");
	int n = 0;
	QList<TreeItem*> list;
	for (auto m : marks)
	{
		QVector<QVariant> d;
		m->setData(MarkProperty::Name,category + QString("#%1").arg(r + n++));
		addMarkInSlice_Helper(m);
		d.append(QVariant::fromValue(m));	
		list.append(new TreeItem(d, TreeItemType::Mark, nullptr));
	}
	item->insertChildren(r, list);		//insert marks at the end
	endInsertRows();
	setDirty();
}

QList<QGraphicsItem*> MarkModel::marks(const QString & category)
{
	auto id = categoryIndex_Helper(category);
	int r = rowCount(id);
	auto item = getItem_Helper(id);
	Q_ASSERT_X(item != m_rootItem,
		"MarkModel::addMark", "insert error");
	Q_ASSERT_X(item->type() == TreeItemType::Mark,
		"MarkModel::marks", "type error");
	QList<QGraphicsItem*> res;
	for (int i = 0; i < r; i++)
	{
		auto d = item->child(i)->data(0).value<QGraphicsItem*>();
		Q_ASSERT_X(d,
			"MarkModel::marks", "null pointer");
		res.append(d);
	}
	return res;
}

bool MarkModel::removeMark(const QString& category, QGraphicsItem* mark)
{
	auto id = categoryIndex_Helper(category);
	int r = rowCount(id);
	auto item = getItem_Helper(id);
	Q_ASSERT_X(item != m_rootItem,
		"MarkModel::removeMark", "insert error");
	Q_ASSERT_X(item->type() == TreeItemType::Mark,
		"MarkModel::removeMark", "type error");
	Q_ASSERT_X(item->child(r)->data(0).canConvert<QGraphicsItem*>(),
		"MarkModel::removeMark", "convert failure");
	int removedId = -1;
	for (int i = 0; i < r; i++)
	{
		auto d = item->child(r)->data(0).value<QGraphicsItem*>();
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
	endRemoveRows();
	setDirty();
	return true;
}

int MarkModel::removeMarks(const QString& category, const QList<QGraphicsItem*>& marks)
{
	int success = 0;
	auto func = std::bind(&MarkModel::removeMark, this,
		category, std::placeholders::_1);
	for (auto item : marks)
		if (func(item))
			success++;
	return success;
}

int MarkModel::markCount(const QString & category)
{
	auto i = categoryIndex_Helper(category);
	return rowCount(i);
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
		//qRegisterMetaTypeStreamOperators<CategoryItem>("CategoryItem");
		qRegisterMetaTypeStreamOperators<QSharedPointer<CategoryItem>>("QSharedPointer<CategoryItem>");
		QDataStream stream(&file);
		stream.setVersion(QDataStream::Qt_5_10);
		stream << static_cast<qint32>(MagicNumber);
		stream << m_topSliceVisibleMarks;
		stream << m_rightSliceVisibleMarks;
		stream << m_frontSliceVisibleMarks;
		stream << m_rootItem;

		resetDirty();
		return true;

	}else if(format == MarkFormat::Raw)
	{
		return false;
	}
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
				Q_ASSERT_X(d.canConvert<QSharedPointer<CategoryItem>>(),
					"MarkModel::data", "convert failure");
				return d.value<QSharedPointer<CategoryItem>>()->name();
			}else if(index.column() == 1)
			{
				//display total count

			}
			return QVariant();
		case TreeItemType::Mark:

			if(index.column() == 0)
			{
				Q_ASSERT_X(d.canConvert<QGraphicsItem*>(),
					"MarkModel::data", "convert failure");
				auto mark = d.value<QGraphicsItem*>();

				return QVariant::fromValue(mark->data(MarkProperty::Name).toString());
			}
			else if(index.column() == 1)
			{
				//display slice index
				auto d = item->data(0);
				auto mark = d.value<QGraphicsItem*>();
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
			Q_ASSERT_X(d.canConvert<QSharedPointer<CategoryItem>>(),
				"MarkModel::data", "convert failure");
			return d.value<QSharedPointer<CategoryItem>>()->visible() ? Qt::Checked : Qt::Unchecked;
		case TreeItemType::Mark:
			Q_ASSERT_X(d.canConvert<QGraphicsItem*>(),
				"MarkModel::data", "convert failure");
			auto mark = d.value<QGraphicsItem*>();
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
			Q_ASSERT_X(d.canConvert<QSharedPointer<CategoryItem>>(),
				"MarkModel::data", "convert failure");
			return d.value<QSharedPointer<CategoryItem>>()->color();
		case TreeItemType::Mark:
			Q_ASSERT_X(d.canConvert<QGraphicsItem*>(),
				"MarkModel::data", "convert failure");
			auto mark = d.value<QGraphicsItem*>();

			//get color
			return mark->data(MarkProperty::Color);
		}
	}
	return QVariant();
}

Qt::ItemFlags MarkModel::flags(const QModelIndex & index) const
{
	if (index.isValid() == false)
		return 0;
	if (index.column() == 0)
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
	return QAbstractItemModel::flags(index);
}

bool MarkModel::setData(const QModelIndex & index, const QVariant & value, int role)
{

	if (role == Qt::CheckStateRole && index.column() == 0)
	{
		TreeItem * item = static_cast<TreeItem*>(index.internalPointer());
		QVariant d = item->data(index.column());
		switch (item->type())
		{
		case TreeItemType::Root:
			return false;
		case TreeItemType::Category:
		{
			Q_ASSERT_X(d.canConvert<QSharedPointer<CategoryItem>>(),
				"MarkModel::setData", "convert failure");
			d.value<QSharedPointer<CategoryItem>>()->setVisible(value == Qt::Checked);
			emit dataChanged(index, index, QVector<int>{Qt::CheckStateRole});
			//update child state recursively
			int c = rowCount(index);
			for (int i = 0; i < c; i++)
				setData(MarkModel::index(i, 0, index), value, Qt::CheckStateRole);
			return true;
		}
		case TreeItemType::Mark:
			Q_ASSERT_X(d.canConvert<QGraphicsItem*>(),
				"MarkModel::setData", "convert failure");
			auto mark = d.value<QGraphicsItem*>();
			bool vis = value == Qt::Checked;
			//set display states
			mark->setData(MarkProperty::VisibleState, vis);
			//This is a bull shit
			//auto m = QueryMarkItemInterface(mark);
			//m->setVisible(vis);
			updateMarkVisible_Helper(mark);
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
	TreeItem * item = getItem_Helper(parent);
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
	TreeItem * item = getItem_Helper(parent);
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
	TreeItem * parentItem = getItem_Helper(parent);
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

	TreeItem * item = getItem_Helper(index);
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
	TreeItem * item = getItem_Helper(parent);
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

