//Qt headers
#include <QHeaderView>
#include <QDebug>
//custom headers
#include "markinfowidget.h"
#include "globals.h"
#include "model/markitem.h"

//#include "markitem.h"

namespace
{
	QString propertyToString(const QVariant & var,MarkProperty::Property type)
	{
		switch(type)
		{
		case MarkProperty::Color:
		case MarkProperty::CategoryColor:
		{
			if (var.canConvert<QColor>() == false)
				return QString();
			QColor c = var.value<QColor>();
			QString str = QStringLiteral("(%1,%2,%3)").arg(c.red()).arg(c.green()).arg(c.blue());
			return str;
		}
		case MarkProperty::Name:
		case MarkProperty::CategoryName:
			return var.toString();
		case MarkProperty::SliceIndex:
			return QString::number(var.toInt());
		case MarkProperty::SliceType:
			switch(static_cast<SliceType>(var.toInt()))
			{
			case SliceType::Top:
				return QStringLiteral("Top");
			case SliceType::Right:
				return QStringLiteral("Right");
			case SliceType::Front:
				return QStringLiteral("Front");
			default:
				return QStringLiteral("");
			}
		case MarkProperty::VisibleState:
			return var.toBool() ? QStringLiteral("True") : QStringLiteral("False");
		case MarkProperty::Length:
			return QString::number(var.toDouble());
		default:
			return QString();
			break;
		}

	}


	QTableWidgetItem * tableViewItem(const QVariant & var, MarkProperty::Property type)
	{
		QTableWidgetItem * item = nullptr;
		item = new QTableWidgetItem(propertyToString(var, type));
		switch(type)
		{
		case MarkProperty::CategoryColor:
		case MarkProperty::Color:
			item->setData(Qt::BackgroundColorRole, var);
			return item;
		case MarkProperty::Name:
		case MarkProperty::CategoryName:
		case MarkProperty::SliceIndex:
		case MarkProperty::SliceType:	
		case MarkProperty::VisibleState:
		case MarkProperty::Length:
			break;
		}
		return item;
	}
}


MarkInfoWidget::MarkInfoWidget(QWidget * parent):QTableWidget(parent)
{
	horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	setShowGrid(false);
}

void MarkInfoWidget::setMark(QGraphicsItem* item)
{
	if (item == nullptr)
		return;
	auto var = item->data(MarkProperty::PropertyInfo);
	bool flag = var.canConvert<MarkPropertyInfo>();
	if (!flag)
		return;
	auto propertyInfo = item->data(MarkProperty::PropertyInfo).value<MarkPropertyInfo>();

	clearContents();
	setColumnCount(2);
	setRowCount(propertyInfo.count());

	for(int i=0;i<propertyInfo.size();i++)
	{
		const auto & p = propertyInfo[i];
		setItem(i, 0, new QTableWidgetItem(p.second));
		setItem(i, 1, tableViewItem(item->data(p.first),p.first));
	}
}
