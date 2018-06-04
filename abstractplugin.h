#ifndef ABSTRACTPLUGIN_H
#define ABSTRACTPLUGIN_H

#include <QWidget>
#include "imageviewer.h"



class SliceItem;
class AbstractSliceDataModel;
class GraphicsView;



class AbstractPlugin:public QWidget
{
public:
    AbstractPlugin(SliceType type,GraphicsView * view = nullptr, AbstractSliceDataModel * model = nullptr, QWidget * parent = nullptr);
protected:
	SliceItem * getSliceItem();
	virtual void sliceChangeEvent(int index);
	virtual void sliceStopEvent(int index);
	virtual void sliceOpenEvent(int index);
	QImage getOriginalImage(int index);

private:
	Q_OBJECT
	AbstractSliceDataModel * m_model;
	GraphicsView *m_view;
	SliceType m_type;
};

#endif // ABSTRACTPLUGIN_H