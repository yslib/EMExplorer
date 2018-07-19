#include "abstractplugin.h"
#include "sliceitem.h"
#include "sliceview.h"
#include "abstractslicedatamodel.h"

AbstractPlugin::AbstractPlugin(SliceType type,
	const QString & name,
	SliceView * view,
	AbstractSliceDataModel * model,
	QWidget * parent):
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
	Q_UNUSED(index);
}

void AbstractPlugin::sliceSelected(const QPoint& pos)
{
	Q_UNUSED(pos);
}

void AbstractPlugin::sliceOpened(int index)
{
	Q_UNUSED(index);
}

void AbstractPlugin::slicePlayStoped(int index)
{
	Q_UNUSED(index);
}

void AbstractPlugin::slicePlaying(int index)
{
	Q_UNUSED(index);
}

SliceItem * AbstractPlugin::sliceItem()
{
	return static_cast<SliceItem*>(m_view->items().value(1));
}
QImage AbstractPlugin::originalImage(int index)
{
	Q_ASSERT_X(m_model, 
		"AbstractPlugin::originalImage", "null pointer");
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
