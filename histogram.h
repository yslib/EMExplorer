#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QObject>
#include <QWidget>
#include <QImage>
#include <QColor>
#include <QVector>
#include <QMessageBox>
#include <QPainter>
#include <cmath>
#include <QLayout>
#include <titledsliderwithspinbox.h>

class Histogram:public QWidget
{
    Q_OBJECT
public:
    explicit Histogram(QWidget *parent = nullptr);
    explicit Histogram(QWidget *parent, const QImage & image);

    void setImage(const QImage & image);
    QVector<int> getHist()const;
    QSize sizeHint()const override;
	void setMinimumValue(int value);
	void setMaximumValue(int value);
signals:
    //void minValueChanged(int value);
    //void maxValueChanged(int value);
	void valueChanged(int minVal, int maxVal);
protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
private:
    static const int MIN_WIDTH = 300;
    static const int MIN_HEIGHT = 100;
    static const int BIN_COUNT = 256;

    QVector<int> m_hist;
    int m_minValue;
    int m_maxValue;
    size_t m_count;
    QColor m_curColor;
    QImage m_histImage;
};


class HistogramViewer:public QWidget
{
    Q_OBJECT
public:
    explicit HistogramViewer(QWidget * parent = nullptr)noexcept;
    explicit HistogramViewer(QWidget * parent, const QImage & image)noexcept;

    void setImage(const QImage & image);
    QVector<int> getHist()const;
    void setMinimumValue(int value);
    void setMaximumValue(int value);
signals:
    void valueChanged(int min,int max);
private:
    QGridLayout * m_layout;
    Histogram * m_hist;
    TitledSliderWithSpinBox * m_minSlider;
    TitledSliderWithSpinBox * m_maxSlider;
};

#endif // HISTOGRAM_H
