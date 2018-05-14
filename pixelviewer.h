#ifndef PIXELVIWER_H
#define PIXELVIWER_H
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QImage>
#include <QPoint>
#include <QSharedPointer>
#include "ItemContext.h"

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

	//model interface
	void setModel(DataItemModel * model);
	void activateItem(const QModelIndex & index);

public slots:
	void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    void setPosition(const QPoint & p);
protected:
	void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
private:
    void changeLayout(QSize areaSize);
    void changeValue(const QImage & image,const QPoint & pos);
	void calcCount(QSize areaSize);
	void setWidget(QWidget * widget,int xpos, int ypos);
private:
	static const int s_width = 50;
	static const int s_height = 20;


	QModelIndex getDataIndex(const QModelIndex & itemIndex);
	QAbstractItemModel * m_model;
	QSharedPointer<ItemContext> m_ptr;
	QModelIndex m_activedIndex;


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
    //QGridLayout * layout;
};

#endif // PIXELVIWER_H
