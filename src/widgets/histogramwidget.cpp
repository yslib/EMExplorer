#include <qdebug.h>

#include <QComboBox>

#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <cmath>


#include "histogramwidget.h"
#include "model/sliceitem.h"
#include "widgets/sliceeditorwidget.h"
#include "widgets/doubleslider.h"


#define cimg_display 0 //
#define cimg_OS 0
#include "algorithm/CImg.h"


Histogram::Histogram(QWidget *parent) :QWidget(parent),
m_hist{ QVector<double>(BIN_COUNT) },
m_minValue{ 0 },
m_maxValue{ BIN_COUNT - 1 },
m_count{ 0 },
m_histUpdate(false),
m_mousePressed{ false },
m_rightCursorSelected{ false },
m_leftCursorSelected{ false },
m_cursorEnable{ true },
xRange(QVector2D(0.f, 1.f)),
yRange(QVector2D(0.f, 1.f)),
padding(12),
arrowLength(10),
arrowWidth(3),       ///< width of the arrows at the end of coordinate axes
xAxisText("pixel"),    ///< caption of the x axis
yAxisText("density"),     ///< caption of the y axis
gridSpacing(QVector2D(1.0,1.0))
{
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	//setMinimumSize(MIN_WIDTH,MIN_HEIGHT);
	//resize(MIN_WIDTH,MIN_HEIGHT);
}

Histogram::Histogram(QWidget *parent, const QImage &image) :Histogram(parent)
{
	setImage(image);
}

void Histogram::setImage(const QImage &image)
{
	if (image.depth() != 8)
	{
		qWarning("Only Support 8bit image.");
		return;
	}
	m_hist.clear();
	m_hist.resize(BIN_COUNT);
	m_count = image.width()*image.height();
	const unsigned char * data = image.bits();

	m_maxGrayValue = 0;
	for (auto h = 0; h < image.height(); h++) {
		for (auto w = 0; w < image.width(); w++) {
			int g = *(image.constScanLine(h) + w);
			if (m_maxGrayValue < g) {
				m_maxGrayValue = g;
			}
			m_hist[g]++;
		}
	}
	m_histUpdate = true;
	update();
	updateGeometry();
}

QVector<double> Histogram::getHist() const
{
	return m_hist;
}

QSize Histogram::sizeHint() const {
	return QSize(300, 150);
}

//QSize Histogram::sizeHint() const
//{
//    return m_histImage.size();
//}

void Histogram::setLeftCursorValue(int value)
{
	if (value < 0) {
		value = 0;
	}
	else if (value > 255) {
		value = 255;
	}
	if (value > m_maxValue) {
		m_maxValue = m_minValue = value;
		emit minValueChanged(value);
		emit maxValueChanged(value);
	}
	else {
		m_minValue = value;
		emit minValueChanged(value);
	}
	update();
	updateGeometry();
}

void Histogram::setRightCursorValue(int value)
{
	if (value < 0) {
		value = 0;
	}
	else if (value > 255) {
		value = 255;
	}
	if (value < m_minValue) {
		m_minValue = m_maxValue = value;
		emit minValueChanged(value);
		emit maxValueChanged(value);
	}
	else {
		m_maxValue = value;
		emit maxValueChanged(value);
	}
	//update will emit signal
	update();
	updateGeometry();
}

int Histogram::getMinimumCursorValue() const
{
	return m_minValue;
}

int Histogram::getMaximumCursorValue() const
{
	return m_maxValue;
}

int Histogram::getBinCount() const
{
	return BIN_COUNT;
}

void Histogram::setDragEnable(bool enable)
{
	m_cursorEnable = enable;
}



void Histogram::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

	//QImage image(size(),QImage::Format_ARGB32_Premultiplied);
	//QPainter imagePainter(&image);
	//imagePainter.initFrom(this);
	//imagePainter.setRenderHint(QPainter::Antialiasing,true);
	//imagePainter.fillRect(rect(),QBrush(Qt::white));

	//qreal height = static_cast<qreal>(image.height());
	//qreal width = static_cast<qreal>(image.width());
	//qreal binWidth = width/BIN_COUNT;

	////Drawing histogram
	//if(m_count != 0){
	//    imagePainter.setPen(QColor(0,0,0));
	//    imagePainter.setBrush(QBrush(QColor(0,0,0)));

	   // QVarLengthArray<qreal, 256> f(256);
	//    for(int i=0;i<BIN_COUNT;i++){
	//        f[i] = ((static_cast<double>(m_hist[i])/static_cast<double>(m_count)));
	//    }
	   // qreal *fmax = std::max_element(f.begin(), f.end());
	   // qreal mag = 0.7 / *fmax;		//the height of the max bin is set as 0.7 of height of the widget/  
	   // for(int i=0;i<BIN_COUNT;i++)
	   // {
		  //  int binHeight = f[i] * height*mag;
		  //  imagePainter.drawRect(QRectF
		  //  (QPointF(i*binWidth, height - binHeight), QSize(binWidth, binHeight))
		  //  );
	   // }
	//}

	////Drawing lower bound and upper bound lines
	//imagePainter.setPen(QColor(255,0,0));
	//imagePainter.setBrush(QBrush(QColor(255,0,0)));

	//qreal lowerBoundLineX = m_minValue*binWidth+binWidth/2;
	//qreal upperBoundLineX = m_maxValue*binWidth+binWidth/2;
	//imagePainter.drawLine(QPointF(lowerBoundLineX,0),QPointF(lowerBoundLineX,height));
	//imagePainter.drawLine(QPointF(upperBoundLineX,0),QPointF(upperBoundLineX,height));

	//imagePainter.end();
	//QPainter widgetPainter(this);
	//widgetPainter.drawImage(0,0,image);

	// draw grid
	QPainter paint(this);

	QMatrix m;
	m.translate(0.0, static_cast<float>(height()) - 1);
	m.scale(1.f, -1.f);
	paint.setMatrix(m);

	paint.setMatrixEnabled(true);
	paint.setRenderHint(QPainter::Antialiasing, false);
	paint.setPen(Qt::NoPen);
	paint.setBrush(Qt::white);
	paint.drawRect(0, 0, width() - 1, height() - 1);

	paint.setPen(QColor(220, 220, 220));
	paint.setRenderHint(QPainter::Antialiasing, false);

	QVector2D pmin = QVector2D(0.f, 0.f);
	QVector2D pmax = QVector2D(1.f, 1.f);

	for (float f = pmin.x(); f < pmax.x() + gridSpacing.x()*0.5; f += gridSpacing.x()) {
		QVector2D p = wtos(QVector2D(f, 0.f));
		QVector2D a = wtos(QVector2D(0.f, 0.f));
		QVector2D b = wtos(QVector2D(0.f, 1.f));
		paint.drawLine(QPointF(p.x(), a.y()),
			QPointF(p.x(), b.y()));
	}

	for (float f = pmin.y(); f < pmax.y() + gridSpacing.y()*0.5; f += gridSpacing.y()) {
		QVector2D p = wtos(QVector2D(0.f, f));
		QVector2D a = wtos(QVector2D(0.f, 0.f));
		QVector2D b = wtos(QVector2D(1.f, 0.f));
		paint.drawLine(QPointF(a.x(), p.y()),
			QPointF(b.x(), p.y()));
	}

	// draw x and y axes
	paint.setRenderHint(QPainter::Antialiasing, true);
	paint.setPen(Qt::gray);
	paint.setBrush(Qt::gray);

	// draw axes independently from visible range
	float oldx0 = xRange[0];
	float oldx1 = xRange[1];
	xRange[0] = 0.f;
	xRange[1] = 1.f;

	QVector2D origin = wtos(QVector2D(0.f, 0.f));
	origin.setX(std::floor(origin.x()) + 0.5f);
	origin.setY(std::floor(origin.y()) + 0.5f);

	paint.setRenderHint(QPainter::Antialiasing, true);

	paint.drawLine(QPointF(padding, origin.y()),
		QPointF(width() - padding, origin.y()));

	paint.drawLine(QPointF(origin.x(), padding),
		QPointF(origin.x(), height() - padding));

	QPointF arrow[3];
	arrow[0] = QPointF(origin.x(), height() - padding);
	arrow[1] = QPointF(origin.x() + arrowWidth, height() - padding - arrowLength);
	arrow[2] = QPointF(origin.x() - arrowWidth, height() - padding - arrowLength);

	paint.drawConvexPolygon(arrow, 3);

	arrow[0] = QPointF(width() - padding, origin.y());
	arrow[1] = QPointF(width() - padding - arrowLength, origin.y() - arrowWidth);
	arrow[2] = QPointF(width() - padding - arrowLength, origin.y() + arrowWidth);

	paint.drawConvexPolygon(arrow, 3);

	paint.scale(-1.f, 1.f);
	paint.rotate(180.f);
	paint.drawText(static_cast<int>(width() - 6.2f * padding), static_cast<int>(-1 * (origin.y() - 0.8f * padding)), xAxisText);
	paint.drawText(static_cast<int>(1.6f * padding), static_cast<int>(-1 * (height() - 1.85f * padding)), yAxisText);

	paint.rotate(180.f);
	paint.scale(-1.f, 1.f);

	xRange[0] = oldx0;
	xRange[1] = oldx1;


	QPainter p(this);
	drawHist(&p);
}

void Histogram::mouseMoveEvent(QMouseEvent *event)
{
	if (m_mousePressed == true) {
		if (m_rightCursorSelected == true) {
			int value = static_cast<qreal>(event->pos().x()) / width()*BIN_COUNT;
			m_maxValue = value < m_minValue ? m_minValue : value;
			emit maxValueChanged(m_maxValue);
		}
		else if (m_leftCursorSelected == true) {
			int value = static_cast<qreal>(event->pos().x()) / width()*BIN_COUNT;
			m_minValue = value > m_maxValue ? m_maxValue : value;
			emit minValueChanged(m_minValue);
		}
		update();
		updateGeometry();
	}
}

void Histogram::mousePressEvent(QMouseEvent * event)
{
	m_mousePressed = true;
	if (m_cursorEnable == true) {
		qreal x = event->pos().x();
		qreal dl = std::abs(x - getXofLeftCursor());
		qreal dr = std::abs(x - getXofRightCursor());
		if (dl < dr && dl < 5.0) {
			m_leftCursorSelected = true;
		}
		else if (dr < dl && dr < 5.0) {
			m_rightCursorSelected = true;
		}
	}
}

void Histogram::mouseReleaseEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
	if (m_mousePressed == true)
		m_mousePressed = false;
	m_leftCursorSelected = false;
	m_rightCursorSelected = false;
}

void Histogram::drawHist(QPainter* painter) {
	//return;

	if (cache.isNull() || cache.rect() != rect() || m_histUpdate) //{
	{
		
		cache = QPixmap(rect().size());
		cache.fill(Qt::transparent);
		QPainter paint(&cache);
		// put origin in lower lefthand corner
		QMatrix m;
		m.translate(0.0, static_cast<float>(height()) - 1);
		m.scale(1.f, -1.f);
		paint.setMatrix(m);
		paint.setMatrixEnabled(true);

		// draw histogram
		paint.setPen(Qt::NoPen);
		paint.setBrush(QColor(200, 0, 0, 120));
		paint.setRenderHint(QPainter::Antialiasing, true);

		//TODO::
		const auto histogramWidth = 256;

		double logMaxValue = std::log(m_maxGrayValue);
		double * histogram = m_hist.data();

		QVector2D p;
		QPointF* points = new QPointF[histogramWidth + 2];
		int count = 0;

		for (int x = 0; x < histogramWidth; ++x) {
			float xpos = static_cast<float>(x) / histogramWidth;
			// Do some simple clipping here, as the automatic clipping of drawPolygon()
			// gets very slow if lots of polygons have to be clipped away, e.g. when
			// zooming to small part of the histogram.
			if (xpos >= xRange[0] && xpos <= xRange[1]) {
				float value = histogram[x] > 0 ? log(histogram[x]) / logMaxValue : 0;
				if (value > 1) value = 1;
				p = wtos(QVector2D(xpos, value * (yRange[1] - yRange[0]) + yRange[0]));

				// optimization: if the y-coord has not changed from the two last points
				// then just update the last point's x-coord to the current one
				if ((count >= 2) && (points[count - 2].ry() == p.y()) && (points[count - 1].ry() == p.y()) && (count >= 2)) {
					points[count - 1].rx() = p.x();
				}
				else {
					points[count].rx() = p.x();
					points[count].ry() = p.y();
					count++;
				}
			}
		}

		// Qt can't handle polygons that have more than 65536 points
		// so we have to split the polygon
		bool needSplit = false;
		if (count > 65536 - 2) { // 16 bit dataset
			needSplit = true;
			count = 65536 - 2; // 2 points needed for closing the polygon
		}

		if (count > 0) {
			// move x coordinate of first and last points to prevent vertical holes caused
			// by clipping
			points[0].rx() = wtos(QVector2D(xRange[0], 0.f)).x();
			if (count < histogramWidth - 2) // only when last point was actually clipped
				points[count - 1].rx() = wtos(QVector2D(xRange[1], 0.f)).x();

			// needed for a closed polygon
			p = wtos(QVector2D(0.f, yRange[0]));
			points[count].rx() = points[count - 1].rx();
			points[count].ry() = p.y();
			count++;
			p = wtos(QVector2D(0.f, yRange[0]));
			points[count].rx() = points[0].rx();
			points[count].ry() = p.y();
			count++;

			paint.drawPolygon(points, count);
		}

		// draw last points when splitting is needed
		if (needSplit && false) {
			delete[] points;
			points = new QPointF[5];
			count = 0;
			for (int x = histogramWidth - 2; x < histogramWidth; ++x) {
				float xpos = static_cast<float>(x) / histogramWidth;
				if (xpos >= xRange[0] && xpos <= xRange[1]) {
					float value = log(histogram[x]) / logMaxValue;
					if (value > 1) value = 1;
					p = wtos(QVector2D(xpos, value * (yRange[1] - yRange[0]) + yRange[0]));
					points[x - histogramWidth + 3].rx() = p.x();
					points[x - histogramWidth + 3].ry() = p.y();
					count++;
				}
			}
			if (count > 0) {
				// move x coordinate of last point to prevent vertical holes caused by clipping
				points[count - 1].rx() = wtos(QVector2D(xRange[1], 0.f)).x();

				// needed for a closed polygon
				p = wtos(QVector2D(0.f, yRange[0]));
				points[count].rx() = points[count - 1].rx();
				points[count].ry() = p.y();
				count++;
				p = wtos(QVector2D(0, yRange[0]));
				points[count].rx() = points[0].rx();
				points[count].ry() = p.y();
				count++;

				paint.drawPolygon(points, 5);
			}
		}
		delete[] points;
	}
	m_histUpdate = false;
	painter->drawPixmap(0, 0, cache);
}

QVector2D Histogram::wtos(const QVector2D& p) {
	float sx = (p.x() - xRange[0]) / (xRange[1] - xRange[0]) * (static_cast<float>(width()) - 2 * padding - 1.5 * arrowLength) + padding;
	float sy = (p.y() - yRange[0]) / (yRange[1] - yRange[0]) * (static_cast<float>(height()) - 2 * padding - 1.5 * arrowLength) + padding;
	return QVector2D(sx, sy);
}

qreal Histogram::getXofLeftCursor()
{
	qreal binWidth = static_cast<qreal>(size().width()) / BIN_COUNT;
	qreal x = m_minValue * binWidth + binWidth / 2;
	return x;
}

qreal Histogram::getXofRightCursor()
{
	qreal binWidth = static_cast<qreal>(size().width()) / BIN_COUNT;
	qreal x = m_maxValue * binWidth + binWidth / 2;
	return x;
}

BrightnessContrastControlWidget::BrightnessContrastControlWidget(QWidget* parent) :QWidget(parent)
{
	m_contrastFactor = new TitledSliderWidthDoubleSpinBox(QStringLiteral("Contrast"), Qt::Horizontal, this);
	m_contrastFactor->setRange(0, 5);
	m_contrastFactor->setSingleStep(0.1);
	m_contrastFactor->setValue(1.0);
	connect(m_contrastFactor, &TitledSliderWidthDoubleSpinBox::valueChanged, [this](double value) {emit valueChanged(); });
	m_brightnessFactor = new TitledSliderWidthDoubleSpinBox(QStringLiteral("Brightness"), Qt::Horizontal, this);
	m_brightnessFactor->setRange(0, 5);
	m_brightnessFactor->setSingleStep(0.1);
	m_brightnessFactor->setValue(1.0);
	connect(m_brightnessFactor, &TitledSliderWidthDoubleSpinBox::valueChanged, [this](double value) {emit valueChanged(); });

	auto group = new QGroupBox(QStringLiteral("Brightness and Contrast"));
	auto layout = new QVBoxLayout;
	layout->addWidget(m_contrastFactor);
	layout->addWidget(m_brightnessFactor);
	layout->addStretch(1);
	group->setLayout(layout);

	auto mainLayout = new QVBoxLayout;
	mainLayout->addWidget(group);
	setLayout(mainLayout);

	blockSignals(true);
	reset();
	blockSignals(false);
}

double BrightnessContrastControlWidget::contrast() const { return m_contrastFactor->value(); }
double BrightnessContrastControlWidget::brightness() const { return m_brightnessFactor->value(); }
void BrightnessContrastControlWidget::reset() {
	m_contrastFactor->setValue(1.0);
	m_brightnessFactor->setValue(1.0);
}

/**
 * \brief Constructs a widgets used for image processing for certain
 * \a type type of slice in \a sliceEditor
 *
 * \sa SliceEditorWidget
 */
HistogramWidget::HistogramWidget(SliceType type, SliceEditorWidget * sliceEditor, QWidget * parent) :
	AbstractSliceViewPlugin(type, sliceEditor, parent)
{
	createWidgets();
	init();
	setEnabled(sliceItem(sliceType()) != nullptr);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
}

void HistogramWidget::onMinValueChanged(int value)
{
	Q_UNUSED(value);
	histEqualizeImage();
}

void HistogramWidget::onMaxValueChanged(int value)
{
	Q_UNUSED(value);
	histEqualizeImage();
}
void HistogramWidget::resetOriginalImage()
{

	//m_maxSlider->blockSignals(old);
	//SliceItem * item = sliceItem(sliceType());
	//Q_ASSERT_X(item, "HistogramViewer::reset", "null pointer");

	const auto curIndex = currentIndex(sliceType());
	const auto origin = originalImage(sliceType(), curIndex);
	
	// reset slice view with original image
	//item->setPixmap(QPixmap::fromImage(origin));
	// reset histogram with original image
	//m_hist->setImage(origin);
	m_preview->setImage(origin);
	//setImage(sliceType(), curIndex, origin);
	initWidgets();
	m_result = origin;
	
}

void HistogramWidget::filterImage()
{
	const auto text = m_filterWidget->text();

	//int currentIndex = m_ptr->getCurrentSliceIndex();
	//QImage slice = m_ptr->getOriginalTopSlice(currentIndex);

	//m_result = currentImage(sliceType()).copy();


	const auto width = m_result.width();
	const auto height = m_result.height();
	//slice.data_ptr();
	Q_ASSERT_X(m_result.depth() == 8, "HistogramViewer::onFilterButton", "Only support 8-bit image.");
	cimg_library::CImg<unsigned char> image(m_result.bits(), width, height, 1, 1, true);
	//filter parameters
	if (text == "Median Filter") {
		int kernelSize = m_filterWidget->medianKernelSize();
		image.blur_median(kernelSize);
	}
	else if (text == "Gaussian Filter") {
		double sigX = m_filterWidget->sigmaX();
		double sigY = m_filterWidget->sigmaY();
		image.blur(sigX, sigY, 0, true, true);
	}
	//m_internalUpdate = true;
	//m_ptr->setSlice(slice,currentIndex,SliceType::SliceZ);
	//m_model->setData(getDataIndex(m_modelIndex),QVariant::fromValue(m_ptr));
	//SliceItem * item = sliceItem(sliceType());
	//Q_ASSERT_X(item, "HistogramViewer::filterImage", "null pointer");

	//item->setPixmap(QPixmap::fromImage(slice));
	//qDebug() << "Current Index:" << currentIndex(SliceType::Top);
	setCurrentImage(sliceType(),m_result);

	m_preview->setImage(m_result);
}


void HistogramWidget::updateDataModel() {

	init();
}

void HistogramWidget::initWidgets()
{
	m_filterWidget->reset();
	m_brightWidget->reset();
	m_filterWidget->reset();
}

void HistogramWidget::init() {
	// reset all widgets and get image to draw histogram
	initWidgets();
	SliceItem * item = sliceItem(sliceType());
	const auto curImg = currentImage(sliceType());
	item->setPixmap(QPixmap::fromImage(curImg));
	m_result = curImg;
	m_preview->setImage(curImg);
}

void HistogramWidget::histEqualizeImage()
{
	const auto minValue = m_equliWidget->minValue();
	const auto maxValue = m_equliWidget->maxValue();
	//update min and max value

	auto oriImage = m_result.copy();//originalImage(sliceType(), currentIndex(sliceType())).copy();
	//QImage oriImage = currentImage(sliceType());

	Q_ASSERT_X(oriImage.isNull() == false, "HistogramViewer::updateImage", "null image");
	unsigned char *image = oriImage.bits();
	const auto width = oriImage.width();
	const auto height = oriImage.height();

	//memory buffer is shared between CImg and QImage
	cimg_library::CImg<unsigned char> equalizedImage(image, width, height, 1, 1, true);
	equalizedImage.equalize(m_equliWidget->equalizationLevel(), minValue, maxValue);

	m_preview->setImage(oriImage);			//Note:: this variable is no longer the original image.
	//SliceItem * item = sliceItem(sliceType());
	//Q_ASSERT_X(item, "HistogramViewer::filterImage", "null pointer");
	//qDebug() << item->pos();
	//item->setPixmap(QPixmap::fromImage(oriImage));
	//qDebug() << item->pos();
	//setCurrentImage(sliceType(), oriImage);
}

void HistogramWidget::updateContrastAndBrightness()
{

	const auto brightness = m_brightWidget->brightness();
	const auto contrast = m_brightWidget->contrast();

	auto image = m_result.copy();//originalImage(sliceType(), currentIndex(sliceType())).copy();		// There need to use copy(), think why.
	//auto image = currentImage(sliceType());
	const auto width = image.width();
	const auto height = image.height();
	//unsigned char * imageData = image.bits();


	cimg_library::CImg<unsigned char> imageHelper(
		image.bits(),
		image.bytesPerLine(),			// QImage requires 32-bit aligned for each scanLine, but CImg don't.
		height,
		1,
		true);							// Share data

	const auto mean = imageHelper.mean();

	//QImage newImage(width, height, QImage::Format_Grayscale8);


#pragma omp parallel for
	for (auto h = 0; h < height; ++h) {
		const auto scanLine = image.scanLine(h);
		for (auto w = 0; w < width; ++w) {
			auto t = scanLine[w] - mean;
			t *= contrast; //Adjust contrast
			t += mean * brightness; // Adjust brightness
			scanLine[w] = (t > 255.0) ? (255) : (t < 0.0 ? (0) : (t));
		}
	}
	//setCurrentImage(sliceType(), image);
	//SliceItem * item = sliceItem(sliceType());
	//Q_ASSERT_X(item, "HistogramViewer::filterImage", "null pointer");
	//item->setPixmap(QPixmap::fromImage(image));

	m_preview->setImage(image);			//Note:: this variable is no longer the original image.
}


void HistogramWidget::createWidgets()
{

	auto layout = new QVBoxLayout;
	m_preview = new ImagePreviewWidget(this);
	layout->addWidget(m_preview);

	m_equliWidget = new EqualizationControlWidget(this);
	connect(m_equliWidget, &EqualizationControlWidget::valueChanged, this, &HistogramWidget::histEqualizeImage);
	m_brightWidget = new BrightnessContrastControlWidget(this);
	connect(m_brightWidget, &BrightnessContrastControlWidget::valueChanged, this, &HistogramWidget::updateContrastAndBrightness);
	m_filterWidget = new FilterControlWidget(this);
	connect(m_filterWidget, &FilterControlWidget::filter, this, &HistogramWidget::filterImage);

	m_tabWidget = new QTabWidget(this);
	m_tabWidget->addTab(m_equliWidget, QStringLiteral("Equalization"));
	m_tabWidget->addTab(m_brightWidget, QStringLiteral("Brightness and Contrast"));
	m_tabWidget->addTab(m_filterWidget, QStringLiteral("Filter"));
	layout->addWidget(m_tabWidget);

	m_reset = new QPushButton(QStringLiteral("Reset"), this);
	connect(m_reset, &QPushButton::clicked, this, &HistogramWidget::resetOriginalImage);
	m_apply = new QPushButton(QStringLiteral("Apply"), this);
	connect(m_apply, &QPushButton::clicked, [this]() 
	{
		setCurrentImage(sliceType(), m_preview->image());
		SliceItem * item = sliceItem(sliceType());
		item->setPixmap(QPixmap::fromImage(m_preview->image()));
	});
	auto buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(m_reset);
	buttonLayout->addWidget(m_apply);
	layout->addLayout(buttonLayout);
	setLayout(layout);

}


ImagePreviewWidget::ImagePreviewWidget(const QImage &image, QWidget *parent) :ImagePreviewWidget(parent)
{
	setImage(image);
}

ImagePreviewWidget::ImagePreviewWidget(QWidget *parent) : QWidget(parent)
{
	auto layout = new QVBoxLayout;
	m_preview = new QLabel(this);
	layout->addWidget(m_preview, 1, Qt::AlignCenter);
	m_hist = new Histogram(parent);
	layout->addWidget(m_hist);

	auto group = new QGroupBox(QStringLiteral("Preview"));
	group->setLayout(layout);
	auto mainLayout = new QVBoxLayout;
	mainLayout->addWidget(group);

	setLayout(mainLayout);
}

void ImagePreviewWidget::setImage(const QImage & image)
{
	m_preview->setPixmap(QPixmap::fromImage(image).scaled(300,300,Qt::KeepAspectRatio));
	m_hist->setImage(image);
	m_image = image;
}

QImage ImagePreviewWidget::image() const {
	return m_image;
}

EqualizationControlWidget::EqualizationControlWidget(QWidget* parent) :QWidget(parent)
{
	auto layout = new QVBoxLayout;
	m_doubleSlier = new DoubleSlider(this);
	m_equalLevel = new TitledSliderWithSpinBox(this, QStringLiteral("Level:"));
	m_equalLevel->setMaximum(10);
	layout->addWidget(m_equalLevel);
	layout->addWidget(m_doubleSlier);
	layout->addStretch(1);

	auto group = new QGroupBox(QStringLiteral("Equalization"), this);
	group->setLayout(layout);
	auto mainLayout = new QVBoxLayout;
	mainLayout->addWidget(group);
	setLayout(mainLayout);

	blockSignals(true);
	reset();
	blockSignals(false);

	connect(m_doubleSlier, &DoubleSlider::valuesChanged, [this](float a, float b) {emit valueChanged(); });
	connect(m_equalLevel, &TitledSliderWithSpinBox::valueChanged, [this](int value) {emit valueChanged(); });
}

int EqualizationControlWidget::equalizationLevel() const
{
	return m_equalLevel->value();
}

int EqualizationControlWidget::minValue() const
{
	return m_doubleSlier->getMinValue() * 255;
}

int EqualizationControlWidget::maxValue() const
{
	return m_doubleSlier->getMaxValue() * 255;
}

void EqualizationControlWidget::reset()
{
	m_equalLevel->setValue(3);
	m_doubleSlier->setValues(0.f, 1.0f);
}


FilterControlWidget::FilterControlWidget(QWidget* parent) :QWidget(parent) {

	m_filterLabel = new QLabel(QStringLiteral("Filters:"), this);
	m_filterComboBox = new QComboBox(this);
	m_filterComboBox->addItem(QStringLiteral("..."));
	m_filterComboBox->addItem(QStringLiteral("Median Filter"));
	m_filterComboBox->addItem(QStringLiteral("Gaussian Filter"));
	m_filterComboBox->setEditable(false);
	connect(m_filterComboBox, QOverload<const QString&>::of(&QComboBox::currentTextChanged), this, &FilterControlWidget::updateLayout);
	m_filterButton = new QPushButton(QStringLiteral("Filter"), this);
	connect(m_filterButton, &QPushButton::clicked, this, &FilterControlWidget::filter);

	auto filterTextLayout = new QHBoxLayout;
	filterTextLayout->addWidget(m_filterLabel);
	filterTextLayout->addWidget(m_filterComboBox);
	filterTextLayout->addWidget(m_filterButton);


	m_medianKernelController = new TitledSliderWithSpinBox(this, QStringLiteral("Median Kernel Size"));
	m_medianKernelController->setMinimum(1);
	m_medianKernelController->setMaximum(10);
	m_medianKernelController->setVisible(false);
	m_sigmaXController = new TitledSliderWidthDoubleSpinBox(QStringLiteral("SigmaX"), Qt::Horizontal, this);
	m_sigmaXController->setMinimum(0);
	m_sigmaXController->setMaximum(10);
	m_sigmaXController->setVisible(false);
	m_sigmaYController = new TitledSliderWidthDoubleSpinBox(QStringLiteral("SigmaY"), Qt::Horizontal, this);
	m_sigmaYController->setVisible(false);
	m_sigmaYController->setMinimum(0);
	m_sigmaYController->setMaximum(10);
	m_controllerLayout = new QVBoxLayout;
	m_controllerLayout->addWidget(m_medianKernelController);
	m_controllerLayout->addWidget(m_sigmaXController);
	m_controllerLayout->addWidget(m_sigmaYController);
	m_controllerLayout->addStretch(1);

	auto layout = new QVBoxLayout;
	layout->addLayout(filterTextLayout);
	layout->addLayout(m_controllerLayout);

	auto group = new QGroupBox(QStringLiteral("Filter"));
	group->setLayout(layout);

	auto mainLayout = new QVBoxLayout;
	mainLayout->addWidget(group);
	setLayout(mainLayout);

	blockSignals(true);
	reset();
	blockSignals(false);

}
double FilterControlWidget::sigmaX() const
{
	return m_sigmaXController->value();
}

double FilterControlWidget::sigmaY() const
{
	return m_sigmaYController->value();
}

int FilterControlWidget::medianKernelSize() const
{
	return m_medianKernelController->value();
}

QString FilterControlWidget::text() const {
	return m_filterComboBox->currentText();
}

void FilterControlWidget::reset() {
	m_filterComboBox->setCurrentText(QStringLiteral("..."));
	m_medianKernelController->setValue(4);
	m_sigmaXController->setValue(1.0);
	m_sigmaYController->setValue(1.0);
}

void FilterControlWidget::updateLayout(const QString & text)
{
	if (text == "Median Filter") {
		m_medianKernelController->setVisible(true);
		m_sigmaXController->setVisible(false);
		m_sigmaYController->setVisible(false);
	}
	else if (text == "Gaussian Filter") {
		m_medianKernelController->setVisible(false);
		m_sigmaXController->setVisible(true);
		m_sigmaYController->setVisible(true);
	}
	else {
		m_medianKernelController->setVisible(false);
		m_sigmaXController->setVisible(false);
		m_sigmaYController->setVisible(false);

	}
}
