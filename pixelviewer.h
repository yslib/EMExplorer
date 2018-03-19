#ifndef PIXELVIWER_H
#define PIXELVIWER_H
#include <QWidget>
#include <QVector>
#include <QLabel>
#include <QVariant>
#include <QGridLayout>
#include <QPushButton>
#include <QImage>
#include <QPoint>
#include <QSharedPointer>
class PixelViewer:public QWidget
{
    Q_OBJECT
public:
    PixelViewer(QWidget*parent = 0, int width=5, int height=5, const QImage & image = QImage());
    int getWidth()const;
    int getHeight()const;
    void setWidth(int width);
    void setHeight(int height);
    void setImage(const QImage & image);
public slots:
    void setPosition(const QPoint & p);
private:
    void changeLayout(int width,int height);
    void changeValue(const QImage & image,const QPoint & pos);
private:
    QImage m_image;
    QVector<QSharedPointer<QPushButton>> m_pixelLabels;
    QVector<QSharedPointer<QLabel>> m_columnHeadersLabels;
    QVector<QSharedPointer<QLabel>> m_rowHeadersLabels;
    QSharedPointer<QLabel> m_cornerLabel;
    QPoint m_pos;
    int m_width;
    int m_height;
    int m_minValueIndex;
    int m_maxValueIndex;
    QGridLayout * layout;
};

#endif // PIXELVIWER_H
