#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H
#include <QWidget>
#include <QScrollArea>
#include <QImage>
#include <QPainter>
#include <QPicture>
#include <QVector>
#include <QList>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QScopedPointer>
#include <memory>
QT_BEGIN_NAMESPACE
class QGridLayout;
class QLabel;
class QWheelEvent;
class QGraphicsView;
class QGraphicsScene;
class QMouseEvent;
QT_END_NAMESPACE

class ImageViewer : public QScrollArea
{
    Q_OBJECT
    QWidget * m_displayWidget;

    QLabel *m_topImageLabel;
    QImage m_topImage;
    bool m_topImageEnablePaint;
    QVector<QPicture> m_marksOnTop;

    QLabel *m_rightImageLabel;
    QImage m_rightImage;
    bool m_rightImageEnablePaint;
    QVector<QPicture> m_marksOnRight;

    QLabel *m_frontImageLabel;
    QImage m_frontImage;
    bool m_frontImageEnablePaint;
    QVector<QPicture> m_marksOnFront;

    QGridLayout * m_layout;
    qreal m_factor;
    bool m_loaded;

    QPoint m_firstPoint;
    QPoint m_prevPaint;
    QList<QPoint> m_pointBuffer;



    QPainter m_painter;

    static constexpr int Width= 500;
    static constexpr int Height = 500;
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void wheelEvent(QWheelEvent * event);
    void paintEvent(QPaintEvent *event);
public:
    explicit ImageViewer(QWidget *parent = nullptr);
    void setTopImage(const QImage & image);
    void setRightImage(const QImage & image);
    void setFrontImage(const QImage & image);

//    void setMarksOnTopImage(const QVector<QPicture> & marks);
//    void addMarksOnTopImage(const QPicture & mark);
//    void setMarksOnRightImage(const QVector<QPicture> & marks);
//    void addMarksOnTopImage(const QPicture & mark);
//    void setMarksOnFrontImage(const QVector<QPicture> & marks);
//    void addMarksOnFrontImage(const QPicture & mark);


    void zoom(qreal factor);
    //bool event(QEvent *event)override;
signals:
private:
    void updateStretchFactor();
    void paintLine(const QPoint &begin,const QPoint &end,QPaintDevice * dev);

public slots:
};


class StrokeMark:public QGraphicsItem{
    QRectF m_boundingRect;
    QPainterPath m_painterPath;
    QList<QPoint> m_points;
public:
    StrokeMark(QGraphicsItem * parent = nullptr);
    QRectF boundingRect()const override;
    QPainterPath shape()const override;
    void paint(QPainter * painter,const QStyleOptionGraphicsItem * option,QWidget * widget)override;
};

class GraphicsScene:public QGraphicsScene
{
public:
    GraphicsScene(QObject * parent =nullptr);
protected:

};

class GraphicsView:public QGraphicsView
{
    qreal m_scaleFactor;
    bool m_paint;
    QList<QPoint> m_points;
public:
    GraphicsView(QWidget * parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent * event)override;
    void mouseMoveEvent(QMouseEvent * event)override;
    void mouseReleaseEvent(QMouseEvent * event)override;
    void wheelEvent(QWheelEvent * event)override;
};

class ImageView:public QWidget
{
    Q_OBJECT
    GraphicsView * m_view;
    GraphicsScene * m_scene;
    QGridLayout *m_layout;
    QGraphicsPixmapItem * m_slice;
    //QList<QPoint> m_points;
    bool m_paint;
public:
    ImageView(QWidget * parent = nullptr);
    void setTopImage(const QImage &image);
    void setRightImage(const QImage &image);
    void setFrontImage(const QImage & image);
public slots:
};



#endif // IMAGEVIEWER_H
