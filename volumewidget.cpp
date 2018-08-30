#include "volumewidget.h"
#include "abstractslicedatamodel.h"
#include "markmodel.h"
#include <QDebug>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <iostream>
#include "volume/Shaders/shaderprogram.h"
#define GLERROR(str)									\
	{													\
		GLenum err;										\
		while ((err = glGetError()) != GL_NO_ERROR)		\
		{												\
			std::cout<<err<<" "<<str<<std::endl;		\
		}												\
	}													\

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
"   fragColor = texture(tex,texCoord);\n"
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

//static const float xCoord = 1.0, yCoord = 1.0, zCoord = 1.0;
//float positionVert[] = {
//	0,0,0,
//	xCoord, 0, 0 ,
//	0, yCoord, 0 ,
//	xCoord, yCoord, 0 ,
//	0, 0, zCoord ,
//	xCoord, 0, zCoord ,
//	0, yCoord, zCoord ,
//	xCoord, yCoord, zCoord
//};
const static int faceIndex[] = { 1, 3, 7, 5, 0, 4, 6, 2, 2, 6, 7, 3, 0, 1, 5, 4, 4, 5, 7, 6, 0, 2, 3, 1 };


static QVector<QVector2D> cubeTexCoord = {
	{ 0.f,0.f },{ 1.0f,0.f },{ 0.f,0.f },
	{ 0.f,0.f },{ 0.f,1.0f },{ 0.f,0.f },
	{ 0.f,0.f },{ 0.f,1.0f },{ 0.f,0.f },
	{ 0.f,0.f },{ 0.f,1.0f },{ 0.f,0.f },
	{ 0.f,0.f },{ 0.f,1.0f },{ 0.f,0.f },
	{ 0.f,0.f },{ 0.f,1.0f },{ 0.f,0.f },
	{ 0.f,0.f },{ 0.f,1.0f },{ 0.f,0.f },
	{ 0.f,0.f },{ 0.f,1.0f },{ 0.f,0.f },
	{ 0.f,0.f },{ 0.f,1.0f },{ 0.f,0.f },
	{ 0.f,0.f },{ 0.f,1.0f },{ 0.f,0.f },
	{ 0.f,0.f },{ 0.f,1.0f },{ 0.f,0.f },
	{ 0.f,0.f },{ 0.f,1.0f },{ 0.f,0.f },
};

static QVector<QVector3D> cubeVert =
{
	//back
	
	{ 0.5f, -0.5f, -0.5f },{ 0.f,0.f,-1.f },
	{ -0.5f, -0.5f, -0.5f },{ 0.f,0.f,-1.f },
	{ 0.5f,  0.5f, -0.5f },{ 0.f,0.f,-1.f },
	{ -0.5f,  0.5f, -0.5f } ,{ 0.f,0.f,-1.f },
	{ 0.5f,  0.5f, -0.5f } ,{ 0.f,0.f,-1.f },
	{ -0.5f, -0.5f, -0.5f } ,{ 0.f,0.f,-1.f },
	//front
	{ -0.5f, -0.5f,  0.5f },{0.f,0.f,1.f},
	{ 0.5f, -0.5f,  0.5f },{ 0.f,0.f,1.f },
	{ 0.5f,  0.5f,  0.5f },{ 0.f,0.f,1.f },
	{ 0.5f,  0.5f,  0.5f },{ 0.f,0.f,1.f },
	{ -0.5f,  0.5f,  0.5f },{ 0.f,0.f,1.f },
	{ -0.5f, -0.5f,  0.5f },{ 0.f,0.f,1.f },
	//left
	{ -0.5f,  0.5f,  0.5f },{-1.0f,0.f,0.f},
	{ -0.5f,  0.5f, -0.5f },{ -1.0f,0.f,0.f },
	{ -0.5f, -0.5f, -0.5f },{ -1.0f,0.f,0.f },
	{ -0.5f, -0.5f, -0.5f },{ -1.0f,0.f,0.f },
	{ -0.5f, -0.5f,  0.5f },{ -1.0f,0.f,0.f },
	{ -0.5f,  0.5f,  0.5f },{ -1.0f,0.f,0.f },
	//right
	{ 0.5f,  0.5f, -0.5f },{ 1.0f,0.f,0.f },
	{ 0.5f,  0.5f,  0.5f },{ 1.0f,0.f,0.f },
	{ 0.5f, -0.5f, -0.5f } ,{ 1.0f,0.f,0.f },
	{ 0.5f, -0.5f,  0.5f } ,{ 1.0f,0.f,0.f },
	{ 0.5f, -0.5f, -0.5f } ,{ 1.0f,0.f,0.f },
	{ 0.5f,  0.5f,  0.5f } ,{ 1.0f,0.f,0.f },
	//bottom
	{ -0.5f, -0.5f, -0.5f },{ 0.f,-1.f,0.f },
	{ 0.5f, -0.5f, -0.5f },{ 0.f,-1.f,0.f },
	{ 0.5f, -0.5f,  0.5f },{ 0.f,-1.f,0.f },
	{ 0.5f, -0.5f,  0.5f } ,{ 0.f,-1.f,0.f },
	{ -0.5f, -0.5f,  0.5f } ,{ 0.f,-1.f,0.f },
	{ -0.5f, -0.5f, -0.5f }  ,{ 0.f,-1.f,0.f },
	//up
	{ 0.5f,  0.5f, -0.5f }  ,{ 0.f,1.f,0.f },
	{ -0.5f,  0.5f, -0.5f }  ,{ 0.f,1.f,0.f },
	{ 0.5f,  0.5f,  0.5f }  ,{ 0.f,1.f,0.f },
	{ -0.5f,  0.5f,  0.5f } ,{ 0.f,1.f,0.f },
	{ 0.5f,  0.5f,  0.5f } ,{ 0.f,1.f,0.f },
	{ -0.5f,  0.5f, -0.5f }  ,{ 0.f,1.f,0.f },
};




GradientCalculator::GradientCalculator(const AbstractSliceDataModel * slice, const MarkModel * mark, QObject * parent)
:QObject(parent),
m_sliceModel(slice),
m_mark(mark)
{
}

void GradientCalculator::setDataModel(const AbstractSliceDataModel* slice)
{
	m_sliceModel = slice;
}

void GradientCalculator::setMarkModel(const MarkModel* mark)
{
	m_mark = mark;
}

bool GradientCalculator::ready() const
{
	return m_sliceModel != nullptr/*m_mark != nullptr*/;
}

unsigned char * GradientCalculator::data() const
{
	 return m_gradient.get();
}

 void GradientCalculator::init()
 {
	 if (m_sliceModel == nullptr) return;
	 const int z = m_sliceModel->topSliceCount();
	 const int y = m_sliceModel->rightSliceCount();
	 const int x = m_sliceModel->frontSliceCount();
	 QMutexLocker locker(&m_mutex);
	 m_gradient.reset(new unsigned char[x*y*z * 4]);
 }

QVector3D GradientCalculator::triCubicIntpGrad(const unsigned char* pData, double px, double py, double pz)
{
	double ulo, vlo, wlo;
	const double ut = modf(px, &ulo);
	const double vt = modf(py, &vlo);
	const double wt = modf(pz, &wlo);

	int		xlo = int(ulo);
	int		ylo = int(vlo);
	int		zlo = int(wlo);

	// We need all the voxels around the primary
	double voxels[4][4][4];
	{
		for (int z = 0; z<4; z++) {
			for (int y = 0; y<4; y++) {
				for (int x = 0; x<4; x++) {
					int px = (xlo - 1 + x);
					int py = (ylo - 1 + y);
					int pz = (zlo - 1 + z);
					voxels[z][y][x] = value(pData, px, py, pz);
				}
			}
		}
	}

	QVector3D direction;

	// Now that we have all our voxels, run the cubic interpolator in one dimension to collapse it (we choose to collapase x)
	double voxelcol[4][4];
	double voxelcol2[4];
	int x, y, z;
	for (z = 0; z<4; z++) {
		for (y = 0; y<4; y++) {
			voxelcol[z][y] = cubicIntpGrad(voxels[z][y][0], voxels[z][y][1], voxels[z][y][2], voxels[z][y][3], ut);
		}
	}
	// Then collapse the y dimension
	for (z = 0; z<4; z++) {
		voxelcol2[z] = cubicIntpValue(voxelcol[z][0], voxelcol[z][1], voxelcol[z][2], voxelcol[z][3], vt);
	}

	// The collapse the z dimension to get our value
	direction.setX(cubicIntpValue(voxelcol2[0], voxelcol2[1], voxelcol2[2], voxelcol2[3], wt));

	for (z = 0; z<4; z++) {
		for (x = 0; x<4; x++) {
			voxelcol[z][x] = cubicIntpGrad(voxels[z][0][x], voxels[z][1][x], voxels[z][2][x], voxels[z][3][x], vt);
		}
	}
	// Then collapse the x dimension
	for (z = 0; z<4; z++) {
		voxelcol2[z] = cubicIntpValue(voxelcol[z][0], voxelcol[z][1], voxelcol[z][2], voxelcol[z][3], ut);
	}

	// The collapse the z dimension to get our value
	direction.setY(cubicIntpValue(voxelcol2[0], voxelcol2[1], voxelcol2[2], voxelcol2[3], wt));

	for (y = 0; y<4; y++) {
		for (x = 0; x<4; x++) {
			voxelcol[y][x] = cubicIntpGrad(voxels[0][y][x], voxels[1][y][x], voxels[2][y][x], voxels[3][y][x], wt);
		}
	}
	// Then collapse the x dimension
	for (y = 0; y<4; y++) {
		voxelcol2[y] = cubicIntpValue(voxelcol[y][0], voxelcol[y][1], voxelcol[y][2], voxelcol[y][3], ut);
	}

	// The collapse the z dimension to get our value
	direction.setZ(cubicIntpValue(voxelcol2[0], voxelcol2[1], voxelcol2[2], voxelcol2[3], vt));

	return direction;
}

double GradientCalculator::value(const unsigned char* pData, double x, double y, double z) const
{
	const int xiSize = m_sliceModel->topSliceCount();
	const int yiSize = m_sliceModel->rightSliceCount();
	const int ziSize = m_sliceModel->frontSliceCount();
	x = std::max(std::min(x + 0.5, xiSize - 1.0), 0.0);
	y = std::max(std::min(y + 0.5, yiSize - 1.0), 0.0);
	z = std::max(std::min(z + 0.5, ziSize - 1.0), 0.0);
	int index = (int)z * xiSize * yiSize + (int)y * xiSize + (int)x;
	return pData[index];
}

double GradientCalculator::cubicIntpGrad(double v0, double v1, double v2, double v3, double mu)
{
	const double mu2 = mu * mu;
	const double mu3 = mu2 * mu;

	const double a0 = 0.0 * v0 + 0.0 * v1 + 0.0 * v2 + 0.0 * v3;
	const double a1 = -1.0 * v0 + 3.0 * v1 - 3.0 * v2 + 1.0 * v3;
	const double a2 = 2.0 * v0 - 4.0 * v1 + 2.0 * v2 + 0.0 * v3;
	const double a3 = -1.0 * v0 + 0.0 * v1 + 1.0 * v2 + 0.0 * v3;

	return (a0 * mu3 + a1 * mu2 + a2 * mu + a3) / 2.0;
}

double GradientCalculator::cubicIntpValue(double v0, double v1, double v2, double v3, double mu)
{
	const double mu2 = mu * mu;
	const double mu3 = mu2 * mu;

	const double a0 = -1.0 * v0 + 3.0 * v1 - 3.0 * v2 + 1.0 * v3;
	const double a1 = 3.0 * v0 - 6.0 * v1 + 3.0 * v2 + 0.0 * v3;
	const double a2 = -3.0 * v0 + 0.0 * v1 + 3.0 * v2 + 0.0 * v3;
	const double a3 = 1.0 * v0 + 4.0 * v1 + 1.0 * v2 + 0.0 * v3;

	return (a0 * mu3 + a1 * mu2 + a2 * mu + a3) / 6.0;
}

void GradientCalculator::calcGradent()
{
	if (m_sliceModel == nullptr)
		return;
	init();
	const int ziSize = m_sliceModel->topSliceCount();
	const int yiSize = m_sliceModel->rightSliceCount();
	const int xiSize = m_sliceModel->frontSliceCount();
	const auto pOriginalData = m_sliceModel->constData();

	//QMutexLocker locker(&m_mutex);
	if (m_gradient == nullptr)
		return;
	// calculate gradient and the normalized direction
#pragma omp parallel for
	for (int i = 0; i < ziSize; ++i) {
		for (int j = 0; j < yiSize; ++j) {
			for (int k = 0; k < xiSize; ++k) {
				unsigned int index = i * xiSize * yiSize + j * xiSize + k;
				
				// for 16 bit, already convert to 8 bit
				m_gradient[index * 4 + 3] = pOriginalData[index];
				QVector3D gradient = triCubicIntpGrad(pOriginalData, k, j, i);		// x, y, z
				if (gradient.lengthSquared() > 1e-10) {
					gradient.normalize();
					m_gradient[index * 4 + 0] = (unsigned char)((gradient.x() + 1.0) / 2.0 * 255 + 0.5);
					m_gradient[index * 4 + 1] = (unsigned char)((gradient.y() + 1.0) / 2.0 * 255 + 0.5);
					m_gradient[index * 4 + 2] = (unsigned char)((gradient.z() + 1.0) / 2.0 * 255 + 0.5);
				}
				else {	// gradient = (0, 0, 0)
					m_gradient[index * 4 + 0] = 128;
					m_gradient[index * 4 + 1] = 128;
					m_gradient[index * 4 + 2] = 128;
				}
			}
		}
	}

	emit finished();
}




VolumeWidget::VolumeWidget(AbstractSliceDataModel * dataModel, MarkModel * markModel, QWidget * parent)
	:QOpenGLWidget(parent),
	m_dataModel(dataModel),
	m_markModel(markModel),
	m_rayStep(0.05),
	m_camera(QVector3D(0.f,0.f,5.f)),
	m_gradCalc(dataModel,markModel),
#ifdef USE_QT_FRAMEBUFFEROBJECT
	m_fbo(nullptr),
	//m_FBO(nullptr),
#endif
#ifdef USE_QT_TEXTUREOBJECT
	m_volumeTexture(nullptr),
#else
	m_volumeTextureId(0),
#endif
	m_positionEBO(QOpenGLBuffer::IndexBuffer),
	m_positionVBO(QOpenGLBuffer::VertexBuffer)
{

}

void VolumeWidget::setDataModel(AbstractSliceDataModel * model)
{
	m_dataModel = model;
	updateVolumeData();
	emit dataModelChanged();
	update();
}

void VolumeWidget::setMarkModel(MarkModel* model)
{
	m_markModel = model;
	updateMarkData();
	emit markModelChanged();
	update();
}

//ShaderDataInterface


QSize VolumeWidget::minimumSizeHint() const
{
	return QSize(50, 50);
}

QSize VolumeWidget::sizeHint() const
{
	return QSize(800, 600);
}

VolumeWidget::~VolumeWidget()
{

}

void VolumeWidget::initializeGL()
{
	initializeOpenGLFunctions();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &VolumeWidget::cleanup);


	glEnable(GL_DEPTH_TEST);
	// initiliazeTextures();
	// 1D Texture
	m_tfTexture.reset(new QOpenGLTexture(QOpenGLTexture::Target1D));
	m_tfTexture->setMagnificationFilter(QOpenGLTexture::Linear);
	m_tfTexture->setMinificationFilter(QOpenGLTexture::Linear);
	m_tfTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
	m_tfTexture->setSize(256);
	m_tfTexture->setFormat(QOpenGLTexture::RGBA16_UNorm);		//Equivalent to GL_RGBA16
	m_tfTexture->allocateStorage();
	Q_ASSERT_X(m_tfTexture->isCreated(), 
		"VolumeWidget::initializeGL", "TF texture creating failed.");

	

	//initilize framebuffer object
#ifndef USE_QT_FRAMEBUFFEROBJECT

	glGenFramebuffers(1, &m_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);



	glEnable(GL_TEXTURE_RECTANGLE_NV);
	glGenTextures(1, &m_startFaceTextureId);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_startFaceTextureId);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_NV, m_startFaceTextureId, 0);

	glGenTextures(1, &m_endFaceTextureId);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_endFaceTextureId);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE_NV, m_endFaceTextureId, 0);

	glGenTextures(1, &m_depthTextureId);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_depthTextureId);
	glDisable(GL_TEXTURE_RECTANGLE_NV);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE_ARB, m_depthTextureId, 0);

#endif

#ifndef USE_QT_TEXTUREOBJECT
	glEnable(GL_TEXTURE_3D);
	glGenTextures(1, &m_volumeTextureId);
	glBindTexture(GL_TEXTURE_3D, m_volumeTextureId);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#endif

	//initialize position shader
	m_positionShader.reset(new PositionShader);
	m_positionShader->link();
	Q_ASSERT_X(m_positionShader->isLinked(), "VolumeWidget::initializeGL", "positionShader linking failed.");
	//initialize position vao and vbo
	m_positionVAO.create();
	Q_ASSERT_X(m_positionVAO.isCreated(), "VolumeWidget::initializeGL", "VAO is not created.");
	QOpenGLVertexArrayObject::Binder positionVAOBinder(&m_positionVAO);
	m_positionVBO.create();
	m_positionVBO.bind();
	m_positionVBO.allocate(16 * 3 * sizeof(float));
	//Q_ASSERT_X(m_positionVBO.isCreated(), "VolumeWidget::updateVolumeData", "VBO is not created");
	//m_positionVBO.write(0, positionVert, sizeof(positionVert));
	//m_positionVBO.allocate(positionVert.constData(), positionVert.count() * 3 * sizeof(float));
	//setup vertex attribute

	m_positionShader->bind();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));	//vertex
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));	//texture coord

	m_positionEBO.create();
	m_positionEBO.bind();
	m_positionEBO.allocate(faceIndex, sizeof(faceIndex));

	m_positionVBO.release();
	//initilize ray casting shader
	m_currentShader.reset(new RayCastingShader);
	m_currentShader->link();
	Q_ASSERT_X(m_currentShader->isLinked(), "VolumeWidget::initializeGL", "currentShader linking failed.");
	//intialize ray casting vao and vbo
	m_currentShader->bind();
	m_rayCastingTextureVAO.create();
	QOpenGLVertexArrayObject::Binder binder(&m_rayCastingTextureVAO);
	m_rayCastingTextureVBO.create();
	m_rayCastingTextureVBO.bind();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<void*>(2*sizeof(GLfloat)));
	m_rayCastingTextureVBO.allocate(16 * sizeof(GLfloat));
	m_rayCastingTextureVBO.release();
}

void VolumeWidget::resizeGL(int w, int h)
{
	double aspect = GLfloat(w) / h;
	m_otho.ortho(-aspect * 2, aspect * 2, -2, 2, -10.0, 10.0);
	m_proj.perspective(45.f, aspect, 0.01f, 100.f);

	///TODO :: resize framebuffer

#ifdef USE_QT_FRAMEBUFFEROBJECT
	m_fbo.reset(new QOpenGLFramebufferObject(w,h));
	m_fbo->addColorAttachment(w, h);
#else
	glEnable(GL_TEXTURE_RECTANGLE_NV);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_startFaceTextureId);
	glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA32F_ARB, w, h, 0, GL_RGBA, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_endFaceTextureId);
	glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA32F_ARB, w, h, 0, GL_RGBA, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, m_depthTextureId);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_DEPTH_COMPONENT,w,h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glDisable(GL_TEXTURE_RECTANGLE_NV);
#endif
	//if (m_FBO == nullptr) {
	//	m_FBO.reset( new FramebufferObject());
	//	m_FBO->Bind();
	//	m_FBO->AttachTexture(GL_TEXTURE_RECTANGLE_NV, m_startFaceTextureId, GL_COLOR_ATTACHMENT0_EXT);
	//	m_FBO->AttachTexture(GL_TEXTURE_RECTANGLE_NV, m_endFaceTextureId, GL_COLOR_ATTACHMENT1_EXT);
	//	m_FBO->AttachTexture(GL_TEXTURE_RECTANGLE_ARB, m_depthTextureId, GL_DEPTH_ATTACHMENT_EXT);
	//	m_FBO->IsValid();
	//	FramebufferObject::Disable();
	//}


	//update texture size in ray casting vbo

	static QVector<QVector2D> rayCastingVB = { 
		{0.0f,0.0f},{0.0f,0.0f},		//rect texture coord and vertex coord
		{(float)w,0.0f},{ (float)w,0.0f},
		{ (float)w,(float)h},{ (float)w,(float)h},
		{0.0f,(float)h},{0.0f,(float)h}
	};
	m_rayCastingTextureVBO.bind();
	m_rayCastingTextureVBO.write(0, rayCastingVB.constData(), 16 * sizeof(GLfloat));
	m_rayCastingTextureVBO.release();
}

void VolumeWidget::paintGL()
{
	//if (m_volumeTexture == nullptr)
	//	return;
	//Q_ASSERT_X(m_volumeTexture->isCreated(), 
	//	"VolumeWidget::paintGL", "Volume texture is no created yet.");
	//Q_ASSERT_X(m_fbo, "VolumeWidget::paintGL", "FBO is invalid.");

	// Create position FrameBuffer
	
#ifdef USE_QT_FRAMEBUFFEROBJECT
	m_fbo->bind();
#else
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
#endif
	//m_FBO->Bind();
	m_positionShader->load(this);
	QOpenGLVertexArrayObject::Binder binder1(&m_positionVAO);
	// Front
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT,0);
	// Back
	glDrawBuffer(GL_COLOR_ATTACHMENT1);
	glClear(GL_COLOR_BUFFER_BIT);
	glDepthFunc(GL_GREATER);

	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT,0);

	glDepthFunc(GL_LESS);
	m_positionShader->release();
	//m_FBO->IsValid();
	//FramebufferObject::Disable();

#ifdef USE_QT_FRAMEBUFFEROBJECT
	m_fbo->release();
#else
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
	//m_fbo->toImage().save("C:\\Users\\ysl\\Desktop\\iii\\image.jpg");
	//Ray Casting in shader

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

void VolumeWidget::mousePressEvent(QMouseEvent* event)
{
	m_lastPos = event->pos();
	update();
}
void VolumeWidget::mouseMoveEvent(QMouseEvent* event)
{
	const auto & p = event->pos();
	float dx = p.x() - m_lastPos.x();
	float dy = m_lastPos.y() - p.y();
	if((event->buttons() &Qt::LeftButton) && (event->buttons() & Qt::RightButton))
	{
		auto direction = m_camera.up()*dy + m_camera.right()*dx;
		m_camera.movement(direction, 0.01);
	}
	else if(event->buttons() & Qt::LeftButton)
	{
		m_camera.rotation(dx,dy, QVector3D(0, 0, 0));
	}else if(event->buttons() == Qt::RightButton)
	{
		auto direction = m_camera.front()*dy;
		m_camera.movement(direction, 0.01);
	}
	m_lastPos = p;
	update();
}


void VolumeWidget::updateTransferFunction(const float * func)
{
	makeCurrent();
	//glEnable(GL_TEXTURE_1D);
	//glBindTexture(GL_TEXTURE_1D, m_tfTextureId);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA16, 256, 0, GL_RGBA, GL_FLOAT, func);
	//glDisable(GL_TEXTURE_1D);
	m_tfTexture->setData(QOpenGLTexture::RGBA, QOpenGLTexture::Float32,func);	//Equivalent to GL_RGBA and GL_FLOAT
	GLERROR("update tf failed");
	qDebug() << "update Transfer Functions";
	doneCurrent();
	update();
}
/**
 * \fn	void VolumeWidget::updateVolumeData()
 *
 * \brief	Updates the volume data
 *
 * \author	Ysl
 * \date	2018.07.19
 */

void VolumeWidget::updateVolumeData()
{
	m_gradCalc.setDataModel(m_dataModel);
	// update volume side length, position vertex vbo
	const auto z = (float)m_dataModel->topSliceCount()*0.01;
	const auto y = (float)m_dataModel->rightSliceCount()*0.01;
	const auto x = (float)m_dataModel->frontSliceCount()*0.01;

	static const float xCoord = 1.0, yCoord = 1.0, zCoord = 1.0;
	float positionVert[] = {
		0,0,0, 0,0,0 ,
		xCoord, 0, 0 , x,0,0 ,
		0, yCoord, 0 , 0,y,0 ,
		xCoord, yCoord, 0 ,x,y,0 ,
		0, 0, zCoord ,0,0,z ,
		xCoord, 0, zCoord , x,0,z ,
		0, yCoord, zCoord , 0,y,z ,
		xCoord, yCoord, zCoord , x,y,z 
	};
	m_positionVBO.bind();
	m_positionVBO.write(0, positionVert, sizeof(positionVert));
	m_positionVBO.release();
	loadDataToTextures();
}

/**
 * \fn	void VolumeWidget::updateMarkData()
 *
 * \brief	Updates the mark data
 *
 * \author	Ysl
 * \date	2018.07.19
 */

void VolumeWidget::updateMarkData()
{
	return;
}

void VolumeWidget::initializeShaders()
{

}

void VolumeWidget::initiliazeTextures()
{

}

void VolumeWidget::loadDataToTextures()
{
	if (m_gradCalc.ready() == false)
		return;

	qDebug() << "Starting calculating gradient of valume data.";
	m_gradCalc.calcGradent();		//Time-consuming 
	qDebug() << "Calculating gradient ended.";
	const auto d = m_gradCalc.data();
	const auto z = m_dataModel->topSliceCount();
	const auto y = m_dataModel->rightSliceCount();
	const auto x = m_dataModel->frontSliceCount();
	/////////
#ifndef USE_QT_TEXTUREOBJECT
	glEnable(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D,m_volumeTextureId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, x, y, z, 0, GL_RGBA, GL_UNSIGNED_BYTE, d);
	glDisable(GL_TEXTURE_3D);
#else
	//GLERROR("####$")
	m_volumeTexture.reset(new QOpenGLTexture(QOpenGLTexture::Target3D));
	m_volumeTexture->setMagnificationFilter(QOpenGLTexture::Linear);
	m_volumeTexture->setMinificationFilter(QOpenGLTexture::Linear);
	m_volumeTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
	m_volumeTexture->setSize(x, y, z);
	m_volumeTexture->setFormat(QOpenGLTexture::RGBA8_UNorm);
	m_volumeTexture->bind();
	m_volumeTexture->setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,d);
#endif
	/*Q_ASSERT_X(m_volumeTexture->isCreated(),
		"VolumeWidget::loadDataToTextures", "Loading failed.");*/
	//glEnable(GL_TEXTURE_1D);
	//glBindTexture(GL_TEXTURE_1D, m_tfTextureId);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA16,256,0,GL_RGBA,GL_FLOAT,nullptr);
	////////
}

void VolumeWidget::cleanup()
{
	makeCurrent();
#ifdef TESTCUBE
	m_cubeVBO.destroy();
	m_cubeVAO.destroy();
#endif
	doneCurrent();
}
