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
	void updateTable();
	
	static QPair<int, int> calcColumnAndRow(const QGraphicsItem * item)
	{
		auto p = qgraphicsitem_cast<StrokeMarkItem*>(item);
		if(p != nullptr)
		{
			return qMakePair(6, 2);
		}
		return qMakePair(0, 0);
	}
public:
	MarkInfoWidget(QWidget * parent = nullptr);
public slots:
	void setMark(QGraphicsItem * item);
	
};

#endif // MARKINFOWIDGET_H