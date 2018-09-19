#include "Triangulate.h"

#include "model/markitem.h"

#include <QGraphicsItem>

#include <memory>
#include <cmath>
#include <algorithm>
#include <iostream>

Triangulate::Triangulate():
m_spacing(0.0)
,m_vertexCount(0)
,m_triangleCount(0)
{
	
}

Triangulate::Triangulate(const QList<StrokeMarkItem*> marks):
m_spacing(1.0)
,m_vertexCount(0)
,m_triangleCount(0)
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
	Q_ASSERT_X(nSlice >= 2, "Triangulate::triangulate", "unadquate slices");
	//if(nSlice < 2) return false;

	for(int s = 0 ;s<nSlice-1;s++) {
		auto & firstVI = m_levelIndices[s];
		auto & secondVI = m_levelIndices[s + 1];
		auto nFirst = firstVI.size();
		auto nSecond = secondVI.size();

		Q_ASSERT_X(nFirst > 0, "Triangulate::triangulate", "nFrist > 0");
		Q_ASSERT_X(nSecond > 0, "Triangulate::triangulate", "nSecond > 0");


		if(nFirst > nSecond) {
			secondVI = firstVI;
			firstVI = m_levelIndices[s + 1];
			nSecond = nFirst;
			nFirst = m_levelIndices[s + 1].size();
		}

		//The numbers of vertex of two adjacent slice should not be 1 at the same time.
		Q_ASSERT_X(nFirst != 1 && nSecond != 1, "Triangulate::triangulate", "nFirst != 1 && nSecond != 1");
		const auto p = ((nSecond - 1) / static_cast<double>(nFirst));		

		translateVertex(firstVI[0], secondVI);

		auto j = 0.0;
		for(auto i = 0 ;i < nFirst ;i++){
			Q_ASSERT_X(j <= nSecond, "Triangulate::triangulate", "j <= nSencond failed");
			const auto beginIndex = static_cast<int>(std::ceil(j)), endIndex = static_cast<int>(std::floor(j + p));
			const auto num = endIndex - beginIndex + 1;
			const auto nextBeginIndex = static_cast<int>(std::ceil(j + p));

			std::cout << j << " " << j + p << " " << beginIndex << " " << endIndex << " " << nextBeginIndex << std::endl;
			Q_ASSERT_X(beginIndex < nSecond, "Triangulate::triangulate", "beginIndex < nSecond");
			Q_ASSERT_X(endIndex< nSecond, "Triangulate::triangulate", "endIndex < nSecond");
			subdivisionTriangle(firstVI[i], secondVI.constData() + beginIndex, num);
			if (i == nFirst - 1) {		// Do clothing
				triangulateTetragonum(firstVI[i], *(secondVI.constData() + endIndex), *(secondVI.constData()), firstVI[0]);
			}
			else {
				triangulateTetragonum(firstVI[i], *(secondVI.constData() + endIndex), *(secondVI.constData() + nextBeginIndex), firstVI[i + 1]);
			}
			j += p;		//Segment advance
		}
	}

	return (m_ready = true);
}

void Triangulate::initVertex(const QList<StrokeMarkItem*>& marks) {

	m_allVertices.clear();
	const auto markCount = marks.size();
	m_levelIndices.resize(markCount);
	m_vertexCount = 0;
	m_triangleCount = 0;
	m_ready = false;

	for(int i=0;i<markCount;i++) {
		const auto & poly = marks[i]->polygon();
		const auto n = poly.size();
		m_levelIndices[i].resize(n);
		for(int j=0;j<n;j++) {
			m_levelIndices[i][j] = j + m_vertexCount;
			m_allVertices.push_back({static_cast<float>(poly[j].x()),static_cast<float>(poly[j].y()),static_cast<float>(i*m_spacing)});		//(x,y,z) in current slice
		}
		m_vertexCount += n;
	}
	m_resultIndices.clear();
}

void Triangulate::subdivisionTriangle(int vi, const int * others,int size)
{
	if (size < 2) return;
	for(int i = 0 ;i<size-1;i++) {
		m_resultIndices.push_back(vi);
		m_resultIndices.push_back(others[i]);
		m_resultIndices.push_back(others[i+1]);
		m_triangleCount++;
	}
}

void Triangulate::triangulateTetragonum(int vi1, int vi2, int vi3, int vi4)
{
	if(vi2 == vi3) {
		m_resultIndices.push_back(vi1);
		m_resultIndices.push_back(vi3);
		m_resultIndices.push_back(vi4);
		m_triangleCount++;
	}else {
		m_resultIndices.push_back(vi1);
		m_resultIndices.push_back(vi2);
		m_resultIndices.push_back(vi3);
		m_resultIndices.push_back(vi1);
		m_resultIndices.push_back(vi3);
		m_resultIndices.push_back(vi4);
		m_triangleCount += 2;
	}

}

void Triangulate::translateVertex(int vi, QVector<int>& others)
{
	int startIndex = -1;
	auto mind = std::numeric_limits<double>::max();
	int i = 0;
	for (auto id : others) {
		double d = distanceSquare(m_allVertices[vi], m_allVertices[id]);
		if (mind > d) {
			mind = d;
			startIndex = i;
		}
		i++;
	}
	std::reverse(others.begin(), others.begin()+startIndex);
	std::reverse(others.begin() + startIndex, others.end());
	std::reverse(others.begin(), others.end());
}

const int * Triangulate::properPointer(const QVector<int>& vec, int startIndex, int length, int * buffer)
{
	const auto size = vec.size();
	Q_ASSERT_X(size > startIndex,"Triangulate::properPointer", "size error");
	if(startIndex + length > size) {
		int left = startIndex + length - size;
		std::memcpy(buffer, vec.constData() + startIndex, sizeof(int) * (length - left));
		std::memcpy(buffer, vec.constData(), sizeof(int)*(left));
		return buffer;
	}else {
		return vec.constData() + startIndex;
	}
}

