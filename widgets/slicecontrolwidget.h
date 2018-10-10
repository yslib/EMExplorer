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
QT_END_NAMESPACE;

enum class SliceType;

class SliceControlWidget:public QWidget
{
	Q_OBJECT
public:
    SliceControlWidget(SliceEditorWidget* sliceWidget, RenderWidget* volumeWidget,QWidget * parent = nullptr);
	void setSliceModel(SliceEditorWidget* sliceWidget, RenderWidget* volumeWidget);
signals:
	//void topSliceIndexChanged(int index);
	//void rightSliceIndexChanged(int index);
	//void frontSliceIndexChanged(int index);

	//void topSliceVisibilityChanged(bool vis);
	//void rightSliceVisibilityChanged(bool vis);
	//void frontSliceVisibilityChanged(bool vis);

private slots:
	void onSliceTimer();
private:

	AbstractSliceDataModel * m_dataModel;
	RenderWidget *m_volumeWidget;
	SliceEditorWidget *m_sliceWidget;

	enum class PlayDirection
	{
		Forward,
		Backward
	};

	QCheckBox * m_topSliceCheckBox;
	TitledSliderWithSpinBox * m_topSlider;
	QCheckBox * m_rightSliceCheckBox;
	TitledSliderWithSpinBox * m_rightSlider;
	QCheckBox * m_frontSliceCheckBox;
	TitledSliderWithSpinBox * m_frontSlider;

	//actions on view toolbar

	QToolButton *m_topSlicePlayAction;
	QToolButton *m_rightSlicePlayAction;
	QToolButton *m_frontSlicePlayAction;

	QToolButton * m_menuButton;


	PlayDirection m_topSlicePlayDirection;
	PlayDirection m_rightSlicePlayDirection;
	PlayDirection m_frontSlicePlayDirection;

	SliceType m_playSliceType;

	QTimer *m_sliceTimer;

	
	QButtonGroup * m_playButtonGroup;


};

#endif // SLICECONTROLWIDGET_H