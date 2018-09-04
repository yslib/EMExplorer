#include "mesh.h"

TriangleMesh::TriangleMesh(const Point3f * vertices, 
	const Vector3f * normals, 
	int nVertex,
	const int * vertexIndices, 
	int nIndex, 
	const Transform3 & trans) noexcept
{
	m_vertices.reset(new Point3f[nIndex]);
	m_vertexIndices.reset(new int[nIndex]);
	m_normals.reset(new Vector3f[nIndex]);

	for (int i = 0; i < nIndex; i++)
	{
		m_vertices[i] = trans * vertices[vertexIndices[i]];
	}
	for (int i = 0; i < nIndex; i++)
	{
		m_vertexIndices[i] = vertexIndices[i];
	}
	//create normals vertices
	for (int i = 0; i < nIndex; i++) {
		m_normals[i] = normals[vertexIndices[i]];
	}
}