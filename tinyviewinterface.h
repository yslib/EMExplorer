#ifndef TINYVIEWINTERFACE_H
#define TINYVIEWINTERFACE_H

#include <QModelIndex>
#include <QVector>

class QAbstractItemModel;

class TinyViewInterface
{
	//QAbstractItemModel * m_model;
public:
    TinyViewInterface();
	//void setModel(QAbstractItemModel * model) { if(m_model != model)m_model = model; }
	//virtual void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
	//QAbstractItemModel * model()const{ return m_model; }
};

#endif // TINYVIEWINTERFACE_H