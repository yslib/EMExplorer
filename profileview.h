#ifndef PROFILEVIEW_H
#define PROFILEVIEW_H

#include <QWidget>
#include <QAbstractTableModel>


class QTableView;
class QComboBox;

class ProfileView:public QWidget
{
	Q_OBJECT
public:
    ProfileView(QWidget * parent = nullptr):QWidget(parent){}
	QTableView * view()const{}

private:
	QTableView * m_tableView;
};

class TableModel :public QAbstractTableModel
{
	Q_OBJECT
public:
	TableModel(QObject * parent = nullptr);
	int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
	int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole)const Q_DECL_OVERRIDE;
private:
	QVector<QVector<QVariant>> m_data;
};

#endif // PROFILEVIEW_H