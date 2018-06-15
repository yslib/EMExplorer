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
	virtual ~AbstractSliceDataModel();
private:
	QVector<QImage> m_modifiedTopSlice;
	QVector<bool> m_modifiedTopSliceFlags;
	QVector<QImage> m_modifiedRightSlice;
	QVector<bool> m_modifiedRightSliceFlags;
	QVector<QImage> m_modifiedFrontSlice;
	QVector<bool> m_modifiedFrontSliceFlags;
};
#endif // ABSTRACTSLICEDATAMODEL_H
