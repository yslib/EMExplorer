#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QMenu>

#include "volumewidget.h"
#include "abstract/abstractslicedatamodel.h"
#include "model/markmodel.h"
#include "model/shaderprogram.h"
#include "model/raycastingshader.h"

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
const static int faceIndex[] = { 1, 3, 7, 5, 0, 4, 6, 2, 2, 6, 7, 3, 0, 1, 5, 4, 4, 5, 7, 6, 0, 2, 3, 1 };


static const float xCoord = 1.0, yCoord = 1.0, zCoord = 1.0;
float positionVert[] = {
	0,0,0,
	xCoord, 0, 0 ,
	0, yCoord, 0 ,
	xCoord, yCoord, 0 ,
	0, 0, zCoord ,
	xCoord, 0, zCoord ,
	0, yCoord, zCoord ,
	xCoord, yCoord, zCoord ,
};

const static int triIndex[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35 };

static QVector<QVector2D> cubeTex = {
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

static QVector<QVector3D> cubeNor = {
{ 0.f,0.f,-1.f },
{ 0.f,0.f,-1.f },
{ 0.f,0.f,-1.f },
{ 0.f,0.f,-1.f },
{ 0.f,0.f,-1.f },
{ 0.f,0.f,-1.f },
{ 0.f,0.f,1.f },
{ 0.f,0.f,1.f },
{ 0.f,0.f,1.f },
{ 0.f,0.f,1.f },
{ 0.f,0.f,1.f },
{ 0.f,0.f,1.f },
{ -1.0f,0.f,0.f },
{ -1.0f,0.f,0.f },
{ -1.0f,0.f,0.f },
{ -1.0f,0.f,0.f },
{ -1.0f,0.f,0.f },
{ -1.0f,0.f,0.f },
{ 1.0f,0.f,0.f },
{ 1.0f,0.f,0.f },
{ 1.0f,0.f,0.f },
{ 1.0f,0.f,0.f },
{ 1.0f,0.f,0.f },
{ 1.0f,0.f,0.f },
{ 0.f,-1.f,0.f },
{ 0.f,-1.f,0.f },
{ 0.f,-1.f,0.f },
{ 0.f,-1.f,0.f },
{ 0.f,-1.f,0.f },
{ 0.f,-1.f,0.f },
{ 0.f,1.f,0.f },
{ 0.f,1.f,0.f },
{ 0.f,1.f,0.f },
{ 0.f,1.f,0.f },
{ 0.f,1.f,0.f },
{ 0.f,1.f,0.f },
};


static QVector<QVector3D> cubeVertex =
{
	//back
{ 0.5f, -0.5f, -0.5f },
{ -0.5f, -0.5f, -0.5f },
{ 0.5f,  0.5f, -0.5f },
{ -0.5f,  0.5f, -0.5f } ,
{ 0.5f,  0.5f, -0.5f } ,
{ -0.5f, -0.5f, -0.5f } ,
//front
{ -0.5f, -0.5f,  0.5f },
{ 0.5f, -0.5f,  0.5f },
{ 0.5f,  0.5f,  0.5f },
{ 0.5f,  0.5f,  0.5f },
{ -0.5f,  0.5f,  0.5f },
{ -0.5f, -0.5f,  0.5f },
//left
{ -0.5f,  0.5f,  0.5f },
{ -0.5f,  0.5f, -0.5f },
{ -0.5f, -0.5f, -0.5f },
{ -0.5f, -0.5f, -0.5f },
{ -0.5f, -0.5f,  0.5f },
{ -0.5f,  0.5f,  0.5f },
//right
{ 0.5f,  0.5f, -0.5f },
{ 0.5f,  0.5f,  0.5f },
{ 0.5f, -0.5f, -0.5f } ,
{ 0.5f, -0.5f,  0.5f } ,
{ 0.5f, -0.5f, -0.5f } ,
{ 0.5f,  0.5f,  0.5f } ,
//bottom
{ -0.5f, -0.5f, -0.5f },
{ 0.5f, -0.5f, -0.5f },
{ 0.5f, -0.5f,  0.5f },
{ 0.5f, -0.5f,  0.5f } ,
{ -0.5f, -0.5f,  0.5f } ,
{ -0.5f, -0.5f, -0.5f }  ,
//up
{ 0.5f,  0.5f, -0.5f }  ,
{ -0.5f,  0.5f, -0.5f }  ,
{ 0.5f,  0.5f,  0.5f }  ,
{ -0.5f,  0.5f,  0.5f } ,
{ 0.5f,  0.5f,  0.5f } ,
{ -0.5f,  0.5f, -0.5f }
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
VolumeWidget::VolumeWidget(AbstractSliceDataModel * dataModel, MarkModel * markModel, RenderParameterWidget * widget,QWidget * parent)
	:QOpenGLWidget(parent),
	m_markModel(markModel),
	m_dataModel(dataModel),
	m_parameterWidget(widget),
	m_camera(QVector3D(0.f, 0.f, 10.f)),
	m_rayStep(0.02),
	m_scale(1.0, 1.0, 1.0),
	m_trans(0.0, 0.0, 0.0),
	m_tfTexture(QOpenGLTexture::Target1D),
	m_gradCalc(dataModel, markModel),
	m_fbo(nullptr),
	m_gradientTexture(QOpenGLTexture::Target3D),
	m_volumeTexture(QOpenGLTexture::Target3D),
	m_positionVBO(QOpenGLBuffer::VertexBuffer),
	m_positionEBO(QOpenGLBuffer::IndexBuffer)
{
	m_contextMenu = new QMenu(QStringLiteral("Context Menu"), this);
	Q_ASSERT_X(widget != nullptr, "VolumeWidget::VolumeWidget", "null pointer");
	connect(widget, &RenderParameterWidget::optionsChanged, [this]() {update(); });
	
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
	return QSize(400, 300);
}

QSize VolumeWidget::sizeHint() const
{
	return QSize(800, 600);
}

void VolumeWidget::addContextAction(QAction* action)
{
	m_contextMenu->addAction(action);
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

	// initialize cube 
	m_cubeShader.reset(new QOpenGLShaderProgram);
	m_cubeShader->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
	m_cubeShader->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
	m_cubeShader->link();


	//test mesh
	Transform3 trans;
	trans.translate(0.5, 0.5, 0.5);
	m_cube.reset(new TriangleMesh(cubeVertex.constData(), 
		cubeNor.constData(),
		cubeTex.constData(), 
		36, 
		triIndex, 
		12, 
		trans));

	// Default matrix parameters
	m_scale = QVector3D(1.0, 1.0, 1.0);
	m_trans = QVector3D(0, 0, 0);
	m_trans -= m_scale / 2;
	m_world.setToIdentity();
	m_world.scale(m_scale);
	m_world.translate(m_trans);


	//slice rendering test
	m_sliceShader.reset(new SliceShader());
	m_sliceShader->link();

	m_topSliceVAO.create();
	QOpenGLVertexArrayObject::Binder binder1(&m_topSliceVAO);
	m_topSliceVBO.create();
	m_topSliceVBO.bind();
	m_topSliceVBO.allocate(4 * 3 * sizeof(float) + 4 * 2 * sizeof(float));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D) + sizeof(QVector2D), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QVector3D) + sizeof(QVector2D), reinterpret_cast<void*>(3*sizeof(float)));

	m_rightSliceVAO.create();
	QOpenGLVertexArrayObject::Binder binder2(&m_rightSliceVAO);
	m_rightSliceVBO.create();
	m_rightSliceVBO.bind();
	m_rightSliceVBO.allocate(4 *3* sizeof(float) + 4 *2* sizeof(float));
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

	// 3D Volume Texture
	loadDataToTextures();

	// Update transfer functions
	emit requireTransferFunction();
}

//#define EXPORT_FBO_IMG

void VolumeWidget::resizeGL(int w, int h)
{
	// Update projection matrices
	double aspect = GLfloat(w) / h;
	m_proj.setToIdentity();
	m_otho.setToIdentity();
	m_otho.ortho(-aspect * 2, aspect * 2, -2, 2, -100.0, 100.0);
	m_proj.perspective(45.f, aspect, 0.01f, 100.f);

	// Update FBO size
	m_fbo.reset(new QOpenGLFramebufferObject(w, h, QOpenGLFramebufferObject::Depth, GL_TEXTURE_RECTANGLE_NV, GL_RGBA32F_ARB));
	m_fbo->addColorAttachment(w, h);

	// Update texture size in ray casting vbo
	static QVector<QVector2D> rayCastingVB = {
		{0.0f,0.0f},
		{0.0,static_cast<float>(h)},
		{ static_cast<float>(w),static_cast<float>(h)},
		{static_cast<float>(w),0.0}
	};
	m_rayCastingTextureVBO.bind();
	m_rayCastingTextureVBO.write(0, rayCastingVB.constData(), rayCastingVB.count() * 2 * sizeof(GLfloat));
	m_rayCastingTextureVBO.release();
}
void VolumeWidget::paintGL()
{

	Q_ASSERT_X(m_parameterWidget != nullptr, "VolumeWidget::paintGL", "null pointer");

	auto renderMode = m_parameterWidget->options()->mode;

	if(renderMode == RenderMode::DVR) {
		m_fbo->bind();
		m_positionShader->load(this);
		m_positionShader->setUniformValue("viewMatrix", m_camera.view());
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
	}else if(renderMode == (RenderMode::LineMesh & RenderMode::SliceTexture)) {
		// Update Slice
		glClear(GL_DEPTH_BUFFER_BIT);
		if (m_dataModel != nullptr) {

			int nTop = m_dataModel->topSliceCount();
			int nRight = m_dataModel->rightSliceCount();
			int nFront = m_dataModel->frontSliceCount();

			//m_topSlice = 75;
			//m_rightSlice = 75;
			//m_frontSlice = 75;

			float topCoord = float(m_topSlice) / float(nTop) ;
			float rightCoord = float(m_rightSlice) / float(nRight) ;
			float frontCoord = float(m_frontSlice) / float(nFront) ;

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
			m_sliceShader->setSliceIndex(m_topSlice);
			m_sliceShader->load(this);
			m_sliceShader->setUniformValue("projMatrix", m_proj);
			m_sliceShader->setUniformValue("viewMatrix", m_camera.view());
			m_sliceShader->setUniformValue("worldMatrix", m_world);

			m_topSliceVAO.bind();
			glDrawArrays(GL_QUADS, 0, 4);

			m_sliceShader->setSliceType(1);
			m_sliceShader->setSliceIndex(m_rightSlice);
			m_sliceShader->load(this);
			m_rightSliceVAO.bind();
			glDrawArrays(GL_QUADS, 0, 4);

			m_sliceShader->setSliceType(2);
			m_sliceShader->setSliceIndex(m_frontSlice);
			m_sliceShader->load(this);
			m_frontSliceVAO.bind();
			glDrawArrays(GL_QUADS, 0, 4);
			m_sliceShader->release();

			//auto shader = static_cast<QScopedPointer<SliceShader>>(m_sliceShader);
		}

		// Draw Slice

		//glClear(GL_DEPTH_BUFFER_BIT);
		m_cubeShader->bind();
		m_cubeShader->setUniformValue("projMatrix", m_proj);
		m_cubeShader->setUniformValue("mvMatrix", m_camera.view()*m_world);
		m_cube->render();

		for (auto & m : m_markMeshes)
			m->render();

		m_cubeShader->release();
	}
	



	
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
	if ((event->buttons() &Qt::LeftButton) && (event->buttons() & Qt::RightButton))
	{
		auto direction = m_camera.up()*dy + m_camera.right()*dx;
		m_camera.movement(direction, 0.01);
	}
	else if (event->buttons() & Qt::LeftButton)
	{
		m_camera.rotation(dx, dy, QVector3D(0, 0, 0));
	}
	else if (event->buttons() == Qt::RightButton)
	{
		auto direction = m_camera.front()*dy;
		m_camera.movement(direction, 0.01);
	}
	m_lastPos = p;
	update();
}

void VolumeWidget::contextMenuEvent(QContextMenuEvent* event)
{
	const auto pos = event->pos();
	m_contextMenu->exec(this->mapToGlobal(pos));
}


void VolumeWidget::updateTransferFunction(const float * func, bool updated)
{
	makeCurrent();
	if (m_tfTexture.isCreated() == false)
	{
		m_tfTexture.setMagnificationFilter(QOpenGLTexture::Linear);
		m_tfTexture.setMinificationFilter(QOpenGLTexture::Linear);
		m_tfTexture.setWrapMode(QOpenGLTexture::ClampToEdge);
		m_tfTexture.setSize(256);
		m_tfTexture.setFormat(QOpenGLTexture::RGBA16_UNorm);		//Equivalent to GL_RGBA16
		m_tfTexture.allocateStorage();
	}
	m_tfTexture.setData(QOpenGLTexture::RGBA, QOpenGLTexture::Float32, func);	//Equivalent to GL_RGBA and GL_FLOAT
	doneCurrent();
	if (updated)
		update();
}

void VolumeWidget::updateMarkMesh() {
	//TODO:: update m_markMeshes
	if (m_markModel == nullptr)
		return;

}



void VolumeWidget::updateVolumeData()
{
	m_gradCalc.setDataModel(m_dataModel);
	//// update volume side length, position vertex vbo
	auto z = m_dataModel->topSliceCount();
	auto y = m_dataModel->rightSliceCount();
	auto x = m_dataModel->frontSliceCount();
	m_topSlice = z;
	m_rightSlice = y;
	m_frontSlice = x;
	//z *= 0.01;
	//y *= 0.01;
	//x *= 0.01;
	m_scale = QVector3D(x, y, z);
	m_scale.normalize();
	m_trans = QVector3D(0, 0, 0);
	m_trans -= m_scale / 2;
	m_world.setToIdentity();
	m_world.scale(m_scale);
	m_world.translate(m_trans);

	m_sliceShader->setSliceDataModel(m_dataModel);

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
	if (m_gradCalc.hasData() == false)
		return;
	if (m_gradCalc.ready() == false)
		m_gradCalc.calcGradent();		//Time-consuming
	const auto d = m_gradCalc.data3();
	const auto z = m_dataModel->topSliceCount();
	const auto y = m_dataModel->rightSliceCount();
	const auto x = m_dataModel->frontSliceCount();

	makeCurrent();
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
	m_volumeTexture.setData(QOpenGLTexture::Red, QOpenGLTexture::UInt8, m_dataModel->constData());

	//update slice vbo

	doneCurrent();
}

void VolumeWidget::contextMenuAddedHelper(QWidget* widget) {

}

void VolumeWidget::cleanup()
{
	makeCurrent();
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

	m_tfTexture.destroy();
	m_gradientTexture.destroy();
	m_fbo.reset();
	m_cube.reset();

	for (auto & mesh : m_markMeshes)
		mesh.reset();

	doneCurrent();
}
