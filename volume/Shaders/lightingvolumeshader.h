#ifndef LIGHTINGVOLUMESHADER_H
#define LIGHTINGVOLUMESHADER_H

#include "shaderprogram.h"

class LightingVolumeShader:public ShaderProgram
{
public:
    LightingVolumeShader(){}
	void load(const ShaderDataInterface * data)override;
};

#endif // LIGHTINGVOLUMESHADER_H