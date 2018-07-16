#ifndef _VOLUME_H_
#define _VOLUME_H_

#include "Rendering/Vector.h"

class Volume
{
public:
	typedef enum {
		UINT8,
		INT8,
		UINT12,
		INT12,
		UINT16,
		INT16,
		UINT32,  // For some DICOM files (RT or SC)
		INT32,   //                        "   "
		FLOAT16, // sure why not...
		FLOAT32, // good ol' 'float'
		FLOAT64, // aka 'double'
		SINGLEBIT, // bool / monochrome
		UNKNOWN // aka BitsAllocated == 0 && PixelRepresentation == 0}
	} VoxelFormat;

public:
	// Construction / Destruction
	Volume();
	~Volume();

	// load raw data
	bool loadRawData(const char* filename);

	// lad dicom data
	bool loadDICOMData(const char* filename);

	// Slices number
	inline int    getXiSize()  { return xiSize;  }
	inline int    getYiSize()  { return yiSize;  }
	inline int    getZiSize()  { return ziSize;  }
	inline int    getXpSize() { return xpSize; }
	inline int    getYpSize() { return ypSize; }
	inline int    getZpSize() { return zpSize; }
	inline double getXSpace()  { return xSpace;  }
	inline double getYSpace()  { return ySpace;  }
	inline double getZSpace()  { return zSpace;  }
	inline double getXfSize()  { return xfSize;  }
	inline double getYfSize()  { return yfSize;  }
	inline double getZfSize()  { return zfSize;  }

	// Isosurface statistic
	inline int     getBucketCount()      { return 256;             }
	inline double  getMaxIsoValue()      { return maxIsoStatValue; }
	inline double* getIsosurfaceStat()   { return isoStat;         }

	// intensity and magnitude statistic
	inline int     getVGWidth()    { return tableWidth;          }
	inline int     getVGHeight()   { return tableHeight;         }
	inline double* getVGTable()    { return pHistogram2D;        }
	inline double  getMaxVGValue() { return maxHistogram2DValue; }

	// volume data and normals
	void getOriginalXYZV(unsigned char* pXYZV);	
	
	// gradient magnitude data
	void getGradientData(unsigned char* pG);

protected:
	// get the power of 2 greater than or equal to size
	int getPow2(int size);

	// get the voxel size in bytes from the voxer format
	int getVoxelSize(VoxelFormat voxerFormat);

	// read DICOM information
	//void readDICOMInfo(std::vector<std::string> const &filenames);

	void quantize(short int* pData);

	// nearest neighbor scalar value
	double getValue(unsigned char* pData, double x, double y, double z);

	// trilinear interpolation
	double intpTrilinear(unsigned char* pData, double x, double y, double z);

	// trilinear cubic bspline scalar interpolation
	double cubicIntpValue(double v0, double v1, double v2, double v3, double mu);
	double triCubicIntpValue(unsigned char* pData, double x, double y, double z);

	// calculate gradient
	Vector3d calculateGradient(unsigned char* pData, int x, int y, int z);

	// triline cubic bspline gradient interpolation
	double cubicIntpGrad(double v0, double v1, double v2, double v3, double mu);
	Vector3d triCubicIntpGrad(unsigned char* pData, double px, double py, double pz);
	Vector3d triCubicIntpGrad(unsigned char* pData, unsigned char transferTable[256][4], double px, double py, double pz);

	// calculate the isosurface statistic
	void calcIsoStat(unsigned char* pData);

private:
	VoxelFormat voxelFormat;

	unsigned char* pOriginalData;		// the original data

	int xiSize, yiSize, ziSize;			// size of the original volume
	int xpSize, ypSize, zpSize;         // size of the padding volume
    double xSpace, ySpace, zSpace;		// spacing size of the original volume 
	double xfSize, yfSize, zfSize;		// size of the original volume

	double isoStat[256];				// isosurface statistic
	double maxIsoStatValue;				// the max isosurface value in range [1, maxValue]

	double* pHistogram2D;				// intensity and gradient magnitude table
	double  maxHistogram2DValue;		// the max value of the histogram 2D
	int tableWidth, tableHeight;		// the width and height of the histogram 2D
};

#endif
