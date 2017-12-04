#include "mrcmodel.h"
MRCModel::MRCModel() :m_mrcContext{}
{
}
MRCModel::MRCModel(const QString & fileName):MRCModel()
{
	m_mrcFile.open(fileName);
	m_mrcContext.valid = m_mrcFile.isOpened();
}
MRCModel::~MRCModel()
{

}
