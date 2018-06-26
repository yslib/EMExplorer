#ifndef MARKMODEL_H
#define MARKMODEL_H

#include <QAbstractItemModel>
#include <QSharedPointer>
#include <QColor>

#include "treeitem.h"
#include "abstractslicedatamodel.h"

//#include <QDataStream>

QT_BEGIN_NAMESPACE
class QGraphicsItem;
QT_END_NAMESPACE
class QGraphicsItem;
class ImageView;
class AbstractSliceDataModel;
class TreeItem;

class CategoryItem
{
	QString m_name;
	int m_count;
	bool m_visible;
	QColor m_color;
public:
	CategoryItem(const QString & name = QString(),const QColor & color = Qt::black, int count = 0, bool visible = true):
	m_name(name),m_color(color),m_count(count),m_visible(visible){}
	inline QString name()const noexcept;
	inline int count()const noexcept;
	inline bool visible()const noexcept{ return m_visible; }
	inline QColor color()const noexcept { return m_color; }
	inline void setName(const QString & n)noexcept{ m_name = n; }
	inline void setCount(int c)noexcept { m_count = c; }
	inline void setVisible(bool visible)noexcept { m_visible = visible; }
	inline void setColor(const QColor & c)noexcept { m_color = c; }
	inline void increaseCount()noexcept{ m_count++; }
	inline void decreaseCount()noexcept{ if (m_count != 0)m_count--; }

	friend QDataStream & operator<< (QDataStream & stream, const CategoryItem & item);
	friend QDataStream & operator>>(QDataStream & stream, CategoryItem & item);
	friend QDataStream & operator<< (QDataStream & stream, const QSharedPointer<CategoryItem> & item);
	friend QDataStream & operator>>(QDataStream & stream, QSharedPointer<CategoryItem>& item);
};

inline QString CategoryItem::name()const noexcept {return m_name;}
inline int CategoryItem::count()const noexcept { return m_count; }

Q_DECLARE_METATYPE(CategoryItem);
Q_DECLARE_METATYPE(QSharedPointer<CategoryItem>);

class MarkModel :public QAbstractItemModel
{
	Q_OBJECT
	using MarkSliceList = QVector<QList<QGraphicsItem*>>;
	//state member
	const AbstractSliceDataModel * m_dataModel;
	const ImageView * m_view;
	bool m_dirty;

	//a copy from TreeItem
	MarkSliceList m_topSliceVisibleMarks;		//store the visible marks for every slice
	MarkSliceList m_rightSliceVisibleMarks;
	MarkSliceList m_frontSliceVisibleMarks;

	//Need te be serialized
	SliceDataIdentityTester m_identity;
	TreeItem * m_rootItem;

	//Helper functions
	TreeItem* getItemHelper(const QModelIndex& index) const;
	QModelIndex modelIndexHelper(const QModelIndex& root, const QString& display)const;
	QModelIndex categoryIndexHelper(const QString& category)const;
	QModelIndex categoryAddHelper(const QString& category);		//set dirty
	inline bool checkMatchHelper(const AbstractSliceDataModel * dataModel)const;
	void addMarkInSliceHelper(QGraphicsItem * mark);				//set dirty
	void removeMarkInSliceHelper(QGraphicsItem * mark);
	void updateMarkVisibleHelper(QGraphicsItem * mark);			//set dirty

	static void retrieveDataFromTreeItemHelper(const TreeItem * root, TreeItemType type,int column, QVector<QVariant> & data);
	void initSliceMarkContainerHelper();

	//Functions used by ImageView
	const MarkSliceList & topSliceVisibleMarks()const { return m_topSliceVisibleMarks; }
	const MarkSliceList & rightSliceVisibleMarks()const { return m_rightSliceVisibleMarks; }
	const MarkSliceList & frontSliceVisibleMarks()const { return m_frontSliceVisibleMarks; }
	MarkModel(AbstractSliceDataModel * dataModel,ImageView * view,TreeItem * root ,QObject * parent = nullptr);
	enum {MagicNumber = 1827635234};

	friend class ImageView;
public:
	enum MarkFormat
	{
		Binary,
		Raw
	};
	MarkModel() = delete;
	MarkModel(const QString & fileName);
	virtual ~MarkModel();

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

	//Custom functions for accessing and setting data
	inline void addMark(const QString & category, QGraphicsItem * mark);		//set dirty
	void addMarks(const QString & category, const QList<QGraphicsItem*> & marks);			//set dirty
	QList<QGraphicsItem*> marks(const QString & category)const;
	QList<QGraphicsItem*> marks()const;			//This is time-consuming operation
	QStringList categoryText()const;
	QList<QSharedPointer<CategoryItem>> categoryItems()const;

	bool removeMark(QGraphicsItem* mark);			//set dirty
	int removeMarks(const QList<QGraphicsItem*>& marks = QList<QGraphicsItem*>());		//set dirty
	inline int markCount(const QString & category)const;

	bool save(const QString & fileName,MarkFormat format = MarkFormat::Binary);

	inline void setDirty();
	inline bool dirty()const;
	inline void resetDirty();
};


//inline member functions definations
inline void MarkModel::addMark(const QString& category, QGraphicsItem* mark)
{
	addMarks(category, QList<QGraphicsItem*>{mark});
}
inline int MarkModel::markCount(const QString & category)const
{
	return rowCount(categoryIndexHelper(category));
}

inline void MarkModel::setDirty()
{
	m_dirty = true;
}
inline bool MarkModel::dirty()const
{
	return m_dirty;
}
inline void MarkModel::resetDirty()
{
	m_dirty = false;
}

inline bool MarkModel::checkMatchHelper(const AbstractSliceDataModel* dataModel) const
{
	return m_identity == SliceDataIdentityTester::createTester(dataModel);
}



#endif // MARKMODEL_H