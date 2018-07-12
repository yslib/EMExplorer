#include <iostream>
#include <string>
#include <ctime>
#include <omp.h>
#include "Volume.h"
//#include "gdcmSystem.h"
//#include "gdcmDirectory.h"
//#include "gdcmImageReader.h"
using namespace std;

/*
 *	Volume Construction / Destruction
 */
Volume::Volume()
{
	voxelFormat = UINT8;
	pOriginalData = NULL;
	xiSize  = yiSize  = ziSize  = 0;
	xSpace  = ySpace  = zSpace  = 1.0;
	xfSize  = yfSize  = zfSize  = 0.0;
	
	tableWidth  = 256;
	tableHeight = 128;
	pHistogram2D = new double[tableWidth * tableHeight];
}

Volume::~Volume()
{
	delete []pOriginalData;
	pOriginalData = NULL;
	delete []pHistogram2D;
	pHistogram2D = NULL;
}

/*
 *	Load Raw Volume Data
 */
bool Volume::loadRawData(const char* filename)
{
	FILE *fp = fopen(filename, "r");
	if(fp == NULL) {
		cout<<"Can't open file "<<filename<<endl;
		return false;
	}

	// read volume information
	char dataFile[1024];
	char opacityFile[1024];
	fscanf(fp, "%d %d %d\n", &xiSize, &yiSize, &ziSize);
	fscanf(fp, "%lf %lf %lf\n", &xSpace, &ySpace, &zSpace);
	fscanf(fp, "%s", dataFile);
	//fscanf(fp, "%s", opacityFile);

	fclose(fp);

	// calculate the padding size and the normalized size
	xfSize  = xiSize * xSpace;
	yfSize  = yiSize * ySpace;
	zfSize  = ziSize * zSpace;
	xpSize = getPow2(xiSize);
	ypSize = getPow2(yiSize);
	zpSize = getPow2(ziSize);
	double maxSize = Max(Max(xfSize, yfSize), zfSize);
	xfSize  = xfSize / maxSize;
	yfSize  = yfSize / maxSize;
	zfSize  = zfSize / maxSize;

	string filePath(filename);
	filePath = filePath.substr(0, filePath.rfind('/') + 1);
	filePath += dataFile;

	// read volume data
	fp = fopen(filePath.c_str(), "rb");
	if(fp == NULL) {
		cout<<"Can't open data file "<<dataFile<<endl;
		return false;
	}
	voxelFormat = UINT8;
	delete []pOriginalData;
	pOriginalData = NULL;
	pOriginalData = new unsigned char[xiSize * yiSize * ziSize];
	if(pOriginalData == NULL) {
		fclose(fp);
		delete []pOriginalData;
		pOriginalData = NULL;
		cout<<"Out of memory "<<xiSize * yiSize * ziSize / 1024.0 / 1024.0<<" M"<<endl;
		return false;
	}
	fread(pOriginalData, sizeof(unsigned char), xiSize * yiSize * ziSize, fp);
	fclose(fp);

	// calculate isosurface statistic
	calcIsoStat(pOriginalData);

	return true;
}

/*
 * load DICOM data
 */
bool Volume::loadDICOMData(const char* filename)
{
//	string directory(filename);
//	if(!gdcm::System::FileIsDirectory(directory.c_str()))
//		directory = directory.substr(0, directory.rfind('/')); 
//
//	clock_t time1 = clock();
//	if(gdcm::System::FileIsDirectory(directory.c_str())) {
//		gdcm::Directory dir;
//		dir.Load(directory.c_str(), 0);
//		gdcm::Directory::FilenamesType const &filenames = dir.GetFilenames();
//		readDICOMInfo(filenames);
//
//		int offset    = 0;
//		int voxelSize = getVoxelSize(voxelFormat);
//		int imageSize = voxelSize * xiSize * yiSize;
//		char *pData   = new char[voxelSize * xiSize * yiSize * ziSize];
//		for(gdcm::Directory::FilenamesType::const_iterator it = filenames.begin(); it != filenames.end(); ++it) {
//			gdcm::ImageReader imageReader;
//			imageReader.SetFileName((*it).c_str());
//			if(imageReader.Read()) {
//				const gdcm::Image &gimage = imageReader.GetImage();
//				gimage.GetBuffer(pData + offset);
//				offset += imageSize;
//			}
//		}
//		if(voxelFormat != INT16 && voxelFormat != UINT16)
//			cout<<"Not Support Voxel Format"<<endl;
//		quantize((short int*)pData);
//		delete []pData;
//    }
//	else {
//		cout<<"Invalid filename: "<<filename<<endl;
//		return false;
//    }
//	cout<<"Load DICM Time: "<<(clock() - time1) / (double)CLOCKS_PER_SEC<<" sec."<<endl;
//
//	// calculate isosurface statistic
//	calcIsoStat(pOriginalData);
//
   return true;
}

/*
 *	get the power of 2 greater than or equal to size
 */
int Volume::getPow2(int size)
{
	double tmp = size / 2.0;
	int i = 2;
	while(tmp > 1.0){
		i *= 2;
		tmp = tmp / 2.0;
	}
	return i;
}

/*
 *  get the voxel size in bytes from the voxer format
 */
int Volume::getVoxelSize(VoxelFormat voxelFormat)
{
	int pixelSize = 0;
	switch(voxelFormat) {
		case UINT8:
		case INT8:
			pixelSize = 1;
			break;

		case UINT12:
		case INT12:
			pixelSize = 2;
			break;

		case UINT16:
		case INT16:
			pixelSize = 2;
			break;

		case UINT32:
		case INT32:
			pixelSize = 4;
			break;

		case FLOAT16:
			pixelSize = 2;
			break;

		case FLOAT32:
			pixelSize = 4;
			break;

		case FLOAT64:
			pixelSize = 8;
			break;

		case SINGLEBIT:
			pixelSize = 1;
			break;

		default:
			std::cout<<"Unsupported pixel format"<<std::endl;
		}
	return pixelSize;
}

/*
 * read DICOM information
 */
//void Volume::readDICOMInfo(/*gdcm::Directory::FilenamesType const& filenames*/)
//{
	/*ziSize = 0;
	double preZ = 0;
	bool lastValid = false;
	for(gdcm::Directory::FilenamesType::const_iterator it = filenames.begin(); it != filenames.end(); ++it) {
		gdcm::ImageReader imageReader;
		imageReader.SetFileName((*it).c_str());
		if(imageReader.Read()) {
			const gdcm::Image &image = imageReader.GetImage();
            const unsigned int *dim = image.GetDimensions();
            const double *sp = image.GetSpacing();
			const gdcm::PixelFormat pf = image.GetPixelFormat();
			const double *origin = image.GetOrigin();

			if(ziSize == 0) {
		        xiSize = dim[0];
				yiSize = dim[1];
				voxelFormat = (VoxelFormat)int(pf); 
				xSpace  = sp[0];
				ySpace  = sp[1];
			} else {
				if(ziSize == 1)
					zSpace = abs(origin[2] - preZ);
				if(xiSize != dim[0] || yiSize != dim[1])
					cout<<"XY Size: "<<xiSize<<" "<<xiSize<<" "<<dim[0]<<" "<<yiSize<<" "<<dim[1]<<endl;
				if(voxelFormat != (VoxelFormat)int(pf))
					cout<<"VoxelFormat: "<<voxelFormat<<" "<<pf<<endl;
				if(xSpace != sp[0] || ySpace != sp[1] || abs(zSpace - abs(origin[2] - preZ)) > 1e-6)
					cout<<"XY Size: "<<xiSize<<" "<<xiSize<<" "<<dim[0]<<" "<<yiSize<<" "<<dim[1]<<" "
						<<zSpace<<" "<<abs(origin[2] - preZ)<<endl;
			}

			ziSize++;
			preZ = origin[2];
			lastValid = true;
		}
		else {
			lastValid = false;
			std::cout << "filename "<<ziSize<<": " << *it << std::endl;
		}
	}

	xfSize  = xiSize * xSpace;
	yfSize  = yiSize * ySpace;
	zfSize  = ziSize * zSpace;
	double maxSize = Max(Max(xfSize, yfSize), zfSize);
	xfSize  = xfSize / maxSize;
	yfSize  = yfSize / maxSize;
	zfSize  = zfSize / maxSize;
	
	cout<<voxelFormat<<endl;
	cout<<xiSize<<" "<<yiSize<<" "<<ziSize<<endl;
	cout<<xSpace<<" "<<ySpace<<" "<<zSpace<<endl;*/
//}

void Volume::quantize(short int* pData)
{
	short int maxValue = pData[0];
	short int minValue = pData[0];
	int voxelNum = xiSize * yiSize * ziSize;
	for(int i = 0; i < voxelNum; ++i) {
		minValue = Min(minValue, pData[i]);
		maxValue = Max(maxValue, pData[i]);
	}
	cout<<"Range: ["<<minValue<<", "<<maxValue<<"]"<<endl;

	delete []pOriginalData;
	pOriginalData = NULL;
	pOriginalData = new unsigned char[xiSize * yiSize * ziSize];
	if(pOriginalData == NULL) {
		delete []pOriginalData;
		pOriginalData = NULL;
		cout<<"Out of memory "<<xiSize * yiSize * ziSize / 1024.0 / 1024.0<<" M"<<endl;
		return;
	}

	#pragma omp parallel for
	for(int i = 0; i < voxelNum; ++i)
		pOriginalData[i] = pData[i] / (double)maxValue * 255;
}

/*
 * volume data and normals
 */
void Volume::getOriginalXYZV(unsigned char* pXYZV)
{
	memset(pXYZV, 0, xiSize * yiSize * ziSize * 4 * sizeof(unsigned char));

	clock_t time1 = clock();
	// calculate gradient and the normalized direction
	#pragma omp parallel for
	for(int i = 0; i < ziSize; ++i) {
		for(int j = 0; j < yiSize; ++j) {
			for(int k = 0; k < xiSize; ++k) {
				unsigned int index = i * xiSize * yiSize + j * xiSize + k;
				// for 16 bit, already convert to 8 bit
				pXYZV[index * 4 + 3] = pOriginalData[index];
				Vector3d gradient = triCubicIntpGrad(pOriginalData, k, j, i);		// x, y, z
				if(gradient.norm() > DOUBLE_EPSILON) {
					gradient = gradient.normalize();
					pXYZV[index * 4 + 0] = (unsigned char)((gradient.x + 1.0) / 2.0 * 255 + 0.5);
					pXYZV[index * 4 + 1] = (unsigned char)((gradient.y + 1.0) / 2.0 * 255 + 0.5);
					pXYZV[index * 4 + 2] = (unsigned char)((gradient.z + 1.0) / 2.0 * 255 + 0.5);
				}
				else {	// gradient = (0, 0, 0)
					pXYZV[index * 4 + 0] = 128;
					pXYZV[index * 4 + 1] = 128;
					pXYZV[index * 4 + 2] = 128;
				}
			}
		}
	}
	cout<<"Calculate Gradient Time: "<<(clock() - time1) / (double)CLOCKS_PER_SEC<<" sec."<<endl;
}

/*
 * volume gradient magnitude and table
 */
void Volume::getGradientData(unsigned char* pG)
{
	memset(pG, 0, xiSize * yiSize * ziSize * sizeof(unsigned char));
	memset(pHistogram2D, 0, tableWidth * tableHeight * sizeof(double));

	clock_t time1 = clock();
	double maxMagnitude = 0;
	for(int i = 0; i < ziSize; ++i) {
		for(int j = 0; j < yiSize; ++j) {
			for(int k = 0; k < xiSize; ++k) {
				Vector3d gradient = calculateGradient(pOriginalData, k, j, i);
				if(gradient.norm() > maxMagnitude)
					maxMagnitude = gradient.norm();
			}
		}
	}

	for(int i = 0; i < ziSize; ++i) {
		for(int j = 0; j < yiSize; ++j) {
			for(int k = 0; k < xiSize; ++k) {
				unsigned int index = i * xiSize * yiSize + j * xiSize + k;
				Vector3d gradient = calculateGradient(pOriginalData, k, j, i);
				double magnitude = gradient.norm();
				pG[index] = magnitude / maxMagnitude * 255;
				int id = int(magnitude / maxMagnitude * (tableHeight - 1) + 0.5) * tableWidth + pOriginalData[index];
				pHistogram2D[id] += 1;
			}		
		}
	}

	maxHistogram2DValue = 0;
	for(int i = 1, num = tableWidth * tableHeight; i < num; ++i) {
		if(pHistogram2D[i] > maxHistogram2DValue)
			maxHistogram2DValue = pHistogram2D[i];
	}
	cout<<"Calculate Magnitude Time: "<<(clock() - time1) / (double)CLOCKS_PER_SEC<<" sec."<<endl;
}

/*
 *	nearest neighbor scalar value
 */
double Volume::getValue(unsigned char* pData, double x, double y, double z)
{
	x = Max(Min(x + 0.5, xiSize - 1.0), 0.0);
	y = Max(Min(y + 0.5, yiSize - 1.0), 0.0);
	z = Max(Min(z + 0.5, ziSize - 1.0), 0.0);
	int index = (int)z * xiSize * yiSize + (int)y * xiSize + (int)x;
	return pData[index];
}

/*
 *	 Trilinear interpolation, assume 0<=xIndex<xiSize 0<=yIndex<yiSize 0<=zIndex<ziSize 
 */
double Volume::intpTrilinear(unsigned char* pData, double x, double y, double z)
{
	int xIndex = (int)x, yIndex = (int)y, zIndex = (int)z;
	double xFraction = x - xIndex;
	double yFraction = y - yIndex;
	double zFraction = z - zIndex;
	if(xIndex < 0 || xIndex > xiSize - 1 || yIndex < 0 || yIndex > yiSize || zIndex < 0 || zIndex > ziSize - 1) {
		cout<<"interpolation size out of range"<<endl;
		return 0;
	}
	int index = zIndex * xiSize * yiSize + yIndex * xiSize + xIndex;
	int xNext = (xIndex < xiSize - 1) ? 1 : 0;
	int yNext = (yIndex < yiSize - 1) ? xiSize : 0;
	int zNext = (zIndex < ziSize - 1) ? xiSize * yiSize : 0;
	int f000 = pData[index];
	int f100 = pData[index + xNext];
	int f010 = pData[index + yNext];
	int f001 = pData[index + zNext];
	int f101 = pData[index + xNext + zNext];
	int f011 = pData[index + yNext + zNext];
	int f110 = pData[index + xNext + yNext];
	int f111 = pData[index + xNext + yNext + zNext];
	double fResult = f000 * (1 - xFraction) * (1 - yFraction) * (1 - zFraction) +
					 f100 * xFraction * (1 - yFraction) * (1 - zFraction) +
					 f010 * (1 - xFraction) * yFraction * (1 - zFraction) +
					 f001 * (1 - xFraction) * (1 - yFraction) * zFraction +
					 f101 * xFraction * (1 - yFraction) * zFraction +
					 f011 * (1 - xFraction) * yFraction * zFraction +
					 f110 * xFraction * yFraction * (1 - zFraction) +
					 f111 * xFraction * yFraction * zFraction;
	return fResult;
}

/*
 *  trilinear cubic BSpline scalar interpolation
 */
double Volume::cubicIntpValue(double v0, double v1, double v2, double v3, double mu)
{
	const double mu2 = mu * mu;
	const double mu3 = mu2 * mu;

	const double a0 = -1.0 * v0 + 3.0 * v1 - 3.0 * v2 + 1.0 * v3;
	const double a1 =  3.0 * v0 - 6.0 * v1 + 3.0 * v2 + 0.0 * v3;
	const double a2 = -3.0 * v0 + 0.0 * v1 + 3.0 * v2 + 0.0 * v3;
	const double a3 =  1.0 * v0 + 4.0 * v1 + 1.0 * v2 + 0.0 * v3;
			
	return (a0 * mu3 + a1 * mu2 + a2 * mu + a3) / 6.0;
}

double Volume::triCubicIntpValue(unsigned char* pData, double x, double y, double z)
{
	// Extract the integer and decimal components of the x, y co-ordinates
	double ulo, vlo, wlo;
	const double ut = modf( x, &ulo );
	const double vt = modf( y, &vlo );
	const double wt = modf( z, &wlo );

	int		xlo = int( ulo );
	int		ylo = int( vlo );
	int		zlo = int( wlo );

	// We need all the voxels around the primary
	double voxels[4][4][4];
	{
		for( int z=0; z<4; z++ ) {
			for( int y=0; y<4; y++ ) {
				for( int x=0; x<4; x++ ) {
					int px = (xlo-1+x);
					int py = (ylo-1+y);
					int pz = (zlo-1+z);
					voxels[z][y][x] =  getValue(pData, px, py, pz);
				}
			}
		}
	}

	// Now that we have all our voxels, run the cubic interpolator in one dimension to collapse it (we choose to collapase x)
	double voxelcol[4][4];
	{
		for( int z=0; z<4; z++ ) {
			for( int y=0; y<4; y++ ) {
				voxelcol[z][y] = cubicIntpValue(voxels[z][y][0], voxels[z][y][1], voxels[z][y][2], voxels[z][y][3], ut);
			}
		}
	}

	// Then collapse the y dimension 
	double voxelcol2[4];
	{
		for( int z=0; z<4; z++ ) {
			voxelcol2[z] = cubicIntpValue(voxelcol[z][0], voxelcol[z][1], voxelcol[z][2], voxelcol[z][3], vt);
		}
	}

	// The collapse the z dimension to get our value
	return cubicIntpValue(voxelcol2[0], voxelcol2[1], voxelcol2[2], voxelcol2[3], wt);
}

/*
 *	Calculate the gradient direction of the volume data using central difference
 */
Vector3d Volume::calculateGradient(unsigned char* pData, int x, int y, int z)
{
	double gx, gy, gz;
	int index = z * xiSize * yiSize + y * xiSize + x;
	int value = pData[index];

	if(x > 0)
		if(x < xiSize - 1) gx = (pData[index + 1] - pData[index - 1]) / 2.0;
		else gx = value - pData[index - 1];
	else gx = pData[index + 1] - value;

	if(y > 0)
		if(y < yiSize - 1) gy = (pData[index + xiSize] - pData[index - xiSize]) / 2.0;
		else gy = value - pData[index - xiSize];
	else gy = pData[index + xiSize] - value;
	
	if(z > 0)
		if(z < ziSize - 1) gz = (pData[index + xiSize * yiSize] - pData[index - xiSize * yiSize]) / 2.0;
		else gz = value - pData[index - xiSize * yiSize];
	else gz = pData[index + xiSize * yiSize] - value;
    
	return Vector3d(gx, gy, gz);
}

/*
 * Calculate the gradient direction of the volume data using trilinear cubic BSpline gradient interpolation
 */
double Volume::cubicIntpGrad(double v0, double v1, double v2, double v3, double mu)
{
	const double mu2 = mu*mu;
	const double mu3 = mu2*mu;

	const double a0 =  0.0 * v0 + 0.0 * v1 + 0.0 * v2 + 0.0 * v3;
	const double a1 = -1.0 * v0 + 3.0 * v1 - 3.0 * v2 + 1.0 * v3;
	const double a2 =  2.0 * v0 - 4.0 * v1 + 2.0 * v2 + 0.0 * v3;
	const double a3 = -1.0 * v0 + 0.0 * v1 + 1.0 * v2 + 0.0 * v3;

	return (a0 * mu3 + a1 * mu2 + a2 * mu + a3) / 2.0;
}

Vector3d Volume::triCubicIntpGrad(unsigned char* pData, double px, double py, double pz)
{
	// Extract the integer and decimal components of the x, y co-ordinates
	double ulo, vlo, wlo;
	const double ut = modf( px, &ulo );
	const double vt = modf( py, &vlo );
	const double wt = modf( pz, &wlo );

	int		xlo = int( ulo );
	int		ylo = int( vlo );
	int		zlo = int( wlo );

	// We need all the voxels around the primary
	double voxels[4][4][4];
	{
		for( int z=0; z<4; z++ ) {
			for( int y=0; y<4; y++ ) {
				for( int x=0; x<4; x++ ) {
					int px = (xlo-1+x);
					int py = (ylo-1+y);
					int pz = (zlo-1+z);
					voxels[z][y][x] = getValue(pData, px, py, pz );
				}
			}
		}
	}

	Vector3d direction;

	// Now that we have all our voxels, run the cubic interpolator in one dimension to collapse it (we choose to collapase x)
	double voxelcol[4][4];
	double voxelcol2[4];
	int x, y, z;
	for( z=0; z<4; z++ ) {
		for( y=0; y<4; y++ ) {
			voxelcol[z][y] = cubicIntpGrad(voxels[z][y][0], voxels[z][y][1], voxels[z][y][2], voxels[z][y][3], ut);
		}
	}
	// Then collapse the y dimension
	for( z=0; z<4; z++ ) {
		voxelcol2[z] = cubicIntpValue(voxelcol[z][0], voxelcol[z][1], voxelcol[z][2], voxelcol[z][3], vt);
	}

	// The collapse the z dimension to get our value
	direction.x = cubicIntpValue(voxelcol2[0], voxelcol2[1], voxelcol2[2], voxelcol2[3], wt);

	for( z=0; z<4; z++ ) {
		for( x=0; x<4; x++ ) {
			voxelcol[z][x] = cubicIntpGrad(voxels[z][0][x], voxels[z][1][x], voxels[z][2][x], voxels[z][3][x], vt);
		}
	}
	// Then collapse the x dimension
	for( z=0; z<4; z++ ) {
		voxelcol2[z] = cubicIntpValue(voxelcol[z][0], voxelcol[z][1], voxelcol[z][2], voxelcol[z][3], ut);
	}

	// The collapse the z dimension to get our value
	direction.y = cubicIntpValue(voxelcol2[0], voxelcol2[1], voxelcol2[2], voxelcol2[3], wt);

	for( y=0; y<4; y++ ) {
		for( x=0; x<4; x++ ) {
			voxelcol[y][x] = cubicIntpGrad(voxels[0][y][x], voxels[1][y][x], voxels[2][y][x], voxels[3][y][x], wt);
		}
	}
	// Then collapse the x dimension
	for( y=0; y<4; y++ ) {
		voxelcol2[y] = cubicIntpValue(voxelcol[y][0], voxelcol[y][1], voxelcol[y][2], voxelcol[y][3], ut);
	}

	// The collapse the z dimension to get our value
	direction.z = cubicIntpValue(voxelcol2[0], voxelcol2[1], voxelcol2[2], voxelcol2[3], vt);

	return direction;
}

Vector3d Volume::triCubicIntpGrad(unsigned char* pData, unsigned char transferTable[256][4], double px, double py, double pz)
{
	// Extract the integer and decimal components of the x, y co-ordinates
	double ulo, vlo, wlo;
	const double ut = modf( px, &ulo );
	const double vt = modf( py, &vlo );
	const double wt = modf( pz, &wlo );

	int		xlo = int( ulo );
	int		ylo = int( vlo );
	int		zlo = int( wlo );

	// We need all the voxels around the primary
	double voxels[4][4][4];
	{
		for( int z=0; z<4; z++ ) {
			for( int y=0; y<4; y++ ) {
				for( int x=0; x<4; x++ ) {
					int px = (xlo-1+x);
					int py = (ylo-1+y);
					int pz = (zlo-1+z);
					int value = (int)getValue(pData, px, py, pz);
					voxels[z][y][x] = transferTable[value][3];
				}
			}
		}
	}

	Vector3d direction;

	// Now that we have all our voxels, run the cubic interpolator in one dimension to collapse it (we choose to collapase x)
	double voxelcol[4][4];
	double voxelcol2[4];
	int x, y, z;
	for( z=0; z<4; z++ ) {
		for( y=0; y<4; y++ ) {
			voxelcol[z][y] = cubicIntpGrad(voxels[z][y][0], voxels[z][y][1], voxels[z][y][2], voxels[z][y][3], ut);
		}
	}
	// Then collapse the y dimension
	for( z=0; z<4; z++ ) {
		voxelcol2[z] = cubicIntpValue(voxelcol[z][0], voxelcol[z][1], voxelcol[z][2], voxelcol[z][3], vt);
	}

	// The collapse the z dimension to get our value
	direction.x = cubicIntpValue(voxelcol2[0], voxelcol2[1], voxelcol2[2], voxelcol2[3], wt);

	for( z=0; z<4; z++ ) {
		for( x=0; x<4; x++ ) {
			voxelcol[z][x] = cubicIntpGrad(voxels[z][0][x], voxels[z][1][x], voxels[z][2][x], voxels[z][3][x], vt);
		}
	}
	// Then collapse the x dimension
	for( z=0; z<4; z++ ) {
		voxelcol2[z] = cubicIntpValue(voxelcol[z][0], voxelcol[z][1], voxelcol[z][2], voxelcol[z][3], ut);
	}

	// The collapse the z dimension to get our value
	direction.y = cubicIntpValue(voxelcol2[0], voxelcol2[1], voxelcol2[2], voxelcol2[3], wt);

	for( y=0; y<4; y++ ) {
		for( x=0; x<4; x++ ) {
			voxelcol[y][x] = cubicIntpGrad(voxels[0][y][x], voxels[1][y][x], voxels[2][y][x], voxels[3][y][x], wt);
		}
	}
	// Then collapse the x dimension
	for( y=0; y<4; y++ ) {
		voxelcol2[y] = cubicIntpValue(voxelcol[y][0], voxelcol[y][1], voxelcol[y][2], voxelcol[y][3], ut);
	}

	// The collapse the z dimension to get our value
	direction.z = cubicIntpValue(voxelcol2[0], voxelcol2[1], voxelcol2[2], voxelcol2[3], vt);

	return direction;
}

/*
 * calculate the isosurface statistic
 */
void Volume::calcIsoStat(unsigned char* pData)
{
	memset(isoStat, 0, sizeof(double) * 256);
	//for(int i = 0; i < ziSize; ++i) {
	//	for(int j = 0; j < yiSize; ++j) {
	//		for(int k = 0; k < xiSize; ++k) {
	//			int index = i * xiSize * yiSize + j * xiSize + k;
	//			int value = pData[index];
	//			isoStat[value] += 1.0;
	//		}
	//	}
	//}

	for(int i = 0; i < ziSize; ++i) {
		int zNext = (i < ziSize - 1) ? xiSize * yiSize : 0;
		for(int j = 0; j < yiSize; ++j) {
			int yNext = (j < yiSize - 1) ? xiSize : 0;
			for(int k = 0; k < xiSize; ++k) {
				int index = i * xiSize * yiSize + j * xiSize + k;
				int xNext = (k < xiSize - 1) ? 1 : 0;
				int f000 = pData[index];
				int f100 = pData[index + xNext];
				int f010 = pData[index + yNext];
				int f001 = pData[index + zNext];
				int f101 = pData[index + xNext + zNext];
				int f011 = pData[index + yNext + zNext];
				int f110 = pData[index + xNext + yNext];
				int f111 = pData[index + xNext + yNext + zNext];
				int minValue = Min(f000, f100, Min(f010, f001, Min(f101, f011, Min(f110, f111, 255))));
				int maxValue = Max(f000, f100, Max(f010, f001, Max(f101, f011, Max(f110, f111, 0))));
				for(int m = minValue; m <= maxValue; ++m)
					isoStat[m] += 1.0;
			}
		}
	}

	maxIsoStatValue = isoStat[1];
	for(int i = 2; i < 256; ++i)
		maxIsoStatValue = Max(maxIsoStatValue, isoStat[i]);	
}


