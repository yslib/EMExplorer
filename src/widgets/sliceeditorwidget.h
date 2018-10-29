#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QList>
#include <QWidget>
#include <QGraphicsScene>

enum class SliceType;
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
class QPushButton;
class QTimer;
QT_END_NAMESPACE
class TitledSliderWithSpinBox;
class ItemContext;
class MarkModel;
class HistogramWidget;
class PixelWidget;
class MRC;
class AbstractSliceEditorPlugin;
class AbstractSliceDataModel;
class SliceItem;
class SliceWidget;
class RenderWidget;
class SliceToolWidget;
class CategoryInfo;
class StrokeMarkItem;

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


struct SliceState {
	int currentTopSliceIndex;
	int currentRightSliceIndex;
	int currentFrontSliceIndex;
	QString currentCategory;
	SliceState() :currentTopSliceIndex(0)
		, currentRightSliceIndex(0)
		, currentFrontSliceIndex(0)
	{}
};

class SliceEditorWidgetPrivate {
public:
	SliceEditorWidgetPrivate():
	state(new SliceState)
	{}
	QScopedPointer<SliceState> state;
};

class SliceEditorWidget :public QWidget
{
	Q_OBJECT
public:
	SliceEditorWidget(QWidget * parent = nullptr,
		bool topSliceVisible = true, 
		bool rightSliceVisible = true,
		bool frontSliceVisible = true,
		AbstractSliceDataModel * model = nullptr);

	bool eventFilter(QObject* watched, QEvent* event) override;

	bool topSliceVisible() const;
	bool rightSliceVisible() const;
	bool frontSliceVisible() const;

	QPen pen()const;
	void setPen(const QPen & pen);

	void setTopSliceVisible(bool enable);
	void setRightSliceVisible(bool enable);
	void setFrontSliceVisible(bool enable);
	void setSliceVisible(SliceType type, bool visible);

	int currentSliceIndex(SliceType type)const;
	void setSliceIndex(SliceType type, int index);

	//void playSlice(SliceType type, bool play);

	QString currentCategory()const;
	void setCurrentCategory(const QString & name);
	bool addCategory(const CategoryInfo & info);
	QStringList categories() const;


	SliceWidget * topView()const;
	SliceWidget * rightView()const;
	SliceWidget * frontView()const;
	
	void resetZoom(bool check);
	void zoomIn();
	void zoomOut();
	void setOperation(SliceType type,int opt);

	void setTopSliceVisibility(bool enable);
	void setFrontSliceVisibility(bool check);
	void setRightSliceVisibility(bool check);

	void deleteSelectedMarks();

	AbstractSliceDataModel* takeSliceModel(AbstractSliceDataModel* model);
	inline AbstractSliceDataModel * sliceModel()const;

	MarkModel* takeMarkModel(MarkModel* model, bool * success)noexcept;
	MarkModel* markModel();


	//QSize sizeHint() const Q_DECL_OVERRIDE { return {800,800}; }
	//QSize minimumSizeHint() const Q_DECL_OVERRIDE { return { 600,600 }; }
	~SliceEditorWidget();
signals:

	/**
	 * \brief This signal is emitted when top slice changed
	 * \param index The new current top slice index
	 */
	void topSliceChanged(int index);

	/**
	 * \brief This signal is emitted when right slice changed
	 * \param index The new current right slice index
	 */
	void rightSliceChanged(int index);

	/**
	 * \brief This signal is emitted when front slice changed
	 * \param index The new current front slice index
	 */
	void frontSliceChanged(int index);

	/**
	 * \brief This signal is emitted when slice data model changed
	 */
	void dataModelChanged();

	/**
	 * \brief This signal is emitted when mark model changed
	 */
	void markModelChanged();

	/**
	 * \brief 
	 * \param point 
	 */
	void topSliceSelected(const QPoint & point);

	/**
	 * \brief 
	 * \param point 
	 */
	void rightSliceSelected(const QPoint & point);

	/**
	 * \brief 
	 * \param point 
	 */
	void frontSliceSelected(const QPoint & point);

	void viewFocus(SliceType type);

	void markModified();
	
	void markSaved();

	void markSelected(StrokeMarkItem * item);

	void marksSelected(QList<StrokeMarkItem*> items);
	


private:

	enum class PlayDirection {
		Forward,
		Backward
	};
	void createConnections();
	//update helper
	
	//void updateSliceCount(SliceType type);
	
	void updateMarks(SliceType type);
	void updateActions();

	void installMarkModel(MarkModel* model);
	void updateSliceModel();
	void detachMarkModel();
	//void detachSliceModel();
	inline bool contains(const QWidget* widget, const QPoint& pos);

	void markAddedHelper(SliceType type, StrokeMarkItem* mark);
	
	void markSingleSelectionHelper();

	//void setCategoryManagerHelper(const QVector<QPair<QString, QColor>> & cates);
	//void addCategoryManagerHelper(const QString & name, const QColor & color);
	void changeSliceHelper(int value, SliceType type);
	int currentIndexHelper(SliceType type);

	SliceWidget * focusOn();
	static MarkModel * createMarkModel(SliceEditorWidget * view, AbstractSliceDataModel * d);
	//Data Model

	SliceEditorWidgetPrivate * const d_ptr;
	Q_DECLARE_PRIVATE(SliceEditorWidget);

	AbstractSliceDataModel * m_sliceModel;
	MarkModel * m_markModel;

	//main layout
	QGridLayout *m_layout;
	SliceWidget * m_topView;
	SliceWidget * m_rightView;
	SliceWidget * m_frontView;




	//ContextMenu
	//QMenu *m_contextMenu;
	//QAction * m_zoomIn;
	//QAction * m_zoomOut;

	//QWidget * m_menuWidget;

};

//inline VolumeWidget* ImageCanvas::volumeWidget()const {return m_renderView;}
inline AbstractSliceDataModel * SliceEditorWidget::sliceModel()const { return m_sliceModel; }

inline SliceWidget * SliceEditorWidget::topView()const { return m_topView; }
inline SliceWidget * SliceEditorWidget::rightView()const { return m_rightView; }
inline SliceWidget * SliceEditorWidget::frontView()const { return m_frontView; }



#endif // IMAGEVIEWER_H
