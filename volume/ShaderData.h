#ifndef _SHADERDATA_H_
#define _SHADERDATA_H_

class ShaderData
{
public:
	ShaderData() {}
	~ShaderData() {}

	// volume texture idx
	virtual unsigned int getVolumeTexIdx()             = 0;
	virtual unsigned int getMagnitudeTexIdx()		   = 0;

	// voxel size
	virtual Vector3d     getVoxelSize()                = 0;

	// ray casting start and end position texture idx
	virtual unsigned int getStartPosTexIdx()           = 0;
	virtual unsigned int getEndPosTexIdx()             = 0;
	
	// ray casting step
	virtual float        getRayStep()                  = 0;

	// transfer function idx
	virtual unsigned int getTF1DIdx()                  = 0;
	virtual unsigned int getTF2DIdx()				   = 0;

	// camera parameters
	virtual Vector3d     getCameraEye()                = 0;
	virtual Vector3d     getCameraTowards()            = 0;
	virtual Vector3d     getCameraUp()                 = 0;
	virtual Vector3d     getCameraRight()              = 0;

	// lighting parameters
	virtual Vector3d     getLightDirection()           = 0;
	virtual float        getAmbient()                  = 0;
	virtual float        getDiffuse()                  = 0;
	virtual float        getShininess()                = 0;
	virtual float        getSpecular()                 = 0;

	// mouse position
	virtual unsigned int getMouseX()                   = 0;
	virtual unsigned int getMouseY()                   = 0;

	// volume boundary
	virtual Vector3d     getVolumeBound()              = 0;
};

#endif