#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>
#include <QMenu>

#include "renderwidget.h"
#include "abstract/abstractslicedatamodel.h"
#include "model/markmodel.h"
#include "3drender/shader/shaderprogram.h"
#include "3drender/geometry/slicevolume.h"
#include "algorithm/triangulate.h"
#include "globals.h"
#include "model/markitem.h"


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
{ -0.5f, -0.5f,  0.5f },{ 0.f,0.f,1.f },
{ 0.5f, -0.5f,  0.5f },{ 0.f,0.f,1.f },
{ 0.5f,  0.5f,  0.5f },{ 0.f,0.f,1.f },
{ 0.5f,  0.5f,  0.5f },{ 0.f,0.f,1.f },
{ -0.5f,  0.5f,  0.5f },{ 0.f,0.f,1.f },
{ -0.5f, -0.5f,  0.5f },{ 0.f,0.f,1.f },
//left
{ -0.5f,  0.5f,  0.5f },{ -1.0f,0.f,0.f },
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

#define GLERROR(str)									\
	{													\
		GLenum err;										\
		while ((err = glGetError()) != GL_NO_ERROR)		\
		{												\
			std::cout<<err<<" "<<str<<std::endl;		\
		}												\
	}													\

RenderWidget::RenderWidget(AbstractSliceDataModel * dataModel,
	MarkModel * markModel,
	RenderParameterWidget * widget, 
	QWidget * parent)
	:QOpenGLWidget(parent),
	m_markModel(markModel),
	m_dataModel(dataModel),
	m_parameterWidget(widget),
	m_camera(QVector3D(0.f, 0.f, 10.f)),
	m_rayStep(0.02),
	m_tfTexture(QOpenGLTexture::Target1D),
	m_volume(nullptr),
	m_selectMode(false)
{
	m_selectMode = true;
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
	if(initializeOpenGLFunctions() == false) {
		qFatal("initializeOpenGLFunctions failed");
		return;
	}
	glClearColor(1.0, 1.0, 1.0, 1.0);

	connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &RenderWidget::cleanup);
	glEnable(GL_DEPTH_TEST);

	// Update transfer functions
	emit requireTransferFunction();

	if (m_volume != nullptr)
		m_volume->initializeGLResources();

	for (auto & item : m_markMeshes)
		item->initializeGLResources();
	// mesh shader
	makeCurrent();
	m_meshShader.create();
	m_meshShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/resources/shaders/mesh_shader_v.glsl");
	m_meshShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/resources/shaders/mesh_shader_f.glsl");
	if(m_meshShader.link() == false) {
		qFatal("Mesh shader is not linked.");
		return;
	}
	// pick shader
	m_selectShader.create();
	m_selectShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/resources/shaders/pick_shader_v.glsl");
	m_selectShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/resources/shaders/pick_shader_f.glsl");
	if(m_selectShader.link() == false) {
		qFatal("Select shader is not linked.");
		return;
	}
	doneCurrent();

}

//#define EXPORT_FBO_IMG

void RenderWidget::resizeGL(int w, int h)
{
	// Update projection matrices
	const double aspect = GLfloat(w) / h;
	m_proj.setToIdentity();
	m_otho.setToIdentity();
	m_otho.ortho(-aspect * 2, aspect * 2, -2, 2, -100.0, 100.0);
	m_proj.perspective(45.f, aspect, 0.01f, 100.f);

	m_pickFBO.reset(new QOpenGLFramebufferObject(w,h, QOpenGLFramebufferObject::Depth, GL_TEXTURE_RECTANGLE_NV, GL_RGBA32F_ARB));

	emit windowResized(w, h);
}
void RenderWidget::paintGL()
{
	Q_ASSERT_X(m_parameterWidget != nullptr, "VolumeWidget::paintGL", "null pointer");
	const auto renderMode = m_parameterWidget->options()->mode;

	if(m_volume != nullptr) {
		if(renderMode == RenderMode::DVR)
			m_volume->sliceMode(false);
		else
			m_volume->sliceMode(true);
		m_volume->render();
	}

	if(renderMode != RenderMode::DVR) {

		const auto viewMatrix = camera().view();
		const auto cameraPos = camera().position();
		QMatrix4x4 world;
		world.setToIdentity();

		if(m_startSelect ==  true) {
			m_pickFBO->bind();
			//glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			m_selectShader.bind();
			m_selectShader.setUniformValue("viewMatrix", viewMatrix);
			m_selectShader.setUniformValue("projMatrix", m_proj);
			m_selectShader.setUniformValue("modelMatrix", world);
			for(int i=0;i<m_markMeshes.size();i++) {
				const auto color = idToColor(i);
				m_selectShader.setUniformValue("pickColor", color);
				//m_selectShader.setUniformValue("pickColor", color);
				m_markMeshes[i]->render();
			}
			m_selectShader.release();
			m_pickFBO->toImage().save("C:\\Users\\ysl\\Desktop\\fb\\pickfbo.jpg");
			m_pickFBO->release();

		}

		m_meshShader.bind();
		m_meshShader.setUniformValue("viewMatrix", viewMatrix);
		m_meshShader.setUniformValue("projMatrix", m_proj);
		m_meshShader.setUniformValue("modelMatrix", world);
		m_meshShader.setUniformValue("normalMatrix", world.normalMatrix());
		m_meshShader.setUniformValue("lightPos", cameraPos);
		m_meshShader.setUniformValue("viewPos", cameraPos); 
		
		for(int i=0;i<m_markMeshes.size();i++) {
			QColor color = m_markColor[i];
			if(i == m_selectedId) {
				color = m_markColor[i].lighter(150);
			}
			m_meshShader.setUniformValue("objectColor", color);
			m_markMeshes[i]->render();
		}

		m_meshShader.release();

	}

}

void RenderWidget::mousePressEvent(QMouseEvent* event)
{
	m_lastPos = event->pos();


}

void RenderWidget::mouseMoveEvent(QMouseEvent* event)
{
	const auto & p = event->pos();
	// Update Camera
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
	//

	update();

}

void RenderWidget::mouseReleaseEvent(QMouseEvent* event) {

	if (m_selectMode == true) {
		m_startSelect = true;
		repaint();

		const auto & p = event->pos();
		m_selectedId = selectMesh(p.x(), p.y());
		m_startSelect = false;
		qDebug() << m_selectedId;
		repaint();
		
	}
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
	if(m_markModel == nullptr || m_dataModel == nullptr) {
		return;
	}
	m_markMeshes.clear();

	const auto cates = m_markModel->categoryText();

	makeCurrent();
	//auto ptr = QSharedPointer<TriangleMesh>(new TriangleMesh(cubeVertex.constData(), cubeNor.constData(), cubeTex.constData(), 36, triIndex, 12, m_world, this));
	//ptr->initializeGLResources();
	//m_markMeshes.push_back(ptr);
	const auto z = m_dataModel->topSliceCount();
	const auto y = m_dataModel->rightSliceCount();
	const auto x = m_dataModel->frontSliceCount();
	QMatrix4x4 trans;
	trans.setToIdentity();
	trans.scale(1 / static_cast<double>(x), 1 / static_cast<double>(y), 1 / static_cast<double>(z));

	

	for(const auto & c:cates) {
		const auto mesh = m_markModel->markMesh(c);

		/**
		 *This is a low efficient operation because color of category could not be retrieved
		 *  directly by category item, which is not stored color when created.
		 *  TODO:: The issue would be addressed soon.
		*/
		const QColor color = m_markModel->marks(c)[0]->data(MarkProperty::CategoryColor).value<QColor>();		//Temporarily

		Q_ASSERT_X(mesh->isReady(), "RenderWidget::updateMark", "Mesh not ready");
		const auto v = mesh->vertices();
		const auto idx = mesh->indices();
		const auto nV = mesh->vertexCount();
		const auto nT = mesh->triangleCount();
		const auto n = mesh->normals();
		auto ptr = QSharedPointer<TriangleMesh>(new TriangleMesh(v, 
			n,
			nullptr,
			nV,
			idx,
			nT, 
			m_world*trans,
			this));
		//ptr->setPolyMode(true);
		ptr->initializeGLResources();
		m_markMeshes.push_back(ptr);
		m_markColor.push_back(color);
		qDebug() << "Mesh Color:" << color;
	}
	doneCurrent();
}

void RenderWidget::updateVolumeData()
{
	if (m_dataModel == nullptr)
		return;

	const auto z = m_dataModel->topSliceCount();
	const auto y = m_dataModel->rightSliceCount();
	const auto x = m_dataModel->frontSliceCount();

	QVector3D m_scale = QVector3D(x, y, z);
	m_scale.normalize();
	m_world.setToIdentity();
	m_world.scale(m_scale);

	m_volume.reset(new SliceVolume(m_dataModel, m_world,VolumeFormat(), this));
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

int RenderWidget::selectMesh(int x, int y)
{
	makeCurrent();
	m_pickFBO->bind();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	unsigned char color[4];
	glReadPixels(x,size().height()-y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
	const auto id = colorToId(QColor(color[0],color[1],color[2]));
	m_pickFBO->release();
	doneCurrent();
	return id;
}


void RenderWidget::cleanup()
{
	makeCurrent();

	m_tfTexture.destroy();
	if(m_volume != nullptr)
		m_volume->destroyGLResources();

	for (auto & mesh : m_markMeshes)
		mesh->destoryGLResources();

	m_meshShader.destroyed();
	m_selectShader.destroyed();

	doneCurrent();
}
