#ifndef ZOOMVIWER_H
#define ZOOMVIWER_H
#include <QWidget>

class ZoomViwer:public QWidget
{
public:
    ZoomViwer(QWidget* parent = nullptr);
    void setImage(const QImage & image);
    void clearImage();
    QRectF zoomRegion()const;
    QPointF zoomPosition()const;
protected:
    void paintEvent(QPaintEvent *event)override;
    void mousePressEvent(QMouseEvent *event)override;
private:
    QRectF m_rect;
    QImage m_image;
    QPointF m_topLeftOfRect;
};

#endif // ZOOMVIWER_H
