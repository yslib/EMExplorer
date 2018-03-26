#include "imageviewer.h"
#include <QLabel>
#include <QLayout>
#include <QWheelEvent>
#include <QDebug>
bool ImageViewer::eventFilter(QObject *obj, QEvent *event)
{
    qDebug()<<"eventFilter";
    if(obj == m_imageLabel){
        qDebug()<<"asdfsafaaa66666666666666";
        if(event->type() == QEvent::Wheel){
            qDebug()<<"asdfsadfsaf";
            return false;
        }else{
            return true;
        }
    }else if(obj == this){
        qDebug()<<"obj is this";
        return true;
    }
    qDebug()<<"aaaaaaaaaaaasdfsadf";
    return QObject::eventFilter(obj,event);
}

void ImageViewer::wheelEvent(QWheelEvent *event)
{
    qDebug()<<"WheelEvent";
    if(event->orientation() == Qt::Horizontal){

    }else{
        if(m_loaded == true){
            if(event->delta() > 0){
               zoom(m_factor + 0.25);
               qDebug()<<"wheel > 0";
           }else{
            zoom(m_factor - 0.25);
               qDebug()<<"Wheel < 0";
           }

        }
     }
}

ImageViewer::ImageViewer(QWidget *parent) : QWidget(parent),m_factor(1.0),m_loaded(false)
{
    m_layout = new QGridLayout(this);
    m_imageLabel = new QLabel(this);
    installEventFilter(this);
    m_imageLabel->installEventFilter(this);
    m_imageLabel->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
    m_imageLabel->setScaledContents(true);
    m_layout->addWidget(m_imageLabel,0,0);
    setVisible(false);
    setBackgroundRole(QPalette::Dark);
    setFocusPolicy(Qt::ClickFocus);
}

void ImageViewer::setImage(const QImage &image)
{
    m_image = image;
    m_imageLabel->setPixmap(QPixmap::fromImage(m_image));
    //m_loaded = true;
}

void ImageViewer::zoom(qreal factor)
{
    if(factor > 2.00)
        factor = 2.00;
    else if(factor < 0.25)
        factor = 0.25;
    m_factor = factor;
    m_imageLabel->resize(factor*m_imageLabel->pixmap()->size());
}
