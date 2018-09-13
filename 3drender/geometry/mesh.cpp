#include "mesh.h"


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


TriangleMesh::TriangleMesh(const Point3f* vertices, const Vector3f* normals, const Point2f* textures, int nVertex,
	const int* vertexIndices, int nTriangles, const Transform3& trans) noexcept:
m_nVertex(nVertex),
m_vertexIndices(vertexIndices, vertexIndices + 3 * nTriangles),
m_nTriangles(nTriangles),
m_created(false),
m_ebo(QOpenGLBuffer::IndexBuffer)
{

	if (m_glfuncs.initializeOpenGLFunctions() == false)
		return;

	m_vertices.reset(new Point3f[nVertex]);
	for(int i=0;i<nVertex;i++)  m_vertices[i] = trans * vertices[i];

	int  normalBytes = 0;
	int textureBytes = 0;
	if(normals != nullptr) {
		normalBytes = m_nVertex * sizeof(Vector3f);
		m_normals.reset(new Vector3f[nVertex]);
		for (int i = 0; i < nVertex; i++)m_normals[i] = trans * normals[i];
	}
	if(textures != nullptr) {
		textureBytes = m_nVertex * sizeof(Point2f);

		m_textures.reset(new Point2f[nVertex]);
		std::memcpy(m_textures.get(), textures,nVertex * sizeof(Point2f));
	}


	m_vao.create();
	QOpenGLVertexArrayObject::Binder binder(&m_vao);
	m_vbo.create();
	m_vbo.bind();
	int  vertexBytes = m_nVertex * sizeof(Point3f);
	int  indexBytes = m_nTriangles * 3 * sizeof(int);

	m_vbo.allocate(vertexBytes + normalBytes + textureBytes);
	m_vbo.write(0, m_vertices.get(), vertexBytes);
	m_vbo.write(vertexBytes, m_normals.get(), normalBytes);
	m_vbo.write(vertexBytes+normalBytes, m_textures.get(), textureBytes);
	m_glfuncs.glEnableVertexAttribArray(0);
	m_glfuncs.glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(Point3f), reinterpret_cast<void*>(0));
	m_glfuncs.glEnableVertexAttribArray(1);
	m_glfuncs.glVertexAttribPointer(1, 3, GL_FLOAT,GL_FALSE, sizeof(Vector3f), reinterpret_cast<void*>(vertexBytes));
	m_glfuncs.glEnableVertexAttribArray(2);
	m_glfuncs.glVertexAttribPointer(2, 2, GL_FLOAT,GL_FALSE, sizeof(Point2f), reinterpret_cast<void*>(vertexBytes+normalBytes));

	m_ebo.create();
	m_ebo.bind();
	m_ebo.allocate(m_vertexIndices.data(), indexBytes);
	m_vbo.release();
	//Note ::Don't unbind the ebo before unbinding vao
	m_created = true;
}

void TriangleMesh::setPolyMode(bool enable) {
	m_poly = enable;
}

void TriangleMesh::render(){
	QOpenGLVertexArrayObject::Binder binder(&m_vao);
		m_glfuncs.glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		m_glfuncs.glDrawElements(GL_TRIANGLES, m_nTriangles * 3, GL_UNSIGNED_INT, 0);
		m_glfuncs.glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}
