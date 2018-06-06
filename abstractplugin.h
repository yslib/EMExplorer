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
public slots:
	virtual void sliceChanged(int index);
	virtual void sliceSelected(const QPoint & pos);
	virtual void sliceOpened(int index);
	virtual void slicePlayStoped(int index);
protected:
	SliceItem * getSliceItem();
	QImage getOriginalImage(int index);
private:
	Q_OBJECT
	AbstractSliceDataModel * m_model;
	GraphicsView *m_view;
	SliceType m_type;
	friend class ImageView;
};

#endif // ABSTRACTPLUGIN_H