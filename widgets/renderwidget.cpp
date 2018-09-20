#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QMenu>

#include "renderwidget.h"
#include "abstract/abstractslicedatamodel.h"
#include "model/markmodel.h"
#include "3drender/shader/shaderprogram.h"
#include "3drender/shader/raycastingshader.h"
#include "3drender/geometry/slicevolume.h"
#include "algorithm/triangulate.h"
#include "globals.h"
#include "model/markitem.h"





#define GLERROR(str)									\
	{													\
		GLenum err;										\
		while ((err = glGetError()) != GL_NO_ERROR)		\
		{												\
			std::cout<<err<<" "<<str<<std::endl;		\
		}												\
	}													\



RenderWidget::RenderWidget(AbstractSliceDataModel * dataModel, MarkModel * markModel, RenderParameterWidget * widget, QWidget * parent)
	:QOpenGLWidget(parent),
	m_markModel(markModel),
	m_dataModel(dataModel),
	m_parameterWidget(widget),
	m_camera(QVector3D(0.f, 0.f, 10.f)),
	m_rayStep(0.02),
	m_tfTexture(QOpenGLTexture::Target1D)
{
	m_contextMenu = new QMenu(QStringLiteral("Context Menu"), this);
	Q_ASSERT_X(widget != nullptr, "VolumeWidget::VolumeWidget", "null pointer");
	connect(widget, &RenderParameterWidget::optionsChanged, [this]() {update(); });
	connect(widget, &RenderParameterWidget::markUpdated,this,&RenderWidget::updateMark);
}

void RenderWidget::setDataModel(AbstractSliceDataModel * model)
{
	m_dataModel = model;
	updateVolumeData();
	emit dataModelChanged();
	update();
}

void RenderWidget::setMarkModel(MarkModel* model)
{
	m_markModel = model;
	updateMarkData();
	emit markModelChanged();
	update();
}

//ShaderDataInterface

QSize RenderWidget::minimumSizeHint() const
{
	return QSize(400, 300);
}

QSize RenderWidget::sizeHint() const
{
	return QSize(800, 600);
}

void RenderWidget::addContextAction(QAction* action)
{
	m_contextMenu->addAction(action);
}

RenderWidget::~RenderWidget()
{

}

void RenderWidget::initializeGL()
{
	initializeOpenGLFunctions();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &RenderWidget::cleanup);
	glEnable(GL_DEPTH_TEST);
	

	// Update transfer functions
	emit requireTransferFunction();

	if (m_volume != nullptr)
		m_volume->initializeGLResources();

	for (auto & item : m_markMeshes)
		item->initializeGLResources();
}

//#define EXPORT_FBO_IMG

void RenderWidget::resizeGL(int w, int h)
{
	// Update projection matrices
	double aspect = GLfloat(w) / h;
	m_proj.setToIdentity();
	m_otho.setToIdentity();
	m_otho.ortho(-aspect * 2, aspect * 2, -2, 2, -100.0, 100.0);
	m_proj.perspective(45.f, aspect, 0.01f, 100.f);

	emit windowResized(w, h);
}
void RenderWidget::paintGL()
{
	Q_ASSERT_X(m_parameterWidget != nullptr, "VolumeWidget::paintGL", "null pointer");

	auto renderMode = m_parameterWidget->options()->mode;

	if(m_volume != nullptr) {
		if(renderMode == RenderMode::DVR)
			m_volume->sliceMode(false);
		else
			m_volume->sliceMode(true);
		m_volume->render();
	}

	for (auto & mesh : m_markMeshes) {
		mesh->render();
	}

}

void RenderWidget::mousePressEvent(QMouseEvent* event)
{
	m_lastPos = event->pos();
	update();
}

void RenderWidget::mouseMoveEvent(QMouseEvent* event)
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

void RenderWidget::contextMenuEvent(QContextMenuEvent* event)
{
	const auto pos = event->pos();
	m_contextMenu->exec(this->mapToGlobal(pos));
}


void RenderWidget::updateTransferFunction(const float * func, bool updated)
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

void RenderWidget::updateMarkMesh() {
	//TODO:: update m_markMeshes
	if (m_markModel == nullptr)
		return;

}



void RenderWidget::updateMark() {
	qDebug() << "RenderWidget::updateMark";
	if(m_markModel == nullptr) {
		qDebug() << "Mark Model is empty";
		return;
	}
	m_markMeshes.clear();

	const auto cates = m_markModel->categoryText();
	Transform3 trans;
	trans.setToIdentity();
	trans.scale(0.001, 0.001, 0.001);

	for(const auto & c:cates) {
		const auto mesh = m_markModel->markMesh(c);
		Q_ASSERT_X(mesh->isReady(), "RenderWidget::updateMark", "Mesh not ready");
		const auto v = mesh->vertices();
		const auto idx = mesh->indices();
		const auto nV = mesh->vertexCount();
		const auto nT = mesh->triangleCount();

		auto ptr = QSharedPointer<TriangleMesh>(new TriangleMesh(v, nullptr, nullptr, nV, idx, nT, trans));
		ptr->setPolyMode(true);
		makeCurrent();
		ptr->initializeGLResources();
		doneCurrent();
		m_markMeshes.push_back(ptr);
	}
}

void RenderWidget::updateVolumeData()
{
	if (m_dataModel == nullptr)
		return;

	auto z = m_dataModel->topSliceCount();
	auto y = m_dataModel->rightSliceCount();
	auto x = m_dataModel->frontSliceCount();

	QVector3D m_scale = QVector3D(x, y, z);
	m_scale.normalize();
	QVector3D m_trans = QVector3D(0, 0, 0);
	m_trans -= m_scale / 2;
	m_world.setToIdentity();
	m_world.scale(m_scale);
	m_world.translate(m_trans);
	m_volume.reset(new SliceVolume(m_dataModel, VolumeFormat(), this));
	makeCurrent();
	m_volume->initializeGLResources();
	doneCurrent();
}

/**
 * \fn	void VolumeWidget::updateMarkData()
 *
 * \brief	Updates the mark data
 *
 * \author	Ysl
 * \date	2018.07.19
 */

void RenderWidget::updateMarkData()
{

	//triangulate mark to mesh
}

void RenderWidget::contextMenuAddedHelper(QWidget* widget) {

}

void RenderWidget::cleanup()
{
	makeCurrent();
	m_tfTexture.destroy();

	m_volume->destoryGLResources();

	for (auto & mesh : m_markMeshes)
		mesh.reset();

	doneCurrent();
}
