#ifndef SHADERDATAINTERFACE_H
#define SHADERDATAINTERFACE_H

#include <QVector3D>
#include <QMatrix4x4>

class ShaderDataInterface
{
public:
	//ShaderDataInterface() {}
	//virtual ~ShaderDataInterface() {}

	// volume texture idx
	virtual unsigned int getVolumeTexIdx() = 0;
	//virtual unsigned int getMagnitudeTexIdx() = 0;

	// voxel size
	virtual QVector3D     getVoxelSize() = 0;

	// ray casting start and end position texture idx
	virtual unsigned int getStartPosTexIdx() = 0;
	virtual unsigned int getEndPosTexIdx() = 0;

	// ray casting step
	virtual float        getRayStep() = 0;

	// transfer function idx
	virtual unsigned int getTF1DIdx() = 0;

	// camera parameters
	virtual QVector3D     getCameraEye() = 0;
	virtual QVector3D     getCameraTowards() = 0;
	virtual QVector3D     getCameraUp() = 0;
	virtual QVector3D     getCameraRight() = 0;

	// lighting parameters
	virtual QVector3D     getLightDirection() = 0;
	virtual float        getAmbient() = 0;
	virtual float        getDiffuse() = 0;
	virtual float        getShininess() = 0;
	virtual float        getSpecular() = 0;

	// mouse position
	//virtual unsigned int getMouseX() = 0;
	//virtual unsigned int getMouseY() = 0;

	// volume boundary
	virtual QVector3D     getVolumeBound() = 0;
};


#endif // SHADERDATAINTERFACE_H
