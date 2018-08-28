#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

class ShaderDataInterface;
class ShaderProgram :public QOpenGLShaderProgram
{
public:
    ShaderProgram();
	virtual void load(const ShaderDataInterface * data) = 0;
	void setUniformSampler(const std::string &name, GLenum texUnit, GLenum target, GLuint texID);
private:
	QOpenGLFunctions m_glfuncs;
};

class PositionShader:public ShaderProgram
{
public:
	PositionShader();
	virtual void load(const ShaderDataInterface * data)override;
};

class RayCastingShader:public ShaderProgram
{
public:
	RayCastingShader();
	virtual void load(const ShaderDataInterface * data)override;
};


#endif // SHADERPROGRAM_H