#ifndef MRCDATAMODEL_H
#define MRCDATAMODEL_H
#include <QSharedPointer>
#include "abstractslicedatamodel.h"


class MRC;

class MRCDataModel:public AbstractSliceDataModel
{
public:
    MRCDataModel(const QSharedPointer<MRC> & data);
    QImage originalTopSlice(int index) const Q_DECL_OVERRIDE;
    QImage originalRightSlice(int index) const Q_DECL_OVERRIDE;
    QImage originalFrontSlice(int index) const Q_DECL_OVERRIDE;
    inline int topSliceCount() const Q_DECL_OVERRIDE;
    inline int rightSliceCount() const Q_DECL_OVERRIDE;
    inline int frontSliceCount() const Q_DECL_OVERRIDE;

	
private:
    QSharedPointer<MRC> m_d;
};
#endif // MRCDATAMODEL_H
