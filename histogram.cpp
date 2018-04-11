#include "histogram.h"
#include <qdebug.h>
#include <QMouseEvent>
#include <algorithm>

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
HistogramViewer::HistogramViewer(QWidget *parent)noexcept:QWidget(parent)
{
    m_layout = new QGridLayout(this);
    m_hist = new Histogram(this);
    m_hist->setDragEnable(false);
    m_minSlider = new TitledSliderWithSpinBox(this,QString("MinValue:"));
    m_maxSlider = new TitledSliderWithSpinBox(this,QString("MaxValue:"));

    m_layout->addWidget(m_hist,0,0);
    m_layout->addWidget(m_minSlider,1,0);
    m_layout->addWidget(m_maxSlider,2,0);

    connect(m_minSlider,SIGNAL(valueChanged(int)),this,SIGNAL(minValueChanged(int)));
    connect(m_maxSlider,SIGNAL(valueChanged(int)),this,SIGNAL(maxValueChanged(int)));
    connect(m_minSlider,SIGNAL(valueChanged(int)),m_hist,SLOT(setLeftCursorValue(int)));
    connect(m_maxSlider,SIGNAL(valueChanged(int)),m_hist,SLOT(setRightCursorValue(int)));

    int maxValue = m_hist->getBinCount()-1;
    m_maxSlider->setMaximum(maxValue);
    m_minSlider->setMaximum(maxValue);
    m_minSlider->setValue(0);
    m_maxSlider->setValue(maxValue);
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
void HistogramViewer::setLeftCursorValue(int value)
{
    m_hist->setLeftCursorValue(value);
}

void HistogramViewer::setRightCursorValue(int value)
{
    m_hist->setRightCursorValue(value);
}

void HistogramViewer::setEnabled(bool enable)
{
    m_minSlider->setEnabled(enable);
    m_maxSlider->setEnabled(enable);
    m_hist->setEnabled(enable);
}

int HistogramViewer::getLeftCursorValue() const
{
   return m_minSlider->value();
}

int HistogramViewer::getRightCursorValue() const
{
    return m_maxSlider->value();
}


