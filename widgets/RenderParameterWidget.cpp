#include "RenderParameterWidget.h"

#include <QGroupBox>
#include <QBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include "volumewidget.h"


RenderParameterWidget::RenderParameterWidget(VolumeWidget* volumeWidget, QWidget* parent)
	:QWidget(parent),m_volumeWidget(volumeWidget)
{
	//volume info
	m_volumeInfoGroup = new QGroupBox(QStringLiteral("Volume Info"));
	QVBoxLayout * vLayout = new QVBoxLayout;
	m_volumeSizeLabel = new QLabel(QStringLiteral("Slice Size"));
	m_volumeSpacingLabel = new QLabel(QStringLiteral("Spacing"));
	vLayout->addWidget(m_volumeSizeLabel);
	vLayout->addWidget(m_volumeSpacingLabel);
	m_volumeInfoGroup->setLayout(vLayout);

	//lighting parameter
	m_lightingGroup = new QGroupBox(QStringLiteral("Lighting Parameters"));
	vLayout = new QVBoxLayout;
	// ambient
	QHBoxLayout * hLayout = new QHBoxLayout;
	m_ambientLabel = new QLabel(QStringLiteral("ambient"));
	m_ambientSpinBox = new QDoubleSpinBox;
	m_ambientSpinBox->setRange(0, 1.0);
	m_ambientSpinBox->setValue(1.0);
	m_ambientSpinBox->setSingleStep(0.01);
	//
	//TODO:: connect with related update function in volumeWidget
	// 
	hLayout->addWidget(m_ambientLabel);
	hLayout->addWidget(m_ambientSpinBox);
	vLayout->addLayout(hLayout);

	//diffuse
	hLayout = new QHBoxLayout;
	m_diffuseLabel = new QLabel(QStringLiteral("diffuse"));
	m_diffuseSpinBox = new QDoubleSpinBox;
	m_diffuseSpinBox->setRange(0, 1.0);
	m_diffuseSpinBox->setValue(1.0);
	m_diffuseSpinBox->setSingleStep(0.01);
	//
	// TODO::connect with realted update function in volumeWidget
	// 
	hLayout->addWidget(m_diffuseLabel);
	hLayout->addWidget(m_diffuseSpinBox);
	vLayout->addLayout(hLayout);

	//shininess
	hLayout = new QHBoxLayout;
	m_shininessLabel = new QLabel(QStringLiteral("shininess"));
	m_shininessSpinBox = new QDoubleSpinBox;
	m_shininessSpinBox->setRange(0, 100);
	m_shininessSpinBox->setValue(50);
	m_shininessSpinBox->setSingleStep(1);
	//
	//TODO::connect with related update function in volumeWidget
	//
	hLayout->addWidget(m_shininessLabel);
	hLayout->addWidget(m_shininessSpinBox);
	vLayout->addLayout(hLayout);

	//specular
	hLayout = new QHBoxLayout;
	m_specularLabel = new QLabel(QStringLiteral("specular"));
	m_specularSpinBox = new  QDoubleSpinBox;
	m_specularSpinBox->setRange(0, 1.0);
	m_specularSpinBox->setValue(0.75);
	m_specularSpinBox->setSingleStep(0.01);
	//
	// TODO::connect with related update function in volumeWidget
	// 
	hLayout->addWidget(m_specularLabel);
	hLayout->addWidget(m_specularSpinBox);
	vLayout->addLayout(hLayout);

	m_lightingGroup->setLayout(vLayout);

	vLayout = new QVBoxLayout;
	vLayout->addWidget(m_volumeInfoGroup);
	vLayout->addWidget(m_lightingGroup);
	vLayout->addStretch();
	setLayout(vLayout);
}

RenderParameterWidget::~RenderParameterWidget()
{

}

void RenderParameterWidget::updateDataModel()
{
}

void RenderParameterWidget::updateMarkModel()
{
}
