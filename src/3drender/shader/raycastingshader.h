#ifndef RAYCASTINGSHADER_H
#define RAYCASTINGSHADER_H

#include "3drender/shader/shaderprogram.h"
class AbstractSliceDataModel;
class RayCastingShader :public ShaderProgram
{
public:
	RayCastingShader();
	void load(const ShaderDataInterface * data)override;
};

class RayCastingModuloShader :public ShaderProgram
{
public:
	RayCastingModuloShader();
	void load(const ShaderDataInterface * data)override;
};

class PositionShader :public ShaderProgram
{
public:
	PositionShader();
	void load(const ShaderDataInterface * data)override;
};

class SliceShader:public ShaderProgram {

public:
	SliceShader();
	void load(const ShaderDataInterface * data)override;
};


#endif // RAYCASTINGSHADER_H
