#include "raycastingshader.h"
#include "abstract/shaderdatainterface.h"
#include "abstract/abstractslicedatamodel.h"

RayCastingShader::RayCastingShader() :ShaderProgram()
{
	addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/resources/shaders/raycast_pre_gradient_v.glsl");
    addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/resources/shaders/raycast_pre_gradient_f.glsl");
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
	this->setUniformSampler("texStartPos", GL_TEXTURE0, GL_TEXTURE_RECTANGLE_NV, data->startPosTexIdx());
	this->setUniformSampler("texEndPos", GL_TEXTURE1, GL_TEXTURE_RECTANGLE_NV, data->endPosTexIdx());
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

SliceShader::SliceShader() :m_sliceType(0),m_sliceIndex(0),m_dataModel(nullptr){
	addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/resources/shaders/slice_shader_v.glsl");
	addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/resources/shaders/slice_shader_f.glsl");
	link();
}
void SliceShader::load(const ShaderDataInterface* data) {
	if (m_dataModel == nullptr)
		return;
	this->bind();
	this->setUniformValue("sliceType", m_sliceType);
	int count = 1;
	if(m_sliceType == 0) {
		count = m_dataModel->topSliceCount();
	}else if(m_sliceType == 1) {
		count = m_dataModel->rightSliceCount();
	}else if(m_sliceType == 2) {
		count = m_dataModel->frontSliceCount();
	}
	this->setUniformValue("sliceIndex",m_sliceIndex);
	this->setUniformValue("sliceCount", count);
	this->setUniformSampler("volume", GL_TEXTURE0, GL_TEXTURE_3D, data->volumeTexId());
}

void SliceShader::setSliceDataModel(const AbstractSliceDataModel* model) {
	m_dataModel = model;
}

void SliceShader::setSliceType(int type) {
	m_sliceType = type;
}

void SliceShader::setSliceIndex(int index) {
	m_sliceIndex = index;
}
