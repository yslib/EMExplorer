#include "mrcfileinfoviewer.h"
#include <QDebug>
MRCFileInfoViewer::MRCFileInfoViewer(QWidget *parent)noexcept:QWidget(parent){
   m_layout = new QGridLayout(this);
   m_label = new QLabel(this);
   m_filesComboBox = new QComboBox(this);
   m_filesInfoTextEdit = new QTextEdit(this);

   m_filesInfoTextEdit->setReadOnly(true);

   m_label->setText(tr("MRCFiles:"));


   m_layout->addWidget(m_label,0,0);
   m_layout->addWidget(m_filesComboBox,0,1);
   m_layout->addWidget(m_filesInfoTextEdit,1,0,1,2);


   createConnections();
}
void MRCFileInfoViewer::addFileName(const QString &fileName)
{
    m_filesComboBox->addItem(fileName);
}

void MRCFileInfoViewer::addItem(const QString &fileName, const QVariant &userData)
{
   m_filesComboBox->addItem(fileName,userData);
}

void MRCFileInfoViewer::setText(const QString &info)
{
   m_filesInfoTextEdit->setText(info);
}

void MRCFileInfoViewer::createConnections()
{
    connect(m_filesComboBox,SIGNAL(currentIndexChanged(int)),this,SIGNAL(currentIndexChanged(int)));
    connect(m_filesComboBox,SIGNAL(activated(int)),this,SIGNAL(activated(int)));
}

void MRCFileInfoViewer::setCurrentIndex(int index)
{
    m_filesComboBox->setCurrentIndex(index);
}
int MRCFileInfoViewer::count()const{
    return m_filesComboBox->count();
}
QVariant MRCFileInfoViewer::itemData(int index,int role) const{
    return m_filesComboBox->itemData(index,role);
}




//InformationModel Defination

InformationModel::InformationModel(const QString & data, QObject * parent):QAbstractItemModel(parent)
{
	///TODO:: construct a new root
	QVector<QVariant> headers;
	headers << "File" << "Info";

	m_rootItem = new TreeItem(headers);

}

InformationModel::~InformationModel()
{
	delete m_rootItem;
}

QVariant InformationModel::data(const QModelIndex & index, int role) const
{
	if(role != Qt::DisplayRole)
		return QVariant();
	if (index.isValid() == false)
		return QVariant();
	TreeItem * item = static_cast<TreeItem*>(index.internalPointer());
	qDebug() << item->data(index.column());
	return item->data(index.column());
}

Qt::ItemFlags InformationModel::flags(const QModelIndex & index) const
{
	if (index.isValid() == false)
		return 0;
	return QAbstractItemModel::flags(index);
}

bool InformationModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if (role != Qt::EditRole)return false;
	
	TreeItem * item = getItem(index);

	bool ok = item->setData(index.column(), value);
	if (ok == true)
		emit dataChanged(index, index);
	return ok;
}

bool InformationModel::insertColumns(int column, int count, const QModelIndex & parent)
{
	beginInsertColumns(parent, column, column + count - 1);
	//insert same columns at same position from the top of the tree to down recursively
	bool success = m_rootItem->insertColumns(column, count);
	endInsertColumns();
	return success;
}

bool InformationModel::removeColumns(int column, int count, const QModelIndex & parent)
{
	beginRemoveColumns(parent, column, column + count - 1);
	bool success = m_rootItem->removeColumns(column, count);
	endRemoveColumns();

	if (m_rootItem->columnCount() == 0)
		removeRows(0, rowCount());
	return success;
}

bool InformationModel::insertRows(int row, int count, const QModelIndex & parent)
{
	TreeItem * item = getItem(parent);

	beginInsertRows(parent, row, count + row - 1);
	bool success = item->insertChildren(row, count, columnCount());
	endInsertRows();

	return success;
}

bool InformationModel::removeRows(int row, int count, const QModelIndex & parent)
{
	TreeItem * item = getItem(parent);
	bool success = true;

	beginRemoveRows(parent, row, row + count - 1);
	success = item->removeChildren(row, count);
	endRemoveRows();
	return success;
}

QVariant InformationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return m_rootItem->data(section);
	return QVariant();
}

QModelIndex InformationModel::index(int row, int column, const QModelIndex & parent) const
{

	//Check if the index is valid
	//if(hasIndex(row,column,parent) == false)
	//	return QModelIndex();

	//If the index points to a non-root node and its column is not 0
	if (parent.isValid() == true && parent.column() != 0)
		return QModelIndex();

	TreeItem * parentItem = getItem(parent);

	TreeItem * childItem = parentItem->child(row);
	if (childItem == nullptr)
		return QModelIndex();
	else
		return createIndex(row, column, childItem);
}

QModelIndex InformationModel::parent(const QModelIndex & index) const
{
	//Index points to a root item
	if (index.isValid() == false)return QModelIndex();			

	TreeItem * item = getItem(index);
	TreeItem * parentItem = item->parentItem();

	//If index points to a child item of root item
	if (parentItem == m_rootItem)		
		return QModelIndex();				

	return createIndex(parentItem->row(), 0, parentItem);
}

int InformationModel::rowCount(const QModelIndex & parent) const
{
	//Only a item with 0 column number has children
	if (parent.column() > 0)
		return 0;
	TreeItem * item = getItem(parent);
	return item->childCount();
}

int InformationModel::columnCount(const QModelIndex & parent) const
{
	if (parent.isValid() == false)
		return m_rootItem->columnCount();
	return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
}

void InformationModel::addNewFileInfo(const QString & fileName, const QString & info)
{
	{
		bool success;
		success = insertRows(rowCount(), 1);		//add a new file to as the last child of the parent
		if (success == false)
			return;
	}

	QModelIndex newFileNode = index(rowCount() - 1, 0);
	setData(newFileNode, fileName);
	QStringList lines = info.split('\n', QString::SkipEmptyParts);
	for(auto & it:lines)
	{
		auto line = it.split(':', QString::SkipEmptyParts);
		{
			bool success;
			success = insertRows(rowCount(newFileNode),1, newFileNode);
			int lastRow = rowCount(newFileNode)-1;
			QModelIndex valueIndex = index(lastRow,0, newFileNode);
			QModelIndex descIndex = index(lastRow,1, newFileNode);
			if (success == true) {
				setData(valueIndex, line.value(1));
				setData(descIndex, line.value(0));
			}
		}
	}
}




