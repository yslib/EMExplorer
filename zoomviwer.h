#ifndef ZOOMVIWER_H
#define ZOOMVIWER_H
#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QWheelEvent>
#include <QSlider>
#include <QLayout>
#include <QLabel>
#include <QDoubleSpinBox>

class ZoomViwer:public QWidget
{
	Q_OBJECT
public:
    ZoomViwer(QWidget* parent = nullptr);
    void setImage(const QImage & image);
    void clearImage();
    QRectF zoomRegion()const;
    QPointF zoomPosition()const;
	void setZoomFactor(qreal factor);
	qreal zoomFactor()const { return m_zoomFactor; }
	void setMinZoomFactor(qreal minFactor);
	qreal minZoomFactor()const { return m_minZoomFactor; }
	virtual ~ZoomViwer();
protected:
    void paintEvent(QPaintEvent *event)override;
    void mousePressEvent(QMouseEvent *event)override;
	void wheelEvent(QWheelEvent *event)override;
signals:
	void zoomRegionChanged(QRectF region);
private:
	static const int WIDTH = 300;
	static const int HEIGHT = 200;
		

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
