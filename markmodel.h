#ifndef MARKMODEL_H
#define MARKMODEL_H

#include <QAbstractItemModel>


QT_BEGIN_NAMESPACE
class QGraphicsItem;
QT_END_NAMESPACE

class AbstractMarkItem;


class MarksModel :public QAbstractItemModel
{
	Q_OBJECT
	//QHash<QString, QList<AbstractMarkItem*>> m_items;
	
	QList<QList<AbstractMarkItem*>> m_marks;
	QList<QString> m_class;
	QHash<AbstractMarkItem *, int> m_indexHash;

	static constexpr quintptr FirstLevel = 1;
	static constexpr quintptr SecondLevel = 2;
	static constexpr quintptr ThirdLevel = 3;

	/**
	* \brief
	* \param index
	* \return return a non-null internal pointer of the index or return root pointer
	*/
public:
	explicit  MarksModel(QObject * parent = nullptr);
	~MarksModel();
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

	void addMark(const QString & category, AbstractMarkItem* mark);
	void addMarks(const QString & category, const QList<AbstractMarkItem*> & marks);
	QList<AbstractMarkItem*> marks(const QString & category);
	bool removeMark(const QString & category,AbstractMarkItem * mark);
	int removeMarks(const QString & category, const QList<AbstractMarkItem*> & marks = QList<AbstractMarkItem*>());

};




#endif // MARKMODEL_H