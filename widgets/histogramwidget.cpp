#include <qdebug.h>
#include <QMessageBox>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <cmath>
#include <QMouseEvent>

#include "histogramwidget.h"
#include "model/sliceitem.h"
#include "widgets/sliceeditorwidget.h"


#define cimg_display 0 //
#define cimg_OS 0
#include "algorithm/CImg.h"


Histogram::Histogram(QWidget *parent):QWidget(parent),
    m_hist{QVector<int>(BIN_COUNT)},
    m_minValue{0},
    m_maxValue{BIN_COUNT-1},
    m_count{0},
    m_mousePressed{false},
    m_rightCursorSelected{false},
    m_leftCursorSelected{false},
    m_cursorEnable{true}
{
    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    setMinimumSize(MIN_WIDTH,MIN_HEIGHT);
    resize(MIN_WIDTH,MIN_HEIGHT);
}

Histogram::Histogram(QWidget *parent, const QImage &image):Histogram(parent)
{
    setImage(image);
}

void Histogram::setImage(const QImage &image)
{
    ///TODO: bull shit design
	//qDebug() << image.depth() << " " << image.bytesPerLine() << " " << image.width() << " " << image.height();
    if(image.depth() != 8 ){

        QMessageBox::critical(this,tr("Error"),
                              tr("Only Support 8bit image."),
                              QMessageBox::Yes,QMessageBox::Yes);
        return;
    }

    m_hist.clear();
    m_hist.resize(BIN_COUNT);

    m_count = image.width()*image.height();
    const unsigned char * data = image.bits();
    for(int i=0;i<m_count;i++)
        m_hist[*(data+i)]++;
    update();
    updateGeometry();
}

QVector<int> Histogram::getHist() const
{
	return m_hist;
}

QSize Histogram::sizeHint() const
{
    return m_histImage.size();
}

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
   QImage image(size(),QImage::Format_ARGB32_Premultiplied);
   QPainter imagePainter(&image);
   imagePainter.initFrom(this);
   imagePainter.setRenderHint(QPainter::Antialiasing,true);
   imagePainter.fillRect(rect(),QBrush(Qt::white));

   qreal height = static_cast<qreal>(image.height());
   qreal width = static_cast<qreal>(image.width());
   qreal binWidth = width/BIN_COUNT;

   //Drawing histogram
   if(m_count != 0){
       imagePainter.setPen(QColor(0,0,0));
       imagePainter.setBrush(QBrush(QColor(0,0,0)));

	   QVarLengthArray<qreal, 256> f(256);
       for(int i=0;i<BIN_COUNT;i++){
           f[i] = ((static_cast<double>(m_hist[i])/static_cast<double>(m_count)));
       }
	   qreal *fmax = std::max_element(f.begin(), f.end());
	   qreal mag = 0.7 / *fmax;		//the height of the max bin is set as 0.7 of height of the widget/  
	   for(int i=0;i<BIN_COUNT;i++)
	   {
		   int binHeight = f[i] * height*mag;
		   imagePainter.drawRect(QRectF
		   (QPointF(i*binWidth, height - binHeight), QSize(binWidth, binHeight))
		   );
	   }
   }

   //Drawing lower bound and upper bound lines
   imagePainter.setPen(QColor(255,0,0));
   imagePainter.setBrush(QBrush(QColor(255,0,0)));

   qreal lowerBoundLineX = m_minValue*binWidth+binWidth/2;
   qreal upperBoundLineX = m_maxValue*binWidth+binWidth/2;
   imagePainter.drawLine(QPointF(lowerBoundLineX,0),QPointF(lowerBoundLineX,height));
   imagePainter.drawLine(QPointF(upperBoundLineX,0),QPointF(upperBoundLineX,height));

   imagePainter.end();
   QPainter widgetPainter(this);
   widgetPainter.drawImage(0,0,image);

}

void Histogram::mouseMoveEvent(QMouseEvent *event)
{
    if(m_mousePressed == true){
        if(m_rightCursorSelected == true){
            int value = static_cast<qreal>(event->pos().x())/width()*BIN_COUNT;
            m_maxValue = value<m_minValue?m_minValue:value;
            emit maxValueChanged(m_maxValue);
        }else if(m_leftCursorSelected == true){
            int value = static_cast<qreal>(event->pos().x())/width()*BIN_COUNT;
            m_minValue = value>m_maxValue?m_maxValue:value;
            emit minValueChanged(m_minValue);
        }
        update();
        updateGeometry();
    }
}

void Histogram::mousePressEvent(QMouseEvent * event)
{
    m_mousePressed = true;
    if(m_cursorEnable == true){
       qreal x = event->pos().x();
       qreal dl = std::abs(x - getXofLeftCursor());
       qreal dr = std::abs(x - getXofRightCursor());
       if(dl < dr && dl < 5.0){
           m_leftCursorSelected = true;
       }else if(dr < dl && dr < 5.0){
           m_rightCursorSelected = true;
       }
    }
}

void Histogram::mouseReleaseEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
    if(m_mousePressed == true)
        m_mousePressed = false;
    m_leftCursorSelected = false;
    m_rightCursorSelected = false;
}

qreal Histogram::getXofLeftCursor()
{
    qreal binWidth = static_cast<qreal>(size().width())/BIN_COUNT;
    qreal x = m_minValue*binWidth+binWidth/2;
    return x;
}

qreal Histogram::getXofRightCursor()
{
    qreal binWidth = static_cast<qreal>(size().width())/BIN_COUNT;
    qreal x = m_maxValue*binWidth+binWidth/2;
    return x;
}
/*
 * HistogramViewer Definitions
*/
HistogramWidget::HistogramWidget(SliceType type, SliceEditorWidget * sliceEditor,  QWidget * parent):
AbstractSliceViewPlugin(type,sliceEditor,parent)
{
    createWidgets();

	createConnections();

	init();

	setEnabled(sliceItem(sliceType()) != nullptr);
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
	SliceItem * item = sliceItem(sliceType());

	Q_ASSERT_X(item, "HistogramViewer::reset", "null pointer");
	//Q_ASSERT_X(m_sliceWidget, "HistogramWidget", "null pointer");

	const auto curIndex = currentIndex(sliceType());
	const auto origin = originalImage(sliceType(),curIndex);

	// reset slice view with original image
	item->setPixmap(QPixmap::fromImage(origin));

	// reset histogram with original image
	m_hist->setImage(origin);

	setImage(sliceType(),curIndex,origin);


	initWidgets();
}

void HistogramWidget::filterImage()
{
    QString text = m_filterComboBox->currentText();
	//TODO::get QImage

    //int currentIndex = m_ptr->getCurrentSliceIndex();
    //QImage slice = m_ptr->getOriginalTopSlice(currentIndex);

	QImage slice = currentImage(sliceType()).copy();
	

    int width = slice.width();
    int height = slice.height();
    //slice.data_ptr();
    Q_ASSERT_X(slice.depth() ==8,"HistogramViewer::onFilterButton","Only support 8-bit image.");
    cimg_library::CImg<unsigned char> image(slice.bits(),width,height,1,1,true);
    //filter parameters
    if(text == "Median Filter"){
        int kernelSize = m_medianKernelSizeSpinBox->value();
        image.blur_median(kernelSize);
    }else if(text == "Gaussian Filter"){
        double sigX = m_sigmaXSpinBox->value();
        double sigY = m_sigmaYSpinBox->value();
        image.blur(sigX,sigY,0,true,true);
    }
    //m_internalUpdate = true;
    //m_ptr->setSlice(slice,currentIndex,SliceType::SliceZ);
    //m_model->setData(getDataIndex(m_modelIndex),QVariant::fromValue(m_ptr));
	SliceItem * item = sliceItem(sliceType());
	Q_ASSERT_X(item, "HistogramViewer::filterImage", "null pointer");
	item->setPixmap(QPixmap::fromImage(slice));

	setCurrentImage(sliceType(), slice);
}


void HistogramWidget::updateDataModel() {
	qDebug() << "Data Model Changed";
	init();
}

void HistogramWidget::initWidgets() {
	m_minSlider->blockSignals(true);
	m_minSlider->setValue(0);
	m_minSlider->blockSignals(false);
	m_maxSlider->blockSignals(true);
	m_maxSlider->setValue(255);
	m_maxSlider->blockSignals(false);

	m_contrastFactor->setValue(1.0);
	m_brightnessFactor->setValue(1.0);

}

void HistogramWidget::init() {
	// reset all widgets and get image to draw histogram

	initWidgets();

	SliceItem * item = sliceItem(sliceType());
	const auto curImg = currentImage(sliceType());

	item->setPixmap(QPixmap::fromImage(curImg));

	// reset histogram with original image
	m_hist->setImage(curImg);

}

void HistogramWidget::histEqualizeImage()
{
	const auto minValue = m_minSlider->value();
	const auto maxValue = m_maxSlider->value();
	//qDebug() << minValue << " " << maxValue;
	//update min and max value
	QImage oriImage = originalImage(sliceType(),currentIndex(sliceType())).copy();

	Q_ASSERT_X(oriImage.isNull() == false, "HistogramViewer::updateImage", "null image");
	unsigned char *image = oriImage.bits();
	const auto width = oriImage.width();
	const auto height = oriImage.height();

    //memory buffer is shared between CImg and QImage
    cimg_library::CImg<unsigned char> equalizedImage(image,width,height,1,1,true);
    equalizedImage.equalize(m_histNumSpinBox->value(),minValue,maxValue);

//	qreal k = 256.0 / static_cast<qreal>(maxValue - minValue);
//	QImage strechingImage(width, height, QImage::Format_Grayscale8);
//	///TODO::a time-cost processure
//	unsigned char * data = strechingImage.bits();
//	for (int j = 0; j<height; j++) {
//		for (int i = 0; i<width; i++) {
//			int index = i + j * width;
//			unsigned char pixelGrayValue = image[index];
//			unsigned char clower = static_cast<unsigned char>(minValue);
//			unsigned char cupper = static_cast<unsigned char>(maxValue);
//			if (pixelGrayValue < clower) {
//				data[index] = 0;
//			}
//			else if (pixelGrayValue>cupper) {
//				data[index] = 255;
//			}
//			else {
//				data[index] = static_cast<unsigned char>(pixelGrayValue* k + 0.5);
//			}
//		}
//	}
    m_hist->setImage(oriImage);			//Note:: this variable is no longer the original image.
	SliceItem * item = sliceItem(sliceType());
	Q_ASSERT_X(item, "HistogramViewer::filterImage", "null pointer");
	//qDebug() << item->pos();
	item->setPixmap(QPixmap::fromImage(oriImage));
	//qDebug() << item->pos();
	setCurrentImage(sliceType(),oriImage);
}

void HistogramWidget::updateContrastAndBrightness() 
{
	const auto brightness = m_brightnessFactor->value();
	const auto contrast = m_contrastFactor->value();


	auto image = originalImage(sliceType(),currentIndex(sliceType())).copy();		// There need to use copy(), think why.

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
	for(auto h = 0;h < height;++h) {
		const auto scanLine = image.scanLine(h);
		for(auto w = 0;w<width;++w) {
			auto t = scanLine[w] - mean;

			t *= contrast; //Adjust contrast
			t += mean * brightness; // Adjust brightness

			scanLine[w] = (t > 255.0) ? (255) : (t<0.0 ? (0):(t));

		}
	}

	setCurrentImage(sliceType(), image);

	m_hist->setImage(image);			//Note:: this variable is no longer the original image.
	SliceItem * item = sliceItem(sliceType());
	Q_ASSERT_X(item, "HistogramViewer::filterImage", "null pointer");
	//qDebug() << item->pos();
	item->setPixmap(QPixmap::fromImage(image));

}


void HistogramWidget::createWidgets()
{
	m_mainLayout = new QGridLayout;
	m_histogramLayout = new QGridLayout;
    m_histogramGroupBox = new QGroupBox(QStringLiteral("Histogram"),this);
    m_histogramGroupBox->setLayout(m_histogramLayout);
    m_histNumSpinBox = new QSpinBox(this);
    m_histNumSpinBox->setMinimum(1);
    m_histNumSpinBox->setMaximum(10);
    m_histNumLabel = new QLabel(QStringLiteral("Equalization Levels:"),this);
    m_hist = new Histogram(this);
    m_hist->setDragEnable(false);
    m_minSlider = new TitledSliderWithSpinBox(this,QStringLiteral("Min:"));
    m_maxSlider = new TitledSliderWithSpinBox(this,QStringLiteral("Max:"));

	m_contrastFactor = new TitledSliderWidthDoubleSpinBox(QStringLiteral("Contrast"), Qt::Horizontal, this);
	m_contrastFactor->setRange(0, 5);
	m_contrastFactor->setSingleStep(0.1);
	m_contrastFactor->setValue(1.0);
	connect(m_contrastFactor, &TitledSliderWidthDoubleSpinBox::valueChanged, [this](double value) {updateContrastAndBrightness();});
	m_brightnessFactor = new TitledSliderWidthDoubleSpinBox(QStringLiteral("Brightness"), Qt::Horizontal, this);
	m_brightnessFactor->setRange(0, 5);
	m_brightnessFactor->setSingleStep(0.1);
	m_brightnessFactor->setValue(1.0);
	connect(m_brightnessFactor, &TitledSliderWidthDoubleSpinBox::valueChanged, [this](double value) {updateContrastAndBrightness(); });


    m_histogramLayout->addWidget(m_hist,0,0,1,2);
    m_histogramLayout->addWidget(m_histNumLabel,1,0);
    m_histogramLayout->addWidget(m_histNumSpinBox,1,1);
	m_histogramLayout->addWidget(m_minSlider, 2, 0, 1, 2);
	m_histogramLayout->addWidget(m_maxSlider, 3, 0, 1, 2);
	m_histogramLayout->addWidget(m_contrastFactor, 4, 0,1,2);
	m_histogramLayout->addWidget(m_brightnessFactor, 5,0,1,2);


    m_filterLabel = new QLabel(QStringLiteral("Filters:"),this);
    m_filterComboBox = new QComboBox(this);
    m_filterComboBox->addItem(QStringLiteral("..."));
    m_filterComboBox->addItem(QStringLiteral("Median Filter"));
    m_filterComboBox->addItem(QStringLiteral("Gaussian Filter"));
    m_filterComboBox->setEditable(false);
    m_filterButton = new QPushButton(QStringLiteral("Filter"),this);
	m_parameterLayout = new QGridLayout;

	m_filterLayout = new QGridLayout;
    m_filterGroupBox = new QGroupBox(QStringLiteral("Filter"),this);
    m_filterGroupBox->setLayout(m_filterLayout);
    m_filterLayout->addWidget(m_filterLabel,0,1);
    m_filterLayout->addWidget(m_filterComboBox,0,2);
    m_filterLayout->addWidget(m_filterButton,0,3);
    m_filterLayout->addLayout(m_parameterLayout,1,0,1,3);
    m_reset = new QPushButton(QStringLiteral("Reset"),this);

    m_mainLayout->addWidget(m_histogramGroupBox,0,0);
    m_mainLayout->addWidget(m_filterGroupBox,1,0);
    m_mainLayout->addWidget(m_reset,2,0);


    //![1]median parameter widgets
    m_medianKernelSizeLabel = new QLabel(QStringLiteral("Kernel Size:"));
    m_medianKernelSizeSpinBox = new  QSpinBox;
    m_medianKernelSizeSpinBox->setMinimum(1);
    m_medianKernelSizeSpinBox->setMaximum(10);

    //![2]gaussian parameter widgets
    m_sigmaXLabel = new QLabel(QStringLiteral("X sigma:"));
    m_sigmaYLabel = new QLabel(QStringLiteral("Y sigma:"));
    m_sigmaXSpinBox = new QDoubleSpinBox;
    m_sigmaXSpinBox->setMinimum(0);
    m_sigmaXSpinBox->setMaximum(10.0);
    m_sigmaXSpinBox->setSingleStep(0.05);
    m_sigmaYSpinBox = new QDoubleSpinBox;
    m_sigmaYSpinBox->setMinimum(0.0);
    m_sigmaYSpinBox->setMaximum(10.0);
    m_sigmaYSpinBox->setSingleStep(0.05);

    int maxValue = m_hist->getBinCount()-1;
    m_maxSlider->setMaximum(maxValue);
    m_minSlider->setMaximum(maxValue);
    m_minSlider->setValue(0);
    m_maxSlider->setValue(maxValue);
	setLayout(m_mainLayout);

}

void HistogramWidget::createConnections()
{
	connect(m_minSlider, &TitledSliderWithSpinBox::valueChanged, m_hist, &Histogram::setLeftCursorValue);
	connect(m_maxSlider, &TitledSliderWithSpinBox::valueChanged, m_hist, &Histogram::setRightCursorValue);
	connect(m_minSlider, &TitledSliderWithSpinBox::valueChanged, this, &HistogramWidget::onMinValueChanged);
	connect(m_maxSlider, &TitledSliderWithSpinBox::valueChanged, this, &HistogramWidget::onMaxValueChanged);

	connect(m_filterButton, &QPushButton::clicked, this, &HistogramWidget::filterImage);

	connect(m_filterComboBox, QOverload<const QString &>::of(&QComboBox::activated), [=](const QString & text) {
		Q_UNUSED(text);
	});
	connect(m_filterComboBox, &QComboBox::currentTextChanged, [=](const QString & text) {
		//qDebug() << "Signal::currentTextChanged";
		updateParameterLayout(text);
	});
	connect(m_reset, &QPushButton::clicked, this, &HistogramWidget::resetOriginalImage);


}

void HistogramWidget::updateParameterLayout(const QString &text)
{
    if(text == "Median Filter"){
        m_parameterLayout->removeWidget(m_sigmaXLabel);
        m_sigmaXLabel->setParent(nullptr);
        m_parameterLayout->removeWidget(m_sigmaXSpinBox);
        m_sigmaXSpinBox->setParent(nullptr);
        m_parameterLayout->removeWidget(m_sigmaYLabel);
        m_sigmaYLabel->setParent(nullptr);
        m_parameterLayout->removeWidget(m_sigmaYSpinBox);
        m_sigmaYSpinBox->setParent(nullptr);
        m_parameterLayout->addWidget(m_medianKernelSizeLabel,0,0);
        m_parameterLayout->addWidget(m_medianKernelSizeSpinBox,0,1);

    }else if(text == "Gaussian Filter"){
        m_parameterLayout->removeWidget(m_medianKernelSizeLabel);
        m_medianKernelSizeLabel->setParent(nullptr);
        m_parameterLayout->removeWidget(m_medianKernelSizeSpinBox);
        m_medianKernelSizeSpinBox->setParent(nullptr);
        m_parameterLayout->addWidget(m_sigmaXLabel,0,0);
        m_parameterLayout->addWidget(m_sigmaXSpinBox,0,1);
        m_parameterLayout->addWidget(m_sigmaYLabel,1,0);
        m_parameterLayout->addWidget(m_sigmaYSpinBox,1,1);
    }else{
        m_parameterLayout->removeWidget(m_sigmaXLabel);
        m_sigmaXLabel->setParent(nullptr);
        m_parameterLayout->removeWidget(m_sigmaXSpinBox);
        m_sigmaXSpinBox->setParent(nullptr);
        m_parameterLayout->removeWidget(m_sigmaYLabel);
        m_sigmaYLabel->setParent(nullptr);
        m_parameterLayout->removeWidget(m_sigmaYSpinBox);
        m_sigmaYSpinBox->setParent(nullptr);
        m_parameterLayout->removeWidget(m_medianKernelSizeLabel);
        m_medianKernelSizeLabel->setParent(nullptr);
        m_parameterLayout->removeWidget(m_medianKernelSizeSpinBox);
        m_medianKernelSizeSpinBox->setParent(nullptr);

    }
}

