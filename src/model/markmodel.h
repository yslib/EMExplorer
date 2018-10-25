

#ifndef MARKMODEL_H
#define MARKMODEL_H

#include <QAbstractItemModel>

#include "model/treeitem.h"
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

		enum
	{
		MeshRole = Qt::ItemDataRole::UserRole + 1,
		MetaDataRole = Qt::ItemDataRole::UserRole + 2,
	};

	//typedef QSharedPointer<QGraphicsItem> __Internal_Mark_Type_;
	//typedef QWeakPointer<QGraphicsItem> __Internal_Mark_Type_Weak_Ref_;
	//typedef QSharedPointer<CategoryItem> __Internal_Categroy_Type_;
	
	using MarkSliceList = QVector<QList<QGraphicsItem*>>;
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
	TreeItem* getItemHelper(const QModelIndex& index) const;
	//QModelIndex modelIndexHelper(const QModelIndex& root, const QString& display)const;
	QModelIndex categoryIndexHelper(const QString& category)const;
	QModelIndex categoryAddHelper(const QString& category, const QColor& color);		//set dirty
	QModelIndex categoryAddHelper(const CategoryInfo & info);							//set setdirty
	inline bool checkMatchHelper(const AbstractSliceDataModel * dataModel)const;
	void addMarkInSliceHelper(QGraphicsItem * mark);									//set dirty
	void removeMarkInSliceHelper(QGraphicsItem * mark);
	void updateMarkVisibleHelper(QGraphicsItem * mark);							//set dirty
	bool updateMeshMarkHelper(const QString& cate);
	void detachFromView();


	static void retrieveDataFromTreeItemHelper(TreeItem* root, TreeItemType type, int column, QVector<QVariant> & data, int role);
	void initSliceMarkContainerHelper();
	static QVector<QList<StrokeMarkItem*>> refactorMarks(QList<StrokeMarkItem*> &marks);

	//Functions used by ImageView

	const MarkSliceList & topSliceVisibleMarks()const { return m_topSliceVisibleMarks; }
	const MarkSliceList & rightSliceVisibleMarks()const { return m_rightSliceVisibleMarks; }
	const MarkSliceList & frontSliceVisibleMarks()const { return m_frontSliceVisibleMarks; }

	MarkModel(AbstractSliceDataModel * dataModel,
		SliceEditorWidget * view, 
		QObject * parent = nullptr);

	enum {MagicNumber = 1827635234};

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

	/**
	 * \brief A deleted constructor 
	 * 
	 * \a MarkModel is not allowed constructed neither out of a instance of \a SliceEditorWidget nor
	 *  without any parameters. 
	 */
	MarkModel() = delete;

	/**
	 * \overload
	 * \brief Constructs a mark model with a \a fileName
	 *
	 * Creates a mark model from a saved one.
	 * \param fileName mark model file path on disk
	 * \sa 
	 */
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
	inline bool addMark(const QString& text, QGraphicsItem* mark);					//set dirty
	bool addMarks(const QString& text, const QList<QGraphicsItem*>& marks);			//set dirty
	bool addCategory(const CategoryInfo& info);											//set dirty


	QItemSelectionModel * selctionModelOfThisModel()const { return m_selectionModel; };


	QList<QGraphicsItem*> marks(const QString & text)const;
	QList<QGraphicsItem*> marks()const;													//This is time-consuming operation

	QStringList categoryText()const;
	QList<QModelIndex> categoryModelIndices()const;


	//QList<QSharedPointer<CategoryItem>> categoryItems()const;
	//QSharedPointer<CategoryItem> categoryItem(const QString & cate)const;
	QVector<QSharedPointer<Triangulate>> markMesh(const QString& cate);



	bool removeMark(QGraphicsItem* mark);			//set dirty
	int removeMarks(const QList<QGraphicsItem*>& marks = QList<QGraphicsItem*>());		//set dirty
	inline int markCount(const QString & category)const;
	bool save(const QString & fileName,MarkFormat format = MarkFormat::Binary);
	inline void setDirty();
	inline bool dirty()const;
	inline void resetDirty();


	virtual ~MarkModel();

	friend class MarkManagerWidget;
};



/**
 * \brief Filters events if this object has been installed as an event filter for the watched object.
 * 
 *  The filter is used to filter the focus event on 
 * \param watched 
 * \param event 
 * \warning Warning: If you delete the receiver object in this function, be sure to return true. Otherwise, 
 * Qt will forward the event to the deleted object and the program might crash.
 * \return 
 */


/**
 * \brief 
 * \param text 
 * \param mark 
 * \return 
 */
inline bool MarkModel::addMark(const QString& text, QGraphicsItem* mark){return addMarks(text, QList<QGraphicsItem*>{mark});}

/**
 * \brief 
 * \param category 
 * \return 
 */
inline int MarkModel::markCount(const QString & category)const{return rowCount(categoryIndexHelper(category));}

/**
 * \brief Sets dirty  
 */
inline void MarkModel::setDirty() { m_dirty = true; emit modified(); }

/**
 * \brief 
 * \return 
 */
inline bool MarkModel::dirty()const{return m_dirty;}

/**
 * \brief 
 */
inline void MarkModel::resetDirty() { m_dirty = false; emit saved(); }

/**
 * \brief 
 * \param dataModel 
 * \return 
 */
inline bool MarkModel::checkMatchHelper(const AbstractSliceDataModel* dataModel) const{return m_identity == SliceDataIdentityTester::createTester(dataModel);}


#endif // MARKMODEL_H
