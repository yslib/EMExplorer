#ifndef IMAGEVIEWCONTROLPANEL_H
#define IMAGEVIEWCONTROLPANEL_H

#include <QWidget>

enum class SliceType;
QT_BEGIN_NAMESPACE
class QLabel;
class QWheelEvent;
class QToolBar;
class QMenu;
class QGroupBox;
class QToolButton;
class QCheckBox;
class QComboBox;
class QPushButton;
QT_END_NAMESPACE
class TitledSliderWithSpinBox;
class ImageCanvas;

class ImageViewControlPanel:public QWidget
{
	Q_OBJECT
public:
    ImageViewControlPanel(ImageCanvas * canvas,QWidget * parent = nullptr);
	void setImageCanvas(ImageCanvas * canvas);
	int sliceIndex(SliceType type)const;
	void setSliceIndex(SliceType type, int value);
	int sliceCount(SliceType type)const;
	void setSliceCount(SliceType type, int count);
	bool sliceVisible(SliceType type)const;
	QString currentCategoryName()const;
	QColor currentCategoryColor()const;
	int categoryCount()const;

private:
	void createWidgets();
	void updateActions();
	void updateProperty();
	void connections();

	void setCategoryInfoPrivate(const QVector<QPair<QString, QColor>>& cates);
	void addCategoryInfoPrivate(const QString & name, const QColor & color);
	void updateDeleteActionPrivate();
	void updateSliceActions(SliceType type,bool checked);
	QIcon createColorIcon(const QColor & color);
	//private slots:
	void onCategoryAdded();
	void colorChanged();

	enum class PlayDirection {
		Forward,
		Backward
	};
	ImageCanvas * m_canvas;

	TitledSliderWithSpinBox * m_topSlider;
	QCheckBox * m_topSliceCheckBox;
	QCheckBox * m_rightSliceCheckBox;
	TitledSliderWithSpinBox * m_rightSlider;
	QCheckBox * m_frontSliceCheckBox;
	TitledSliderWithSpinBox * m_frontSlider;

	//actions on view toolbar
	

	QToolButton *m_zoomInAction;
	QToolButton *m_zoomOutAction;
	QToolButton * m_resetAction;

	QToolButton *m_topSlicePlayAction;
	QToolButton *m_rightSlicePlayAction;
	QToolButton *m_frontSlicePlayAction;

	QToolButton * m_menuButton;

	//menu on view toolbar
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
	QToolButton *m_markAction;
	QToolButton *m_markSelectionAction;
	QToolButton *m_colorAction;
	//QAction *m_markMergeAction;
	QToolButton *m_markDeletionAction;
	QToolButton *m_addCategoryAction;


	friend class ImageCanvas;
};

#endif // IMAGEVIEWCONTROLPANEL_H