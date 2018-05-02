/*Custom Headers*/
#include "imageviewer.h"
#include "titledsliderwithspinbox.h"
#include "histogram.h"
#include "ItemContext.h"
/*Qt Headers*/
#include <QToolBar>
#include <QLabel>
#include <QWheelEvent>
#include <QDebug>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPolygon>
#include <QColorDialog>
#include <QPicture>
#include <cassert>


bool ImageViewer::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == m_topImageLabel) {
		if (event->type() == QEvent::MouseButtonPress) {
			m_topImageEnablePaint = true;
			auto e = static_cast<QMouseEvent*>(event);
			m_prevPaint = e->pos();
			//qDebug() << m_prevPaint;
			return true;
		}
		else if (event->type() == QEvent::MouseMove) {
			if (m_topImageEnablePaint == true) {
				auto e = static_cast<QMouseEvent*>(event);
				//update();
				QPoint p = e->pos();
				paintLine(m_prevPaint, p, m_topImageLabel);
				m_prevPaint = p;
			}
			return true;
		}
		else if (event->type() == QEvent::MouseButtonRelease) {
			m_topImageEnablePaint = false;
			return true;
		}
	}
	else if (obj == m_rightImageLabel) {
		if (event->type() == QEvent::MouseButtonPress) {
			m_rightImageEnablePaint = true;
			auto e = static_cast<QMouseEvent*>(event);
			m_prevPaint = e->pos();
			update();
			return true;
		}
		else if (event->type() == QEvent::MouseMove) {

			return true;
		}
		else if (event->type() == QEvent::MouseButtonRelease) {
			m_rightImageEnablePaint = false;
			return true;
		}


	}
	else if (obj == m_frontImageLabel) {
		if (event->type() == QEvent::MouseButtonPress) {
			m_frontImageEnablePaint = true;
			auto e = static_cast<QMouseEvent*>(event);
			m_prevPaint = e->pos();
			update();
			return true;
		}
		else if (event->type() == QEvent::MouseMove) {

			return true;
		}
		else if (event->type() == QEvent::MouseButtonRelease) {
			m_frontImageLabel = false;
			return true;
		}

	}
	return QScrollArea::eventFilter(obj, event);
}

void ImageViewer::wheelEvent(QWheelEvent *event)
{
	if (event->type() == QEvent::Wheel) {
		QWheelEvent *e = static_cast<QWheelEvent*>(event);
		if (e->orientation() == Qt::Horizontal) {

		}
		else {
			if (m_loaded == true) {
				if (e->delta() > 0) {
					zoom(m_factor + 0.25);
				}
				else {
					zoom(m_factor - 0.25);
				}

			}
		}
	}
}

void ImageViewer::paintEvent(QPaintEvent *event)
{

}

ImageViewer::ImageViewer(QWidget *parent) : QScrollArea(parent), m_factor(1.0), m_loaded(false)
{

	m_displayWidget = new QWidget(this);
	m_displayWidget->resize(Width, Height);
	//m_displayWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	m_layout = new QGridLayout(this);
	m_topImageLabel = new QLabel(this);
	m_rightImageLabel = new QLabel(this);
	m_frontImageLabel = new QLabel(this);

	installEventFilter(m_topImageLabel);
	installEventFilter(m_rightImageLabel);
	installEventFilter(m_frontImageLabel);
	//installEventFilter(this);
	//setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
	m_topImageLabel->installEventFilter(this);

	//m_topImageLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
	m_topImageLabel->setScaledContents(true);
	m_rightImageLabel->setScaledContents(true);
	m_frontImageLabel->setScaledContents(true);

	m_displayWidget->setLayout(m_layout);
	m_layout->addWidget(m_topImageLabel, 0, 0);
	m_layout->addWidget(m_rightImageLabel, 0, 1);
	m_layout->addWidget(m_frontImageLabel, 1, 0);

	//m_layout->setSizeConstraint(QLayout::SetFixedSize);
	setVisible(false);
	setBackgroundRole(QPalette::Dark);
	setFocusPolicy(Qt::ClickFocus);
}

void ImageViewer::setTopImage(const QImage &image)
{
	//qDebug() << "Set Image";
	m_topImageLabel->setPixmap(QPixmap::fromImage(image));
	//m_topImageLabel->resize(image.size());
	m_topImage = image;
	setVisible(true);
	m_loaded = true;
	updateStretchFactor();
}

void ImageViewer::setRightImage(const QImage &image)
{
	//qDebug() << "Set Right Image";
	m_rightImageLabel->setPixmap(QPixmap::fromImage(image));
	m_loaded = true;
	setVisible(true);
	m_rightImage = image;
	updateStretchFactor();
}

void ImageViewer::setFrontImage(const QImage &image)
{
	//qDebug() << "Set Front Image";
	m_frontImageLabel->setPixmap(QPixmap::fromImage(image));
	m_loaded = true;
	setVisible(true);
	m_topImage = image;
	updateStretchFactor();
}
void ImageViewer::zoom(qreal factor)
{
	if (factor > 2.00)
		factor = 2.00;
	else if (factor < 0.25)
		factor = 0.25;
	m_factor = factor;
	//qDebug()<<"after adjusting:"<<m_factor;
	//m_imageLabel->resize(factor*m_imageLabel->pixmap()->size());
	m_displayWidget->resize(factor*QSize(Width, Height));
	//qDebug()<<m_topImageLabel->size();
}

void ImageViewer::updateStretchFactor()
{
	//    QSizePolicy topImagePolicy = m_topImageLabel->sizePolicy();
	//    QSizePolicy rightImagePolicy = m_rightImageLabel->sizePolicy();
	//    QSizePolicy frontImagePolicy = m_frontImageLabel->sizePolicy();

	QSize topImageSize;
	if (m_topImageLabel->pixmap() != nullptr) {
		topImageSize = m_topImageLabel->pixmap()->size();
		//   qDebug()<<"aaa";
	}
	QSize rightImageSize;
	if (m_rightImageLabel->pixmap() != nullptr) {
		rightImageSize = m_rightImageLabel->pixmap()->size();
		// qDebug()<<"asdf";
	}
	QSize frontImageSize;
	if (m_frontImageLabel->pixmap() != nullptr) {
		frontImageSize = m_frontImageLabel->pixmap()->size();
		//  qDebug()<<"asdfsadf";
	}

	m_layout->setColumnStretch(0, topImageSize.width());
	m_layout->setColumnStretch(1, rightImageSize.width());
	m_layout->setRowStretch(0, topImageSize.height());
	m_layout->setRowStretch(1, frontImageSize.height());
	//qDebug()<<topImageSize<<" "<<rightImageSize<<" "<<frontImageSize;
	//topImagePolicy.setHorizontalStretch(topImageSize.width());
	//rightImagePolicy.setHorizontalStretch(rightImageSize.width());
	//topImagePolicy.setVerticalStretch(topImageSize.height());
	//frontImagePolicy.setVerticalStretch(frontImageSize.height());
	// m_topImageLabel->setSizePolicy(topImagePolicy);
	// m_rightImageLabel->setSizePolicy(rightImagePolicy);
	// m_frontImageLabel->setSizePolicy(frontImagePolicy);
}

void ImageViewer::paintLine(const QPoint &begin, const QPoint &end, QPaintDevice *dev)
{
	m_painter.begin(dev);
	m_painter.drawLine(begin, end);
	m_painter.end();
}
//bool ImageViewer::event(QEvent *event)
//{
//    qDebug()<<"in event";

//     return QObject::event(event);
//}

void ImageView::createActions()
{
	//createActions()
	m_topSlicePlayAction = new QAction(tr("Play"), this);
	m_topSlicePlayAction->setCheckable(true);
	m_rightSlicePlayAction = new QAction(tr("Play"), this);
	m_rightSlicePlayAction->setCheckable(true);
	m_frontSlicePlayAction = new QAction(tr("Play"), this);
	m_frontSlicePlayAction->setCheckable(true);
	m_markAction = new QAction(tr("Mark"), this);
	m_markAction->setCheckable(true);
	m_colorAction = new QAction(tr("Color"), this);

	m_zoomInAction = new QAction(tr("ZoomIn"), this);
	m_zoomOutAction = new QAction(tr("ZoomOut"), this);

	//tool bar
	m_toolBar = new QToolBar(this);
	m_layout->addWidget(m_toolBar, 0, 0);
	m_toolBar->addWidget(m_topSlider);
	m_toolBar->addAction(m_topSlicePlayAction);
	m_toolBar->addWidget(m_rightSlider);
	m_toolBar->addAction(m_rightSlicePlayAction);
	m_toolBar->addWidget(m_frontSlider);
	m_toolBar->addAction(m_frontSlicePlayAction);
	m_toolBar->addAction(m_colorAction);
	m_toolBar->addAction(m_markAction);
	m_toolBar->addAction(m_zoomInAction);
	m_toolBar->addAction(m_zoomOutAction);

	connect(m_markAction, SIGNAL(triggered(bool)), m_view, SLOT(paintEnable(bool)));
	connect(m_topSlicePlayAction, SIGNAL(triggered(bool)), this, SLOT(onTopSliceTimer(bool)));
	connect(m_rightSlicePlayAction, SIGNAL(triggered(bool)), this, SLOT(onRightSliceTimer(bool)));
	connect(m_frontSlicePlayAction, SIGNAL(triggered(bool)), this, SLOT(onFrontSliceTimer(bool)));
	connect(m_colorAction, &QAction::triggered, this, &ImageView::onColorChanged);
	connect(m_zoomInAction, &QAction::triggered, [=]()
	{
		double factor = std::pow(1.125, 1);
		m_view->scale(factor, factor);
	});
	connect(m_zoomOutAction, &QAction::triggered, [=]()
	{
		double factor = std::pow(1.125, -1);
		m_view->scale(factor, factor);
	});
	updateActions();
}

void ImageView::updateActions()
{
	bool enable = (m_model != nullptr && m_modelIndex.isValid() == true && m_ptr.isNull() == false);
	setEnabled(enable);
}

ImageView::ImageView(QWidget *parent) :
	QWidget(parent),
	m_topSlice(nullptr),
	m_rightSlice(nullptr),
	m_frontSlice(nullptr),
	m_model(nullptr)
{
	//layout
	m_layout = new QGridLayout(this);
	//QGraphicsView
	m_view = new GraphicsView(this);
	m_layout->addWidget(m_view, 1, 0);
	//m_view->setScene(m_scene);

	//sliders
	m_topSlider = new TitledSliderWithSpinBox(this, tr("Z:"));
	m_rightSlider = new TitledSliderWithSpinBox(this, tr("X:"));
	m_frontSlider = new TitledSliderWithSpinBox(this, tr("Y:"));

	connect(m_topSlider, &TitledSliderWithSpinBox::valueChanged,[=](int value){emit sliderChanged(value, SliceType::SliceZ);});
	connect(m_rightSlider, &TitledSliderWithSpinBox::valueChanged,[=](int value){emit sliderChanged(value, SliceType::SliceY);});
	connect(m_frontSlider, &TitledSliderWithSpinBox::valueChanged,[=](int value){emit sliderChanged(value, SliceType::SliceX);});

	connect(m_topSlider, &TitledSliderWithSpinBox::valueChanged, [=](int value){sliceChanged(value, SliceType::SliceZ);});
	connect(m_rightSlider, &TitledSliderWithSpinBox::valueChanged, [=](int value){sliceChanged(value, SliceType::SliceY);});
	connect(m_frontSlider, &TitledSliderWithSpinBox::valueChanged, [=](int value){sliceChanged(value, SliceType::SliceX);});

	connect(m_view, SIGNAL(zSliceSelected(const QPoint &)), this, SIGNAL(zSliceSelected(const QPoint &)));
	connect(m_view, SIGNAL(ySliceSelected(const QPoint &)), this, SIGNAL(ySliceSelected(const QPoint &)));
	connect(m_view, SIGNAL(xSliceSelected(const QPoint &)), this, SIGNAL(xSliceSelected(const QPoint &)));

	//This routine should be a part of updateModel
	connect(m_view, &GraphicsView::markAdded, [=](QGraphicsItem * item, SliceType type)
	{
		if (m_model == nullptr)
		{
			qWarning("Model is empty.");
			return;
		}
		int index = currentIndex(type);
		m_ptr->addSliceMark(item, index, type);
		m_internalUpdate = true;
		m_model->setData(getDataIndex(m_modelIndex), QVariant::fromValue(m_ptr));

		//find its mark index
		constexpr int MarkRowNumber = 1;
		QModelIndex markIndex= m_model->index(MarkRowNumber, 0, m_modelIndex);
		QModelIndex countField = m_model->index(MarkRowNumber, 1, m_modelIndex);

		int count;
		QVariant var = m_model->data(countField);
		if (var.canConvert<int>() == true)
			count = (var.value<int>());

		bool success = m_model->insertRow(m_model->rowCount(markIndex), markIndex);
		if(success == false)
		{
			qCritical("Inserting row into model for mark failed\n");
			return;
		}
		QModelIndex newMark = m_model->index(m_model->rowCount(markIndex) - 1, 0, markIndex);
		if(newMark.isValid() == true)
		{
			m_internalUpdate = true;
			//set text
			m_model->setData(newMark, QString("#%1").arg(count));
			//set bool
			m_internalUpdate = true;
			m_model->setData(newMark.sibling(newMark.row(), newMark.column() + 1), QVariant::fromValue(true));

			m_internalUpdate = true;
			m_model->setData(countField, QVariant::fromValue(++count));
		}
	});
	//action
	createActions();

	setLayout(m_layout);
}

void ImageView::sliceChanged(int value, SliceType type)
{
	qDebug() << "asdfsadfas";
	Q_ASSERT(m_ptr.isNull() == false);
	resetSliceAndVisibleMarks(type);
	m_internalUpdate = true;
	updateModel();
}

void ImageView::setTopSliceCount(int value)
{
	m_topSlider->setMaximum(value - 1);
}
void ImageView::setRightSliceCount(int value)
{
	m_rightSlider->setMaximum(value - 1);
}
void ImageView::setFrontSliceCount(int value)
{
	m_frontSlider->setMaximum(value - 1);
}
int ImageView::currentIndex(SliceType type)
{
	switch (type)
	{
	case SliceType::SliceZ:
		return m_topSlider->value();
	case SliceType::SliceY:
		return m_rightSlider->value();
	case SliceType::SliceX:
		return m_frontSlider->value();
	default:
		return -1;
	}
}

void ImageView::resetSliceAndVisibleMarks(SliceType type)
{
	if (m_ptr.isNull() == true)
		return;
	int index = currentIndex(type);
	m_view->setImage(m_ptr->slice(index,type), type);
	m_view->clearSliceMarks(type);

	m_view->setMarks(m_ptr->visibleSliceMarks(index, type), type);
}

//void ImageView::setFrontSliceVisibleMarks()
//{
//	if (m_ptr.isNull() == true)
//	{
//		qWarning("Model is empty");
//		return;
//	}
//	//clear previous items
//	//m_view->clearFrontSliceMarks();
//
//	int index = m_topSlider->value();
//	auto items = m_ptr->getFrontSliceMarks(index);
//	QList<QGraphicsItem *> visibleItems;
//	foreach(QGraphicsItem * item, items) {
//		bool visible = m_ptr->frontSliceMarkVisble(item);
//		if (visible)
//			visibleItems.push_back(item);
//	}
//	m_view->setFrontSliceMarks(visibleItems);
//}
//
//void ImageView::setTopSliceVisibleMarks()
//{
//	if (m_ptr.isNull() == true)
//	{
//		qWarning("Model is empty");
//		return;
//	}
//	//clear previous items
//	//m_view->clearTopSliceMarks();
//
//	int index = m_topSlider->value();
//	auto items = m_ptr->getTopSliceMarks(index);
//	QList<QGraphicsItem *> visibleItems;
//	foreach(QGraphicsItem * item, items) {
//		bool visible = m_ptr->topSliceMarkVisble(item);
//		if (visible)
//			visibleItems.push_back(item);
//	}
//	m_view->setTopSliceMarks(visibleItems);
//}
//
//void ImageView::setRightSliceVisibleMarks()
//{
//	if (m_ptr.isNull() == true)
//	{
//		qWarning("Model is empty");
//		return;
//	}
//	//clear previous items
//	//m_view->clearRightSliceMarks();
//
//	int index = m_topSlider->value();
//	auto items = m_ptr->getRightSliceMarks(index);
//	QList<QGraphicsItem *> visibleItems;
//	foreach(QGraphicsItem * item, items) {
//		bool visible = m_ptr->rightSliceMarkVisble(item);
//		if (visible)
//			visibleItems.push_back(item);
//	}
//	m_view->setRightSliceMarks(visibleItems);
//}

int ImageView::getZSliceValue() const
{
	return m_topSlider->value();
}

int ImageView::getYSliceValue() const
{
	return m_rightSlider->value();
}

int ImageView::getXSliceValue() const
{
	return m_frontSlider->value();
}

void ImageView::setModel(DataItemModel * model)
{
	if (m_model != model)
	{
		m_model = model;
		disconnect(m_model, &DataItemModel::dataChanged, this, &ImageView::dataChanged);

		connect(m_model, &DataItemModel::dataChanged, this, &ImageView::dataChanged);
		///TODO::get corresponding data i.e. current slice (top)
	}
}

void ImageView::dataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int>& roles)
{
	qDebug() << "In ImageView:Model has been updated.";
	if (m_internalUpdate == true)
	{
		qDebug() << "Internal Update.";
		m_internalUpdate = false;
		return;
	}
	QModelIndex dataIndex = getDataIndex(m_modelIndex);
	if ((topLeft != bottomRight)||(topLeft != dataIndex))
	{
		qDebug() << "Trival update in ImageView.";
		return;
	}
	/**
	 * the modification invoked by dataChanged should not yield any data model change again.
	 */
	 ///TODO:: This function needs parameters to determine whether the update is trival for this view
	//if (m_ptr.isNull() == true)
	//	return;

	//const int currentTopSliceIndex = m_ptr->getCurrentSliceIndex();
	//const int currentRightSliceIndex = m_ptr->getCurrentRightSliceIndex();
	//const int currentFrontSliceIndex = m_ptr->getCurrentFrontSliceIndex();

	//update slice and corresponding marks (e.g. grayscale streching and change of visibility of marks)
	resetSliceAndVisibleMarks( SliceType::SliceZ);
	resetSliceAndVisibleMarks( SliceType::SliceY);
	resetSliceAndVisibleMarks( SliceType::SliceX);
	updateActions();
}

void ImageView::activateItem(const QModelIndex & index)
{
	if (m_model == nullptr)
	{
		qWarning("Model is empty.");
		return;
	}

	QVariant var = m_model->data(getDataIndex(index));

	if (var.canConvert<QSharedPointer<ItemContext>>() == true)
	{
		m_modelIndex = index;
		m_ptr = var.value<QSharedPointer<ItemContext>>();

		const int currentTopSliceIndex = m_ptr->getCurrentSliceIndex();
		const int currentRightSliceIndex = m_ptr->getCurrentRightSliceIndex();
		const int currentFrontSliceIndex = m_ptr->getCurrentFrontSliceIndex();

		qDebug() << currentTopSliceIndex << " " << currentRightSliceIndex << " " << currentFrontSliceIndex;

		setTopSliceCount(m_ptr->getTopSliceCount());
		setRightSliceCount(m_ptr->getRightSliceCount());
		setFrontSliceCount(m_ptr->getFrontSliceCount());

		//set current slice and corresponding marks(remove previous marks)
		sliceChanged(currentTopSliceIndex, SliceType::SliceZ);
		sliceChanged(currentRightSliceIndex, SliceType::SliceY);
		sliceChanged(currentTopSliceIndex, SliceType::SliceX);
	}
	else
	{
		//invalid
		m_ptr.reset();
		m_modelIndex = QModelIndex();
	}
	updateActions();
}

void ImageView::setEnabled(bool enable)
{
	m_view->setEnabled(enable);
	m_topSlider->setEnabled(enable);
	m_frontSlider->setEnabled(enable);
	m_rightSlider->setEnabled(enable);

	m_colorAction->setEnabled(enable);
	m_markAction->setEnabled(enable);
	m_zoomInAction->setEnabled(enable);
	m_zoomOutAction->setEnabled(enable);

	m_frontSlicePlayAction->setEnabled(enable);
	m_topSlicePlayAction->setEnabled(enable);
	m_rightSlicePlayAction->setEnabled(enable);
}

//
//void ImageView::onZSliderValueChanged(int value)
//{
//	Q_ASSERT(m_ptr.isNull() == false);
//	resetSliceAndVisibleMarks(m_ptr->slice(value,SliceType::SliceZ),SliceType::SliceZ);
//	m_internalUpdate = true;
//	updateModel();
//}
//
//void ImageView::onYSliderValueChanged(int value)
//{
//	Q_ASSERT(m_ptr.isNull() == false);
//	//setRightImage(m_ptr->getOriginalRightSlice(value));
//	resetSliceAndVisibleMarks(m_ptr->slice(value, SliceType::SliceZ), SliceType::SliceZ);
//	m_internalUpdate = true;
//	updateModel();
//}
//
//void ImageView::onXSliderValueChanged(int value)
//{
//	Q_ASSERT(m_ptr.isNull() == false);
//	setFrontImage(m_ptr->getOriginalFrontSlice(value));
//	m_internalUpdate = true;
//	updateModel();
//}
void ImageView::onTopSliceTimer(bool enable)
{
	if (enable)
	{
		m_topTimerId = startTimer(10);
		//qDebug() << "onTopSliceTimer" << m_topTimerId;
		m_topSlicePlayDirection = Direction::Forward;
	}
	else
	{
		killTimer(m_topTimerId);
		m_topTimerId = 0;
	}
}

void ImageView::onRightSliceTimer(bool enable)
{
	if (enable)
	{
		m_rightTimerId = startTimer(10);
		//qDebug() << "onRightSliceTimer" << m_rightTimerId;
		m_rightSlicePlayDirection = Direction::Forward;
	}
	else
	{
		killTimer(m_rightTimerId);
		m_rightTimerId = 0;
	}
}

void ImageView::onFrontSliceTimer(bool enable)
{
	if (enable)
	{
		m_frontTimerId = startTimer(10);
		//qDebug() << "onFrontSliceTimer" << m_frontTimerId;
		m_frontSlicePlayDirection = Direction::Forward;
	}
	else
	{
		killTimer(m_frontTimerId);
		m_frontTimerId = 0;
	}
}

void ImageView::onColorChanged()
{
	qDebug() << "color";
	QColor color = QColorDialog::getColor(Qt::black, this, QStringLiteral("Color"));
	///TODO:: update color
	m_view->setColor(color);
}

void ImageView::timerEvent(QTimerEvent* event)
{
	int timeId = event->timerId();
	if (timeId == m_topTimerId)
	{
		int maxSlice = m_topSlider->maximum();
		int cur = m_topSlider->value();
		if (m_topSlicePlayDirection == Direction::Forward)
		{
			cur++;
		}
		else
		{
			cur--;
		}
		if (cur >= maxSlice)
		{
			m_topSlicePlayDirection = Direction::Backward;
		}
		else if (cur <= 0)
		{
			m_topSlicePlayDirection = Direction::Forward;
		}
		m_topSlider->setValue(cur);
	}
	else if (timeId == m_rightTimerId)
	{
		int maxSlice = m_rightSlider->maximum();
		int cur = m_rightSlider->value();
		if (m_rightSlicePlayDirection == Direction::Forward)
		{
			cur++;
		}
		else
		{
			cur--;
		}
		if (cur >= maxSlice)
		{
			m_rightSlicePlayDirection = Direction::Backward;
		}
		else if (cur <= 0)
		{
			m_rightSlicePlayDirection = Direction::Forward;
		}
		m_rightSlider->setValue(cur);

	}
	else if (timeId == m_frontTimerId)
	{
		int maxSlice = m_frontSlider->maximum();
		int cur = m_frontSlider->value();
		if (m_frontSlicePlayDirection == Direction::Forward)
		{
			cur++;
		}
		else
		{
			cur--;
		}
		if (cur >= maxSlice)
		{
			m_frontSlicePlayDirection = Direction::Backward;
		}
		else if (cur <= 0)
		{
			m_frontSlicePlayDirection = Direction::Forward;
		}
		m_frontSlider->setValue(cur);
	}
}

QModelIndex ImageView::getDataIndex(const QModelIndex & itemIndex)
{

	if (m_model == nullptr)
	{
		qWarning("Model pointer is nullptr");
		return QModelIndex();
	}
	return m_model->index(itemIndex.row(), 1, m_model->parent(itemIndex));
}

void ImageView::updateModel()
{
	if (m_model == nullptr)
	{
		qWarning("Model is empty.");
		return;
	}
	Q_ASSERT(m_model != nullptr);
	Q_ASSERT(m_ptr.isNull() == false);

	///TODO:: update marks added

	///TODO:: update current slice
	const int newCurrentTopSliceIndex = m_topSlider->value();
	const int newCurrentRightSliceIndex = m_rightSlider->value();
	const int newCurrentFrontSliceIndex = m_frontSlider->value();
	QModelIndex dataIndex = getDataIndex(m_modelIndex);
	Q_ASSERT(dataIndex.isValid());
	m_ptr->setCurrentSliceIndex(newCurrentTopSliceIndex);
	m_ptr->setCurrentRightSliceIndex(newCurrentRightSliceIndex);
	m_ptr->setCurrentFrontSliceIndex(newCurrentFrontSliceIndex);


	m_model->setData(dataIndex, QVariant::fromValue(m_ptr));
}

GraphicsView::GraphicsView(QWidget *parent) :QGraphicsView(parent),
m_scaleFactor(0.5),
m_currentPaintItem(nullptr),
m_paint(false),
m_moveble(true),
m_color(Qt::black),
m_topSlice(nullptr),
m_rightSlice(nullptr),
m_frontSlice(nullptr)
{
	m_scene = new GraphicsScene(this);
	setScene(m_scene);
	scale(m_scaleFactor, m_scaleFactor);
}

void GraphicsView::setMarks(const QList<QGraphicsItem*>& items, SliceType type)
{
	switch (type)
	{
	case SliceType::SliceZ:
		setTopSliceMarks(items);
		return;
	case SliceType::SliceY:
		setRightSliceMarks(items);
		return;
	case SliceType::SliceX:
		setFrontSliceMarks(items);
		return;
	}
}

void GraphicsView::setTopSliceMarks(const QList<QGraphicsItem*>& items)
{
	foreach(QGraphicsItem * item, items)
	{
		item->setParentItem(m_topSlice);
		item->setVisible(true);
	}
}

void GraphicsView::setRightSliceMarks(const QList<QGraphicsItem*>& items)
{
	foreach(QGraphicsItem * item, items)
	{
		item->setParentItem(m_rightSlice);
		item->setVisible(true);
	}
}

void GraphicsView::setFrontSliceMarks(const QList<QGraphicsItem*>& items)
{
	foreach(QGraphicsItem * item, items)
	{
		item->setParentItem(m_frontSlice);
		item->setVisible(true); 
	}
}

void GraphicsView::wheelEvent(QWheelEvent *event) {
	double numDegrees = -event->delta() / 8.0;
	double numSteps = numDegrees / 15.0;
	double factor = std::pow(1.125, numSteps);
	scale(factor, factor);
	//int delta = event->delta();
	//if (delta > 0) {
	//	scale(1.1, 1.1);
	//}
	//else {
	//	scale(0.9, 0.9);
	//}
	//qDebug() << "wheelEvent in View";
}


void GraphicsView::mousePressEvent(QMouseEvent *event)
{
	QPoint viewPos = event->pos();
	QPointF pos = mapToScene(viewPos);
	m_prevScenePoint = pos;
	auto items = scene()->items(pos);
	for (const auto & item : items) {
		SliceItem * slice = qgraphicsitem_cast<SliceItem*>(item);
		QPoint itemPoint = slice->mapFromScene(pos).toPoint();
		if (slice == m_topSlice)
		{
			//qDebug() << "emit z slice point" << itemPoint;
			emit zSliceSelected(itemPoint);
		}
		else if (slice == m_rightSlice)
		{
			emit ySliceSelected(itemPoint);
		}
		else if (slice == m_frontSlice)
		{
			emit xSliceSelected(itemPoint);
		}
		//find first slice item on which to draw
		if (slice != nullptr)
		{
			if (m_paint == true)
				m_currentPaintItem = slice;
			m_paintViewPointsBuffer.clear();
			m_paintViewPointsBuffer << viewPos;
			return;
		}
	}
	m_currentPaintItem = nullptr;

	QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	if (m_paint == true)
	{
		if (m_currentPaintItem != nullptr)
		{
			QPoint viewPos = event->pos();
			m_paintViewPointsBuffer << viewPos;
			return;
		}
	}
	else
	{
		QPointF currentScenePoint = mapToScene(event->pos());
		QPointF delta = currentScenePoint - m_prevScenePoint;
		m_prevScenePoint = currentScenePoint;
		auto items = scene()->items();
		for (const auto & item : items)
		{
			SliceItem * sliceItem = qgraphicsitem_cast<SliceItem*>(item);
			if (sliceItem != nullptr)
			{
				sliceItem->setPos(sliceItem->pos() + delta);
			}
		}
	}
	QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_paint == true)
	{
		if (m_currentPaintItem != nullptr)
		{
			if (m_paintViewPointsBuffer.empty() == false)
			{
				//draw a polygon and add to scene as the child of current paint item
				QPolygon poly(m_paintViewPointsBuffer);
				QPolygonF polyF = mapToScene(poly);

				polyF = m_currentPaintItem->mapFromScene(polyF);
				QGraphicsPolygonItem * polyItem = new QGraphicsPolygonItem(polyF, m_currentPaintItem);
				QBrush aBrush(m_color);
				QPen aPen(aBrush, 5, Qt::SolidLine);
				polyItem->setPen(aPen);
				polyItem->setZValue(100);

				//emit
				if (m_currentPaintItem == m_topSlice)
				{
					emit markAdded(polyItem, SliceType::SliceZ);
				}
				else if (m_currentPaintItem == m_rightSlice)
				{
					emit markAdded(polyItem, SliceType::SliceY);
				}
				else if (m_currentPaintItem == m_frontSlice)
				{
					emit markAdded(polyItem, SliceType::SliceX);
				}
				return;
			}
		}
	}
	QGraphicsView::mouseReleaseEvent(event);
}

GraphicsScene::GraphicsScene(QObject *parent) :QGraphicsScene(parent)
{

}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	qDebug() << "mousePressEvent in scene";
	QGraphicsScene::mousePressEvent(event);
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
	qDebug() << "mouseMoveEvent in scene";
	QGraphicsScene::mouseMoveEvent(event);
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
	qDebug() << "mouseReleaseEvent in scene";
	QGraphicsScene::mouseReleaseEvent(event);
}

void GraphicsScene::wheelEvent(QGraphicsSceneWheelEvent * event)
{
	qDebug() << "wheelEvent in scene";
	QGraphicsScene::wheelEvent(event);
}

StrokeMarkItem::StrokeMarkItem(QGraphicsItem * parent) :QGraphicsItem(parent)
{

}
void StrokeMarkItem::addPoint(const QPointF& p)
{
	prepareGeometryChange();
	m_boundingRect = unionWith(m_boundingRect, p);
	m_points << p;
	update();
}

void StrokeMarkItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{

}

QRectF StrokeMarkItem::unionWith(const QRectF & rect, const QPointF & p)
{
	const QPointF &topLeft = rect.topLeft();
	const QPointF &bottomRight = rect.bottomRight();
	QPointF newTopLeft = QPointF(std::min(p.x(), topLeft.x()), std::min(p.y(), topLeft.y()));
	QPointF newBottomRight = QPointF(std::max(p.x(), bottomRight.x()), std::max(p.y(), bottomRight.y()));
	return QRectF(newTopLeft, newBottomRight);
}

void StrokeMarkItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{

	QGraphicsItem::mousePressEvent(event);
}

void StrokeMarkItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{

	QGraphicsItem::mouseMoveEvent(event);
}

void StrokeMarkItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{

	QGraphicsItem::mouseReleaseEvent(event);
}

void StrokeMarkItem::wheelEvent(QGraphicsSceneWheelEvent * event)
{

	QGraphicsItem::wheelEvent(event);
}
void SliceItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{

	QGraphicsPixmapItem::mousePressEvent(event);
}

void SliceItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{

	QGraphicsPixmapItem::mouseMoveEvent(event);
}

void SliceItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{

	QGraphicsPixmapItem::mouseReleaseEvent(event);
}

void SliceItem::wheelEvent(QGraphicsSceneWheelEvent* event)
{

	QGraphicsPixmapItem::wheelEvent(event);
}

void GraphicsView::setTopImage(const QImage & image)
{
	if (m_topSlice == nullptr)
	{
		m_topSlice = new SliceItem(QPixmap::fromImage(image));
		m_topSlice->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
		m_scene->addItem(m_topSlice);
	}
	else
	{
		m_topSlice->setPixmap(QPixmap::fromImage(image));
	}
	QSize size = image.size();
	m_topSlice->setPos(-size.width() / 2, -size.height() / 2);

}

void GraphicsView::setRightImage(const QImage & image)
{
	if (m_rightSlice == nullptr)
	{
		m_rightSlice = new SliceItem(QPixmap::fromImage(image));
		m_scene->addItem(m_rightSlice);
		m_rightSlice->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
	}
	else
	{
		m_rightSlice->setPixmap(QPixmap::fromImage(image));
	}
	assert(m_topSlice != nullptr);
	QSize size = m_topSlice->pixmap().size();
	m_rightSlice->setPos(m_topSlice->pos() + QPointF(size.width() + 40, 0));

}

void GraphicsView::setFrontImage(const QImage & image)
{
	if (m_frontSlice == nullptr)
	{
		m_frontSlice = new SliceItem(QPixmap::fromImage(image));
		m_scene->addItem(m_frontSlice);
		m_frontSlice->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
	}
	else
	{
		m_frontSlice->setPixmap(QPixmap::fromImage(image));
	}
	assert(m_topSlice != nullptr);
	QSize size = m_topSlice->pixmap().size();
	m_frontSlice->setPos(m_topSlice->pos() + QPointF(0, size.height() + 40));
}

void GraphicsView::clearTopSliceMarks()
{
	if (m_topSlice == nullptr)
	{
		qWarning("Top slice is empty.");
		return;
	}
	auto children = m_topSlice->childItems();
	foreach(QGraphicsItem * item, children)
	{
		item->setParentItem(nullptr);
		item->setVisible(false);
	}
}

void GraphicsView::clearRightSliceMarks()
{
	if (m_rightSlice == nullptr)
	{
		qWarning("Right slice is empty.");
		return;
	}
	auto children = m_rightSlice->childItems();
	foreach(QGraphicsItem * item, children)
	{
		item->setParentItem(nullptr);
		item->setVisible(false);
	}
}

void GraphicsView::clearFrontSliceMarks()
{
	if (m_frontSlice == nullptr)
	{
		qWarning("Front slice is empty.");
		return;
	}
	auto children = m_frontSlice->childItems();
	foreach(QGraphicsItem * item, children)
	{
		item->setParentItem(nullptr);
		item->setVisible(false);
	}
}

void GraphicsView::setImage(const QImage& image, SliceType type)
{
	switch (type)
	{
	case SliceType::SliceZ:
		setTopImage(image);
		return;
	case SliceType::SliceY:
		setRightImage(image);
		return;
	case SliceType::SliceX:
		setFrontImage(image);
		return;
	}
}

void GraphicsView::clearSliceMarks(SliceType type)
{
	switch (type)
	{
	case SliceType::SliceZ:
		clearTopSliceMarks();
		return;
	case SliceType::SliceY:
		clearRightSliceMarks();
		return;
	case SliceType::SliceX:
		clearFrontSliceMarks();
		return;
	}
}

