#include "ItemContext.h"
//#include "imageviewer.h"
#include <QPicture>
#include <QModelIndex>
#include <QCheckBox>
#include <qdebug.h>
#include <cassert>
#include <memory>
#include <iostream>


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
	return *this;
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
	//TODO: insert return statement here
	return *this;
}

ItemContext::~ItemContext()
{

}

QImage ItemContext::getOriginalTopSlice(int index) const
{
	int width = m_mrcFile.getWidth();
	int height = m_mrcFile.getHeight();
	unsigned char * d = m_mrcFile.data<unsigned char>();
	Q_ASSERT_X(d != nullptr, "ItemContext::getOriginalTopSlice", "type convertion error");
	return QImage(d+index * width*height, width, height, QImage::Format_Grayscale8);

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
		//int sliceCount = getTopSliceCount();
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
		//	MRC::DataType::Integer8);



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
			if (sizeOfWrite < (int)image.width()*image.height()) {
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
	auto data = m_mrcFile.data<unsigned char>();
	if(data == nullptr)
	{
		qCritical() << "Format error\n";
		return QImage();
	}
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
	auto data = m_mrcFile.data<unsigned char>();
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
	case SliceType::Top:
		return getTopSliceCount();
	case SliceType::Right:
		return getRightSliceCount();
	case SliceType::Front:
		return getFrontSliceCount();
	default:
		return 0;
	}
}

QImage ItemContext::orignalSlice(int index, SliceType type)
{
	switch (type)
	{
	case SliceType::Top:
		return getOriginalTopSlice(index);
	case SliceType::Right:
		return getOriginalRightSlice(index);
	case SliceType::Front:
		return getOriginalFrontSlice(index);
	default:
		return QImage();
	}
}

QImage ItemContext::slice(int index, SliceType type) const
{
	switch (type)
	{
	case SliceType::Top:
		return getTopSlice(index);
	case SliceType::Right:
		return getRightSlice(index);
	case SliceType::Front:
		return getFrontSlice(index);
	default:
		return QImage();

	}
}

void ItemContext::setSlice(const QImage & image, int index, SliceType type)
{
	switch (type)
	{
	case SliceType::Top:
		setTopSlice(image, index);
		return;
	case SliceType::Right:
		setRightSlice(image, index);
		return;
	case SliceType::Front:
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
	case SliceType::Top:
		addTopSliceMark(index, mark);
		return;
	case SliceType::Right:
		addRightSliceMark(index, mark);
		return;
	case SliceType::Front:
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
	case SliceType::Top:
		return getTopSliceMarks(index);
	case SliceType::Right:
		return getRightSliceMarks(index);
	case SliceType::Front:
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
	case SliceType::Top:
		foreach(auto item,m_topSliceMarks[index])
		{
			///TODO::please ensure that item in marks also must be in visible hash table
			if (m_topSliceMarkVisble[item])
				items.push_back(item);
		}
		break;
	case SliceType::Right:
		foreach(auto item, m_rightSliceMarks[index])
		{
			///TODO::please ensure that item in marks also must be in visible hash table
			if (m_rightSliceMarkVisble[item])
				items.push_back(item);
		}
		break;
	case SliceType::Front:
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
	case SliceType::Top:
		return topSliceMarkVisble(item);
	case SliceType::Right:
		return rightSliceMarkVisble(item);
	case SliceType::Front:
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
	case SliceType::Top:
		setTopSliceMarkVisible(item, visible);
		return;
	case SliceType::Right:
		setRightSliceMarkVisible(item, visible);
		return;
	case SliceType::Front:
		setFrontSLiceMarkVisible(item,visible);
		return;
	default:
		return;
	}
}

void ItemContext::createScene()
{
	m_scene.reset(new SliceScene(nullptr));

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




QWidget * MarkItemModelDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	Q_UNUSED(option);
	auto m = index.model();
	auto var = m->data(index);
	if (var.canConvert<QString>() == true) {
		QString text =  var.value<QString>();
		if (text[0] == '#') {
			//Mark Item
			QWidget* checkBox = new QCheckBox(text, parent);
			return checkBox;
		}
	}
	return nullptr;
}

void MarkItemModelDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
	auto m = index.model();
	auto var = m->data(index);
	if (var.canConvert<QString>() == true) {
		QString text = var.value<QString>();
		if (text[0] == '#') {
			//Mark Item
			QModelIndex visibleField = index.sibling(index.row(), index.column() + 1);
			QVariant v = m->data(visibleField);
			Q_ASSERT_X(v.canConvert<bool>(), "DataItemModelDelegate::setEditorData", "Visible field can not be converted to boolean.");
			bool visible = v.value<bool>();
			auto w = static_cast<QCheckBox *>(editor);
			Q_ASSERT_X(w, "DataItemModelDelegate::setEditorData", "Widget * can not be converted to QCheckBox");
			w->setChecked(visible);
		}
	}
}

void MarkItemModelDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
	auto var = model->data(index);
	if (var.canConvert<QString>() == true) {
		QString text = var.value<QString>();
		if (text[0] == '#') {
			//Mark Item
			QModelIndex visibleField = index.sibling(index.row(), index.column() + 1);
			QVariant v = model->data(visibleField);
			Q_ASSERT_X(v.canConvert<bool>(), "DataItemModelDelegate::setEditorData", "Visible field can not be converted to boolean.");
			auto w = static_cast<QCheckBox *>(editor);
			Q_ASSERT_X(w, "DataItemModelDelegate::setEditorData", "Widget * can not be converted to QCheckBox");
			bool visible = w->isChecked();
			model->setData(visibleField, QVariant::fromValue(visible));
		}
	}
}

void MarkItemModelDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	Q_UNUSED(option);
	Q_UNUSED(index);
	editor->setGeometry(option.rect);
}

int MarkItemModelDelegate::level(const QModelIndex & index)
{
	auto m = index.model();
	int level = 0;
	QModelIndex parentIndex;
	QModelIndex currentIndex = index;
	while ((parentIndex = m->parent(currentIndex)).isValid() == true) {
		currentIndex = parentIndex;
		level++;
	}
	return level;
}

bool MarkItemModelDelegate::isMark(const QModelIndex & index)
{
	Q_UNUSED(index);
	return true;
}
