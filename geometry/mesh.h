#ifndef MESH_H
#define MESH_H
#include <memory>
#include <QVector3D>
#include <QMatrix4x4>


typedef QVector3D Point3f;
typedef QVector3D Vector3f;
typedef QMatrix4x4 Transform3;


class TriangleMesh {
	std::unique_ptr<Point3f[]> m_vertices;
	int m_nVertex;
	std::unique_ptr<int[]> m_vertexIndices;
	int m_nIndex;
	std::unique_ptr<Vector3f[]> m_normals;
public:
	TriangleMesh(const Point3f * vertices,
		const Vector3f * normals,
		int nVertex,
		const int * vertexIndices,
		int nIndex,
		const Transform3 & trans)noexcept;

	const Point3f * vertexArray()const
	{
		return m_vertices.get();
	}
	int vertexCount()const
	{
		return m_nVertex;
	}
	const int * indexArray()const
	{
		return m_vertexIndices.get();
	}
	int indexCount()const
	{
		return m_nIndex;
	}
	const Vector3f *normalArray()const {
		return m_normals.get();
	}
	void transform(const Transform3 & trans)
	{
		for (int i = 0; i < m_nIndex; i++)
		{
			m_vertices[i] = trans * m_vertices[i];
		}
	}

};

#endif // MESH_H