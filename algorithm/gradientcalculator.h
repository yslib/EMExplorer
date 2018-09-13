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
	QMutex m_mutex;

	bool m_ready;
	struct InternalDataType {
		const unsigned char * d;
		int x;
		int y;
		int z;
		InternalDataType(const unsigned char * dat = nullptr,int xx =0,int yy = 0 ,int zz = 0):d(dat),x(xx),y(yy),z(zz){}
	} m_d;
	
public:
	explicit GradientCalculator(QObject * parent = nullptr);
	GradientCalculator(const unsigned char * data,int x,int y,int z, QObject * parent = nullptr);
	//void setDataModel(const AbstractSliceDataModel * slice);
	void setData(const unsigned char * data, int xSize, int ySize, int zSize);
	bool ready()const;
	bool hasData()const;
	void calcGradent();
	unsigned char * data3()const;
private:


	QVector3D triCubicIntpGrad(const unsigned char * pData, double px, double py, double pz);
	inline double value(const unsigned char* pData, double x, double y, double z) const;
	inline static double cubicIntpGrad(double v0, double v1, double v2, double v3, double mu);
	inline static double cubicIntpValue(double v0, double v1, double v2, double v3, double mu);
	public slots:
	
signals:
	void finished();
};
#endif // GRADIENTCALCULATOR_H