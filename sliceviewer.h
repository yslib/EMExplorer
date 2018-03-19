#ifndef SLICEVIEWER_H_
#define SLICEVIEWER_H_
#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QMouseEvent>
#include <QPicture>
#include <QDebug>
#include <QVector>
#include <QSlider>
#include <QSpinBox>
#include <QLayout>
#include "titledsliderwithspinbox.h"

class SliceViewer :public QWidget
{
	Q_OBJECT
public:
	enum class Shape{Line,Ellipse};
	enum class PaintState{LineState,EllipseState,All};
public:
    SliceViewer(QWidget * parent = nullptr);
	SliceViewer(QWidget * parent, const QImage & image,const QRect & rect = QRect());
    void setImage(const QImage & image,const QRect & region = QRect());
	void addMark(const QPicture & mark);
    void setMarks(const QVector<QPicture> & marks);
	void originalSize(bool original = false) { m_originalSize = original; }
	void setMarkColor(const QColor & color) { m_pen.setColor(color); }
	QColor getMarkColor()const { return m_pen.color(); }
	void setPenWidth(int w) { m_pen.setWidth(w); }
	void setShape(Shape shape) { m_shape = shape; }
    Shape getShape()const { return m_shape; }
public slots:
	void paintEnable(bool enable) { m_paintEnable = enable; }
private:
    QPoint thisCoordToImageCoord(const QPoint & p);
    void drawLineOnThis(const QPoint &p1, const QPoint &p2);
    void drawEllipseOnThis(const QPoint & center,int rx,int ry);

protected:
	void paintEvent(QPaintEvent *event)override;
	void mousePressEvent(QMouseEvent *event)override;
	void mouseReleaseEvent(QMouseEvent *event)override;
	void mouseMoveEvent(QMouseEvent *event)override;
signals:
	void onDrawing(const QPoint & point);
	void drawingFinished(const QPicture & points);
    void onMouseMoving(const QPoint & pos);
private:
    //Size of the Widget
	const int WIDTH = 500;
	const int HEIGHT = 500;
    //states for painting
	bool m_originalSize;
	bool m_paintEnable;
	bool m_painting;
    //Some temp varibles
	Shape m_shape;
	PaintState m_paintState;
	QPoint m_linePoint1, m_linePoint2;
	QPoint m_ellipseCenter;
    int m_rx, m_ry;
    //Temp points generated during painting
	QVector<QPoint> m_points;
	QPoint m_prevPoint;
	QPoint m_firstPoint;
	QPoint m_lastPoint;
    //Pen properties
	QPen m_pen;
    //canvas for painting
    QPixmap m_canvas;

    //a zoom rect for certain area of the image
    QRect m_imageRect;
    //marks displayed on the widgets
    QVector<QPicture> m_marks;

    QGridLayout * m_gridLayout;
};


class NestedSliceViewer:public QWidget{
    Q_OBJECT
public:
    NestedSliceViewer(const QSize &mainSize, const QSize &rightSize, const QSize &frontSize, QWidget * parent= nullptr);
    void setImage(const QImage & image,const QRect & region = QRect());
    void setRightImage(const QImage & image);
    void setFrontImage(const QImage & image);
    void addMark(const QPicture & mark);
    void setMarks(const QVector<QPicture> & marks);
    void setMarkColor(const QColor & color);
    QColor getMarkColor()const;
	void setMaximumImageCount(int main = 0, int right = 0, int front = 0);
	void setEnable(bool enable);

    int getXSliceValue()const{return m_rightEasySlider->value();}
    int getYSliceValue()const{return m_frontEasySlider->value();}
    int getZSliceValue()const{return m_mainEasySlider->value();}
public slots:
    void paintEnable(bool enable){return m_mainSliceViewer->paintEnable(enable);}
signals:
    void onDrawing(const QPoint & point);
    void drawingFinished(const QPicture & points);
    void onMouseMoving(const QPoint & pos);
	void XSliderChanged(int index);
	void YSliderChanged(int index);
	void ZSliderChanged(int index);
protected:
	void resizeEvent(QResizeEvent* event) override;
private:
    QGridLayout * m_gridLayout;
    SliceViewer * m_mainSliceViewer;
    TitledSliderWithSpinBox * m_mainEasySlider;
    SliceViewer * m_rightSliceViewer;
    TitledSliderWithSpinBox * m_rightEasySlider;
    SliceViewer * m_frontSliceViewer;
    TitledSliderWithSpinBox * m_frontEasySlider;
};


#endif // !SLICEVIEWER_H_
