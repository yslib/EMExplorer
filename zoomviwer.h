#ifndef ZOOMVIWER_H
#define ZOOMVIWER_H
#include <QImage>
#include <QPainter>
#include <QWheelEvent>
#include <QSlider>
#include <QLayout>

class ZoomViwer:public QWidget
{
	Q_OBJECT
public:
    ZoomViwer(QWidget* parent = nullptr);
    void setImage(const QImage & image,const QRect & region = QRect());
    void clearImage();

    QRectF zoomRegion()const;
    void setZoomRegion(const QRect &region);
    QPointF zoomPosition()const;

    //void setZoomFactor(qreal factor);
	qreal zoomFactor()const { return m_zoomFactor; }

	void setMinZoomFactor(qreal minFactor);
	qreal minZoomFactor()const { return m_minZoomFactor; }

	virtual ~ZoomViwer();
protected:
    void paintEvent(QPaintEvent *event)override;
    void mousePressEvent(QMouseEvent *event)override;
	void wheelEvent(QWheelEvent *event)override;
private:
    void _setZoomRect(qreal factor,const QPointF &leftTopPos = QPointF());
    QRectF _regionToRect(const QRect &region);
signals:
    void zoomRegionChanged(const QRectF & region);
    //void zoomFactorChanged(qreal factor);
private:
    static constexpr int WIDTH = 300;
    static constexpr int HEIGHT = 200;
		

	qreal m_zoomFactor;
	qreal m_minZoomFactor;
	QRectF m_imageRect;		//The position of the thumbnail in the Widget
    QRectF m_zoomRect;		//The zoom region of the image in the thumbnail
	/*Width and Height of original image*/
	size_t m_originalWidth;	
	size_t m_originalHeight;
	QImage m_thumbnail;
};

#endif // ZOOMVIWER_H
