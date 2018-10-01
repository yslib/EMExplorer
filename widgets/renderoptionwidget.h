#pragma once

#include <QWidget>
//#include "renderwidget.h"
#include <QSharedPointer>


class QAbstractItemModel;
class MarkModel;
class TF1DEditor;
class RenderWidget;
class RenderOptions;
QT_BEGIN_NAMESPACE
class QGroupBox;
class QLabel;
class QDoubleSpinBox;
class QSlider;
class QCheckBox;
class QPushButton;
class QComboBox;
class QListWidget;
class QListView;
class QPushButton;
class QLineEdit;
class QToolButton;
QT_END_NAMESPACE


class RenderParameterWidget : public QWidget
{
	Q_OBJECT
public:
	RenderParameterWidget(RenderWidget * widget, QWidget *parent = nullptr);
	QSize sizeHint() const Q_DECL_OVERRIDE { return { 300,600 }; }
	QSize minimumSizeHint() const Q_DECL_OVERRIDE { return { 300,600 }; }
	~RenderParameterWidget(){}
signals:
	void optionsChanged();
	void markUpdated();
	void transferFunctionsChanged(const float * funcs);
private slots:
	void radialSliderChanged(int value);
	void thetaSliderChanged(int value);
	void phiSliderChanged(int value);
	void transferFunctionChanged();
	void renderTypeChanged(const QString & text);
	void tfButtonClicked();
private:
	//void connectWith(VolumeWidget * widget);
	//void disconnectWith(VolumeWidget * widget);
	void setRenderWidget(RenderWidget * widget);

	RenderWidget * m_widget;

	QSharedPointer<RenderOptions>	m_renderOptions;

	QGroupBox *m_volumeInfoGroup;
	QLabel    *m_volumeSizeLabel;
	QLabel    *m_volumeSpacingLabel;

	QDoubleSpinBox *m_xSpacingSpinBox;
	QDoubleSpinBox *m_ySpacingSpinBox;
	QDoubleSpinBox *m_zSpacingSpinBox;

	QGroupBox *m_lightingGroup;
	QLabel    *m_ambientLabel;
	QLabel    *m_diffuseLabel;
	QLabel    *m_shininessLabel;
	QLabel    *m_specularLabel;
	QDoubleSpinBox *m_ambientSpinBox;
	QDoubleSpinBox *m_diffuseSpinBox;
	QDoubleSpinBox *m_shininessSpinBox;
	QDoubleSpinBox *m_specularSpinBox;
	QGroupBox *m_renderOptionGroup;
	QLabel *m_renderTypeLabel;
	QComboBox *m_renderTypeCCBox;

	QGroupBox *m_transferFunctionGroupBox;
	QToolButton *m_tfButton;
	TF1DEditor *m_tfEditor;

	//QGroupBox *m_markListGroup;
	//QListView * m_markListView;

	QGroupBox *m_meshGroup;
	QPushButton * m_meshUpdateButton;

	QGroupBox * m_sliceGroup;
	QLabel * m_radLabel;
	QSlider* m_radSlider;
	QLabel *m_radValueLabel;
	QLabel * m_thetaLabel;
	QSlider* m_thetaSlider;
	QLabel *m_thetaValueLabel;
	QLabel * m_phiLabel;
	QSlider* m_phiSlider;
	QLabel *m_phiValueLabel;


};