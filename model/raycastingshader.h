#ifndef RAYCASTINGSHADER_H
#define RAYCASTINGSHADER_H

#include "model/shaderprogram.h"

class RayCastingShader :public ShaderProgram
{
public:
	RayCastingShader();
	void load(const ShaderDataInterface * data)override;
};


class PositionShader :public ShaderProgram
{
public:
	PositionShader();
	void load(const ShaderDataInterface * data)override;
};



#endif // RAYCASTINGSHADER_H
