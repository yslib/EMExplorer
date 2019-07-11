

#ifndef MARKMODEL_H
#define MARKMODEL_H


#include "model/treeitem.h"
#include "model/instanceitem.h"

#include "abstract/abstractslicedatamodel.h"
#include <QItemSelection>


//#include "treeitem.h"

//#include <QDataStream>
class MarkModel;
class TreeItem;
class QGraphicsItem;
class SliceEditorWidget;
class AbstractSliceDataModel;
class RootTreeItem;
//class CategoryItem;
class StrokeMarkItem;
class QItemSelectionModel;
enum TreeItemType;


/**
 * \brief This selection model is specified for the deletion operation of mark
 */
//
//class MarkItemSelectionModel:public QItemSelectionModel
//{
//public:
//	MarkItemSelectionModel(MarkModel * markModel);
//	void removeSelectedItem(const QModelIndex & parent);
//};


/**
 * \class MarkModel markmodel.h "model/markmodel.h"
 * 
 * \brief This class is used to represent the mark model created by marking.
 * 
 * It inherits from \a QAbstractItemModel
 * 
 */
class MarkModel :public QAbstractItemModel
{
	Q_OBJECT

	/**
	 * \brief These enums are used to identify the user-defined roles
	 */

	enum MarkModelItemRole
	{
		MeshRole = Qt::ItemDataRole::UserRole + 1,			
		MetaDataRole = Qt::ItemDataRole::UserRole + 2,
		TreeItemRole = Qt::ItemDataRole::UserRole + 3
	};
	
	using MarkSliceList = QVector<QList<StrokeMarkItem*>>;
	//state member
	//const AbstractSliceDataModel * m_dataModel;
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



	inline bool checkMatchHelper(const AbstractSliceDataModel * dataModel)const;
	void addMarkInSliceHelper(StrokeMarkItem * mark);									//set dirty
	void removeMarkInSliceHelper(StrokeMarkItem* mark);
	void updateMarkVisibleHelper(StrokeMarkItem * mark);							//set dirty
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
		Raw,		///< Save mark as raw format which is easily accessed by any other raw format reader
		Mask,
		Obj			///< Save mark as .obj file
	};

	/**
	 * \brief This is a deleted constructor.
	 * 
	 * An instance of MarkModel is not allowed to be created externally.
	 * It only can be created from an instance of \a SliceEditorWidget automatically.
	 * Because a mark model is tightly coupled with the \a SliceEditorWidget.
	 */
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


	TreeItem * treeItem(const QModelIndex& index) const;
	TreeItem * rootItem() const;

	//Custom functions for accessing and setting data

	QList<TreeItem*> treeItems(const QModelIndex & parent, int type);
	QModelIndex indexByItem(TreeItem * item);

	bool insertTreeItem(TreeItem * item, const QModelIndex & parent);
	bool insertTreeItems(const QList<TreeItem*> & items, const QModelIndex & parent);
	bool removeTreeItem(TreeItem* item);
	bool removeTreeItems(const QList<TreeItem*> & items);
	QItemSelectionModel * selectionModelOfThisModel()const {return m_selectionModel;};

	MarkSliceList topVisibleMarks()const { return m_topSliceVisibleMarks; }
	MarkSliceList rightVisibleMarks()const { return m_rightSliceVisibleMarks; }
	MarkSliceList frontVisibleMarks()const { return m_frontSliceVisibleMarks; }
	const SliceDataIdentityTester & tester()const { return m_identity;}

	QSharedPointer<char> rawMarks()const;
	QSharedPointer<int> markMask()const;
	

	static void retrieveData(TreeItem * root, TreeItemType type, int column, QVector<QVariant> & data, int role);
	static void retrieveTreeItem(TreeItem * parent, TreeItemType type, QList<TreeItem*>* items);

	bool save(const QString & fileName,MarkFormat format = MarkFormat::Binary);

	inline void setDirty();
	inline bool dirty()const;
	inline void resetDirty();
	virtual ~MarkModel();

};
/**
 * \brief Sets dirty bit of the mark model
 * 
 * \note This function will emit modified() signal
 */
inline void MarkModel::setDirty() { m_dirty = true; emit modified(); }

/**
 * \brief Returns the dirty bit of the mark model
 * 
 * Returns \a true if the mark model is modified, otherwise returns \a false
 */
inline bool MarkModel::dirty()const{return m_dirty;}

/**
 * \brief Reset the dirty bit.
 * 
 * \note This function will emit saved() signal
 */
inline void MarkModel::resetDirty() { m_dirty = false; emit saved(); }

/**
 * \brief 
 * \param dataModel 
 * \return 
 */
inline bool MarkModel::checkMatchHelper(const AbstractSliceDataModel* dataModel) const{return m_identity == SliceDataIdentityTester::createTester(dataModel);}

#endif // MARKMODEL_H

