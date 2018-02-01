#include "pixelviewer.h"

PixelViewer::PixelViewer(QWidget *parent,int width, int height,const QImage & image ):
    m_width(width),m_height(height),m_image(image),
    QWidget(parent)
{

    //create a gridlayout
    layout = new QGridLayout(this);
    setLayout(layout);

    //init layout
    changeLayout(m_width,m_height);
}

void PixelViewer::setWidth(int width)
{
    if(m_width != width){
        m_width = width;
        changeLayout(m_width,m_height);
    }
}

void PixelViewer::setHeight(int height)
{
    if(m_height != height){
        m_height = height;
        changeLayout(m_width,m_height);
    }
}

void PixelViewer::setImage(const QImage &image)
{
    m_image = image;
    changeValue(m_image,m_pos);
}

void PixelViewer::onPositionChanged(const QPoint &p)
{
    m_pos = p;
    changeValue(m_image,m_pos);
}

void PixelViewer::changeLayout(int width, int height)
{
    for(auto item:m_pixels){
        delete item;
    }
    m_pixels.clear();
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            m_pixels.push_back(new QLabel(this));
            layout->addWidget(m_pixels.back(),1+i,1+j);
        }
    }
    //TEST:
    //
    changeValue(m_image,m_pos);
}
void PixelViewer::changeValue(const QImage &image, const QPoint &pos)
{
    if(pos.isNull() == false && image.isNull() == false){

        int imageWidth = image.width();
        int imageHeight = image.height();

        //start point on the image
        int startRow = pos.x()-m_width/2;
        int startColumn = pos.y()-m_height/2;

        for(int i=startRow;i<startRow+m_width;i++){
            for(int j = startColumn;j<startColumn+m_height;j++){
                if(i>=0 && i<imageWidth && j>=0&&j<imageHeight){
                    //(i,j) are in the image
                    int index = i*imageHeight+imageWidth;
                    int id = (i-startRow)*m_height + m_width;
                    //id is the index of the label in the m_pixels
//                    m_pixels[id]->setText(QString::number(
//                                             static_cast<int>(*(image.bits()+index))
//                                                         )
//                                         );
                    m_pixels[id]->setText(QString("255"));


                }
            }
        }
    }
}


