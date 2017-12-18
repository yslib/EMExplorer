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
	resize(WIDTH, HEIGHT);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	setMinimumSize(WIDTH, HEIGHT);
	m_pen.setColor(Qt::black);
	m_pen.setWidth(5);
}
SliceViewer::SliceViewer(QWidget * parent, const QImage & image,const QRect & rect) :SliceViewer{ parent } 
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

void SliceViewer::setGrayscaleStrechingLowerBound(int value)
{

}
void SliceViewer::setGrayscaleStrechingUpperBound(int value)
{
}

inline QPoint SliceViewer::_thisCoordToImageCoord(const QPoint & p)
{
	QSize windowSize = size();
	qreal kx = static_cast<qreal>(m_imageRect.width()) / static_cast<qreal>(windowSize.width());
	qreal ky = static_cast<qreal>(m_imageRect.height()) / static_cast<qreal>(windowSize.height());
	return QPoint(p.x()*kx+m_imageRect.x(), p.y()*ky+m_imageRect.y());
}

void SliceViewer::paintEvent(QPaintEvent *event) {
		/*Repainting image decide by the rect and all marks*/
		//QImage canvas = m_image;
		//QImage canvas = m_image.convertToFormat(QImage::Format_ARGB32);
		for (const QPicture & pic : m_marks) {
			QPainter canvasPainter(&m_canvas);
			canvasPainter.drawPicture(0, 0, pic);
		}
		QPainter painter(this);
		painter.drawPixmap(0, 0, m_canvas.copy(m_imageRect).scaled(size()));

		QPen pen(m_pen);
		qreal k = static_cast<qreal>(m_canvas.width()) / static_cast<qreal>(m_imageRect.width());
		pen.setWidth(k*pen.width());
		painter.setPen(pen);
		painter.drawPolyline(m_points.constData(), m_points.size());
		painter.end();
}

void SliceViewer::mousePressEvent(QMouseEvent * event)
{
	if (m_paintEnable == true) {
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
					transformedCoords.push_back(_thisCoordToImageCoord(p));
				}
				QPicture pic;
				QPainter p(&pic);
				p.setPen(m_pen);
				p.drawPolyline(transformedCoords.constData(), transformedCoords.size());
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
				QPoint transformedCenter = _thisCoordToImageCoord(center);
				QPoint transformedLeftTop = _thisCoordToImageCoord(m_firstPoint);
				QPoint transformedRightBottom = _thisCoordToImageCoord(m_lastPoint);
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
	if (m_paintEnable == true) {
		if (m_painting == true) {
			QPoint currentPoint = event->pos();
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
				update();
			}
				break;
			default:
				break;
			}
		}
	}

}

void SliceViewer::_drawLineOnThis(const QPoint & p1, const QPoint & p2)
{
	m_linePoint1 = p1;
	m_linePoint2 = p2;
	m_paintState = PaintState::LineState;
	update();
}

void SliceViewer::_drawEllipseOnThis(const QPoint & center, int rx, int ry)
{
	m_ellipseCenter = center;
	m_rx = rx;
	m_ry = ry;
	m_paintState = PaintState::EllipseState;
	update();
}

