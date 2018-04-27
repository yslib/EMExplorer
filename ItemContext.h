#pragma once
#ifndef MRCMODEL_H_
#define MRCMODEL_H_
#include <QObject>
#include <QString>
#include <QPicture>
//#include <qDebug>
#include <qdebug.h>
#include <QImage>
#include <QRectF>
#include <QAbstractItemModel>
#include <QSharedPointer>

#include "mrc.h"

//class MRCDataBaseModel
//{
//public:
//
//	MRCDataBaseModel();
//    MRCDataBaseModel(const QString & fileName,int width,int height);
//	virtual ~MRCDataBaseModel();
//
//	int getWidth()const { return m_mrcFile.getWidth(); }
//	int getHeight()const { return m_mrcFile.getHeight(); }
//	int getSliceCount()const { return m_mrcFile.getSliceCount(); }
//	bool save(const QString & fileName);
//	bool open(const QString & fileName);
//
//	bool isOpened()const { return m_mrcFile.isOpened(); }
//	//const QSharedPointer<MRC> & getMRC()const { return QSharedPointer<MRC>(m_mrcFile); }
//private:
//	MRC m_mrcFile;
//
//};

class ItemContext //:public QObject
{
	//Q_OBJECT
public:
    enum class MarkFormat {MRC,RAW,mks};
    enum class DataFormat{mrc,raw};
public:
	ItemContext();
    explicit ItemContext(const QString & fileName);
	explicit ItemContext(const ItemContext & model);
	explicit ItemContext(ItemContext && model)noexcept;
	ItemContext & operator=(const ItemContext & model);
	ItemContext & operator=(ItemContext && model)noexcept;
	virtual ~ItemContext();

	void setGrayscaleStrechingLowerBound(int value) { m_mrcContext.grayscaleStrechingLowerBound = value; }
	void setGrayScaleStrechingUpperBound(int value) { m_mrcContext.grayscaleStrechingUpperBound = value; }
	void setGrayscaleStrechBound(int lower, int upper) { setGrayscaleStrechingLowerBound(lower); setGrayScaleStrechingUpperBound(upper); }
	void setGrayscaleLowerBound(int value) { m_mrcContext.grayscaleLowerBound = value; }
	void setGrayscaleUpperBound(int value) { m_mrcContext.grayscaleUpperBound = value; }
	void setGrayscaleBound(int lower, int upper) { setGrayscaleLowerBound(lower); setGrayscaleUpperBound(upper); }
	int getGrayscaleStrechingLowerBound()const { return m_mrcContext.grayscaleStrechingLowerBound; }
	int getGrayscaleStrechingUpperBound()const { return m_mrcContext.grayscaleStrechingUpperBound; }
	int getGrayscaleLowerBound()const { return m_mrcContext.grayscaleLowerBound; }
	int getGrayscaleUpperBound()const { return m_mrcContext.grayscaleUpperBound; }

	int getMinGrayscale()const { return m_mrcContext.minGrayscale; }
	int getMaxGrayscale()const { return m_mrcContext.maxGrayscale; }

	int getWidth()const { return m_mrcFile.getWidth(); }
	int getHeight()const { return m_mrcFile.getHeight(); }
	QImage getOriginalSlice(int index)const;

	void setZoomFactor(qreal factor) { m_mrcContext.zoomFactor = factor; }
	qreal getZoomFactor()const { return m_mrcContext.zoomFactor; }
	void setZoomRegion(QRect region) { m_mrcContext.zoomRegion = region;}
	QRect getZoomRegion()const { return m_mrcContext.zoomRegion;  }

	bool isValid()const { return m_mrcContext.valid;}

	void setCurrentSliceIndex(int slice) { m_mrcContext.currentTopSliceIndex=slice; }
	int getCurrentSliceIndex()const { return m_mrcContext.currentTopSliceIndex; }

	void setCurrentRightSliceIndex(int index) { m_mrcContext.currentRightSliceIndex = index; }
	int getCurrentRightSliceIndex()const { return m_mrcContext.currentRightSliceIndex; }

	void setCurrentFrontSliceIndex(int index) { m_mrcContext.currentFrontSliceIndex = index; }
	int getCurrentFrontSliceIndex()const { return m_mrcContext.currentFrontSliceIndex; }

	int getTopSliceCount()const { return m_mrcFile.getSliceCount(); }

    bool save(const QString & fileName, ItemContext::DataFormat formate = ItemContext::DataFormat::mrc);
	bool open(const QString & fileName);
	bool isOpened()const { return isValid(); }



	bool openMarks(const QString & fileName);
	bool saveMarks(const QString & fileName,MarkFormat format = MarkFormat::MRC);
	QString getMRCInfo()const { return QString(QString::fromLocal8Bit(m_mrcFile.getMRCInfo().c_str())); }
	QImage getSlice(int index)const;
	void setSlice(const QImage & image, int index);
	QImage getRightSlice(int index) const;
    int getRightSliceCount()const{return m_mrcFile.getWidth();}
    int getFrontSliceCount()const{return m_mrcFile.getHeight();}
	QImage getFrontSlice(int index) const;

	QVector<QImage> getSlices()const;
    void setMark(const QPicture & mark, int index);
    void addMark(int slice,const QPicture& mark);
	QPicture getMark(int index)const;
    QVector<QPicture> getMarks(int slice)const;
	const MRC & getMRCFile()const { return m_mrcFile; }

private:
	struct MRCContext {
		MRCContext() :grayscaleStrechingLowerBound{ 0 },
			grayscaleStrechingUpperBound{ 255 },
			grayscaleUpperBound{ 255 },
			grayscaleLowerBound{ 0 },
			minGrayscale{ 0 },
			maxGrayscale{ 255 },
			currentTopSliceIndex{ -1 },
			currentRightSliceIndex(-1),
			currentFrontSliceIndex(-1),
            zoomFactor{1.0},
            zoomRegion{},
			valid{ false } {}

		int grayscaleLowerBound;
		int grayscaleUpperBound;
		int grayscaleStrechingLowerBound;
		int grayscaleStrechingUpperBound;
		int minGrayscale;
		int maxGrayscale;
		int currentTopSliceIndex;
		int currentRightSliceIndex;
		int currentFrontSliceIndex;
		qreal zoomFactor;
        QRect zoomRegion;
		bool valid;
	};
private:
	MRC m_mrcFile;
	MRCContext m_mrcContext;



	QVector<QImage> m_modified;
	QVector<bool> m_modifiedFlags;
	///Useless
    QVector<QVector<QPicture>> m_marks;


};


Q_DECLARE_METATYPE(QSharedPointer<MRC>);
Q_DECLARE_METATYPE(QSharedPointer<ItemContext>);


class TreeItem
{
	TreeItem * m_parent;
	QVector<TreeItem*> m_children;
	QVector<QVariant> m_data;
public:
	explicit TreeItem(const QVector<QVariant> & data, TreeItem * parent = nullptr) :m_data(data), m_parent(parent) {}
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
	int columnCount()const { return m_data.size(); }
	QVariant data(int column = 0)const { return m_data.value(column); }

	/**
	*All above methods are necessary for a read-only TreeView.
	*Following methods are requried for a editable TreeView.
	*
	*/

	bool insertChildren(int position, int count, int columns)
	{
		///TODO:: Is this check necessary? 
		if (position < 0 || position > m_children.size())return false;
		for (int row = 0; row<count; row++)
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
		for (int i = 0; i<count; i++)
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
		for (auto & child : m_children)
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



class DataItemModel :public QAbstractItemModel
{
	Q_OBJECT
		TreeItem * m_rootItem;

	QList<QModelIndex> m_itemRootIndex;
	/**
	* \brief
	* \param index
	* \return return a non-null internal pointer of the index or return root pointer
	*/
	TreeItem * getItem(const QModelIndex & index)const
	{
		if (index.isValid())
		{
			TreeItem * item = static_cast<TreeItem*>(index.internalPointer());
			if (item)return item;
		}
		return m_rootItem;
	}

private:

	QModelIndex appendChild(const QModelIndex & parent = QModelIndex(), bool * success = nullptr);
	bool removeChild(const QModelIndex & index, const QModelIndex & parent = QModelIndex());
	QModelIndex modelIndexOf(int column, const QModelIndex & parent);
	void insertRootItemIndex(const QModelIndex & index, int position = -1);
	void removeRootItemIndex(int position);
	QModelIndex rootItemIndex(int position);
	QModelIndex addItemHelper(const QString& fileName, const QString& info);

public:
	explicit DataItemModel(const QString & data, QObject * parent = nullptr);
	~DataItemModel();
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


	void addItem(const QSharedPointer<MRC> & item);
	void addItem(const QSharedPointer<ItemContext> & item);
	//void addData(data);
	//void addMarks(data,marks);
	//bool saveData(data);
	//bool saveMarks(data);
};


#endif
