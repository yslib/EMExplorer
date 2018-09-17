#ifndef TRIANGULATE_H
#define TRIANGULATE_H

#include <QPolygon>
#include <QPolygonF>
#include <QVector3D>


class StrokeMarkItem;

class Triangulate
{
	QVector<QVector3D>         m_allVertices;
	QVector<QVector<int>>	   m_levelIndices;
	QVector<int>               m_resultIndices;
	double					   m_spacing;
public:
	Triangulate(const QVector<StrokeMarkItem*> marks);
	const float * vertices()const;
	const int * indices()const;
	bool triangulate();
private:
	void initVertex(const QVector<StrokeMarkItem*> & marks);
	void subdivisionTriangle(int vi, const int * others,int size);
	void triangulateTetragonum(int vi1, int vi2, int vi3, int vi4);
	void translateVertex(int vi,QVector<int> & others);
	const int * properPointer(const QVector<int> & vec, int startIndex, int length, int * buffer);

};

inline const float * Triangulate::vertices()const { return reinterpret_cast<const float*>(m_allVertices.constData()); }
inline const int * Triangulate::indices()const {
	Q_ASSERT_X(m_resultIndices.size() % 3 == 0, "Triangulate::indices", "Not a triangle mesh.");
	return m_resultIndices.constData();
}


inline auto & tanslateVector(const QVector<QPointF> & vec, int start, int index) {
	int size = vec.size();
	Q_ASSERT_X(size > start, "translateVector", "out of range");
	return vec[(start + size+index)%size];
}




inline auto distanceSquare(float x1,float y1,float z1,float x2,float y2,float z2) {
	return (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2);
}
inline auto distanceSquare(const QPointF & p1,float z1,const QPointF & p2,float z2) {
	return distanceSquare(p1.x(), p1.y(), z1, p2.x(), p2.y(), z2);
}
inline auto distanceSquare(const QVector3D & v1,const QVector3D & v2) {
	return distanceSquare(v1.x(), v1.y(), v1.z(), v2.x(), v2.y(), v2.z());
}
void triangulate(const QVector<QPointF> & upper, const QVector<QPointF> & lower,float density);
#endif // TRIANGULATE_H