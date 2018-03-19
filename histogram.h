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
    void setMinimumCursorValue(int value);
    void setMaximumCursorValue(int value);
    int getMinimumCursorValue()const;
    int getMaximumCursorValue()const;
    void setCursorEnable(bool enable);
signals:
    void minCursorValueChanged(int value);
    void maxCursorValueChanged(int value);
    void cursorValueChanged(int minVal, int maxVal);
protected:
    void paintEvent(QPaintEvent *event)override;
    void mouseMoveEvent(QMouseEvent *event)override;
    void mousePressEvent(QMouseEvent *event)override;
    void mouseReleaseEvent(QMouseEvent *event)override;
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

    bool m_mousePressed;
    bool m_rightCursorSelected;
    bool m_leftCursorSelected;
    bool m_cursorEnable;

private:
    qreal getXofLeftCursor();
    qreal getXofRightCursor();


};


class HistogramViewer:public QWidget
{
    Q_OBJECT
public:
    explicit HistogramViewer(QWidget * parent = nullptr)noexcept;
    explicit HistogramViewer(QWidget * parent, const QImage & image)noexcept;

    void setImage(const QImage & image);
    QVector<int> getHist()const;
    void setMinimumMarkPosition(int value);
    void setMaximumMarkPosition(int value);
signals:
    void valueChanged(int min,int max);
private:
    QGridLayout * m_layout;
    Histogram * m_hist;
    TitledSliderWithSpinBox * m_minSlider;
    TitledSliderWithSpinBox * m_maxSlider;
};

#endif // HISTOGRAM_H
