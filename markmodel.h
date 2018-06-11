#ifndef MARKMODEL_H
#define MARKMODEL_H

#include <QAbstractItemModel>
#include "ItemContext.h"


QT_BEGIN_NAMESPACE
class QGraphicsItem;
QT_END_NAMESPACE

class AbstractMarkItem;


class MarkModel :public QAbstractItemModel
{
	Q_OBJECT
	TreeItem * m_rootItem;

	QList<QList<AbstractMarkItem*>> m_marks;

	/**
	* \brief
	* \param index
	* \return return a non-null internal pointer of the index or return root pointer
	*/
	TreeItem* get_item_helper_(const QModelIndex& index) const;
	QModelIndex model_index_helper_(const QModelIndex & root, const QString & display)
	{
		int c = rowCount(root);
		for (int i = 0; i<c; i++)
		{
			QModelIndex id = index(i, 0, root);
			auto item = static_cast<TreeItem*>(id.internalPointer());
			auto d = item->data(0);
			QString value;
			switch (item->type())
			{
			case TreeItemType::Category:
				value = d.value<QString>();
				break;
			case TreeItemType::Mark:
				value = d.value<AbstractMarkItem*>()->name();
				break;
			case TreeItemType::Root:
			default:
				break;
			}
			if (value == display)
				return id;
			else
				model_index_helper_(id, display);
		}
	}

	QModelIndex category_index_helper_(const QString & category)
	{
		int c = rowCount();
		for (int i = 0; i<c; i++)
		{
			auto id = index(i, 0);
			auto item = static_cast<TreeItem*>(id.internalPointer());
			auto d = item->data(0).value<QString>();
			if (d == category)
			{
				return id;
			}
		}
		return QModelIndex();
	}
	QModelIndex category_add_helper_(const QString & category)
	{
		int c = rowCount();
		beginInsertRows(QModelIndex(), c, c);
		QVector<QVariant> d{ category };
		auto p = new TreeItem(d, TreeItemType::Category, m_rootItem);
		//qDebug() << p;
		m_rootItem->appendChild(p);
		endInsertRows();
		return createIndex(c, 0, p);
	}
public:
	explicit MarkModel(QObject * parent = nullptr);
	~MarkModel();

	QVariant data(const QModelIndex & index, int role = Qt::EditRole)const Q_DECL_OVERRIDE;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole)const Q_DECL_OVERRIDE;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex())const Q_DECL_OVERRIDE;
	QModelIndex parent(const QModelIndex&index)const Q_DECL_OVERRIDE;
	int rowCount(const QModelIndex & parent = QModelIndex())const Q_DECL_OVERRIDE;
	int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
	/*
	*Read-only tree models only need to provide the above functions.
	*The following functions provide support for editing and resizing.
	*/
	Qt::ItemFlags flags(const QModelIndex & index)const Q_DECL_OVERRIDE;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
	bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;
	bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;
	bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;

	//test file
	//Custom functions for accessing and setting data
	void addMark(const QString & category, AbstractMarkItem * mark);
	void addMarks(const QString & category, const QList<AbstractMarkItem*> & marks);
	QList<AbstractMarkItem*> marks(const QString & category);
	bool removeMark(const QString& category, AbstractMarkItem* mark);
	int removeMarks(const QString& category, const QList<AbstractMarkItem*>& marks = QList<AbstractMarkItem*>());
	int markCount(const QString & category);
};


#endif // MARKMODEL_H