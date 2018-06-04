#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H
//#include <QMainWindow>
#include <QPicture>
#include <QImage>
#include <QVector>
#include <QList>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QSharedPointer>
#include <QModelIndex>

QT_BEGIN_NAMESPACE
class QGridLayout;
class QLabel;
class QWheelEvent;
class QGraphicsView;
class QGraphicsScene;
class QMouseEvent;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;
class QAbstractItemModel;
class QGraphicsItem;
class QToolBar;
class QMenu;
class QGroupBox;
class QToolButton;
class QCheckBox;
QT_END_NAMESPACE

class TitledSliderWithSpinBox;
class ItemContext;
class DataItemModel;
class HistogramViewer;
class PixelViewer;
class MRC;



enum class SliceType
{
	SliceZ,
	SliceY,
	SliceX
};


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
	static constexpr int Width = 500;
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
	void paintLine(const QPoint &begin, const QPoint &end, QPaintDevice * dev);

	public slots:
};


enum ItemTypes
{
	Slice = 1,
	Mark
};

class StrokeMarkItem :public QGraphicsItem {
	QRectF m_boundingRect;
	QPainterPath m_painterPath;
	QList<QPointF> m_points;
public:
	enum { Type = UserType + Mark };
	StrokeMarkItem(QGraphicsItem * parent = nullptr);
	QRectF boundingRect()const override { return m_boundingRect; }
	QPainterPath shape()const override { return QPainterPath(); }
	void addPoint(const QPointF & p);
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)override;
	int type() const override { return Type; }
private:
	QRectF unionWith(const QRectF & rect, const QPointF & p);
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
	void wheelEvent(QGraphicsSceneWheelEvent* event) override;
};

class SliceItem :public QGraphicsPixmapItem
{
public:
	enum { Type = UserType + Slice };
	SliceItem(QGraphicsItem * parent = nullptr) :QGraphicsPixmapItem(parent) {}
	SliceItem(const QPixmap & pixmap, QGraphicsItem * parent = nullptr) :QGraphicsPixmapItem(pixmap, parent) {}
	int type() const override { return Type; }
	virtual  ~SliceItem() = default;
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
	void wheelEvent(QGraphicsSceneWheelEvent* event) override;
};


class GraphicsScene :public QGraphicsScene
{
public:
	GraphicsScene(QObject * parent = nullptr);
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
	void wheelEvent(QGraphicsSceneWheelEvent* event) override;
};
class GraphicsView :public QGraphicsView
{
public:
	GraphicsView(QWidget * parent = nullptr);
	void setMarks(const QList<QGraphicsItem *> & items, SliceType type = SliceType::SliceZ);
	public slots:
	void paintEnable(bool enable) { m_paint = enable; }
	void moveEnable(bool enable) { m_moveble = enable; }
	void setImage(const QImage & image, SliceType type = SliceType::SliceZ);
	void setColor(const QColor & color) { m_color = color; }
	void clearSliceMarks(SliceType tpye = SliceType::SliceZ);
protected:
	void mousePressEvent(QMouseEvent * event)Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent * event)Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent * event)Q_DECL_OVERRIDE;
	void wheelEvent(QWheelEvent * event)Q_DECL_OVERRIDE;

signals:
	void zSliceSelected(const QPoint & point);
	void ySliceSelected(const QPoint & point);
	void xSliceSelected(const QPoint & point);
	void slicedSelected(const QPoint & point, SliceType type = SliceType::SliceZ);
	//void zSliceMarkAdded(QGraphicsItem * item);
	//void ySliceMarkAdded(QGraphicsItem * item);
	//void xSliceMarkAdded(QGraphicsItem * item);
	void markAdded(QGraphicsItem * item, SliceType type = SliceType::SliceZ);

private:
	void clearTopSliceMarks();

	//TODO::
	void clearRightSliceMarks();
	void clearFrontSliceMarks();
	//===========

	void setTopSliceMarks(const QList<QGraphicsItem*> & items);

	//TODO::
	void setRightSliceMarks(const QList<QGraphicsItem*> & items);
	void setFrontSliceMarks(const QList<QGraphicsItem*> & items);
	//============

	void setTopImage(const QImage &image);

	//TODO::
	void setRightImage(const QImage &image);
	void setFrontImage(const QImage & image);
	//============

	void setImageHelper(const QPoint& pos, const QImage& inImage, SliceItem *& sliceItem, QImage * outImage);
	void clearSliceMarksHelper(SliceItem * sliceItem);
	void setMarksHelper(SliceItem * sliceItem,const QList<QGraphicsItem*>& items);


	Q_OBJECT
	//GraphicsScene *m_scene;
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
	QImage  m_topImage;
};

class AbstractSliceDataModel
{
public:
	AbstractSliceDataModel(int nTop,int nRight,int nFront);
	virtual int topSliceCount()const = 0;
	virtual int rightSliceCount()const = 0;
	virtual int frontSliceCount()const = 0;
	virtual QImage originalTopSlice(int index) const =0;
	virtual QImage originalRightSlice(int index) const =0;
	virtual QImage originalFrontSlice(int index) const =0;

    virtual void setTopSlice(const QImage& image, int index);
    virtual void setRightSlice(const QImage& image, int index);
    virtual void setFrontSlice(const QImage& image, int index);

	virtual QImage topSlice(int index)const;
	virtual QImage rightSlice(int index)const;
	virtual QImage frontSlice(int index)const;
private:
	//inline void setSliceHelper(const QImage & image,int index,QVector<QImage> * imgVec,QVector<bool> * flgVec);
	//inline QImage sliceHelper(int index, QVector<QImage> * imgVec, QVector<bool> * flgVec);
    QVector<QImage> m_modifiedTopSlice;
    QVector<bool> m_modifiedTopSliceFlags;
    QVector<QImage> m_modifiedRightSlice;
    QVector<bool> m_modifiedRightSliceFlags;
    QVector<QImage> m_modifiedFrontSlice;
    QVector<bool> m_modifiedFrontSliceFlags;
};


class ImageView :public QWidget
{
	Q_OBJECT
public:
	ImageView(QWidget * parent = nullptr,bool topSliceVisible= true,bool rightSliceVisible = true,bool frontSliceVisible = true,AbstractSliceDataModel * model = nullptr);
	//MVC pattern will be employed later and these function will be removed
	inline int getZSliceValue()const;
	inline int getYSliceValue()const;
	inline int getXSliceValue()const;

	inline void setZXliceEnable(bool enable);
	inline void setYXliceEnable(bool enable);
	inline void setXXliceEnable(bool enable);

	///TODO::
    void setSliceModel(AbstractSliceDataModel * model);
    void setMarkModel(QAbstractItemModel * model);
    QAbstractItemModel * getMarkModel();



	
signals:
	//void ZSliderChanged(int value);
	//void YSliderChanged(int value);
	//void XSliderChanged(int value);
	void sliderChanged(int value, SliceType type);
	void zSliceSelected(const QPoint & point);
	void ySliceSelected(const QPoint & point);
	void xSliceSelected(const QPoint & point);
	void sliceSeletecd(const QPoint & point,SliceType type);
public slots:
	void setEnabled(bool enable);
	void onTopSliceTimer(bool enable);
	void onRightSliceTimer(bool enable);
	void onFrontSliceTimer(bool enable);
	void onColorChanged();
protected:
	void timerEvent(QTimerEvent* event) override;
private:
	//QModelIndex getDataIndex(const QModelIndex & itemIndex);
	//void updateModel();
	//----
	void updateSliceCount(SliceType type);
	void updateSlice(SliceType type);
	void updateMarks(SliceType type);

	void updateActions();

	void updateTopSliceActions();
	void updateFrontSliceActions();
	void updateRightSliceActions();

	void createToolBar();
	void createConnections();
	void createContextMenu();


	void sliceChanged(int value, SliceType type);
	inline void setTopSliceCount(int value);
	inline void setRightSliceCount(int value);
	inline void setFrontSliceCount(int value);
	int currentIndex(SliceType type);
	//void resetSliceAndVisibleMarks(SliceType type);


	enum class Direction {
		Forward,
		Backward
	};
	//Data Model
	AbstractSliceDataModel * m_sliceModel;
	QAbstractItemModel * m_markModel;

	//QAbstractItemModel * m_model;

	//-----
	//QModelIndex m_modelIndex;
	//QSharedPointer<ItemContext> m_ptr;
	//bool m_internalUpdate;
	//------
	QGridLayout *m_layout;
	GraphicsView * m_topView;
    GraphicsView * m_rightView;
    GraphicsView * m_frontView;
	//Tool Bar
	QToolBar * m_toolBar;
    //widgets on toolbar
	TitledSliderWithSpinBox * m_topSlider;
    QAction *m_topSlicePlayAction;
    QCheckBox * m_topSliceCheckBox;
    SliceItem * m_rightSlice;
    QCheckBox * m_rightSliceCheckBox;
	TitledSliderWithSpinBox * m_rightSlider;
	QAction *m_rightSlicePlayAction;
    SliceItem * m_frontSlice;
    QCheckBox * m_frontSliceCheckBox;
	TitledSliderWithSpinBox * m_frontSlider;
	QAction *m_frontSlicePlayAction;

    //actions on toolbar
	QAction *m_markAction;
	QAction *m_colorAction;
	QAction *m_zoomInAction;
	QAction *m_zoomOutAction;
	QToolButton * m_menuButton;

    //menu on toolbar
	QMenu * m_menu;
	QAction * m_histDlg;

	Direction m_topSlicePlayDirection;
    int m_topTimerId;

	Direction m_rightSlicePlayDirection;
	int m_rightTimerId;
	
	Direction m_frontSlicePlayDirection;
	int m_frontTimerId;


	//ContextMenu
	QMenu *m_contextMenu;
	QAction * m_zoomIn;
	QAction * m_zoomOut;
	QAction * m_histDlgAction;
	QAction * m_pixelViewDlgAction;
	QAction * m_marksManagerDlgAction;

};



#endif // IMAGEVIEWER_H
