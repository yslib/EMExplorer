#include "imageviewer.h"
#include "titledsliderwithspinbox.h"
#include "histogram.h"
#include "ItemContext.h"
#include "pixelviewer.h"
#include "markcategray.h"
#include "markmodel.h"
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
#include <QComboBox>
#include <QMessageBox>


inline QGraphicsItem * QueryMarkItemInterface(AbstractMarkItem * mark)
{
	return static_cast<QGraphicsItem*>(static_cast<PolyMarkItem*>(mark));
}
inline AbstractMarkItem * QueryMarkItemInterface(QGraphicsItem* mark)
{
	return static_cast<AbstractMarkItem*>(static_cast<PolyMarkItem*>(mark));
}

void ImageView::createWidgets()
{

}

void ImageView::createToolBar()
{
	//createToolBar()
	m_topSlicePlayAction = new QAction(tr("Play"), this);
	m_topSlicePlayAction->setCheckable(true);
	m_rightSlicePlayAction = new QAction(tr("Play"), this);
	m_rightSlicePlayAction->setCheckable(true);
	m_frontSlicePlayAction = new QAction(tr("Play"), this);
	m_frontSlicePlayAction->setCheckable(true);


	m_markAction = new QAction(QStringLiteral("Mark"), this);
	m_markAction->setCheckable(true);
	m_colorAction = new QAction(QStringLiteral("Color"), this);

	m_zoomInAction = new QAction(QStringLiteral("ZoomIn"), this);
	m_zoomOutAction = new QAction(QStringLiteral("ZoomOut"), this);

	m_addCategoryAction = new QAction(QStringLiteral("Add..."), this);
	m_categoryLabel = new QLabel(QStringLiteral("Category:"), this);
	m_categoryCBBox = new QComboBox(this);

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
	m_toolBar->addWidget(m_categoryLabel);
	m_toolBar->addWidget(m_categoryCBBox);
	m_toolBar->addAction(m_addCategoryAction);
	m_toolBar->addAction(m_colorAction);
	m_toolBar->addAction(m_markAction);
	m_toolBar->addSeparator();
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
	//forward slider signals
	connect(m_topSlider, &TitledSliderWithSpinBox::valueChanged, this, &ImageView::topSliceChanged);
	connect(m_rightSlider, &TitledSliderWithSpinBox::valueChanged, this, &ImageView::rightSliceChanged);
	connect(m_frontSlider, &TitledSliderWithSpinBox::valueChanged, this, &ImageView::frontSliceChanged);
	//update slice
	connect(m_topSlider, &TitledSliderWithSpinBox::valueChanged, [=](int value) {Q_UNUSED(value); updateSlice(SliceType::Top); });
	connect(m_rightSlider, &TitledSliderWithSpinBox::valueChanged, [=](int value) {Q_UNUSED(value); updateSlice(SliceType::Right); });
	connect(m_frontSlider, &TitledSliderWithSpinBox::valueChanged, [=](int value) {Q_UNUSED(value); updateSlice(SliceType::Front); });
	//forward selected signals
	connect(m_topView, &SliceView::sliceSelected, this, &ImageView::topSliceSelected);
	connect(m_rightView, &SliceView::sliceSelected, this, &ImageView::rightSliceSelected);
	connect(m_frontView, &SliceView::sliceSelected, this, &ImageView::frontSliceSelected);

	connect(m_topView, &SliceView::markAdded, [this](QGraphicsItem* mark)
	{
		qDebug() << "top View mark added";
		int index = m_topSlider->value();
		QString cate = m_categoryCBBox->currentText();
		if (cate.isEmpty())
		{
			cate = QString("Category #%1").arg(m_categoryCBBox->count());
			m_categoryCBBox->addItem(cate, QVariant::fromValue(Qt::black));
			m_categoryCBBox->setCurrentText(cate);
		}
		auto m = dynamic_cast<AbstractMarkItem*>(mark);
		Q_ASSERT_X(m, "SliceView::markAdded", "dynamic_cast error");
		m->setSliceIndex(index);
		if (m_markModel != nullptr)m_markModel->addMark(cate, m);

	});
	connect(m_rightView, &SliceView::markAdded, [this](QGraphicsItem* mark)
	{
		qDebug() << "right View mark added";
		int index = m_rightSlider->value();
		QString cate = m_categoryCBBox->currentIndex();
		if (cate.isEmpty())
		{
			cate = QString("Category #%1").arg(m_categoryCBBox->count());
			m_categoryCBBox->addItem(cate, QVariant::fromValue(Qt::black));
			m_categoryCBBox->setCurrentText(cate);
		}
		auto m = dynamic_cast<AbstractMarkItem*>(mark);
		Q_ASSERT_X(m, "SliceView::markAdded", "dynamic_cast error");
		m->setSliceIndex(index);
		if (m_markModel != nullptr)m_markModel->addMark(cate, m);
	});
	connect(m_frontView, &SliceView::markAdded, [this](QGraphicsItem* mark)
	{
		qDebug() << "front View mark added";
		int index = m_frontSlider->value();
		QString cate = m_categoryCBBox->currentIndex();
		if (cate.isEmpty())
		{
			cate = QString("Category #%1").arg(m_categoryCBBox->count());
			m_categoryCBBox->addItem(cate, QVariant::fromValue(Qt::black));
			m_categoryCBBox->setCurrentText(cate);
		}
		auto m = dynamic_cast<AbstractMarkItem*>(mark);
		Q_ASSERT_X(m, "SliceView::markAdded", "dynamic_cast error");
		m->setSliceIndex(index);
		if (m_markModel != nullptr)m_markModel->addMark(cate, m);
	});

	connect(m_reset, &QPushButton::clicked, [this](bool click)
	{
		Q_UNUSED(click);
		m_topView->resetMatrix();
		m_rightView->resetMatrix();
		m_frontView->resetMatrix();
	});
	connect(m_markAction, &QAction::triggered, m_topView, &SliceView::paintEnable);
	connect(m_topSlicePlayAction, &QAction::triggered, [this](bool enable) {onTopSlicePlay(enable); if (!enable)emit topSlicePlayStoped(m_topSlider->value()); });
	connect(m_rightSlicePlayAction, &QAction::triggered, [this](bool enable) {onRightSlicePlay(enable); if (!enable)emit rightSlicePlayStoped(m_rightSlider->value()); });
	connect(m_frontSlicePlayAction, &QAction::triggered, [this](bool enable) {onFrontSlicePlay(enable); if (!enable)emit frontSlicePlayStoped(m_frontSlider->value()); });
	connect(m_addCategoryAction, &QAction::triggered, [this]()
	{
		MarkCategray dlg(this);
		connect(&dlg, &MarkCategray::resultReceived, [this](const QString & name, const QColor & color)
		{
			m_categoryCBBox->addItem(name);
			m_categoryCBBox->setCurrentText(name);
			int index = m_categoryCBBox->currentIndex();
			m_categoryCBBox->setItemData(index, color);
			setColor(color);
		});
		dlg.exec();
	});
	connect(m_colorAction, &QAction::triggered, [this]()
	{
		auto d = m_categoryCBBox->itemData(m_categoryCBBox->currentIndex());
		QColor c = d.canConvert<QColor>() ? d.value<QColor>() : Qt::black;
		setColor(QColorDialog::getColor(c, this, QStringLiteral("Color")));
	});
	connect(m_zoomInAction, &QAction::triggered, [this]() {double factor = std::pow(1.125, 1); m_topView->scale(factor, factor); });
	connect(m_zoomOutAction, &QAction::triggered, [this]() {double factor = std::pow(1.125, -1); m_topView->scale(factor, factor); });
	connect(m_histDlg, &QAction::triggered, []()
	{
		///TODO::open histogram dialog
	});
	connect(m_topSliceCheckBox, &QCheckBox::toggled, [this](bool toggle) {Q_UNUSED(toggle); updateTopSliceActions(); });
	connect(m_rightSliceCheckBox, &QCheckBox::toggled, [this](bool toggle) {Q_UNUSED(toggle); updateRightSliceActions(); });
	connect(m_frontSliceCheckBox, &QCheckBox::toggled, [this](bool toggle) {Q_UNUSED(toggle); updateFrontSliceActions(); });
}

void ImageView::updateActions()
{
	bool enable = m_sliceModel != nullptr;
	m_addCategoryAction->setEnabled(enable);
	m_categoryCBBox->setEnabled(enable);

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
	m_reset->setHidden(enable == false && !m_frontSliceCheckBox->isChecked() && !m_rightSliceCheckBox->isChecked());
}

void ImageView::updateFrontSliceActions()
{
	bool enable = m_frontSliceCheckBox->isChecked() && m_sliceModel != nullptr;
	m_frontSlicePlayAction->setEnabled(enable);
	m_frontSlider->setEnabled(enable);
	m_frontView->setHidden(!enable);
	m_reset->setHidden(enable == false && !m_topSliceCheckBox->isChecked() && !m_rightSliceCheckBox->isChecked());
}

void ImageView::updateRightSliceActions()
{
	bool enable = m_rightSliceCheckBox->isChecked() && m_sliceModel != nullptr;
	m_rightSlicePlayAction->setEnabled(enable);
	m_rightSlider->setEnabled(enable);
	m_rightView->setHidden(!enable);
	m_reset->setHidden(enable == false && !m_frontSliceCheckBox->isChecked() && !m_topSliceCheckBox->isChecked());
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
			histViewDlg = new HistogramViewer(SliceType::Top, QStringLiteral("Top Slice Histogram"), m_topView, m_sliceModel, this);
			histViewDlg->setWindowFlag(Qt::Window);
			histViewDlg->show();
			histViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//histViewDlg->sliceOpenEvent(m_topSlider->value());
			connect(this, &ImageView::topSliceOpened, histViewDlg, &AbstractPlugin::sliceOpened);
			connect(this, &ImageView::topSlicePlayStoped, histViewDlg, &AbstractPlugin::slicePlayStoped);
			connect(this, &ImageView::topSliceChanged, histViewDlg, &AbstractPlugin::sliceChanged);

			emit topSliceOpened(m_topSlider->value());
		}
		else if (m_menuWidget == m_rightView)
		{
			histViewDlg = new HistogramViewer(SliceType::Right, QStringLiteral("Right Slice Histogram"), m_rightView, m_sliceModel, this);
			histViewDlg->setWindowFlag(Qt::Window);
			histViewDlg->show();
			histViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//histViewDlg->sliceOpenEvent(m_rightSlider->value());
			connect(this, &ImageView::rightSliceOpened, histViewDlg, &AbstractPlugin::sliceOpened);
			connect(this, &ImageView::rightSlicePlayStoped, histViewDlg, &AbstractPlugin::slicePlayStoped);
			connect(this, &ImageView::rightSliceChanged, histViewDlg, &AbstractPlugin::sliceChanged);
			emit rightSliceOpened(m_rightSlider->value());
		}
		else if (m_menuWidget == m_frontView)
		{
			histViewDlg = new HistogramViewer(SliceType::Front, QStringLiteral("Front Slice Histogram"), m_frontView, m_sliceModel, this);
			histViewDlg->setWindowFlag(Qt::Window);
			histViewDlg->show();
			histViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//histViewDlg->sliceOpenEvent(m_frontSlider->value());
			connect(this, &ImageView::frontSliceOpened, histViewDlg, &AbstractPlugin::sliceOpened);
			connect(this, &ImageView::frontSlicePlayStoped, histViewDlg, &AbstractPlugin::slicePlayStoped);
			connect(this, &ImageView::frontSliceChanged, histViewDlg, &AbstractPlugin::sliceChanged);
			emit frontSliceOpened(m_frontSlider->value());
		}
		//connect(this, &ImageView::sliceChanged, histViewDlg, &AbstractPlugin::sliceChanged);
		connect(this, &ImageView::topSlicePlayStoped, histViewDlg, &AbstractPlugin::slicePlayStoped);
	});
	connect(m_pixelViewDlgAction, &QAction::triggered, [this]()
	{
		PixelViewer * pixelViewDlg;
		if (m_menuWidget == m_topView)
		{
			pixelViewDlg = new PixelViewer(SliceType::Top, QStringLiteral("Top Slice Pixel View"), m_topView, m_sliceModel, this);
			pixelViewDlg->setWindowFlag(Qt::Window);
			pixelViewDlg->show();
			pixelViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//			pixelViewDlg->setImage(m_sliceModel->topSlice(m_topSlider->value()));
			connect(m_topView, &SliceView::sliceSelected, pixelViewDlg, &AbstractPlugin::sliceSelected);
			connect(this, &ImageView::topSliceOpened, pixelViewDlg, &AbstractPlugin::sliceOpened);
			emit topSliceOpened(m_topSlider->value());

		}
		else if (m_menuWidget == m_rightView)
		{
			pixelViewDlg = new PixelViewer(SliceType::Right, QStringLiteral("Right Slice Pixel View"), m_rightView, m_sliceModel, this);
			pixelViewDlg->setWindowFlag(Qt::Window);
			pixelViewDlg->show();
			pixelViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//	pixelViewDlg->setImage(m_sliceModel->rightSlice(m_rightSlider->value()));
			connect(m_rightView, &SliceView::sliceSelected, pixelViewDlg, &AbstractPlugin::sliceSelected);
			connect(this, &ImageView::rightSliceOpened, pixelViewDlg, &AbstractPlugin::sliceOpened);
			emit rightSliceOpened(m_rightSlider->value());
		}
		else if (m_menuWidget == m_frontView)
		{
			pixelViewDlg = new PixelViewer(SliceType::Front, QStringLiteral("Front Slice Pixel View"), m_frontView, m_sliceModel, this);
			pixelViewDlg->setWindowFlag(Qt::Window);
			pixelViewDlg->show();
			pixelViewDlg->setAttribute(Qt::WA_DeleteOnClose);
			//	pixelViewDlg->setImage(m_sliceModel->frontSlice(m_frontSlider->value()));
			connect(m_frontView, &SliceView::sliceSelected, pixelViewDlg, &AbstractPlugin::sliceSelected);
			connect(this, &ImageView::frontSliceOpened, pixelViewDlg, &AbstractPlugin::sliceOpened);
			emit frontSliceOpened(m_frontSlider->value());
		}
	});
	connect(m_marksManagerDlgAction, &QAction::triggered, [this]()
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
	m_markModel(nullptr),
	m_sliceModel(model)
{
	//layout
	m_layout = new QGridLayout;
	//QGraphicsView
	m_topView = new SliceView;
	m_rightView = new SliceView;
	m_frontView = new SliceView;
	m_topView->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	m_rightView->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	m_frontView->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

	m_reset = new QPushButton(QStringLiteral("Reset"));
	m_reset->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
	//m_view->resize(500, 500);
	//m_rightView->resize(50, 500);
	//m_frontView->resize(500, 50);
	m_layout->setSizeConstraint(QLayout::SetFixedSize);
	//m_layout->setColumnStretch(0, 5);
	//m_layout->setColumnStretch(1,1);
	m_layout->addWidget(m_topView, 1, 0, 1, 1, Qt::AlignCenter);
	m_layout->addWidget(m_rightView, 1, 1, 1, 1, Qt::AlignCenter);
	m_layout->addWidget(m_frontView, 2, 0, 1, 1, Qt::AlignCenter);
	m_layout->addWidget(m_reset, 2, 1, 1, 1, Qt::AlignCenter);
	//m_view->setScene(m_scene);

	//sliders

	m_topSliceCheckBox = new QCheckBox;
	m_topSliceCheckBox->setChecked(topSliceVisible);
	m_topSlider = new TitledSliderWithSpinBox(this, tr("Z:"));
	m_rightSliceCheckBox = new QCheckBox;
	m_rightSliceCheckBox->setChecked(rightSliceVisible);
	m_rightSlider = new TitledSliderWithSpinBox(this, tr("Y:"));
	m_frontSliceCheckBox = new QCheckBox;
	m_frontSliceCheckBox->setChecked(frontSliceVisible);
	m_frontSlider = new TitledSliderWithSpinBox(this, tr("X:"));

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

void ImageView::changeSlice(int value, SliceType type)
{
	Q_ASSERT_X(m_sliceModel != nullptr, "ImageView::sliceChanged", "null model pointer");
	SliceView * view = nullptr;
	std::function<QImage(int)> sliceGetter;
	//TODO:: 
	switch (type)
	{
	case SliceType::Top:
		view = m_topView;
		sliceGetter = std::bind(&AbstractSliceDataModel::frontSlice, m_sliceModel, std::placeholders::_1);
		break;
	case SliceType::Right:
		view = m_rightView;
		sliceGetter = std::bind(&AbstractSliceDataModel::rightSlice, m_sliceModel, std::placeholders::_1);
		break;
	case SliceType::Front:
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
	case SliceType::Top:
		return topSliceIndex();
	case SliceType::Right:
		return rightSliceIndex();
	case SliceType::Front:
		return frontSliceIndex();
	default:
		return -1;
	}
}

inline bool ImageView::contains(const QWidget* widget, const QPoint& pos) { return (widget->rect()).contains(pos); }

MarkModel* ImageView::createMarkModel(AbstractSliceDataModel * d)
{
	const MarkModel::MarkSliceList top(d->topSliceCount());
	const MarkModel::MarkSliceList right(d->rightSliceCount());
	const MarkModel::MarkSliceList front(d->frontSliceCount());
	return new MarkModel(new TreeItem(QVector<QVariant>{QStringLiteral("Name")}, TreeItemType::Root), d, top, right, front, nullptr);
}

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

inline int ImageView::topSliceIndex() const { return m_topSlider->value(); }

inline int ImageView::rightSliceIndex() const { return m_rightSlider->value(); }

inline int ImageView::frontSliceIndex() const { return m_frontSlider->value(); }

inline void ImageView::topSliceEnable(bool enable)
{
	m_topSliceCheckBox->setChecked(enable);
	updateActions();
}

inline void ImageView::rightSliceEnable(bool enable)
{
	m_rightSliceCheckBox->setChecked(enable);
	updateActions();
}

inline void ImageView::frontSliceEnable(bool enable)
{
	m_frontSliceCheckBox->setChecked(enable);
	updateActions();
}

void ImageView::setColor(const QColor & color)
{
	m_topView->setColor(color);
	m_rightView->setColor(color);
	m_frontView->setColor(color);
}

void ImageView::setSliceModel(AbstractSliceDataModel * model)
{
	m_sliceModel = model;
	updateSliceCount(SliceType::Top);
	updateSliceCount(SliceType::Right);
	updateSliceCount(SliceType::Front);
	updateSlice(SliceType::Front);
	updateSlice(SliceType::Right);
	updateSlice(SliceType::Top);

	//TODO::update marks
	if (m_markModel != nullptr)
	{
		if (m_markModel->check_match_helper_(model) == false)		//If the mark model doesnt match the slice model
			m_markModel = nullptr;
	}
	else
	{
		m_markModel = createMarkModel(m_sliceModel);
	}//create a new mark model
	updateMarks(SliceType::Top);
	updateMarks(SliceType::Right);
	updateMarks(SliceType::Front);
	updateActions();
}

MarkModel* ImageView::replaceMarkModel(MarkModel* model,bool * success)noexcept
{
	//check the model
	if (m_sliceModel == nullptr)
	{
		QMessageBox::critical(this, QStringLiteral("Error"),
			QStringLiteral("Mark model can't be set without slice data."),
			QMessageBox::StandardButton::Ok,QMessageBox::StandardButton::Ok);
		if (success != nullptr)
			*success = false;
		return nullptr;
	}
	if(model == nullptr)
	{
		auto temp = m_markModel;
		m_markModel = nullptr;
		updateMarks(SliceType::Top);
		updateMarks(SliceType::Right);
		updateMarks(SliceType::Front);
		if (success != nullptr)
			*success = true;
		return temp;
	}
	if (model->check_match_helper_(m_sliceModel) == false)
	{
		if (success != nullptr)
			*success = false;
		return nullptr;
	}
	auto temp = m_markModel;
	m_markModel = model;
	if (success != nullptr)
		*success = true;
	return temp;
}
MarkModel * ImageView::markModel()
{
	return m_markModel;
}
//MarkModel* ImageView::createMarkModel()
//{
//	//Warning:This function will create a new mark model and return the old model
//	typedef MarkModel::MarkSliceList MarkSliceList;
//	
//	auto model = MarkModel(new TreeItem(QVector<QVariant>{QStringLiteral("Name:")}, TreeItemType::Root),
//		m_sliceModel, MarkSliceList(1), MarkSliceList(1), MarkSliceList(1),this);
//
//}


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


void ImageView::onTopSlicePlay(bool enable)
{
	if (enable)
	{
		m_topTimerId = startTimer(10);
		//qDebug() << "onTopSliceTimer" << m_topTimerId;
		m_topSlicePlayDirection = PlayDirection::Forward;
	}
	else
	{
		killTimer(m_topTimerId);
		m_topTimerId = 0;
	}
}

void ImageView::onRightSlicePlay(bool enable)
{
	if (enable)
	{
		m_rightTimerId = startTimer(10);
		//qDebug() << "onRightSliceTimer" << m_rightTimerId;
		m_rightSlicePlayDirection = PlayDirection::Forward;
	}
	else
	{
		killTimer(m_rightTimerId);
		m_rightTimerId = 0;
	}
}

void ImageView::onFrontSlicePlay(bool enable)
{
	if (enable)
	{
		m_frontTimerId = startTimer(10);
		//qDebug() << "onFrontSliceTimer" << m_frontTimerId;
		m_frontSlicePlayDirection = PlayDirection::Forward;
	}
	else
	{
		killTimer(m_frontTimerId);
		m_frontTimerId = 0;
	}
}

void ImageView::timerEvent(QTimerEvent* event)
{
	int timeId = event->timerId();
	if (timeId == m_topTimerId)
	{
		int maxSlice = m_topSlider->maximum();
		int cur = m_topSlider->value();
		if (m_topSlicePlayDirection == PlayDirection::Forward)
		{
			cur++;
		}
		else
		{
			cur--;
		}
		if (cur >= maxSlice)
		{
			m_topSlicePlayDirection = PlayDirection::Backward;
		}
		else if (cur <= 0)
		{
			m_topSlicePlayDirection = PlayDirection::Forward;
		}
		m_topSlider->setValue(cur);
	}
	else if (timeId == m_rightTimerId)
	{
		int maxSlice = m_rightSlider->maximum();
		int cur = m_rightSlider->value();
		if (m_rightSlicePlayDirection == PlayDirection::Forward)
		{
			cur++;
		}
		else
		{
			cur--;
		}
		if (cur >= maxSlice)
		{
			m_rightSlicePlayDirection = PlayDirection::Backward;
		}
		else if (cur <= 0)
		{
			m_rightSlicePlayDirection = PlayDirection::Forward;
		}
		m_rightSlider->setValue(cur);

	}
	else if (timeId == m_frontTimerId)
	{
		int maxSlice = m_frontSlider->maximum();
		int cur = m_frontSlider->value();
		if (m_frontSlicePlayDirection == PlayDirection::Forward)
		{
			cur++;
		}
		else
		{
			cur--;
		}
		if (cur >= maxSlice)
		{
			m_frontSlicePlayDirection = PlayDirection::Backward;
		}
		else if (cur <= 0)
		{
			m_frontSlicePlayDirection = PlayDirection::Forward;
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
void ImageView::updateSliceCount(SliceType type)
{
	switch (type)
	{
	case SliceType::Top:
		setTopSliceCount(m_sliceModel->topSliceCount() - 1);
		break;
	case SliceType::Right:
		setRightSliceCount(m_sliceModel->rightSliceCount() - 1);
		break;
	case SliceType::Front:
		setFrontSliceCount(m_sliceModel->frontSliceCount() - 1);
		break;
	default:
		break;
	}
}

void ImageView::updateSlice(SliceType type)
{
	//
	SliceView * view = nullptr;
	std::function<QImage(int)> sliceGetter;
	switch (type)
	{
	case SliceType::Top:
		view = m_topView;
		sliceGetter = std::bind(&AbstractSliceDataModel::topSlice, m_sliceModel, std::placeholders::_1);
		break;
	case SliceType::Right:
		view = m_rightView;
		sliceGetter = std::bind(&AbstractSliceDataModel::rightSlice, m_sliceModel, std::placeholders::_1);
		break;
	case SliceType::Front:
		view = m_frontView;
		sliceGetter = std::bind(&AbstractSliceDataModel::frontSlice, m_sliceModel, std::placeholders::_1);
		break;
	default:
		Q_ASSERT_X(false,
			"ImageView::updateSlice", "SliceType error.");
	}
	Q_ASSERT_X(static_cast<bool>(sliceGetter) == true,
		"ImageView::updateSlice", "null function");
	int index = currentIndex(type);
	view->setImage(sliceGetter(index));
	view->clearSliceMarks();
}

void ImageView::updateMarks(SliceType type)
{
	Q_ASSERT_X(m_markModel,
		"ImageView::updateMarks", "null pointer");
	switch (type)
	{
	case SliceType::Top:
	{
		QList<QGraphicsItem*> res;
		auto m = m_markModel->topSliceVisibleMarks()[topSliceIndex()];
		for (int i = 0; i < m.size(); i++)
			res.append(QueryMarkItemInterface(m[i]));
		m_topView->setMarks(res);
	}
	break;
	case SliceType::Right:
	{
		QList<QGraphicsItem*> res;
		auto m = m_markModel->rightSliceVisibleMarks()[rightSliceIndex()];
		for (int i = 0; i < m.size(); i++)
			res.append(QueryMarkItemInterface(m[i]));
		m_rightView->setMarks(res);
	}
	break;
	case SliceType::Front:
	{
		QList<QGraphicsItem*> res;
		auto m = m_markModel->frontSliceVisibleMarks()[frontSliceIndex()];
		for (int i = 0; i < m.size(); i++)
			res.append(QueryMarkItemInterface(m[i]));
		m_frontView->setMarks(res);
	}
	break;
	}
}

SliceView::SliceView(QWidget *parent) :QGraphicsView(parent),
m_scaleFactor(0.5),
m_currentPaintItem(nullptr),
m_paint(false),
m_moveble(true),
m_color(Qt::black),
m_slice(nullptr)
{
	setScene(new SliceScene(this));
	scale(m_scaleFactor, m_scaleFactor);
	//setContextMenuPolicy(Qt::CustomContextMenu);
	//createContextMenu();
	//createDialog();
	//connect(this, &GraphicsView::customContextMenuRequested, [this](const QPoint&pos) {m_contextMenu->exec(this->mapToGlobal(pos)); });
}

void SliceView::setMarks(const QList<QGraphicsItem*>& items)
{
	setTopSliceMarks(items);
}

void SliceView::setTopSliceMarks(const QList<QGraphicsItem*>& items)
{
	foreach(QGraphicsItem * item, items)
	{
		item->setParentItem(m_slice);
		item->setVisible(true);
	}
}
void SliceView::wheelEvent(QWheelEvent *event) {
	double numDegrees = -event->delta() / 8.0;
	double numSteps = numDegrees / 15.0;
	double factor = std::pow(1.125, numSteps);
	scale(factor, factor);

}

void SliceView::focusInEvent(QFocusEvent* event)
{
	Q_UNUSED(event);
	this->setStyleSheet("border:2px solid red");
}

void SliceView::focusOutEvent(QFocusEvent* event)
{
	Q_UNUSED(event);
	this->setStyleSheet("");
}


void SliceView::mousePressEvent(QMouseEvent *event)
{
	QPoint viewPos = event->pos();
	QPointF pos = mapToScene(viewPos);
	m_prevScenePoint = pos;
	auto items = scene()->items(pos);
	for (const auto & item : items) {
		SliceItem * slice = qgraphicsitem_cast<SliceItem*>(item);
		if (slice == m_slice)
		{
			QPoint itemPoint = slice->mapFromScene(pos).toPoint();
			emit sliceSelected(itemPoint);

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

void SliceView::mouseMoveEvent(QMouseEvent *event)
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

void SliceView::mouseReleaseEvent(QMouseEvent *event)
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
				PolyMarkItem * polyItem = new PolyMarkItem(polyF, m_currentPaintItem);
				QBrush aBrush(m_color);
				QPen aPen(aBrush, 5, Qt::SolidLine);
				polyItem->setPen(aPen);
				polyItem->setZValue(100);
				//emit
				if (m_currentPaintItem == m_slice)
				{
					emit markAdded(polyItem);
				}
				return;
			}
		}
	}
	QGraphicsView::mouseReleaseEvent(event);
}

SliceScene::SliceScene(QObject *parent) :QGraphicsScene(parent)
{

}

void SliceScene::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	qDebug() << "mousePressEvent in scene";
	QGraphicsScene::mousePressEvent(event);
}

void SliceScene::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
	qDebug() << "mouseMoveEvent in scene";
	QGraphicsScene::mouseMoveEvent(event);
}

void SliceScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
	qDebug() << "mouseReleaseEvent in scene";
	QGraphicsScene::mouseReleaseEvent(event);
}

void SliceScene::wheelEvent(QGraphicsSceneWheelEvent * event)
{
	qDebug() << "wheelEvent in scene";
	QGraphicsScene::wheelEvent(event);
}

StrokeMarkItem::StrokeMarkItem(QGraphicsItem * parent, int index, const QString & name, const QColor & color, SliceType type, bool visible) : QGraphicsItem(parent), AbstractMarkItem(name, 0.0, color, type, index, visible)
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
	Q_UNUSED(painter);
	Q_UNUSED(option);
	Q_UNUSED(widget);
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

void SliceView::set_image_helper(const QImage & image)
{
	QSize size = image.size();
	QPoint pos = QPoint(-size.width() / 2, -size.height() / 2);
	setImageHelper(pos, image, m_slice, &m_image);

}



void SliceView::setImageHelper(const QPoint& pos, const QImage& inImage, SliceItem*& sliceItem, QImage * outImage)
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

void SliceView::clear_slice_marks_helper_(SliceItem* sliceItem)
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

void SliceView::set_mark_helper_(SliceItem* sliceItem, const QList<QGraphicsItem*>& items)
{
	foreach(QGraphicsItem * item, items)
	{
		item->setParentItem(sliceItem);
		item->setVisible(true);
	}
}




void SliceView::clearTopSliceMarks()
{
	if (m_slice == nullptr)
	{
		qWarning("Top slice is empty.");
		return;
	}
	auto children = m_slice->childItems();
	foreach(QGraphicsItem * item, children)
	{
		item->setParentItem(nullptr);
		item->setVisible(false);
	}
}


void SliceView::setImage(const QImage& image)
{
	set_image_helper(image);
}

void SliceView::clearSliceMarks()
{
	clear_slice_marks_helper_(m_slice);
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

