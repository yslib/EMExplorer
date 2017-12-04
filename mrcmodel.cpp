#include "mrcmodel.h"
MRCModel::MRCModel() :m_mrcContext{}
{
}
MRCModel::MRCModel(const QString & fileName):MRCModel()
{
	m_mrcFile.open(fileName);
	m_mrcContext.valid = m_mrcFile.isOpened();
}
MRCModel::~MRCModel()
{

}

bool MRCModel::save(const QString & fileName)
{
	return false;
}

bool MRCModel::open(const QString & fileName)
{
	m_mrcFile.open(fileName);
	m_mrcContext.valid = m_mrcFile.isOpened();
	return isOpened();
}

bool MRCModel::openMarks(const QString & fileName)
{
	return false;
}

bool MRCModel::saveMarks(const QString & fileName)
{
	return false;
}

QImage MRCModel::getSlice(int index) const
{
	int width = m_mrcFile.getWidth();
	int height = m_mrcFile.getHeight();
	return QImage(m_mrcFile.data() + index*width*height, width, height, QImage::Format_Grayscale8);
}

void MRCModel::setSlice(const QImage & image, int index)
{
	if (image.format() != QImage::Format_Grayscale8)
		return;
	int width = image.width();
	int height = image.height();
	if (width != m_mrcFile.getWidth() || height != m_mrcFile.getHeight())
		return ;
	memcpy(m_mrcFile.data(), image.bits(), width*height * sizeof(unsigned char));
	return ;
}

QVector<QImage> MRCModel::getSlices() const
{
	QVector<QImage> imgVec;
	int slices = m_mrcFile.getSliceCount();
	for (int i = 0; i < slices; i++) {
		imgVec.push_back(getSlice(i));
	}
	return imgVec;
}

void MRCModel::setMark(const QImage & image, int index)
{
	if (m_marks.size() == 0)return;
	m_marks[index] = image.copy(QRect());
}

QImage MRCModel::getMark(int index)
{
	if (m_marks.size() == 0) {
		return QImage();
	}
	return m_marks[index];
}

QVector<QImage> MRCModel::getMarks() const
{
	return m_marks;
}
