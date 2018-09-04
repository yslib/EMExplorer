#include "gradientcalculator.h"
#include "abstract/abstractslicedatamodel.h"
#include "model/markmodel.h"

#include <QVector3D>



GradientCalculator::GradientCalculator(const AbstractSliceDataModel * slice, const MarkModel * mark, QObject * parent)
	:QObject(parent),
	m_sliceModel(slice),
	m_mark(mark),
	m_ready(false)
{
}

void GradientCalculator::setDataModel(const AbstractSliceDataModel* slice)
{
	if (m_sliceModel == slice)
		return;
	m_sliceModel = slice;
	m_ready = false;
}

void GradientCalculator::setMarkModel(const MarkModel* mark)
{
	//TODO::
}

bool GradientCalculator::ready() const
{
	return m_ready /*m_mark != nullptr*/;
}

bool GradientCalculator::hasData() const
{
	return m_sliceModel != nullptr;
}

unsigned char * GradientCalculator::data() const
{
	return m_gradient.get();
}

void GradientCalculator::init()
{
	if (hasData() == false)
		return;
	const int z = m_sliceModel->topSliceCount();
	const int y = m_sliceModel->rightSliceCount();
	const int x = m_sliceModel->frontSliceCount();
	QMutexLocker locker(&m_mutex);
	m_gradient.reset(new unsigned char[x*y*z * 4]);
}

QVector3D GradientCalculator::triCubicIntpGrad(const unsigned char* pData, double px, double py, double pz)
{
	double ulo, vlo, wlo;
	const double ut = modf(px, &ulo);
	const double vt = modf(py, &vlo);
	const double wt = modf(pz, &wlo);

	int		xlo = int(ulo);
	int		ylo = int(vlo);
	int		zlo = int(wlo);

	// We need all the voxels around the primary
	double voxels[4][4][4];
	{
		for (int z = 0; z < 4; z++) {
			for (int y = 0; y < 4; y++) {
				for (int x = 0; x < 4; x++) {
					int px = (xlo - 1 + x);
					int py = (ylo - 1 + y);
					int pz = (zlo - 1 + z);
					voxels[z][y][x] = value(pData, px, py, pz);
				}
			}
		}
	}

	QVector3D direction;

	// Now that we have all our voxels, run the cubic interpolator in one dimension to collapse it (we choose to collapase x)
	double voxelcol[4][4];
	double voxelcol2[4];
	int x, y, z;
	for (z = 0; z < 4; z++) {
		for (y = 0; y < 4; y++) {
			voxelcol[z][y] = cubicIntpGrad(voxels[z][y][0], voxels[z][y][1], voxels[z][y][2], voxels[z][y][3], ut);
		}
	}
	// Then collapse the y dimension
	for (z = 0; z < 4; z++) {
		voxelcol2[z] = cubicIntpValue(voxelcol[z][0], voxelcol[z][1], voxelcol[z][2], voxelcol[z][3], vt);
	}

	// The collapse the z dimension to get our value
	direction.setX(cubicIntpValue(voxelcol2[0], voxelcol2[1], voxelcol2[2], voxelcol2[3], wt));

	for (z = 0; z < 4; z++) {
		for (x = 0; x < 4; x++) {
			voxelcol[z][x] = cubicIntpGrad(voxels[z][0][x], voxels[z][1][x], voxels[z][2][x], voxels[z][3][x], vt);
		}
	}
	// Then collapse the x dimension
	for (z = 0; z < 4; z++) {
		voxelcol2[z] = cubicIntpValue(voxelcol[z][0], voxelcol[z][1], voxelcol[z][2], voxelcol[z][3], ut);
	}

	// The collapse the z dimension to get our value
	direction.setY(cubicIntpValue(voxelcol2[0], voxelcol2[1], voxelcol2[2], voxelcol2[3], wt));

	for (y = 0; y < 4; y++) {
		for (x = 0; x < 4; x++) {
			voxelcol[y][x] = cubicIntpGrad(voxels[0][y][x], voxels[1][y][x], voxels[2][y][x], voxels[3][y][x], wt);
		}
	}
	// Then collapse the x dimension
	for (y = 0; y < 4; y++) {
		voxelcol2[y] = cubicIntpValue(voxelcol[y][0], voxelcol[y][1], voxelcol[y][2], voxelcol[y][3], ut);
	}

	// The collapse the z dimension to get our value
	direction.setZ(cubicIntpValue(voxelcol2[0], voxelcol2[1], voxelcol2[2], voxelcol2[3], vt));

	return direction;
}

double GradientCalculator::value(const unsigned char* pData, double x, double y, double z) const
{
	const int xiSize = m_sliceModel->topSliceCount();
	const int yiSize = m_sliceModel->rightSliceCount();
	const int ziSize = m_sliceModel->frontSliceCount();
	x = std::max(std::min(x + 0.5, xiSize - 1.0), 0.0);
	y = std::max(std::min(y + 0.5, yiSize - 1.0), 0.0);
	z = std::max(std::min(z + 0.5, ziSize - 1.0), 0.0);
	int index = (int)z * xiSize * yiSize + (int)y * xiSize + (int)x;
	return pData[index];
}

double GradientCalculator::cubicIntpGrad(double v0, double v1, double v2, double v3, double mu)
{
	const double mu2 = mu * mu;
	const double mu3 = mu2 * mu;

	const double a0 = 0.0 * v0 + 0.0 * v1 + 0.0 * v2 + 0.0 * v3;
	const double a1 = -1.0 * v0 + 3.0 * v1 - 3.0 * v2 + 1.0 * v3;
	const double a2 = 2.0 * v0 - 4.0 * v1 + 2.0 * v2 + 0.0 * v3;
	const double a3 = -1.0 * v0 + 0.0 * v1 + 1.0 * v2 + 0.0 * v3;

	return (a0 * mu3 + a1 * mu2 + a2 * mu + a3) / 2.0;
}

double GradientCalculator::cubicIntpValue(double v0, double v1, double v2, double v3, double mu)
{
	const double mu2 = mu * mu;
	const double mu3 = mu2 * mu;

	const double a0 = -1.0 * v0 + 3.0 * v1 - 3.0 * v2 + 1.0 * v3;
	const double a1 = 3.0 * v0 - 6.0 * v1 + 3.0 * v2 + 0.0 * v3;
	const double a2 = -3.0 * v0 + 0.0 * v1 + 3.0 * v2 + 0.0 * v3;
	const double a3 = 1.0 * v0 + 4.0 * v1 + 1.0 * v2 + 0.0 * v3;

	return (a0 * mu3 + a1 * mu2 + a2 * mu + a3) / 6.0;
}

void GradientCalculator::calcGradent()
{
	if (hasData() == false)
		return;
	init();
	const int ziSize = m_sliceModel->topSliceCount();
	const int yiSize = m_sliceModel->rightSliceCount();
	const int xiSize = m_sliceModel->frontSliceCount();
	const auto pOriginalData = m_sliceModel->constData();

	//QMutexLocker locker(&m_mutex);
	if (m_gradient == nullptr)
		return;
	// calculate gradient and the normalized direction
#pragma omp parallel for
	for (int i = 0; i < ziSize; ++i) {
		for (int j = 0; j < yiSize; ++j) {
			for (int k = 0; k < xiSize; ++k) {
				unsigned int index = i * xiSize * yiSize + j * xiSize + k;
				// for 16 bit, already convert to 8 bit
				m_gradient[index * 4 + 3] = pOriginalData[index];
				QVector3D gradient = triCubicIntpGrad(pOriginalData, k, j, i);		// x, y, z
				if (gradient.lengthSquared() > 1e-10) {
					gradient.normalize();
					m_gradient[index * 4 + 0] = (unsigned char)((gradient.x() + 1.0) / 2.0 * 255 + 0.5);
					m_gradient[index * 4 + 1] = (unsigned char)((gradient.y() + 1.0) / 2.0 * 255 + 0.5);
					m_gradient[index * 4 + 2] = (unsigned char)((gradient.z() + 1.0) / 2.0 * 255 + 0.5);
				}
				else {	// gradient = (0, 0, 0)
					m_gradient[index * 4 + 0] = 128;
					m_gradient[index * 4 + 1] = 128;
					m_gradient[index * 4 + 2] = 128;
				}
			}
		}
	}
	emit finished();
	m_ready = true;
}
