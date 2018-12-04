#include <QOpenGLFramebufferObject>
#include <QDebug>

#include "slicevolume.h"
#include "abstract/abstractslicedatamodel.h"
#include "qmath.h"

const static int faceIndex[] = { 1, 3, 7, 5, 0, 4, 6, 2, 2, 6, 7, 3, 0, 1, 5, 4, 4, 5, 7, 6, 0, 2, 3, 1 };
static const float xCoord = 1.0, yCoord = 1.0, zCoord = 1.0;
const float offset = 0.5;
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


#define GLERROR(str)									\
	{													\
		GLenum err;										\
		while ((err = glfuncs->glGetError()) != GL_NO_ERROR)		\
		{												\
			std::cout<<err<<" "<<str<<std::endl;		\
		}												\
	}													\


void SliceVolume::loadDataAndGradientToTexture() {

	//if (m_gradCalc.hasData() == false)
	//	return;
	//if (m_gradCalc.ready() == false)
	//	m_gradCalc.calcGradent();		//Time-consuming

	//const auto d = m_gradCalc.data3();
	const auto z = zLength();
	const auto y = yLength();
	const auto x = xLength();
	//m_gradientTexture.destroy();
	if (m_initialized == true) {
		return;
	}
	if (m_volumeTexture == nullptr) {
		m_volumeTexture = new QOpenGLTexture(QOpenGLTexture::Target3D);
		m_volumeTexture->setMagnificationFilter(QOpenGLTexture::Linear);
		m_volumeTexture->setMinificationFilter(QOpenGLTexture::Linear);
		m_volumeTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
		m_volumeTexture->setSize(x, y, z);
		m_volumeTexture->setFormat(QOpenGLTexture::R16F);		//Internal format
		m_volumeTexture->allocateStorage();
		reloadVolumeData();
	}
}

unsigned SliceVolume::volumeTexId() const
{
	return m_volumeTexture->textureId();
}
QVector3D SliceVolume::voxelSize() const
{
	return QVector3D(xLength(), yLength(), zLength());
}
unsigned SliceVolume::startPosTexIdx() const
{
	return m_fbo->textures()[0];
}
unsigned SliceVolume::endPosTexIdx() const
{
	return m_fbo->textures()[1];
}
unsigned SliceVolume::gradientTexId() const
{
	return 0;
	//return m_gradientTexture->textureId();
}
float SliceVolume::rayStep() const
{
	Q_ASSERT_X(m_renderer, "SliceVolume::rayStep", "null pointer");
	return m_renderer->m_rayStep;
}
unsigned SliceVolume::transferFunctionsTexId() const
{
	Q_ASSERT_X(m_renderer, "SliceVolume::transferFunctionsTexId", "null pointer");
	//qDebug() << m_renderer->m_tfTexture;
	if (m_renderer->m_tfTexture == nullptr) return 0;
	return m_renderer->m_tfTexture->textureId();
}

QVector3D SliceVolume::cameraPos() const
{
	Q_ASSERT_X(m_renderer, "SliceVolume::cameraPos", "null pointer");
	const auto & cam = m_renderer->camera();
	return cam.position();
}

QVector3D SliceVolume::cameraTowards() const
{
	Q_ASSERT_X(m_renderer, "SliceVolume::Towards", "null pointer");
	const auto & cam = m_renderer->camera();
	return cam.front();
}

QVector3D SliceVolume::cameraUp() const
{
	Q_ASSERT_X(m_renderer, "SliceVolume::cameraUp", "null pointer");
	const auto & cam = m_renderer->camera();
	return cam.up();
}

QVector3D SliceVolume::cameraRight() const
{
	Q_ASSERT_X(m_renderer, "SliceVolume::cameraRight", "null pointer");
	const auto & cam = m_renderer->camera();
	return cam.right();
}

QMatrix4x4 SliceVolume::viewMatrix() const
{
	const auto & cam = m_renderer->camera();
	return cam.view();
}
QMatrix4x4 SliceVolume::worldMatrix() const
{
	Q_ASSERT_X(m_renderer, "SliceVolume::worldMatrix", "null pointer");
	return m_normalizeTransform * transform();
}
QMatrix4x4 SliceVolume::othoMatrix() const
{
	Q_ASSERT_X(m_renderer, "SliceVolume::othoMatrix", "null pointer");
	return m_renderer->m_otho;
}
QMatrix4x4 SliceVolume::perspMatrix() const
{
	Q_ASSERT_X(m_renderer, "SliceVolume::perspMatrix", "null pointer");
	return m_renderer->m_proj;
}
QVector3D SliceVolume::lightDirection() const
{
	Q_ASSERT_X(m_renderer, "SliceVolume::lightDirection", "null pointer");
	return m_renderer->d_ptr->options->lightDirection;
}
float SliceVolume::ambient() const
{
	Q_ASSERT_X(m_renderer, "SliceVolume::ambient", "null pointer");
	return m_renderer->d_ptr->options->ambient;
}
float SliceVolume::diffuse() const
{
	Q_ASSERT_X(m_renderer, "SliceVolume::diffuse", "null pointer");
	return m_renderer->d_ptr->options->diffuse;
}
float SliceVolume::shininess() const
{
	Q_ASSERT_X(m_renderer, "SliceVolume::shininess", "null pointer");
	return m_renderer->d_ptr->options->shininess;
}
float SliceVolume::specular() const
{
	Q_ASSERT_X(m_renderer, "SliceVolume::specular", "null pointer");
	return m_renderer->d_ptr->options->specular;
}
QVector3D SliceVolume::volumeBound() const
{
	Q_ASSERT_X(m_renderer, "SliceVolume::volumeBound", "null pointer");
	return m_renderer->m_volumeBound;
}
QSize SliceVolume::windowSize() const
{
	Q_ASSERT_X(m_renderer, "SliceVolume::windowSize", "null pointer");
	return m_renderer->size();
}

SliceVolume::SliceVolume(const void * data, int x, int y, int z, const QMatrix4x4 & trans, const VolumeFormat& fmt, RenderWidget * renderer) :
	GPUVolume(data, x, y, z, trans, fmt)
	, m_fbo(nullptr)
	//, m_gradientTexture(nullptr)
	, m_positionVBO(QOpenGLBuffer::VertexBuffer)
	, m_volumeTexture(nullptr)
	, m_positionEBO(QOpenGLBuffer::IndexBuffer)
	//	, m_gradCalc(data->constData(), data->frontSliceCount(), data->rightSliceCount(), data->topSliceCount())
	, m_renderer(renderer)
	//, m_sliceMode(false)
	, m_renderType(RenderType::DVR)
	, m_frontSliceVisible(true)
	, m_rightSliceVisible(true)
	, m_topSliceVisible(true)
	, m_positionShader(nullptr)
	, m_currentShader(nullptr)

	, m_sliceShader(nullptr)
	, m_initialized(false)
{
	QVector3D scale = QVector3D(x, y, z);
	scale.normalize();
	m_normalizeTransform.setToIdentity();
	m_normalizeTransform.scale(scale);
	setRenderWidget(renderer);
}

void SliceVolume::setRenderWidget(RenderWidget* widget) {
	if (m_renderer != nullptr) {
		disconnect(m_renderer, 0, this, 0);
	}
	m_renderer = widget;
	connect(m_renderer, &RenderWidget::windowResized, this, &SliceVolume::setFramebufferSize, Qt::AutoConnection);
}

bool SliceVolume::initializeGLResources() {

	if (m_renderer == nullptr)
		return false;
	auto glfuncs = m_renderer->context()->versionFunctions<QOpenGLFunctions_3_3_Core>();
	if (glfuncs == nullptr)
		return false;

	// Initialize Front and back face texture
	m_positionShader = new PositionShader;
	m_positionShader->link();
	Q_ASSERT_X(m_positionShader->isLinked(), "SliceVolume::initializeGLResources", "positionShader linking failed.");
	m_positionVAO.create();
	Q_ASSERT_X(m_positionVAO.isCreated(), "SliceVolume::initializeGLResources", "VAO is not created.");
	QOpenGLVertexArrayObject::Binder positionVAOBinder(&m_positionVAO);
	m_positionVBO.create();
	m_positionVBO.bind();
	m_positionVBO.allocate(positionVert, sizeof(positionVert));
	glfuncs->glEnableVertexAttribArray(0);
	glfuncs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void*>(0));	//vertex
	glfuncs->glEnableVertexAttribArray(1);
	glfuncs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void*>(0));	//texture coord
	m_positionShader->bind();
	m_positionEBO.create();
	m_positionEBO.bind();
	m_positionEBO.allocate(faceIndex, sizeof(faceIndex));
	m_positionVBO.release();

	// initialize  shaders

	m_shaders.clear();
	m_currentShader = new RayCastingShader;
	Q_ASSERT_X(m_currentShader->link(), "SliceVolume::initializeGLResources", "RayCastingShader linking failed.");
	m_shaders[RenderType::DVR] = m_currentShader;

	auto modulo = new RayCastingModuloShader;
	Q_ASSERT_X(modulo->link(), "SliceVolume::initializeGLResources", "RayCastingModulo shader linking failed.");
	m_shaders[RenderType::Modulo] = modulo;

	m_sliceShader = new SliceShader;
	Q_ASSERT_X(m_sliceShader->link(), "SliceVolume::initializeGLResources", "Slice shader linking failed.");

	// Ray casting vao
	{
		m_rayCastingTextureVAO.create();
		QOpenGLVertexArrayObject::Binder binder(&m_rayCastingTextureVAO);
		m_rayCastingTextureVBO.create();
		m_rayCastingTextureVBO.bind();
		m_rayCastingTextureVBO.allocate(8 * sizeof(GLfloat));
		glfuncs->glEnableVertexAttribArray(0);
		glfuncs->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), reinterpret_cast<void*>(0));
		glfuncs->glEnableVertexAttribArray(1);
		glfuncs->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), reinterpret_cast<void*>(0));
		m_rayCastingTextureVBO.release();
	}

	// axis-aligned slice vao and vbo
	{
		m_axisAlignedSliceVAO.create();
		QOpenGLVertexArrayObject::Binder binder(&m_axisAlignedSliceVAO);
		m_axisAlignedSliceVBO.create();
		m_axisAlignedSliceVBO.bind();
		m_axisAlignedSliceVBO.allocate(6 * 3 * sizeof(float));
		glfuncs->glEnableVertexAttribArray(0);
		glfuncs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), reinterpret_cast<void*>(0));
		glfuncs->glEnableVertexAttribArray(1);
		glfuncs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), reinterpret_cast<void*>(0));	//texture coord

	}

	// arbitrary slice vao and vbo
	{
		m_arbitrarySliceVAO.create();
		QOpenGLVertexArrayObject::Binder binder(&m_arbitrarySliceVAO);
		m_arbitrarySliceVBO.create();
		m_arbitrarySliceVBO.bind();
		m_arbitrarySliceVBO.allocate(6 * 3 * sizeof(float));
		glfuncs->glEnableVertexAttribArray(0);
		glfuncs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), reinterpret_cast<void*>(0));
		glfuncs->glEnableVertexAttribArray(1);
		glfuncs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), reinterpret_cast<void*>(0));	//texture coord

	}

	m_polys.resize(6);
	for (int i = 0; i < 6; i++) {
		m_polys[i].vao = new QOpenGLVertexArrayObject;
		m_polys[i].vbo = new QOpenGLBuffer;
		auto & vao = m_polys[i].vao;
		auto & vbo = m_polys[i].vbo;
		vao->create();
		QOpenGLVertexArrayObject::Binder binder(vao);
		vbo->create();
		vbo->bind();
		glfuncs->glEnableVertexAttribArray(0);
		glfuncs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), reinterpret_cast<void*>(0));	//vertex
		glfuncs->glEnableVertexAttribArray(1);
		glfuncs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), reinterpret_cast<void*>(0));	//texture coord
	}

	//load volume data and gradient
	loadDataAndGradientToTexture();
	setFramebufferSize(windowSize().width(), windowSize().height());
	return (m_initialized = true);
}

void SliceVolume::destroyGLResources()
{
	m_positionVAO.destroy();
	m_positionVBO.destroy();
	m_positionEBO.destroy();
	m_rayCastingTextureVAO.destroy();
	m_rayCastingTextureVBO.destroy();
	m_axisAlignedSliceVAO.destroy();
	m_axisAlignedSliceVBO.destroy();

	m_arbitrarySliceVAO.destroy();
	m_arbitrarySliceVBO.destroy();

	for (auto & item : m_polys)
	{
		item.vao->destroy();
		item.vao->destroy();
		delete item.vao;
		delete item.vbo;
		item.vao = nullptr;
		item.vbo = nullptr;
	}

	delete m_volumeTexture;
	m_volumeTexture = nullptr;

	delete m_positionShader;
	m_positionShader = nullptr;
	//delete m_currentShader;
	//m_currentShader = nullptr;
	delete m_sliceShader;
	m_sliceShader = nullptr;
	delete m_fbo;
	m_fbo = nullptr;


	for (auto s : m_shaders)
		delete s;

	m_initialized = false;
}

bool SliceVolume::render()
{
	if (m_renderer == nullptr || m_fbo == nullptr)
		return false;

	auto glfuncs = m_renderer->context()->versionFunctions<QOpenGLFunctions_3_3_Core>();
	if (glfuncs == nullptr)
		return false;

	if (m_polygonUpdate)
	{
		m_polygonUpdate = false;
		m_arbitrarySliceVertex = sliceCoord(m_A, m_B, m_C, m_D);
		m_arbitrarySliceVBO.bind();
		m_arbitrarySliceVBO.write(0, m_arbitrarySliceVertex.constData(), sizeof(QVector3D)*m_arbitrarySliceVertex.size());
		m_arbitrarySliceVBO.release();
		updatePolygons();
	}


	if (m_renderType == RenderType::Slice) {
		// Mesh rendering
		glfuncs->glClear(GL_DEPTH_BUFFER_BIT);
		const auto topCoord = float(m_renderer->d_ptr->topSliceIndex) / float(zLength());
		const auto rightCoord = float(m_renderer->d_ptr->rightSliceIndex) / float(yLength());
		const auto frontCoord = float(m_renderer->d_ptr->frontSliceIndex) / float(xLength());

		m_sliceShader->load(this);
		m_sliceShader->setUniformValue("projMatrix", m_renderer->m_proj);
		m_sliceShader->setUniformValue("viewMatrix", m_renderer->m_camera.view());
		m_sliceShader->setUniformValue("worldMatrix", transform()*m_normalizeTransform);

		glfuncs->glClear(GL_DEPTH_BUFFER_BIT);
		{
			QOpenGLVertexArrayObject::Binder binder(&m_axisAlignedSliceVAO);
			m_axisAlignedSliceVBO.bind();
			if (m_topSliceVisible) {
				float top[] =
				{
					0.0,0.0,topCoord,
					1.0,0.0,topCoord,
					1.0,1.0,topCoord,
					0.0,1.0,topCoord,
				};
				m_axisAlignedSliceVBO.write(0, top, sizeof(top));
				glfuncs->glDrawArrays(GL_QUADS, 0, 4);
			}
			if (m_rightSliceVisible) {
				float right[] =
				{
					0.0,rightCoord,0.0,
					1.0,rightCoord,0.0,
					1.0,rightCoord,1.0,
					0.0,rightCoord,1.0,
				};
				m_axisAlignedSliceVBO.write(0, right, sizeof(right));
				glfuncs->glDrawArrays(GL_QUADS, 0, 4);
			}
			if (m_frontSliceVisible) {
				float front[] =
				{
					frontCoord,0.0,0.0,
					frontCoord,1.0,0.0,
					frontCoord,1.0,1.0,
					frontCoord,0.0,1.0,
				};
				m_axisAlignedSliceVBO.write(0, front, sizeof(front));
				glfuncs->glDrawArrays(GL_QUADS, 0, 4);
			}
			m_axisAlignedSliceVBO.release();
		}

		{
			QOpenGLVertexArrayObject::Binder binder(&m_arbitrarySliceVAO);
			glfuncs->glDrawArrays(GL_TRIANGLE_FAN, 0, m_arbitrarySliceVertex.size());
			m_sliceShader->release();
		}
	}
	else {		// Volume rendering



		m_fbo->bind();
		m_positionShader->load(this);
		// Draw Front to fbo 
		glfuncs->glDepthFunc(GL_LESS);
		glfuncs->glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glfuncs->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawEntryPoint();
		// Draw Back to fbo 
		glfuncs->glDrawBuffer(GL_COLOR_ATTACHMENT1);
		glfuncs->glClear(GL_COLOR_BUFFER_BIT);
		glfuncs->glDepthFunc(GL_GREATER);
		drawEntryPoint();

		glfuncs->glDepthFunc(GL_LESS);
		m_positionShader->release();
		m_fbo->release();

		// Ray casting in shader
		glfuncs->glEnable(GL_BLEND);
		glfuncs->glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
		glfuncs->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfuncs->glEnable(GL_TEXTURE_RECTANGLE);
		glfuncs->glEnable(GL_TEXTURE_3D);

		m_currentShader = m_shaders[m_renderType];
		m_currentShader->load(this);
		QOpenGLVertexArrayObject::Binder binder2(&m_rayCastingTextureVAO);
		glfuncs->glDrawArrays(GL_QUADS, 0, 4);
		glfuncs->glDisable(GL_TEXTURE_RECTANGLE);
		glfuncs->glDisable(GL_TEXTURE_3D);
		glfuncs->glDisable(GL_BLEND);

		m_currentShader->release();
	}
	return true;
}

SliceVolume::~SliceVolume() {

}

void SliceVolume::setFramebufferSize(int w, int h) {

	if (m_fbo != nullptr)
		delete m_fbo;
	m_fbo = new QOpenGLFramebufferObject(w, h, QOpenGLFramebufferObject::Depth, GL_TEXTURE_RECTANGLE, GL_RGBA32F_ARB);
	m_fbo->addColorAttachment(w, h);
	QVector<QVector2D> rayCastingVB =
	{
		{ 0.0f,0.0f },
		{ 0.0,static_cast<float>(h) },
		{ static_cast<float>(w),static_cast<float>(h) },
		{ static_cast<float>(w),0.0 }
	};

	m_rayCastingTextureVBO.bind();
	m_rayCastingTextureVBO.write(0, rayCastingVB.constData(), rayCastingVB.count() * 2 * sizeof(GLfloat));
	m_rayCastingTextureVBO.release();
}

QVector<QVector3D> SliceVolume::sliceCoord(double A, double B, double C, double D) {
	QVector<QVector3D> coords;
	const auto v1 = float(-(D / C));
	const auto v2 = float(-(D + A) / C);
	const auto v3 = float(-(D + B) / C);
	const auto v4 = float(-(D + A + B) / C);
	const auto v5 = float(-(D) / A);
	const auto v6 = float(-(B + D) / A);
	const auto v7 = float(-(C + D) / A);
	const auto v8 = float(-(B + C + D) / A);
	const auto v9 = float(-(D) / B);
	const auto v10 = float(-(D + A) / B);
	const auto v11 = float(-(D + C) / B);
	const auto v12 = float(-(D + A + C) / B);
	//qDebug("%f %f %f %f %f %f %f %f %f %f %f %f\n",v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12);
	if (isInRange(v1))
		coords.push_back({ 0,0,v1 });
	if (isInRange(v2))
		coords.push_back({ 1,0,v2 });
	if (isInRange(v3))
		coords.push_back({ 0,1,v3 });
	if (isInRange(v4))
		coords.push_back({ 1,1,v4 });
	if (isInRange(v5))
		coords.push_back({ v5, 0,0 });
	if (isInRange(v6))
		coords.push_back({ v6 ,1,0 });
	if (isInRange(v7))
		coords.push_back({ v7,0,1 });
	if (isInRange(v8))
		coords.push_back({ v8,1,1 });
	if (isInRange(v9))
		coords.push_back({ 0,v9,0 });
	if (isInRange(v10))
		coords.push_back({ 1,v10,0 });
	if (isInRange(v11))
		coords.push_back({ 0,v11,1 });
	if (isInRange(v12))
		coords.push_back({ 1,v12,1 });
	if (coords.empty() == true)
		return coords;
	makeConvexPolygon(A, B, C, coords);
	return coords;
}

void SliceVolume::makeConvexPolygon(double A, double B, double C, QVector<QVector3D>& coords)
{
	if (coords.empty())
		return;
	// Convex Polygon Sorting
	// Step1: Calculate centroid

	auto c = QVector3D(0, 0, 0);
	for (const auto & p : coords) {
		c += p;
	}
	c /= coords.size();

	// Step2: Calculate normal N
	const auto N = QVector3D(A, B, C);
	//const auto N = QVector3D::crossProduct(coords[1]-c,coords[0]-c).normalized();

	// Step3: Get vector from the centroid to the first point:ZA
	const auto V = (coords[0] - c).normalized();

	// Step4: order all points P by the signed angle ZA to ZP with normal N
	// Note:  signed angle == angle(ZA,ZP) * sign(n dot (ZA cross ZP))
	std::sort(coords.begin(), coords.end(), [&V, &N, &c](const QVector3D & v1, const QVector3D & v2)->bool {
		const auto zp1 = (v1 - c).normalized(), zp2 = (v2 - c).normalized();
		const auto signedAngle1 = qRadiansToDegrees(std::acos(clamp(QVector3D::dotProduct(V, zp1), -1, 1)))*getSign(QVector3D::dotProduct(N, QVector3D::crossProduct(V, zp1)));
		const auto signedAngle2 = qRadiansToDegrees(std::acos(clamp(QVector3D::dotProduct(V, zp2), -1, 1)))*getSign(QVector3D::dotProduct(N, QVector3D::crossProduct(V, zp2)));
		return signedAngle1 < signedAngle2;
	});

}

/**
 * \brief  This function must be called enclosed with OpenGL context
 */
void SliceVolume::updatePolygons()
{
	auto glfuncs = m_renderer->context()->versionFunctions<QOpenGLFunctions_3_3_Core>();
	if (glfuncs == nullptr)
		return;

	auto oneside = m_arbitrarySliceVertex;
	for (auto i = 0; i < 8; i++) {
		const auto v = QVector3D{
			positionVert[3 * i],positionVert[3 * i + 1],positionVert[3 * i + 2]
		};
		if ((v.x() * m_A + v.y()*m_B + v.z() * m_C + m_D) > 0) {
			oneside.push_back(v);
		}
	}

	QVector<QVector3D> faces[6];
	for (int i = 0; i < 6; i++) {
		for (const auto & v : oneside) {
			switch (i) {
			case 0:if (v.x() == 0.0)faces[i].push_back(v); break;
			case 1:if (v.x() == 1.0)faces[i].push_back(v); break;
			case 2:if (v.y() == 0.0)faces[i].push_back(v); break;
			case 3:if (v.y() == 1.0)faces[i].push_back(v); break;
			case 4:if (v.z() == 0.0)faces[i].push_back(v); break;
			case 5:if (v.z() == 1.0)faces[i].push_back(v); break;
			default:Q_ASSERT(false);
			}
		}

	}


	for (auto i = 0; i < 6; i++) {
		makeConvexPolygon(m_A, m_B, m_C, faces[i]);
		auto & vbo = m_polys[i].vbo;
		vbo->bind();
		vbo->allocate(faces[i].constData(), sizeof(QVector3D)*faces[i].size());
		m_polys[i].nVertex = faces[i].size();
		vbo->release();
	}

}

void SliceVolume::drawEntryPoint() {
	auto glfuncs = m_renderer->context()->versionFunctions<QOpenGLFunctions_3_3_Core>();
	if (glfuncs == nullptr)
		return;
	for (const auto & poly : m_polys) {
		if (poly.nVertex) {
			QOpenGLVertexArrayObject::Binder binder(poly.vao);
			glfuncs->glDrawArrays(GL_TRIANGLE_FAN, 0, poly.nVertex);
		}
	}

	QOpenGLVertexArrayObject::Binder binder(&m_arbitrarySliceVAO);
	glfuncs->glDrawArrays(GL_TRIANGLE_FAN, 0, m_arbitrarySliceVertex.size());
	//glfuncs->glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, 0);
}

void SliceVolume::reloadVolumeData()
{
	if (!m_volumeTexture)
		return;


	const auto fmt = format();
	QOpenGLTexture::PixelFormat pfmt;
	QOpenGLTexture::PixelType ptype;

	switch (fmt.fmt)
	{
		case VoxelFormat::Grayscale:pfmt = QOpenGLTexture::Red; break;
		case VoxelFormat::RGB:pfmt = QOpenGLTexture::RGB; break;
		case VoxelFormat::RGBA:pfmt = QOpenGLTexture::RGBA; break;;
		default: Q_ASSERT(false);
	}

	switch (fmt.type)
	{
		case VoxelType::Float32:ptype = QOpenGLTexture::Float32; break;
		case VoxelType::UInt8:ptype = QOpenGLTexture::UInt8; break;
		default: Q_ASSERT(false);
	}

	m_volumeTexture->setData(pfmt, ptype, data());		//External format

}

QVector3D SliceVolume::sliceNormal(const QVector<QVector3D>& slice) {
	Q_ASSERT_X(slice.size() >= 3, "", "");
	return QVector3D::crossProduct(slice[1] - slice[0], slice[2] - slice[1]).normalized();
}

