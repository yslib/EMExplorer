#ifndef MARKINFOWIDGET_H
#define MARKINFOWIDGET_H
#include <QWidget>
#include <qgraphicsitem.h>
#include "markitem.h"

class QGraphicsItem;
class QGridLayout;
class QTableWidget;

class MarkInfoWidget:public QWidget
{
	Q_OBJECT
	QGridLayout *m_layout;
	QTableWidget *m_table;
public:
	MarkInfoWidget(QWidget * parent = nullptr);
public slots:
	void setMark(QGraphicsItem * item);

};

#endif // MARKINFOWIDGET_H