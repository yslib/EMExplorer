#include "mrcdatamodel.h"
#include "mrc.h"
#include <QDebug>

MRCDataModel::MRCDataModel(const QSharedPointer<MRC> &data):
	AbstractSliceDataModel(data->slice(),data->width(),data->height()),
	m_d(data)
{
	Q_ASSERT_X(m_d->isOpened(), 
		"MRCDataModel::MRCDataModel", "Invalid MRC Data.");
}
QImage MRCDataModel::originalTopSlice(int index) const
{
	const auto width = m_d->width();
	const auto height = m_d->height();
	const auto d = m_d->data<MRC::MRCUInt8>();
	Q_ASSERT_X(d != nullptr, "MRCDataModel::originalTopSlice", "type convertion error");

	const auto data = d + width * height * index;

	QImage newImage(width, height, QImage::Format_Grayscale8);

	// For 32-bit aligned requirement of the QImage, 
	// A new image has to be created and copy original data manually.

#pragma omp parallel for
	for(auto i=0;i<height;i++) {
		const auto scanLine = newImage.scanLine(i);
		for(auto j = 0;j<width;j++) {
			const auto idx = i * width + j;
			*(scanLine + j) = *(data + idx);
		}
	}
	
	return newImage;
}

QImage MRCDataModel::originalRightSlice(int index) const
{
	const auto width = m_d->width();
	const auto height = m_d->height();
	const auto slice = m_d->slice();
	const auto size = width * height *slice;
	//std::unique_ptr<unsigned char[]> imageBuffer(new unsigned char[slice*height]);
	const auto data = m_d->data<MRC::MRCUInt8>();
	Q_ASSERT_X(data != nullptr, "MRCDataModel::originalRightSlice", "type error");

	QImage newImage(slice, height, QImage::Format_Grayscale8);

#pragma omp parallel for

	for (auto i = 0; i < height; i++)
	{
		const auto scanLine = newImage.scanLine(i);
		for (auto j = 0; j < slice; j++)
		{
			const auto idx = index + i * width + j * width*height;
			Q_ASSERT_X(idx < size, "MRCDataModel::originalRightSlice", "size error");
			//imageBuffer[j + i * slice] = data[idx];

			scanLine[j] = data[idx];
		}
	}

	//return QImage(imageBuffer.get(), slice, height, QImage::Format_Grayscale8).copy();
	return newImage;
}

QImage MRCDataModel::originalFrontSlice(int index) const
{
	const auto  width = m_d->width();
	const auto height = m_d->height();
	const auto slice = m_d->slice();
	const auto size = width * height *slice;
	//std::unique_ptr<unsigned char[]> imageBuffer(new unsigned char[width*slice]);
	const auto data = m_d->data<MRC::MRCUInt8>();
	Q_ASSERT_X(data != nullptr, "MRCDataModel::originalFrontSlice", "type error");

	QImage newImage(width,slice, QImage::Format_Grayscale8);

#pragma omp parallel for
	for (auto i = 0; i < slice; i++)
	{
		const auto scanLine = newImage.scanLine(i);
		for (auto j = 0; j < width; j++)
		{
			const auto idx = j + index * width + i * width*height;
			Q_ASSERT_X(idx < size,"MRCDataModel::originalFrontSlice","size error");
			//imageBuffer[j + i * width] = data[idx];
			scanLine[j] = data[idx];
		}
	}
	//return QImage(imageBuffer.get(), width, slice, QImage::Format_Grayscale8).copy();
	return newImage;
}

unsigned char* MRCDataModel::data()
{
	return const_cast<unsigned char*>(constData());
}

const unsigned char* MRCDataModel::constData() const
{
	//TODO:: There may be a bug.
	//Only surport byte data output
	return reinterpret_cast<unsigned char*>(m_d->data<MRC::MRCInt8>());
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