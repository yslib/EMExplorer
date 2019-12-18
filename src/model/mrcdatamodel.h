#ifndef MRCDATAMODEL_H
#define MRCDATAMODEL_H
#include <QSharedPointer>
#include "src/abstract/abstractslicedatamodel.h"

#ifdef _OPENMP
#include <omp.h>
#endif

class MRC;

class MRCDataModel:public AbstractSliceDataModel
{
public:
    MRCDataModel(const QSharedPointer<MRC> & data);

    QImage originalTopSlice(int index) const override;
    QImage originalRightSlice(int index) const override;
    QImage originalFrontSlice(int index) const override;

	QVector<QVector<int>> topSliceGradient(int index) const { return m_d_gradient[index];}
	int dataType() override;
	void * rawData() override;
	const void * constRawData()override;
    inline int topSliceCount() const override;
    inline int rightSliceCount() const override;
    inline int frontSliceCount() const override;

	float minValue() const;
	float maxValue() const;

	~MRCDataModel();
private:
    QSharedPointer<MRC> m_d;
	QVector<QVector<QVector<int>>> m_d_gradient;
	struct MRCStatistic 
	{
		double mean;
		double var;
		MRCStatistic():mean(0),var(0){}
	}m_statistic;

	void adjustImage(QImage & image) const;
	void preCalc();
	void calGradient();
};




#endif // MRCDATAMODEL_H
