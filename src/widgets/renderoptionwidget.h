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


/**
 * \brief This is a widget used to control \a RenderWidget
 */
class RenderParameterWidget : public QWidget
{
	Q_OBJECT
public:
	RenderParameterWidget(RenderWidget * widget, QWidget *parent = nullptr);
	void setRenderWidget(RenderWidget * widget);
	QSharedPointer<float> transferFunction()const;
	~RenderParameterWidget(){}
signals:
	void optionsChanged();
	void markUpdated();
	void transferFunctionsChanged(const float * funcs);
private slots:
	void updateDataModel();
	void radialSliderChanged(int value);
	void thetaSliderChanged(int value);
	void phiSliderChanged(int value);
	void transferFunctionChanged();
	void renderTypeChanged(const QString & text);
	void tfButtonClicked();
private:
	//void connectWith(VolumeWidget * widget);
	//void disconnectWith(VolumeWidget * widget);
	

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
	//QLabel *m_renderTypeLabel;
	QComboBox *m_renderTypeCCBox;

	QGroupBox *m_transferFunctionGroupBox;
	QToolButton *m_tfButton;
	TF1DEditor *m_tfEditor;

	//QGroupBox *m_markListGroup;
	//QListView * m_markListView;

	QGroupBox *m_meshGroup;
	QPushButton * m_meshUpdateButton;
	QPushButton * m_saveMeshButton;

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