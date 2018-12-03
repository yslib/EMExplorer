#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>
#include <QPainter>
#include <QMenu>

#include "renderwidget.h"
#include "abstract/abstractslicedatamodel.h"
#include "model/markmodel.h"
#include "3drender/shader/shaderprogram.h"
#include "3drender/geometry/slicevolume.h"
#include "algorithm/triangulate.h"
#include "globals.h"
#include "model/markitem.h"

#include "base/geometry.h"

#include "base/transformation.h"

static QVector<QVector3D> cubeVertex =
{
	{0.0f,1.0f,0.0f},{1,1,0},{1,1,1},{0,1,1},			// Top
	{0.0f,0.0f,0.0f},{0,0,1},{1,0,1},{1,0,0},			// Bottom
	{0.0f,0.0f,0.0f},{0,1,0},{0,1,1},{0,0,1},			// Left
	{1.0f,0.0f,0.0f},{1,0,1},{1,1,1},{1,1,0},			// Right
	{0.0f,0.0f,0.0f},{1,0,0},{1,1,0},{0,1,0},			// Front
	{1.0f,0.0f,1.0f},{0,0,1},{0,1,1},{1,1,1}			// Back
};

static int cubeVertIndex[] =
{
	2,6,7,3,	// top
	0,1,5,4,	// bottom
	1,0,2,3,	// left
	4,5,7,6,	// right
	0,4,6,2,	// front
	5,1,3,7		// back
};

QVector<QVector3D> axisVertex
{
	{0.f,0.f,0.f},{1.0f,0.f,0.f},						// X axis
	{0.f,0.f,0.f},{0.f,1.f,0.f},						// Y axis
	{0.f,0.f,0.f},{0.f,0.f,1.f}							// Z axis
};

static const char trivialVertexShader[] = "#version 330\n"
"uniform mat4 modelViewMat;\n"
"uniform mat4 projMat;\n"
"layout(location = 0) in vec3 vertex;\n"
"void main()\n"
"{\n"
"	gl_Position = projMat * modelViewMat*vec4(vertex, 1.0);\n"
"}\n";

static const char trivialFragShader[] = "#version 330\n"
"out vec4 fgColor;\n"
"uniform vec4 color;\n"
"void main()\n"
"{\n"
"	fgColor = color;\n"
"}\n";




/**
 * \brief Creates a widget for rendering
 *
 * \note OpenGL 3.3 ProfileCore is needed.
 */
RenderWidget::RenderWidget(AbstractSliceDataModel * dataModel,
	MarkModel * markModel,

	QWidget * parent)
	:QOpenGLWidget(parent),
	m_markModel(markModel),
	m_dataModel(dataModel),
	//m_parameterWidget(widget),
	m_camera(QVector3D(0.f, 0.f, 10.f)),
	m_rayStep(0.02),
	m_tfTexture(nullptr),
	m_volume(nullptr),
	m_meshShader(nullptr),
	m_selectShader(nullptr),
	m_trivialShader(nullptr),
	m_pickFBO(nullptr),
	d_ptr(new RenderWidgetPrivate(this))
{
	//m_contextMenu = new QMenu(QStringLiteral("Context Menu"), this);

	//Q_ASSERT_X(widget != nullptr, "VolumeWidget::VolumeWidget", "null pointer");
	//connect(widget, &RenderParameterWidget::optionsChanged, [this]() {update(); });
	//connect(widget, &RenderParameterWidget::markUpdated,this,&RenderWidget::updateMark);
}

/**
 * \brief Sets the data model as the given \a model
 *
 * \note This function will emit requireTransferFunction() signal and dataModelChanged() signal
 */
void RenderWidget::setDataModel(AbstractSliceDataModel * model)
{
	m_dataModel = model;
	updateVolumeData();
	emit requireTransferFunction();
	emit dataModelChanged();
	update();
}

/**
 * \brief Sets the slice model as the given \a model
 *
 * \note This function will emit markModelChanged() signal
 */
void RenderWidget::setMarkModel(MarkModel* model)
{
	if (m_markModel != nullptr && m_markModel != model) {
		disconnect(m_markModel, &MarkModel::dataChanged, this, &RenderWidget::_slot_markModelDataChanged);
		disconnect(m_markModel->selectionModelOfThisModel(), &QItemSelectionModel::currentChanged, this, &RenderWidget::_slot_currentChanged_selectionModel);
		disconnect(m_markModel->selectionModelOfThisModel(), &QItemSelectionModel::selectionChanged, this, &RenderWidget::_slot_selectionChanged_selectionModel);
	}

	m_markModel = model;
	connect(m_markModel, &MarkModel::dataChanged, this, &RenderWidget::_slot_markModelDataChanged);
	connect(m_markModel->selectionModelOfThisModel(), &QItemSelectionModel::currentChanged, this, &RenderWidget::_slot_currentChanged_selectionModel);
	connect(m_markModel->selectionModelOfThisModel(), &QItemSelectionModel::selectionChanged, this, &RenderWidget::_slot_selectionChanged_selectionModel);

	updateMark();
	//emit markModelChanged();
	emit markModelChanged();

	update();		// Repaint immediately
}

//ShaderDataInterface

/**
 * \brief Returns the volume object
 *
 * \sa GPUVolume
 */
GPUVolume* RenderWidget::volume() const
{
	return m_volume.data();
}

/**
 * \brief Reimplemented from QOpenGLWidget::minimumSizeHint()
 */
QSize RenderWidget::minimumSizeHint() const
{
	return { 400,300 };
}

/**
 * \brief Reimplemented from QOpenGLWidget::sizeHint()
 */
QSize RenderWidget::sizeHint() const
{
	return { 800,600 };
}

/**
 * \brief Reimplemented from QOpenGLWidget::initializeGL()
 *
 * Initializes OpenGL related resources.
 */
void RenderWidget::initializeGL()
{
	if (!initializeOpenGLFunctions()) {
		//qFatal("initializeOpenGLFunctions failed");
		return;
	}
	connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &RenderWidget::cleanup);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);

	// Update transfer functions
	emit requireTransferFunction();

	makeCurrent();
	if (m_volume != nullptr)
		m_volume->initializeGLResources();

	for (auto & item : m_integration)
		item.mesh->initializeGLResources();

	// mesh shader

	m_meshShader = new QOpenGLShaderProgram;
	m_meshShader->create();
	m_meshShader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/resources/shaders/mesh_shader_v.glsl");
	m_meshShader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/resources/shaders/mesh_shader_f.glsl");
	if (m_meshShader->link() == false) {
		qFatal("Mesh shader is not linked.");
		return;
	}
	// pick shader
	m_selectShader = new QOpenGLShaderProgram;
	m_selectShader->create();
	m_selectShader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/resources/shaders/pick_shader_v.glsl");
	m_selectShader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/resources/shaders/pick_shader_f.glsl");
	if (m_selectShader->link() == false) {
		qFatal("Select shader is not linked.");
		return;
	}


	// Trival Shader
	m_trivialShader = new QOpenGLShaderProgram;
	m_trivialShader->create();
	m_trivialShader->addShaderFromSourceCode(QOpenGLShader::Vertex, trivialVertexShader);
	m_trivialShader->addShaderFromSourceCode(QOpenGLShader::Fragment, trivialFragShader);
	if (m_trivialShader->link() == false)
	{
		qFatal("Boundingbox Shader is not linked");
	}

	// Bounding box
	m_boundingBoxVAO.create();
	Q_ASSERT_X(m_boundingBoxVAO.isCreated(), "", "bounding box vao");
	m_boundingBoxVAO.bind();
	m_boundingBoxVBO.create();
	m_boundingBoxVBO.bind();
	m_boundingBoxVBO.allocate(cubeVertex.constData(), sizeof(QVector3D)*cubeVertex.size());
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), reinterpret_cast<void*>(0));

	// Axis
	m_axisVAO.create();
	Q_ASSERT_X(m_axisVAO.isCreated(), "", "axis vao");
	m_axisVAO.bind();
	m_axisVBO.create();
	m_axisVBO.bind();
	m_axisVBO.allocate(sizeof(QVector3D) * 2);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), reinterpret_cast<void*>(0));


	doneCurrent();
}

//#define EXPORT_FBO_IMG

/**
 * \brief Reimplemented from QOpenGLWidget::resizeGL()
 *
 * \note This function will emit windowResize(int w,int h) signal
 */
void RenderWidget::resizeGL(int w, int h)
{
	// Update projection matrices
	const double aspect = GLfloat(w) / h;
	m_proj.setToIdentity();
	m_otho.setToIdentity();
	m_otho.ortho(-aspect * 2, aspect * 2, -2, 2, -100.0, 100.0);
	m_proj.perspective(45.f, aspect, 0.01f, 100.f);

	if (m_pickFBO != nullptr);
	delete m_pickFBO;
	m_pickFBO = new QOpenGLFramebufferObject(w, h, QOpenGLFramebufferObject::Depth, GL_TEXTURE_RECTANGLE, GL_RGBA32F_ARB);
	//glViewport(0, 0, w, h);
	emit windowResized(w, h);
}

/**
 * \brief Reimplemented from QOpenGLWidget::paintGL()
 *
 * Rendering code here
 */
void RenderWidget::paintGL()
{
	//Q_ASSERT_X(m_parameterWidget != nullptr, "VolumeWidget::paintGL", "null pointer");
	Q_D(RenderWidget);
	glEnable(GL_DEPTH_TEST);

	const auto renderMode = d->options->mode;
	//
	const auto world = worldMatrix();

	const auto center = d->volumeNormalTransform*world*QVector3D(0.5, 0.5, 0.5);

	/*ysl::Transform worldTransform = toTransform(world);
	ysl::Transform normalizedTransform = toTransform(d->volumeNormalTransform);
	ysl::Vector3f vec{ 0.5,0.5,0.5 };

	const auto centerVector3f = (normalizedTransform * worldTransform) * vec;
	qDebug() << "QVector3D:" << center << " ysl::Vector3f" << toQVector3D(centerVector3f);*/


	//*Vector3f{ 0.5,0.5,0.5 };

	//update camera center
	m_camera.setCenter(center);

	if (m_volume != nullptr) {
		if (renderMode & RenderMode::DVR) {
			//m_volume->sliceMode(false);
			m_volume->setRenderType(SliceVolume::RenderType::DVR);
		}
		else if (renderMode & RenderMode::SliceTexture) {
			//m_volume->sliceMode(true)
			m_volume->setRenderType(SliceVolume::RenderType::Slice);
		}
		else if (renderMode & RenderMode::Modulo) {
			m_volume->setRenderType(SliceVolume::RenderType::Modulo);
		}
		m_volume->setSliceSphereCoord(d->options->sliceNormal);
		m_volume->setTransform(world);
		m_volume->render();

		if (renderMode & RenderMode::SliceTexture) {

			m_boundingBoxVAO.bind();
			m_boundingBoxVBO.bind();
			m_trivialShader->bind();
			m_trivialShader->setUniformValue("modelViewMat", camera().view()*world*d->volumeNormalTransform);
			m_trivialShader->setUniformValue("projMat", m_proj);
			m_trivialShader->setUniformValue("color", QVector4D{ 1.0f,0.0f,0.0f,1.0f });

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_QUADS, 0, 24);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			m_trivialShader->release();
			m_boundingBoxVAO.release();
		}
	}
	if (renderMode & RenderMode::SliceTexture) {

		const auto viewMatrix = camera().view();
		const auto cameraPos = camera().position();


		if (d->enableStartPicking == true) {
			m_pickFBO->bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_selectShader->bind();
			m_selectShader->setUniformValue("viewMatrix", viewMatrix);
			m_selectShader->setUniformValue("projMatrix", m_proj);
			m_selectShader->setUniformValue("modelMatrix", world);

			for (int i = 0; i < m_integration.size(); i++) {
				if (m_integration[i].visible == true) {
					const auto color = idToColor(i);
					m_selectShader->setUniformValue("pickColor", color);
					m_integration[i].mesh->render();
				}
			}
			m_selectShader->release();
			m_pickFBO->release();
		}

		m_meshShader->bind();
		m_meshShader->setUniformValue("viewMatrix", viewMatrix);
		m_meshShader->setUniformValue("projMatrix", m_proj);
		m_meshShader->setUniformValue("modelMatrix", world);
		m_meshShader->setUniformValue("normalMatrix", world.normalMatrix());
		m_meshShader->setUniformValue("lightPos", cameraPos);
		m_meshShader->setUniformValue("viewPos", cameraPos);

		for (int i = 0; i < m_integration.size(); i++) {

			if (m_integration[i].visible == false)
				continue;

			QColor color = m_integration[i].color;

			if (i == d->selectedObjectId) {
				color = m_integration[i].color.lighter(150);
				m_meshShader->setUniformValue("outlining", false);
				m_meshShader->setUniformValue("objectColor", color);
				m_meshShader->setUniformValue("modelMatrix", world);
				m_integration[i].mesh->render();
			}
			else {
				m_meshShader->setUniformValue("outlining", false);
				m_meshShader->setUniformValue("objectColor", color);
				m_meshShader->setUniformValue("modelMatrix", world);
				m_integration[i].mesh->render();
			}
		}
		m_meshShader->release();
	}
	drawThreeAxis();
	glDisable(GL_DEPTH_TEST);		// Depth test must be closed if draw something with QPainter next
}

void RenderWidget::mousePressEvent(QMouseEvent* event)
{
	Q_D(RenderWidget);
	d->lastMousePos = event->pos();
}

void RenderWidget::mouseMoveEvent(QMouseEvent* event)
{
	Q_D(RenderWidget);
	const auto & p = event->pos();
	// Update Camera
	float dx = p.x() - d->lastMousePos.x();
	float dy = d->lastMousePos.y() - p.y();
	if ((event->buttons() & Qt::LeftButton) && (event->buttons() & Qt::RightButton))
	{
		const auto direction = m_camera.up()*dy + m_camera.right()*dx;
		m_camera.movement(direction, 0.002);
	}
	else if (event->buttons() & Qt::LeftButton)
	{
		m_camera.rotation(dx, dy);
	}
	else if (event->buttons() == Qt::RightButton)
	{
		const auto direction = m_camera.front()*dy;
		m_camera.movement(direction, 0.01);
	}
	d->lastMousePos = p;
	//
	update();
}

/**
 * \brief Reimplemented from QOpenGLWidget::mouseReleaseEvent(QMouseEvent * event)
 *
 * This event handler is used to implement the mouse picking feature.
 * \param event
 */
void RenderWidget::mouseReleaseEvent(QMouseEvent* event) {
	Q_D(RenderWidget);
	if (d->enablePickingMode == true) {
		d->enableStartPicking = true;
		repaint();			// Paint the object into the color frame buffer immediately.
		const auto & p = event->pos();
		const auto previous = d->selectedObjectId;
		d->selectedObjectId = selectMesh(p.x(), p.y());


		_slot_currentMeshChanged(d->selectedObjectId, previous);
		d->enableStartPicking = false;
		repaint();			// Show the result immediately
	}
	update();
}

/**
 * \brief Updates the transfer function that volume rendering uses
 */
void RenderWidget::updateTransferFunction(const float * func)
{
	makeCurrent();
	if (m_tfTexture == nullptr)
	{
		//m_tfTexture.isCreated() == false
		m_tfTexture = new QOpenGLTexture(QOpenGLTexture::Target1D);
		m_tfTexture->setMagnificationFilter(QOpenGLTexture::Linear);
		m_tfTexture->setMinificationFilter(QOpenGLTexture::Linear);
		m_tfTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
		m_tfTexture->setSize(256);
		m_tfTexture->setFormat(QOpenGLTexture::RGBA16_UNorm);		//Equivalent to GL_RGBA16
		m_tfTexture->allocateStorage();
	}
	m_tfTexture->setData(QOpenGLTexture::RGBA, QOpenGLTexture::Float32, func);	//Equivalent to GL_RGBA and GL_FLOAT
	doneCurrent();
	update();
}



/**
 * \brief This property holds the visibility of the top slice when renders data with slice type render.
 */
void RenderWidget::setTopSliceVisible(bool check)
{
	if (m_volume != nullptr) {
		m_volume->setTopSliceVisible(check);
		update();
	}
}

/**
 * \brief This property holds the visibility of the right slice when renders data with slice type render.
 */
void RenderWidget::setRightSliceVisible(bool check) {

	if (m_volume != nullptr) {
		m_volume->setRightSliceVisible(check);
		update();
	}
}

/**
 * \brief This property holds the visibility of the front slice when renders data with slice type render.
 */
void RenderWidget::setFrontSliceVisible(bool check) {

	if (m_volume != nullptr) {
		m_volume->setFrontSliceVisible(check);
		update();
	}
}

/**
 * \brief This is a slot function
 */
void RenderWidget::_slot_markModelDataChanged(const QModelIndex & begin, const QModelIndex & end, const QVector<int>& role)
{
	if (begin != end || begin.isValid() == false)
		return;
	if (role[0] == Qt::CheckStateRole) {
		const auto id = m_query.query(begin);
		if (id != -1) {
			const auto item = static_cast<InstanceTreeItem*>(begin.internalPointer());
			m_integration[id].visible = item->visible();
			update();
		}
	}

}

/**
 * \brief This is a private slot function
 */
void RenderWidget::_slot_currentMeshChanged(int current, int previous)
{
	//qDebug() << "RenderWidget::_slot_currentMeshChanged " << " RenderWidget should be clicked";
	if (m_markModel == nullptr)
		return;
	const auto index = m_query.query(current);
	//m_markModel->selectionModelOfThisModel()->clear();
	m_markModel->selectionModelOfThisModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);

}

/**
 * \brief This is a private slot function
 */
void RenderWidget::_slot_currentChanged_selectionModel(const QModelIndex& current,
	const QModelIndex& previous) {

	const auto treeItem = static_cast<TreeItem*>(current.internalPointer());
	if (treeItem != nullptr) {
		if (treeItem->type() == TreeItemType::Mark) {

			// The parent item of a mark item, which should be a mesh item should also be selected

			//qDebug() << "RenderWidget::_slot_currentChanged_selectionModel " << " Mark should be clicked from SliceEditorWidget";
			const auto parent = m_markModel->parent(current);
			if (static_cast<TreeItem*>(parent.internalPointer())->type() == TreeItemType::Instance) {
				const auto id = m_query.query(parent);
				if (id != -1) {
					Q_D(RenderWidget);
					d->selectedObjectId = id;
					update();
				}
			}

		}
		else {		// Mesh item is selected
			//qDebug() << "RenderWidget::_slot_currentChanged_selectionModel " << " Instance should be clicked from QTreeView";
			const auto id = m_query.query(current);
			if (id != -1) {
				Q_D(RenderWidget);
				d->selectedObjectId = id;
				update();
			}
		}
	}

}

/**
 * \brief This is a private slot function
 */
void RenderWidget::_slot_selectionChanged_selectionModel(const QItemSelection & selected, const QItemSelection & deselected)
{

}

void RenderWidget::drawCoordinate(QPainter* painter)
{
	const auto view = m_camera.view();

	const QPen pen(Qt::red, 2, Qt::SolidLine);

	//qDebug() << view.column(0) <<" "<< view.column(1) << " " << view.column(2);

	const auto axisX = (m_proj * view.column(0)).toVector2DAffine().toPointF();
	const auto axisY = (m_proj * view.column(1)).toVector2DAffine().toPointF();
	const auto axisZ = (m_proj * view.column(2)).toVector2DAffine().toPointF();

	const auto o = QPointF{ 100.f,static_cast<float>((size().height() - 100)) };

	const QVector<QLineF> lines{ {o,axisX},{o,axisY},{o,axisZ} };

	painter->setPen(pen);
	painter->drawLines(lines);
}

//QLineF RenderWidget::lineOnScreen(const QVector3D& begin, const QVector3D& end) 
//{
//	const auto view = QMatrix4x4(m_camera.view().toGenericMatrix<3,3>());
//
//}

void RenderWidget::drawThreeAxis()
{
	//Q_D(RenderWidget);

	glClear(GL_DEPTH_BUFFER_BIT);
	const auto wh = size();
	glViewport(0, 0, wh.width()*0.1, wh.height()*0.1);
	//const auto view = QMatrix4x4(m_camera.view().toGenericMatrix<3, 3>());
	auto view = m_camera.view();
	const auto pos = view.column(3).toVector3D().normalized();
	auto newPos = (pos - m_camera.center()).normalized() + m_camera.center();
	view.setColumn(3, QVector4D(pos, 1.0));

	m_trivialShader->bind();
	m_trivialShader->setUniformValue("modelViewMat", view);
	m_trivialShader->setUniformValue("projMat", m_proj);
	glLineWidth(2.0f);
	{
		QOpenGLVertexArrayObject::Binder binder(&m_axisVAO);
		m_axisVBO.bind();
		QVector<QVector3D> lineX = { {0.f,0.f,0.f},{1.0f,0.0f,0.0f} };
		QVector<QVector3D> lineY = { {0.f,0.f,0.f},{0.0f,1.0f,0.0f} };
		QVector<QVector3D> lineZ = { {0.f,0.f,0.f},{0.0f,0.0f,1.0f} };
		m_axisVBO.write(0, lineX.constData(),sizeof(QVector3D) * lineX.size());
		m_trivialShader->setUniformValue("color", QVector4D{ 1.0f,0.0f,0.0f,1.0f });
		glDrawArrays(GL_LINES, 0, 2);
		m_axisVBO.write(0, lineY.constData(),sizeof(QVector3D) * lineY.size());
		m_trivialShader->setUniformValue("color", QVector4D{ 0.0f,1.0f,0.0f,1.0f });
		glDrawArrays(GL_LINES, 0, 2);
		m_axisVBO.write(0, lineZ.constData(),sizeof(QVector3D) * lineZ.size());
		m_trivialShader->setUniformValue("color", QVector4D{ 0.0f,0.0f,1.0f,1.0f });
		glDrawArrays(GL_LINES, 0, 2);

	}
	glLineWidth(1.0f);
	m_trivialShader->release();
	glViewport(0, 0, wh.width(), wh.height());
}

QMatrix4x4 RenderWidget::worldMatrix() const
{
	const auto xs = d_ptr->options->xSpacing;
	const auto ys = d_ptr->options->ySpacing;
	const auto zs = d_ptr->options->zSpacing;
	QMatrix4x4 world;
	world.setToIdentity();
	world.scale(xs, ys, zs);
	return world;
}

/**
 * \brief This is a private
 */
void RenderWidget::updateMark() {

	Q_D(RenderWidget);
	if (m_markModel == nullptr || m_dataModel == nullptr)
	{
		return;
	}

	m_integration.clear();
	m_query.clear();
	//const auto cates = m_markModel->categoryText();
	makeCurrent();

	// update mark mesh
	const auto z = m_dataModel->topSliceCount();
	const auto y = m_dataModel->rightSliceCount();
	const auto x = m_dataModel->frontSliceCount();
	QMatrix4x4 trans;
	trans.setToIdentity();
	trans.scale(1 / static_cast<double>(x), 1 / static_cast<double>(y), 1 / static_cast<double>(z));
	{
		const auto instances = m_markModel->treeItems(QModelIndex(), TreeItemType::Instance);
		const QColor color = Qt::red;
		for (const auto & inst : instances) {
			const auto mesh = static_cast<InstanceTreeItem*>(inst)->mesh();
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
				d->volumeNormalTransform*trans,		//Make mesh coordinate matching with normalized volume coordinates
				this));
			ptr->initializeGLResources();
			m_integration.push_back({ ptr,color,true });

			const auto id = m_integration.size() - 1;
			m_query.addQueryPair(inst->persistentModelIndex(), id);
		}
	}

	// update volume data (time-consuming)
	if(m_volume != nullptr) {
		const auto p = m_markModel->rawMarks();
		VolumeFormat fmt;
		fmt.fmt = VoxelFormat::Grayscale;
		fmt.type = VoxelType::UInt8;
		m_volume->blend(0, 0, 0, p.data(), x, y, z, fmt);

		m_volume->reloadVolumeData();
	}

	doneCurrent();
}

/**
 * \brief
 */
void RenderWidget::updateVolumeData()
{
	if (m_dataModel == nullptr)
		return;
	const auto z = m_dataModel->topSliceCount();
	const auto y = m_dataModel->rightSliceCount();
	const auto x = m_dataModel->frontSliceCount();

	Q_D(RenderWidget);

	QVector3D m_scale = QVector3D(x, y, z);
	m_scale.normalize();

	d->volumeNormalTransform.setToIdentity();		//Transform size from (1,1,1) to (x/N,y/N,z/N)
	d->volumeNormalTransform.scale(m_scale);

	QMatrix4x4 I;
	I.setToIdentity();
	I.translate(-0.5, -0.5, -0.5);

	// Decide data format

	VolumeFormat fmt;
	fmt.fmt = VoxelFormat::Grayscale;			// AbstractSliceDataModel only support 1-channel data
	if (m_dataModel->dataType() == 0)		// uint8
	{
		fmt.type = VoxelType::UInt8;
	}
	else if (m_dataModel->dataType() == 1) {
		fmt.type = VoxelType::Float32;
	}

	m_volume.reset(new SliceVolume(m_dataModel->constRawData(), x, y, z, I, fmt, this));
	makeCurrent();
	m_volume->initializeGLResources();
	doneCurrent();
}


int RenderWidget::selectMesh(int x, int y)
{
	makeCurrent();
	m_pickFBO->bind();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	unsigned char color[4];
	glReadPixels(x, size().height() - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
	const auto id = colorToId(QColor(color[0], color[1], color[2]));
	m_pickFBO->release();
	doneCurrent();
	return id;
}


void RenderWidget::cleanup()
{
	makeCurrent();

	if (m_volume != nullptr)
		m_volume->destroyGLResources();

	for (auto & inte : m_integration)
		inte.mesh->destoryGLResources();

	delete m_meshShader;
	m_meshShader = nullptr;
	delete m_selectShader;
	m_selectShader = nullptr;
	delete m_tfTexture;
	m_tfTexture = nullptr;
	delete m_pickFBO;
	m_pickFBO = nullptr;
	delete m_trivialShader;
	m_trivialShader = nullptr;


	m_boundingBoxVAO.destroy();
	m_boundingBoxVBO.destroy();


	doneCurrent();
}

/**
 * \brief Destroyes the render widget.
 *
 * \note disconnect the QOpenGLContext::aboutToBeDestroyed() signal from QOpenGLContext links to \a RenderWidget::cleanup() is necessary
 * Because the context is not destroyed yet after the widget is destroyed. The signal still emits and will activate a empty slot and the
 * program will crash.
 *
 * \sa QOpenGLContext RenderWidget:cleanup()
 */
RenderWidget::~RenderWidget()
{
	cleanup();			//
	Q_D(RenderWidget);
	delete d;

	/*!
		Critical: The context will emit aboutToBeDestroyed() signal and will cause a crash
		even after this class was destroyed if below disconnection had not been called
	 */

	disconnect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &RenderWidget::cleanup);
}