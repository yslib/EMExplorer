#ifndef PIXELVIWER_H
#define PIXELVIWER_H
#include <QWidget>
#include <QVector>
#include <QLabel>
#include <QVariant>
#include <QGridLayout>
#include <QImage>
#include <QPoint>

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
    void onPositionChanged(const QPoint & p);
private:
    void changeLayout(int width,int height);
    void changeValue(const QImage & image,const QPoint & pos);
private:
    QImage m_image;
    QVector<QLabel*> m_pixels;
    QPoint m_pos;
    int m_width;
    int m_height;
    QGridLayout * layout;
};

#endif // PIXELVIWER_H
