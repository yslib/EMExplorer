#include "ItemContext.h"
#include <QPainter>
#include <qdebug.h>
#include <memory>
#include <QModelIndex>

#include <cassert>

ItemContext::ItemContext() :
	m_mrcContext{},
	m_modified{},
	m_modifiedFlags{},
	m_marks{}
{

}
ItemContext::ItemContext(const QString & fileName):ItemContext()
{

    bool opened = open(fileName);
    if(opened == false)
        return;
	if(m_mrcFile.isOpened() == true)
		m_marks.resize(m_mrcFile.getSliceCount());

    /*Initialzed the zoom region in the context */
    m_mrcContext.zoomRegion=QRect(0,0,m_mrcFile.getWidth(),m_mrcFile.getHeight());
}

ItemContext::ItemContext(const ItemContext & model)
{
	m_mrcFile = model.m_mrcFile;
	m_mrcContext = model.m_mrcContext;
	m_marks = model.m_marks;
	m_modified = model.m_modified;
	m_modifiedFlags = model.m_modifiedFlags;
}

ItemContext::ItemContext(ItemContext && model)noexcept
{
	m_mrcFile = std::move(model.m_mrcFile);
	m_mrcContext = model.m_mrcContext;
	m_marks = std::move(model.m_marks);
	m_modified = std::move(model.m_modified);
	m_modifiedFlags = std::move(model.m_modifiedFlags);
}

ItemContext & ItemContext::operator=(const ItemContext & model)
{
	// TODO: insert return statement here
	if (this == &model)
		return *this;
	m_mrcFile = model.m_mrcFile;
	m_mrcContext = model.m_mrcContext;
	m_marks = model.m_marks;
	m_modified = model.m_modified;
	m_modifiedFlags = model.m_modifiedFlags;
}

ItemContext & ItemContext::operator=(ItemContext && model)noexcept
{
	if (this == &model)
		return *this;
	m_mrcFile = std::move(model.m_mrcFile);
	m_mrcContext = model.m_mrcContext;
	m_marks = std::move(model.m_marks);
	m_modified = std::move(model.m_modified);
	m_modifiedFlags = std::move(model.m_modifiedFlags);
	// TODO: insert return statement here
}

ItemContext::~ItemContext()
{

}

QImage ItemContext::getOriginalSlice(int index) const
{
	int width = m_mrcFile.getWidth();
	int height = m_mrcFile.getHeight();
	return QImage(m_mrcFile.data() + index*width*height, width, height, QImage::Format_Grayscale8);
}

bool ItemContext::save(const QString & fileName,ItemContext::DataFormat formate)
{
	//TODO:
	/*This function need to check whether the data
	* has been modified before saving
	*/

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
	m_mrcContext.currentTopSliceIndex = 0;
	m_mrcContext.currentRightSliceIndex = 0;
	m_mrcContext.currentFrontSliceIndex = 0;

	m_modified.resize(m_mrcFile.getSliceCount());
	m_modifiedFlags = QVector<bool>(m_mrcFile.getSliceCount(), false);
	return true;
}

bool ItemContext::openMarks(const QString & fileName)
{
	if (m_mrcFile.isOpened() == false)
		return false;
	//TODO:

	return true;
}

bool ItemContext::saveMarks(const QString & fileName,MarkFormat format)
{
	/*
	*Transform the QPicture to images and save the pixel data
	*/


	/*This function is really really a mess. */

	/*Convert QPicture to QImage */
	bool empty = true;
	QVector<QImage> images;
	for (int i = 0; i < m_marks.size();i++) {
		images.push_back(QImage(getWidth(),getHeight(),
			QImage::Format_Grayscale8));
		/*Uninitialized*/
		images.back().fill(Qt::black); 
		for (auto & pic : m_marks[i]) {
			if (pic.isNull() == false) {
				empty = false;
			}
			QPainter p(&images[i]);
			p.drawPicture(0,0,pic);
		}
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
		unsigned char * data = new unsigned char[m_marks.size()*getWidth()*getHeight()];
		if (data == nullptr) {
			qDebug() << "allocating faild";
			std::cerr << __LINE__;
			return false;
		}
		//Copy memory
		//This is a overhead operation
		for (int i = 0; i <images.size(); i++) {
			memcpy(data+i*width*height, images[i].bits(), sizeof(unsigned char)*width*height);
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
	else if(format == MarkFormat::RAW) {
		//Later,this need to be replace with Qt-style file IO
		FILE * fp = fopen(fileName.toStdString().c_str(), "wb");
		if (fp == nullptr){
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

QImage ItemContext::getSlice(int index) const
{
	if (m_modifiedFlags[index] == false) {
		return getOriginalSlice(index);
	}
	else {
		return m_modified[index];
	}
}

void ItemContext::setSlice(const QImage & image, int index)
{
	if (image.format() != QImage::Format_Grayscale8)
		return;
	int width = image.width();
	int height = image.height();
	if (width != m_mrcFile.getWidth() || height != m_mrcFile.getHeight())
		return;
	//memcpy(m_mrcFile.data(), image.bits(), width*height * sizeof(unsigned char));
	m_modifiedFlags[index] = true;
	m_modified[index] = image;
}


QImage ItemContext::getRightSlice(int index) const
{
	int width = m_mrcFile.getWidth();
	int height = m_mrcFile.getHeight();
	int slice = m_mrcFile.getSliceCount();
	int size = width * height *slice;
	std::unique_ptr<unsigned char[]> imageBuffer(new unsigned char[slice*height]);
	auto data = m_mrcFile.data();
	for(int i=0;i<height;i++)
	{
		for(int j =0;j<slice;j++)
		{
			int idx = index + i * width + j * width*height;
			assert(idx < size);
			imageBuffer[j + i * slice] = data[idx];
		}
	}
	return QImage(imageBuffer.get(), slice, height, QImage::Format_Grayscale8).copy();
}

QImage ItemContext::getFrontSlice(int index) const
{
	int width = m_mrcFile.getWidth();
	int height = m_mrcFile.getHeight();
	int slice = m_mrcFile.getSliceCount();
	int size = width * height *slice;
	std::unique_ptr<unsigned char[]> imageBuffer(new unsigned char[width*slice]);
	auto data = m_mrcFile.data();
	for(int i=0;i<slice;i++)
	{
		for(int j =0;j<width;j++)
		{
			int idx = j + index * width+ i* width*height;
			assert(idx < size);
			imageBuffer[j + i * width] = data[idx];
		}
	}
	return QImage(imageBuffer.get(),width,slice, QImage::Format_Grayscale8).copy();
}

QVector<QImage> ItemContext::getSlices() const
{
	QVector<QImage> imgVec;
	int slices = m_mrcFile.getSliceCount();
	for (int i = 0; i < slices; i++) {
		imgVec.push_back(getSlice(i));
	}
	return imgVec;
}

void ItemContext::setMark(const QPicture& mark, int index)
{

}

void ItemContext::addMark(int slice, const QPicture &mark)
{
   m_marks[slice].push_back(mark);
}

QPicture ItemContext::getMark(int index)const
{
	return QPicture();
}

QVector<QPicture> ItemContext::getMarks(int slice) const
{
    return m_marks[slice];
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


	//add information row

	//success = insertRows(rowCount(newRootItemIndex), 1, newRootItemIndex);
	//if (success == false)
	//	return;
	//const QModelIndex & informationIndex = index(rowCount(newRootItemIndex)-1, 0, newRootItemIndex);

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

	return newRootItemIndex;
}

void DataItemModel::addItem(const QSharedPointer<MRC>& item)
{
	
	const auto fileName = QString::fromStdString(item->getFileName());
	const auto info = QString::fromStdString(item->getMRCInfo());

	qDebug() << fileName << " " << info;
	const QModelIndex & newModelIndex = addItemHelper(fileName, info);


	QModelIndex idx = modelIndexOf(1, newModelIndex);
	setData(idx, QVariant::fromValue<QSharedPointer<MRC>>(item));

	//test:
	//QVariant var = data(idx,Qt::DisplayRole);

}

void DataItemModel::addItem(const QSharedPointer<ItemContext>& item)
{
	const auto fileName = QString::fromStdString(item->getMRCFile().getFileName());
	const auto info = QString::fromStdString(item->getMRCInfo().toStdString());
	qDebug() << fileName << " " << info;
	const QModelIndex & newModelIndex = addItemHelper(fileName, info);
	QModelIndex idx = modelIndexOf(1, newModelIndex);
	setData(idx, QVariant::fromValue<QSharedPointer<ItemContext>>(item));

	QVariant var = data(idx, Qt::DisplayRole);
	qDebug()<<var.canConvert<QSharedPointer<ItemContext>>();
}

