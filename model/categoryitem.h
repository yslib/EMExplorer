#ifndef CATEGORYITEM_H
#define CATEGORYITEM_H
#include <QSharedPointer>
#include <QColor>



class CategoryItem
{
	QString m_name;
	int m_count;
	bool m_visible;
	QColor m_color;
public:
	CategoryItem(const QString & name = QString(), const QColor & color = Qt::black, int count = 0, bool visible = true) :
		m_name(name), m_color(color), m_count(count), m_visible(visible) {}
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

	friend QDataStream & operator<< (QDataStream & stream, const CategoryItem & item);
	friend QDataStream & operator>>(QDataStream & stream, CategoryItem & item);
	friend QDataStream & operator<< (QDataStream & stream, const QSharedPointer<CategoryItem> & item);
	friend QDataStream & operator>>(QDataStream & stream, QSharedPointer<CategoryItem>& item);
};

inline QString CategoryItem::name()const noexcept { return m_name; }
inline int CategoryItem::count()const noexcept { return m_count; }
inline bool CategoryItem::visible()const noexcept { return m_visible; }

inline QColor CategoryItem::color() const noexcept
{
	return m_color;
}

inline void CategoryItem::setName(const QString & n) noexcept
{
	m_name = n;
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
	m_color = c;
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