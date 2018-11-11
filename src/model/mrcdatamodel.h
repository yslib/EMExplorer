#ifndef MRCDATAMODEL_H
#define MRCDATAMODEL_H
#include <QSharedPointer>
#include "src/abstract/abstractslicedatamodel.h"
#include <functional>


class MRC;

class MRCDataModel:public AbstractSliceDataModel
{
public:
    MRCDataModel(const QSharedPointer<MRC> & data);
    QImage originalTopSlice(int index) const override;
    QImage originalRightSlice(int index) const override;
    QImage originalFrontSlice(int index) const override;

	int dataType() override;
	void * rawData() override;
	const void * constRawData()override;
    inline int topSliceCount() const override;
    inline int rightSliceCount() const override;
    inline int frontSliceCount() const override;
private:
    QSharedPointer<MRC> m_d;


	unsigned char float2uint(float v);

	//template<typename T>
	//QImage extractImage(const std::function<int(int,int)> & indexFunc, const T * data, int width, int height);


};

#endif // MRCDATAMODEL_H
