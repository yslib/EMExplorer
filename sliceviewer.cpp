#include "sliceviewer.h"

SliceViewer::SliceViewer(QWidget * parent /* = nullptr */) :
	QWidget(parent),
m_paintEnable{ false },
m_image{QImage()},
m_painting{ false },
m_imageRect{}
{
	resize(WIDTH, HEIGHT);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	setMinimumSize(WIDTH, HEIGHT);
}
SliceViewer::SliceViewer(QWidget * parent, const QImage & image,const QRect & rect) :SliceViewer{ parent } 
{
	setImage(image,rect);
}
void SliceViewer::setImage(const QImage & image,const QRect & rect ) {
	m_image = image;
	m_imageRect = rect;
	update();
	updateGeometry();
}

void SliceViewer::setMark(const QPicture & mark)
{
    m_mark = mark;
	qDebug() << m_mark.size();
	qDebug() << "Mark Setted\n";
	update();
    updateGeometry();
}

void SliceViewer::setMarks(const QVector<QPicture> &marks)
{
    m_marks= marks;
    update();
    updateGeometry();
}

void SliceViewer::setGrayscaleStrechingLowerBound(int value)
{

}
void SliceViewer::setGrayscaleStrechingUpperBound(int value)
{
}
void SliceViewer::paintEvent(QPaintEvent *event) {
	QImage canvas = m_image;
    for(const QPicture & pic:m_marks){
        QPainter canvasPainter(&canvas);
        canvasPainter.drawPicture(0, 0, pic);
    }
    QPainter painter(this);
	painter.drawImage(0, 0, canvas.copy(m_imageRect).scaled(size()));
	painter.end();
}

void SliceViewer::mousePressEvent(QMouseEvent * event)
{
	if (m_paintEnable == true) {
		m_painting = true;
		//TODO:
		//Do some transform
	}
}

void SliceViewer::mouseReleaseEvent(QMouseEvent * event)
{
	if (m_paintEnable == true) {
		m_painting = false;
	}
}

void SliceViewer::mouseMoveEvent(QMouseEvent * event)
{
	if (m_paintEnable == true) {
		if (m_painting == true) {
			QSize windowSize = size();
			qreal kx = static_cast<qreal>(m_imageRect.width()) / static_cast<qreal>(windowSize.width());
			qreal ky = static_cast<qreal>(m_imageRect.height()) / static_cast<qreal>(windowSize.height());
			QPoint point(event->x()*kx, event->y()*ky);
			qDebug() << "P:" << point;
			emit onDrawing(point);
			qDebug() << "mouse moving\n";
		}
	}

}