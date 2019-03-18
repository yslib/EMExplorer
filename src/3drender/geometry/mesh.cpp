#include "mesh.h"
#include "widgets/renderwidget.h"
#include "widgets/renderoptionwidget.h"

#include <cstring>   // for std::memcpy

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



void TriangleMesh::updateShader() 
{
	//m_shader->bind();
	//const auto option = m_renderer->m_parameterWidget->options();
	//QMatrix4x4 world;
	//world.setToIdentity();
	//m_shader->setUniformValue("viewMatrix", m_renderer->camera().view());
	//m_shader->setUniformValue("projMatrix", m_renderer->m_proj);
	//m_shader->setUniformValue("modelMatrix", world);
	//m_shader->setUniformValue("normalMatrix", m_renderer->m_world.normalMatrix());
	//m_shader->setUniformValue("lightPos",m_renderer->camera().position());
	//m_shader->setUniformValue("viewPos", m_renderer->camera().position());
	//m_shader->setUniformValue("objectColor", QVector3D(0.3,0.6,0.9));
}

TriangleMesh::TriangleMesh(const ysl::Point3f* vertices, const ysl::Vector3f* normals, const ysl::Point2f* textures, int nVertex,
	const int* vertexIndices, int nTriangles, const ysl::Transform& trans,RenderWidget * renderer) noexcept:
m_nVertex(nVertex),
m_vertexIndices(vertexIndices, vertexIndices + 3 * nTriangles),
m_nTriangles(nTriangles),
m_created(false),
m_renderer(renderer),
m_ebo(QOpenGLBuffer::IndexBuffer),
m_centroid{0,0,0}
{
	m_vertices.reset(new ysl::Point3f[nVertex]);
	for (int i = 0; i < nVertex; i++) 
	{
		m_vertices[i] = trans * vertices[i];
		m_centroid += m_vertices[i];
	}
	m_centroid /= nVertex;

	int  normalBytes = 0;
	int textureBytes = 0;
	if(normals != nullptr) {
		//const auto nm = trans.normalMatrix();
		const auto nm = trans.Matrix().NormalMatrix();
		normalBytes = m_nVertex * sizeof(ysl::Vector3f);
		m_normals.reset(new ysl::Vector3f[nVertex]);
		for (int i = 0; i < nVertex; i++) {
			//m_normals[i] = trans * normals[i];
			const auto x = normals[i].x, y = normals[i].y, z = normals[i].z;
			m_normals[i] = ysl::Vector3f{ x*nm.m[0][0] + y * nm.m[0][1] + z * nm.m[0][2],x*nm.m[1][0] + y * nm.m[1][1] + z * nm.m[1][2],x*nm.m[2][0] + y * nm.m[2][1] + z * nm.m[2][2] }.Normalized();
		}
	}
	if(textures != nullptr) {
		textureBytes = m_nVertex * sizeof(ysl::Point2f);
		m_textures.reset(new ysl::Point2f[nVertex]);
		std::memcpy(m_textures.get(), textures,nVertex * sizeof(ysl::Point2f));
	}
}

bool TriangleMesh::initializeGLResources()
{
	if (m_renderer == nullptr)
		return false;
	const auto glfuncs = m_renderer->context()->versionFunctions<QOpenGLFunctions_3_3_Core>();
	if (glfuncs == nullptr)
		return false;

	m_vao.create();
	QOpenGLVertexArrayObject::Binder binder(&m_vao);
	m_vbo.create();
	m_vbo.bind();
	const int  vertexBytes = m_nVertex * sizeof(ysl::Point3f);
	const int  indexBytes = m_nTriangles * 3 * sizeof(int);

	const int normalBytes = m_normals != nullptr? m_nVertex * sizeof(ysl::Vector3f):0;
	const int textureBytes = m_textures != nullptr?m_nVertex * sizeof(ysl::Point2f):0;

	m_vbo.allocate(vertexBytes + normalBytes + textureBytes);
	m_vbo.write(0, m_vertices.get(), vertexBytes);
	m_vbo.write(vertexBytes, m_normals.get(), normalBytes);
	m_vbo.write(vertexBytes + normalBytes, m_textures.get(), textureBytes);
	glfuncs->glEnableVertexAttribArray(0);
	glfuncs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ysl::Point3f), reinterpret_cast<void*>(0));
	if(normalBytes != 0) {
		glfuncs->glEnableVertexAttribArray(1);
		glfuncs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ysl::Vector3f), reinterpret_cast<void*>(vertexBytes));
	}else if(textureBytes != 0){
		glfuncs->glEnableVertexAttribArray(2);
		glfuncs->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ysl::Point2f), reinterpret_cast<void*>(vertexBytes + normalBytes));
	}

	m_ebo.create();
	m_ebo.bind();
	m_ebo.allocate(m_vertexIndices.data(), indexBytes);
	m_vbo.release();
	//Note ::Don't unbind the ebo before unbinding vao
	m_created = true;
	return true;
}

void TriangleMesh::destoryGLResources()
{
	m_vao.destroy();
	m_vbo.destroy();
	m_ebo.destroy();
}

bool TriangleMesh::render(){
	if (m_renderer == nullptr)
		return false;
	const auto glfuncs = m_renderer->context()->versionFunctions<QOpenGLFunctions_3_3_Core>();
	if (glfuncs == nullptr)
		return false;
	QOpenGLVertexArrayObject::Binder binder(&m_vao);
	glfuncs->glDrawElements(GL_TRIANGLES, m_nTriangles * 3, GL_UNSIGNED_INT, 0);
}
