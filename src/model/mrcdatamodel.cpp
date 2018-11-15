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
/**
 * \brief  Returns the data type of the internal data
 * 
 * 0 represents the byte or unsigned byte type. 1 represents the float type
 * -1 represents the unsupported data type
 */
int MRCDataModel::dataType()
{
	switch (m_d->dataType()) 
	{
		case MRC::DataType::Integer8:return 0;
		case MRC::DataType::Real32:return 1;
		default:return -1;
	}
}

void * MRCDataModel::rawData()
{
	return  const_cast<void*>(constRawData());
}

const void * MRCDataModel::constRawData()
{
	return m_d->data<void>();
}



QImage MRCDataModel::originalTopSlice(int index) const
{
	const auto width = m_d->width();
	const auto height = m_d->height();
	QImage newImage(width, height, QImage::Format_Grayscale8);

	// For 32-bit aligned requirement of the QImage, 
	// A new image has to be created and copy original data manually.

	switch(m_d->dataType()) 
	{
		case MRC::DataType::Integer8: 
			{
				const auto d = m_d->data<MRC::MRCUInt8>();
				Q_ASSERT_X(d != nullptr, "MRCDataModel::originalTopSlice", "type convertion error");
				const auto data = d + width * height * index;
		#pragma omp parallel for
				for (auto i = 0; i < height; i++) {
					const auto scanLine = newImage.scanLine(i);
					for (auto j = 0; j < width; j++) {
						const auto idx = i * width + j;
						*(scanLine + j) = *(data + idx);
					}
				}
			}
			break;
		case MRC::DataType::Real32: 
			{
				const auto dmin = m_d->minValue();
				const auto dmax = m_d->maxValue();
				const auto d = m_d->data<MRC::MRCFloat>();
				Q_ASSERT_X(d != nullptr, "MRCDataModel::originalTopSlice", "type convertion error");
				const auto data = d + width * height * index;
		#pragma omp parallel for
				for (auto i = 0; i < height; i++) {
					const auto scanLine = newImage.scanLine(i);
					for (auto j = 0; j < width; j++) {
						const auto idx = i * width + j;
						scanLine[j] = (data[idx] - dmin) / (dmax - dmin) * 255;
					}
				}
			}
			break;
	}
	adjustImage(newImage);
	return newImage;
}

QImage MRCDataModel::originalRightSlice(int index) const
{
	const auto width = m_d->width();
	const auto height = m_d->height();
	const auto slice = m_d->slice();
	const auto size = width * height *slice;
	QImage newImage(slice, height, QImage::Format_Grayscale8);


	switch (m_d->dataType()) 
	{
		case MRC::DataType::Integer8:
			{
				const auto data = m_d->data<MRC::MRCUInt8>();
				Q_ASSERT_X(data != nullptr, "MRCDataModel::originalRightSlice", "type error");
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
			}
			break;
		case MRC::DataType::Real32: 
			{
			const auto dmin = m_d->minValue();
			const auto dmax = m_d->maxValue();
				const auto data = m_d->data<MRC::MRCFloat>();
				Q_ASSERT_X(data != nullptr, "MRCDataModel::originalRightSlice", "type error");
	#pragma omp parallel for
				for (auto i = 0; i < height; i++)
				{
					const auto scanLine = newImage.scanLine(i);
					for (auto j = 0; j < slice; j++)
					{
						const auto idx = index + i * width + j * width*height;
						Q_ASSERT_X(idx < size, "MRCDataModel::originalRightSlice", "size error");
						scanLine[j] = (data[idx] - dmin) / (dmax - dmin) * 255;
					}
				}
			}
			break;
	}
	adjustImage(newImage);
	return newImage;
}

QImage MRCDataModel::originalFrontSlice(int index) const
{
	const auto  width = m_d->width();
	const auto height = m_d->height();
	const auto slice = m_d->slice();
	const auto size = width * height *slice;
	QImage newImage(width, slice, QImage::Format_Grayscale8);


	switch (m_d->dataType()) 
	{
		case MRC::DataType::Integer8: 
			{
				const auto data = m_d->data<MRC::MRCUInt8>();
				Q_ASSERT_X(data != nullptr, "MRCDataModel::originalFrontSlice", "type error");
	#pragma omp parallel for
				for (auto i = 0; i < slice; i++)
				{
					const auto scanLine = newImage.scanLine(i);
					for (auto j = 0; j < width; j++)
					{
						const auto idx = j + index * width + i * width*height;
						Q_ASSERT_X(idx < size, "MRCDataModel::originalFrontSlice", "size error");
						//imageBuffer[j + i * width] = data[idx];
						scanLine[j] = data[idx];
					}
				}
			}
			break;
		case MRC::DataType::Real32: 
			{
				const auto dmin = m_d->minValue();
				const auto dmax = m_d->maxValue();
				const auto data = m_d->data<MRC::MRCFloat>();
				Q_ASSERT_X(data != nullptr, "MRCDataModel::originalFrontSlice", "type error");
	#pragma omp parallel for
				for (auto i = 0; i < slice; i++)
				{
					const auto scanLine = newImage.scanLine(i);
					for (auto j = 0; j < width; j++)
					{
						const auto idx = j + index * width + i * width*height;
						Q_ASSERT_X(idx < size, "MRCDataModel::originalFrontSlice", "size error");
						//imageBuffer[j + i * width] = data[idx];
						scanLine[j] = (data[idx] - dmin) / (dmax - dmin) * 255;
					}
				}
			}
			break;
	}
	adjustImage(newImage);
	return newImage;
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

void MRCDataModel::adjustImage(QImage& image) const {

	const auto  height = image.height();
	const auto width = image.width();

	auto sum = 0.0;
#pragma omp parallel for
	for (auto i = 0; i < height; i++)
	{
		const auto scanLine = image.scanLine(i);
		for (auto j = 0; j < width; j++)
		{
			sum += scanLine[j];
		}
	}

	const auto mean = sum / (width*height);

	auto var = 0;
#pragma omp parallel for
	for (auto i = 0; i < height; i++)
	{
		const auto scanLine = image.scanLine(i);
		for (auto j = 0; j < width; j++)
		{
			var += (scanLine[j] - mean)*(scanLine[j]-mean);
		}
	}
	var = std::sqrt(var/(width*height));

	const auto min = mean - 3 * var;
	const auto max = mean + 3 * var;

#pragma omp parallel for
	for (auto i = 0; i < height; i++)
	{
		const auto scanLine = image.scanLine(i);
		for (auto j = 0; j < width; j++)
		{
			const auto v = (scanLine[j] - min) / (max - min) * 255.0;
			scanLine[j] = v < 0 ? 0 : (v > 255 ? 255 : v);
		}
	}
}
