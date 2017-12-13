#include "MRCDataModel.h"
#include <QPainter>
#include <qdebug.h>
MRCDataModel::MRCDataModel() :
	m_mrcContext{},
	m_modified{},
	m_modifiedFlags{},
	m_marks{}
{

}
MRCDataModel::MRCDataModel(const QString & fileName):MRCDataModel()
{
	open(fileName);
	if(m_mrcFile.isOpened() == true)
		m_marks.resize(m_mrcFile.getSliceCount());
}

MRCDataModel::~MRCDataModel()
{

}

QImage MRCDataModel::getOriginalSlice(int index) const
{
	int width = m_mrcFile.getWidth();
	int height = m_mrcFile.getHeight();
	return QImage(m_mrcFile.data() + index*width*height, width, height, QImage::Format_Grayscale8);
}

bool MRCDataModel::save(const QString & fileName)
{
	//TODO:
	/*This function need to check whether the data
	* has been modified before saving
	*/

	return false;
}

bool MRCDataModel::open(const QString & fileName)
{
	m_mrcFile.open(fileName.toStdString());
	m_mrcContext.valid = m_mrcFile.isOpened();
	if (m_mrcFile.isOpened() == false) {
		return false;
	}
	m_mrcContext.currentSlice = 0;
	m_modified.resize(m_mrcFile.getSliceCount());
	m_modifiedFlags = QVector<bool>(m_mrcFile.getSliceCount(), false);
	return true;
}

bool MRCDataModel::openMarks(const QString & fileName)
{
	if (m_mrcFile.isOpened() == false)
		return false;
	//TODO:

	return true;
}

bool MRCDataModel::saveMarks(const QString & fileName,MarkFormat format)
{
	/*
	*Transform the QPicture to images and save the pixel data
	*/
	QVector<QImage> images;
	for (int i = 0; i < m_marks.size();i++) {
		images.push_back(QImage(getWidth(),getHeight(),
			QImage::Format_Grayscale8));
		for (auto & pic : m_marks[i]) {
			QPainter p(&images[i]);
			p.drawPicture(0,0,pic);
		}
	}
	if (format == MarkFormat::MRC) {
		//TODO:
		//Wait the save() member of MRC class to be implemented
		//
		std::cerr << __LINE__;
		return false;
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

QImage MRCDataModel::getSlice(int index) const
{
	if (m_modifiedFlags[index] == false) {
		return getOriginalSlice(index);
	}
	else {
		return m_modified[index];
	}
}

void MRCDataModel::setSlice(const QImage & image, int index)
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

QVector<QImage> MRCDataModel::getSlices() const
{
	QVector<QImage> imgVec;
	int slices = m_mrcFile.getSliceCount();
	for (int i = 0; i < slices; i++) {
		imgVec.push_back(getSlice(i));
	}
	return imgVec;
}

void MRCDataModel::setMark(const QPicture& mark, int index)
{

}

void MRCDataModel::addMark(int slice, const QPicture &mark)
{
   m_marks[slice].push_back(mark);
}

QPicture MRCDataModel::getMark(int index)const
{
	return QPicture();
}

QVector<QPicture> MRCDataModel::getMarks(int slice) const
{
    return m_marks[slice];
}
