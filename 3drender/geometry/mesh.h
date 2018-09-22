#ifndef MESH_H
#define MESH_H
#include <memory>
#include <vector>
#include <QMatrix4x4>
#include <QVector2D>

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include "3drender/shader/shaderdatainterface.h"
#include <QOpenGLShaderProgram>


typedef QVector3D Point3f;
typedef QVector3D Vector3f;
typedef QVector2D Point2f;
typedef QMatrix4x4 Transform3;

class RenderWidget;

class TriangleMesh{
	std::unique_ptr<Point3f[]> m_vertices;
	std::unique_ptr<Vector3f[]> m_normals;
	std::unique_ptr<Point2f[]> m_textures;
	int m_nVertex;

	std::vector<int> m_vertexIndices;
	int m_nTriangles;

	//QScopedPointer<QOpenGLShaderProgram> m_shader;

	QOpenGLBuffer m_vbo;
	QOpenGLBuffer m_ebo;
	QOpenGLVertexArrayObject m_vao;

	RenderWidget * m_renderer;

	bool m_created;
	bool m_poly;

public:
	TriangleMesh(const Point3f * vertices,		// Vertex array
		const Vector3f * normals,				// Normal vector array
		const Point2f * textures,				// Texture coordinates array
		int nVertex,							// The number of vertex the triangle mesh has
		const int * vertexIndices,				// Vertex index array
		int nTriangles,							// The number of triangle the mesh has
		const Transform3 & trans,RenderWidget * widget)noexcept;

	bool initializeGLResources();

	void destoryGLResources();

	bool isCreated()const;

	void setPolyMode(bool enable);

	const Point3f * vertexArray()const;

	int vertexCount()const;

	const int * indexArray()const;

	int indexCount()const;

	const Vector3f *normalArray()const;

	bool render();

	void updateShader();
};

inline bool TriangleMesh::isCreated()const { return m_created; }
inline void TriangleMesh::setPolyMode(bool enable) {m_poly = enable;}
inline const Point3f * TriangleMesh::vertexArray()const { return m_vertices.get(); }
inline int TriangleMesh::vertexCount()const { return m_nVertex; }
inline const int * TriangleMesh::indexArray()const { return m_vertexIndices.data(); }
inline int TriangleMesh::indexCount()const { return m_nTriangles*3; }
inline const Vector3f * TriangleMesh::normalArray()const { return m_normals.get();}




#endif // MESH_H