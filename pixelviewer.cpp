#include "pixelviewer.h"
#include <QVector>
#include <QVariant>

PixelViewer::PixelViewer(QWidget *parent,int width, int height,const QImage & image ):
    m_width(width),m_height(height),m_image(image),m_model(nullptr),
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

void PixelViewer::setModel(DataItemModel * model)
{
	if(m_model != model)
	{
		m_model = model;
		disconnect(m_model, &DataItemModel::dataChanged, this, &PixelViewer::dataChanged);
		connect(m_model, &DataItemModel::dataChanged, this, &PixelViewer::dataChanged);

		///TODO::get corresponding data
		/*
		 * We don't need to get coreesponding data here for now,because this is not a strict MVC framework
		 * So far,we only need activateItem(...) method to activate pull data
		 */
	}
}

void PixelViewer::activateItem(const QModelIndex & index)
{
	if(m_model == nullptr)
	{
		qWarning("Model is empty.");
		return;
	}

	QVariant var = m_model->data(getDataIndex(index));
	if(var .canConvert<QSharedPointer<ItemContext>>() == true)
	{
		m_activedIndex = index;
		auto p = var.value<QSharedPointer<ItemContext>>();
		m_ptr = p;
		int currentSlice = p->getCurrentSliceIndex();
		setImage(p->getOriginalSlice(currentSlice));
	}

}

void PixelViewer::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
	///TODO::fecth updated currentSlice
	qDebug("PixelViewer:data model has been updated");


}

void PixelViewer::setPosition(const QPoint &p)
{
    m_pos = p;
    changeValue(m_image,m_pos);
}


void PixelViewer::changeLayout(int width, int height)
{
    //set old index invalid
    m_minValueIndex = -1;
    m_maxValueIndex = -1;

    //clear previous labels
    m_pixelLabels.clear();
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            QSharedPointer<QPushButton> sharedPtr(new QPushButton(this));
            sharedPtr->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
            m_pixelLabels.push_back(sharedPtr);
            layout->addWidget(m_pixelLabels.back().data(),1+i,1+j);
        }
    }

    //cornel label
    m_cornerLabel.reset(new QLabel(this));
    m_cornerLabel->setText(QString("..."));
    m_cornerLabel->setAlignment(Qt::AlignCenter);
    m_cornerLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    layout->addWidget(m_cornerLabel.data(),0,0);

    //column and row header labels

    m_columnHeadersLabels.clear();
    for(int i=0;i<m_width;i++){
        QSharedPointer<QLabel> ptr(new QLabel(this));
        ptr->setText(QString("..."));
        ptr->setAlignment(Qt::AlignCenter);
        ptr->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
        layout->addWidget(ptr.data(),0,i+1);
        m_columnHeadersLabels.push_back(ptr);
    }
    m_rowHeadersLabels.clear();
    for(int i = 0;i<m_height;i++){
        QSharedPointer<QLabel> ptr(new QLabel(this));
        ptr->setText(QString("..."));
        ptr->setAlignment(Qt::AlignCenter);
        ptr->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
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
        //reset previous label's color with min and max value
        if(m_minValueIndex != -1){
            m_pixelLabels[m_minValueIndex]->setStyleSheet(QString());
        }
        if(m_maxValueIndex != -1){
            m_pixelLabels[m_maxValueIndex]->setStyleSheet(QString());
        }
        //Update grayscale value
        int imageWidth = image.width();
        int imageHeight = image.height();

        //start point on the image
        int startRow = pos.x()-m_width/2;
        int startColumn = pos.y()-m_height/2;

        //update grayscale
        int minValueIndex = -1,maxValueIndex = -1;
        int minValue = std::numeric_limits<int>::max(),maxValue = std::numeric_limits<int>::lowest();
        for(int i=startRow;i<startRow+m_width;i++){
            for(int j = startColumn;j<startColumn+m_height;j++){
                if(i>=0 && i<imageWidth && j>=0&&j<imageHeight){
                    //(i,j) are in the image
                    int index = i*imageHeight+j;
                    int id = (i-startRow)*m_height + (j-startColumn);
                    //id is the index of the label in the m_pixels
                    int value = static_cast<int>(*(image.bits()+index));
                    if(minValue > value){
                        minValue = value;
                        minValueIndex = id;
                    }
                    if(maxValue < value){
                        maxValue = value;
                        maxValueIndex = id;
                    }
                    m_pixelLabels[id]->setText(QString::number(value));
                }
            }
        }
        //change the label's color with min and max value
        m_pixelLabels[minValueIndex]->setStyleSheet(QString("background-color: rgb(0,0,255)"));
        m_pixelLabels[maxValueIndex]->setStyleSheet(QString("background-color: rgb(255,0,0)"));
        m_minValueIndex = minValueIndex;
        m_maxValueIndex = maxValueIndex;

        //update headers
        for(int i = startColumn;i<startColumn+m_height;i++){
            m_columnHeadersLabels[i-startColumn]->setText(QString::number(i));
        }
        for(int i=startRow;i<startRow+m_width;i++){
            m_rowHeadersLabels[i-startRow]->setText(QString::number(i));
        }
    }
}

QModelIndex PixelViewer::getDataIndex(const QModelIndex & itemIndex)
{
	if (m_model == nullptr)
	{
		qWarning("Model is empty.");
	}
	return m_model->index(itemIndex.row(), 1, m_model->parent(itemIndex));
}
