#include "abstractplugin.h"
#include "imageviewer.h"

AbstractPlugin::AbstractPlugin(SliceType type,const QString & name,SliceView * view,AbstractSliceDataModel * model,QWidget * parent):
QWidget(parent),
m_model(model),
m_view(view),
m_type(type),
m_sliceName(name)
{
	setWindowTitle(name);
}

void AbstractPlugin::sliceChanged(int index)
{

}

void AbstractPlugin::sliceSelected(const QPoint& pos)
{

}

void AbstractPlugin::sliceOpened(int index)
{
}

void AbstractPlugin::slicePlayStoped(int index)
{
}

void AbstractPlugin::slicePlaying(int index)
{

}

SliceItem * AbstractPlugin::sliceItem()
{
	return static_cast<SliceItem*>(m_view->items().value(0));
}
QImage AbstractPlugin::originalImage(int index)
{
	Q_ASSERT_X(m_model, "asdfasf", "adsfsadfsadfsdaf");
	if (m_model == nullptr)
		return QImage();
	switch(m_type)
	{
	case SliceType::Top:
		return m_model->originalTopSlice(index);
	case SliceType::Right:
		return m_model->originalRightSlice(index);
	case SliceType::Front:
		return m_model->originalFrontSlice(index);
	}
	return QImage();
}
SliceView * AbstractPlugin::view()
{
	return m_view;
}

QString AbstractPlugin::sliceName() const
{
	return m_sliceName;
}
