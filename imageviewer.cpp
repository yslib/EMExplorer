#include "imageviewer.h"
#include <QLabel>
#include <QLayout>
#include <QWheelEvent>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

bool ImageViewer::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_topImageLabel){
        if(event->type() == QEvent::MouseButtonPress){
            m_topImageEnablePaint = true;
            auto e = static_cast<QMouseEvent*>(event);
            m_prevPaint = e->pos();
            qDebug()<<m_prevPaint;
            return true;
        }else if(event->type() == QEvent::MouseMove){
            if(m_topImageEnablePaint == true){
                 auto e = static_cast<QMouseEvent*>(event);
                 //update();
                     QPoint p = e->pos();
                     paintLine(m_prevPaint,p,m_topImageLabel);
                     m_prevPaint = p;
            }
            return true;
        }else if(event->type() == QEvent::MouseButtonRelease){
            m_topImageEnablePaint= false;
            return true;
        }
    }else if(obj == m_rightImageLabel){
         if(event->type() == QEvent::MouseButtonPress){
            m_rightImageEnablePaint = true;
            auto e = static_cast<QMouseEvent*>(event);
            m_prevPaint = e->pos();
            update();
            return true;
        }else if(event->type() == QEvent::MouseMove){

            return true;
        }else if(event->type() == QEvent::MouseButtonRelease){
             m_rightImageEnablePaint = false;
            return true;
        }


    }else if(obj == m_frontImageLabel){
         if(event->type() == QEvent::MouseButtonPress){
            m_frontImageEnablePaint = true;
            auto e = static_cast<QMouseEvent*>(event);
            m_prevPaint = e->pos();
            update();
            return true;
        }else if(event->type() == QEvent::MouseMove){

             return true;
         }else if(event->type() == QEvent::MouseButtonRelease){
              m_frontImageLabel = false;
             return true;
         }

     }
     return QScrollArea::eventFilter(obj,event);
 }

 void ImageViewer::wheelEvent(QWheelEvent *event)
 {
     if(event->type() == QEvent::Wheel){
             QWheelEvent *e = static_cast<QWheelEvent*>(event);
             if(e->orientation() == Qt::Horizontal){

             }else{
                 if(m_loaded == true){
                     if(e->delta() > 0){
                        zoom(m_factor + 0.25);
                    }else{
                     zoom(m_factor - 0.25);
                    }

                 }
              }
     }
 }

 void ImageViewer::paintEvent(QPaintEvent *event)
 {

 }

 ImageViewer::ImageViewer(QWidget *parent) : QScrollArea(parent),m_factor(1.0),m_loaded(false)
 {

     m_displayWidget = new QWidget(this);
     m_displayWidget->resize(Width,Height);
     //m_displayWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
     m_layout = new QGridLayout(this);
     m_topImageLabel = new QLabel(this);
     m_rightImageLabel = new QLabel(this);
     m_frontImageLabel = new QLabel(this);

     installEventFilter(m_topImageLabel);
     installEventFilter(m_rightImageLabel);
     installEventFilter(m_frontImageLabel);
     //installEventFilter(this);
     //setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
     m_topImageLabel->installEventFilter(this);

     //m_topImageLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
     m_topImageLabel->setScaledContents(true);
     m_rightImageLabel->setScaledContents(true);
     m_frontImageLabel->setScaledContents(true);

     m_displayWidget->setLayout(m_layout);
     m_layout->addWidget(m_topImageLabel,0,0);
     m_layout->addWidget(m_rightImageLabel,0,1);
     m_layout->addWidget(m_frontImageLabel,1,0);

     //m_layout->setSizeConstraint(QLayout::SetFixedSize);
     setVisible(false);
     setBackgroundRole(QPalette::Dark);
     setFocusPolicy(Qt::ClickFocus);
 }

 void ImageViewer::setTopImage(const QImage &image)
 {
     qDebug()<<"Set Image";
     m_topImageLabel->setPixmap(QPixmap::fromImage(image));
     //m_topImageLabel->resize(image.size());
     m_topImage = image;
     setVisible(true);
     m_loaded = true;
     updateStretchFactor();
 }

 void ImageViewer::setRightImage(const QImage &image)
 {
     qDebug()<<"Set Right Image";
     m_rightImageLabel->setPixmap(QPixmap::fromImage(image));
     m_loaded = true;
     setVisible(true);
     m_rightImage = image;
     updateStretchFactor();
 }

 void ImageViewer::setFrontImage(const QImage &image)
 {
     qDebug()<<"Set Front Image";
     m_frontImageLabel->setPixmap(QPixmap::fromImage(image));
     m_loaded = true;
     setVisible(true);
     m_topImage = image;
     updateStretchFactor();
 }
 void ImageViewer::zoom(qreal factor)
 {
     if(factor > 2.00)
         factor = 2.00;
     else if(factor < 0.25)
         factor = 0.25;
     m_factor = factor;
     //qDebug()<<"after adjusting:"<<m_factor;
     //m_imageLabel->resize(factor*m_imageLabel->pixmap()->size());
     m_displayWidget->resize(factor*QSize(Width,Height));
    //qDebug()<<m_topImageLabel->size();
}

void ImageViewer::updateStretchFactor()
{
//    QSizePolicy topImagePolicy = m_topImageLabel->sizePolicy();
//    QSizePolicy rightImagePolicy = m_rightImageLabel->sizePolicy();
//    QSizePolicy frontImagePolicy = m_frontImageLabel->sizePolicy();

    QSize topImageSize ;
    if(m_topImageLabel->pixmap() != nullptr){
        topImageSize = m_topImageLabel->pixmap()->size();
     //   qDebug()<<"aaa";
    }
    QSize rightImageSize;
    if(m_rightImageLabel->pixmap()!= nullptr){
        rightImageSize = m_rightImageLabel->pixmap()->size();
   // qDebug()<<"asdf";
    }
    QSize frontImageSize;
    if(m_frontImageLabel->pixmap()!= nullptr){
        frontImageSize = m_frontImageLabel->pixmap()->size();
      //  qDebug()<<"asdfsadf";
    }

    m_layout->setColumnStretch(0,topImageSize.width());
    m_layout->setColumnStretch(1,rightImageSize.width());
    m_layout->setRowStretch(0,topImageSize.height());
    m_layout->setRowStretch(1,frontImageSize.height());
    //qDebug()<<topImageSize<<" "<<rightImageSize<<" "<<frontImageSize;
    //topImagePolicy.setHorizontalStretch(topImageSize.width());
    //rightImagePolicy.setHorizontalStretch(rightImageSize.width());
    //topImagePolicy.setVerticalStretch(topImageSize.height());
    //frontImagePolicy.setVerticalStretch(frontImageSize.height());
    // m_topImageLabel->setSizePolicy(topImagePolicy);
    // m_rightImageLabel->setSizePolicy(rightImagePolicy);
    // m_frontImageLabel->setSizePolicy(frontImagePolicy);
}

void ImageViewer::paintLine(const QPoint &begin, const QPoint &end, QPaintDevice *dev)
{
    m_painter.begin(dev);
    m_painter.drawLine(begin,end);
    m_painter.end();
}
//bool ImageViewer::event(QEvent *event)
//{
//    qDebug()<<"in event";

//     return QObject::event(event);
//}

ImageView::ImageView(QWidget *parent):QWidget(parent)
{
    m_view = new GraphicsView(this);
    m_scene = new GraphicsScene(this);
    m_view->setScene(m_scene);
    m_layout = new QGridLayout(this);
    m_layout->addWidget(m_view);
    //m_view->resize(QSize(500,500));
    m_view->scale(0.1,0.1);
    setLayout(m_layout);
}

void ImageView::setTopImage(const QImage &image)
{
    QGraphicsPixmapItem * item = m_scene->addPixmap(QPixmap::fromImage(image));
    item->setPos(500,500);
    m_scene->update();
}

void ImageView::setRightImage(const QImage &image)
{

}

void ImageView::setFrontImage(const QImage &image)
{

}

GraphicsView::GraphicsView(QWidget *parent):QGraphicsView(parent)
{

}

GraphicsScene::GraphicsScene(QObject *parent):QGraphicsScene(parent)
{

}
