#ifndef BASICCONTROLWIDGET_H
#define BASICCONTROLWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QGroupBox;
class QLabel;
class QDoubleSpinBox;
class QCheckBox;
class QPushButton;
QT_END_NAMESPACE

class Volume;
class VolumeRenderWidget;

class BasicControlWidget : public QWidget
{
	Q_OBJECT

public:
	BasicControlWidget(VolumeRenderWidget *renderWidget, QWidget * parent = 0);
	~BasicControlWidget();

	QSize minimumSizeHint() const override;
	QSize sizeHint() const override;

	void updateVolumeInfo(Volume* volume);

public slots:
	void updateViewpointNum(double value);
	void setHeapixNsideValue(int value);

public:
	QGroupBox *volumeInfoGroup;
	QLabel    *volumeSizeLabel;
	QLabel    *volumeSpacingLabel;

	QGroupBox *lightingGroup;
	QLabel    *ambientLabel;
	QLabel    *diffuseLabel;
	QLabel    *shininessLabel;
	QLabel    *specularLabel;
	QDoubleSpinBox *ambientSpinBox;
	QDoubleSpinBox *diffuseSpinBox;
	QDoubleSpinBox *shininessSpinBox;
	QDoubleSpinBox *specularSpinBox;

	QGroupBox *viewpointGroup;
	QLabel    *heapixNsideLabel;
	QDoubleSpinBox *heapixNsideSpinBox;
};


#endif