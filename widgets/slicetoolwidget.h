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
class QTimer;
QT_END_NAMESPACE
class TitledSliderWithSpinBox;
class SliceEditorWidget;

class SliceToolWidget:public QWidget
{
	Q_OBJECT
public:
    SliceToolWidget(SliceEditorWidget * canvas, QWidget * parent = nullptr);
	void setImageCanvas(SliceEditorWidget * canvas);
	QString currentCategoryName()const;
	QColor currentCategoryColor()const;
	int categoryCount()const;
signals:
	void topSliceIndexChanged(int value);
	void rightSliceIndexChanged(int value);
	void frontSliceIndexChanged(int value);
private slots:
	void onSliceTimer();
	void onTopSlicePlayAction();
	void onRightSlicePlayAction();
	void onFrontSlicePlayAction();

private:
	void createWidgets();
	void connections();
	void updateDataModel();

	void setCategoryInfoPrivate(const QVector<QPair<QString, QColor>>& cates);
	void addCategoryInfoPrivate(const QString & name, const QColor & color);

	void updateDeleteActionPrivate();
	void updateSliceActions(SliceType type,bool checked);

	QIcon createColorIcon(const QColor & color);
	//private slots:
	void onCategoryAdded();
	void colorChanged();

	enum class PlayDirection 
	{
		Forward,
		Backward
	};
	SliceEditorWidget * m_canvas;
	//RenderWidget * m_volumeWidget;

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


	//Widgets on edit toolbar
	QLabel * m_categoryLabel;
	QComboBox * m_categoryCBBox;
	QLabel * m_penSizeLabel;
	QComboBox* m_penSizeCBBox;
	QToolButton *m_colorAction;
	QToolButton *m_addCategoryAction;




	friend class SliceEditorWidget;
};

#endif // IMAGEVIEWCONTROLPANEL_H