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
	static const char * frag = 
		"#version 330\n"
		"in vec3 vertCoord;\n"
		"void main(){\n"
		" gl_FragColor = vertCoord;\n"
		"}\n";
	addShaderFromSourceCode(QOpenGLShader::Fragment, frag);
}

void PositionShader::load(const ShaderDataInterface* data)
{

}

RayCastingShader::RayCastingShader() :ShaderProgram()
{
	addShaderFromSourceFile(QOpenGLShader::Fragment, "D:\\code\\MRCEditor\\volume\\Shaders\\raycast_lighting.frag");
	link();
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
