#ifndef MRCFILEINFO_H
#define MRCFILEINFO_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <QLayout>
#include <QTreeView>


class MRCFileInfoViewer : public QWidget
{
    Q_OBJECT
public:
    explicit MRCFileInfoViewer(QWidget *parent = nullptr)noexcept;
   void addFileName(const QString & fileName);
   void addItem(const QString & fileName,const QVariant & userData = QVariant());
   void setText(const QString & info);
   void getText()const;
   int count()const;
   QVariant itemData(int index, int role = Qt::UserRole)const;

private:
    QGridLayout * m_layout;
    QLabel * m_label;

    QComboBox * m_filesComboBox;
    QTextEdit * m_filesInfoTextEdit;
private:
    void createConnections();
signals:
    void currentIndexChanged(int index);
    void activated(int index);
public slots:
   void setCurrentIndex(int index);
};




class TreeItem
{
	TreeItem * m_parent;
	QVector<TreeItem*> m_children;
	QVector<QVariant> m_data;
public:
	explicit TreeItem(const QVector<QVariant> & data,TreeItem * parent = nullptr):m_data(data),m_parent(parent){}
	~TreeItem() { qDeleteAll(m_children); }

	
	void appendChild(TreeItem * child) { m_children.append(child); }
	void setParentItem(TreeItem * parent) { m_parent = parent; }
	TreeItem* parentItem()const { return m_parent; };
	TreeItem* child(int row)const { return m_children.value(row); }
	/**
	 * \brief This is convinence for Model to create QModelIndex in Model::parent() method
	 * \return return the index of the child in its parent's list of children/
	 */
	int row() const {
		if (m_parent != nullptr)
			return m_parent->m_children.indexOf(const_cast<TreeItem*>(this));
		return 0;
	}
	int childCount()const { return m_children.size(); }
	int columnCount()const{return m_data.size();}
	QVariant data(int column = 0)const{return m_data.value(column);}

	/**
	 *All above methods are necessary for a read-only TreeView.
	 *Following methods are requried for a editable TreeView.
	 *
	 */

	bool insertChildren(int position, int count, int columns)
	{
		///TODO:: Is this check necessary? 
		if (position < 0 || position > m_children.size())return false;
		for(int row = 0 ;row<count;row++)
		{
			QVector<QVariant> data(columns);
			TreeItem * item = new TreeItem(data, this);
			m_children.insert(position, item);
		}
		return true;
	}
	bool insertColumns(int position, int columns)
	{
		if (position<0 || position > m_data.size())return false;
		//insert corresponding columns form current node.
		for (int i = 0; i < columns; i++)
			m_data.insert(position, QVariant());

		//and insert same columns at same position of its all children recursively.
		for (auto & child : m_children)
			child->insertColumns(position, columns);
		return true;
	}
	bool removeChildren(int position, int count)
	{
		if (position < 0 || position >= m_children.size())
			return false;
		for(int i=0;i<count;i++)
			delete m_children.takeAt(position);
		return true;
	}
	bool removeColumns(int position, int columns)
	{
		if (position<0 || position > m_data.size())return false;
		//remove corresponding columns from current node.
		for (int i = 0; i < columns; i++)
			m_data.remove(position);
		//and remove same columns at same position of its all children recursively. 
		for(auto & child:m_children)
			child->removeColumns(position, columns);
		return true;
	}
	/**
	 * \brief To make implementation of the model easier, we return true 
	 * \brief to indicate whether the data was set successfully, or false if an invalid column
	 * \param column 
	 * \param value 
	 * \return 
	 */
	bool setData(int column, const QVariant & value)
	{
		if (column < 0 || column >= m_data.size())return false;
		m_data[column] = value;
		return true;
	}


};

class InformationModel:public QAbstractItemModel
{
	Q_OBJECT
	TreeItem * m_rootItem;


	/**
	 * \brief 
	 * \param index 
	 * \return return a non-null internal pointer of the index or return root pointer
	 */
	TreeItem * getItem(const QModelIndex & index)const
	{
		if(index.isValid())
		{
			TreeItem * item = static_cast<TreeItem*>(index.internalPointer());
			if (item)return item;
		}
		return m_rootItem;
	}

public:
	explicit InformationModel(const QString & data, QObject * parent = nullptr);
	~InformationModel();

	QVariant data(const QModelIndex & index, int role)const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole)const override;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex())const override;
	QModelIndex parent(const QModelIndex&index)const override;
	int rowCount(const QModelIndex & parent = QModelIndex())const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;

	/*
	 *Read-only tree models only need to provide the above functions.
	 *The following functions provide support for editing and resizing.
	 */

	Qt::ItemFlags flags(const QModelIndex & index)const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

	bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;
	bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;
	bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

	//test file
	void addNewFileInfo(const QString & fileName,const QString & info);
};
#endif // MRCFILEINFO_H
