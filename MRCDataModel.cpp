#include "MRCDataModel.h"
#include <qdebug.h>
MRCDataModel::MRCDataModel() :m_mrcContext{}
{
}
MRCDataModel::MRCDataModel(const QString & fileName):MRCDataModel()
{
	open(fileName);
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
	m_mrcFile.open(fileName);
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
	return false;
}

bool MRCDataModel::saveMarks(const QString & fileName)
{
	return false;
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
	qDebug() << "setSlice " << index;
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

void MRCDataModel::setMark(const QImage & image, int index)
{
	if (m_marks.size() == 0)return;
	m_marks[index] = image.copy(QRect());
}

QImage MRCDataModel::getMark(int index)
{
	if (m_marks.size() == 0) {
		return QImage();
	}
	return m_marks[index];
}

QVector<QImage> MRCDataModel::getMarks() const
{
	return m_marks;
}
