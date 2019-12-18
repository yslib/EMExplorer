#include "pixelwidget.h"

#include <QLineEdit>
#include <QDebug>
#include <QLabel>

/**
 * \brief Constructs a widget used for pixel-level display for a certain \a type type in \a SliceEditorWidget
 * 
 * \sa SliceEditorWidget
 */
PixelWidget::PixelWidget(SliceType type,  SliceEditorWidget*widget, QWidget * parent) :
	AbstractSliceViewPlugin(type,widget,parent)
{
	m_cornerLabel.reset(new QLabel(this), &QObject::deleteLater);
	m_cornerLabel->setText(QString("..."));
	m_cornerLabel->setAlignment(Qt::AlignCenter);
	m_cornerLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	m_cornerLabel->resize(s_width, s_height);

	m_cornerLabel->setContentsMargins(0, 0, 0, 0);

	resize(360, 150);
}

void PixelWidget::setWidth(int width)
{
	Q_UNUSED(width);
}

void PixelWidget::setHeight(int height)
{
	Q_UNUSED(height);
}

void PixelWidget::setPosition(const QPoint &p)
{
	m_pos = p;
	changeValue(currentImage(sliceType()), m_pos);
}

void PixelWidget::resizeEvent(QResizeEvent* event)
{
	Q_UNUSED(event);
	changeLayout(size());
}
void PixelWidget::changeLayout(QSize areaSize)
{
	calcCount(areaSize);

	int horizontalCount = m_width, verticalCount = m_height;
	//qDebug() << m_width << " " << m_height;
	//set old index invalid
	m_minValueIndex = -1;
	m_maxValueIndex = -1;
	m_centroidIndex = -1;
	//clear previous labels and layout
	m_pixelLabels.clear();
	m_flags.clear();
	for (int i = 0; i < verticalCount; i++) {
		for (int j = 0; j < horizontalCount; j++) {
			QSharedPointer<QLineEdit> sharedPtr(new QLineEdit(this), &QObject::deleteLater);
			sharedPtr->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
			sharedPtr->setReadOnly(true);
			sharedPtr->setAlignment(Qt::AlignCenter);
			m_pixelLabels.push_back(sharedPtr);
			m_flags.push_back(false);

			//layout->addWidget(m_pixelLabels.back().data(),1+i,1+j);
			setWidget(m_pixelLabels.back().data(), 1 + i, 1 + j);
		}
	}
	//layout->addWidget(m_cornerLabel.data(),0,0);
	setWidget(m_cornerLabel.data(), 0, 0);
	//column and row header labels
	m_columnHeadersLabels.clear();
	for (int i = 0; i < horizontalCount; i++) {
		QSharedPointer<QLabel> ptr(new QLabel(this), &QObject::deleteLater);
		ptr->setText(QString("..."));
		ptr->setAlignment(Qt::AlignCenter);
		ptr->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

		//layout->addWidget(ptr.data(),0,i+1);
		setWidget(ptr.data(), 0, i + 1);
		m_columnHeadersLabels.push_back(ptr);
	}
	m_rowHeadersLabels.clear();
	for (int i = 0; i < verticalCount; i++) {
		QSharedPointer<QLabel> ptr(new QLabel(this), &QObject::deleteLater);
		ptr->setText(QString("..."));
		ptr->setAlignment(Qt::AlignCenter);
		ptr->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

		//layout->addWidget(ptr.data(),i+1,0);
		setWidget(ptr.data(), i + 1, 0);
		m_rowHeadersLabels.push_back(ptr);
	}
	//qDebug() << m_cornerLabel->size();
	changeValue(currentImage(sliceType()), m_pos);
}

/*
 * This function will update the image grayscale value at the new position
*/
void PixelWidget::changeValue(const QImage &image, const QPoint &pos)
{
	if (image.isNull())
		return;
	//reset previous label's color with min and max value
	if (m_minValueIndex != -1) {
		m_pixelLabels[m_minValueIndex]->setStyleSheet(QString());
	}
	if (m_maxValueIndex != -1) {
		m_pixelLabels[m_maxValueIndex]->setStyleSheet(QString());
	}
	if (m_centroidIndex != -1)
	{
		m_pixelLabels[m_centroidIndex]->setStyleSheet(QString());
	}
	for (int i = 0; i < m_pixelLabels.size(); i++)
		m_flags[i] = false;

	//Update grayscale value
	int imageWidth = image.width();
	int imageHeight = image.height();

	//start point on the image
	int startRow = pos.x() - m_width / 2;
	int startColumn = pos.y() - m_height / 2;
	bool flag = false;

	//update grayscale
	int minValueIndex = -1, maxValueIndex = -1;
	int minValue = std::numeric_limits<int>::max(), maxValue = std::numeric_limits<int>::lowest();
	;
	for (int i = startRow; i < startRow + m_height; i++) {
		for (int j = startColumn; j < startColumn + m_width; j++) {
			if (i >= 0 && i < imageWidth && j >= 0 && j < imageHeight) {
				//(i,j) are in the image
				int index = i * imageHeight + j;
				int id = (i - startRow)*m_width + (j - startColumn);
				//id is the index of the label in the m_pixels
				int value = static_cast<int>(*(image.bits() + index));
				if (minValue > value) {
					minValue = value;
					minValueIndex = id;
				}
				if (maxValue < value) {
					maxValue = value;
					maxValueIndex = id;
				}
				m_pixelLabels[id]->setText(QString::number(value));
				//
				m_pixelLabels[id]->setStyleSheet(QString("background-color: rgb(%1,%1,%1)").arg(value));
				m_flags[id] = true;
				if (!flag&&i == pos.x() && j == pos.y())
				{
					m_centroidIndex = id;
					flag = true;

				}
			}
		}
	}
	//clear invalid pos label
	for (int i = 0; i < m_flags.size(); i++)
	{
		if (!m_flags[i])
		{
			m_pixelLabels[i]->setText(QString(""));
			m_pixelLabels[i]->setStyleSheet(QString(""));
		}
	}
	//change the label's color with min and max value
	m_pixelLabels[m_minValueIndex = minValueIndex]->setStyleSheet(QStringLiteral("background-color: rgb(0,0,255)"));
	m_pixelLabels[m_maxValueIndex = maxValueIndex]->setStyleSheet(QStringLiteral("background-color: rgb(255,0,0)"));
	m_pixelLabels[m_centroidIndex]->setStyleSheet(QStringLiteral("background-color: rgb(0,255,0)"));
	//update headers
	for (int i = startColumn; i < startColumn + m_width; i++) {
		m_columnHeadersLabels[i - startColumn]->setText(QString::number(i));
	}
	for (int i = startRow; i < startRow + m_height; i++) {
		m_rowHeadersLabels[i - startRow]->setText(QString::number(i));
	}
}

void PixelWidget::calcCount(QSize areaSize)
{
	QSize size = areaSize;
	QSize pixelHolderSize = QSize(s_width + s_left + s_right, s_height + s_top + s_bottom);
	m_width = (static_cast<double>(size.width()) / pixelHolderSize.width()) - 1;
	m_height = (static_cast<double>(size.height()) / pixelHolderSize.height()) - 1;
	if (m_width < 0)m_width = 0;
	if (m_height < 0)m_height = 0;
}

void PixelWidget::setWidget(QWidget * widget, int xpos, int ypos)
{
	int width = widget->size().width();
	int height = widget->size().height();
	Q_UNUSED(width);
	Q_UNUSED(height);
	QPoint topLeft((s_left + s_width + s_right)*xpos + s_left, (s_top + s_height + s_bottom)*ypos + s_top);
	QRect rect(topLeft, QSize(s_width, s_height));
	widget->setContentsMargins(s_left, s_top, s_right, s_bottom);
	widget->setParent(this);
	widget->setGeometry(rect);
	widget->show();
}