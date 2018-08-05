#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <QOpenGLShaderProgram>

class ShaderDataInterface;
class ShaderProgram:public QOpenGLShaderProgram
{
public:
    ShaderProgram();
	virtual void load(const ShaderDataInterface * data) = 0;
};

#endif // SHADERPROGRAM_H