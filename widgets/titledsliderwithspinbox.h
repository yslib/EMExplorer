#ifndef LABELSLIDERWITHSPINBOX_H
#define LABELSLIDERWITHSPINBOX_H
#include <QWidget>

class QLayout;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QSlider;
class QLabel;
class QSpinBox;
class QDoubleSpinBox;

class TitledSliderWithSpinBox:public QWidget
{
    Q_OBJECT
public:
    explicit TitledSliderWithSpinBox(QWidget * parent = nullptr,const QString & title = QString(),Qt::Orientation orientation = Qt::Horizontal);
    void setMinimum(int value);
    void setMaximum(int value);
    void setRange(int min,int max);
	void setTracking(bool enable);
	bool hasTracking()const;
    int minimum()const;
    int maximum()const;
    int value()const;
	//void setEnabled(bool enabled);
    void setOrientation(Qt::Orientation orientation);

public slots:
    void setValue(int value);
signals:
    void valueChanged(int value);
private:
    void createConnections()const;
    void createLayout(Qt::Orientation orientation = Qt::Horizontal);
private:
    QSpinBox * m_spinBox;
    QSlider * m_slider;
    QLabel * m_label;
    QGridLayout * m_layout;
};

class TitledSliderWidthDoubleSpinBox:public QWidget{
  Q_OBJECT
public:
	TitledSliderWidthDoubleSpinBox(const QString & title,Qt::Orientation orientation,QWidget * parent = nullptr);
	void setMinimum(double value);
	void setMaximum(double value);
	void setValue(double value);
	void setRange(double min, double max);
	void setSingleStep(double step);
	double singleStep() const;
	void setText(const QString & text);
	double minimum()const;
	double maximum()const;
	double value()const;
signals:
	void valueChanged(double value);
private:
	void updateSliderProperty()const;
private:
	QHBoxLayout * m_layout;
	QDoubleSpinBox * m_spinBox;
	QSlider * m_slider;
	QLabel * m_label;
};
#endif // LABELSLIDERWITHSPINBOX_H
