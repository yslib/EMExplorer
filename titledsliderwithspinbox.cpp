#include "titledsliderwithspinbox.h"

TitledSliderWithSpinBox::TitledSliderWithSpinBox(QWidget *parent, const QString &title):QWidget(parent)
{
    m_label = new QLabel(title,this);
    m_slider = new QSlider(Qt::Horizontal,this);
    m_spinBox = new QSpinBox(this);

    createLayout();
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
    m_slider->setRange(min,max);
    m_spinBox->setRange(min,max);
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
   m_spinBox->setValue(value);
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

void TitledSliderWithSpinBox::createConnections()
{
    connect(m_slider,SIGNAL(valueChanged(int)),this,SIGNAL(valueChanged(int)));
    connect(m_spinBox,SIGNAL(valuedChanged(int)),this,SIGNAL(valueChanged(int)));
    connect(m_slider,SIGNAL(valueChanged(int)),m_spinBox,SLOT(setValue(int)));
    connect(m_spinBox,SIGNAL(valueChanged(int)),m_slider,SLOT(setValue(int)));
}

void TitledSliderWithSpinBox::createLayout()
{
    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setMargin(0);
    setLayout(layout);

    layout->addWidget(m_label);
    layout->addWidget(m_slider);
    layout->addWidget(m_spinBox);
}


