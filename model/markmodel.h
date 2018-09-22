#ifndef MARKMODEL_H
#define MARKMODEL_H

#include <QAbstractItemModel>

#include "model/treeitem.h"
#include "abstract/abstractslicedatamodel.h"
#include <QGraphicsItem>

//#include <QDataStream>

QT_BEGIN_NAMESPACE
class QGraphicsItem;
QT_END_NAMESPACE
class QGraphicsItem;
class SliceEditorWidget;
class AbstractSliceDataModel;
class TreeItem;
class CategoryItem;
class Triangulate;


/*
 * Data storage specification:
 * 1) Category name is stored at 0 column
 * 2) Mark Item is stored at 0 column
 * 3) Mark Mesh is stored at 1 column of Category node 
 */

class MarkModel :public QAbstractItemModel
{

	enum {
		MeshRole = Qt::ItemDataRole::UserRole + 1
	};

	typedef QSharedPointer<QGraphicsItem> __Internal_Mark_Type_;
	typedef QWeakPointer<QGraphicsItem> __Internal_Mark_Type_Weak_Ref_;
	typedef QSharedPointer<CategoryItem> __Internal_Categroy_Type_;

	Q_OBJECT
	using MarkSliceList = QVector<QList<QGraphicsItem*>>;
	//state member
	const AbstractSliceDataModel * m_dataModel;
	const SliceEditorWidget * m_view;
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
	QModelIndex categoryAddHelper(const QString& category, const QColor& color);		//set dirty
	inline bool checkMatchHelper(const AbstractSliceDataModel * dataModel)const;
	void addMarkInSliceHelper(QGraphicsItem * mark);				//set dirty
	void removeMarkInSliceHelper(QGraphicsItem * mark);
	void updateMarkVisibleHelper(__Internal_Mark_Type_& mark);			//set dirty
	bool updateMeshMarkHelper(const QString& cate);
	void detachFromView();


	static void retrieveDataFromTreeItemHelper(const TreeItem * root, TreeItemType type,int column, QVector<QVariant> & data);


	void initSliceMarkContainerHelper();
	void createContextMenu();

	//Functions used by ImageView
	const MarkSliceList & topSliceVisibleMarks()const { return m_topSliceVisibleMarks; }
	const MarkSliceList & rightSliceVisibleMarks()const { return m_rightSliceVisibleMarks; }
	const MarkSliceList & frontSliceVisibleMarks()const { return m_frontSliceVisibleMarks; }

	MarkModel(AbstractSliceDataModel * dataModel,SliceEditorWidget * view,TreeItem * root ,QObject * parent = nullptr);
	enum {MagicNumber = 1827635234};

	friend class SliceEditorWidget;
signals:
	void modified();
	void saved();
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
	QSharedPointer<CategoryItem> categoryItem(const QString & cate)const;
	const Triangulate * markMesh(const QString & cate);

	bool removeMark(QGraphicsItem* mark);			//set dirty
	int removeMarks(const QList<QGraphicsItem*>& marks = QList<QGraphicsItem*>());		//set dirty
	inline int markCount(const QString & category)const;
	bool save(const QString & fileName,MarkFormat format = MarkFormat::Binary);
	inline void setDirty();
	inline bool dirty()const;
	inline void resetDirty();

	friend class MarkManagerWidget;
};


//inline member functions definations
/**
 * \brief:
 * \Note: The operation will take the ownership of the pointer. If the model is destructed, 
 * \Note: the pointer will also be deleted.
 */
inline void MarkModel::addMark(const QString& category, QGraphicsItem* mark){addMarks(category, QList<QGraphicsItem*>{mark});}
inline int MarkModel::markCount(const QString & category)const{return rowCount(categoryIndexHelper(category));}
inline void MarkModel::setDirty() { m_dirty = true; emit modified(); }
inline bool MarkModel::dirty()const{return m_dirty;}
inline void MarkModel::resetDirty() { m_dirty = false; emit saved(); }
inline bool MarkModel::checkMatchHelper(const AbstractSliceDataModel* dataModel) const{return m_identity == SliceDataIdentityTester::createTester(dataModel);}


#endif // MARKMODEL_H
