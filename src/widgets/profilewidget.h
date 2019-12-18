#ifndef PROFILEVIEW_H
#define PROFILEVIEW_H

#include <QTableView>
#include <QAbstractTableModel>

//#include <QHash>

class MRC;
class QTableView;
class QComboBox;
class QListWidget;
class QGridLayout;


class ProfileWidget:public QTableView
{
	Q_OBJECT
public:
	ProfileWidget(QWidget* parent = nullptr);
	QAbstractItemModel* takeModel(QAbstractItemModel* model);

signals:
	void itemSelected(const QString & text);
	void itemChanged(const QString & text);
private:
	//QTableView * m_tableView;
};

class MRCInfoTableModel :public QAbstractTableModel
{
	Q_OBJECT
public:
	MRCInfoTableModel(int row,int column,QObject * parent = nullptr);
	int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
	int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole)const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex& index, const QVariant& value, int role) Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
private:
	QVector<QVector<QVariant>> m_data;
	int m_rowCount;
	int m_columnCount;
};

#endif // PROFILEVIEW_H
