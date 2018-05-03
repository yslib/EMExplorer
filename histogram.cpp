#include <qdebug.h>
#include <QMouseEvent>
#include <QMessageBox>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QGroupBox>
#include <cmath>

#include "histogram.h"


#define cimg_display 0 //
#define cimg_OS 1
#include "CImg.h"


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
    if(image.depth() != 8 || image.bytesPerLine() != image.width()){
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

       for(int i=0;i<BIN_COUNT;i++){
           qreal c = (static_cast<double>(m_hist[i])/static_cast<double>(m_count));
		   int binHeight = c*height*10;
           imagePainter.drawRect(QRectF
                                 (QPointF(i*binWidth,height-binHeight),QSize(binWidth,binHeight))
                   );
       }
   }

   //Drawing lower bound and upper bound lines
   imagePainter.setPen(QColor(255,100,0));
   imagePainter.setBrush(QBrush(QColor(255,100,0)));

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
HistogramViewer::HistogramViewer(QWidget *parent)noexcept:QWidget(parent),m_model(nullptr)
{

    createWidgets();

    connect(m_minSlider, &TitledSliderWithSpinBox::valueChanged, [=](int value) { updateImage(); });
    connect(m_maxSlider, &TitledSliderWithSpinBox::valueChanged, [=](int value) { updateImage(); });

    //connect(m_minSlider,SIGNAL(valueChanged(int)),this,SIGNAL(minValueChanged(int)));
    //connect(m_maxSlider,SIGNAL(valueChanged(int)),this,SIGNAL(maxValueChanged(int)));

    connect(m_minSlider,SIGNAL(valueChanged(int)),m_hist,SLOT(setLeftCursorValue(int)));
    connect(m_maxSlider,SIGNAL(valueChanged(int)),m_hist,SLOT(setRightCursorValue(int)));

    connect(m_filterButton,&QPushButton::clicked,this,&HistogramViewer::onFilterButton);

    connect(m_filterComboBox,QOverload<const QString &>::of(&QComboBox::activated),[=](const QString & text){
        updateParameterLayout(text);
    });
    connect(m_filterComboBox,&QComboBox::currentTextChanged,[=](const QString & text){

    });

    connect(m_reset,&QPushButton::clicked,this,&HistogramViewer::onResetButton);

    connect(m_minSlider,&TitledSliderWithSpinBox::valueChanged,this,&HistogramViewer::onMinValueChanged);
    connect(m_maxSlider,&TitledSliderWithSpinBox::valueChanged,this,&HistogramViewer::onMaxValueChanged);

    updateActions();


}

HistogramViewer::HistogramViewer(QWidget *parent, const QImage &image)noexcept:HistogramViewer(parent)
{
   setImage(image);
}

void HistogramViewer::setImage(const QImage &image)
{
    m_hist->setImage(image);
}

QVector<int> HistogramViewer::getHist() const
{
    return m_hist->getHist();
}
void HistogramViewer::setModel(DataItemModel * model)
{
	if(m_model != model)
	{
		m_model = model;
		disconnect(m_model, &DataItemModel::dataChanged, this, &HistogramViewer::dataChanged);
		connect(m_model, &DataItemModel::dataChanged, this, &HistogramViewer::dataChanged);
		///TODO::get corresponding data i.e. current slice, and draw the histgoram 
	}
}

void HistogramViewer::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
	qDebug() << "HistogramViewer:model has been updated";
	///TODO::update data
	if(m_internalUpdate == true)
	{
		m_internalUpdate == false;
		return;
	}

	if(m_ptr.isNull() == true)
	{
		qWarning("Model is empty.");
		return;
	}
	int currentSlice = m_ptr->getCurrentSliceIndex();
    qDebug()<<"In HistogramViewer:Slice:"<<currentSlice;

	//different slice should have different bounds.

	int low = m_ptr->getGrayscaleStrechingLowerBound();
	int high = m_ptr->getGrayscaleStrechingUpperBound();

	m_minSlider->setValue(low);
	m_maxSlider->setValue(high);
	m_hist->setLeftCursorValue(low);
	m_hist->setRightCursorValue(high);
	setImage(m_ptr -> getTopSlice(currentSlice));

}

void HistogramViewer::activateItem(const QModelIndex & index)
{
	if (m_model == nullptr)
	{
		qWarning("Model is empty.",__LINE__);
		return;
	}
	
	//QVariant var = m_model->data(getDataIndex(index));
	QVariant var = m_model->data(index.sibling(index.row(),index.column()+1));
	if(var.canConvert<QSharedPointer<ItemContext>>() == true)
	{
		m_modelIndex = index;
		auto p = var.value<QSharedPointer<ItemContext>>();
		m_ptr = p;
		int currentSlice = p->getCurrentSliceIndex();
		int low = p->getGrayscaleStrechingLowerBound();
		int high = p->getGrayscaleStrechingUpperBound();
		m_minSlider->setValue(low);
		m_maxSlider->setValue(high);
		m_hist->setLeftCursorValue(low);
		m_hist->setRightCursorValue(high);
		setImage(p->getTopSlice(currentSlice));
	}
    updateActions();
}



void HistogramViewer::onMinValueChanged(int value)
{
    //qDebug()<<"min";
    updateImage();
}

void HistogramViewer::onMaxValueChanged(int value)
{
    //qDebug()<<"max";
    updateImage();
}

void HistogramViewer::onResetButton()
{
    m_internalUpdate = true;
    bool old = m_minSlider->blockSignals(true);
    m_minSlider->setValue(0);
    m_minSlider->blockSignals(old);
    old = m_maxSlider->blockSignals(true);
    m_maxSlider->setValue(255);
    m_maxSlider->blockSignals(old);
    int currentIndex = m_ptr->getCurrentSliceIndex();
    m_ptr->setSlice(m_ptr->getOriginalTopSlice(currentIndex),currentIndex,SliceType::SliceZ);
    m_model->setData(getDataIndex(m_modelIndex),QVariant::fromValue(m_ptr));
}

void HistogramViewer::onFilterButton()
{
    QString text = m_filterComboBox->currentText();
    int currentIndex = m_ptr->getCurrentSliceIndex();
    QImage slice = m_ptr->getOriginalTopSlice(currentIndex);
    int width = slice.width();
    int height = slice.height();
    slice.data_ptr();

    Q_ASSERT_X(slice.depth() ==8,"HistogramViewer::onFilterButton","Only support 8-bit image.");
    cimg_library::CImg<unsigned char> image(slice.bits(),width,height,1,1,true);

    //filter parameters
    int kernelSize = 3;

    if(text == "Median Filter"){

        image.blur_median(kernelSize);
    }else if(text == "Gaussian Filter"){

    }
    m_internalUpdate = true;
    m_ptr->setSlice(slice,currentIndex,SliceType::SliceZ);
    m_model->setData(getDataIndex(m_modelIndex),QVariant::fromValue(m_ptr));
}

void HistogramViewer::updateImage()
{

	if(m_ptr == nullptr)
	{
		qWarning("Model Pointer is Nullptr");
		return;
	}
	///TODO::update data
	int minValue = m_minSlider->value();
	int maxValue = m_maxSlider->value();
	int currentSlice = m_ptr->getCurrentSliceIndex();

	//update min and max value
	m_ptr->setGrayscaleStrechingLowerBound(minValue);
	m_ptr->setGrayScaleStrechingUpperBound(maxValue);
	QImage originalImage = m_ptr->getOriginalTopSlice(currentSlice);

	unsigned char *image = originalImage.bits();
	int width = originalImage.width();
	int height = originalImage.height();

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
    m_ptr->setTopSlice(originalImage, currentSlice);
    m_hist->setImage(originalImage);
	m_internalUpdate = true;
    m_model->setData(getDataIndex(m_modelIndex),QVariant::fromValue(m_ptr));
}

QModelIndex HistogramViewer::getDataIndex(const QModelIndex& itemIndex)
{
    return itemIndex.sibling(itemIndex.row(),itemIndex.column()+1);
}

void HistogramViewer::updateActions()
{
    setEnabled(m_modelIndex.isValid());
}

void HistogramViewer::createWidgets()
{
    m_mainLayout = new QGridLayout(this);


    m_histogramLayout= new QGridLayout(this);
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

    m_histogramLayout->addWidget(m_hist,0,0,1,2);
    m_histogramLayout->addWidget(m_histNumLabel,1,0);
    m_histogramLayout->addWidget(m_histNumSpinBox,1,1);
    m_histogramLayout->addWidget(m_minSlider,2,0,1,2);
    m_histogramLayout->addWidget(m_maxSlider,3,0,1,2);

    m_filterLabel = new QLabel(QStringLiteral("Filters:"),this);
    m_filterComboBox = new QComboBox(this);
    m_filterComboBox->addItem(QStringLiteral("Median Filter"));
    m_filterComboBox->addItem(QStringLiteral("Gaussian Filter"));
    m_filterButton = new QPushButton(QStringLiteral("Filter"),this);

    m_filterLayout =new QGridLayout(this);
    m_filterGroupBox = new QGroupBox(QStringLiteral("Filter"),this);
    m_filterGroupBox->setLayout(m_filterLayout);
    m_filterLayout->addWidget(m_filterLabel,0,1);
    m_filterLayout->addWidget(m_filterComboBox,0,2);
    m_filterLayout->addWidget(m_filterButton,0,3);
    m_reset = new QPushButton(QStringLiteral("Reset"),this);

    m_mainLayout->addWidget(m_histogramGroupBox,0,0);
    m_mainLayout->addWidget(m_filterGroupBox,1,0);
    m_mainLayout->addWidget(m_reset,2,0);


    // Parameter layout: Add these layouts at row 1 of filter layout
    //![1]median parameter layout
    m_medianKernelSizeLabel = new QLabel(QStringLiteral("Kernel Size:"),this);
    m_medianKernelSizeSpinBox = new  QSpinBox(this);
    m_medianKernelSizeSpinBox->setMinimum(1);
    m_medianKernelSizeSpinBox->setMaximum(10);
    m_medianFilterParameterLayout = new QGridLayout(this);
    m_medianFilterParameterLayout->addWidget(m_medianKernelSizeLabel,0,0);
    m_medianFilterParameterLayout->addWidget(m_medianKernelSizeLabel,0,1);
    //![2]gaussian parameter layout

    m_sigmaXLabel = new QLabel(QStringLiteral("X sigma:"),this);
    m_sigmaYLabel = new QLabel(QStringLiteral("Y sigma:"),this);
    m_sigmaXSpinBox = new QDoubleSpinBox(this);
    m_sigmaXSpinBox->setMinimum(0);
    m_sigmaXSpinBox->setMaximum(10.0);
    m_sigmaYSpinBox = new QDoubleSpinBox(this);
    m_sigmaYSpinBox->setMinimum(0.0);
    m_sigmaYSpinBox->setMaximum(10.0);

    m_gaussianFilterParameterLayout = new QGridLayout(this);
    m_gaussianFilterParameterLayout->addWidget(m_sigmaXLabel,0,0);
    m_gaussianFilterParameterLayout->addWidget(m_sigmaXSpinBox,0,1);
    m_gaussianFilterParameterLayout->addWidget(m_sigmaYLabel,1,0);
    m_gaussianFilterParameterLayout->addWidget(m_sigmaYSpinBox,1,1);

    int maxValue = m_hist->getBinCount()-1;
    m_maxSlider->setMaximum(maxValue);
    m_minSlider->setMaximum(maxValue);
    m_minSlider->setValue(0);
    m_maxSlider->setValue(maxValue);

}

void HistogramViewer::updateParameterLayout(const QString &text)
{
    QLayout * removed;
    QLayout * added;
    if(text == "Median Filter"){
        removed = m_gaussianFilterParameterLayout;
        added= m_medianFilterParameterLayout;
    }else if(text == "Gaussian Filter"){
        removed = m_medianFilterParameterLayout;
        added = m_gaussianFilterParameterLayout;
    }
    m_filterLayout->removeItem(removed);
    m_filterLayout->addLayout(added,1,1,1,3);
}


void HistogramViewer::setEnabled(bool enable)
{
    m_histNumSpinBox->setEnabled(enable);
    m_minSlider->setEnabled(enable);
    m_maxSlider->setEnabled(enable);
    m_hist->setEnabled(enable);
    m_filterComboBox->setEditable(enable);
    m_filterButton->setEnabled(enable);
    m_reset->setEnabled(enable);
}

int HistogramViewer::getLeftCursorValue() const
{
   return m_minSlider->value();
}

int HistogramViewer::getRightCursorValue() const
{
    return m_maxSlider->value();
}


