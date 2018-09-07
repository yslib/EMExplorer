#include "renderoptionwidget.h"

#include <QGroupBox>
#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include "renderwidget.h"


RenderParameterWidget::RenderParameterWidget(QWidget* parent)
	:QWidget(parent)
{
	m_renderOptions.reset(new RenderOptions);
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
	m_ambientSpinBox->setValue(m_renderOptions->ambient);
	m_ambientSpinBox->setSingleStep(0.01);
	//
	//TODO:: connect with related update function in volumeWidget
	// 
	connect(m_ambientSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {m_renderOptions->ambient = value; emit optionsChanged(); });


	hLayout->addWidget(m_ambientLabel);
	hLayout->addWidget(m_ambientSpinBox);
	vLayout->addLayout(hLayout);

	//diffuse
	hLayout = new QHBoxLayout;
	m_diffuseLabel = new QLabel(QStringLiteral("diffuse"));
	m_diffuseSpinBox = new QDoubleSpinBox;
	m_diffuseSpinBox->setRange(0, 1.0);
	m_diffuseSpinBox->setValue(m_renderOptions->diffuse);
	m_diffuseSpinBox->setSingleStep(0.01);
	//
	// TODO::connect with realted update function in volumeWidget
	// 
	connect(m_ambientSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {m_renderOptions->diffuse = value; emit optionsChanged(); });
	hLayout->addWidget(m_diffuseLabel);
	hLayout->addWidget(m_diffuseSpinBox);
	vLayout->addLayout(hLayout);


	//shininess
	hLayout = new QHBoxLayout;
	m_shininessLabel = new QLabel(QStringLiteral("shininess"));
	m_shininessSpinBox = new QDoubleSpinBox;
	m_shininessSpinBox->setRange(0, 100);
	m_shininessSpinBox->setValue(m_renderOptions->shininess);
	m_shininessSpinBox->setSingleStep(1);
	//
	//TODO::connect with related update function in volumeWidget
	//
	connect(m_shininessSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {m_renderOptions->shininess = value; emit optionsChanged(); });
	hLayout->addWidget(m_shininessLabel);
	hLayout->addWidget(m_shininessSpinBox);
	vLayout->addLayout(hLayout);

	//specular
	hLayout = new QHBoxLayout;
	m_specularLabel = new QLabel(QStringLiteral("specular"));
	m_specularSpinBox = new  QDoubleSpinBox;
	m_specularSpinBox->setRange(0, 1.0);
	m_specularSpinBox->setValue(m_renderOptions->specular);
	m_specularSpinBox->setSingleStep(0.01);
	//
	// TODO::connect with related update function in volumeWidget
	// 
	connect(m_specularSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {m_renderOptions->specular = value; emit optionsChanged(); });
	hLayout->addWidget(m_specularLabel);
	hLayout->addWidget(m_specularSpinBox);
	vLayout->addLayout(hLayout);

	m_lightingGroup->setLayout(vLayout);


	m_renderOptionGroup = new QGroupBox(QStringLiteral("Render Option"), this);
	vLayout = new QVBoxLayout;
	m_renderTypeLabel = new QLabel(QStringLiteral("Render Type"), this);
	m_renderTypeCCBox = new QComboBox(this);
	m_renderTypeCCBox->addItem(QStringLiteral("DVR"));
	m_renderTypeCCBox->addItem(QStringLiteral("Mark FillMesh with Slice"));
	m_renderTypeCCBox->addItem(QStringLiteral("Mark LineMesh with Slice"));
	

	connect(m_renderTypeCCBox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), [this](const QString & text) {
		if(text == QStringLiteral("DVR")) {
			m_renderOptions->mode = (RenderMode::DVR);
		}else if(text == QStringLiteral("Mark FillMesh with Slice")) {
			m_renderOptions->mode = RenderMode(RenderMode::SliceTexture&RenderMode::FillMesh);
		}else if(text == QStringLiteral("Mark LineMesh with Slice")) {
			m_renderOptions->mode = RenderMode(RenderMode::SliceTexture&RenderMode::LineMesh);
		}
		emit optionsChanged();
	});

	hLayout = new QHBoxLayout;
	hLayout->addWidget(m_renderTypeLabel);
	hLayout->addWidget(m_renderTypeCCBox);
	vLayout->addLayout(hLayout);
	m_renderOptionGroup->setLayout(vLayout);


	vLayout = new QVBoxLayout;
	vLayout->addWidget(m_volumeInfoGroup);
	vLayout->addWidget(m_lightingGroup);
	vLayout->addWidget(m_renderOptionGroup);
	vLayout->addStretch();
	setLayout(vLayout);



}



//void RenderParameterWidget::connectWith(VolumeWidget* widget) {
//	if (widget == nullptr)return;
//	connect(widget, &VolumeWidget::markModelChanged, this, &RenderParameterWidget::updateMarkModel);
//	connect(widget, &VolumeWidget::dataModelChanged, this, &RenderParameterWidget::updateDataModel);
//
//}
//void RenderParameterWidget::disconnectWith(VolumeWidget* widget) {
//	if (widget == nullptr) return;
//	disconnect(widget, &VolumeWidget::markModelChanged, this, &RenderParameterWidget::updateMarkModel);
//	disconnect(widget, &VolumeWidget::dataModelChanged, this, &RenderParameterWidget::updateDataModel);
//
//}

