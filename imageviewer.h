#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>
#include <QScrollArea>
#include <QImage>


QT_BEGIN_NAMESPACE
class QGridLayout;
class QLabel;
class QWheelEvent;
QT_END_NAMESPACE
class ImageViewer : public QWidget
{
    Q_OBJECT
    QLabel *m_imageLabel;
    QGridLayout * m_layout;
    qreal m_factor;
    QImage m_image;
    bool m_loaded;
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void wheelEvent(QWheelEvent * event);
public:
    explicit ImageViewer(QWidget *parent = nullptr);
    void setImage(const QImage & image);
    void zoom(qreal factor);
signals:

public slots:
};

#endif // IMAGEVIEWER_H
