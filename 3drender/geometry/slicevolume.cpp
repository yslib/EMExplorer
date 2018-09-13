#include <QOpenGLFramebufferObject>

#include "slicevolume.h"
#include "abstract/abstractslicedatamodel.h"

const static int faceIndex[] = { 1, 3, 7, 5, 0, 4, 6, 2, 2, 6, 7, 3, 0, 1, 5, 4, 4, 5, 7, 6, 0, 2, 3, 1 };
static const float xCoord = 1.0, yCoord = 1.0, zCoord = 1.0;
static float positionVert[] = {
	0,0,0,
	xCoord, 0, 0 ,
	0, yCoord, 0 ,
	xCoord, yCoord, 0 ,
	0, 0, zCoord ,
	xCoord, 0, zCoord ,
	0, yCoord, zCoord ,
	xCoord, yCoord, zCoord ,
};


void SliceVolume::loadDataAndGradientToTexture() {

	if (m_gradCalc.hasData() == false)
		return;
	if (m_gradCalc.ready() == false)
		m_gradCalc.calcGradent();		//Time-consuming

	const auto d = m_gradCalc.data3();
	const auto z = zLength();
	const auto y = yLength();
	const auto x = xLength();

	//auto fmt = format();
	std::cout << "SliceVolume::loadDataAndGradientToTexture" << std::endl;

	m_gradientTexture.destroy();
	m_gradientTexture.setMagnificationFilter(QOpenGLTexture::Linear);
	m_gradientTexture.setMinificationFilter(QOpenGLTexture::Linear);
	m_gradientTexture.setWrapMode(QOpenGLTexture::ClampToEdge);
	m_gradientTexture.setSize(x, y, z);

	m_gradientTexture.setFormat(QOpenGLTexture::RGB8_UNorm);


	m_gradientTexture.allocateStorage();
	m_gradientTexture.setData(QOpenGLTexture::RGB, QOpenGLTexture::UInt8, d);

	m_volumeTexture.destroy();
	m_volumeTexture.setMagnificationFilter(QOpenGLTexture::Linear);
	m_volumeTexture.setMinificationFilter(QOpenGLTexture::Linear);
	m_volumeTexture.setWrapMode(QOpenGLTexture::ClampToEdge);
	m_volumeTexture.setSize(x, y, z);
	m_volumeTexture.setFormat(QOpenGLTexture::R16F);
	m_volumeTexture.allocateStorage();
	m_volumeTexture.setData(QOpenGLTexture::Red, QOpenGLTexture::UInt8, data());

	std::cout << "SliceVolume::loadDataAndGradientToTexture--2" << std::endl;
}

unsigned SliceVolume::volumeTexId() const {
	return m_volumeTexture.textureId();
}
QVector3D SliceVolume::voxelSize() const {
	Q_ASSERT_X(m_dataModel, "SliceVolume::voxelSize", "null pointer");
	return QVector3D(m_dataModel->frontSliceCount(), m_dataModel->rightSliceCount(), m_dataModel->topSliceCount());
}
unsigned SliceVolume::startPosTexIdx() const {
	return m_fbo->textures()[0];
}
unsigned SliceVolume::endPosTexIdx() const {
	return m_fbo->textures()[1];
}
unsigned SliceVolume::gradientTexId() const {
	return m_gradientTexture.textureId();
}
float SliceVolume::rayStep() const {
	Q_ASSERT_X(m_renderer, "SliceVolume::rayStep", "null pointer");
	return m_renderer->m_rayStep;
}
unsigned SliceVolume::transferFunctionsTexId() const {
	Q_ASSERT_X(m_renderer, "SliceVolume::transferFunctionsTexId", "null pointer");
	return m_renderer->m_tfTexture.textureId();
}
QVector3D SliceVolume::cameraPos() const {
	Q_ASSERT_X(m_renderer, "SliceVolume::cameraPos", "null pointer");
	const auto & cam = m_renderer->camera();
	return cam.position();
}
QVector3D SliceVolume::cameraTowards() const {
	Q_ASSERT_X(m_renderer, "SliceVolume::Towards", "null pointer");
	const auto & cam = m_renderer->camera();
	return cam.front();
}
QVector3D SliceVolume::cameraUp() const {
	Q_ASSERT_X(m_renderer, "SliceVolume::cameraUp", "null pointer");
	const auto & cam = m_renderer->camera();
	return cam.up();
}
QVector3D SliceVolume::cameraRight() const {
	Q_ASSERT_X(m_renderer, "SliceVolume::cameraRight", "null pointer");
	const auto & cam = m_renderer->camera();
	return cam.right();
}
QMatrix4x4 SliceVolume::viewMatrix() const {
	const auto & cam = m_renderer->camera();
	return cam.view();
}
QMatrix4x4 SliceVolume::worldMatrix() const {
	Q_ASSERT_X(m_renderer, "SliceVolume::worldMatrix", "null pointer");
	return m_renderer->m_world;
}
QMatrix4x4 SliceVolume::othoMatrix() const {
	Q_ASSERT_X(m_renderer, "SliceVolume::othoMatrix", "null pointer");
	return m_renderer->m_otho;
}
QMatrix4x4 SliceVolume::perspMatrix() const {
	Q_ASSERT_X(m_renderer, "SliceVolume::perspMatrix", "null pointer");
	return m_renderer->m_proj;
}
QVector3D SliceVolume::lightDirection() const {
	Q_ASSERT_X(m_renderer, "SliceVolume::lightDirection", "null pointer");

	return m_renderer->m_parameterWidget->options()->lightDirection;
}
float SliceVolume::ambient() const {
	Q_ASSERT_X(m_renderer, "SliceVolume::ambient", "null pointer");
	return m_renderer->m_parameterWidget->options()->ambient;
}
float SliceVolume::diffuse() const {
	Q_ASSERT_X(m_renderer, "SliceVolume::diffuse", "null pointer");
	return m_renderer->m_parameterWidget->options()->diffuse;
}
float SliceVolume::shininess() const {
	Q_ASSERT_X(m_renderer, "SliceVolume::shininess", "null pointer");
	return m_renderer->m_parameterWidget->options()->shininess;
}
float SliceVolume::specular() const {
	Q_ASSERT_X(m_renderer, "SliceVolume::specular", "null pointer");
	return m_renderer->m_parameterWidget->options()->specular;
}
QVector3D SliceVolume::volumeBound() const {
	Q_ASSERT_X(m_renderer, "SliceVolume::volumeBound", "null pointer");
	return m_renderer->m_volumeBound;
}
QSize SliceVolume::windowSize() const {
	Q_ASSERT_X(m_renderer, "SliceVolume::windowSize", "null pointer");
	return m_renderer->size();
}

SliceVolume::SliceVolume(const AbstractSliceDataModel * data, const VolumeFormat& fmt, RenderWidget * renderer) :
	GPUVolume(data->constData(), data->frontSliceCount(), data->rightSliceCount(), data->topSliceCount(), fmt)
	, m_fbo(nullptr)
	, m_gradientTexture(QOpenGLTexture::Target3D)
	, m_volumeTexture(QOpenGLTexture::Target3D)
	, m_positionVBO(QOpenGLBuffer::VertexBuffer)
	, m_positionEBO(QOpenGLBuffer::IndexBuffer)
	, m_gradCalc(data->constData(), data->frontSliceCount(), data->rightSliceCount(), data->topSliceCount())
	, m_sliceMode(false)
	, m_renderer(renderer)
	, m_dataModel(data)
{
	setRenderWidget(renderer);
}

void SliceVolume::setRenderWidget(RenderWidget* widget) {
	if (m_renderer != nullptr) {
		disconnect(m_renderer, &RenderWidget::windowResized, this, &SliceVolume::windowSizeChanged);
	}
	m_renderer = widget;
	connect(m_renderer, &RenderWidget::windowResized, this, &SliceVolume::windowSizeChanged);
}

bool SliceVolume::initializeGLResources() {

	if (initializeOpenGLFunctions() == false)
		return false;
	// Initialize Front and back face texture
	m_positionShader.reset(new PositionShader);
	m_positionShader->link();
	Q_ASSERT_X(m_positionShader->isLinked(), "VolumeWidget::initializeGL", "positionShader linking failed.");
	m_positionVAO.create();
	Q_ASSERT_X(m_positionVAO.isCreated(), "VolumeWidget::initializeGL", "VAO is not created.");
	QOpenGLVertexArrayObject::Binder positionVAOBinder(&m_positionVAO);
	m_positionVBO.create();
	m_positionVBO.bind();
	m_positionVBO.allocate(positionVert, sizeof(positionVert));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void*>(0));	//vertex
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void*>(0));	//texture coord
	m_positionShader->bind();
	m_positionEBO.create();
	m_positionEBO.bind();
	m_positionEBO.allocate(faceIndex, sizeof(faceIndex));
	m_positionVBO.release();

	// Initilize ray casting shader
	m_currentShader.reset(new RayCastingShader);
	m_currentShader->link();
	Q_ASSERT_X(m_currentShader->isLinked(), "VolumeWidget::initializeGL", "currentShader linking failed.");
	m_rayCastingTextureVAO.create();
	QOpenGLVertexArrayObject::Binder binder(&m_rayCastingTextureVAO);
	m_rayCastingTextureVBO.create();
	m_rayCastingTextureVBO.bind();
	m_currentShader->bind();
	m_rayCastingTextureVBO.allocate(8 * sizeof(GLfloat));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), reinterpret_cast<void*>(0));
	m_rayCastingTextureVBO.release();

	m_sliceShader.reset(new SliceShader());
	m_sliceShader->setSliceDataModel(m_dataModel);
	m_sliceShader->link();

	m_topSliceVAO.create();
	QOpenGLVertexArrayObject::Binder binder1(&m_topSliceVAO);
	m_topSliceVBO.create();
	m_topSliceVBO.bind();
	m_topSliceVBO.allocate(4 * 3 * sizeof(float) + 4 * 2 * sizeof(float));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D) + sizeof(QVector2D), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QVector3D) + sizeof(QVector2D), reinterpret_cast<void*>(3 * sizeof(float)));

	m_rightSliceVAO.create();
	QOpenGLVertexArrayObject::Binder binder2(&m_rightSliceVAO);
	m_rightSliceVBO.create();
	m_rightSliceVBO.bind();
	m_rightSliceVBO.allocate(4 * 3 * sizeof(float) + 4 * 2 * sizeof(float));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D) + sizeof(QVector2D), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QVector3D) + sizeof(QVector2D), reinterpret_cast<void*>(3 * sizeof(float)));

	m_frontSliceVAO.create();
	QOpenGLVertexArrayObject::Binder binder3(&m_frontSliceVAO);
	m_frontSliceVBO.create();
	m_frontSliceVBO.bind();
	m_frontSliceVBO.allocate(4 * 3 * sizeof(float) + 4 * 2 * sizeof(float));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D) + sizeof(QVector2D), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QVector3D) + sizeof(QVector2D), reinterpret_cast<void*>(3 * sizeof(float)));

	//load volume data and gradient
	loadDataAndGradientToTexture();
}

void SliceVolume::destoryGLResources()
{
	m_positionVAO.destroy();
	m_positionVBO.destroy();
	m_positionEBO.destroy();
	m_rayCastingTextureVAO.destroy();
	m_rayCastingTextureVBO.destroy();

	m_topSliceVAO.destroy();
	m_topSliceVBO.destroy();
	m_rightSliceVAO.destroy();
	m_rightSliceVBO.destroy();
	m_frontSliceVAO.destroy();
	m_frontSliceVBO.destroy();
	m_gradientTexture.destroy();
	m_fbo.reset();
}

bool SliceVolume::render() {
	if (m_renderer == nullptr || m_fbo == nullptr)
		return false;

	if (m_sliceMode == true) {
		glClear(GL_DEPTH_BUFFER_BIT);

		float topCoord = float(m_renderer->m_topSlice) / float(m_dataModel->topSliceCount());
		float rightCoord = float(m_renderer->m_rightSlice) / float(m_dataModel->rightSliceCount());
		float frontCoord = float(m_renderer->m_frontSlice) / float(m_dataModel->frontSliceCount());


		float top[] =
		{
			0.0,0.0,topCoord,0.0,0.0 ,
			1.0,0.0,topCoord,1.0,0.0 ,
			1.0,1.0,topCoord,1.0,1.0 ,
			0.0,1.0,topCoord,0.0,1.0 ,
		};
		m_topSliceVBO.bind();
		m_topSliceVBO.write(0, top, sizeof(top));
		float right[] =
		{
			0.0,rightCoord,0.0, 0.0,0.0 ,
			1.0,rightCoord,0.0, 1.0,0.0 ,
			1.0,rightCoord,1.0, 1.0,1.0 ,
			0.0,rightCoord,1.0, 0.0,1.0 ,
		};
		m_rightSliceVBO.bind();
		m_rightSliceVBO.write(0, right, sizeof(right));
		float front[] =
		{
			frontCoord,0.0,0.0, 0.0,0.0 ,
			frontCoord,1.0,0.0, 1.0,0.0 ,
			frontCoord,1.0,1.0,1.0,1.0 ,
			frontCoord,0.0,1.0,0.0,1.0 ,
		};

		m_frontSliceVBO.bind();
		m_frontSliceVBO.write(0, front, sizeof(front));

		glClear(GL_DEPTH_BUFFER_BIT);

		m_sliceShader->setSliceType(0);
		m_sliceShader->setSliceIndex(m_renderer->m_topSlice);
		m_sliceShader->load(this);
		m_sliceShader->setUniformValue("projMatrix", m_renderer->m_proj);
		m_sliceShader->setUniformValue("viewMatrix", m_renderer->m_camera.view());
		m_sliceShader->setUniformValue("worldMatrix", m_renderer->m_world);

		m_topSliceVAO.bind();
		glDrawArrays(GL_QUADS, 0, 4);

		m_sliceShader->setSliceType(1);
		m_sliceShader->setSliceIndex(m_renderer->m_rightSlice);
		m_sliceShader->load(this);
		m_rightSliceVAO.bind();
		glDrawArrays(GL_QUADS, 0, 4);

		m_sliceShader->setSliceType(2);
		m_sliceShader->setSliceIndex(m_renderer->m_frontSlice);
		m_sliceShader->load(this);
		m_frontSliceVAO.bind();
		glDrawArrays(GL_QUADS, 0, 4);
		m_sliceShader->release();

	}
	else {
		std::cout << m_fbo << std::endl;
		m_fbo->bind();
		m_positionShader->load(this);
		QOpenGLVertexArrayObject::Binder binder1(&m_positionVAO);
		// Draw Front to fbo 
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, 0);
		// Draw Back to fbo 
		glDrawBuffer(GL_COLOR_ATTACHMENT1);
		glClear(GL_COLOR_BUFFER_BIT);
		glDepthFunc(GL_GREATER);
		glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, 0);
		glDepthFunc(GL_LESS);
		m_positionShader->release();
		m_fbo->release();

		// Ray casting in shader
		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_TEXTURE_RECTANGLE_NV);
		glEnable(GL_TEXTURE_3D);
		m_currentShader->load(this);
		QOpenGLVertexArrayObject::Binder binder2(&m_rayCastingTextureVAO);
		glDrawArrays(GL_QUADS, 0, 4);
		glDisable(GL_TEXTURE_RECTANGLE_NV);
		glDisable(GL_TEXTURE_3D);
		glDisable(GL_BLEND);
		m_currentShader->release();
	}
	return true;
}

void SliceVolume::windowSizeChanged(int w, int h) {
	m_fbo.reset(new QOpenGLFramebufferObject(w, h, QOpenGLFramebufferObject::Depth, GL_TEXTURE_RECTANGLE_NV, GL_RGBA32F_ARB));
	m_fbo->addColorAttachment(w, h);

	static QVector<QVector2D> rayCastingVB = {
		{ 0.0f,0.0f },
	{ 0.0,static_cast<float>(h) },
	{ static_cast<float>(w),static_cast<float>(h) },
	{ static_cast<float>(w),0.0 }
	};
	m_rayCastingTextureVBO.bind();
	m_rayCastingTextureVBO.write(0, rayCastingVB.constData(), rayCastingVB.count() * 2 * sizeof(GLfloat));
	m_rayCastingTextureVBO.release();
}
