#include "mrcdatamodel.h"
#include "mrc.h"
#include <QDebug>

#include <cmath>

MRCDataModel::MRCDataModel(const QSharedPointer<MRC> &data):
	AbstractSliceDataModel(data->slice(),data->width(),data->height()),
	m_d(data)
{
	Q_ASSERT_X(m_d->isOpened(), 
		"MRCDataModel::MRCDataModel", "Invalid MRC Data.");

	preCalc();

}
/**
 * \brief  Returns the data type of the internal data
 * 
 * 0 represents the byte or unsigned byte type. 1 represents the float type
 * 2 represents the short integer or unsigned short integer
 * -1 represents the unsupported data type
 */
int MRCDataModel::dataType()
{
	switch (m_d->dataType()) 
	{
		case MRC::DataType::Integer8:return 0;
		case MRC::DataType::Real32:return 1;
		case MRC::DataType::Integer16:return 2;
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

	const size_t width = m_d->width();
	const size_t height = m_d->height();
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
#ifdef _OPENMP
		#pragma omp parallel for
#endif
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
				//qDebug() << dmin << " " << dmax;
				const auto d = m_d->data<MRC::MRCFloat>();
				Q_ASSERT_X(d != nullptr, "MRCDataModel::originalTopSlice", "type convertion error");
				const auto data = d + width * height * index;
#ifdef _OPENMP
#pragma omp parallel for
#endif
				for (auto i = 0; i < height; i++) {
					const auto scanLine = newImage.scanLine(i);
					for (auto j = 0; j < width; j++) {
						const auto idx = i * width + j;
						scanLine[j] = (data[idx] - dmin) / (dmax - dmin) * 255;
					}
				}
			}
			break;
        case MRC::DataType::Integer16:
            {
                const auto dmin = m_d->minValue();
                const auto dmax = m_d->maxValue();
                //qDebug() << dmin << " " << dmax;
                const auto d = m_d->data<MRC::MRCInt16>();
                Q_ASSERT_X(d != nullptr, "MRCDataModel::originalTopSlice", "type convertion error");
                const auto data = d + width * height * index;
#ifdef _OPENMP
#pragma omp parallel for
#endif
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
	const size_t width = m_d->width();
	const size_t height = m_d->height();
	const size_t slice = m_d->slice();
	const auto size = width * height *slice;
	QImage newImage(slice, height, QImage::Format_Grayscale8);


	switch (m_d->dataType()) 
	{
		case MRC::DataType::Integer8:
			{
				const auto data = m_d->data<MRC::MRCUInt8>();
				Q_ASSERT_X(data != nullptr, "MRCDataModel::originalRightSlice", "type error");
#ifdef _OPENMP
#pragma omp parallel for
#endif
                for (std::size_t i = 0; i < height; i++)
				{
					const auto scanLine = newImage.scanLine(i);
                    for (std::size_t j = 0; j < slice; j++)
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
#ifdef _OPENMP
#pragma omp parallel for
#endif
                for (std::size_t i = 0; i < height; i++)
				{
					const auto scanLine = newImage.scanLine(i);
                    for (std::size_t j = 0; j < slice; j++)
					{
						const auto idx = index + i * width + j * width*height;
						Q_ASSERT_X(idx < size, "MRCDataModel::originalRightSlice", "size error");
						scanLine[j] = (data[idx] - dmin) / (dmax - dmin) * 255;
					}
				}
			}
			break;

    case MRC::DataType::Integer16:
    {
                const auto dmin = m_d->minValue();
            const auto dmax = m_d->maxValue();
                const auto data = m_d->data<MRC::MRCInt16>();
                Q_ASSERT_X(data != nullptr, "MRCDataModel::originalRightSlice", "type error");
#ifdef _OPENMP
#pragma omp parallel for
#endif
                for (std::size_t i = 0; i < height; i++)
                {
                    const auto scanLine = newImage.scanLine(i);
                    for (std::size_t j = 0; j < slice; j++)
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
	const size_t width = m_d->width();
	const size_t height = m_d->height();
	const size_t slice = m_d->slice();
	const auto size = width * height *slice;
	QImage newImage(width, slice, QImage::Format_Grayscale8);


	switch (m_d->dataType()) 
	{
		case MRC::DataType::Integer8: 
			{
				const auto data = m_d->data<MRC::MRCUInt8>();
				Q_ASSERT_X(data != nullptr, "MRCDataModel::originalFrontSlice", "type error");
#ifdef _OPENMP
#pragma omp parallel for
#endif
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
#ifdef _OPENMP
#pragma omp parallel for
#endif
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
            case MRC::DataType::Integer16:
            {
                const auto dmin = m_d->minValue();
                const auto dmax = m_d->maxValue();
                const auto data = m_d->data<MRC::MRCInt16>();
                Q_ASSERT_X(data != nullptr, "MRCDataModel::originalFrontSlice", "type error");
#ifdef _OPENMP
#pragma omp parallel for
#endif
                for (std::size_t i = 0; i < slice; i++)
                {
                    const auto scanLine = newImage.scanLine(i);
                    for (std::size_t j = 0; j < width; j++)
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

float MRCDataModel::minValue() const
{
	return m_d->minValue();
}

float MRCDataModel::maxValue() const
{
	return m_d->maxValue();
}

MRCDataModel::~MRCDataModel() 
{
}

void MRCDataModel::adjustImage(QImage& image) const {

	const size_t height = image.height();
	const size_t width = image.width();

	const auto min = m_statistic.mean - 3 * m_statistic.var;
	const auto max = m_statistic.mean + 3 * m_statistic.var;

#ifdef _OPENMP
#pragma omp parallel for
#endif
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



void MRCDataModel::preCalc() 
{

	m_statistic.var = 0.0;
	m_statistic.mean = 0.0;
	const auto count = (m_d->width()) * (m_d->height()) * (m_d->slice());

	// Calculate mean and var
	auto mean = 0.0, var = 0.0;
	switch (m_d->dataType())
	{
		case MRC::DataType::Integer8:
		{
			const auto data = m_d->data<MRC::MRCUInt8>();
#ifdef _OPENMP
#pragma omp parallel for reduction(+:mean)
#endif
			for(auto i = 0;i<count;i++) 
			{
				mean += data[i];
			}

			mean /= count;
#ifdef _OPENMP
#pragma omp parallel for reduction(+:var)
#endif
			for (auto i = 0; i < count; i++) 
			{
				var += (data[i] - mean)*(data[i] - mean);
			}
			
			var = std::sqrt(var / count);
		}
		break;
		case MRC::DataType::Real32:
		{
			const auto dmin = m_d->minValue();
			const auto dmax = m_d->maxValue();
			const auto data = m_d->data<MRC::MRCFloat>();
			Q_ASSERT_X(data != nullptr, "MRCDataModel::originalFrontSlice", "type error");

#ifdef _OPENMP
#pragma omp parallel for reduction(+:mean)
#endif

			for(auto i = 0;i<count;i++) 
			{
				mean += (data[i] - dmin) / (dmax - dmin) * 255;
			}
			mean /= count;

#ifdef _OPENMP
#pragma omp parallel for reduction(+:var)
#endif

			for (auto i = 0; i < count; i++) {
				const auto value = (data[i] - dmin) / (dmax - dmin) * 255;
				var += (value - mean)*(value - mean);
			}
			var = std::sqrt(var / count);
		}
		break;
	}

	m_statistic.mean = mean;
	m_statistic.var = var;
}
