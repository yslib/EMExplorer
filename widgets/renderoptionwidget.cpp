#include "renderoptionwidget.h"
#include "renderwidget.h"

#include <QGroupBox>
#include <QBoxLayout>
#include <QListView>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QSlider>
#include <QLineEdit>


RenderParameterWidget::RenderParameterWidget(QWidget* parent)
	:QWidget(parent)
{
	m_renderOptions.reset(new RenderOptions);
	//volume info
	m_volumeInfoGroup = new QGroupBox(QStringLiteral("Volume Info"));
	QVBoxLayout * vLayout = new QVBoxLayout;
	//m_volumeSizeLabel = new QLabel(QStringLiteral("Slice Size"));
	m_volumeSpacingLabel = new QLabel(QStringLiteral("Spacing"));
	m_xSpacingSpinBox = new QDoubleSpinBox(this);
	m_xSpacingSpinBox->setRange(0.1, 10.0);
	m_xSpacingSpinBox->setSingleStep(0.1);
	m_xSpacingSpinBox->setValue(1.0);

	connect(m_xSpacingSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
		m_renderOptions->xSpacing = value;
		emit optionsChanged();
	});

	m_ySpacingSpinBox = new QDoubleSpinBox(this);
	m_ySpacingSpinBox->setRange(0.1, 10.0);
	m_ySpacingSpinBox->setSingleStep(0.1);
	m_ySpacingSpinBox->setValue(1.0);

	connect(m_ySpacingSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
		m_renderOptions->ySpacing = value;
		emit optionsChanged();
	});
	m_zSpacingSpinBox = new QDoubleSpinBox(this);
	m_zSpacingSpinBox->setRange(0.1, 10.0);
	m_zSpacingSpinBox->setSingleStep(0.1);
	m_zSpacingSpinBox->setValue(1.0);
	connect(m_zSpacingSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
		m_renderOptions->zSpacing = value;
		emit optionsChanged();
	});
	QHBoxLayout * hLayout = new QHBoxLayout;
	//vLayout->addWidget(m_volumeSizeLabel);
	//vLayout->addWidget(m_volumeSpacingLabel);
	hLayout->addWidget(m_volumeSpacingLabel);
	hLayout->addWidget(m_xSpacingSpinBox);
	hLayout->addWidget(m_ySpacingSpinBox);
	hLayout->addWidget(m_zSpacingSpinBox);
	vLayout->addLayout(hLayout);
	m_volumeInfoGroup->setLayout(vLayout);

	//lighting parameter
	m_lightingGroup = new QGroupBox(QStringLiteral("Lighting Parameters"));
	vLayout = new QVBoxLayout;
	// ambient
	hLayout = new QHBoxLayout;
	m_ambientLabel = new QLabel(QStringLiteral("ambient"));
	m_ambientSpinBox = new QDoubleSpinBox;
	m_ambientSpinBox->setRange(0, 1.0);
	m_ambientSpinBox->setValue(m_renderOptions->ambient);
	m_ambientSpinBox->setSingleStep(0.01);
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
	m_markListGroup = new QGroupBox(QStringLiteral("Mark List"));
	m_markListView = new QListView(this);
	vLayout = new QVBoxLayout;
	vLayout->addWidget(m_markListView);
	m_markListGroup->setLayout(vLayout);


	m_meshGroup = new QGroupBox(QStringLiteral("Mesh"));
	m_meshUpdateButton = new QPushButton(QStringLiteral("Update Mesh"));
	connect(m_meshUpdateButton, &QPushButton::clicked, this,&RenderParameterWidget::markUpdated);
	vLayout = new QVBoxLayout;
	vLayout->addWidget(m_meshUpdateButton);
	m_meshGroup->setLayout(vLayout);

	m_sliceGroup = new QGroupBox(QStringLiteral("Slice"));
	m_radLabel = new QLabel(QStringLiteral("R:"));
	m_radSlider = new QSlider(Qt::Horizontal);
	m_radSlider->setMinimum(0);
	m_radSlider->setMaximum(100);
	connect(m_radSlider, &QSlider::valueChanged, this, &RenderParameterWidget::radialSliderChanged);
	m_radValueLabel = new QLabel;
	m_radValueLabel->setText(QStringLiteral("0.00"));
	m_thetaLabel = new QLabel(QStringLiteral("Theta:"));
	m_thetaSlider = new QSlider(Qt::Horizontal);
	m_thetaSlider->setMinimum(0);
	m_thetaSlider->setMaximum(360);
	connect(m_thetaSlider, &QSlider::valueChanged, this, &RenderParameterWidget::thetaSliderChanged);
	m_thetaValueLabel = new QLabel;
	m_thetaValueLabel->setText(QStringLiteral("0"));
	m_phiLabel = new QLabel(QStringLiteral("Phi:"));
	m_phiSlider = new QSlider(Qt::Horizontal);
	m_phiSlider->setMinimum(-90);
	m_phiSlider->setMaximum(90);
	connect(m_phiSlider, &QSlider::valueChanged, this, &RenderParameterWidget::phiSliderChanged);
	m_phiValueLabel = new QLabel;
	m_phiValueLabel->setText(QStringLiteral("0"));
	auto gridLayout = new QGridLayout;
	gridLayout->addWidget(m_radLabel,0, 0);
	gridLayout->addWidget(m_radSlider, 0, 1);
	gridLayout->addWidget(m_radValueLabel, 0, 2);
	gridLayout->addWidget(m_thetaLabel, 1, 0);
	gridLayout->addWidget(m_thetaSlider, 1, 1);
	gridLayout->addWidget(m_thetaValueLabel, 1, 2);
	gridLayout->addWidget(m_phiLabel, 2, 0);
	gridLayout->addWidget(m_phiSlider, 2, 1);
	gridLayout->addWidget(m_phiValueLabel, 2, 2);
	m_sliceGroup->setLayout(gridLayout);

	/// Add New Widget above

	vLayout = new QVBoxLayout;
	vLayout->addWidget(m_volumeInfoGroup);
	vLayout->addWidget(m_lightingGroup);
	vLayout->addWidget(m_renderOptionGroup);
	vLayout->addWidget(m_markListGroup);
	vLayout->addWidget(m_meshGroup);
	vLayout->addWidget(m_sliceGroup);
	vLayout->addStretch();
	setLayout(vLayout);
}

void RenderParameterWidget::setMarkModel(QAbstractItemModel* model) 
{
	m_markListView->setModel(model);
}

void RenderParameterWidget::radialSliderChanged(int value) {
	const auto rad = value * 0.01;
	m_radValueLabel->setText(QString::number(rad,'f',2));
	m_renderOptions->sliceNormal.setX(rad);
	emit optionsChanged();
}

void RenderParameterWidget::thetaSliderChanged(int value)
{
	const auto theta = static_cast<double>(value);
	m_thetaValueLabel->setText(QString::number(theta, 'f',0));
	m_renderOptions->sliceNormal.setY(theta);
	emit optionsChanged();
}


void RenderParameterWidget::phiSliderChanged(int value)
{
	const auto phi = static_cast<double>(value);
	m_phiValueLabel->setText(QString::number(phi, 'f', 0));
	m_renderOptions->sliceNormal.setZ(phi);
	emit optionsChanged();
}
