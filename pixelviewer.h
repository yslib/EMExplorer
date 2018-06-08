#ifndef PIXELVIWER_H
#define PIXELVIWER_H
#include <QLabel>
#include <QGridLayout>
#include <QImage>
#include <QPoint>
#include <QSharedPointer>
#include <QLineEdit>
//#include "ItemContext.h"

#include "abstractplugin.h"


class PixelViewer:public AbstractPlugin
{
    Q_OBJECT
public:
    PixelViewer(SliceType type, const QString & name,SliceView * view = nullptr, AbstractSliceDataModel * model = nullptr, QWidget * parent = nullptr);
    int getWidth()const;
    int getHeight()const;
public slots:
    void setPosition(const QPoint & p);
	void sliceSelected(const QPoint& pos) Q_DECL_OVERRIDE;
protected:
	void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
protected slots:
	void sliceOpened(int index) override;
private:
    void changeLayout(QSize areaSize);
    void changeValue(const QImage & image,const QPoint & pos);
	void calcCount(QSize areaSize);
	void setWidget(QWidget * widget,int xpos, int ypos);
	void setWidth(int width);
	void setHeight(int height);
	void setImage(const QImage & image);
private:
	static const int s_width = 50;
	static const int s_height = 20;
	static const int s_top = 0;
	static const int s_bottom = 0;
	static const int s_left = 0;
	static const int s_right = 0;

    QImage m_image;
    QVector<QSharedPointer<QLineEdit>> m_pixelLabels;
	QVector<bool> m_flags;
    QVector<QSharedPointer<QLabel>> m_columnHeadersLabels;
    QVector<QSharedPointer<QLabel>> m_rowHeadersLabels;
    QSharedPointer<QLabel> m_cornerLabel;
    QPoint m_pos;
    int m_width;
    int m_height;
    int m_minValueIndex;
    int m_maxValueIndex;
	int m_centroidIndex;
    //QGridLayout * layout;
};

#endif // PIXELVIWER_H
