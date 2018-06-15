#include "abstractslicedatamodel.h"

void AbstractSliceDataModel::setTopSlice(const QImage& image, int index)
{
	m_modifiedTopSliceFlags[index] = true;
	m_modifiedTopSlice[index] = image;
}
void AbstractSliceDataModel::setRightSlice(const QImage& image, int index)
{
	m_modifiedRightSliceFlags[index] = true;
	m_modifiedRightSlice[index] = image;
}

void AbstractSliceDataModel::setFrontSlice(const QImage& image, int index)
{
	m_modifiedFrontSliceFlags[index] = true;
	m_modifiedFrontSlice[index] = image;
}

QImage AbstractSliceDataModel::topSlice(int index) const
{
	if (m_modifiedTopSliceFlags[index] == false)
		return originalTopSlice(index);
	return m_modifiedTopSlice[index];
}

QImage AbstractSliceDataModel::rightSlice(int index) const
{
	if (m_modifiedRightSliceFlags[index] == false)
		return originalRightSlice(index);
	return m_modifiedRightSlice[index];
}

QImage AbstractSliceDataModel::frontSlice(int index) const
{
	if (m_modifiedFrontSliceFlags[index] == false)
		return originalFrontSlice(index);
	return m_modifiedFrontSlice[index];

}
AbstractSliceDataModel::~AbstractSliceDataModel()
{
}
AbstractSliceDataModel::AbstractSliceDataModel(int nTop, int nRight, int nFront)
{
	m_modifiedFrontSlice.resize(nFront);
	m_modifiedFrontSliceFlags.resize(nFront);
	m_modifiedRightSlice.resize(nRight);
	m_modifiedRightSliceFlags.resize(nRight);
	m_modifiedTopSlice.resize(nTop);
	m_modifiedTopSliceFlags.resize(nTop);
}

