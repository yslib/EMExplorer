#ifndef SLICECONTROLWIDGET_H
#define SLICECONTROLWIDGET_H


#include <QWidget>

class SliceEditorWidget;
class RenderWidget;
class AbstractSliceDataModel;
class TitledSliderWithSpinBox;
QT_BEGIN_NAMESPACE
class QCheckBox;
class QToolButton;
class QButtonGroup;
class QLabel;
class QComboBox;
QT_END_NAMESPACE;

enum class SliceType;

/**
 * \brief This is widget used to control the slice index for both slice view and volume view
 */
class SliceControlWidget:public QWidget
{
	Q_OBJECT
public:
    SliceControlWidget(SliceEditorWidget* sliceWidget, RenderWidget* volumeWidget,QWidget * parent = nullptr);
	void setControlledWidget(SliceEditorWidget* sliceWidget, RenderWidget* volumeWidget);


private slots:
	void onSliceTimer();
	void updateDataModel();
private:

	AbstractSliceDataModel * m_dataModel;
	RenderWidget *m_volumeWidget;
	SliceEditorWidget *m_sliceWidget;

	enum class PlayDirection
	{
		Forward,
		Backward
	};

	// Widgets
	QCheckBox * m_topSliceCheckBox;
	TitledSliderWithSpinBox * m_topSlider;
	QToolButton *m_topSlicePlayAction;
	QCheckBox * m_rightSliceCheckBox;
	TitledSliderWithSpinBox * m_rightSlider;
	QToolButton *m_rightSlicePlayAction;
	QCheckBox * m_frontSliceCheckBox;
	TitledSliderWithSpinBox * m_frontSlider;
	QToolButton *m_frontSlicePlayAction;
	QLabel * m_intervalLabel;
	QComboBox * m_intervalCBBox;
	QToolButton *m_stopAction;

	//QToolButton * m_menuButton;

	// State

	PlayDirection m_topSlicePlayDirection;
	PlayDirection m_rightSlicePlayDirection;
	PlayDirection m_frontSlicePlayDirection;
	SliceType m_playSliceType;

	// Timer 
	QTimer *m_sliceTimer;
	
	QButtonGroup * m_playButtonGroup;


};

#endif // SLICECONTROLWIDGET_H