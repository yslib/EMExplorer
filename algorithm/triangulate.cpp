#include "Triangulate.h"

#include "model/markitem.h"

#include <memory>
#include <cmath>
#include <algorithm>

Triangulate::Triangulate(const QVector<StrokeMarkItem*> marks):
m_spacing(1.0)
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

		if(nFirst > nSecond) {
			secondVI = firstVI;
			firstVI = m_levelIndices[s + 1];
			nSecond = nFirst;
			nFirst = m_levelIndices[s + 1].size();
		}
		const int p = std::ceil(nSecond / static_cast<double>(nFirst));
		translateVertex(firstVI[0], secondVI);

		std::unique_ptr<int[]> buffer(new int[p]);
		int j = 0, i = 0;
		for(;i<nFirst ;i++,j+=p){
			Q_ASSERT_X(j <= nSecond, "Triangulate::triangulate", "j <= nSencond failed");
			subdivisionTriangle(firstVI[i], secondVI.constData()+j, p);
			triangulateTetragonum(firstVI[i], *(secondVI.constData() + j - 1), *(secondVI.constData() + j), firstVI[i + 1]);
			if (i == nFirst - 1) {
				int left = nSecond % p + 1;
				subdivisionTriangle(firstVI[i], secondVI.constData() + j, left);
				//Do closing
				triangulateTetragonum(firstVI[i], *(secondVI.constData() + j - 1), *(secondVI.constData() + j), firstVI[0]);
			}
		}
	}
	return true;
}

void Triangulate::initVertex(const QVector<StrokeMarkItem*> & marks) {

	m_allVertices.clear();
	const auto markCount = marks.size();
	m_levelIndices.resize(markCount);
	int vertexCount = 0;

	for(int i=0;i<markCount;i++) {
		const auto & poly = marks[i]->polygon();
		const auto n = poly.size();
		m_levelIndices[i].resize(n);
		for(int j=0;j<n;j++) {
			m_levelIndices[i][j] = j + vertexCount;
			m_allVertices.push_back({static_cast<float>(poly[j].x()),static_cast<float>(poly[j].y()),static_cast<float>(i*m_spacing)});		//(x,y,z) in current slice
		}
		vertexCount += n;
	}
	m_resultIndices.clear();
}

void Triangulate::subdivisionTriangle(int vi, const int * others,int size)
{
	Q_ASSERT_X(size > 2, "Triangulate::subdivisionTriangle", "unadquate vertices");
	for(int i = 0 ;i<size-1;i++) {
		m_resultIndices.push_back(vi);
		m_resultIndices.push_back(others[i]);
		m_resultIndices.push_back(others[i+1]);
	}
}

void Triangulate::triangulateTetragonum(int vi1, int vi2, int vi3, int vi4)
{
	m_resultIndices.push_back(vi1);
	m_resultIndices.push_back(vi2);
	m_resultIndices.push_back(vi3);
	m_resultIndices.push_back(vi1);
	m_resultIndices.push_back(vi3);
	m_resultIndices.push_back(vi4);
}

void Triangulate::translateVertex(int vi, QVector<int>& others)
{
	int startIndex = -1;
	auto mind = std::numeric_limits<double>::max();
	for (auto i : others) {
		double d = distanceSquare(m_allVertices[vi], m_allVertices[i]);
		if (mind > d) {
			mind = d;
			startIndex = i;
		}
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

