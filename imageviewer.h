#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H
//#include <QMainWindow>

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
class ImageView :public QWidget
{
	Q_OBJECT
public:
	ImageView(QWidget * parent = nullptr, bool topSliceVisible = true, bool rightSliceVisible = true, bool frontSliceVisible = true, AbstractSliceDataModel * model = nullptr);
	 int topSliceIndex()const;
	 int rightSliceIndex()const;
	int frontSliceIndex()const;

	inline void topSliceEnable(bool enable);
	inline void rightSliceEnable(bool enable);
	inline void frontSliceEnable(bool enable);

	void setColor(const QColor & color);
	void setSliceModel(AbstractSliceDataModel * model);
	AbstractSliceDataModel * sliceModel()const { return m_sliceModel; }
	MarkModel* replaceMarkModel(MarkModel* model,bool * success)noexcept;
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

	static MarkModel * createMarkModel(ImageView * view,AbstractSliceDataModel * d);

	void mark_created_helper_(SliceType type,QGraphicsItem * mark);


	//Data Model
	AbstractSliceDataModel * m_sliceModel;
	MarkModel * m_markModel;

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
	PlayDirection m_topSlicePlayDirection;
	int m_topTimerId;
	PlayDirection m_rightSlicePlayDirection;
	int m_rightTimerId;
	PlayDirection m_frontSlicePlayDirection;
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
