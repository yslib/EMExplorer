#ifndef MARKINFOWIDGET_H
#define MARKINFOWIDGET_H
#include <QTableWidget>


class QGraphicsItem;

class MarkInfoWidget:public QTableWidget
{
	Q_OBJECT
public:
	MarkInfoWidget(QWidget * parent = nullptr);
public slots:
	void setMark(QGraphicsItem * item);
};

#endif // MARKINFOWIDGET_H