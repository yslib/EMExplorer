#include "shaderprogram.h"
#include "shaderdatainterface.h"



ShaderProgram::ShaderProgram():QOpenGLShaderProgram()
{
	m_glfuncs.initializeOpenGLFunctions();
}

void ShaderProgram::setUniformSampler(const std::string& name, GLenum texUnit, GLenum target, GLuint texID)
{
	GLint l = uniformLocation(name.c_str());
	if (l == -1)
		return;
	m_glfuncs.glActiveTexture(texUnit);		
	m_glfuncs.glBindTexture(target, texID);
	m_glfuncs.glUniform1i(l, texUnit - GL_TEXTURE0); 
}

PositionShader::PositionShader():ShaderProgram()
{

	static const char * positionFragShaderSource =
		"#version 150\n"
		"in vec4 texCoord;\n"
		"out vec4 fragColor;\n"
		"void main(){\n"
		" fragColor = texCoord;\n"
		"}\n";

	static const char * positionVertShaderSource =
		"#version 150\n"
		"in vec3 vertex;\n"
		"in vec3 tex;\n"
		"out vec4 texCoord;\n"
		"uniform mat4 projMatrix;\n"
		"uniform mat4 othoMatrix;\n"
		"uniform mat4 worldMatrix;\n"
		"uniform mat4 viewMatrix;\n"
		"void main() {\n"
		"	texCoord = vec4(tex,1.0);\n"
		"   gl_Position = othoMatrix * viewMatrix * worldMatrix * vec4(vertex,1.0);\n"
		"}\n";

	addShaderFromSourceCode(QOpenGLShader::Fragment, positionFragShaderSource);
	addShaderFromSourceCode(QOpenGLShader::Vertex, positionVertShaderSource);
	bindAttributeLocation("vertex", 0);
	bindAttributeLocation("tex", 1);
}

void PositionShader::load(const ShaderDataInterface* data)
{
	this->bind();
	this->setUniformValue("projMatrix",data->getPerspMatrix());
	this->setUniformValue("othoMatrix",data->getOthoMatrix());
	this->setUniformValue("worldMatrix",data->getModelMatrix());
	this->setUniformValue("viewMatrix",data->getViewMatrix());
}

RayCastingShader::RayCastingShader() :ShaderProgram()
{

	addShaderFromSourceFile(QOpenGLShader::Fragment, "D:\\code\\MRCEditor\\volume\\Shaders\\raycast_lighting.frag");
	static const char * raycastingShaderSource=
		"#version 150\n"
		"in vec2 vertex;\n"
		"in vec2 textureRect;\n"
		"out vec2 textureRectCoord;\n"
		"void main() {\n"
		"	textureRectCoord = textureRect;\n"
		"   gl_Position = vec4(vertex.x,vertex.y,0.0,1.0);\n"
		"}\n";
	addShaderFromSourceCode(QOpenGLShader::Vertex, raycastingShaderSource);
	bindAttributeLocation("vertex", 0);
	bindAttributeLocation("textureRect", 1);
}

void RayCastingShader::load(const ShaderDataInterface* data)
{
	this->bind();
	QVector3D L = data->getLightDirection();
	QVector3D H = L - data->getCameraTowards();
	if (H.lengthSquared() > 1e-10) H.normalize();
	this->setUniformSampler("texVolume", GL_TEXTURE3, GL_TEXTURE_3D, data->getVolumeTexIdx());
	this->setUniformSampler("texStartPos", GL_TEXTURE0, GL_TEXTURE_RECTANGLE_ARB, data->getStartPosTexIdx());
	this->setUniformSampler("texEndPos", GL_TEXTURE1, GL_TEXTURE_RECTANGLE_ARB, data->getEndPosTexIdx());
	this->setUniformSampler("texTransfunc", GL_TEXTURE2, GL_TEXTURE_1D, data->getTF1DIdx());
	this->setUniformValue("step", data->getRayStep());
	this->setUniformValue("ka", data->getAmbient());
	this->setUniformValue("ks", data->getSpecular());
	this->setUniformValue("kd", data->getDiffuse());
	this->setUniformValue("shininess", data->getShininess());
	this->setUniformValue("lightdir", (float)L.x(), (float)L.y(), (float)L.z());
	this->setUniformValue("halfway", (float)H.x(), (float)H.y(), (float)H.z());
}
