#ifndef MRCDATAMODEL_H
#define MRCDATAMODEL_H
#include <QSharedPointer>
#include "src/abstract/abstractslicedatamodel.h"

#include <omp.h>

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

	~MRCDataModel();
private:
    QSharedPointer<MRC> m_d;

	omp_lock_t* m_ompLock;


	struct MRCStatistic 
	{
		double mean;
		double var;
		MRCStatistic():mean(0),var(0){}
	}m_statistic;


	//void initCache();
	//template<typename T>
	//QImage extractImage(const std::function<int(int,int)> & indexFunc, const T * data, int width, int height);

	void adjustImage(QImage & image) const;

	void preCalc();

};

#endif // MRCDATAMODEL_H
