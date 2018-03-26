 #include "sliceviewer.h"

SliceViewer::SliceViewer(QWidget * parent /* = nullptr */) :
	QWidget(parent),
	m_paintEnable{ false },
	m_painting{ false },
	m_imageRect{},
	m_shape{ Shape::Line },
	m_pen{},
	m_paintState {PaintState::All}
{
    //resize(WIDTH, HEIGHT);
    //resize(size);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	//setMinimumSize(WIDTH, HEIGHT);
    //resize(500,30);
	m_pen.setColor(Qt::black);
	m_pen.setWidth(5);
    setStyleSheet(QString("border:1px solid red"));
}
SliceViewer::SliceViewer(QWidget * parent, const QImage & image,const QRect & rect) :SliceViewer(parent)
{
    setImage(image,rect);
}
void SliceViewer::setImage(const QImage & image, const QRect & region ) {
    m_canvas = QPixmap::fromImage(image.convertToFormat(QImage::Format_ARGB32));

	if (region != QRect()) {
		m_imageRect = region;
	}
    update();
    updateGeometry();
}

void SliceViewer::addMark(const QPicture & mark)
{
	m_marks.push_back(mark);
	update();
	updateGeometry();
}

void SliceViewer::setMarks(const QVector<QPicture> &marks)
{
    m_marks= marks;
    update();
    updateGeometry();
}
inline QPoint SliceViewer::thisCoordToImageCoord(const QPoint & p)
{
	QSize windowSize = size();
	qreal kx = static_cast<qreal>(m_imageRect.width()) / static_cast<qreal>(windowSize.width());
	qreal ky = static_cast<qreal>(m_imageRect.height()) / static_cast<qreal>(windowSize.height());
	return QPoint(p.x()*kx+m_imageRect.x(), p.y()*ky+m_imageRect.y());
}

void SliceViewer::paintEvent(QPaintEvent *event) {

    Q_UNUSED(event);
        /*Repainting image decide by the rect and all marks*/
		//QImage canvas = m_image;
		//QImage canvas = m_image.convertToFormat(QImage::Format_ARGB32);

        //Painting marks that are already produced onto the m_canvas
		for (const QPicture & pic : m_marks) {
			QPainter canvasPainter(&m_canvas);
			canvasPainter.drawPicture(0, 0, pic);
		}

        //Clipping the image with zoom region and scaled it to the size of the widget to display
        QPainter painter(this);
        painter.drawPixmap(0, 0, m_canvas.copy(m_imageRect).scaled(size()));

        QPen pen(m_pen);
        //this is a factor for a proper pen size
		qreal k = static_cast<qreal>(m_canvas.width()) / static_cast<qreal>(m_imageRect.width());
		pen.setWidth(k*pen.width());
		painter.setPen(pen);
        //Painting the marks that are on drawing
		painter.drawPolyline(m_points.constData(), m_points.size());
}

void SliceViewer::mousePressEvent(QMouseEvent * event)
{
    if (m_paintEnable == true) {
        //Starting painting the marks
		m_painting = true;
		m_firstPoint = event->pos();
		m_prevPoint = m_firstPoint;
		switch (m_shape)
		{
		case SliceViewer::Shape::Line:
			m_points.clear();
			m_points.push_back(m_firstPoint);
			break;
		case SliceViewer::Shape::Ellipse:
			break;
		default:
			break;
		}
		//TODO:
		//Do some transform
	}
}

void SliceViewer::mouseReleaseEvent(QMouseEvent * event)
{
	if (m_paintEnable == true) {
		m_painting = false;
		m_lastPoint = event->pos();
		QPoint currentPoint = m_lastPoint;
		switch (m_shape)
		{
		case SliceViewer::Shape::Line:
			{
				m_prevPoint = currentPoint;
				QVector<QPoint> transformedCoords;
				for (const QPoint &p : m_points) {
                    transformedCoords.push_back(thisCoordToImageCoord(p));
				}
				QPicture pic;
				QPainter p(&pic);
				p.setPen(m_pen);
				p.drawPolygon(transformedCoords.constData(), transformedCoords.size());
				m_points.clear();
				update();
				emit drawingFinished(pic);
			}
			break;
		case SliceViewer::Shape::Ellipse:
			{
				QPoint center = (currentPoint - m_firstPoint)/2+m_firstPoint;
				QPoint r = currentPoint - m_firstPoint;
				QPicture pic;
				QPainter p(&pic);
				p.setPen(m_pen);
                QPoint transformedCenter = thisCoordToImageCoord(center);
                QPoint transformedLeftTop = thisCoordToImageCoord(m_firstPoint);
                QPoint transformedRightBottom = thisCoordToImageCoord(m_lastPoint);
				QPoint transformedR = transformedRightBottom - transformedLeftTop;
				p.drawEllipse(transformedCenter, transformedR.x(), transformedR.y());
				emit drawingFinished(pic);
			}
			break;
		default:
			break;
		}
	}
}

void SliceViewer::mouseMoveEvent(QMouseEvent * event)
{
    QPoint currentPoint = event->pos();
    emit onMouseMoving(thisCoordToImageCoord(currentPoint));
    qDebug()<<"mouse moving in slice viewer";
	if (m_paintEnable == true) {
		if (m_painting == true) {

			switch (m_shape)
			{
			case SliceViewer::Shape::Line:
			{
				m_points.push_back(currentPoint);
				m_prevPoint = currentPoint;
				update();
			}

				break;
			case SliceViewer::Shape::Ellipse:
			{
				m_paintState = PaintState::All;
				QPoint center = (currentPoint - m_firstPoint) / 2 + m_firstPoint;
				QPoint r = currentPoint - m_firstPoint;
                Q_UNUSED(r);
				update();
			}
				break;
			default:
				break;
			}
		}
	}

}

void SliceViewer::drawLineOnThis(const QPoint & p1, const QPoint & p2)
{
	m_linePoint1 = p1;
	m_linePoint2 = p2;
	m_paintState = PaintState::LineState;
	update();
}

void SliceViewer::drawEllipseOnThis(const QPoint & center, int rx, int ry)
{
	m_ellipseCenter = center;
	m_rx = rx;
	m_ry = ry;
	m_paintState = PaintState::EllipseState;
    update();
}

//void SliceViewer::createLayouts()
//{
//   m_gridLayout = new QGridLayout(this);
//   /*
//    * (0,0) for contral widget of axis 1
//    * (1,0) for display widget of axis 1
//    * (2,0) for display widget of axis 2
//    * (3,0) for contral widget of axis 2
//    * (1,2) for display widget of axis 3
//    * (1,3) for contral widget of axis 3
//    */

//   //Now,we just implement widgets for axis 1
//   m_gridLayout->addWidget(m_axis1SliceContralWidget,0,0);
//   m_gridLayout->addWidget(m_axis1SliceDisplayWidget,1,0);


//   setLayout(m_gridLayout);
//}


NestedSliceViewer::NestedSliceViewer(const QSize &mainSize, const QSize &rightSize, const QSize &frontSize, QWidget *parent):
    QWidget(parent){

    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setMargin(30);

    //m_gridLayout->setSpacing(10);
    m_mainEasySlider = new TitledSliderWithSpinBox(this,tr("T:"));
    m_mainSliceViewer = new SliceViewer(this);

    m_rightSliceViewer = new SliceViewer(this);
    m_rightEasySlider = new TitledSliderWithSpinBox(this,tr("L:"));

    m_frontSliceViewer = new SliceViewer(this);
    m_frontEasySlider = new TitledSliderWithSpinBox(this,tr("F:"));

    //qDebug()<<"In NestedSliceViewer Constructor"<<m_frontSliceViewer->size();

    setLayout(m_gridLayout);

    //main slice
    m_gridLayout->addWidget(m_mainEasySlider,0,0);
    m_gridLayout->addWidget(m_mainSliceViewer,1,0,1,3);

    //right slice
    m_gridLayout->addWidget(m_rightSliceViewer,1,3,1,1);
    m_gridLayout->addWidget(m_rightEasySlider,0,1);

    //front slice
    m_gridLayout->addWidget(m_frontSliceViewer,2,0,1,3);
    m_gridLayout->addWidget(m_frontEasySlider,0,2);

    connect(m_mainSliceViewer,SIGNAL(onDrawing(const QPoint&)),this,SIGNAL(onDrawing(const QPoint &)));
    connect(m_mainSliceViewer,SIGNAL(drawingFinished(QPicture)),this,SIGNAL(drawingFinished(const QPicture&)));
    connect(m_mainSliceViewer,SIGNAL(onMouseMoving(const QPoint&)),this,SIGNAL(onMouseMoving(const QPoint&)));
	connect(m_mainEasySlider, SIGNAL(valueChanged(int)), this, SIGNAL(ZSliderChanged(int)));
	connect(m_frontEasySlider, SIGNAL(valueChanged(int)), this, SIGNAL(YSliderChanged(int)));
	connect(m_rightEasySlider, SIGNAL(valueChanged(int)), this, SIGNAL(XSliderChanged(int)));
}

void NestedSliceViewer::setImage(const QImage &image, const QRect &region)
{
    m_mainSliceViewer->setImage(image,region);
}

void NestedSliceViewer::setRightImage(const QImage &image)
{
    m_rightSliceViewer->setImage(image);
}

void NestedSliceViewer::setFrontImage(const QImage &image)
{
    m_frontSliceViewer->setImage(image);
}
void NestedSliceViewer::addMark(const QPicture &mark)
{
    m_mainSliceViewer->addMark(mark);
}
void NestedSliceViewer::setMarks(const QVector<QPicture> &marks){
    m_mainSliceViewer->setMarks(marks);
}
void NestedSliceViewer::setMarkColor(const QColor &color){
    m_mainSliceViewer->setMarkColor(color);
}
QColor NestedSliceViewer::getMarkColor()const{
    return m_mainSliceViewer->getMarkColor();
}

void NestedSliceViewer::setMaximumImageCount(int main,int right,int front)
{
	m_mainEasySlider->setMaximum(main);
	m_rightEasySlider->setMaximum(right);
	m_frontEasySlider->setMaximum(front);
}

void NestedSliceViewer::setEnable(bool enable)
{
    m_mainEasySlider->setEnabled(enable);
	m_mainSliceViewer->setEnabled(enable);
	m_rightEasySlider->setEnabled(enable);
	m_rightSliceViewer->setEnabled(enable);
	m_frontEasySlider->setEnabled(enable);
	m_frontSliceViewer->setEnabled(enable);
}

void NestedSliceViewer::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event);
    int width = m_mainSliceViewer->width();
    int height = m_mainSliceViewer->height();
    int depth = 30;
    m_rightSliceViewer->resize(depth,height);
    m_frontSliceViewer->resize(width,depth);
    qDebug()<<"frontSliceViewerSizeHint"<<m_frontSliceViewer->size();
}

