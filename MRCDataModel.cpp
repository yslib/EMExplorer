#include "mrcdatamodel.h"
#include "mrc.h"
#include <QDebug>

MRCDataModel::MRCDataModel(const QSharedPointer<MRC> &data):
	AbstractSliceDataModel(data->slice(),data->width(),data->height()),
	m_d(data)
{

}
QImage MRCDataModel::originalTopSlice(int index) const
{
	int width = m_d->width();
	int height = m_d->height();
	unsigned char * d = m_d->data<unsigned char>();
	Q_ASSERT_X(d != nullptr, "ItemContext::getOriginalTopSlice", "type convertion error");
	return QImage(d + index * width*height, width, height, QImage::Format_Grayscale8);
}

QImage MRCDataModel::originalRightSlice(int index) const
{
	int width = m_d->width();
	int height = m_d->height();
	int slice = m_d->slice();
	int size = width * height *slice;
	std::unique_ptr<unsigned char[]> imageBuffer(new unsigned char[slice*height]);
	auto data = m_d->data<unsigned char>();
	Q_ASSERT_X(data != nullptr, "MRCDataModel::originalRightSlice", "type error");
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < slice; j++)
		{
			int idx = index + i * width + j * width*height;
			Q_ASSERT_X(idx < size, "MRCDataModel::originalRightSlice", "size error");
			imageBuffer[j + i * slice] = data[idx];
		}
	}
	return QImage(imageBuffer.get(), slice, height, QImage::Format_Grayscale8).copy();
}

QImage MRCDataModel::originalFrontSlice(int index) const
{
	int width = m_d->width();
	int height = m_d->height();
	int slice = m_d->slice();
	int size = width * height *slice;
	std::unique_ptr<unsigned char[]> imageBuffer(new unsigned char[width*slice]);
	auto data = m_d->data<unsigned char>();
	Q_ASSERT_X(data != nullptr, "MRCDataModel::originalFrontSlice", "type error");
	for (int i = 0; i < slice; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int idx = j + index * width + i * width*height;
			Q_ASSERT_X(idx < size,"MRCDataModel::originalFrontSlice","size error");
			imageBuffer[j + i * width] = data[idx];
		}
	}
	return QImage(imageBuffer.get(), width, slice, QImage::Format_Grayscale8).copy();
}
inline int MRCDataModel::topSliceCount() const
{
	return m_d->slice();
}
inline int MRCDataModel::rightSliceCount() const
{
	return m_d->width();
}

inline int MRCDataModel::frontSliceCount() const
{
	return m_d->height();
}