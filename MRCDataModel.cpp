#include "MRCDataModel.h"
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
	//m_marks.resize(m_mrcFile.getSliceCount());
	return true;
}

bool MRCDataModel::openMarks(const QString & fileName)
{
	if (m_mrcFile.isOpened() == false)
		return false;
	//TODO:

	return true;
}

bool MRCDataModel::saveMarks(const QString & fileName)
{

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
