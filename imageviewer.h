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
#include <QPushButton>

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
class QComboBox;
QT_END_NAMESPACE

class TitledSliderWithSpinBox;
class ItemContext;
class MarkModel;
class HistogramViewer;
class PixelViewer;
class MRC;
class AbstractPlugin;

enum class SliceType
{
	Top,		//Z
	Right,		//Y
	Front		//X
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
	void mousePressEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;
	void wheelEvent(QGraphicsSceneWheelEvent* event) Q_DECL_OVERRIDE;
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
	void mousePressEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;
	void wheelEvent(QGraphicsSceneWheelEvent* event) Q_DECL_OVERRIDE;
};


class SliceScene :public QGraphicsScene
{
public:
	SliceScene(QObject * parent = nullptr);
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;
	void wheelEvent(QGraphicsSceneWheelEvent* event) Q_DECL_OVERRIDE;
};
class SliceView :public QGraphicsView
{
public:
	SliceView(QWidget * parent = nullptr);
	void setMarks(const QList<QGraphicsItem *> & items);
public slots:
	void paintEnable(bool enable) { m_paint = enable; }
	void moveEnable(bool enable) { m_moveble = enable; }
	void setImage(const QImage & image);
	void setColor(const QColor & color) { m_color = color; }
	void clearSliceMarks();
protected:
	void mousePressEvent(QMouseEvent * event)Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent * event)Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent * event)Q_DECL_OVERRIDE;

	void wheelEvent(QWheelEvent * event)Q_DECL_OVERRIDE;
	void focusInEvent(QFocusEvent* event) Q_DECL_OVERRIDE;
	void focusOutEvent(QFocusEvent* event)Q_DECL_OVERRIDE;
signals:
	void sliceSelected(const QPoint & point);
	void markAdded(QGraphicsItem * item);

private:
	void clearTopSliceMarks();
	void setTopSliceMarks(const QList<QGraphicsItem*> & items);
	void setTopImage(const QImage &image);
	void setImageHelper(const QPoint& pos, const QImage& inImage, SliceItem *& sliceItem, QImage * outImage);
	void clearSliceMarksHelper(SliceItem * sliceItem);
	void setMarksHelper(SliceItem * sliceItem,const QList<QGraphicsItem*>& items);

	Q_OBJECT
	qreal m_scaleFactor;
	QVector<QPoint> m_paintViewPointsBuffer;
	SliceItem * m_currentPaintItem;
	bool m_paint;
	bool m_moveble;
	QPointF m_prevScenePoint;
	QColor m_color;
	SliceItem * m_slice;
	QImage  m_image;
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
	//void sliderChanged(int value, SliceType type);
	void topSliceOpened(int index);
	void topSliceChanged(int index);
	void topSlicePlayStoped(int index);
	void rightSliceOpened(int index);
	void rightSliceChanged(int index);
	void rightSlicePlayStoped(int index);
	void frontSliceOpened(int index);
	void frontSliceChanged(int index);
	void frontSlicePlayStoped(int index);

	void topSliceSelected(const QPoint & point);
	void rightSliceSelected(const QPoint & point);
	void frontSliceSelected(const QPoint & point);

	//void sliceSeletecd(const QPoint & point,SliceType type);

public slots:
	void setEnabled(bool enable);
	void topSlicePlay(bool enable);
	void rightSlicePlay(bool enable);
	void frontSlicePlay(bool enable);
	void onColorChanged();
protected:
	void timerEvent(QTimerEvent* event) override;
	void contextMenuEvent(QContextMenuEvent* event) Q_DECL_OVERRIDE;
	//void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
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

	void createWidgets();
	void createToolBar();
	void createConnections();
	void createContextMenu();



	void changeSlice(int value, SliceType type);
	inline void setTopSliceCount(int value);
	inline void setRightSliceCount(int value);
	inline void setFrontSliceCount(int value);
	int currentIndex(SliceType type);


	inline bool contains(const QWidget* widget, const QPoint& pos);

	//void resetSliceAndVisibleMarks(SliceType type);
	enum class Direction {
		Forward,
		Backward
	};
	//Data Model
	AbstractSliceDataModel * m_sliceModel;
	QAbstractItemModel * m_markModel;

	//------
	QGridLayout *m_layout;
	SliceView * m_topView;
    SliceView * m_rightView;
    SliceView * m_frontView;
	QPushButton * m_reset;
	//Tool Bar
	QToolBar * m_toolBar;

    //Widgets on toolbar
	TitledSliderWithSpinBox * m_topSlider;
    QCheckBox * m_topSliceCheckBox;
    QCheckBox * m_rightSliceCheckBox;
	TitledSliderWithSpinBox * m_rightSlider;
    QCheckBox * m_frontSliceCheckBox;
	TitledSliderWithSpinBox * m_frontSlider;
	QLabel * m_categoryLabel;;
	QComboBox * m_categoryCBBox;


	//slice
	SliceItem * m_rightSlice;
	SliceItem * m_frontSlice;

    //actions on toolbar
	QAction *m_addCategoryAction;
	QAction *m_markAction;
	QAction *m_colorAction;
	QAction *m_zoomInAction;
	QAction *m_zoomOutAction;

	QAction *m_topSlicePlayAction;
	QAction *m_rightSlicePlayAction;
	QAction *m_frontSlicePlayAction;
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
	QWidget * m_menuWidget;

};



#endif // IMAGEVIEWER_H
