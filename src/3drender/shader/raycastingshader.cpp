#include "raycastingshader.h"
#include "3drender/shader/shaderdatainterface.h"
#include "abstract/abstractslicedatamodel.h"


static const char *vertexShaderSource =
"#version 150\n"
"#extension GL_ARB_explicit_attrib_location : enable\n"
"layout(location = 0) in vec4 vertex;\n"
"layout(location = 1) in vec3 normal;\n"
"layout(location = 2) in vec2 tex;"
"out vec2 texCoord;\n"
"out vec3 vert;\n"
"out vec3 vertNormal;\n"
"uniform mat4 projMatrix;\n"
"uniform mat4 mvMatrix;\n"
"uniform mat3 normalMatrix;\n"
"void main() {\n"
"   vert = vertex.xyz;\n"
"   vertNormal = normalMatrix * normal;\n"
"	texCoord = tex;\n"
"   gl_Position = projMatrix * mvMatrix * vertex;\n"
"}\n";


static const char *fragmentShaderSource =
"#version 150\n"
"in highp vec3 vert;\n"
"in highp vec3 vertNormal;\n"
"out highp vec4 fragColor;\n"
"in vec2 texCoord;\n"
"uniform highp vec3 lightPos;\n"
"uniform sampler2D tex;\n"
"void main() {\n"
"   highp vec3 L = normalize(lightPos - vert);\n"
"   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
"   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"
"   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
"   fragColor = vec4(1.0,0.0,0.0,1.0);\n"
"}\n";


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
"out vec4 texCoord;\n"
"uniform mat4 projMatrix;\n"
"uniform mat4 othoMatrix;\n"
"uniform mat4 worldMatrix;\n"
"uniform mat4 viewMatrix;\n"
"void main() {\n"
"   vec4 vert = vec4(vertex,1.0);\n"
"	texCoord = vert;\n"
"   gl_Position = othoMatrix * viewMatrix * worldMatrix * vert;\n"
"}\n";

RayCastingShader::RayCastingShader() :ShaderProgram()
{
	addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/resources/shaders/raycast_on_the_fly_gradient_v_glsl");
    addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/resources/shaders/raycast_on_the_fly_gradient_f.glsl");
	link();
}
void RayCastingShader::load(const ShaderDataInterface* data)
{
	this->bind();
	QVector3D L = data->lightDirection();
	QVector3D H = L - data->cameraTowards();
	if (H.length() > 1e-10) H.normalize();

	const auto w = data->windowSize().width();
	const auto h = data->windowSize().height();

	QMatrix4x4 otho;
	otho.setToIdentity();
	otho.ortho(0, w, 0, h, -10, 100);

	this->setUniformValue("othoMatrix", otho);

	this->setUniformSampler("texVolume", GL_TEXTURE3, GL_TEXTURE_3D, data->volumeTexId());
    this->setUniformSampler("texStartPos", GL_TEXTURE0, GL_TEXTURE_RECTANGLE, data->startPosTexIdx());

    this->setUniformSampler("texEndPos", GL_TEXTURE1, GL_TEXTURE_RECTANGLE, data->endPosTexIdx());
	this->setUniformSampler("texTransfunc", GL_TEXTURE2, GL_TEXTURE_1D, data->transferFunctionsTexId());

	this->setUniformSampler("texGradient", GL_TEXTURE4, GL_TEXTURE_3D, data->gradientTexId());

	this->setUniformValue("viewMatrix", data->viewMatrix());
	this->setUniformValue("step", data->rayStep());
	this->setUniformValue("ka", data->ambient());
	this->setUniformValue("ks", data->specular());
	this->setUniformValue("kd", data->diffuse());
	this->setUniformValue("shininess", data->shininess());
	this->setUniformValue("lightdir", (float)L.x(), (float)L.y(), (float)L.z());
	this->setUniformValue("halfway", (float)H.x(), (float)H.y(), (float)H.z());
}


RayCastingModuloShader::RayCastingModuloShader() :ShaderProgram()
{
	addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/resources/shaders/raycast_gradient_modulo_based_v_glsl");
	addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/resources/shaders/raycast_gradient_modulo_based_f.glsl");
	link();
}
void RayCastingModuloShader::load(const ShaderDataInterface* data)
{
	this->bind();
	auto L = data->lightDirection();
	auto H = L - data->cameraTowards();
	if (H.length() > 1e-10) H.normalize();

	const auto w = data->windowSize().width();
	const auto h = data->windowSize().height();

	QMatrix4x4 otho;
	otho.setToIdentity();
	otho.ortho(0, w, 0, h, -10, 100);

	this->setUniformValue("othoMatrix", otho);
	this->setUniformSampler("texVolume", GL_TEXTURE3, GL_TEXTURE_3D, data->volumeTexId());
	this->setUniformSampler("texStartPos", GL_TEXTURE0, GL_TEXTURE_RECTANGLE, data->startPosTexIdx());
	this->setUniformSampler("texEndPos", GL_TEXTURE1, GL_TEXTURE_RECTANGLE, data->endPosTexIdx());
	this->setUniformSampler("texTransfunc", GL_TEXTURE2, GL_TEXTURE_1D, data->transferFunctionsTexId());
	this->setUniformSampler("texGradient", GL_TEXTURE4, GL_TEXTURE_3D, data->gradientTexId());

	this->setUniformValue("viewMatrix", data->viewMatrix());
	this->setUniformValue("step", data->rayStep());
	this->setUniformValue("ka", data->ambient());
	this->setUniformValue("ks", data->specular());
	this->setUniformValue("kd", data->diffuse());
	this->setUniformValue("shininess", data->shininess());
	this->setUniformValue("lightdir", float(L.x()), float(L.y()), float(L.z()));
	this->setUniformValue("halfway", float(H.x()), float(H.y()), float(H.z()));
}


PositionShader::PositionShader() :ShaderProgram()
{
	addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/resources/shaders/position_v.glsl");
	addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/resources/shaders/position_f.glsl");
	link();
}

void PositionShader::load(const ShaderDataInterface* data)
{
	this->bind();
	this->setUniformValue("projMatrix", data->perspMatrix());
	this->setUniformValue("worldMatrix", data->worldMatrix());
	this->setUniformValue("viewMatrix", data->viewMatrix());
}

SliceShader::SliceShader() {
	addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/resources/shaders/slice_shader_v.glsl");
	addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/resources/shaders/slice_shader_f.glsl");
	link();
}
void SliceShader::load(const ShaderDataInterface* data) {
	this->bind();
	this->setUniformSampler("volume", GL_TEXTURE0, GL_TEXTURE_3D, data->volumeTexId());
	this->setUniformValue("projMatrix", data->perspMatrix());
	this->setUniformValue("viewMatrix", data->viewMatrix());
	this->setUniformValue("worldMatrix", data->worldMatrix());
}
