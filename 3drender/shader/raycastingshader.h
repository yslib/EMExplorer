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


class PositionShader :public ShaderProgram
{
public:
	PositionShader();
	void load(const ShaderDataInterface * data)override;
};

class SliceShader:public ShaderProgram {
	const AbstractSliceDataModel * m_dataModel;
	int m_sliceType;
	int m_sliceIndex;
public:
	SliceShader();
	void load(const ShaderDataInterface * data)override;
	void setSliceDataModel(const AbstractSliceDataModel * model);
	void setSliceType(int type);
	void setSliceIndex(int index);
};



#endif // RAYCASTINGSHADER_H
