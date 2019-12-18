#ifndef MESH_H
#define MESH_H
#include <memory>
#include <vector>

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include "3drender/shader/shaderdatainterface.h"
#include <QOpenGLShaderProgram>


//typedef QVector3D Point3f;
//typedef QVector3D Vector3f;
//typedef QVector2D Point2f;

//typedef QMatrix4x4 Transform;


class RenderWidget;

class TriangleMesh{

	std::unique_ptr<ysl::Point3f[]> m_vertices;
	std::unique_ptr<ysl::Vector3f[]> m_normals;
	std::unique_ptr<ysl::Point2f[]> m_textures;
	int m_nVertex;
	std::vector<int> m_vertexIndices;
	int m_nTriangles;
	QOpenGLBuffer m_vbo;
	QOpenGLBuffer m_ebo;
	QOpenGLVertexArrayObject m_vao;
	ysl::Point3f m_centroid;
	RenderWidget * m_renderer;
	bool m_created;
	bool m_poly;
public:
	TriangleMesh(const ysl::Point3f * vertices,		// Vertex array
		const ysl::Vector3f * normals,				// Normal vector array
		const ysl::Point2f * textures,				// Texture coordinates array
		int nVertex,							// The number of vertex the triangle mesh has
		const int * vertexIndices,				// Vertex index array
		int nTriangles,							// The number of triangle the mesh has
		const ysl::Transform & trans,RenderWidget * widget)noexcept;
	bool initializeGLResources();

	void destoryGLResources();

	bool isCreated()const;

	void setPolyMode(bool enable);

	const ysl::Point3f* vertexArray() const;

	int vertexCount()const;

	const int * indexArray()const;

	int indexCount()const;

	const ysl::Vector3f* normalArray() const;

	ysl::Point3f centroid() const;

	bool render();

	void saveAsObj(std::ofstream & os,std::size_t vertexIndexOffset = 0);

	void updateShader();
};

inline bool TriangleMesh::isCreated()const { return m_created; }
inline void TriangleMesh::setPolyMode(bool enable) {m_poly = enable;}
inline const ysl::Point3f* TriangleMesh::vertexArray() const { return m_vertices.get(); }
inline int TriangleMesh::vertexCount()const { return m_nVertex; }
inline const int * TriangleMesh::indexArray()const { return m_vertexIndices.data(); }
inline int TriangleMesh::indexCount()const { return m_nTriangles*3; }
inline const ysl::Vector3f* TriangleMesh::normalArray() const { return m_normals.get();}
inline ysl::Point3f TriangleMesh::centroid() const { return m_centroid; }




#endif // MESH_H