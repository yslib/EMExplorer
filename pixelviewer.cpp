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

void PixelViewer::setPosition(const QPoint &p)
{
    m_pos = p;
    changeValue(m_image,m_pos);
}



void PixelViewer::changeLayout(int width, int height)
{
   // for(auto item:m_pixels){
   //     delete item;
   // }
    m_pixelLabels.clear();
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            QSharedPointer<QLabel> sharedPtr(new QLabel(this));
            m_pixelLabels.push_back(sharedPtr);
            layout->addWidget(m_pixelLabels.back().data(),1+i,1+j);
        }
    }
    /*
     * Update column and row headers
     */
    m_cornerLabel.reset(new QLabel(this));
    m_cornerLabel->setText(QString("..."));
    m_columnHeadersLabels.clear();
    for(int i=0;i<m_width;i++){
        QSharedPointer<QLabel> ptr(new QLabel(this));
        ptr->setText(QString("..."));
        layout->addWidget(ptr.data(),0,i+1);
        m_columnHeadersLabels.push_back(ptr);
    }
    m_rowHeadersLabels.clear();
    for(int i = 0;i<m_height;i++){
        QSharedPointer<QLabel> ptr(new QLabel(this));
        ptr->setText(QString("..."));
        layout->addWidget(ptr.data(),i+1,0);
        m_rowHeadersLabels.push_back(ptr);
    }
    changeValue(m_image,m_pos);
}

/*
 * This function will update the image grayscale value at the new position
*/
void PixelViewer::changeValue(const QImage &image, const QPoint &pos)
{
    if(pos.isNull() == false && image.isNull() == false){
        //Update grayscale value
        int imageWidth = image.width();
        int imageHeight = image.height();

        //start point on the image
        int startRow = pos.x()-m_width/2;
        int startColumn = pos.y()-m_height/2;


        //update grayscale
        for(int i=startRow;i<startRow+m_width;i++){
            for(int j = startColumn;j<startColumn+m_height;j++){
                if(i>=0 && i<imageWidth && j>=0&&j<imageHeight){
                    //(i,j) are in the image
                    int index = i*imageHeight+j;
                    int id = (i-startRow)*m_height + (j-startColumn);
                    //id is the index of the label in the m_pixels
                    m_pixelLabels[id]->setText(QString::number(
                                             static_cast<int>(*(image.bits()+index))
                                         )
                                         );
                }
            }
        }
        //update headers
        for(int i = startColumn;i<startColumn+m_height;i++){
            m_columnHeadersLabels[i-startColumn]->setText(QString::number(i));
        }
        for(int i=startRow;i<startRow+m_width;i++){
            m_rowHeadersLabels[i-startRow]->setText(QString::number(i));
        }
    }
}
