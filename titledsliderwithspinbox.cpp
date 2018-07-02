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

void TitledSliderWithSpinBox::setEnabled(bool enabled)
{
	m_slider->setEnabled(enabled);
	m_spinBox->setEnabled(enabled);
}

void TitledSliderWithSpinBox::setOrientation(Qt::Orientation orientation)
{
	createLayout(orientation);
}

bool TitledSliderWithSpinBox::blockSignals(bool block)
{
	bool old1 = m_slider->blockSignals(block);
	bool old2 = m_spinBox->blockSignals(block);
	Q_ASSERT(old1 == old2);
	return old1;
}

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


