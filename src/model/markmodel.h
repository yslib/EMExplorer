

#ifndef MARKMODEL_H
#define MARKMODEL_H


#include "model/treeitem.h"
#include "model/instanceitem.h"

#include "abstract/abstractslicedatamodel.h"


//#include "treeitem.h"

//#include <QDataStream>


class TreeItem;
struct CategoryInfo;
QT_BEGIN_NAMESPACE
class QGraphicsItem;
QT_END_NAMESPACE
class QGraphicsItem;
class SliceEditorWidget;
class AbstractSliceDataModel;
class RootTreeItem;
class CategoryItem;

class Triangulate;

class StrokeMarkItem;

class QItemSelectionModel;

enum TreeItemType;


/**
 * \class MarkModel markmodel.h "model/markmodel.h"
 * \brief This class is used to represent the mark model created by marking
 * 
 * 
 * \internal
 *  Data storage specification:
 *  Category name is stored at 0 column
 *  Mark Item is stored at 0 column
 *  Mark Mesh is stored at 1 column of Category node 
 * 
 */
class MarkModel :public QAbstractItemModel
{
	Q_OBJECT

	/**
	 * \brief 
	 */
	enum MarkModelItemRole
	{
		MeshRole = Qt::ItemDataRole::UserRole + 1,
		MetaDataRole = Qt::ItemDataRole::UserRole + 2,
		TreeItemRole = Qt::ItemDataRole::UserRole + 3
	};

	//typedef QSharedPointer<QGraphicsItem> __Internal_Mark_Type_;
	//typedef QWeakPointer<QGraphicsItem> __Internal_Mark_Type_Weak_Ref_;
	//typedef QSharedPointer<CategoryItem> __Internal_Categroy_Type_;
	
	using MarkSliceList = QVector<QList<StrokeMarkItem*>>;
	//state member
	const AbstractSliceDataModel * m_dataModel;
	const SliceEditorWidget * m_view;

	QItemSelectionModel * const m_selectionModel;

	bool m_dirty;

	//a copy from TreeItem
	MarkSliceList m_topSliceVisibleMarks;		//store the visible marks for every slice
	MarkSliceList m_rightSliceVisibleMarks;
	MarkSliceList m_frontSliceVisibleMarks;

	//Need te be serialized
	SliceDataIdentityTester m_identity;
	RootTreeItem * m_rootItem;

	//Helper functions
	TreeItem* _hlp_internalPointer(const QModelIndex& index) const;
	//QModelIndex modelIndexHelper(const QModelIndex& root, const QString& display)const;
	QModelIndex _hlp_categoryIndex(const QString& category)const;
	QModelIndex _hlp_categoryAdd(const QString& category, const QColor& color);		//set dirty
	QModelIndex _hlp_categoryAdd(const CategoryInfo & info);							//set setdirty

	//void _hlp_index(const QModelIndex & parent,int type,QList<QModelIndex> * indices);

	QModelIndex _hlp_instanceFind(const QString & category,const StrokeMarkItem * item);
	QModelIndex _hlp_instanceAdd(const QString & category, const StrokeMarkItem* mark);

	inline bool checkMatchHelper(const AbstractSliceDataModel * dataModel)const;
	void addMarkInSliceHelper(StrokeMarkItem * mark);									//set dirty
	void removeMarkInSliceHelper(StrokeMarkItem* mark);
	void updateMarkVisibleHelper(StrokeMarkItem * mark);							//set dirty
	bool updateMeshMarkHelper(const QString& cate);
	void detachFromView();

	static void retrieveDataFromTreeItemHelper(TreeItem* root, TreeItemType type, int column, QVector<QVariant> & data, int role);
	static void _hlp_retrieveTreeItem(TreeItem * parent, TreeItemType type, QList<TreeItem*>* items);
	QModelIndex _hlp_indexByItem(TreeItem* parent, TreeItem * item);

	void initSliceMarkContainerHelper();
	static QVector<QList<StrokeMarkItem*>> refactorMarks(QList<StrokeMarkItem*> &marks);

	//Functions used by ImageView

	const MarkSliceList & topSliceVisibleMarks()const { return m_topSliceVisibleMarks; }
	const MarkSliceList & rightSliceVisibleMarks()const { return m_rightSliceVisibleMarks; }
	const MarkSliceList & frontSliceVisibleMarks()const { return m_frontSliceVisibleMarks; }

	MarkModel(AbstractSliceDataModel * dataModel,
		SliceEditorWidget * view, 
		QObject * parent = nullptr);

	enum {MagicNumber = 1823615231};

	friend class SliceEditorWidget;
signals:

	/**
	 * \brief This is a signal
	 *  
	 * This signal will be emitted when marks in the model are modified.
	 */
	void modified();

	/**
	 * \brief This is a signal
	 * 
	 *  This signal will be emitted when the mark model was saved.
	 */
	void saved();

public:

	/**
	 * \brief Mark format enum for saving or opening 
	 */
	enum MarkFormat
	{
		Binary,		///< Save mark as internal binary format
		Raw			///< Save mark as raw format which is easily accessed by any other raw format reader
	};

	MarkModel() = delete;
	MarkModel(const QString & fileName);


	//bool eventFilter(QObject* watched, QEvent* event) override;
	QVariant data(const QModelIndex & index, int role = Qt::EditRole)const Q_DECL_OVERRIDE;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole)const Q_DECL_OVERRIDE;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex())const Q_DECL_OVERRIDE;
	QModelIndex parent(const QModelIndex&index)const Q_DECL_OVERRIDE;
	int rowCount(const QModelIndex & parent = QModelIndex())const Q_DECL_OVERRIDE;
	int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

	// Read-only tree models only need to provide the above functions.
	// The following functions provide support for editing and resizing.

	Qt::ItemFlags flags(const QModelIndex & index)const Q_DECL_OVERRIDE;

	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
	bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;
	bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;

	bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;

	//Custom functions for accessing and setting data

	bool addMark(const QString& text, StrokeMarkItem* mark);
	bool addMarks(const QString& text, const QList<StrokeMarkItem*>& marks);
	bool addCategory(const CategoryInfo& info);


	QList<TreeItem*> treeItems(const QModelIndex & parent, int type);
	QModelIndex indexByItem(TreeItem * item);

	bool insertTreeItem(TreeItem* item, const QModelIndex & parent);
	bool insertTreeItems(const QList<TreeItem*>& items, const QModelIndex & parent);
	bool removeTreeItem(TreeItem* item);
	bool removeTreeItems(const QList<TreeItem*> & items);

	//bool save(const QString & fileName,const QModelIndex & parent);
	
	QItemSelectionModel * selectionModelOfThisModel()const {return m_selectionModel;};

	QList<StrokeMarkItem*> marks(const QString& text) const;
	QList<QGraphicsItem*> marks()const;													//This is time-consuming operation
	QStringList categoryText()const;
	QList<QModelIndex> categoryModelIndices()const;
	QVector<QSharedPointer<Triangulate>> markMesh(const QString& cate);
	bool removeMark(StrokeMarkItem* mark);
	int removeMarks(const QList<StrokeMarkItem*>& marks = QList<StrokeMarkItem*>());
	inline int markCount(const QString & category)const;

	bool save(const QString & fileName,MarkFormat format = MarkFormat::Binary);
	inline void setDirty();
	inline bool dirty()const;
	inline void resetDirty();
	virtual ~MarkModel();

	//friend class MarkTreeView;
};





/**
 * \brief 
 * \param category 
 * \return 
 */
inline int MarkModel::markCount(const QString & category)const{return rowCount(_hlp_categoryIndex(category));}

/**
 * \brief Sets dirty
 * 
 * This function will emit modified() signal
 */
inline void MarkModel::setDirty() { m_dirty = true; emit modified(); }

/**
 * \brief 
 * \return 
 */
inline bool MarkModel::dirty()const{return m_dirty;}

/**
 * \brief 
 * 
 * This function will emit saved() signal
 */
inline void MarkModel::resetDirty() { m_dirty = false; emit saved(); }

/**
 * \brief 
 * \param dataModel 
 * \return 
 */
inline bool MarkModel::checkMatchHelper(const AbstractSliceDataModel* dataModel) const{return m_identity == SliceDataIdentityTester::createTester(dataModel);}

#endif // MARKMODEL_H

