#ifndef LABELSLIDERWITHSPINBOX_H
#define LABELSLIDERWITHSPINBOX_H

#include <QLabel>
#include <QSpinBox>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QLayout>
#include <QWidget>

class TitledSliderWithSpinBox:public QWidget
{
    Q_OBJECT
public:
    explicit TitledSliderWithSpinBox(QWidget * parent = nullptr,const QString & title = QString());
    void setMinimum(int value);
    void setMaximum(int value);
    void setRange(int min,int max);
    void getMinimum()const;
    void getMaximum()const;
    int minimum()const;
    int maximum()const;

    int value()const;
    void setEnabled(bool enabled);
public slots:
    void setValue(int value);

signals:
    void valueChanged(int value);
private:
    void createConnections();
    void createLayout();
private:
    QSpinBox * m_spinBox;
    QSlider * m_slider;
    QLabel * m_label;
};

class TitledSliderWidthDoubleSpinBox:public QWidget{
  Q_OBJECT
public:
    explicit TitledSliderWidthDoubleSpinBox(QWidget * parent = nullptr){

    }
};
#endif // LABELSLIDERWITHSPINBOX_H
