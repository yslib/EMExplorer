#include "mrcdatamodel.h"
#include "mrc.h"
#include <QDebug>

#include <cmath>
#define cimg_display 0 //
#define cimg_OS 0
#include "algorithm/CImg.h"
MRCDataModel::MRCDataModel(const QSharedPointer<MRC> &data) :
	AbstractSliceDataModel(data->slice(), data->width(), data->height()),
	m_d(data)
{
	Q_ASSERT_X(m_d->isOpened(),
		"MRCDataModel::MRCDataModel", "Invalid MRC Data.");
	preCalc();
	calGradient();
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

	switch (m_d->dataType())
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
		for (auto i = 0; i < height; i++)
		{
			const auto scanLine = newImage.scanLine(i);
			for (auto j = 0; j < slice; j++)
			{
				const auto idx = index + i * width + j * width*height;
				Q_ASSERT_X(idx < size, "MRCDataModel::originalRightSlice", "size error");
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

	case MRC::DataType::Integer16:
	{
		const auto dmin = m_d->minValue();
		const auto dmax = m_d->maxValue();
		const auto data = m_d->data<MRC::MRCInt16>();
		Q_ASSERT_X(data != nullptr, "MRCDataModel::originalRightSlice", "type error");
#ifdef _OPENMP
#pragma omp parallel for
#endif
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
		for (auto i = 0; i < slice; i++)
		{
			const auto scanLine = newImage.scanLine(i);
			for (auto j = 0; j < width; j++)
			{
				const auto idx = j + index * width + i * width*height;
				Q_ASSERT_X(idx < size, "MRCDataModel::originalFrontSlice", "size error");
				scanLine[j] = (data[idx] - dmin) / (dmax - dmin) * 255;
			}
		}
	}


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

void MRCDataModel::calGradient()
{
	m_d_gradient.clear();

	//当前仅计算二维梯度
	for(auto i= 0;i<m_d->slice();i++)
	{
		QImage currentslice = originalTopSlice(i);
		/*QImage lastslice = originalTopSlice(i - 1);
		QImage nextslice = originalTopSlice(i + 1);*/
		
		int width = currentslice.width();
		int height = currentslice.height();

		QVector<QVector<int>> temp_gradient = QVector<QVector<int>>(height, QVector<int>(width, 255));

		
		float maxGradient = 0.0;

		cimg_library::CImg<unsigned char> imageHelper(
			currentslice.bits(),
			currentslice.bytesPerLine(),			// QImage requires 32-bit aligned for each scanLine, but CImg don't.
			height,
			1,
			true);							// Share data

		const auto mean = imageHelper.mean();

		//调整对比度
#ifdef _OPENMP
#pragma omp parallel for
#endif	
		for (auto h = 0; h < height; ++h) {
			const auto scanLine = currentslice.scanLine(h);
			for (auto w = 0; w < width; ++w) {
				auto t = scanLine[w] - mean;
				t *= 3; //Adjust contrast
				t += mean * 1; // Adjust brightness
				scanLine[w] = (t > 255.0) ? (255) : (t < 0.0 ? (0) : (t));
			}
		}

#ifdef _OPENMP
#pragma omp parallel for
#endif			
		for (auto i = 1; i < width - 1; i++)
		{
			for (auto j = 1; j < height - 1; j++)
			{
				/*
				p1 p2 p3
				p4 p5 p6
				p7 p8 p9
				*/
				auto p1 = qGray(currentslice.pixel(i - 1, j - 1));
				auto p2 = qGray(currentslice.pixel(i, j - 1));
				auto p3 = qGray(currentslice.pixel(i + 1, j - 1));

				auto p4 = qGray(currentslice.pixel(i - 1, j));
				auto p5 = qGray(currentslice.pixel(i, j));
				auto p6 = qGray(currentslice.pixel(i + 1, j));

				auto p7 = qGray(currentslice.pixel(i - 1, j + 1));
				auto p8 = qGray(currentslice.pixel(i, j + 1));
				auto p9 = qGray(currentslice.pixel(i + 1, j + 1));

				auto gradient_x = p3 + 2 * p6 + p9 - p1 - 2 * p4 - p7;
				auto gradient_y = p7 + 2 * p8 + p9 - p1 - 2 * p2 - p3;

				auto gradient = (abs(gradient_x) + abs(gradient_y));

				//auto laplacian = p2 + p4 + p6 + p8 - 4 * p5;
				temp_gradient[i][j] = gradient;

				if (gradient > maxGradient) maxGradient = gradient;
			}
		}

#ifdef _OPENMP
#pragma omp parallel for
#endif	
		for (auto i = 1; i < width - 1; i++)
		{
			for (auto j = 1; j < height - 1; j++)
			{
				auto gradient = float(1 - temp_gradient[i][j] / maxGradient) * 255;//inverse
				temp_gradient[i][j] = gradient; //映射到0-255区间
				//QRgb grayPixel = qRgb(gradient, gradient, gradient);
				//m_gradientImage->setPixel(i, j, grayPixel);
			}
		}
		m_d_gradient.append(temp_gradient);
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
		for (auto i = 0; i < count; i++)
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

		for (auto i = 0; i < count; i++)
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


	case MRC::DataType::Integer16:
	{
		const auto dmin = m_d->minValue();
		const auto dmax = m_d->maxValue();
		const auto data = m_d->data<MRC::MRCInt16>();
		Q_ASSERT_X(data != nullptr, "MRCDataModel::originalFrontSlice", "type error");

#ifdef _OPENMP
#pragma omp parallel for reduction(+:mean)
#endif

		for (auto i = 0; i < count; i++)
		{
			mean += (data[i] - dmin) / (dmax - dmin) * 255;
		}
		mean /= count;

#ifdef _OPENMP
#pragma omp parallel for reduction(+:var)
#endif

		for (auto i = 0; i < count; i++)
		{
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
