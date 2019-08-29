#ifndef SLICEVIEW_H
#define SLICEVIEW_H
#include <QGraphicsView>
#include <QSet>
#include "model/markitem.h"
#include "model/commands.h"

class SliceItem;

using AnchorItem = QGraphicsPixmapItem;


struct SliceWidgetState {
	SliceItem * sliceItem;
	AnchorItem * anchorItem;
	StrokeMarkItem * paintingItem;
	QVector<QPoint> m_paintViewPointsBuffer;
	QPointF prevViewPoint;
	bool navigationView;
	bool painting;
	bool selection;
	int opState;
};

class SliceWidgetPrivate {
	public:
};

class SliceWidget :public QGraphicsView
{
public:
	enum Operation
	{
		/*
		 * This action only sets the anchor when click the slice item in the view.
		 */
		None = 0,
		/*
		 * This action marks on the slice item.
		 */
		Paint = 1,
		/*
		 * This action will select marks by clicking the item or dragging a rubber.
		 */
		Selection = 2,

		/*
		 *This action allow to erase mark partially
		 */
		Erase = 3
	};

	SliceWidget(QWidget * parent = nullptr);

	void setMarks(const QList<StrokeMarkItem *> & items);
public slots:

	inline void setOperation(int state);

	void setImage(const QImage & image);

	inline void setPen(const QPen & pen);

	void setDefaultZoom();

	inline void setNavigationViewEnabled(bool enabled);

	bool navigationViewEnabled()const { return m_paintNavigationView; };

	inline QPen pen()const;

	void clearSliceMarks();

	QList<StrokeMarkItem*> selectedItems() const;

	int selectedItemCount()const;

	void moveSlice(const QPointF & dir);

	QSize sizeHint()const override;


protected:
	void mousePressEvent(QMouseEvent * event)Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent * event)Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent * event)Q_DECL_OVERRIDE;
	void wheelEvent(QWheelEvent * event)Q_DECL_OVERRIDE;
	void focusInEvent(QFocusEvent* event) Q_DECL_OVERRIDE;
	void focusOutEvent(QFocusEvent* event)Q_DECL_OVERRIDE;
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
signals:

	/**
	 * \brief The signals is emitted when the slice is selected
	 * \param point The position selected on the slice
	 */
	void sliceSelected(const QPoint & point);

	/**
	 * \brief The signals is emitted when the slice is selected
	 */
	void sliceSelected();

    void markAdded(QUndoCommand * commmand);


	/**
	 * \brief This signals is emitted when the slice is dragged by mouse
	 * \param delta The position difference between movement.
	 */
	void viewMoved(const QPointF & delta);

	/**
	 * \brief This signals is emitted when the number of selected mark items changed.
	 */
	void selectionChanged();

private:

	inline static void clearSliceMarksHelper(SliceItem * slice);

	inline  void setMarkHelper(const QList<StrokeMarkItem*>& items);

	QRect thumbnailRect(const QRectF & sliceRect,const QRectF & viewRect) const;

	QGraphicsItem * createMarkItem();

	static QPixmap createAnchorItemPixmap(const QString & fileName = QString());

	Q_OBJECT

	enum {
		ThumbnailLength = 200
	};

	//qreal m_scaleFactor;
    bool m_paintNavigationView = false;
	QVector<QPoint> m_paintViewPointsBuffer;
	QPointF m_prevViewPoint;
    QPen m_pen = QPen(Qt::black, 5, Qt::SolidLine);
    SliceItem * m_slice = nullptr;
	QImage m_thumbnail;
	//items
    SliceItem * m_currentPaintingSlice = nullptr;
    StrokeMarkItem * m_paintingItem = nullptr;
    AnchorItem * m_anchorItem = nullptr;
	QSet<StrokeMarkItem *> m_erasingMarks;
	//state variable
    bool m_paint = false;
    bool m_selection = false;
    int m_state = Operation::None;
    //commands
    AddMarkCommand *addingMark = nullptr;
};

inline void SliceWidget::setOperation(int state)
{
	Q_ASSERT_X(state == Operation::Paint || 
		state == Operation::Selection||
		state == Operation::None ||
		state == Operation::Erase,
		"SliceView::setFunction", "state must be exclusive");
	m_state = state;
}

inline void SliceWidget::setPen(const QPen & pen){m_pen = pen;}
inline QPen SliceWidget::pen()const{return m_pen;}
inline void SliceWidget::setNavigationViewEnabled(bool enabled)
{
	m_paintNavigationView = enabled;
	update();
	updateGeometry();
}



#endif // SLICEVIEW_H
