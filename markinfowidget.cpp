#include "markinfowidget.h"
#include <QGridLayout>
#include <QTableWidget>



void MarkInfoWidget::updateTable()
{

}

MarkInfoWidget::MarkInfoWidget(QWidget * parent):QWidget(parent)
{
	m_layout = new QGridLayout(this);
	m_table = new QTableWidget(this);
}

void MarkInfoWidget::setMark(QGraphicsItem* item)
{
}
