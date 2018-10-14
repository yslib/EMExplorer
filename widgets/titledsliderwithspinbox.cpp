#include "titledsliderwithspinbox.h"
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLayout>


TitledSliderWithSpinBox::TitledSliderWithSpinBox(QWidget *parent, const QString &title, Qt::Orientation orientation) :QWidget(parent)
{
	m_label = new QLabel(title, this);
	m_slider = new QSlider(orientation, this);
	m_spinBox = new QSpinBox(this);
	m_layout = new QGridLayout(this);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	m_layout->addWidget(m_label, 0, 0);
	createLayout(orientation);
	createConnections();
}

void TitledSliderWithSpinBox::setMinimum(int value)
{
	m_slider->setMinimum(value);
	m_spinBox->setMinimum(value);
}


void TitledSliderWithSpinBox::setMaximum(int value)
{
	m_slider->setMaximum(value);
	m_spinBox->setMaximum(value);
}


void TitledSliderWithSpinBox::setRange(int min, int max)
{
	m_slider->setRange(min, max);
	m_spinBox->setRange(min, max);
}

void TitledSliderWithSpinBox::setTracking(bool enable)
{
	return m_slider->setTracking(enable);
}

bool TitledSliderWithSpinBox::hasTracking() const
{
	return m_slider->hasTracking();
}

int TitledSliderWithSpinBox::minimum() const
{
	return m_slider->minimum();
}

int TitledSliderWithSpinBox::maximum() const
{
	return m_slider->maximum();
}

void TitledSliderWithSpinBox::setValue(int value)
{


	m_slider->setValue(value);
	/*
	 *slider will emit a signal to change the value of spinbox
	 */

}
int TitledSliderWithSpinBox::value() const
{
	return m_slider->value();
}

//void TitledSliderWithSpinBox::setEnabled(bool enabled)
//{
//	m_slider->setEnabled(enabled);
//	m_spinBox->setEnabled(enabled);
//}

void TitledSliderWithSpinBox::setOrientation(Qt::Orientation orientation)
{
	createLayout(orientation);
}

//bool TitledSliderWithSpinBox::blockSignals(bool block)
//{
//	bool old1 = m_slider->blockSignals(block);
//	bool old2 = m_spinBox->blockSignals(block);
//	Q_ASSERT(old1 == old2);
//	return old1;
//}

void TitledSliderWithSpinBox::createConnections() const
{
	connect(m_slider, &QSlider::valueChanged,[=](int value)
	{
		bool old = m_spinBox->blockSignals(true);
		m_spinBox->setValue(value);
		m_spinBox->blockSignals(old);
	});

	//QSpinBox::valueChanged need to be casted first because there are overloaded valueChanged signals in QSpinBox
	connect(m_spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
	{
		bool old = m_slider->blockSignals(true);
		m_slider->setValue(value);
		m_slider->blockSignals(old);
	});
	connect(m_slider, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
	connect(m_spinBox, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));

}

void TitledSliderWithSpinBox::createLayout(Qt::Orientation orientation)
{
	m_layout->removeWidget(m_slider);
	m_layout->removeWidget(m_spinBox);
	if (orientation == Qt::Vertical) {
		m_layout->addWidget(m_slider, 1, 0);
		m_layout->addWidget(m_spinBox, 2, 0);
	}
	else {
		m_layout->addWidget(m_slider, 0, 1);
		m_layout->addWidget(m_spinBox, 0, 2);
	}
}

TitledSliderWidthDoubleSpinBox::TitledSliderWidthDoubleSpinBox(const QString& title, Qt::Orientation orientation,QWidget* parent) {
	m_layout = new QHBoxLayout;
	m_label = new QLabel(title,this);
	m_slider = new QSlider(orientation,this);
	m_spinBox = new QDoubleSpinBox(this);

	m_layout->addWidget(m_label);
	m_layout->addWidget(m_slider);
	m_layout->addWidget(m_spinBox);

	connect(m_slider, &QSlider::valueChanged, [this](int value) {
		const auto newValue = value * m_spinBox->singleStep();
		m_spinBox->blockSignals(true);
		m_spinBox->setValue(newValue);
		m_spinBox->blockSignals(false);
		emit valueChanged(newValue);
	});

	connect(m_spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
		const auto newValue = m_spinBox->value() / m_spinBox->singleStep();
		m_spinBox->blockSignals(true);
		m_slider->setValue(newValue);
		m_spinBox->blockSignals(false);
		emit valueChanged(newValue);
	});
	setLayout(m_layout);
	updateSliderProperty();
}

void TitledSliderWidthDoubleSpinBox::setMinimum(double value)
{
	Q_ASSERT_X(m_spinBox, "TitiledSliderWidthDoubleSpinBox", "null pointer");
	m_spinBox->setMinimum(value);
	updateSliderProperty();
}

void TitledSliderWidthDoubleSpinBox::setMaximum(double value)
{

	Q_ASSERT_X(m_spinBox, "TitiledSliderWidthDoubleSpinBox", "null pointer");
	m_spinBox->setMaximum(value);
	updateSliderProperty();
}

void TitledSliderWidthDoubleSpinBox::setValue(double value) {
	Q_ASSERT_X(m_spinBox, "TitiledSliderWidthDoubleSpinBox", "null pointer");
	m_spinBox->setValue(value);
}

void TitledSliderWidthDoubleSpinBox::setRange(double min, double max)
{
	Q_ASSERT_X(m_spinBox, "TitiledSliderWidthDoubleSpinBox", "null pointer");
	m_spinBox->setRange(min, max);
	updateSliderProperty();
}

void TitledSliderWidthDoubleSpinBox::setSingleStep(double step)
{
	Q_ASSERT_X(m_spinBox, "TitiledSliderWidthDoubleSpinBox", "null pointer");
	m_spinBox->setSingleStep(step);
	updateSliderProperty();
}

double TitledSliderWidthDoubleSpinBox::singleStep() const {
	Q_ASSERT_X(m_spinBox, "TitiledSliderWidthDoubleSpinBox", "null pointer");
	return m_spinBox->singleStep();
}

void TitledSliderWidthDoubleSpinBox::setText(const QString& text) 
{
	Q_ASSERT_X(m_label, "TitiledSliderWidthDoubleSpinBox", "null pointer");
	m_label->setText(text);
}
double TitledSliderWidthDoubleSpinBox::minimum() const 
{
	Q_ASSERT_X(m_spinBox, "TitiledSliderWidthDoubleSpinBox", "null pointer");
	return m_spinBox->minimum();
	
}
double TitledSliderWidthDoubleSpinBox::maximum() const 
{
	Q_ASSERT_X(m_spinBox, "TitiledSliderWidthDoubleSpinBox", "null pointer");
	return m_spinBox->maximum();
}
double TitledSliderWidthDoubleSpinBox::value() const 
{
	Q_ASSERT_X(m_spinBox, "TitiledSliderWidthDoubleSpinBox", "null pointer");
	return m_spinBox->value();
}

void TitledSliderWidthDoubleSpinBox::updateSliderProperty() const{

	Q_ASSERT_X(m_spinBox, "TitiledSliderWidthDoubleSpinBox", "spin box null pointer");
	Q_ASSERT_X(m_slider, "TitledSliderWithDoubleSpinBox", "slider null pointer");

	m_slider->setMinimum(0);
	const auto ticks = static_cast<int>(m_spinBox->maximum() / m_spinBox->singleStep());
	m_slider->setMaximum(ticks);

}
