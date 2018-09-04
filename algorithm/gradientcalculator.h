#ifndef GRADIENTCALCULATOR_H
#define GRADIENTCALCULATOR_H
#include <QObject>
#include <memory>
#include <QMutex>

class AbstractSliceDataModel;
class MarkModel;

class GradientCalculator :public QObject
{
	Q_OBJECT
		std::unique_ptr<unsigned char[]> m_gradient;
	const AbstractSliceDataModel * m_sliceModel;
	const MarkModel * m_mark;
	QMutex m_mutex;
	bool m_ready;
public:
	GradientCalculator(const AbstractSliceDataModel * slice, const MarkModel * mark, QObject * parent = nullptr);
	void setDataModel(const AbstractSliceDataModel * slice);
	void setMarkModel(const MarkModel * mark);
	bool ready()const;
	bool hasData()const;
	unsigned char * data()const;
private:
	void init();
	QVector3D triCubicIntpGrad(const unsigned char * pData, double px, double py, double pz);
	inline double value(const unsigned char* pData, double x, double y, double z) const;
	inline static double cubicIntpGrad(double v0, double v1, double v2, double v3, double mu);
	inline static double cubicIntpValue(double v0, double v1, double v2, double v3, double mu);
	public slots:
	void calcGradent();
signals:
	void finished();
};
#endif // GRADIENTCALCULATOR_H