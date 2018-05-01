#include "ItemContext.h"
//#include "imageviewer.h"
#include <QPicture>
#include <QModelIndex>
#include <qdebug.h>
#include <cassert>
#include <memory>


ItemContext::ItemContext() :
	m_mrcContext{}
{
}
ItemContext::ItemContext(const QString & fileName) : ItemContext()
{
	bool opened = open(fileName);
	if (opened == false)
		return;
}

ItemContext::ItemContext(const ItemContext & model)
{
	m_mrcFile = model.m_mrcFile;
	m_mrcContext = model.m_mrcContext;

	m_topSliceMarks = model.m_topSliceMarks;

	m_modifiedTopSlice = model.m_modifiedTopSlice;
	m_modifiedTopSliceFlags = model.m_modifiedTopSliceFlags;
}

ItemContext::ItemContext(ItemContext && model)noexcept
{
	m_mrcFile = std::move(model.m_mrcFile);
	m_mrcContext = model.m_mrcContext;
	m_topSliceMarks = std::move(model.m_topSliceMarks);
	m_modifiedTopSlice = std::move(model.m_modifiedTopSlice);
	m_modifiedTopSliceFlags = std::move(model.m_modifiedTopSliceFlags);
}

ItemContext & ItemContext::operator=(const ItemContext & model)
{
	// TODO: insert return statement here
	if (this == &model)
		return *this;
	m_mrcFile = model.m_mrcFile;
	m_mrcContext = model.m_mrcContext;
	m_topSliceMarks = model.m_topSliceMarks;
	m_modifiedTopSlice = model.m_modifiedTopSlice;
	m_modifiedTopSliceFlags = model.m_modifiedTopSliceFlags;
}

ItemContext & ItemContext::operator=(ItemContext && model)noexcept
{
	if (this == &model)
		return *this;
	m_mrcFile = std::move(model.m_mrcFile);
	m_mrcContext = model.m_mrcContext;
	m_topSliceMarks = std::move(model.m_topSliceMarks);
	m_modifiedTopSlice = std::move(model.m_modifiedTopSlice);
	m_modifiedTopSliceFlags = std::move(model.m_modifiedTopSliceFlags);
	// TODO: insert return statement here
}

ItemContext::~ItemContext()
{

}

QImage ItemContext::getOriginalTopSlice(int index) const
{
	int width = m_mrcFile.getWidth();
	int height = m_mrcFile.getHeight();
	return QImage(m_mrcFile.data() + index * width*height, width, height, QImage::Format_Grayscale8);
}

bool ItemContext::save(const QString & fileName, ItemContext::DataFormat formate)
{
	//TODO:
	/*This function need to check whether the data
	* has been modified before saving
	*/
	Q_UNUSED(fileName);
	Q_UNUSED(formate);

	return false;
}

/// \brief This function is to open specific mrc file
/// \param fileName file name to open
/// \return return true if success,or false
/// \exception no exception will be throw in this function

bool ItemContext::open(const QString & fileName)
{
	m_mrcFile.open(fileName.toStdString());
	m_mrcContext.valid = m_mrcFile.isOpened();
	if (m_mrcFile.isOpened() == false) {
		return false;
	}

	//Context initialization
	m_mrcContext.currentTopSliceIndex = 0;
	m_mrcContext.currentRightSliceIndex = 0;
	m_mrcContext.currentFrontSliceIndex = 0;


	int topSliceCount = m_mrcFile.getSliceCount();
	int rightSliceCount = m_mrcFile.getWidth();
	int frontSliceCount = m_mrcFile.getHeight();

	//Members initialization
	m_topSliceMarks.resize(topSliceCount);
	m_rightSliceMarks.resize(rightSliceCount);
	m_frontSliceMarks.resize(frontSliceCount);


	m_modifiedTopSlice.resize(topSliceCount);
	m_modifiedTopSliceFlags = QVector<bool>(topSliceCount, false);
	m_modifiedRightSlice.resize(rightSliceCount);
	m_modifiedRightSliceFlags = QVector<bool>(rightSliceCount, false);
	m_modifiedFrontSlice.resize(frontSliceCount);
	m_modifiedFrontSliceFlags = QVector<bool>(frontSliceCount, false);

	//createScene();

	return true;
}

bool ItemContext::openMarks(const QString & fileName)
{
	Q_UNUSED(fileName);
	if (m_mrcFile.isOpened() == false)
		return false;
	//TODO:

	return true;
}

bool ItemContext::saveMarks(const QString & fileName, MarkFormat format)
{
	/*
	*Transform the QPicture to images and save the pixel data
	*/


	/*This function is really really a mess. */

	/*Convert QGraphicsPolygonItem to QImage */

	bool empty = true;
	QVector<QImage> images;
	for (int i = 0; i < m_topSliceMarks.size(); i++) {
		///TODO::Convert QGraphicsPolygonItem into qimage
		//images.push_back(QImage(getWidth(),getHeight(),
		//	QImage::Format_Grayscale8));
		///*Uninitialized*/
		//images.back().fill(Qt::black); 
		//for (auto & pic : m_topSliceMarks[i]) {
		//	
		//	if (pic.isNull() == false) {
		//		empty = false;
		//	}
		//	QPainter p(&images[i]);
		//	p.drawPicture(0,0,pic);
		//}
	}

	if (empty == true) {
		qDebug() << "Empty marks";
		return false;
	}
	if (format == MarkFormat::MRC) {
		//TODO:
		//Wait the save() member of MRC class to be implemented
		//
		int width = getWidth();
		int height = getHeight();
		int sliceCount = getTopSliceCount();
		unsigned char * data = new unsigned char[m_topSliceMarks.size()*getWidth()*getHeight()];
		if (data == nullptr) {
			qDebug() << "allocating faild";
			std::cerr << __LINE__;
			return false;
		}
		//Copy memory
		//This is a overhead operation
		for (int i = 0; i < images.size(); i++) {
			memcpy(data + i * width*height, images[i].bits(), sizeof(unsigned char)*width*height);
		}

		//MRC mrcMarks(data,getWidth(),getHeight(),getSliceCount(),
		//	MRC::ImageDimensionType::ImageStack,
		//	MRC::DataType::Byte8);



		MRC mrcMarks = MRC::fromMRC(m_mrcFile, data);
		/*This function will execute deep copy,so this is also a overhead operation*/
		if (mrcMarks.isOpened() == false) {
			qDebug() << "Cannot create mrc marks file";
			std::cerr << __LINE__;
			return false;
		}
		mrcMarks.save(fileName.toStdString(), MRC::Format::MRC);

		delete[] data;
	}
	else if (format == MarkFormat::RAW) {
		//Later,this need to be replace with Qt-style file IO
		FILE * fp = fopen(fileName.toStdString().c_str(), "wb");
		if (fp == nullptr) {
			std::cerr << __LINE__;
			return false;
		}
		size_t totalCount = 0;
		for (const QImage & image : images) {
			int sizeOfWrite = fwrite(image.bits(),
				sizeof(unsigned char),
				image.width()*image.height(),
				fp);
			if (sizeOfWrite < image.width()*image.height()) {
				std::cerr << __LINE__;
				return false;
			}
			else {
				totalCount += sizeOfWrite * sizeof(unsigned char);
				fseek(fp, 0, SEEK_END);
			}
		}
		size_t totalCountHasBeenWrite = ftell(fp);
		fclose(fp);
		//Check the size
		if (totalCount != totalCountHasBeenWrite) {
			std::cerr << __LINE__;
			return false;
		}
	}

	return true;
}

QImage ItemContext::getTopSlice(int index) const
{
	if (m_modifiedTopSliceFlags[index] == false) {
		return getOriginalTopSlice(index);
	}
	else {
		return m_modifiedTopSlice[index];
	}
}

void ItemContext::setTopSlice(const QImage & image, int index)
{
	if (image.format() != QImage::Format_Grayscale8) {
		qCritical("QImage format must be grayscale8.");
		return;
	}
	int width = image.width();
	int height = image.height();
	if (width != m_mrcFile.getWidth() || height != m_mrcFile.getHeight()) {
		qCritical("Lengths are not matched.");
		return;
	}
	//memcpy(m_mrcFile.data(), image.bits(), width*height * sizeof(unsigned char));
	m_modifiedTopSliceFlags[index] = true;
	m_modifiedTopSlice[index] = image;
}


QImage ItemContext::getOriginalRightSlice(int index) const
{
	int width = m_mrcFile.getWidth();
	int height = m_mrcFile.getHeight();
	int slice = m_mrcFile.getSliceCount();
	int size = width * height *slice;
	std::unique_ptr<unsigned char[]> imageBuffer(new unsigned char[slice*height]);
	auto data = m_mrcFile.data();
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < slice; j++)
		{
			int idx = index + i * width + j * width*height;
			Q_ASSERT(idx < size);
			imageBuffer[j + i * slice] = data[idx];
		}
	}
	return QImage(imageBuffer.get(), slice, height, QImage::Format_Grayscale8).copy();
}

QImage ItemContext::getRightSlice(int index) const
{
	if (m_modifiedRightSliceFlags[index] == false) {
		return getOriginalRightSlice(index);
	}
	else {
		return m_modifiedRightSlice[index];
	}
}

void ItemContext::setRightSlice(const QImage & image, int index)
{
	if (image.format() != QImage::Format_Grayscale8) {
		qCritical("QImage format must be grayscale8.");
		return;
	}
	int width = image.width();
	int height = image.height();
	if (width != m_mrcFile.getSliceCount() || height != m_mrcFile.getHeight()) {
		qCritical("Lengths are not matched.");
		return;
	}
	m_modifiedRightSliceFlags[index] = true;
	m_modifiedRightSlice[index] = image;
}

QImage ItemContext::getOriginalFrontSlice(int index) const
{
	int width = m_mrcFile.getWidth();
	int height = m_mrcFile.getHeight();
	int slice = m_mrcFile.getSliceCount();
	int size = width * height *slice;
	std::unique_ptr<unsigned char[]> imageBuffer(new unsigned char[width*slice]);
	auto data = m_mrcFile.data();
	for (int i = 0; i < slice; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int idx = j + index * width + i * width*height;
			Q_ASSERT(idx < size);
			imageBuffer[j + i * width] = data[idx];
		}
	}
	return QImage(imageBuffer.get(), width, slice, QImage::Format_Grayscale8).copy();
}

QImage ItemContext::getFrontSlice(int index) const
{
	if (m_modifiedFrontSliceFlags[index] == false) {
		return getOriginalFrontSlice(index);
	}
	else {
		return m_modifiedFrontSlice[index];
	}
}

void ItemContext::setFrontSlice(const QImage & image, int index)
{
	if (image.format() != QImage::Format_Grayscale8) {
		qCritical("QImage format must be grayscale8.");
		return;
	}
	int width = image.width();
	int height = image.height();
	if (width != m_mrcFile.getWidth() || height != m_mrcFile.getSliceCount()) {
		qCritical("Lengths are not matched.");
		return;
	}
	//memcpy(m_mrcFile.data(), image.bits(), width*height * sizeof(unsigned char));
	m_modifiedFrontSliceFlags[index] = true;
	m_modifiedFrontSlice[index] = image;
}

int ItemContext::sliceCount(SliceType type)
{
	switch (type)
	{
	case SliceType::SliceZ:
		return getTopSliceCount();
	case SliceType::SliceY:
		return getRightSliceCount();
	case SliceType::SliceX:
		return getFrontSliceCount();
	default:
		return 0;
	}
}

QImage ItemContext::orignalSlice(int index, SliceType type)
{
	switch (type)
	{
	case SliceType::SliceZ:
		return getOriginalTopSlice(index);
	case SliceType::SliceY:
		return getOriginalRightSlice(index);
	case SliceType::SliceX:
		return getOriginalFrontSlice(index);
	default:
		return QImage();
	}
}

QImage ItemContext::slice(int index, SliceType type) const
{
	switch (type)
	{
	case SliceType::SliceZ:
		return getTopSlice(index);
	case SliceType::SliceY:
		return getRightSlice(index);
	case SliceType::SliceX:
		return getFrontSlice(index);
	default:
		return QImage();

	}
}

void ItemContext::setSlice(const QImage & image, int index, SliceType type)
{
	switch (type)
	{
	case SliceType::SliceZ:
		setTopSlice(image, index);
		return;
	case SliceType::SliceY:
		setRightSlice(image, index);
		return;
	case SliceType::SliceX:
		setFrontSlice(image, index);
		return;
	default:
		qWarning("SliceType does not exisit.");
		return;
	}
}

//QVector<QImage> ItemContext::getSlices() const
//{
//	QVector<QImage> imgVec;
//	int slices = m_mrcFile.getSliceCount();
//	for (int i = 0; i < slices; i++) {
//		imgVec.push_back(getTopSlice(i));
//	}
//	return imgVec;
//}

void ItemContext::setTopSliceMark(QGraphicsItem* mark, int index)
{
	Q_UNUSED(mark);
	Q_UNUSED(index);
}

void ItemContext::addTopSliceMark(int slice, QGraphicsItem* mark)
{
	m_topSliceMarks[slice].push_back(mark);
	m_topSliceMarkVisble[mark] = true;
}


QList<QGraphicsItem*>  ItemContext::getTopSliceMarks(int slice) const
{
	return m_topSliceMarks[slice];
}

bool ItemContext::topSliceMarkVisble(QGraphicsItem * item) const
{
	auto itr = m_topSliceMarkVisble.find(item);
	if (itr == m_topSliceMarkVisble.end())
		return false;
	return *itr;
}

void ItemContext::setRightSliceMark(QGraphicsItem* mark, int index)
{
	Q_UNUSED(mark);
	Q_UNUSED(index);
}

void ItemContext::addRightSliceMark(int slice, QGraphicsItem* mark)
{
	m_rightSliceMarks[slice].push_back(mark);
	m_rightSliceMarkVisble[mark] = true;
}

QList<QGraphicsItem*>  ItemContext::getRightSliceMarks(int slice) const
{
	return m_rightSliceMarks[slice];
}

bool ItemContext::rightSliceMarkVisble(QGraphicsItem * item) const
{
	auto itr = m_rightSliceMarkVisble.find(item);
	if (itr == m_rightSliceMarkVisble.end())
		return false;
	return *itr;
}

void ItemContext::setFrontSliceMark(QGraphicsItem* mark, int index)
{
	Q_UNUSED(mark);
	Q_UNUSED(index);
}

void ItemContext::addFrontSliceMark(int slice, QGraphicsItem*mark)
{
	m_frontSliceMarks[slice].push_back(mark);
	m_frontSliceMarkVisble[mark] = true;
}

QList<QGraphicsItem*>  ItemContext::getFrontSliceMarks(int slice) const
{
	return m_frontSliceMarks[slice];

}

bool ItemContext::frontSliceMarkVisble(QGraphicsItem * item) const
{
	auto itr = m_frontSliceMarkVisble.find(item);
	if (itr == m_frontSliceMarkVisble.end())
		return false;
	return *itr;
}

void ItemContext::addSliceMark(QGraphicsItem * mark, int index, SliceType type)
{
	switch (type)
	{
	case SliceType::SliceZ:
		addTopSliceMark(index, mark);
		return;
	case SliceType::SliceY:
		addRightSliceMark(index, mark);
		return;
	case SliceType::SliceX:
		addFrontSliceMark(index, mark);
		return;
	default:
		qWarning("Type does not exisit.");
		return;
	}
}

QList<QGraphicsItem*> ItemContext::sliceMarks(int index, SliceType type)
{
	switch (type)
	{
	case SliceType::SliceZ:
		return getTopSliceMarks(index);
	case SliceType::SliceY:
		return getRightSliceMarks(index);
	case SliceType::SliceX:
		return getFrontSliceMarks(index);
	default:
		qWarning("Type does not exisit.");
		return QList<QGraphicsItem*>();
	}
}

QList<QGraphicsItem*> ItemContext::visibleSliceMarks(int index, SliceType type)
{
	QList<QGraphicsItem *> items;
	switch (type)
	{
	case SliceType::SliceZ:
		foreach(auto item,m_topSliceMarks[index])
		{
			///TODO::please ensure that item in marks also must be in visible hash table
			if (m_topSliceMarkVisble[item])
				items.push_back(item);
		}
		break;
	case SliceType::SliceY:
		foreach(auto item, m_rightSliceMarks[index])
		{
			///TODO::please ensure that item in marks also must be in visible hash table
			if (m_rightSliceMarkVisble[item])
				items.push_back(item);
		}
		break;
	case SliceType::SliceX:
		foreach(auto item, m_frontSliceMarks[index])
		{
			///TODO::please ensure that item in marks also must be in visible hash table
			if (m_frontSliceMarkVisble[item])
				items.push_back(item);
		}
		break;
	}
	return items;
}

bool ItemContext::sliceMarkVisible(QGraphicsItem * item, SliceType type)
{
	switch (type)
	{
	case SliceType::SliceZ:
		return topSliceMarkVisble(item);
	case SliceType::SliceY:
		return rightSliceMarkVisble(item);
	case SliceType::SliceX:
		return frontSliceMarkVisble(item);
	default:
		qWarning("Type does not exisit.");
		return false;
	}
}


void ItemContext::setSliceMarkVisible(QGraphicsItem * item,bool visible, SliceType type)
{
	switch (type)
	{
	case SliceType::SliceZ:
		setTopSliceMarkVisible(item, visible);
		return;
	case SliceType::SliceY:
		setRightSliceMarkVisible(item, visible);
		return;
	case SliceType::SliceX:
		setFrontSLiceMarkVisible(item,visible);
		return;
	default:
		return;
	}
}

void ItemContext::createScene()
{
	m_scene.reset(new GraphicsScene(nullptr));

	for (int i = 0; i < getTopSliceCount(); i++)
	{
		QPixmap pix = QPixmap::fromImage(getTopSlice(i), Qt::MonoOnly);

		qDebug() << pix.depth() << " " << getTopSlice(i).depth();
		m_scene->addPixmap(QPixmap::fromImage(getTopSlice(i), Qt::MonoOnly));
		//qDebug() << "Top Slice:"<<i;
	}
	for (int i = 0; i < getRightSliceCount(); i++)
	{
		m_scene->addPixmap(QPixmap::fromImage(getRightSlice(i), Qt::MonoOnly));
		//qDebug() << "Right Slice:" << i;
	}
	for (int i = 0; i < getFrontSliceCount(); i++)
	{
		//qDebug() << "Front Slice:" << i;
		m_scene->addPixmap(QPixmap::fromImage(getFrontSlice(i), Qt::MonoOnly));
	}
}

void ItemContext::setTopSliceMarkVisible(QGraphicsItem * mark,bool visible)
{
	auto itr = m_topSliceMarkVisble.find(mark);
	if (itr == m_topSliceMarkVisble.end())
		return;
	*itr = visible;
}

void ItemContext::setRightSliceMarkVisible(QGraphicsItem * mark, bool visible)
{
	auto itr = m_rightSliceMarkVisble.find(mark);
	if (itr == m_rightSliceMarkVisble.end())
		return;
	*itr = visible;
}

void ItemContext::setFrontSLiceMarkVisible(QGraphicsItem * mark, bool visible)
{
	auto itr = m_frontSliceMarkVisble.find(mark);
	if (itr == m_frontSliceMarkVisble.end())
		return;
	*itr = visible;
}


/*
 *New data Model
 */


QModelIndex DataItemModel::appendChild(const QModelIndex & parent, bool * success)
{
	bool flag = insertRows(rowCount(parent), 1, parent);
	if (success)*success = flag;
	return index(rowCount(parent) - 1, 0, parent);
}

bool DataItemModel::removeChild(const QModelIndex & index, const QModelIndex & parent)
{
	Q_UNUSED(index);
	return removeRows(0, rowCount(parent), parent);
}

QModelIndex DataItemModel::modelIndexOf(int column, const QModelIndex & itemIndex)
{
	TreeItem * item = static_cast<TreeItem*>(itemIndex.internalPointer());
	if (item == nullptr)return QModelIndex();
	return index(item->row(), column, parent(itemIndex));
}

void DataItemModel::insertRootItemIndex(const QModelIndex & index, int position)
{
	if (position == -1)
		m_itemRootIndex.append(index);
	else
		m_itemRootIndex.insert(position, index);
}

void DataItemModel::removeRootItemIndex(int position)
{
	m_itemRootIndex.removeAt(position);
}

QModelIndex DataItemModel::rootItemIndex(int position)
{
	return m_itemRootIndex.value(position);
}

DataItemModel::DataItemModel(const QString & data, QObject * parent) :QAbstractItemModel(parent)
{
	///TODO:: construct a new root
	Q_UNUSED(data);
	QVector<QVariant> headers;
	headers << "Value:" << "Descption:";
	m_rootItem = new TreeItem(headers);
}

DataItemModel::~DataItemModel()
{
	delete m_rootItem;
}

QVariant DataItemModel::data(const QModelIndex & index, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	if (index.isValid() == false)
		return QVariant();
	TreeItem * item = static_cast<TreeItem*>(index.internalPointer());
	//qDebug() << item->data(index.column());
	return item->data(index.column());
}

Qt::ItemFlags DataItemModel::flags(const QModelIndex & index) const
{
	if (index.isValid() == false)
		return 0;
	return QAbstractItemModel::flags(index);
}

bool DataItemModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if (role != Qt::EditRole)return false;

	TreeItem * item = getItem(index);

	bool ok = item->setData(index.column(), value);
	if (ok == true)
		emit dataChanged(index, index);
	return ok;
}

bool DataItemModel::insertColumns(int column, int count, const QModelIndex & parent)
{
	beginInsertColumns(parent, column, column + count - 1);
	//insert same columns at same position from the top of the tree to down recursively
	bool success = m_rootItem->insertColumns(column, count);
	endInsertColumns();
	return success;
}

bool DataItemModel::removeColumns(int column, int count, const QModelIndex & parent)
{
	beginRemoveColumns(parent, column, column + count - 1);
	bool success = m_rootItem->removeColumns(column, count);
	endRemoveColumns();

	if (m_rootItem->columnCount() == 0)
		removeRows(0, rowCount());
	return success;
}

bool DataItemModel::insertRows(int row, int count, const QModelIndex & parent)
{
	TreeItem * item = getItem(parent);
	beginInsertRows(parent, row, count + row - 1);
	//the number of inserted column is the same as the root, i.e 2
	bool success = item->insertChildren(row, count, columnCount());
	endInsertRows();
	return success;
}

bool DataItemModel::removeRows(int row, int count, const QModelIndex & parent)
{
	TreeItem * item = getItem(parent);
	bool success = true;

	beginRemoveRows(parent, row, row + count - 1);
	success = item->removeChildren(row, count);
	endRemoveRows();
	return success;
}

QVariant DataItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return m_rootItem->data(section);
	return QVariant();
}

QModelIndex DataItemModel::index(int row, int column, const QModelIndex & parent) const
{

	//Check if the index is valid
	//if(hasIndex(row,column,parent) == false)
	//	return QModelIndex();
	//If the index points to a non-root node and its column is not 0

	if (parent.isValid() == true && parent.column() != 0)
		return QModelIndex();
	TreeItem * parentItem = getItem(parent);
	TreeItem * childItem = parentItem->child(row);
	if (childItem == nullptr)
		return QModelIndex();
	else
		return createIndex(row, column, childItem);
}

QModelIndex DataItemModel::parent(const QModelIndex & index) const
{
	//Index points to a root item
	if (index.isValid() == false)return QModelIndex();

	TreeItem * item = getItem(index);
	TreeItem * parentItem = item->parentItem();

	//If index points to a child item of root item
	if (parentItem == m_rootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int DataItemModel::rowCount(const QModelIndex & parent) const
{
	//Only a item with 0 column number has children
	if (parent.column() > 0)
		return 0;
	TreeItem * item = getItem(parent);
	return item->childCount();
}

int DataItemModel::columnCount(const QModelIndex & parent) const
{
	if (parent.isValid() == false)
		return m_rootItem->columnCount();
	return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
}

QModelIndex DataItemModel::addItemHelper(const QString& fileName, const QString& info)
{
	bool success;
	const QModelIndex & newRootItemIndex = appendChild(QModelIndex(), &success);//add a new file to as the last child of the parent
	if (success == false)
		return QModelIndex();

	//get the newest index inserted before
	//const QModelIndex & newRootItemIndex = index(rowCount() - 1, 0);
	//add file row
	insertRootItemIndex(newRootItemIndex);
	setData(newRootItemIndex, fileName);

	//add properties row
	const QModelIndex & informationIndex = appendChild(newRootItemIndex, &success);
	if (success == false)
		return QModelIndex();
	setData(informationIndex, QStringLiteral("Properties"));
	QStringList lines = info.split('\n', QString::SkipEmptyParts);
	for (auto & it : lines)
	{
		auto line = it.split(':', QString::SkipEmptyParts);
		{
			//success = insertRows(rowCount(informationIndex), 1, informationIndex);
			//int lastRow = rowCount(informationIndex) - 1;
			//QModelIndex valueIndex = index(lastRow, 0, informationIndex);
			//QModelIndex descIndex = index(lastRow, 1, informationIndex);
			QModelIndex newInserted = appendChild(informationIndex, &success);
			QModelIndex valueIndex = modelIndexOf(0, newInserted);
			QModelIndex descIndex = modelIndexOf(1, newInserted);
			if (success == true) {
				setData(valueIndex, line.value(1));
				setData(descIndex, line.value(0));
			}
		}
	}

	//add marks row
	const QModelIndex & marksIndex = appendChild(newRootItemIndex, &success);
	if(success == false)
	{
		qCritical("append Child failed\n");
		return QModelIndex();
	}
	setData(marksIndex, QStringLiteral("Marks"));

	setData(modelIndexOf(1, marksIndex), QVariant::fromValue(0));

	return newRootItemIndex;
}

//void DataItemModel::addItem(const QSharedPointer<MRC>& item)
//{
//
//	const auto fileName = QString::fromStdString(item->getFileName());
//	const auto info = QString::fromStdString(item->getMRCInfo());
//	qDebug() << fileName << " " << info;
//	const QModelIndex & newModelIndex = addItemHelper(fileName, info);
//	QModelIndex idx = modelIndexOf(1, newModelIndex);
//
//	setData(idx, QVariant::fromValue<QSharedPointer<MRC>>(item));
//
//	QVariant var = data(idx,Qt::DisplayRole);
//}

void DataItemModel::addItem(const QSharedPointer<ItemContext>& item)
{
	const auto fileName = QString::fromStdString(item->getMRCFile().getFileName());
	const auto info = QString::fromStdString(item->getMRCInfo().toStdString());
	//qDebug() << fileName << " " << info;
	const QModelIndex & newModelIndex = addItemHelper(fileName, info);
	QModelIndex idx = modelIndexOf(1, newModelIndex);
	setData(idx, QVariant::fromValue<QSharedPointer<ItemContext>>(item));

	//QVariant var = data(idx, Qt::DisplayRole);
	//qDebug() << var.canConvert<QSharedPointer<ItemContext>>();
}

