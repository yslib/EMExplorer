#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
class QGroupBox;
class QLabel;
class QDoubleSpinBox;
class QCheckBox;
class QPushButton;
QT_END_NAMESPACE


class VolumeWidget;

class RenderParameterWidget : public QWidget
{
	Q_OBJECT
public:
	RenderParameterWidget(VolumeWidget * volumeWidget,QWidget *parent = nullptr);
	QSize minimumSizeHint() const Q_DECL_OVERRIDE { return QSize(250, 250); }
	QSize sizeHint() const Q_DECL_OVERRIDE { return QSize(250, 250); }
	~RenderParameterWidget();
private:
	void updateDataModel();
	void updateMarkModel();

	VolumeWidget * m_volumeWidget;

	QGroupBox *m_volumeInfoGroup;
	QLabel    *m_volumeSizeLabel;
	QLabel    *m_volumeSpacingLabel;

	QGroupBox *m_lightingGroup;
	QLabel    *m_ambientLabel;
	QLabel    *m_diffuseLabel;
	QLabel    *m_shininessLabel;
	QLabel    *m_specularLabel;
	QDoubleSpinBox *m_ambientSpinBox;
	QDoubleSpinBox *m_diffuseSpinBox;
	QDoubleSpinBox *m_shininessSpinBox;
	QDoubleSpinBox *m_specularSpinBox;

};
