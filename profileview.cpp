#include "profileview.h"
#include <QGridLayout>
#include <QListWidget>
#include <QTableView>
#include <QGroupBox>
#include <QDebug>


ProfileView::ProfileView(QWidget* parent): QWidget(parent)
{
	QGridLayout *layout = new QGridLayout;
	m_listWidget = new QListWidget;
	m_tableView = new QTableView;

	QGroupBox * listGroup = new QGroupBox;
	listGroup->setTitle(QStringLiteral("Explorer"));
	QGridLayout * listLayout = new QGridLayout;
	listLayout->addWidget(m_listWidget, 0, 0);
	listGroup->setLayout(listLayout);

	QGroupBox * tableGroup = new QGroupBox;
	tableGroup->setTitle(QStringLiteral("Details"));
	QGridLayout * tableLayout = new QGridLayout;
	tableLayout->addWidget(m_tableView, 0, 0);
	tableGroup->setLayout(tableLayout);

	layout->addWidget(listGroup, 0,0);
	layout->addWidget(tableGroup, 1, 0);
	layout->setRowStretch(0, 3);
	layout->setRowStretch(1, 7);

	//signals
	//connect(m_listWidget, &QListWidget::currentItemChanged, [](QListWidgetItem * current,QListWidgetItem * previous)
	//{
	//	
	//});

	connect(m_listWidget, &QListWidget::itemDoubleClicked, [](QListWidgetItem * item)
	{

	});
	setLayout(layout);
}

void ProfileView::addModel(const QString & text, QAbstractTableModel * model)
{
	m_hash[text] = model;
	m_listWidget->addItem(text);
    m_tableView->setModel(model);
}

MRCInfoTableModel::MRCInfoTableModel(int row,int column,QObject* parent):
QAbstractTableModel(parent),
m_rowCount(row),
m_columnCount(column)
{
	m_data.resize(row);
	for (int i = 0; i < row; i++)
		m_data[i].resize(column);
}

int MRCInfoTableModel::rowCount(const QModelIndex& parent) const
{
	return m_rowCount;
}

int MRCInfoTableModel::columnCount(const QModelIndex& parent) const
{
	return m_columnCount;
}

QVariant MRCInfoTableModel::data(const QModelIndex& index, int role) const
{
	if (index.isValid() == false)
		return QVariant();
	if(role == Qt::DisplayRole)
	{
		return m_data[index.row()][index.column()];
	}
	return QVariant();
}

bool MRCInfoTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (role != Qt::DisplayRole)
		return true;

	int row = index.row();
	int column = index.column();
	if (row < 0 || row >= m_rowCount || column < 0 || column >= m_columnCount)
		return false;
	m_data[row][column] = value;
    return true;
}

QVariant MRCInfoTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Horizontal){
        if(section == 0){
            return QVariant::fromValue(QString("Property"));
        }else if(section == 1){
            return QVariant::fromValue(QString("Value"));
        }

    }else if(orientation == Qt::Vertical){
        return QVariant::fromValue(QString::number(section));
    }
	return QVariant();
}
