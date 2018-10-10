#include "renderoptionwidget.h"
#include "renderwidget.h"
#include "TF1DEditor.h"
#include "widgets/TF1DTextureCanvas.h"
#include "widgets/TF1DMappingCanvas.h"

#include <QGroupBox>
#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QSlider>
#include <QLineEdit>
#include <QToolButton>

RenderParameterWidget::RenderParameterWidget(RenderWidget * widget,QWidget* parent)
	:QWidget(parent)
	,m_widget(nullptr)
{
	//volume info
	m_volumeInfoGroup = new QGroupBox(QStringLiteral("Volume Info"));
	QVBoxLayout * volumeInfoVLayout = new QVBoxLayout;
	//m_volumeSizeLabel = new QLabel(QStringLiteral("Slice Size"));
	m_volumeSpacingLabel = new QLabel(QStringLiteral("Spacing"));
	m_xSpacingSpinBox = new QDoubleSpinBox(this);
	m_xSpacingSpinBox->setRange(0.1, 10.0);
	m_xSpacingSpinBox->setSingleStep(0.1);
	connect(m_xSpacingSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&,this](double value) {m_renderOptions->xSpacing = value;emit optionsChanged();});

	m_ySpacingSpinBox = new QDoubleSpinBox(this);
	m_ySpacingSpinBox->setRange(0.1, 10.0);
	m_ySpacingSpinBox->setSingleStep(0.1);
	

	connect(m_ySpacingSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&,this](double value) {m_renderOptions->ySpacing = value;emit optionsChanged(); });
	m_zSpacingSpinBox = new QDoubleSpinBox(this);
	m_zSpacingSpinBox->setRange(0.1, 10.0);
	m_zSpacingSpinBox->setSingleStep(0.1);
	
	connect(m_zSpacingSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&,this](double value) {m_renderOptions->zSpacing = value;emit optionsChanged();});
	QHBoxLayout * volumeInfoHLayout = new QHBoxLayout;
	//vLayout->addWidget(m_volumeSizeLabel);
	//vLayout->addWidget(m_volumeSpacingLabel);
	volumeInfoHLayout->addWidget(m_volumeSpacingLabel);
	volumeInfoHLayout->addWidget(m_xSpacingSpinBox);
	volumeInfoHLayout->addWidget(m_ySpacingSpinBox);
	volumeInfoHLayout->addWidget(m_zSpacingSpinBox);
	volumeInfoVLayout->addLayout(volumeInfoHLayout);
	m_volumeInfoGroup->setLayout(volumeInfoVLayout);

	//lighting parameter
	m_lightingGroup = new QGroupBox(QStringLiteral("Lighting Parameters"));
	auto lightingVLayout = new QVBoxLayout;
	// ambient
	auto ambientHLayout = new QHBoxLayout;
	m_ambientLabel = new QLabel(QStringLiteral("ambient"));
	m_ambientSpinBox = new QDoubleSpinBox;
	m_ambientSpinBox->setRange(0, 1.0);
	
	m_ambientSpinBox->setSingleStep(0.01);
	connect(m_ambientSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&,this](double value) {m_renderOptions->ambient = value; emit optionsChanged(); });


	ambientHLayout->addWidget(m_ambientLabel);
	ambientHLayout->addWidget(m_ambientSpinBox);
	lightingVLayout->addLayout(ambientHLayout);

	//diffuse
	auto diffuseHLayout = new QHBoxLayout;
	m_diffuseLabel = new QLabel(QStringLiteral("diffuse"));
	m_diffuseSpinBox = new QDoubleSpinBox;
	m_diffuseSpinBox->setRange(0, 1.0);

	m_diffuseSpinBox->setSingleStep(0.01);
	connect(m_ambientSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&,this](double value) {m_renderOptions->diffuse = value; emit optionsChanged(); });
	diffuseHLayout->addWidget(m_diffuseLabel);
	diffuseHLayout->addWidget(m_diffuseSpinBox);
	lightingVLayout->addLayout(diffuseHLayout);


	//shininess
	auto shininessHLayout = new QHBoxLayout;
	m_shininessLabel = new QLabel(QStringLiteral("shininess"));
	m_shininessSpinBox = new QDoubleSpinBox;
	m_shininessSpinBox->setRange(0, 100);
	
	m_shininessSpinBox->setSingleStep(1);
	connect(m_shininessSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&,this](double value) {m_renderOptions->shininess = value; emit optionsChanged(); });
	shininessHLayout->addWidget(m_shininessLabel);
	shininessHLayout->addWidget(m_shininessSpinBox);
	lightingVLayout->addLayout(shininessHLayout);

	//specular
	auto specularHLayout = new QHBoxLayout;
	m_specularLabel = new QLabel(QStringLiteral("specular"));
	m_specularSpinBox = new  QDoubleSpinBox;
	m_specularSpinBox->setRange(0, 1.0);
	
	m_specularSpinBox->setSingleStep(0.01);
	connect(m_specularSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&,this](double value) {m_renderOptions->specular = value; emit optionsChanged(); });
	specularHLayout->addWidget(m_specularLabel);
	specularHLayout->addWidget(m_specularSpinBox);
	lightingVLayout->addLayout(specularHLayout);

	m_lightingGroup->setLayout(lightingVLayout);

	//Render Type
	m_renderOptionGroup = new QGroupBox(QStringLiteral("Render Option"), this);
	auto renderTypeVLayout = new QVBoxLayout;
	m_renderTypeLabel = new QLabel(QStringLiteral("Render Type"), this);
	m_renderTypeCCBox = new QComboBox(this);
	m_renderTypeCCBox->addItem(QStringLiteral("DVR"));
	m_renderTypeCCBox->addItem(QStringLiteral("Mark FillMesh with Slice"));
	m_renderTypeCCBox->addItem(QStringLiteral("Mark LineMesh with Slice"));

	connect(m_renderTypeCCBox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&RenderParameterWidget::renderTypeChanged);

	auto renderTypeHLayout = new QHBoxLayout;
	renderTypeHLayout->addWidget(m_renderTypeLabel);
	renderTypeHLayout->addWidget(m_renderTypeCCBox);
	renderTypeVLayout->addLayout(renderTypeHLayout);
	m_renderOptionGroup->setLayout(renderTypeVLayout);



	// Transfer Function Widget
	m_transferFunctionGroupBox = new QGroupBox("Transfer Function");
	auto tfWidgetHLayout = new QHBoxLayout;
	m_tfButton = new QToolButton(this);
	m_tfButton->setIcon(QIcon(QStringLiteral(":icons/resources/icons/histogram.png")));
	m_tfButton->setToolTip(QStringLiteral("1D Transfer function"));
	connect(m_tfButton, &QToolButton::clicked, this, &RenderParameterWidget::tfButtonClicked);
	m_tfButton->setStyleSheet("QToolButton::menu-indicator{image: none;}");

	m_tfEditor = new TF1DEditor(this);
	m_tfEditor->setMinimumSize(250, 300);
	m_tfEditor->setWindowFlags(Qt::Dialog);
	m_tfEditor->setWindowModality(Qt::NonModal);
	//m_tfEditor->setVisible(false);

	 auto tfMappingCanvas = m_tfEditor->getTF1DMappingCanvas();
	 auto tfTextureWidget = new TF1DTextureCanvas(tfMappingCanvas, this);
	 tfTextureWidget->setFixedHeight(15);
	auto slot = [tfTextureWidget]() {tfTextureWidget->update();};
	connect(tfMappingCanvas, &TF1DMappingCanvas::changed,slot);
	connect(tfMappingCanvas, &TF1DMappingCanvas::loadTransferFunction,slot);
	connect(tfMappingCanvas, &TF1DMappingCanvas::saveTransferFunction,slot);
	connect(tfMappingCanvas, &TF1DMappingCanvas::resetTransferFunction,slot);
	connect(tfMappingCanvas, &TF1DMappingCanvas::toggleInteraction,slot);

	tfWidgetHLayout->addWidget(tfTextureWidget, 4, Qt::AlignLeft);
	tfWidgetHLayout->addWidget(m_tfButton,1,Qt::AlignLeft);

	m_transferFunctionGroupBox->setLayout(tfWidgetHLayout);
	connect(m_tfEditor, &TF1DEditor::TF1DChanged,this,&RenderParameterWidget::transferFunctionChanged);


	m_meshGroup = new QGroupBox(QStringLiteral("Mesh"));
	m_meshUpdateButton = new QPushButton(QStringLiteral("Update Mesh"));
	connect(m_meshUpdateButton, &QPushButton::clicked, this,&RenderParameterWidget::markUpdated);
	auto meshVLayout = new QVBoxLayout;
	meshVLayout->addWidget(m_meshUpdateButton);
	m_meshGroup->setLayout(meshVLayout);

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

	auto mainVLayout = new QVBoxLayout;
	mainVLayout->addWidget(m_volumeInfoGroup);
	mainVLayout->addWidget(m_lightingGroup);
	mainVLayout->addWidget(m_renderOptionGroup);
	//mainVLayout->addWidget(m_markListGroup);
	mainVLayout->addWidget(m_transferFunctionGroupBox);
	mainVLayout->addWidget(m_meshGroup);
	mainVLayout->addWidget(m_sliceGroup);
	mainVLayout->addStretch();
	setLayout(mainVLayout);

	setRenderWidget(widget);
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

void RenderParameterWidget::transferFunctionChanged() {
	QScopedPointer<float, QScopedPointerArrayDeleter<float>> tfuncs(new float[256 * 4]);
	if (m_tfEditor == nullptr)
		return;
	m_tfEditor->getTransferFunction(tfuncs.data(), 256, 1.0);
	Q_ASSERT_X(m_widget, "&TF1DEditor::TF1DChange", "m_widget != nullptr");
	m_widget->updateTransferFunction(tfuncs.data(), true);
}

void RenderParameterWidget::renderTypeChanged(const QString& text) {

	if (text == QStringLiteral("DVR")) {
		m_renderOptions->mode = (RenderMode::DVR);
	}
	else if (text == QStringLiteral("Mark FillMesh with Slice")) {
		m_renderOptions->mode = RenderMode(RenderMode::SliceTexture&RenderMode::FillMesh);
	}
	else if (text == QStringLiteral("Mark LineMesh with Slice")) {
		m_renderOptions->mode = RenderMode(RenderMode::SliceTexture&RenderMode::LineMesh);
	}
	emit optionsChanged();
}

void RenderParameterWidget::tfButtonClicked() {

	m_tfEditor->setVisible(true);
}


void RenderParameterWidget::setRenderWidget(RenderWidget * widget) {

	if (m_widget == widget)
		return;
	disconnect(this, nullptr, m_widget, nullptr);
	m_widget = widget;
	connect(this, &RenderParameterWidget::optionsChanged, m_widget, [this]() {m_widget->update();});
	connect(this, &RenderParameterWidget::markUpdated, m_widget, &RenderWidget::updateMark);

	m_renderOptions = m_widget != nullptr?widget->options():QSharedPointer<RenderOptions>();
	setEnabled(m_widget != nullptr);
	
	m_xSpacingSpinBox->setValue(1.0);
	m_ySpacingSpinBox->setValue(1.0);
	m_zSpacingSpinBox->setValue(1.0);
	m_ambientSpinBox->setValue(m_renderOptions->ambient);
	m_diffuseSpinBox->setValue(m_renderOptions->diffuse);
	m_shininessSpinBox->setValue(m_renderOptions->shininess);
	m_specularSpinBox->setValue(m_renderOptions->specular);
	m_radSlider->setValue(0);
	m_thetaSlider->setValue(0);
	m_phiSlider->setValue(90);
}
