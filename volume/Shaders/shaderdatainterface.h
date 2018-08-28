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
	virtual unsigned int getVolumeTexIdx()const = 0;
	//virtual unsigned int getMagnitudeTexIdx() = 0;

	// voxel size
	virtual QVector3D     getVoxelSize()const = 0;

	// ray casting start and end position texture idx
	virtual unsigned int getStartPosTexIdx()const = 0;
	virtual unsigned int getEndPosTexIdx()const = 0;

	// ray casting step
	virtual float        getRayStep()const = 0;

	// transfer function idx
	virtual unsigned int getTF1DIdx()const = 0;

	// camera parameters
	virtual QVector3D     getCameraEye()const = 0;
	virtual QVector3D     getCameraTowards()const = 0;
	virtual QVector3D     getCameraUp()const = 0;
	virtual QVector3D     getCameraRight()const = 0;

	// lighting parameters
	virtual QVector3D     getLightDirection()const = 0;
	virtual float        getAmbient()const = 0;
	virtual float        getDiffuse()const = 0;
	virtual float        getShininess()const = 0;
	virtual float        getSpecular()const = 0;

	// mouse position
	//virtual unsigned int getMouseX() = 0;
	//virtual unsigned int getMouseY() = 0;

	// volume boundary
	virtual QVector3D     getVolumeBound() = 0;
};


#endif // SHADERDATAINTERFACE_H
