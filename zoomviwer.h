#ifndef ZOOMVIWER_H
#define ZOOMVIWER_H
#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QWheelEvent>

class ZoomViwer:public QWidget
{
	Q_OBJECT
public:
    ZoomViwer(QWidget* parent = nullptr);
    void setImage(const QImage & image);
    void clearImage();
    QRectF zoomRegion()const;
    QPointF zoomPosition()const;
protected:
    void paintEvent(QPaintEvent *event)override;
    void mousePressEvent(QMouseEvent *event)override;
	void wheelEvent(QWheelEvent *event)override;
signals:
	void zoomRegionChanged(QRectF region);
private:
	static const int WIDTH = 300;
	static const int HEIGHT = 200;

    QRectF m_zoomRect;
	size_t m_originalWidth;
	size_t m_originalHeight;
	QImage m_thumbnail;
};

#endif // ZOOMVIWER_H
