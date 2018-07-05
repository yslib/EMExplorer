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
QT_END_NAMESPACE
class TitledSliderWithSpinBox;
class ItemContext;
class MarkModel;
class HistogramViewer;
class PixelViewer;
class MRC;
class AbstractPlugin;
class AbstractSliceDataModel;
class SliceItem;
class SliceView;

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

class ImageCanvas :public QWidget
{
	Q_OBJECT
public:
	ImageCanvas(QWidget * parent = nullptr,
		bool topSliceVisible = true, 
		bool rightSliceVisible = true,
		bool frontSliceVisible = true,
		AbstractSliceDataModel * model = nullptr);
	int topSliceIndex()const;
	int rightSliceIndex()const;
	int frontSliceIndex()const;
	bool isTopSliceEnabled() const;
	bool isRightSliceEnabled() const;
	bool isFrontSliceEnabled() const;
	QPen pen()const;
	void setPen(const QPen & pen);
	void topSliceEnable(bool enable);
	void rightSliceEnable(bool enable);
	void frontSliceEnable(bool enable);

	AbstractSliceDataModel* takeSliceModel(AbstractSliceDataModel* model);
	AbstractSliceDataModel * sliceModel()const { return m_sliceModel; }
	MarkModel* takeMarkModel(MarkModel* model, bool * success)noexcept;
	MarkModel* markModel();
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
	void topSliceSelected(const QPoint & point);
	void rightSliceSelected(const QPoint & point);
	void frontSliceSelected(const QPoint & point);
	void markModified();
	void markSaved();
	void markSeleteced(QGraphicsItem * item);
public slots:
	void setEnabled(bool enable);
	void onTopSlicePlay(bool enable);
	void onRightSlicePlay(bool enable);
	void onFrontSlicePlay(bool enable);
protected:
	void timerEvent(QTimerEvent* event) Q_DECL_OVERRIDE;
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
	QIcon createColorIcon(const QColor & color);
	//update helper
	void updatePen(const QPen & pen);
	void updateSliceCount(SliceType type);
	void updateSlice(SliceType type);
	void updateMarks(SliceType type);
	void updateActions();
	void updateDeleteAction();
	void updateTopSliceActions();
	void updateFrontSliceActions();
	void updateRightSliceActions();
	void installMarkModel(MarkModel* model);
	void updateSliceModel();
	void detachMarkModel();
	//void detachSliceModel();
	inline bool contains(const QWidget* widget, const QPoint& pos);
	inline void setTopSliceCountHelper(int value);
	inline void setRightSliceCountHelper(int value);
	inline void setFrontSliceCountHelper(int value);
	void markAddedHelper(SliceType type, QGraphicsItem * mark);
	void markDeleteHelper();
	void markSingleSelectionHelper();
	void setCategoryManagerHelper(const QVector<QPair<QString, QColor>> & cates);
	void addCategoryManagerHelper(const QString & name, const QColor & color);
	void changeSliceHelper(int value, SliceType type);
	int currentIndexHelper(SliceType type);
	SliceView * focusOn();
	static MarkModel * createMarkModel(ImageCanvas * view, AbstractSliceDataModel * d);
	//Data Model
	AbstractSliceDataModel * m_sliceModel;
	MarkModel * m_markModel;
	//main layout
	QGridLayout *m_layout;
	SliceView * m_topView;
	SliceView * m_rightView;
	SliceView * m_frontView;
	//Tool Bar
	QToolBar * m_viewToolBar;
	QToolBar * m_editToolBar;
	//Widgets on view toolbar
	TitledSliderWithSpinBox * m_topSlider;
	QCheckBox * m_topSliceCheckBox;
	QCheckBox * m_rightSliceCheckBox;
	TitledSliderWithSpinBox * m_rightSlider;
	QCheckBox * m_frontSliceCheckBox;
	TitledSliderWithSpinBox * m_frontSlider;

	//actions on view toolbar

	QAction *m_zoomInAction;
	QAction *m_zoomOutAction;
	QAction * m_resetAction;

	QAction *m_topSlicePlayAction;
	QAction *m_rightSlicePlayAction;
	QAction *m_frontSlicePlayAction;
	QToolButton * m_menuButton;

	//menu on view toolbar
	QMenu * m_menu;
	QAction * m_histDlg;
	PlayDirection m_topSlicePlayDirection;
	int m_topTimerId;
	PlayDirection m_rightSlicePlayDirection;
	int m_rightTimerId;
	PlayDirection m_frontSlicePlayDirection;
	int m_frontTimerId;

	//Widgets on edit toolbar
	QLabel * m_categoryLabel;
	QComboBox * m_categoryCBBox;

	QLabel * m_penSizeLabel;
	QComboBox* m_penSizeCBBox;

	//actions on edit toolbar
	QAction *m_markAction;
	QAction *m_markSelectionAction;
	//QAction *m_moveAction;
	QAction *m_colorAction;
	//QAction *m_markMergeAction;
	QAction *m_markDeletionAction;
	QAction *m_addCategoryAction;
	//menu on edit toolbar
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
