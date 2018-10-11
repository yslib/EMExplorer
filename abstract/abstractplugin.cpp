#include "abstractplugin.h"
#include "model/sliceitem.h"
#include "widgets/slicewidget.h"
#include "abstractslicedatamodel.h"
#include "widgets/sliceeditorwidget.h"

AbstractPlugin::AbstractPlugin(
	SliceEditorWidget * sliceEditor,
	QWidget * parent) :
	QWidget(parent),
	m_sliceEditor(nullptr)
{
	setSliceEditor(sliceEditor);
}

int AbstractPlugin::currentIndex(SliceType type) const {
	Q_ASSERT_X(m_sliceEditor, "AbstractPlugin::currentIndex", "null pointer");
	return m_sliceEditor->currentSliceIndex(type);
}

SliceItem * AbstractPlugin::sliceItem(SliceType type)const
{
	Q_ASSERT_X(m_sliceEditor, "AbstractPlugin::sliceItem", "null pointer");
	switch (type) {
	case SliceType::Top:
		return static_cast<SliceItem*>(m_sliceEditor->topView()->items().value(1));
	case SliceType::Right:
		return static_cast<SliceItem*>(m_sliceEditor->rightView()->items().value(1));
	case SliceType::Front:
		return static_cast<SliceItem*>(m_sliceEditor->frontView()->items().value(1));
	default:
		return nullptr;
	}
}
QImage AbstractPlugin::originalImage(SliceType type, int index)
{
	Q_ASSERT_X(m_sliceEditor, "AbstractPlugin::originalImage", "null pointer");
	const auto model = m_sliceEditor->sliceModel();
	if (model == nullptr)
		return QImage();
	switch (type)
	{
	case SliceType::Top:
		return model->originalTopSlice(index);
	case SliceType::Right:
		return model->originalRightSlice(index);
	case SliceType::Front:
		return model->originalFrontSlice(index);
	default:
		return QImage();
	}
}

QImage AbstractPlugin::image(SliceType type, int index) {
	Q_ASSERT_X(m_sliceEditor, "AbstractPlugin::originalImage", "null pointer");
	const auto model = m_sliceEditor->sliceModel();
	if (model == nullptr)
		return QImage();
	switch (type)
	{
	case SliceType::Top:
		return model->topSlice(index);
	case SliceType::Right:
		return model->rightSlice(index);
	case SliceType::Front:
		return model->frontSlice(index);
	default:
		return QImage();
	}
}

void AbstractPlugin::setImage(SliceType type, int index, const QImage& image) {

	Q_ASSERT_X(m_sliceEditor, "AbstractPlugin::originalImage", "null pointer");
	const auto model = m_sliceEditor->sliceModel();
	if (model == nullptr)
		return;
	switch (type)
	{
	case SliceType::Top:
		model->setTopSlice(image, index);
		break;
	case SliceType::Right:
		model->setRightSlice(image, index);
		break;
	case SliceType::Front:
		model->setFrontSlice(image, index);
		break;
	default:
		break;
	}
}

void AbstractPlugin::setCurrentImage(SliceType type, const QImage & image)
{
	setImage(type, currentIndex(type), image);
}


QImage AbstractPlugin::currentImage(SliceType type)
{
	return image(type, currentIndex(type));
}

SliceWidget * AbstractPlugin::view(SliceType type)
{
	Q_ASSERT_X(m_sliceEditor, "AbstractPlugin::sliceItem", "null pointer");
	switch (type) {
	case SliceType::Top:
		return m_sliceEditor->topView();
	case SliceType::Right:
		return m_sliceEditor->rightView();
	case SliceType::Front:
		return m_sliceEditor->frontView();
	default:
		return nullptr;
	}
}

void AbstractPlugin::updateDataModel() {

}

void AbstractPlugin::setSliceEditor(SliceEditorWidget* widget) {
	if (m_sliceEditor == widget)
		return;
	if (m_sliceEditor != nullptr)
		disconnect(m_sliceEditor, 0, this, 0);

	m_sliceEditor = widget;

	connect(m_sliceEditor, &SliceEditorWidget::dataModelChanged, this, &AbstractPlugin::updateDataModel);

	updateDataModel();

}


