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

class Histogram:public QWidget
{
    Q_OBJECT
public:
    explicit Histogram(QWidget *parent = nullptr);
    explicit Histogram(QWidget *parent, const QImage & image);

    void setImage(const QImage & image);
    QVector<int> getHist()const;
    QSize sizeHint()const override;
signals:
    void minValueChanged();
    void maxValueChanged();

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
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

#endif // HISTOGRAM_H
