#ifndef MARKMODEL_H
#define MARKMODEL_H

#include <QAbstractItemModel>
#include <QSharedPointer>
#include <QColor>

QT_BEGIN_NAMESPACE
class QGraphicsItem;
QT_END_NAMESPACE
class AbstractMarkItem;
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
	CategoryItem(const QString & name,const QColor & color = Qt::black, int count = 0, bool visible = true):
	m_name(name),m_color(color),m_count(count),m_visible(visible){}
	QString name()const noexcept{ return m_name; }
	int count()const noexcept{ return m_count; }
	bool visible()const noexcept{ return m_visible; }
	QColor color()const noexcept { return m_color; }
	void setName(const QString & n)noexcept{ m_name = n; }
	void setCount(int c)noexcept { m_count = c; }
	void setVisible(bool visible)noexcept { m_visible = visible; }
	void setColor(const QColor & c)noexcept { m_color = c; }
	void increaseCount()noexcept{ m_count++; }
	void decreaseCount()noexcept{ if (m_count != 0)m_count--; }
};

Q_DECLARE_METATYPE(QSharedPointer<CategoryItem>);

class MarkModel :public QAbstractItemModel
{
	Q_OBJECT
	using MarkSliceList = QVector<QList<AbstractMarkItem*>>;
	TreeItem * m_rootItem;
	const AbstractSliceDataModel * m_dataModel;
	ImageView * m_view;

	MarkSliceList m_topSliceVisibleMarks;		//store the visible marks for every slice
	MarkSliceList m_rightSliceVisibleMarks;
	MarkSliceList m_frontSliceVisibleMarks;

	TreeItem* get_item_helper_(const QModelIndex& index) const;
	QModelIndex model_index_helper_(const QModelIndex& root, const QString& display);
	QModelIndex category_index_helper_(const QString& category);
	QModelIndex category_add_helper_(const QString& category);
	bool check_match_helper_(const AbstractSliceDataModel * dataModel);
	void add_mark_in_slice_helper_(AbstractMarkItem * mark);
	void update_mark_visible_helper(AbstractMarkItem * mark);

	const MarkSliceList & topSliceVisibleMarks()const { return m_topSliceVisibleMarks; }
	const MarkSliceList & rightSliceVisibleMarks()const { return m_rightSliceVisibleMarks; }
	const MarkSliceList & frontSliceVisibleMarks()const { return m_frontSliceVisibleMarks; }

	//This constructor is for ImageView to create the MarkModel
	MarkModel(AbstractSliceDataModel * dataModel,ImageView * view,
		MarkSliceList top = MarkSliceList(),
		MarkSliceList right = MarkSliceList(),
		MarkSliceList front = MarkSliceList(),
		QObject * parent = nullptr);

	friend class ImageView;
public:
	MarkModel() = delete;
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

	int save(const QString & fileName);
};


#endif // MARKMODEL_H