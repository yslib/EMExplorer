#ifndef ABSTRACTSLICEDATAMODEL_H
#define ABSTRACTSLICEDATAMODEL_H

#include <QImage>


class AbstractSliceDataModel
{
public:
	AbstractSliceDataModel(int nTop, int nRight, int nFront);
	virtual int topSliceCount()const = 0;
	virtual int rightSliceCount()const = 0;
	virtual int frontSliceCount()const = 0;
	virtual QImage originalTopSlice(int index) const = 0;
	virtual QImage originalRightSlice(int index) const = 0;
	virtual QImage originalFrontSlice(int index) const = 0;
	virtual void setTopSlice(const QImage& image, int index);
	virtual void setRightSlice(const QImage& image, int index);
	virtual void setFrontSlice(const QImage& image, int index);
	virtual QImage topSlice(int index)const;
	virtual QImage rightSlice(int index)const;
	virtual QImage frontSlice(int index)const;
	virtual unsigned char * data()=0;
	virtual const unsigned char * constData()const = 0;

	virtual ~AbstractSliceDataModel();
private:
	QVector<QImage> m_modifiedTopSlice;
	QVector<bool> m_modifiedTopSliceFlags;
	QVector<QImage> m_modifiedRightSlice;
	QVector<bool> m_modifiedRightSliceFlags;
	QVector<QImage> m_modifiedFrontSlice;
	QVector<bool> m_modifiedFrontSliceFlags;
};

/**
 * \class	SliceDataIdentityTester
 *
 * \brief	A slice data identity tester. This class is used 
 * 			to test whether the slice data model matches the mark data model.
 *
 * \author	Ysl
 * \date	2018.08.03
 */

class SliceDataIdentityTester
{
	int m_topSliceCount;
	int m_rightSliceCount;
	int m_frontSliceCount;
	QSize m_topSliceSize;
	QSize m_rightSliceSize;
	QSize m_frontSliceSize;
public:
	SliceDataIdentityTester();
	inline SliceDataIdentityTester(const AbstractSliceDataModel * model);
	inline bool identity(const SliceDataIdentityTester & id)const noexcept;
	inline bool isValid()const noexcept;
	inline bool operator== (const SliceDataIdentityTester & id)const noexcept;
	inline bool operator!=(const SliceDataIdentityTester & id)const noexcept;
	inline int topSliceCount()const;
	inline int rightSliceCount()const;
	inline int frontSliceCount()const;
	inline friend QDataStream & operator<<(QDataStream & stream, const SliceDataIdentityTester & id);
	inline friend QDataStream & operator>>(QDataStream & stream, SliceDataIdentityTester & id);
	inline static SliceDataIdentityTester createTester(const AbstractSliceDataModel * model);

};
inline SliceDataIdentityTester::SliceDataIdentityTester() :
	m_topSliceCount(-1),
	m_rightSliceCount(-1),
	m_frontSliceCount(-1)
{}
inline SliceDataIdentityTester::SliceDataIdentityTester(const AbstractSliceDataModel* model) :
	m_topSliceCount(-1),
	m_rightSliceCount(-1),
	m_frontSliceCount(-1)
{
	if (model != nullptr)
	{
		m_topSliceCount = model->topSliceCount();
		m_rightSliceCount = model->rightSliceCount();
		m_frontSliceCount = model->frontSliceCount();
		m_topSliceSize = model->originalTopSlice(m_topSliceCount - 1).size();
		m_rightSliceSize = model->originalRightSlice(m_rightSliceCount - 1).size();
		m_frontSliceSize = model->originalFrontSlice(m_frontSliceCount - 1).size();
	}
}


inline bool SliceDataIdentityTester::identity(const SliceDataIdentityTester & id)const noexcept
{
	return (m_topSliceCount == id.m_topSliceCount)
		&& (m_rightSliceCount == id.m_rightSliceCount)
		&& (m_frontSliceCount == id.m_frontSliceCount)
		&& (m_topSliceSize == id.m_topSliceSize)
		&& (m_rightSliceSize == id.m_rightSliceSize)
		&& (m_frontSliceSize == id.m_frontSliceSize);
}


inline bool SliceDataIdentityTester::isValid() const noexcept
{
	return m_topSliceCount >= 0 &&
		m_rightSliceCount >= 0 &&
		m_frontSliceCount >= 0 &&
		m_topSliceSize.isValid() &&
		m_rightSliceSize.isValid() &&
		m_frontSliceSize.isValid();
}
inline bool SliceDataIdentityTester::operator==(const SliceDataIdentityTester& id) const noexcept { return identity(id); }
inline bool SliceDataIdentityTester::operator!=(const SliceDataIdentityTester& id) const noexcept { return!identity(id); }
inline int SliceDataIdentityTester::topSliceCount()const { return m_topSliceCount; }
inline int SliceDataIdentityTester::rightSliceCount()const { return m_rightSliceCount; }
inline int SliceDataIdentityTester::frontSliceCount()const { return m_frontSliceCount; }

inline QDataStream & operator<<(QDataStream & stream, const SliceDataIdentityTester &id)
{
	stream << static_cast<qint32>(id.m_topSliceCount)
		<< static_cast<qint32>(id.m_rightSliceCount)
		<< static_cast<qint32>(id.m_frontSliceCount)
		<< id.m_topSliceSize
		<< id.m_rightSliceSize
		<< id.m_frontSliceSize;
	return stream;

}
inline QDataStream & operator>>(QDataStream & stream, SliceDataIdentityTester & id)
{
	qint32 a, b, c;
	stream >> a >> b >> c >> id.m_topSliceSize >> id.m_rightSliceSize >> id.m_frontSliceSize;
	id.m_topSliceCount = static_cast<int>(a);
	id.m_rightSliceCount = static_cast<int>(b);
	id.m_frontSliceCount = static_cast<int>(c);
	return stream;
}

inline SliceDataIdentityTester SliceDataIdentityTester::createTester(const AbstractSliceDataModel* model)
{
	return SliceDataIdentityTester(model);
}


#endif // ABSTRACTSLICEDATAMODEL_H
