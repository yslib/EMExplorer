#include "raycastingshader.h"
#include "abstract/shaderdatainterface.h"

RayCastingShader::RayCastingShader() :ShaderProgram()
{

    addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/resources/shaders/raycast_lighting.frag");
	static const char * raycastingShaderSource =
		"#version 150\n"
		"#extension GL_ARB_explicit_attrib_location : enable\n"
		"layout (location = 0 ) in vec2 vertex;\n"
		"layout (location = 1 ) in vec2 tex;\n"
		"out vec2 textureRectCoord;\n"
		"uniform mat4 othoMatrix;\n"
		"uniform mat4 viewMatrix;\n"
		"void main() {\n"
		"	textureRectCoord = tex;\n"
		"   gl_Position = othoMatrix*vec4(vertex.x,vertex.y,0.0,1.0);\n"
		"}\n";
	addShaderFromSourceCode(QOpenGLShader::Vertex, raycastingShaderSource);
}

void RayCastingShader::load(const ShaderDataInterface* data)
{
	this->bind();
	QVector3D L = data->getLightDirection();
	QVector3D H = L - data->getCameraTowards();
	if (H.length() > 1e-10) H.normalize();

	const auto w = data->windowSize().width();
	const auto h = data->windowSize().height();

	QMatrix4x4 otho;
	otho.setToIdentity();
	otho.ortho(0, w, 0, h, -10, 100);
	this->setUniformValue("othoMatrix", otho);

	this->setUniformSampler("texVolume", GL_TEXTURE3, GL_TEXTURE_3D, data->getVolumeTexIdx());
	this->setUniformSampler("texStartPos", GL_TEXTURE0, GL_TEXTURE_RECTANGLE_NV, data->getStartPosTexIdx());
	this->setUniformSampler("texEndPos", GL_TEXTURE1, GL_TEXTURE_RECTANGLE_NV, data->getEndPosTexIdx());
	this->setUniformSampler("texTransfunc", GL_TEXTURE2, GL_TEXTURE_1D, data->getTF1DIdx());
	this->setUniformValue("viewMatrix", data->getViewMatrix());
	this->setUniformValue("step", data->getRayStep());
	this->setUniformValue("ka", data->getAmbient());
	this->setUniformValue("ks", data->getSpecular());
	this->setUniformValue("kd", data->getDiffuse());
	this->setUniformValue("shininess", data->getShininess());
	this->setUniformValue("lightdir", (float)L.x(), (float)L.y(), (float)L.z());
	this->setUniformValue("halfway", (float)H.x(), (float)H.y(), (float)H.z());
}



PositionShader::PositionShader() :ShaderProgram()
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
		"#extension GL_ARB_explicit_attrib_location : enable\n"
		"layout (location = 0) in vec3 vertex;\n"
		"layout (location = 1) in vec3 tex;\n"
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

}

void PositionShader::load(const ShaderDataInterface* data)
{
	this->bind();
	this->setUniformValue("projMatrix", data->getPerspMatrix());
	this->setUniformValue("othoMatrix", data->getOthoMatrix());
	this->setUniformValue("worldMatrix", data->getModelMatrix());
	this->setUniformValue("viewMatrix", data->getViewMatrix());
}
