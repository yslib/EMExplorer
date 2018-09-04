#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_1>
#include <iostream>

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

#endif // SHADERPROGRAM_H