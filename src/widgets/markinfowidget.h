#ifndef MARKINFOWIDGET_H
#define MARKINFOWIDGET_H
#include <QTableWidget>
#include <QTableView>


class QGraphicsItem;

class MarkInfoWidget:public QTableView
{
	Q_OBJECT
public:
	MarkInfoWidget(QWidget * parent = nullptr);
public slots:
};

#endif // MARKINFOWIDGET_H