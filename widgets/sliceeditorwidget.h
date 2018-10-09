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
class AbstractPlugin;
class AbstractSliceDataModel;
class SliceItem;
class SliceWidget;
class RenderWidget;
class SliceToolWidget;
class CategoryInfo;

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
	int topSliceIndex;
	int rightSliceIndex;
	int frontSliceIndex;
	QString currentCategory;
	SliceState() :topSliceIndex(0)
		, rightSliceIndex(0)
		, frontSliceIndex(0)
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

	void playSlice(SliceType type, bool play);


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

	void deleteSelectedMarks();

	AbstractSliceDataModel* takeSliceModel(AbstractSliceDataModel* model);
	inline AbstractSliceDataModel * sliceModel()const;
	MarkModel* takeMarkModel(MarkModel* model, bool * success)noexcept;
	MarkModel* markModel();


	//QSize sizeHint() const Q_DECL_OVERRIDE { return {800,800}; }
	//QSize minimumSizeHint() const Q_DECL_OVERRIDE { return { 600,600 }; }
	~SliceEditorWidget();
signals:
	void topSliceOpened(int index);
	void topSliceChanged(int index);
	void topSlicePlayStoped(int index);

	void rightSliceOpened(int index);
	void rightSliceChanged(int index);
	void rightSlicePlayStoped(int index);

	void frontSliceOpened(int index);
	void frontSliceChanged(int index);
	void frontSlicePlayStoped(int index);

	void dataModelChanged();
	void markModelChanged();

	void topSliceSelected(const QPoint & point);
	void rightSliceSelected(const QPoint & point);
	void frontSliceSelected(const QPoint & point);

	void viewFocus(SliceType type);

	void markModified();
	void markSaved();
	void markSeleteced(QGraphicsItem * item);
	
protected:
	void contextMenuEvent(QContextMenuEvent* event) Q_DECL_OVERRIDE;
private:
	enum class PlayDirection {
		Forward,
		Backward
	};
	void createWidgets();
	void createToolBar();
	void createConnections();
	void createContextMenu();
	//update helper
	
	//void updateSliceCount(SliceType type);
	
	void updateMarks(SliceType type);
	void updateActions();
	void updateDeleteAction();
	void updateTopSliceActions(bool enable);
	void updateFrontSliceActions(bool check);
	void updateRightSliceActions(bool check);
	void installMarkModel(MarkModel* model);
	void updateSliceModel();
	void detachMarkModel();
	//void detachSliceModel();
	inline bool contains(const QWidget* widget, const QPoint& pos);

	void markAddedHelper(SliceType type, QGraphicsItem * mark);
	
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

	////actions on view toolbar

	QAction *m_zoomInAction;
	QAction *m_zoomOutAction;


	//ContextMenu
	QMenu *m_contextMenu;
	QAction * m_zoomIn;
	QAction * m_zoomOut;
	QAction * m_histDlgAction;
	QAction * m_pixelViewDlgAction;
	QAction * m_marksManagerDlgAction;
	QWidget * m_menuWidget;

	friend class SliceToolWidget;
};

//inline VolumeWidget* ImageCanvas::volumeWidget()const {return m_renderView;}
inline AbstractSliceDataModel * SliceEditorWidget::sliceModel()const { return m_sliceModel; }

inline SliceWidget * SliceEditorWidget::topView()const { return m_topView; }
inline SliceWidget * SliceEditorWidget::rightView()const { return m_rightView; }
inline SliceWidget * SliceEditorWidget::frontView()const { return m_frontView; }



#endif // IMAGEVIEWER_H
