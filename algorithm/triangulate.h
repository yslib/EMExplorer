#ifndef TRIANGULATE_H
#define TRIANGULATE_H

#include <QVector>
#include <QPoint>

#include <QPolygon>

QPolygon poly;

class triangulate
{
public:
	triangulate();
};



void triangulate(const QVector<QPoint> & upper, const QVector<QPoint> & lower);
#endif // TRIANGULATE_H