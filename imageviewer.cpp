#include "imageviewer.h"
#include "titledsliderwithspinbox.h"
#include "histogram.h"
#include "ItemContext.h"
#include "pixelviewer.h"
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
#include <cassert>
#include <algorithm>
#include <complex>
#include <QMenu>
#include <QToolButton>
#include <QCheckBox>

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
			m_frontImageLabel = nullptr;
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

	//layout->setSizeConstraint(QLayout::SetFixedSize);
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

void ImageView::createToolBar()
{
	//createToolBar()
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
	m_layout->addWidget(m_toolBar, 0, 0, 1, 2);
	m_toolBar->addWidget(m_topSliceCheckBox);
	m_toolBar->addWidget(m_topSlider);
	m_toolBar->addAction(m_topSlicePlayAction);
	m_toolBar->addSeparator();
	m_toolBar->addWidget(m_rightSliceCheckBox);
	m_toolBar->addWidget(m_rightSlider);
	m_toolBar->addAction(m_rightSlicePlayAction);
	m_toolBar->addSeparator();
	m_toolBar->addWidget(m_frontSliceCheckBox);
	m_toolBar->addWidget(m_frontSlider);
	m_toolBar->addAction(m_frontSlicePlayAction);
	m_toolBar->addSeparator();
	m_toolBar->addAction(m_colorAction);
	m_toolBar->addAction(m_markAction);
	m_toolBar->addAction(m_zoomInAction);
	m_toolBar->addAction(m_zoomOutAction);

	//create menu on toolbar
	m_menu = new QMenu(this);
	m_menuButton = new QToolButton(this);
	m_menuButton->setText(QStringLiteral("Options"));
	m_menuButton->setPopupMode(QToolButton::MenuButtonPopup);
	m_menuButton->setMenu(m_menu);
	m_toolBar->addSeparator();
	m_toolBar->addWidget(m_menuButton);

	m_histDlg = m_menu->addAction(QStringLiteral("Histogram..."));

	//updateActions();
}

void ImageView::createConnections()
{
	connect(m_topSlider, &TitledSliderWithSpinBox::valueChanged, [=](int value) {emit sliderChanged(value, SliceType::SliceZ); });
	connect(m_rightSlider, &TitledSliderWithSpinBox::valueChanged, [=](int value) {emit sliderChanged(value, SliceType::SliceY); });
	connect(m_frontSlider, &TitledSliderWithSpinBox::valueChanged, [=](int value) {emit sliderChanged(value, SliceType::SliceX); });

	connect(m_topSlider, &TitledSliderWithSpinBox::valueChanged, [=](int value) {updateSlice(SliceType::SliceZ); });
	connect(m_rightSlider, &TitledSliderWithSpinBox::valueChanged, [=](int value) {updateSlice(SliceType::SliceY); });
	connect(m_frontSlider, &TitledSliderWithSpinBox::valueChanged, [=](int value) {updateSlice(SliceType::SliceX); });

	connect(m_topView, &GraphicsView::zSliceSelected, this, &ImageView::zSliceSelected);
	connect(m_rightView, &GraphicsView::zSliceSelected, this, &ImageView::ySliceSelected);
	connect(m_frontView, &GraphicsView::zSliceSelected, this, &ImageView::xSliceSelected);

	connect(m_markAction, SIGNAL(triggered(bool)), m_topView, SLOT(paintEnable(bool)));
	connect(m_topSlicePlayAction, SIGNAL(triggered(bool)), this, SLOT(onTopSliceTimer(bool)));
	connect(m_rightSlicePlayAction, SIGNAL(triggered(bool)), this, SLOT(onRightSliceTimer(bool)));
	connect(m_frontSlicePlayAction, SIGNAL(triggered(bool)), this, SLOT(onFrontSliceTimer(bool)));
	connect(m_colorAction, &QAction::triggered, this, &ImageView::onColorChanged);
	connect(m_zoomInAction, &QAction::triggered, [=]() {double factor = std::pow(1.125, 1); m_topView->scale(factor, factor); });
	connect(m_zoomOutAction, &QAction::triggered, [=]() {double factor = std::pow(1.125, -1); m_topView->scale(factor, factor); });

	connect(m_histDlg, &QAction::triggered, []()
	{
		///TODO::open histogram dialog


	});
	connect(m_topSliceCheckBox, &QCheckBox::toggled, [this](bool toggle) {updateTopSliceActions(); });
	connect(m_rightSliceCheckBox, &QCheckBox::toggled, [this](bool toggle) {updateRightSliceActions(); });
	connect(m_frontSliceCheckBox, &QCheckBox::toggled, [this](bool toggle) {updateFrontSliceActions(); });

}

void ImageView::updateActions()
{
	updateTopSliceActions();
	updateRightSliceActions();
	updateFrontSliceActions();
}

void ImageView::updateTopSliceActions()
{
	bool enable = m_topSliceCheckBox->isChecked() && m_sliceModel != nullptr;
	m_topSlicePlayAction->setEnabled(enable);
	m_topSlider->setEnabled(enable);
	m_topView->setHidden(!enable);
}

void ImageView::updateFrontSliceActions()
{
	bool enable = m_frontSliceCheckBox->isChecked() && m_sliceModel != nullptr;
	m_frontSlicePlayAction->setEnabled(enable);
	m_frontSlider->setEnabled(enable);
	m_frontView->setHidden(!enable);
}

void ImageView::updateRightSliceActions()
{
	bool enable = m_rightSliceCheckBox->isChecked() && m_sliceModel != nullptr;
	m_rightSlicePlayAction->setEnabled(enable);
	m_rightSlider->setEnabled(enable);
	m_rightView->setHidden(!enable);
}

void ImageView::createContextMenu()
{
	m_contextMenu = new QMenu(this);
	///TODO:: add icons here
	m_zoomIn = new QAction(QStringLiteral("Zoom In"), this);
	m_zoomOut = new QAction(QStringLiteral("Zoom Out"), this);

	m_histDlgAction = new QAction(QStringLiteral("Histgoram..."), this);
	m_pixelViewDlgAction = new QAction(QStringLiteral("Pixel View..."), this);
	m_marksManagerDlgAction = new QAction(QStringLiteral("Marks..."), this);

	connect(m_histDlgAction, &QAction::triggered, [this]()
	{
		AbstractPlugin * histViewDlg;
		if (m_menuWidget == m_topView)
		{
			histViewDlg = new HistogramViewer(SliceType::SliceZ, m_topView, m_sliceModel, this);
			histViewDlg->setWindowFlag(Qt::Window);
			histViewDlg->show();
			histViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//histViewDlg->sliceOpenEvent(m_topSlider->value());
			connect(this, &ImageView::sliceOpened, histViewDlg, &AbstractPlugin::sliceOpened);
			connect(this, &ImageView::slicePlayStoped, histViewDlg, &AbstractPlugin::slicePlayStoped);

			emit sliceOpened(m_topSlider->value());
		}
		else if (m_menuWidget == m_rightView)
		{
			histViewDlg = new HistogramViewer(SliceType::SliceY, m_rightView, m_sliceModel, this);
			histViewDlg->setWindowFlag(Qt::Window);
			histViewDlg->show();
			histViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//histViewDlg->sliceOpenEvent(m_rightSlider->value());
			connect(this, &ImageView::sliceOpened, histViewDlg, &AbstractPlugin::sliceOpened);
			connect(this, &ImageView::slicePlayStoped, histViewDlg, &AbstractPlugin::slicePlayStoped);
			emit sliceOpened(m_rightSlider->value());
		}
		else if (m_menuWidget == m_frontView)
		{
			histViewDlg = new HistogramViewer(SliceType::SliceX, m_frontView, m_sliceModel, this);
			histViewDlg->setWindowFlag(Qt::Window);
			histViewDlg->show();
			histViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//histViewDlg->sliceOpenEvent(m_frontSlider->value());
			connect(this, &ImageView::sliceOpened, histViewDlg, &AbstractPlugin::sliceOpened);
			connect(this, &ImageView::slicePlayStoped, histViewDlg, &AbstractPlugin::slicePlayStoped);
			emit sliceOpened(m_frontSlider->value());
		}
		//connect(this, &ImageView::sliceChanged, histViewDlg, &AbstractPlugin::sliceChanged);
		connect(this, &ImageView::slicePlayStoped, histViewDlg, &AbstractPlugin::slicePlayStoped);
	});
	connect(m_pixelViewDlgAction, &QAction::triggered, [this]()
	{
		PixelViewer * pixelViewDlg;
		if(m_menuWidget == m_topView)
		{
			pixelViewDlg = new PixelViewer(SliceType::SliceX, m_frontView, m_sliceModel, this);
			pixelViewDlg->setWindowFlag(Qt::Window);
			pixelViewDlg->show();
			pixelViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			pixelViewDlg->setImage(m_sliceModel->topSlice(m_topSlider->value()));
			connect(m_topView, &GraphicsView::zSliceSelected, pixelViewDlg, &AbstractPlugin::sliceSelected);

		}else if(m_menuWidget == m_rightView)
		{
			pixelViewDlg = new PixelViewer(SliceType::SliceX, m_frontView, m_sliceModel, this);
			pixelViewDlg->setWindowFlag(Qt::Window);
			pixelViewDlg->show();
			pixelViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			pixelViewDlg->setImage(m_sliceModel->rightSlice(m_rightSlider->value()));
			connect(m_rightView, &GraphicsView::zSliceSelected, pixelViewDlg, &AbstractPlugin::sliceSelected);
		}else if(m_menuWidget == m_frontView)
		{
			pixelViewDlg = new PixelViewer(SliceType::SliceX, m_frontView, m_sliceModel, this);
			pixelViewDlg->setWindowFlag(Qt::Window);
			pixelViewDlg->show();
			pixelViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			pixelViewDlg->setImage(m_sliceModel->frontSlice(m_frontSlider->value()));
			connect(m_frontView, &GraphicsView::zSliceSelected, pixelViewDlg, &AbstractPlugin::sliceSelected);
		}
	});
	connect(m_marksManagerDlgAction,&QAction::triggered,[this]()
	{	

	});

	m_contextMenu->addAction(m_zoomIn);
	m_contextMenu->addAction(m_zoomOut);
	m_contextMenu->addSeparator();
	m_contextMenu->addAction(m_histDlgAction);
	m_contextMenu->addAction(m_pixelViewDlgAction);
	m_contextMenu->addAction(m_marksManagerDlgAction);
}

ImageView::ImageView(QWidget *parent, bool topSliceVisible, bool rightSliceVisible, bool frontSliceVisible, AbstractSliceDataModel * model) :
	QWidget(parent),
	//m_topSlice(nullptr),
	m_rightSlice(nullptr),
	m_frontSlice(nullptr),
	m_markModel(nullptr),
	m_sliceModel(model)
{
	//layout
	m_layout = new QGridLayout(this);
	//QGraphicsView
	m_topView = new GraphicsView;
	m_rightView = new GraphicsView;
	m_frontView = new GraphicsView;
	//m_view->resize(500, 500);
	//m_rightView->resize(50, 500);
	//m_frontView->resize(500, 50);
	m_layout->setSizeConstraint(QLayout::SetFixedSize);
	//m_layout->setColumnStretch(0, 5);
	//m_layout->setColumnStretch(1,1);
	m_layout->addWidget(m_topView, 1, 0, 1, 1, Qt::AlignCenter);
	m_layout->addWidget(m_rightView, 1, 1, 1, 1, Qt::AlignCenter);
	m_layout->addWidget(m_frontView, 2, 0, 1, 1, Qt::AlignCenter);
	//m_view->setScene(m_scene);

	//sliders

	m_topSliceCheckBox = new QCheckBox;
	m_topSliceCheckBox->setChecked(topSliceVisible);
	m_topSlider = new TitledSliderWithSpinBox(this, tr("Z:"));
	m_rightSliceCheckBox = new QCheckBox;
	m_rightSliceCheckBox->setChecked(rightSliceVisible);
	m_rightSlider = new TitledSliderWithSpinBox(this, tr("X:"));
	m_frontSliceCheckBox = new QCheckBox;
	m_frontSliceCheckBox->setChecked(frontSliceVisible);
	m_frontSlider = new TitledSliderWithSpinBox(this, tr("Y:"));




	//This routine should be a part of updateModel
	//connect(m_topView, &GraphicsView::markAdded, [=](QGraphicsItem * item, SliceType type)
	//{
	//	if (m_model == nullptr)
	//	{
	//		qWarning("Model is empty.");
	//		return;
	//	}
	//	int index = currentIndex(type);
	//	m_ptr->addSliceMark(item, index, type);
	//	m_internalUpdate = true;
	//	m_model->setData(getDataIndex(m_modelIndex), QVariant::fromValue(m_ptr));

	//	//find its mark index
	//	constexpr int MarkRowNumber = 1;
	//	QModelIndex markIndex = m_model->index(MarkRowNumber, 0, m_modelIndex);
	//	QModelIndex countField = m_model->index(MarkRowNumber, 1, m_modelIndex);

	//	int count;
	//	QVariant var = m_model->data(countField);
	//	if (var.canConvert<int>() == true)
	//		count = (var.value<int>());

	//	bool success = m_model->insertRow(m_model->rowCount(markIndex), markIndex);
	//	if (success == false)
	//	{
	//		qCritical("Inserting row into model for mark failed\n");
	//		return;
	//	}
	//	QModelIndex newMark = m_model->index(m_model->rowCount(markIndex) - 1, 0, markIndex);
	//	if (newMark.isValid() == true)
	//	{
	//		m_internalUpdate = true;
	//		//set text
	//		m_model->setData(newMark, QString("#%1").arg(count));
	//		//set bool
	//		m_internalUpdate = true;
	//		m_model->setData(newMark.sibling(newMark.row(), newMark.column() + 1), QVariant::fromValue(true));

	//		m_internalUpdate = true;
	//		m_model->setData(countField, QVariant::fromValue(++count));
	//	}
	//});
	//action
	createToolBar();


	//m_topGroup = new QGroupBox(QStringLiteral("Top"), this);

	//m_rightGroup = new QGroupBox(QStringLiteral("Right"), this);
	//m_frontGroup = new QGroupBox(QStringLiteral("Front"), this);
	//setContextMenuPolicy(Qt::ActionsContextMenu);
	createContextMenu();


	createConnections();
	updateActions();
	setLayout(m_layout);
}

void ImageView::sliceChanged(int value, SliceType type)
{
	Q_ASSERT_X(m_sliceModel != nullptr, "ImageView::sliceChanged", "null model pointer");
	GraphicsView * view = nullptr;
	std::function<QImage(int)> sliceGetter;
	//TODO:: 
	switch (type)
	{
	case SliceType::SliceZ:
		view = m_topView;
		sliceGetter = std::bind(&AbstractSliceDataModel::frontSlice, m_sliceModel, std::placeholders::_1);
		break;
	case SliceType::SliceY:
		view = m_rightView;
		sliceGetter = std::bind(&AbstractSliceDataModel::rightSlice, m_sliceModel, std::placeholders::_1);
		break;
	case SliceType::SliceX:
		view = m_frontView;
		sliceGetter = std::bind(&AbstractSliceDataModel::frontSlice, m_sliceModel, std::placeholders::_1);
		break;
	default:
		Q_ASSERT_X(false, "ImageView::updateSlice", "SliceType error.");
	}
	Q_ASSERT_X(static_cast<bool>(sliceGetter) == true, "ImageView::updateSlice", "invalid bind");
	view->setImage(sliceGetter(value));
	view->clearSliceMarks();
}

inline void ImageView::setTopSliceCount(int value) { m_topSlider->setMaximum(value - 1); }
inline void ImageView::setRightSliceCount(int value) { m_rightSlider->setMaximum(value - 1); }
inline void ImageView::setFrontSliceCount(int value) { m_frontSlider->setMaximum(value - 1); }
int ImageView::currentIndex(SliceType type)
{
	switch (type)
	{
	case SliceType::SliceZ:
		return getZSliceValue();
	case SliceType::SliceY:
		return getYSliceValue();
	case SliceType::SliceX:
		return getXSliceValue();
	default:
		return -1;
	}
}

inline bool ImageView::contains(const QWidget* widget, const QPoint& pos) { return (widget->rect()).contains(pos); }

//void ImageView::resetSliceAndVisibleMarks(SliceType type)
//{
//	if (m_sliceModel == nullptr)
//		return;
//	GraphicsView * view = nullptr;
//	switch (type)
//	{
//	case SliceType::SliceZ:
//		view = m_topView;
//		break;
//	case SliceType::SliceY:
//		view = m_rightView;
//		break;
//	case SliceType::SliceX:
//		view = m_frontView;
//		break;
//	default:
//		Q_ASSERT_X(false, "ImageView::resetSliceAndVisibleMarks", "SliceType error.");
//	}
//	int index = currentIndex(type);
//	view->setImage(m_ptr->slice(index, type));
//	view->clearSliceMarks(SliceType::SliceZ);
//	view->setMarks(m_ptr->visibleSliceMarks(index, type));
//}

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

inline int ImageView::getZSliceValue() const { return m_topSlider->value(); }

inline int ImageView::getYSliceValue() const { return m_rightSlider->value(); }

inline int ImageView::getXSliceValue() const { return m_frontSlider->value(); }

inline void ImageView::setZXliceEnable(bool enable)
{
	m_topSliceCheckBox->setChecked(enable);
	updateActions();
}

inline void ImageView::setYXliceEnable(bool enable)
{
	m_rightSliceCheckBox->setChecked(enable);
	updateActions();
}

inline void ImageView::setXXliceEnable(bool enable)
{
	m_frontSliceCheckBox->setChecked(enable);
	updateActions();
}

void ImageView::setSliceModel(AbstractSliceDataModel * model)
{
	m_sliceModel = model;
	updateSliceCount(SliceType::SliceZ);
	updateSliceCount(SliceType::SliceY);
	updateSliceCount(SliceType::SliceX);
	updateSlice(SliceType::SliceX);
	updateSlice(SliceType::SliceY);
	updateSlice(SliceType::SliceZ);
	//TODO::update marks

	updateActions();
}

//void ImageView::setModel(DataItemModel * model)
//{
//	if (m_model != model)
//	{
//		m_model = model;
//		disconnect(m_model, &DataItemModel::dataChanged, this, &ImageView::dataChanged);
//
//		connect(m_model, &DataItemModel::dataChanged, this, &ImageView::dataChanged);
//		///TODO::get corresponding data i.e. current slice (top)
//	}
//}
//
//void ImageView::dataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int>& roles)
//{
//	qDebug() << "In ImageView:Model has been updated.";
//	if (m_internalUpdate == true)
//	{
//		qDebug() << "Internal Update.";
//		m_internalUpdate = false;
//		return;
//	}
//	QModelIndex dataIndex = getDataIndex(m_modelIndex);
//	//if ((topLeft != bottomRight)||(topLeft != dataIndex))
//	//{
//	//	qDebug() << "Trival update in ImageView.";
//	//	return;
//	//}
//	/**
//	 * the modification invoked by dataChanged should not yield any data model change again.
//	 */
//	 ///TODO:: This function needs parameters to determine whether the update is trival for this view
//	//if (m_ptr.isNull() == true)
//	//	return;
//
//	//const int currentTopSliceIndex = m_ptr->getCurrentSliceIndex();
//	//const int currentRightSliceIndex = m_ptr->getCurrentRightSliceIndex();
//	//const int currentFrontSliceIndex = m_ptr->getCurrentFrontSliceIndex();
//
//	//update slice and corresponding marks (e.g. grayscale streching and change of visibility of marks)
//	resetSliceAndVisibleMarks(SliceType::SliceZ);
//	resetSliceAndVisibleMarks(SliceType::SliceY);
//	resetSliceAndVisibleMarks(SliceType::SliceX);
//	updateActions();
//}
//
//void ImageView::activateItem(const QModelIndex & index)
//{
//	if (m_model == nullptr)
//	{
//		qWarning("Model is empty.");
//		return;
//	}
//
//	QVariant var = m_model->data(getDataIndex(index));
//
//	if (var.canConvert<QSharedPointer<ItemContext>>() == true)
//	{
//		m_modelIndex = index;
//		m_ptr = var.value<QSharedPointer<ItemContext>>();
//
//		const int currentTopSliceIndex = m_ptr->getCurrentSliceIndex();
//		const int currentRightSliceIndex = m_ptr->getCurrentRightSliceIndex();
//		const int currentFrontSliceIndex = m_ptr->getCurrentFrontSliceIndex();
//
//		qDebug() << currentTopSliceIndex << " " << currentRightSliceIndex << " " << currentFrontSliceIndex;
//
//		setTopSliceCount(m_ptr->getTopSliceCount());
//		setRightSliceCount(m_ptr->getRightSliceCount());
//		setFrontSliceCount(m_ptr->getFrontSliceCount());
//
//		//set current slice and corresponding marks(remove previous marks)
//		sliceChanged(currentTopSliceIndex, SliceType::SliceZ);
//		sliceChanged(currentRightSliceIndex, SliceType::SliceY);
//		sliceChanged(currentTopSliceIndex, SliceType::SliceX);
//	}
//	else
//	{
//		//invalid
//		m_ptr.reset();
//		m_modelIndex = QModelIndex();
//	}
//	updateActions();
//}

void ImageView::setEnabled(bool enable)
{
	m_topSliceCheckBox->setEnabled(enable);
	m_rightSliceCheckBox->setEnabled(enable);
	m_frontSliceCheckBox->setEnabled(enable);

	m_colorAction->setEnabled(enable);
	m_markAction->setEnabled(enable);
	m_zoomInAction->setEnabled(enable);
	m_zoomOutAction->setEnabled(enable);
	m_menuButton->setEnabled(enable);

	m_topSlicePlayAction->setEnabled(enable);
	m_topSlider->setEnabled(enable);
	m_topView->setEnabled(enable);
	m_topSliceCheckBox->setEnabled(enable);

	m_rightView->setEnabled(enable);
	m_rightSlicePlayAction->setEnabled(enable);
	m_rightSlider->setEnabled(enable);
	m_topSliceCheckBox->setEnabled(enable);

	m_frontSlider->setEnabled(enable);
	m_frontView->setEnabled(enable);
	m_frontSlicePlayAction->setEnabled(enable);
	m_frontSliceCheckBox->setEnabled(enable);
}


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
	m_topView->setColor(color);
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

void ImageView::contextMenuEvent(QContextMenuEvent* event)
{
	const QPoint pos = event->pos();
	if (true == contains(m_topView, m_topView->mapFrom(this, pos)))
	{
		m_menuWidget = m_topView;
		m_contextMenu->exec(this->mapToGlobal(pos));
	}
	else if (true == contains(m_rightView, m_rightView->mapFrom(this, pos)))
	{
		m_menuWidget = m_rightView;
		m_contextMenu->exec(this->mapToGlobal(pos));
	}
	else if (true == contains(m_frontView, m_frontView->mapFrom(this, pos)))
	{
		m_menuWidget = m_frontView;
		m_contextMenu->exec(this->mapToGlobal(pos));
	}
	event->accept();
}



//QModelIndex ImageView::getDataIndex(const QModelIndex & itemIndex)
//{
//
//	if (m_model == nullptr)
//	{
//		qWarning("Model pointer is nullptr");
//		return QModelIndex();
//	}
//	return m_model->index(itemIndex.row(), 1, m_model->parent(itemIndex));
//}

//void ImageView::updateModel()
//{
//	if (m_model == nullptr)
//	{
//		qWarning("Model is empty.");
//		return;
//	}
//	Q_ASSERT(m_model != nullptr);
//	Q_ASSERT(m_ptr.isNull() == false);
//
//	//TODO::update marks added
//
//	//TODO::update current slice
//	const int newCurrentTopSliceIndex = m_topSlider->value();
//	const int newCurrentRightSliceIndex = m_rightSlider->value();
//	const int newCurrentFrontSliceIndex = m_frontSlider->value();
//	QModelIndex dataIndex = getDataIndex(m_modelIndex);
//	Q_ASSERT(dataIndex.isValid());
//	m_ptr->setCurrentSliceIndex(newCurrentTopSliceIndex);
//	m_ptr->setCurrentRightSliceIndex(newCurrentRightSliceIndex);
//	m_ptr->setCurrentFrontSliceIndex(newCurrentFrontSliceIndex);
//
//
//	m_model->setData(dataIndex, QVariant::fromValue(m_ptr));
//}

void ImageView::updateSliceCount(SliceType type)
{
	switch (type)
	{
	case SliceType::SliceZ:
		//m_topSlider->setValue(m_sliceModel->topSliceCount()-1);
		setTopSliceCount(m_sliceModel->topSliceCount() - 1);
		break;
	case SliceType::SliceY:
		setRightSliceCount(m_sliceModel->rightSliceCount() - 1);
		break;
	case SliceType::SliceX:
		setFrontSliceCount(m_sliceModel->frontSliceCount() - 1);
		break;
	default:
		break;
	}
}

void ImageView::updateSlice(SliceType type)
{
	//
	GraphicsView * view = nullptr;
	std::function<QImage(int)> sliceGetter;
	switch (type)
	{
	case SliceType::SliceZ:
		view = m_topView;
		sliceGetter = std::bind(&AbstractSliceDataModel::topSlice, m_sliceModel, std::placeholders::_1);
		break;
	case SliceType::SliceY:
		view = m_rightView;
		sliceGetter = std::bind(&AbstractSliceDataModel::rightSlice, m_sliceModel, std::placeholders::_1);
		break;
	case SliceType::SliceX:
		view = m_frontView;
		sliceGetter = std::bind(&AbstractSliceDataModel::frontSlice, m_sliceModel, std::placeholders::_1);
		break;
	default:
		Q_ASSERT_X(false, "ImageView::updateSlice", "SliceType error.");
	}
	Q_ASSERT_X(static_cast<bool>(sliceGetter) == true, "ImageView::updateSlice", "null function");
	int index = currentIndex(type);
	view->setImage(sliceGetter(index));
	view->clearSliceMarks(SliceType::SliceZ);
	//view->setMarks();
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
	setScene(new GraphicsScene(this));
	scale(m_scaleFactor, m_scaleFactor);

	//setContextMenuPolicy(Qt::CustomContextMenu);
	//createContextMenu();
	//createDialog();
	//connect(this, &GraphicsView::customContextMenuRequested, [this](const QPoint&pos) {m_contextMenu->exec(this->mapToGlobal(pos)); });
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
	QSize size = image.size();
	QPoint pos = QPoint(-size.width() / 2, -size.height() / 2);
	setImageHelper(pos, image, m_topSlice, &m_topImage);

}

void GraphicsView::setRightImage(const QImage & image)
{
	Q_ASSERT_X(false, "GraphicsView::setRightImage", "This function should be removed.");

	if (m_rightSlice == nullptr)
	{
		m_rightSlice = new SliceItem(QPixmap::fromImage(image));
		scene()->addItem(m_rightSlice);
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
	Q_ASSERT_X(false, "GraphicsView::setFrontImage", "This function should be removed.");
	if (m_frontSlice == nullptr)
	{
		m_frontSlice = new SliceItem(QPixmap::fromImage(image));
		scene()->addItem(m_frontSlice);
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

void GraphicsView::setImageHelper(const QPoint& pos, const QImage& inImage, SliceItem*& sliceItem, QImage * outImage)
{

	if (sliceItem == nullptr)
	{
		sliceItem = new SliceItem(QPixmap::fromImage(inImage));
		(sliceItem)->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
		sliceItem->setPos(pos);
		scene()->addItem(sliceItem);

	}
	else
	{
		sliceItem->setPixmap(QPixmap::fromImage(inImage));
	}
	QSize size = inImage.size();
	*outImage = inImage;
	//(sliceItem)->setPos(pos);
}

void GraphicsView::clearSliceMarksHelper(SliceItem* sliceItem)
{
	if (sliceItem == nullptr)
	{
		qWarning("Top slice is empty.");
		return;
	}
	auto children = sliceItem->childItems();
	foreach(QGraphicsItem * item, children)
	{
		item->setParentItem(nullptr);
		item->setVisible(false);
	}
}
void GraphicsView::setMarksHelper(SliceItem* sliceItem, const QList<QGraphicsItem*>& items)
{
	foreach(QGraphicsItem * item, items)
	{
		item->setParentItem(sliceItem);
		item->setVisible(true);
	}
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
		clearSliceMarksHelper(m_topSlice);
		return;
	case SliceType::SliceY:
		clearSliceMarksHelper(m_rightSlice);
		return;
	case SliceType::SliceX:
		clearSliceMarksHelper(m_frontSlice);
		return;
	}
}


void AbstractSliceDataModel::setTopSlice(const QImage& image, int index)
{
	m_modifiedTopSliceFlags[index] = true;
	m_modifiedTopSlice[index] = image;
}

void AbstractSliceDataModel::setRightSlice(const QImage& image, int index)
{
	m_modifiedRightSliceFlags[index] = true;
	m_modifiedRightSlice[index] = image;
}

void AbstractSliceDataModel::setFrontSlice(const QImage& image, int index)
{
	m_modifiedFrontSliceFlags[index] = true;
	m_modifiedFrontSlice[index] = image;
}

QImage AbstractSliceDataModel::topSlice(int index) const
{
	if (m_modifiedTopSliceFlags[index] == false)
		return originalTopSlice(index);
	return m_modifiedTopSlice[index];
}

QImage AbstractSliceDataModel::rightSlice(int index) const
{
	if (m_modifiedRightSliceFlags[index] == false)
		return originalRightSlice(index);
	return m_modifiedRightSlice[index];
}

QImage AbstractSliceDataModel::frontSlice(int index) const
{
	if (m_modifiedFrontSliceFlags[index] == false)
		return originalFrontSlice(index);
	return m_modifiedFrontSlice[index];

}
AbstractSliceDataModel::AbstractSliceDataModel(int nTop, int nRight, int nFront)
{
	m_modifiedFrontSlice.resize(nFront);
	m_modifiedFrontSliceFlags.resize(nFront);
	m_modifiedRightSlice.resize(nRight);
	m_modifiedRightSliceFlags.resize(nRight);
	m_modifiedTopSlice.resize(nTop);
	m_modifiedTopSliceFlags.resize(nTop);
}
