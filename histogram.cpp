#include "histogram.h"
#include <qdebug.h>

Histogram::Histogram(QWidget *parent):QWidget(parent),
    m_hist{QVector<int>(BIN_COUNT)},
    m_minValue{0},
    m_maxValue{BIN_COUNT-1},
    m_count{0}
{
    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    setMinimumSize(MIN_WIDTH,MIN_HEIGHT);
    resize(MIN_WIDTH,MIN_HEIGHT);

    for(int i=250;i<BIN_COUNT;i++){
        m_hist[i] = qrand()%1000;
        m_count+=m_hist[i];
    }
}

Histogram::Histogram(QWidget *parent, const QImage &image):Histogram(parent)
{
    setImage(image);
}

void Histogram::setImage(const QImage &image)
{
    if(image.depth() != 8 || image.bytesPerLine() != image.width()){
        QMessageBox::critical(this,tr("Error"),
                              tr("Only Support 8bit image."),
                              QMessageBox::Yes,QMessageBox::Yes);
        return;
    }
    m_hist.clear();
    m_hist.resize(BIN_COUNT);
    //qDebug()<<m_hist.size();
    m_count = image.width()*image.height();
    const unsigned char * data = image.bits();
    for(int i=0;i<m_count;i++){
        m_hist[*(data+i)]++;
    }
    update();
    updateGeometry();
}

QSize Histogram::sizeHint() const
{
    return m_histImage.size();
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
           int binHeight = c*height;
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

}
