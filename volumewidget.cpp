#include "volumewidget.h"
#include "abstractslicedatamodel.h"
#include "markmodel.h"
#include <QDebug>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>


static const char *vertexShaderSource =
"#version 150\n"
"in vec4 vertex;\n"
"in vec3 normal;\n"
"out vec3 vert;\n"
"out vec3 vertNormal;\n"
"uniform mat4 projMatrix;\n"
"uniform mat4 mvMatrix;\n"
"uniform mat3 normalMatrix;\n"
"void main() {\n"
"   vert = vertex.xyz;\n"
"   vertNormal = normalMatrix * normal;\n"
"   gl_Position = projMatrix * mvMatrix * vertex;\n"
"}\n";

static const char *fragmentShaderSource =
"#version 150\n"
"in highp vec3 vert;\n"
"in highp vec3 vertNormal;\n"
"out highp vec4 fragColor;\n"
"uniform highp vec3 lightPos;\n"
"void main() {\n"
"   highp vec3 L = normalize(lightPos - vert);\n"
"   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
"   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"
"   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
"   fragColor = vec4(0.3,0.6,0.9, 1.0);\n"
"}\n";


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

	QMutexLocker locker(&m_mutex);
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
	m_camera(QVector3D(0.f,0.f,5.f)),
	m_gradCalc(dataModel,markModel)
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

	m_program.reset(new QOpenGLShaderProgram);
	m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
	m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
	m_program->bindAttributeLocation("vertex", 0);
	m_program->bindAttributeLocation("normal", 1);

	m_program->link();
	m_program->bind();

	m_projMatLoc = m_program->uniformLocation("projMatrix");
	m_modelViewMatLoc = m_program->uniformLocation("mvMatrix");
	m_normalMatrixLoc = m_program->uniformLocation("normalMatrix");
	m_lightPosLoc = m_program->uniformLocation("lightPos");

	m_cubeVAO.create();
	QOpenGLVertexArrayObject::Binder cubeVAOBinder(&m_cubeVAO);
	m_cubeVBO.create();
	m_cubeVBO.bind();
	m_cubeVBO.allocate(cubeVert.constData(), 36*6*sizeof(float));
	//setup vertex attribute
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));
	m_cubeVBO.release();
	m_program->setUniformValue(m_lightPosLoc,QVector3D(0,0,70));
}

void VolumeWidget::resizeGL(int w, int h)
{
	double aspect = GLfloat(w) / h;
	m_proj.setToIdentity();
	//m_proj.ortho(-aspect * 2, aspect * 2, -2, 2, -10.0, 10.0);
	m_proj.perspective(45.f, aspect, 0.01f, 100.f);
}

void VolumeWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	m_world.setToIdentity();
	m_world.scale(QVector3D(0.5,0.5,0.5));
	QOpenGLVertexArrayObject::Binder binder(&m_cubeVAO);
	m_program->bind();
	m_program->setUniformValue(m_projMatLoc, m_proj);
	m_program->setUniformValue(m_modelViewMatLoc, m_camera.view()*m_world);
	const QMatrix3x3 normalMatrix = m_world.normalMatrix();
	m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, 36);
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
	glEnable(GL_TEXTURE_1D);
	glBindTexture(GL_TEXTURE_1D, m_tfTextureId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA16, 256, 0, GL_RGBA, GL_FLOAT, func);
	glDisable(GL_TEXTURE_1D);
	doneCurrent();
}

void VolumeWidget::drawCube()
{

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
	return;
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
	//1D Texture for transfer function
	glEnable(GL_TEXTURE_1D);
	glGenTextures(1, &m_tfTextureId);
	glBindTexture(GL_TEXTURE_1D, m_tfTextureId);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glDisable(GL_TEXTURE_1D);
	//3D Texture for volume data 
	glEnable(GL_TEXTURE_3D);
	glGenTextures(1, &m_volumeTextureId);
	glBindTexture(GL_TEXTURE_3D, m_volumeTextureId);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//3D Texture for gradient
	glGenTextures(1, &m_gradientTextureId);
	glBindTexture(GL_TEXTURE_3D, m_gradientTextureId);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glDisable(GL_TEXTURE_3D);
	//2D Texture for start position
	glEnable(GL_TEXTURE_RECTANGLE_NV);
	glGenTextures(1, &m_startFaceTextureId);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_startFaceTextureId);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// 2D Texture for end position 
	glGenTextures(1, &m_endFaceTextureId);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_endFaceTextureId);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glDisable(GL_TEXTURE_RECTANGLE_NV);

}

void VolumeWidget::loadDataToTextures()
{
	if (m_gradCalc.ready() == false)
		return;
	m_gradCalc.calcGradent();
	const auto d = m_gradCalc.data();
	const auto z = m_dataModel->topSliceCount();
	const auto y = m_dataModel->topSliceCount();
	const auto x = m_dataModel->topSliceCount();

	glEnable(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D,m_volumeTextureId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, x, y, z, 0, GL_RGBA, GL_UNSIGNED_BYTE, d);
	glDisable(GL_TEXTURE_3D);

	glEnable(GL_TEXTURE_1D);
	glBindTexture(GL_TEXTURE_1D, m_tfTextureId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA16,256,0,GL_RGBA,GL_FLOAT,nullptr);

}

void VolumeWidget::cleanup()
{
	makeCurrent();
	//m_program.reset();
	m_cubeVBO.destroy();
	m_cubeVAO.destroy();
	doneCurrent();
}
