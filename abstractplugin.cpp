#include "abstractplugin.h"
#include "imageviewer.h"

AbstractPlugin::AbstractPlugin(SliceType type,GraphicsView * view,AbstractSliceDataModel * model,QWidget * parent):
QWidget(parent),
m_model(model),
m_view(view),
m_type(type)
{

}

SliceItem * AbstractPlugin::getSliceItem()
{
	return dynamic_cast<SliceItem*>(m_view->items().value(0));
}

void AbstractPlugin::sliceChangeEvent(int index)
{
	//
}

void AbstractPlugin::sliceStopEvent(int index)
{
	//
}

void AbstractPlugin::sliceOpenEvent(int index)
{

}

QImage AbstractPlugin::getOriginalImage(int index)
{
	if (m_model == nullptr)
		return QImage();
	switch(m_type)
	{
	case SliceType::SliceZ:
		return m_model->originalTopSlice(index);
	case SliceType::SliceY:
		return m_model->originalRightSlice(index);
	case SliceType::SliceX:
		return m_model->originalFrontSlice(index);
	}
	return QImage();
}