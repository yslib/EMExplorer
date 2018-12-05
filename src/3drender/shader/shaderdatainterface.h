#ifndef SHADERDATAINTERFACE_H
#define SHADERDATAINTERFACE_H

#include <QVector3D>
#include <QMatrix4x4>
#include "base/geometry.h"
#include "base/transformation.h"

class ShaderDataInterface
{
public:
	//ShaderDataInterface() {}
	//virtual ~ShaderDataInterface() {}

	// volume texture idx
	virtual unsigned int volumeTexId()const = 0;
	//virtual unsigned int getMagnitudeTexIdx() = 0;

	// voxel size
	virtual QVector3D    voxelSize()const = 0;

	// ray casting start and end position texture idx
	virtual unsigned int startPosTexIdx()const = 0;
	virtual unsigned int endPosTexIdx()const = 0;
	virtual unsigned int gradientTexId()const = 0;

	// ray casting step
	virtual float        rayStep()const = 0;

	// transfer function idx
	virtual unsigned int transferFunctionsTexId()const = 0;

	// camera parameters
	virtual ysl::Point3f  cameraPos() const = 0;
	virtual ysl::Vector3f cameraTowards() const = 0;
	virtual ysl::Vector3f cameraUp() const = 0;
	virtual ysl::Vector3f cameraRight() const = 0;

	// matrix parameters
	virtual ysl::Transform viewMatrix() const = 0;
	virtual ysl::Transform worldMatrix() const = 0;
	virtual ysl::Transform othoMatrix() const = 0;
	virtual ysl::Transform perspMatrix() const = 0;

	// lighting parameters
	virtual ysl::Vector3f lightDirection() const = 0;
	virtual float         ambient()const = 0;
	virtual float         diffuse()const = 0;
	virtual float         shininess()const = 0;
	virtual float         specular()const = 0;

	// mouse position
	//virtual unsigned int getMouseX() = 0;
	//virtual unsigned int getMouseY() = 0;

	// volume boundary
	virtual QVector3D     volumeBound()const = 0;
	virtual QSize		  windowSize()const = 0;
};


#endif // SHADERDATAINTERFACE_H
