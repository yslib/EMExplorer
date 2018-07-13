#include <QGroupBox>
#include <QBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <sstream>
#include "BasicControlWidget.h"
#include "Volume.h"
#include "VolumeRenderWidget.h"
using namespace std;

BasicControlWidget::BasicControlWidget(VolumeRenderWidget *renderWidget, QWidget * parent)
	: QWidget(parent)
{
	QVBoxLayout *mainLayout = new QVBoxLayout;

	// volume information
	volumeInfoGroup = new QGroupBox(tr("Volume Info"), this);
	QVBoxLayout *verticalLayout = new QVBoxLayout;
	volumeSizeLabel    = new QLabel("Slice Size");
	volumeSpacingLabel = new QLabel("Spacing");
	verticalLayout->addWidget(volumeSizeLabel);
	verticalLayout->addWidget(volumeSpacingLabel);
	volumeInfoGroup->setLayout(verticalLayout);
	mainLayout->addWidget(volumeInfoGroup);

	// lighting parameters
	lightingGroup = new QGroupBox(tr("Lighting Parameters"), this);
	verticalLayout = new QVBoxLayout();

	//// ambient
	QHBoxLayout *horizonLayout = new QHBoxLayout();
	ambientLabel = new QLabel("ambient");
	ambientSpinBox = new QDoubleSpinBox();
	ambientSpinBox->setRange(0, 1.0);
	ambientSpinBox->setValue(1.0);
	ambientSpinBox->setSingleStep(0.01);
	connect(ambientSpinBox, SIGNAL(valueChanged(double)), renderWidget, SLOT(setAmbient(double)));
	horizonLayout->addWidget(ambientLabel);
	horizonLayout->addWidget(ambientSpinBox);
	verticalLayout->addLayout(horizonLayout);

	// diffuse
	horizonLayout = new QHBoxLayout();
	diffuseLabel = new QLabel("diffuse");
	diffuseSpinBox = new QDoubleSpinBox();
	diffuseSpinBox->setRange(0, 1.0);
	diffuseSpinBox->setValue(0.5);
	diffuseSpinBox->setSingleStep(0.01);
	connect(diffuseSpinBox, SIGNAL(valueChanged(double)), renderWidget, SLOT(setDiffuse(double)));
	horizonLayout->addWidget(diffuseLabel);
	horizonLayout->addWidget(diffuseSpinBox);
	verticalLayout->addLayout(horizonLayout);

	// shininess
	horizonLayout = new QHBoxLayout();
	shininessLabel = new QLabel("shininess");
	shininessSpinBox = new QDoubleSpinBox();
	shininessSpinBox->setRange(0, 100);
	shininessSpinBox->setValue(40);
	shininessSpinBox->setSingleStep(1);
	connect(shininessSpinBox, SIGNAL(valueChanged(double)), renderWidget, SLOT(setShininess(double)));
	horizonLayout->addWidget(shininessLabel);
	horizonLayout->addWidget(shininessSpinBox);
	verticalLayout->addLayout(horizonLayout);

	// specular
	horizonLayout = new QHBoxLayout();
	specularLabel = new QLabel("specular");
	specularSpinBox = new QDoubleSpinBox();
	specularSpinBox->setRange(0, 1.0);
	specularSpinBox->setValue(0.75);
	specularSpinBox->setSingleStep(0.01);
	connect(specularSpinBox, SIGNAL(valueChanged(double)), renderWidget, SLOT(setSpecular(double)));
	horizonLayout->addWidget(specularLabel);
	horizonLayout->addWidget(specularSpinBox);
	verticalLayout->addLayout(horizonLayout);

	lightingGroup->setLayout(verticalLayout);
	mainLayout->addWidget(lightingGroup);

	// viewpoint parameters
	viewpointGroup = new QGroupBox(tr("Viewpoint Selection"), this);
	verticalLayout = new QVBoxLayout();

	// heapixNside
	horizonLayout = new QHBoxLayout();
	heapixNsideLabel = new QLabel("Viewpoint Number(300)");
	heapixNsideSpinBox = new QDoubleSpinBox();
	heapixNsideSpinBox->setRange(1, 30);
	heapixNsideSpinBox->setValue(5);
	heapixNsideSpinBox->setSingleStep(1);
	connect(heapixNsideSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateViewpointNum(double)));
	connect(heapixNsideSpinBox, SIGNAL(valueChanged(double)), renderWidget, SLOT(setHealpixNside(double)));
	horizonLayout->addWidget(heapixNsideLabel);
	horizonLayout->addWidget(heapixNsideSpinBox);	
	horizonLayout->addStretch();
	verticalLayout->addLayout(horizonLayout);

	viewpointGroup->setLayout(verticalLayout);
	mainLayout->addWidget(viewpointGroup);

	mainLayout->addStretch();

	setLayout(mainLayout);
}

BasicControlWidget::~BasicControlWidget()
{

}

QSize BasicControlWidget::minimumSizeHint() const
{
	return QSize(312, 200);
}

QSize BasicControlWidget::sizeHint() const
{
	return QSize(312, 200);
}

void BasicControlWidget::updateVolumeInfo(Volume* volume)
{
	QString info;
	info.sprintf("Slice Size X %d   Y %d   Z %d", volume->getXiSize(), volume->getYiSize(), volume->getZiSize());
	volumeSizeLabel->setText(info);
	info.sprintf("Spacing    X %.2f  Y %.2f  Z %.2f", volume->getXSpace(), volume->getYSpace(), volume->getZSpace());
	volumeSpacingLabel->setText(info);
}

void BasicControlWidget::updateViewpointNum(double value)
{
	int num = int(value) * int(value) * 12;
	stringstream ss;
	ss << "Viewpoint Number(" << num << ")";
	heapixNsideLabel->setText(ss.str().c_str());
}

void BasicControlWidget::setHeapixNsideValue(int value)
{
	heapixNsideSpinBox->setValue(value);
	heapixNsideSpinBox->setEnabled(false);
}