#pragma once

#include <QWidget>
#include <QVector3D>

QT_BEGIN_NAMESPACE
class QGroupBox;
class QLabel;
class QDoubleSpinBox;
class QCheckBox;
class QPushButton;
class QComboBox;
QT_END_NAMESPACE


enum RenderMode {
	SliceTexture = 1,
	LineMesh=2,
	FillMesh=4,
	DVR=8
};




struct RenderOptions {
	float ambient;
	float specular;
	float diffuse;
	float shininess;
	float xSpacing;
	float ySpacing;
	float zSpacing;
	QVector3D lightDirection;
	RenderMode mode;
	RenderOptions() :
		ambient(1.0)
		, specular(0.75)
		, diffuse(0.5)
		, shininess(40.00)
		, lightDirection(0, -1, 0)
		, xSpacing(1.0)
		, ySpacing(1.0)
		, zSpacing(1.0)
	,mode(RenderMode::DVR)
	{}
};



class RenderParameterWidget : public QWidget
{
	Q_OBJECT
public:
	RenderParameterWidget(QWidget *parent = nullptr);
	QSize sizeHint() const Q_DECL_OVERRIDE { return { 250,500 }; }
	QSize minimumSizeHint() const Q_DECL_OVERRIDE { return { 250,300 }; }
	const RenderOptions * options()const { return m_renderOptions.data(); }
	~RenderParameterWidget(){}
signals:
	emit void optionsChanged();
private:
	//void connectWith(VolumeWidget * widget);
	//void disconnectWith(VolumeWidget * widget);

	QScopedPointer<RenderOptions>					    m_renderOptions;

	QSharedPointer<RenderOptions>						m_options;

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
};
