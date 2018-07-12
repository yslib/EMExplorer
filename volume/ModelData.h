#ifndef _MODELDATA_H_
#define _MODELDATA_H_

#include <string>
using namespace std;

class Volume;

class ModelData
{
public:
	ModelData() {}
	~ModelData() {}

	// volume
	virtual Volume* getVolume() = 0;

	// volume name
	virtual string getVolumeName() = 0;
	
	// 1D transfer function
	virtual void         getTransferFunction(float* transferFunction, size_t dimension, float factor) = 0;
	virtual unsigned int getTF1DTextureIdx() = 0;

	// 2D transfer function
	virtual void getTransferFunction(float* transferFunction, size_t width, size_t height, float factor) = 0;
};

#endif