#ifndef PIXELVIWER_H
#define PIXELVIWER_H
//#include <QLineEdit>

#include "abstract/AbstractSliceEditorPlugin.h"


class QLabel;
class QLineEdit;
class PixelWidget:public AbstractSliceViewPlugin
{
    Q_OBJECT
public:

    PixelWidget(SliceType type,SliceEditorWidget*widget, QWidget * parent = nullptr);

public slots:

    void setPosition(const QPoint & p);

protected:

	void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

private:

    void changeLayout(QSize areaSize);

    void changeValue(const QImage & image,const QPoint & pos);

	void calcCount(QSize areaSize);

	void setWidget(QWidget * widget,int xpos, int ypos);

	void setWidth(int width);

	void setHeight(int height);


	static const int s_width = 50;
	static const int s_height = 20;
	static const int s_top = 0;
	static const int s_bottom = 0;
	static const int s_left = 0;
	static const int s_right = 0;

	//SliceType m_sliceType;

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

};

#endif // PIXELVIWER_H
