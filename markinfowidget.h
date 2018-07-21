#ifndef MARKINFOWIDGET_H
#define MARKINFOWIDGET_H
#include <QTableWidget>


class QGraphicsItem;

class MarkInfoWidget:public QTableWidget
{
	Q_OBJECT
public:
	MarkInfoWidget(QWidget * parent = nullptr);
	QSize sizeHint() const Q_DECL_OVERRIDE { return QSize(250,250); }
	QSize minimumSizeHint() const Q_DECL_OVERRIDE { return QSize(250,250); }
public slots:
	void setMark(QGraphicsItem * item);
};

#endif // MARKINFOWIDGET_H