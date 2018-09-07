#ifndef MESH_H
#define MESH_H
#include <memory>
#include <vector>
#include <QMatrix4x4>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector2D>

typedef QVector3D Point3f;
typedef QVector3D Vector3f;
typedef QVector2D Point2f;
typedef QMatrix4x4 Transform3;

class TriangleMesh {
	std::unique_ptr<Point3f[]> m_vertices;
	std::unique_ptr<Vector3f[]> m_normals;
	std::unique_ptr<Point2f[]> m_textures;
	int m_nVertex;

	std::vector<int> m_vertexIndices;
	int m_nTriangles;

	QOpenGLFunctions_3_3_Core m_glfuncs;
	QOpenGLBuffer m_vbo;
	QOpenGLBuffer m_ebo;
	QOpenGLVertexArrayObject m_vao;

	bool m_created;
	bool m_poly;

public:
	TriangleMesh(const Point3f * vertices,
		const Vector3f * normals,
		const Point2f * textures,
		int nVertex,
		const int * vertexIndices,
		int nTriangles,
		const Transform3 & trans)noexcept;

	bool isCreated()const { return m_created; }
	void setPolyMode(bool enable);

	const Point3f * vertexArray()const{return m_vertices.get();}

	int vertexCount()const{return m_nVertex;}

	const int * indexArray()const{return m_vertexIndices.data();}

	int indexCount()const{return m_nTriangles;}

	const Vector3f *normalArray()const {return m_normals.get();}

	void render();
};

#endif // MESH_H