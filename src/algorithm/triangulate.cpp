#include "triangulate.h"

#include "model/markitem.h"
#include <QGraphicsItem>
#include <cmath>
#include <algorithm>
#include <vector>

Triangulate::Triangulate():
m_spacing(0.0)
,m_vertexCount(0)
,m_triangleCount(0)
,m_needNormals(true)
{
	
}

Triangulate::Triangulate(const QList<StrokeMarkItem*> marks):
m_spacing(1.0)
,m_vertexCount(0)
,m_triangleCount(0)
,m_needNormals(true)
{
	initVertex(marks);
}

void triangulate(const QVector<QPoint>& upper, const QVector<QPoint>& lower,float density) {
	auto lowerSize = lower.size();
	auto upperSize = upper.size();
	Q_ASSERT_X(upperSize != 0 && lowerSize != 0, "triangulate", "empty vector");
	int startIndex = -1;
	const auto & p = upper[0];
	const double spacing = 1.0;
	auto minLength = std::numeric_limits<double>::max();
	for (int i = 0; i < lowerSize;i++) {
		double length = distanceSquare(p, 0, lower[i], spacing);
		if(length < minLength) {
			length = minLength;
			startIndex = i;
		}
	}
	int endIndex = (startIndex + lowerSize - 1) % lowerSize;
}



bool Triangulate::triangulate() {
	const auto nSlice = m_levelIndices.size();
	Q_ASSERT_X(nSlice >= 2, "Triangulate::triangulate", "nSlice >= 2");
	if(nSlice < 2) return false;
	for(int s = 0 ;s<nSlice-1;s++) {
		bool positive = true;
		auto * firstVI = &m_levelIndices[s];
		auto * secondVI = &m_levelIndices[s + 1];
		auto nFirst = firstVI->size();
		auto nSecond = secondVI->size();
		Q_ASSERT_X(nFirst > 0, "Triangulate::triangulate", "nFrist > 0");
		Q_ASSERT_X(nSecond > 0, "Triangulate::triangulate", "nSecond > 0");
		if(nFirst > nSecond) {
			secondVI = &m_levelIndices[s];
			firstVI = &m_levelIndices[s + 1];
			nSecond = m_levelIndices[s].size();
			nFirst = m_levelIndices[s + 1].size();
			positive = false;
		}
		//The numbers of vertex of two adjacent slice should not be 1 at the same time.
		Q_ASSERT_X(nFirst != 1 && nSecond != 1, "Triangulate::triangulate", "nFirst != 1 && nSecond != 1");
		const auto p = ((nSecond - 1) / static_cast<double>(nFirst));		
		translateVertex((*firstVI)[0], *secondVI);
		auto j = 0.0;
		for(auto i = 0 ;i < nFirst ;i++){
			Q_ASSERT_X(j <= nSecond, "Triangulate::triangulate", "j <= nSencond failed");
			const auto beginIndex = static_cast<int>(std::ceil(j)), endIndex = static_cast<int>(std::floor(j + p));
			const auto num = endIndex - beginIndex + 1;
			const auto nextBeginIndex = static_cast<int>(std::ceil(j + p));
			Q_ASSERT_X(beginIndex < nSecond, "Triangulate::triangulate", "beginIndex < nSecond");
			Q_ASSERT_X(endIndex< nSecond, "Triangulate::triangulate", "endIndex < nSecond");
			subdivisionTriangle((*firstVI)[i], secondVI->data() + beginIndex, num,positive);
			if (i == nFirst - 1) {		// Do clothing
				triangulateTetragonum((*firstVI)[i], *(secondVI->data() + endIndex), *(secondVI->data()), (*firstVI)[0],positive);
			}
			else {
				triangulateTetragonum((*firstVI)[i], *(secondVI->data() + endIndex), *(secondVI->data() + nextBeginIndex), (*firstVI)[i + 1],positive);
			}
			j += p;		//Segment advance
		}
	}
	computeNormals();
	return (m_ready = true);
}

void Triangulate::initVertex(const QList<StrokeMarkItem*>& marks) {

	m_allVertices.clear();
	const auto markCount = marks.size();
	m_levelIndices.resize(markCount);
	m_vertexCount = 0;
	m_triangleCount = 0;
	m_ready = false;

	auto copyMarks = marks;
	std::sort(copyMarks.begin(), copyMarks.end(), [](const StrokeMarkItem* m1, const StrokeMarkItem *m2) 
	{
		return m1->sliceIndex() < m2->sliceIndex();
	});
	for(int i=0;i<markCount;i++) {
		const auto & poly = copyMarks[i]->polygon();
		const auto n = poly.size();
		m_levelIndices[i].resize(n);
		for(int j=0;j<n;j++) {
			m_levelIndices[i][j] = j + m_vertexCount;
			m_allVertices.push_back({static_cast<float>(poly[j].x()),static_cast<float>(poly[j].y()),static_cast<float>(copyMarks[i]->sliceIndex()*m_spacing)});		//(x,y,z) in current slice
		}
		m_vertexCount += n;
	}
	m_resultIndices.clear();
}

void Triangulate::subdivisionTriangle(int vi, const int * others, int size, bool positive)
{
	if (size < 2) return;
	if(positive) {
		for (int i = 0; i < size - 1; i++) {
			m_resultIndices.push_back(vi);
			m_resultIndices.push_back(others[i]);
			m_resultIndices.push_back(others[i + 1]);
			m_triangleCount++;
		}
	}else {
		for (int i = 0; i <size-1; i++) {
			m_resultIndices.push_back(vi);
			m_resultIndices.push_back(others[i+1]);
			m_resultIndices.push_back(others[i]);
			m_triangleCount++;
		}
	}

}

void Triangulate::triangulateTetragonum(int vi1, int vi2, int vi3, int vi4,bool positive)
{
	if(positive) {
		if (vi2 == vi3) {
			m_resultIndices.push_back(vi1);
			m_resultIndices.push_back(vi3);
			m_resultIndices.push_back(vi4);
			m_triangleCount++;
		}
		else {
			m_resultIndices.push_back(vi1);
			m_resultIndices.push_back(vi2);
			m_resultIndices.push_back(vi3);
			m_resultIndices.push_back(vi1);
			m_resultIndices.push_back(vi3);
			m_resultIndices.push_back(vi4);
			m_triangleCount += 2;
		}
	}else {
		if (vi2 == vi3) {
			m_resultIndices.push_back(vi1);
			m_resultIndices.push_back(vi4);
			m_resultIndices.push_back(vi3);
			m_triangleCount++;
		}
		else {
			m_resultIndices.push_back(vi1);
			m_resultIndices.push_back(vi3);
			m_resultIndices.push_back(vi2);
			m_resultIndices.push_back(vi1);
			m_resultIndices.push_back(vi4);
			m_resultIndices.push_back(vi3);
			m_triangleCount += 2;
		}
	}


}

void Triangulate::translateVertex(int vi, std::vector<int>& others)
{
	int startIndex = -1;
	auto mind = std::numeric_limits<double>::max();
	int i = 0;
	for (auto id : others) {
		const auto d = (m_allVertices[vi] - m_allVertices[id]).LengthSquared();
		//double d = distanceSquare(m_allVertices[vi], m_allVertices[id]);
		if (mind > d) 
		{
			mind = d;
			startIndex = i;
		}
		i++;
	}
	std::reverse(others.begin(), others.begin()+startIndex);
	std::reverse(others.begin() + startIndex, others.end());
	std::reverse(others.begin(), others.end());
}

void Triangulate::computeNormals() {
	if (m_needNormals == false)
		return;

	const auto nVertex = m_allVertices.size();
	const auto nIndex = m_resultIndices.size();
	Q_ASSERT_X(nIndex % 3 == 0, "Triangulate:computeNormals", "nIndex % 3 == 0");

	m_normals.resize(nVertex);
	std::vector<TriFace> triFaces;
	std::vector<std::vector<int>> adjacentFaces(nVertex);

	for(int i=0,faceId = 0;i<nIndex;i+=3,faceId++) {
		TriFace face(&m_resultIndices[i]);
		const auto v0 = m_allVertices[face.v[0]];
		const auto v1 = m_allVertices[face.v[1]];
		const auto v2 = m_allVertices[face.v[2]];
		//face.normal = QVector3D::crossProduct(v1-v0,v2-v0).normalized();
		face.normal = ysl::Vector3f::Cross(v1 - v0, v2 - v0).Normalized();

		adjacentFaces[face.v[0]].push_back(faceId);
		adjacentFaces[face.v[1]].push_back(faceId);
		adjacentFaces[face.v[2]].push_back(faceId);
		triFaces.push_back(face);
	}
	Q_ASSERT_X(m_normals.size() == m_allVertices.size(), "", "");

	for(int i=0;i<m_normals.size();i++) {
		ysl::Vector3f vec(0, 0, 0);
		const auto size = adjacentFaces[i].size();
		for (int j = 0; j < size;j++) 
		{
			vec += triFaces[adjacentFaces[i][j]].normal;
		}
		m_normals[i] = vec / size;
	}

}
