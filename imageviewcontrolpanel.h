#ifndef IMAGEVIEWCONTROLPANEL_H
#define IMAGEVIEWCONTROLPANEL_H

#include <QWidget>

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
private:

	void createWidgets();
	void updateActions();
	void updateProperty();

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
	QAction *m_colorAction;
	//QAction *m_markMergeAction;
	QAction *m_markDeletionAction;
	QAction *m_addCategoryAction;


	friend class ImageCanvas;
};

#endif // IMAGEVIEWCONTROLPANEL_H