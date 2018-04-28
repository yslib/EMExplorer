#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H
#include <QMainWindow>
#include <QImage>
#include <QVector>
#include <QList>
#include <QGraphicsView>
#include <QGraphicsItem>
#include "ItemContext.h"

QT_BEGIN_NAMESPACE
class QGridLayout;
class QLabel;
class QWheelEvent;
class QGraphicsView;
class QGraphicsScene;
class QMouseEvent;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;
QT_END_NAMESPACE

class TitledSliderWithSpinBox;

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


enum ItemTypes
{
	Slice =1 ,
	Mark
};

class StrokeMarkItem:public QGraphicsItem{
    QRectF m_boundingRect;
    QPainterPath m_painterPath;
    QList<QPointF> m_points;
public:
	enum { Type = UserType + Mark };
    StrokeMarkItem(QGraphicsItem * parent = nullptr);
	QRectF boundingRect()const override { return m_boundingRect; }
	QPainterPath shape()const override { return QPainterPath(); }
	void addPoint(const QPointF & p);
    void paint(QPainter * painter,const QStyleOptionGraphicsItem * option,QWidget * widget)override;
	int type() const override { return Type; }
private:
	QRectF unionWith(const QRectF & rect,const QPointF & p);
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
	void wheelEvent(QGraphicsSceneWheelEvent* event) override;
};

class SliceItem:public QGraphicsPixmapItem
{
public:
	enum{Type = UserType + Slice};
	SliceItem(QGraphicsItem * parent = nullptr):QGraphicsPixmapItem(parent){}
	SliceItem(const QPixmap & pixmap, QGraphicsItem * parent = nullptr):QGraphicsPixmapItem(pixmap,parent){}
	int type() const override { return Type; }
	virtual  ~SliceItem() = default;
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
	void wheelEvent(QGraphicsSceneWheelEvent* event) override;
};


class GraphicsScene:public QGraphicsScene
{
public:
    GraphicsScene(QObject * parent =nullptr);
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
	void wheelEvent(QGraphicsSceneWheelEvent* event) override;
};
class GraphicsView:public QGraphicsView
{
	Q_OBJECT
	GraphicsScene *m_scene;

    qreal m_scaleFactor;
    QVector<QPoint> m_paintViewPointsBuffer;
	SliceItem * m_currentPaintItem;
	bool m_paint;
	bool m_moveble;
	QPointF m_prevScenePoint;
	QColor m_color;
	SliceItem * m_topSlice;
	SliceItem * m_rightSlice;
	SliceItem * m_frontSlice;

public:
    GraphicsView(QWidget * parent = nullptr);
public slots:
	void paintEnable(bool enable) { m_paint = enable; }
	void moveEnable(bool enable) { m_moveble = enable; }
	void setTopImage(const QImage &image);
	void setRightImage(const QImage &image);
	void setFrontImage(const QImage & image);
	void setColor(const QColor & color) { m_color = color;}
protected:
    void mousePressEvent(QMouseEvent * event)override;
    void mouseMoveEvent(QMouseEvent * event)override;
    void mouseReleaseEvent(QMouseEvent * event)override;
    void wheelEvent(QWheelEvent * event)override;

signals:
	void zSliceSelected(const QPoint & point);
	void ySliceSelected(const QPoint & point);
	void xSliceSelected(const QPoint & point);
};
class ImageView:public QWidget
{
	Q_OBJECT
public:
    ImageView(QWidget * parent = nullptr);
	//MVC pattern will be employed later and these function will be removed
	int getZSliceValue()const;
	int getYSliceValue()const;
	int getXSliceValue()const;
	void setModel(DataItemModel * model);
	void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
	void activateItem(const QModelIndex & index);
signals:
	void ZSliderChanged(int value);
	void YSliderChanged(int value);
	void XSliderChanged(int value);
	void zSliceSelected(const QPoint & point);
	void ySliceSelected(const QPoint & point);
	void xSliceSelected(const QPoint & point);
public slots:
	void setEnabled(bool enable);

	void onZSliderValueChanged(int value);
	void onYSliderValueChanged(int value);
	void onXSliderValueChanged(int value);

	void onTopSliceTimer(bool enable);
	void onRightSliceTimer(bool enable);
	void onFrontSliceTimer(bool enable);
	void onColorChanged();
protected:
	void timerEvent(QTimerEvent* event) override;

private:
	QModelIndex getDataIndex(const QModelIndex & itemIndex);
	void updateModel();

	void createActions();
	void updateActions();


	void setTopSliceCount(int value);
	void setRightSliceCount(int value);
	void setFrontSliceCount(int value);
	void setTopImage(const QImage &image);
	void setRightImage(const QImage &image);
	void setFrontImage(const QImage & image);



	enum class Direction {
		Forward,
		Backward
	};

	//Model
	QAbstractItemModel * m_model;
	QModelIndex m_modelIndex;
	QSharedPointer<ItemContext> m_ptr;
	//------
	
	QGridLayout *m_layout;
	GraphicsView * m_view;
	//GraphicsScene * m_scene;

	QToolBar * m_toolBar;
	SliceItem * m_topSlice;
	TitledSliderWithSpinBox * m_topSlider;
	SliceItem * m_rightSlice;
	TitledSliderWithSpinBox * m_rightSlider;
	SliceItem * m_frontSlice;
	TitledSliderWithSpinBox * m_frontSlider;

	//actions
	QAction *m_markAction;
	QAction *m_colorAction;

	QAction *m_topSlicePlayAction;
	Direction m_topSlicePlayDirection;
	int m_topTimerId;
	QAction *m_rightSlicePlayAction;
	Direction m_rightSlicePlayDirection;
	int m_rightTimerId;
	QAction *m_frontSlicePlayAction;
	Direction m_frontSlicePlayDirection;
	int m_frontTimerId;

};



#endif // IMAGEVIEWER_H
