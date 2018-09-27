#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <iostream>
#include <QOpenGLFunctions_3_3_Core>

class ShaderDataInterface;
class ShaderProgram :public QOpenGLShaderProgram
{
public:
    ShaderProgram();
	virtual void load(const ShaderDataInterface * data) = 0;
	void setUniformSampler(const std::string &name, GLenum texUnit, GLenum target, GLuint texID);
private:
	QOpenGLFunctions_3_3_Core m_glfuncs;
};



#endif // SHADERPROGRAM_H