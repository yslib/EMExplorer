#ifndef CATEGORYITEM_H
#define CATEGORYITEM_H

#include "model/treeitem.h"
#include <QColor>
#include <QString>
#include <QDataStream>



/**
 * \brief A info struct needed by constructing a CategoryItem
 */
class CategoryInfo {
	public:
	QString name;
	QColor color;
	explicit CategoryInfo(const QString & n = QString(), const QColor & c = Qt::black):name(n),color(c){}
};

/**
 * \brief A class that describes a category's necessary information
 */

class CategoryItem
{
	CategoryInfo m_info;
	bool m_visible;
public:
	CategoryItem(const QString & name = QString(), const QColor & color = Qt::black, int count = 0, bool visible = true) :
		m_visible(visible) {
		m_info.name = name;
		m_info.color = color;
	}
	CategoryItem(const CategoryInfo & info);
	inline QString name()const noexcept;
	inline bool visible()const noexcept;
	inline QColor color()const noexcept;
	inline void setName(const QString & n)noexcept;
	inline void setVisible(bool visible)noexcept;
	inline void setColor(const QColor & c)noexcept;
	inline const CategoryInfo& categoryInfo() const;

	friend QDataStream & operator<< (QDataStream & stream, const CategoryItem & item);
	friend QDataStream & operator>>(QDataStream & stream, CategoryItem & item);
};

/**
 * \brief 
 * \param info 
 */
inline CategoryItem::CategoryItem(const CategoryInfo& info):CategoryItem(info.name,info.color) {}

/**
 * \brief Return category name
 * \return 
 */
inline QString CategoryItem::name()const noexcept { return m_info.name; }

/**
 * \brief Return the category visibility
 * \return 
 */
inline bool CategoryItem::visible()const noexcept { return m_visible; }

/**
 * \brief 
 * \return 
 */
inline const CategoryInfo & CategoryItem::categoryInfo()const {return m_info;}

inline QColor CategoryItem::color() const noexcept{return m_info.color;}

inline void CategoryItem::setName(const QString & n) noexcept{m_info.name = n;}

inline void CategoryItem::setVisible(bool visible) noexcept{m_visible = visible;}

inline void CategoryItem::setColor(const QColor & c) noexcept{m_info.color = c;}




/**
 * \brief This class represents a category node in the mark tree view
 * 
 * 
 */

class CategoryTreeItemInfoModel:public QAbstractItemModel {

	QVector<QVector<QString>> m_table;
public:
	explicit CategoryTreeItemInfoModel(QObject * parent = nullptr);
	QVariant data(const QModelIndex& index, int role) const override;
	int columnCount(const QModelIndex& parent) const override;
	QModelIndex index(int row, int column, const QModelIndex& parent) const override;
	int rowCount(const QModelIndex& parent) const override;
	QModelIndex parent(const QModelIndex& child) const override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;


	// Custom API for setting information
	auto table()->QVector<QVector<QString>>& { return m_table; }
};


class CategoryTreeItem :public TreeItem 
{
	CategoryItem m_categoryItem;
public:
	CategoryTreeItem(const CategoryItem& categoryItem,const QPersistentModelIndex & pIndex, TreeItem* parent);

	QVariant data(int column, int role) const override;

	int columnCount() const override;

	bool setData(int column, const QVariant& value, int role) override;

	bool insertColumns(int position, int columns) override;

	bool removeColumns(int position, int columns) override;

	int type() const override;

	void* metaData() override;

	QAbstractItemModel * infoModel() const override { return nullptr; }

	friend QDataStream& operator<<(QDataStream& stream, const CategoryTreeItem* item);

	friend QDataStream & operator>>(QDataStream & stream, CategoryTreeItem *& item);
};



#endif // CATEGORYITEM_H