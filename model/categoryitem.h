#ifndef CATEGORYITEM_H
#define CATEGORYITEM_H
#include <QSharedPointer>
#include <QColor>


struct CategoryInfo {
	QString name;
	QColor color;
	CategoryInfo(const QString & n, const QColor & c):name(n),color(c){}
};

class CategoryItem
{
	CategoryInfo m_info;
	int m_count;
	bool m_visible;
public:
	CategoryItem(const QString & name = QString(), const QColor & color = Qt::black, int count = 0, bool visible = true) :
		m_count(count), m_visible(visible) {
		m_info.name = name;
		m_info.color = color;
	}
	CategoryItem(const CategoryInfo & info);
	inline QString name()const noexcept;
	inline int count()const noexcept;
	inline bool visible()const noexcept;
	inline QColor color()const noexcept;
	inline void setName(const QString & n)noexcept;
	inline void setCount(int c)noexcept;
	inline void setVisible(bool visible)noexcept;
	inline void setColor(const QColor & c)noexcept;
	inline void increaseCount()noexcept;
	inline void decreaseCount()noexcept;
	inline const CategoryInfo& categoryInfo() const;

	friend QDataStream & operator<< (QDataStream & stream, const CategoryItem & item);
	friend QDataStream & operator>>(QDataStream & stream, CategoryItem & item);
	friend QDataStream & operator<< (QDataStream & stream, const QSharedPointer<CategoryItem> & item);
	friend QDataStream & operator>>(QDataStream & stream, QSharedPointer<CategoryItem>& item);
};

inline CategoryItem::CategoryItem(const CategoryInfo& info):CategoryItem(info.name,info.color) {}
inline QString CategoryItem::name()const noexcept { return m_info.name; }
inline int CategoryItem::count()const noexcept { return m_count; }
inline bool CategoryItem::visible()const noexcept { return m_visible; }
inline const CategoryInfo & CategoryItem::categoryInfo()const 
{
	return m_info;
}

inline QColor CategoryItem::color() const noexcept
{
	return m_info.color;
}

inline void CategoryItem::setName(const QString & n) noexcept
{
	m_info.name = n;
}

inline void CategoryItem::setCount(int c) noexcept
{
	m_count = c;
}

inline void CategoryItem::setVisible(bool visible) noexcept
{
	m_visible = visible;
}

inline void CategoryItem::setColor(const QColor & c) noexcept
{
	m_info.color = c;
}

inline void CategoryItem::increaseCount() noexcept
{
	m_count++;
}

inline void CategoryItem::decreaseCount() noexcept
{
	if (m_count)m_count--;
}

Q_DECLARE_METATYPE(QSharedPointer<CategoryItem>);
#endif // CATEGORYITEM_H